//SPDX-License-Identifier: GPL-3.0  

#include "ui.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 

static unsigned  int  termdim =0 ; 

static int ui_render(char * buffer ,  int wherearea)  
{
  int exit_status =0  ; 
  char *s =  strchr(buffer ,  '?');  
  if(!s)
  {
    exit_status=~0 ;  
    goto _free_buff ;  
  }
  *s = '%';  

  tg(cursor_address ,0 , wherearea); 

  tp(set_a_background , COLOR_YELLOW) ; 
  fprintf(stdout ,buffer,"") ; 
  tx(exit_attribute_mode) ; 

_free_buff: 
  free(buffer),buffer=00 ; 
  
  return  exit_status ; 
}


int ui_init(void)   
{
   int erret = 0 ; 
   
   int status = setupterm((void *)00 , STDOUT_FILENO ,&erret);   
   if(ERR == status)
   {
     switch(erret) 
     {
        case ~0 : fprintf(stderr , "terminfo database could not be found\012") ;break; 
        case 0  : fprintf(stderr , "the terminal could not be found\012"); break; 
        case 1  : fprintf(stderr , "the terminal is hardcopy, and cannot be used\
                      for curses applications.\012"); break; 
        default: 
                  fprintf(stderr , "Unknow Error\012") ;break; 
     }

     return ~0; 
   }

   tx(clear_screen); 
   termdim = columns<<8 | lines ; 
   return OK ;  
}

//TODO : Add color attribute 
int ui_draw_layout(int side ,  const char * __restrict__ title /*Color attribute*/)
{
   
   int remain_column = termdim>> 8 ;  
   char * buff=(char *)00; 
   int  which_line  = 0;  
   int  where = 0 ; 
   if(strlen(title) >0 )
        remain_column+=~strlen(title); 

   if(side &  HEADER) 
   { 
     where = 0 ; 
   }

   if(side & FOOTER) 
     where= termdim  & 0xff;  


  asprintf(&buff , "%s ?%is\012", title , remain_column);   
  return ui_render(buff ,  where) ; 
}


int ui_display_menulist(const char ** item_list , int highlight_item_pos)    
{ 
  struct coords { 
     unsigned xcol ,  yline ;  
  } cursmov = { 
     columns >> 2  , 
     lines   >> 2 
  }; 
  int default_item_selected =0  ; 
  char i = 0 ; 
  while(*(item_list+i))  
  {
     tg(cursor_address ,  cursmov.xcol  , cursmov.yline); 

     default_item_selected = highlight_default_item_at( highlight_item_pos, i , COLOR_YELLOW); 
     if(default_item_selected) 
     {
       printf("%s %10s<\012", *(item_list +i) , " "); 
       tx(exit_attribute_mode) ;  
     }else 
       printf("%s %10s\012", *(item_list +i) , " "); 
     
     i=-~i ; 
     cursmov.yline-=~0; 
   } 
   

   return default_item_selected ;  

}
