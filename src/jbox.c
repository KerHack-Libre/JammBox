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
#include "jboxconfig.h"

#include "diskcheck.h" 
#include "dboxutils.h"
#include "archive.h" 
#include "ui.h" 

#ifdef USE_ZIP_ARCHIVE 
extern zip_t *za;  
#endif 

#define jbox_goto(endprog_status , errcode , ...)\
      endprog_status;do{disk_err(errcode);err(errcode,__VA_ARGS__); goto _eplg;}while(0)  

extern char **environ ; 

#define  DOSBOX_CHSBYTES_ORDER(chsbytes) \
  chsbytes->_sector,chsbytes->_head,chsbytes->_cylinder 

#define  IS_SANDBOX(sandbox) !(0xffff  &~ (0xfffff ^(sandbox))) 
FILE *memrecord=(FILE *)00 ; 
extern FILE * memrecord_ptr ; 
extern char * dbox_emulator ; 

static char * jbox_path_resolve(char  const *_Nonnull dosimg);  
static unsigned int  scan_pte(mbr_t *  _Nonnull) ;    
static int jbox_launch_dosbox_emulator(const char    * __restrict__  _Nonnull, 
                                       char const    * __restrict__  _Nonnull, 
                                       global_chs_t  * __restrict__  _Nonnull) ; 
static int jbox_rm_disk_image(const char * __restrict__   _Nonnull) ; 
static int sanbox_write_log_to(const char  * restrict _Nullable , int ios_redirect) ; 

int jbox_create_sandbox(char **_Nonnull stream_memory_dump); 


int main(int ac , char *const *av) 
{
  unsigned int pstatus= EXIT_SUCCESS, 
               selected_game = 0, 
               apartno =0 , 
               archive_status_mode = 0;  
  global_chs_t * chsbytes = (global_chs_t *)00 ;  
  struct __unzip_t * data =  (struct __unzip_t*)00;  
  char *dosimg= (char*)00, 
       **available_games = (char **) 00; 

  if(!(ac &~(1))) 
  { 
    
#if defined(JBOX_TUI_MENU) && JBOX_TUI_MENU ==1
    available_games =  dbox_games(GAMES_PATH_LOCATION); 
    if(!*available_games) 
    {
      pstatus^= jbox_goto(1, -ENODATA ,  "No Games found \012"); 
    }

    ui_init() ; 
    selected_game =  ui_display_menulist((const char ** )available_games ,0) ;
    if(~0 == selected_game) 
       disk_err(-EINVAL) ; 

    free(dosimg), dosimg=(char *)00; 
    dosimg = strdup(*(available_games+selected_game)) ;  
    
    dbox_deallocate_games_list(available_games); 
    available_games= 00; 
#else  
    pstatus^= jbox_goto(1, -EINVAL,"Require DOS/MBR image or zip archive file\012")  ; 
#endif 
  }else  
    dosimg =  strdup(*(av+(ac -1)))  ; 
   

#if defined(USE_ZIP_ARCHIVE) 
  archive_status_mode =  archive_open(dosimg) ; 
  if(~0 != archive_status_mode)  
  { 
    archive_status_mode&= 0xffff ; 
    struct __unzip_t * data =(struct __unzip_t *) archive_scan(za ,  archive_status_mode);
    if(!data) 
    {
       free(dosimg)  , dosimg =00 ; 
       pstatus^=jbox_goto(1 , -ENODATA, "Fail to uncompress archive entry") ; 
    }  
    dosimg = strdup( ((unzip_t *)data)->_filename);  
    free(data) , data =0;  
  }
  
#endif 

  if(diskload(dosimg)) 
  {
     pstatus^=err_expr(dc_warn("Fail to load image disk"));  
     disk_mesg_err() ; 
     goto _eplg; 
  }

   apartno = scan_pte(&mbr) ;  
   if(~0 == apartno) 
   {
     pstatus^=err_expr(dc_err("No Active partition found")); 
     goto _eplg ; 
   }
   __pte *active_boot_partition = (mbr.ptabs+apartno); 
  
   chsbytes = decode_chsbytes(active_boot_partition) ; 
   if(!chsbytes) 
   { 
     pstatus^=jbox_goto(1,  -ENODATA ,"Disk check fail to decode CHS start and end"); 
   } 

   //!TODO: Need to check startup script first  
   jbox_launch_dosbox_emulator(INIT_ENTRY_LAUNCH_SCRIPT, dosimg ,  chsbytes) ;
   
   free(chsbytes), chsbytes=0; 
#if !ALWAYS_REMOVE_IMG_AFTER_PLAY  
   //!just freeing is enought ...  
   free(dosimg),  dosimg=0 ; 
#endif 
   wait(0) ; 
#if  ALWAYS_REMOVE_IMG_AFTER_PLAY  
   jbox_rm_disk_image(dosimg) ; 
   free(dosimg)  , dosimg=0 ;  
#endif 
   
   tx(clear_screen); 
_eplg: 
  return pstatus ; 
}

