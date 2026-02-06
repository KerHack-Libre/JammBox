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
static int reset_idx_ = 0 ; 
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
  
//!TODO : static  void  dbox_deallocate_games(const char ** restrict list_of_games)  
//+ Should deallocated the list of game from the heap  
char **dbox_games(const char * restrict game_path) 
{ 

  struct dirent **list = (void *)0 ; 
  int direntries  = 0   , idx = 0 ;  
  ssize_t  len =  ~0 ; 
  char *s =(void *) 0  , 
       *root_path = strdup(game_path) ;   
  

  char **founded_games = malloc(sizeof(char)  * 100 * 100 ) ;  
  if(!founded_games) 
    return (void *) 0 ; 
 
  //> __normalize_end_path(root_path)
  s = strrchr(root_path , 0x2f) ; 
  len+= strlen(root_path) ;  
   
  if(!(len ^ (s - root_path))) 
  {
    free(root_path); 
    root_path = strndup(game_path , len)  ; 
  }
  
  new_filter_t catch_only_dir = lambda(int,(const struct dirent * dir), {
      return  ((*dir->d_name & 0xff) ^ 0x2e && (dir->d_type  & DT_DIR)); }) ; 

  direntries= scandir(game_path , &list, catch_only_dir, alphasort) ;

  if(!direntries)  
    goto _skip_data ; /*  No entry found ! */ 

  while(direntries--) 
  { 
    idx =   __scandepth(root_path  , list[direntries]->d_name , founded_games ,  dbox_game_location_filter) ; 
    if(!idx)
      continue ; 
    
  }  
_skip_data: 
  *(founded_games + idx) =  (void *)0 ; 
  reset_idx_=1;   
  free(list) , list =0 ; 
  free(root_path)  , root_path = 0 ; 
  return founded_games ;  
}

static  int  __scandepth(const char * rpath ,  const char * dirent, char ** collections , new_filter_t custom_filter) 
{
  struct dirent **list = 00 ; 
  char *abs_path= 0 ; 
  static int idx=0 ; 
  int filentries = 0 ; 
  
  if(reset_idx_) 
  {
    idx=0; 
    reset_idx_^=1 ;  
  }

  asprintf(&abs_path, "%s/%s",  rpath , dirent) ; 
  filentries =  scandir(abs_path  ,&list, custom_filter , alphasort); 

  if (!filentries) 
    goto _clean; 
  
  while(filentries--) 
  { 
    asprintf((collections+idx) , "%s/%s",abs_path ,list[filentries]->d_name) ;  
    idx=-~idx; 
  } 

_clean: 
  free(list) , list=0 ; 
  free(abs_path)  ; 
  return   idx ;  
}
static int dbox_game_location_filter(const struct  dirent * dirent) 
{
  //TODO: add multi filter   
  char * extension  = strstr(dirent->d_name , ".zip") ;  
  return  ((!!(dirent->d_type ^ DT_DIR)) && extension)  ; 
}

void  dbox_deallocate_games_list(char ** restrict _Nonnull games)
{
   unsigned int  idx =~0 ; 
   while(*(++idx +games))
     free(*(games+idx)) , *(games+idx)=00 ;  

   free(*games), *games =0 ; 

}
