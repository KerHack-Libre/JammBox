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

extern char **environ ; 

#define  DOSBOX_CHSBYTES_ORDER(chsbytes) \
  chsbytes->_sector,chsbytes->_head,chsbytes->_cylinder 

enum DOSBOX_DIRECTIVES { 
  IMGMOUNT,
}; 

#define  FMT_IMGMOUNT\
  "? %s %c %s -size %i,%i,%i,%i ? %c:" 

#define CMDFMT(__directive)\
  FMT_##__directive

#define CMDIR(__directive) \
  #__directive


struct dosbox_entry_t  
{ 
  enum DOSBOX_DIRECTIVES doscmd_directive ; 
  const char *game_path ;
  ssize_t byte_sector; 
  struct __chs_t *end ;
};  

FILE *memrecord=(FILE *)00 ; 
char *dosbox   =0; 

static int has_dosbox(void) 
{
   char  *binloc = getenv("PATH") , 
         *token  = 0 , 
         fullpath[0xff] ={0};  
   int status  = 0 ; 
   if(!binloc) 
     return ~0 ;
   
   while((token = strtok(binloc, ":")) ) 
   {
     if(binloc) 
       binloc = 0; 
     
     sprintf(fullpath ,  "%s/dosbox", token) ; 
     if(!access(fullpath , X_OK|F_OK))
     {
       dosbox =  strdup(fullpath) ;  
       status^=1 ; 
       break ; 
     }
     bzero(fullpath  , 0xff) ; 
   }

   
   return status  ; 
  
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

int dosbox_automount(char const  * diskpart , struct dosbox_entry_t * entry) 
{
  
  switch(entry->doscmd_directive) 
  {
    case IMGMOUNT:
      fprintf(memrecord , 
          CMDFMT(IMGMOUNT) , 
          CMDIR(IMGMOUNT),
          *diskpart  , entry->game_path ,
          entry->byte_sector,
          entry->end->_sector ,
          entry->end->_head, 
          entry->end->_cylinder,
          *diskpart) ; 
      break ; 
      
    default: 
      fprintf(stderr , "Unknow Command \012") ; 
      return ~0; 
  }

  return 0 ; 
} 

static int dosbox_extract(char **memory  ,char *payload[static 0xff]) 
{  
  char * cmd_tkn = 0; 
  int idx=1 ; 
  while((cmd_tkn = strtok(*memory , "?"))) 
  {
    if(*memory) *memory=(void*)0;
    idx-=~0,*(payload+idx)=(char *) cmd_tkn , idx-=~0 ; 
    *(payload+idx) = (char*)"-c" ; 
  }
  *(payload+idx) =(void *)0; 
  return 0; 
}
int  dosbox_autorun(char **memory_dump) 
{
  fprintf(memrecord , "? START.BAT") ; 
  fclose(memrecord) ; 
  
  int sandbox= fork() ; 
  if(~0  == sandbox)  
     return ~0 ; 

  if(!sandbox) 
  { 
    char *payload[0xff]={"dosbox" , "-c"} ; 
    dosbox_extract(memory_dump , payload) ;  
    int status =execv(dosbox , payload) ;   
    if(!(~0 ^ status)) 
      return ~0 ;  
    return 0; 
  }else{
     int s = 0 ; 
     wait(&s) ;  
  }

}

static int scan_pte(mbr_t *) ;   
static int jbox_launch_bosbox_emulator(char const * __restrict__, global_chs_t  * __restrict__ ) ; 

int main(int ac , char *const *av , char * const *ev) 
{
  unsigned int pstatus= EXIT_SUCCESS ; 
  const char *dosimg= (char*)00, 
             *data  =  0 ; 
  
  if(!(ac &~(1))) 
  {
    disk_err(-EINVAL) ; 
    err( (pstatus^=1),"Require DOS/MBR image")  ; 
    goto _eplg ; 
  }
  
  dosimg =  *(av+1) ; 
  data = diskload(dosimg); 
  if(!data) 
  {
    err((pstatus^=1),"Fail to load image disk") ;
    goto _eplg; 
  }
  
  mbr =*(mbr_t*) data ; 

  if(!has_boot_signature(&mbr)) 
  {
     err((pstatus^=1) , "Invalid Disk") ; 
     goto _eplg; 
  } 

   int apartno = scan_pte(&mbr) ;  
   if(~0 == apartno) 
   {
     err((pstatus^=1) , "No Active partition found") ; 
     goto _eplg ; 
   }
   __pte *active_boot_partition = (mbr.ptabs+apartno); 
  
   global_chs_t * chsbytes = decode_chsbytes(active_boot_partition) ; 
   if(!chsbytes) 
   {
     err((pstatus^=1) , "Disk Check fail to decode CHS start and end"); 
     goto _eplg ;  
   } 
  
   jbox_launch_bosbox_emulator(dosimg ,  chsbytes) ;
  
_eplg: 
  return pstatus ; 
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

static int jbox_launch_bosbox_emulator(char const * restrict dosimg, global_chs_t   * restrict active_partition) 
{
  if(!has_dosbox()) 
    return ~0 ; 
  
  char *abs_path= jbox_path_resolve(dosimg) ;  
  struct dosbox_entry_t data = { 
     IMGMOUNT,
     abs_path,
     0x200, 
     .end = &active_partition->_end,  
  }; 

  size_t size = 0 ; 
  char *dump= 0 ; 
  memrecord  = open_memstream(&dump ,  &size) ; 
  if (!memrecord)  
  {
     disk_err(-ESTRPIPE); 
     err(~0 , "Fail to initialize memory record");
  }

  dosbox_automount("c" , &data); 
  dosbox_autorun(&dump) ; 

}
