

#include <stdlib.h> 
#include <stdio.h> 
#include "ui.h"
#include <unistd.h> 
#include <string.h> 


int main(void) 
{
 
 
  ui_init() ; 

  
  char *list[] =  {
     "menu1" , 
     "menu2" , 
     "menu3" , 
     (void *)00 
  };  

  ui_display_menulist((const char **)list ,  0) ;  
  
  
  return 0 ; 
}
