//SPDX-License-Identifier:GPL-3.0 

#define _GNU_SOURCE  
#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h> 
#include <fcntl.h>
#include <string.h> 
#include <assert.h> 
#include <sys/wait.h> 
#include <sys/types.h> 

#include "diskcheck.h" 
#include "dboxutils.h"
#include "archive.h" 
#include "ui.h" 

#ifdef USE_ZIP_ARCHIVE 
extern zip_t *za;  
#endif 

#define jbox_goto(endprog_status , errcode , ...)\
      endprog_status;do{disk_err(errcode);err(errcode,__VA_ARGS__); goto _eplg;}while(0)  

#define GAMES_PATH_LOCATION  "games/princeofpersia"
extern char **environ ; 

#define  DOSBOX_CHSBYTES_ORDER(chsbytes) \
  chsbytes->_sector,chsbytes->_head,chsbytes->_cylinder 

FILE *memrecord=(FILE *)00 ; 
extern FILE * memrecord_ptr ; 
extern char * dbox_emulator ; 

static char * jbox_path_resolve(char  const *_Nonnull dosimg);  
static int scan_pte(mbr_t *) ;   
static int jbox_launch_dosbox_emulator(const char    * __restrict__, 
                                       char const    * __restrict__, 
                                       global_chs_t  * __restrict__ ) ; 

int jbox_create_sandbox(char **_Nonnull stream_memory_dump); 

static int sanbox_write_log_to(char  *_Nullable journal , int ios_direction) ; 

int main(int ac , char *const *av) 
{
  unsigned int pstatus= EXIT_SUCCESS ; 
  const char *dosimg= (char*)00 ; 
  char **available_games = (char **) 00; 
  unsigned int selected_game =  0 ; 

  if(!(ac &~(1))) 
  { 
    
#if defined(JBOX_TUI_MENU) && JBOX_TUI_MENU ==1
    available_games =  dbox_games(GAMES_PATH_LOCATION); 
    
    ui_init() ; 
    selected_game =  ui_display_menulist((const char ** )available_games ,0) ;
    if(~0 == selected_game) 
       disk_err(-EINVAL) ; 

    dosimg = strdup(*(available_games+selected_game)) ;  

#else  
    pstatus^= jbox_goto(1, -EINVAL,"Require DOS/MBR image or zip archive file\012")  ; 
#endif 
  }else  
    dosimg =  strdup(*(av+(ac -1)))  ; 
   
  printf("-> Game  : %s  \012",  dosimg) ; 

#if defined(USE_ZIP_ARCHIVE) 
  unsigned int status_mode =  archive_open(dosimg) ; 
  if(~0 != status_mode)  
  { 
    status_mode&= 0xffff ; 
    struct __unzip_t * data =(struct __unzip_t *) archive_scan(za ,  status_mode);
    if(!data) 
    {
       free(dosimg)  , dosimg =00 ; 
       pstatus^=jbox_goto(1 , -ENODATA, "Fail to uncompress archive entry") ; 
    }  
    puts("\011 ... OK") ; 
    printf("disks found at  %s \012" , data->_filename) ;  
    //free(dosimg);  dosimg =00 ;  
  
    dosimg = strdup( ((unzip_t *)data)->_filename);  
  }
  
#endif 

  if(diskload(dosimg)) 
  {
     pstatus^=err_expr(dc_warn("Fail to load image disk"));  
     disk_mesg_err() ; 
     goto _eplg; 
  }

   int apartno = scan_pte(&mbr) ;  
   if(~0 == apartno) 
   {
     pstatus^=err_expr(dc_err("No Active partition found")); 
     goto _eplg ; 
   }
   __pte *active_boot_partition = (mbr.ptabs+apartno); 
  
   global_chs_t * chsbytes = decode_chsbytes(active_boot_partition) ; 
   if(!chsbytes) 
   {
     err((pstatus^=1) , "Disk Check fail to decode CHS start and end"); 
     goto _eplg ;  
   } 

   //!TODO: Trouver un moyen de  verifier si un script de demarrage  est dispo 
   jbox_launch_dosbox_emulator("START.BAT", dosimg ,  chsbytes) ;
  
_eplg: 
  return pstatus ; 
}


