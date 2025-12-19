//
/*
 */

#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
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