static char * jbox_path_resolve(char  const * dosimg)  
{
  char *path =(char *)00 ,
       *index_start = (char *)dosimg,
       *cwd  =  get_current_dir_name(); 

  int c_start =0 ; memcpy(&c_start , dosimg , 2) , 
      c_start&=0xffff ; 

  if(!(c_start  ^ 0x2f2e))  
    index_start+=2; 

  asprintf(&path ,  "%s/%s", cwd , index_start) ; 
  free(cwd) ; 

  return path ;  
}


static unsigned int  scan_pte(mbr_t  * mbr)  
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

static int jbox_launch_dosbox_emulator(const char * restrict start_script, 
                                       char const * restrict dosimg, 
                                       global_chs_t * restrict active_partition) 
{
  if(!has_dosbox()) 
    return ~0 ; 
  
  size_t size =0 ; 
  char *abs_path= jbox_path_resolve(dosimg), 
       *dump = (char *)00 ; 

  struct dosbox_entry_t payload= { 
     IMGMOUNT,
     abs_path,
     0x200,
#if defined(MBRCHS)
     .end = &active_partition->_end,  
#endif 
  }; 

  memrecord  = open_memstream(&dump ,  &size) ; 
  if (!memrecord)  
  {
     disk_err(-ESTRPIPE); 
     err(~0 , "Fail to initialize memory record");
  }
  memrecord_ptr  =  memrecord ; 

  dbox_automount(DBOX_DRIVE(c), &payload); 
  dbox_autorun(start_script  , jbox_create_sandbox , (void ** )&dump) ;
  free(abs_path) , abs_path=0; 
  return 0 ; 

}

int jbox_create_sandbox(char ** memdump)  
{
  int status = EXIT_SUCCESS ; 
  pid_t sandbox = ~0 ; 
  
  sandbox^=fork() ; 
  if(!sandbox) {
   status^=EXIT_FAILURE ;
   perror("fork") ; 
   goto  _eplg ; 
  }
  if(IS_SANDBOX(~sandbox)){ 

    errno=0 ; 
    int logfd = sanbox_write_log_to((void *)00/* /dev/null*/
                                    ,STDERR_FILENO|STDOUT_FILENO); 

    char *payload[0x3e8] = {
      EMULNAME(dosbox) ,(char[]){0x2d,0x63,00},
    } ; 
    dbox_extract(memdump , payload);
    if (!(~0 ^ execv(dbox_emulator , payload))) 
       perror("execv") ; 
    
    close(logfd) ; 
    exit(errno);   
  } 

_eplg: 
  return  status ; 
}

static int sanbox_write_log_to(const char  * restrict  journal, int ios_direction) 
{
  unsigned int jfd =~0; 
  char logfile[0xff] = "/dev/null"; 
  mode_t usermod = 0,
         io = O_RDONLY ;
  if (journal) 
  {
    memset(logfile ,  0 , 0xff); 
    sprintf(logfile, "%s" , journal) ; 
    io|=O_CREAT|O_WRONLY;
    usermod=S_IRUSR|S_IWUSR;  
  }

  jfd ^=open(logfile, io  , usermod) ; 
  if(!jfd)
  {
    perror("open") ; 
    return 0; 
  }
 
  /* 
   * Redirect the early print of dosbox depending 
   * on what u specified as logfile a.k.a journal
   */
  if(ios_direction &  STDOUT_FILENO) 
    dup2(~jfd, STDOUT_FILENO) ; 

  if(ios_direction &  STDERR_FILENO) 
    dup2(~jfd , STDERR_FILENO) ; 

  return ~jfd ; 
} 

static int jbox_rm_disk_image(const char * restrict  dosbox_disk_image) 
{
   if(~0 < euidaccess(dosbox_disk_image , F_OK))  
     return remove(dosbox_disk_image) ; 

   return ~0 ;  
}