static char * jbox_path_resolve(char  const * dosimg)  
{
  char *path =(char *)0 ;
  char *index_start = (char *)dosimg;  

  char * cwd  =  get_current_dir_name(); 
  int c_start =0 ; memcpy(&c_start , dosimg , 2) , 
      c_start&=0xffff ; 

  if(!(c_start  ^ 0x2f2e))  
    index_start+=2; 

  asprintf(&path ,  "%s/%s", cwd , index_start) ; 
  free(cwd) ; 

  return path ;  
}


static int scan_pte(mbr_t  * mbr)  
{
  unsigned int  idx =~0 ,
                missed=~0 ; 
  while(++idx < MBR_PE) 
  {
    if(active_partition( (mbr->ptabs+idx) ))  
      break ; 
    
    missed-=~0; 
    
  }
  if(!(missed &~ idx))  
  {
     disk_err(122) ; 
     return ~0  ; 
  }

  return  idx; 
}

static int jbox_launch_dosbox_emulator(const char * __restrict__ start_script, 
                                       char const * restrict dosimg, 
                                       global_chs_t * restrict active_partition) 
{
  if(!has_dosbox()) 
    return ~0 ; 
  
  char *abs_path= jbox_path_resolve(dosimg) ;  
  struct dosbox_entry_t payload= { 
     IMGMOUNT,
     abs_path,
     0x200,
#if defined(MBRCHS)
     .end = &active_partition->_end,  
#endif 
  }; 

  size_t size = 0 ; 
  char *dump= 0 ; 
  memrecord  = open_memstream(&dump ,  &size) ; 
  if (!memrecord)  
  {
     disk_err(-ESTRPIPE); 
     err(~0 , "Fail to initialize memory record");
  }
  memrecord_ptr  =  memrecord ; 

  dbox_automount(DBOX_DRIVE(c), &payload); 
  dbox_autorun(start_script  , jbox_create_sandbox , (void ** )&dump) ; 
  
  return 0 ; 

}

int jbox_create_sandbox(char ** memdump)  
{
  int status = EXIT_SUCCESS ; 
  pid_t sandbox = ~0 ; 
  
  sandbox^=fork() ; 
  if(!sandbox) 
  {
   status^=EXIT_FAILURE ;
   perror("fork") ; 
   goto  _eplg ; 
  }
  
  if(!(0xffff  &~ (0xfffff ^ (~sandbox))) )  
  { 
    int logfd = sanbox_write_log_to((void *)00/* /dev/null*/
                                    ,STDERR_FILENO|STDOUT_FILENO); 

    char *payload[1000] = {
      EMULNAME(dosbox) ,(char[]){0x2d,0x63,00},
    } ; 
    dbox_extract(memdump , payload);
    int s = execv(dbox_emulator , payload) ; 
    if(!(~0 ^ s)) 
       perror("execv") ; 

    close(logfd) ; 
    exit(s);   
  }else 
  {  
    wait(&status); 
  }

_eplg: 
  return  status ; 
}

static int sanbox_write_log_to(char  *_Nullable journal, int ios_direction) 
{
  unsigned int jfd =~0; 
  char logfile[0xff] = "/dev/null"; 
  mode_t usermod = 0 ;
  mode_t io = O_RDWR ;
  if (journal) 
  {
    memset(logfile ,  0 , 0xff); 
    sprintf(logfile, "%s" , journal) ; 
    io|=O_CREAT ;
    usermod=S_IRUSR|S_IWUSR;  
  }

  jfd ^=open(logfile, io  , usermod) ; 
  if(!jfd)
  {
    perror("open") ; 
    return 0; 
  }
  
  if(ios_direction &  STDOUT_FILENO) 
    dup2(~jfd, STDOUT_FILENO) ; 

  if(ios_direction &  STDERR_FILENO) 
    dup2(~jfd , STDERR_FILENO) ; 

  return ~jfd ; 
} 

