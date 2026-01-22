//SPDX-License-Identifier:GPL-3.0 

#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <dirent.h> 
#include <sys/types.h> 

#include "mbr.h" 
#include "dboxutils.h"



char * dbox_emulator =(char*)00 ;  
FILE * memrecord_ptr =(FILE *)00; 

int dbox_available(void) 
{
   char  *binloc = getenv("PATH") , 
         *token  = 0 , 
         fullpath[0xff] ={0};  
   unsigned  int status  = 0; 

   if(!binloc) 
     return ~0 ;
   
   while((token = strtok(binloc, ":")) ) 
   {
     if(binloc) 
       binloc = 0; 
     
     sprintf(fullpath ,  "%s/"EMULNAME(dosbox), token) ; 
     if(!access(fullpath , X_OK|F_OK))
     {
       dbox_emulator =  strdup(fullpath) ;  
       status^=1 ; 
       break ; 
     }
     bzero(fullpath  , 0xff) ; 
   }

   return status  ;  
}


int dbox_automount(const   char  * part_drive  , 
                     struct dosbox_entry_t * entry) 
{
  
  switch(entry->doscmd_directive) 
  {
    case IMGMOUNT:
      fprintf(memrecord_ptr , 
          CMDFMT(IMGMOUNT) , 
          CMDIR(IMGMOUNT),
          *part_drive  , entry->game_path ,
          entry->byte_sector,
          entry->end->_sector ,
          entry->end->_head, 
          entry->end->_cylinder,
          *part_drive) ; 
      break ; 
      
    default: 
      fprintf(stderr , "Unknow Command Directive \012") ; 
      return ~0; 
  }

  return 0 ; 
} 

int dbox_extract(char **memory  ,char *payload[static 1000]) 
{  
  char * cmd_tkn = 0; 
  int idx=1; 
  while((cmd_tkn = strtok(*memory , "?"))) 
  {
    if(*memory) *memory=(void*)0;
    idx-=~0,*(payload+idx)=(char *) cmd_tkn , idx-=~0 ; 
    *(payload+idx)= (char*)"-c" ; 
  }
  *(payload+idx) =(void *)0;  
  return 0; 
}

int  dbox_autorun(const char *  restrict  start_script,
                    sandbox_ctx  sandbox ,void ** cmdmem_dump )  
{

  if(start_script) 
    fprintf(memrecord_ptr, "? %s", start_script) ; 

  fclose(memrecord_ptr) ;  

  return sandbox((char ** ) cmdmem_dump ) ; 
}  


char **dbox_games(const char * restrict game_path) 
{ 

  struct dirent **list = (void *)0 ; 
  int direntries  = 0   , idx = 0 ;  
  ssize_t  len =  ~0 ; 
  char *s =(void *) 0  , 
       *root_path = strdup(game_path) ;   

  //> TODO : Set limit of maximum games 
  char **founded_games = malloc(sizeof(char)  * 100 * 100 ) ;  
  if(!founded_games) 
    return (void *) 0 ; 
 
  //> formating the root game path 
  //+ looking for the last '/'
 
  s = strrchr(root_path , 0x2f) ; 
  len+= strlen(root_path) ;  
  
  if(!(len ^ (s - root_path)))  
    free(root_path) , 
      root_path = strndup(game_path , len)  ; 
  
  direntries= scandir(game_path , &list , filter , alphasort) ; 
  while(direntries--) 
  {  
    //*(founded_games+idx) =   strdup(list[direntries]->d_name ) ; 
    asprintf((founded_games+idx) , "%s/%s",root_path  , list[direntries]->d_name) ; 
    printf("-> %s \n" , *(founded_games +idx )) ;  // list[direntries]->d_name ); 
    idx=-~idx ; 
  }  
  
  *(founded_games - (~idx)) =  (void *)0 ; 

  free(root_path)  , root_path = 0 ; 
  return founded_games ; 
  
}
static int filter(const struct  dirent * dirent) 
{
  // char  *extension  = _endwith(dirent->d_name , ".zip" , ".img") ; 
  //> ! looking for zip file 
  char * extension  = strstr(dirent->d_name , ".zip") ;  

  return  ((!!(dirent->d_type ^ DT_DIR)) && extension)  ; 
}
