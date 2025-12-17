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

#ifdef USE_ZIP_ARCHIVE 
extern zip_t *za;  
#endif 

extern char **environ ; 

#define  DOSBOX_CHSBYTES_ORDER(chsbytes) \
  chsbytes->_sector,chsbytes->_head,chsbytes->_cylinder 

FILE *memrecord=(FILE *)00 ; 
extern FILE * memrecord_ptr ; 
extern char * dbox_emulator ; 

static char * jbox_path_resolve(char  const * dosimg);  
static int scan_pte(mbr_t *) ;   
static int jbox_launch_dosbox_emulator(const char    * __restrict__, 
                                       char const    * __restrict__, 
                                       global_chs_t  * __restrict__ ) ; 
int jbox_create_sandbox(char ** memdump);   

int main(int ac , char *const *av) 
{
  unsigned int pstatus= EXIT_SUCCESS ; 
  const char *dosimg= (char*)00, 
             *data  =  0 ; 

  if(!(ac &~(1))) 
  {
    disk_err(-EINVAL) ; 
    err( (pstatus^=1),"Require DOS/MBR image or  zip archive file (should have the image file inside)")  ; 
    goto _eplg ; 
  }
  
  dosimg =  *(av+1) ;  
#if defined(USE_ZIP_ARCHIVE)
  if(!archive_open(dosimg))  
  {
    archive_scan(za, dosimg) ;   
  }
  printf("-> %s \012",  dosimg) ; 
  return 0 ; 
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
  sandbox=~sandbox; 

  //! TODO : redirect  io to /dev/null 
  if(!(0xffff  &~ (0xfffff ^ sandbox))) 
  { 
    //!TODO : turn off dosbox log 
    //!TODO : redirect dosbox log  from  /dev/null or log file 
    //!int lgfd =  sanbox_write_log((void *)0) ;  //(void *)0 -> /dev/null  
    char *black_hole =  "/dev/null";  
    int  dn  = open(black_hole, O_RDONLY) ; 
    //!Je suppose que ca marchera toujours ! 
    dup2(dn , STDERR_FILENO) ; 
    dup2(dn , STDOUT_FILENO) ;  
    
    char *payload[0xff] = {
      EMULNAME(dosbox) ,(char[]){0x2d,0x63,00},
    } ; 
    dbox_extract(memdump , payload);
    int s = execv(dbox_emulator , payload) ; 
    if(!(~0 ^ s)) 
       perror("execv") ; 

    //close(lgfd) ; 
    exit(s);   
  }else 
  {  
    wait(&status);  
  }


_eplg: 
  return  status ; 
}
