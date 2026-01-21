//SPDX-License-Identifier: GPL-3.0  

#include "ui.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <poll.h> 
#include <errno.h> 


struct termios  * __backup_tcios = (void*)0 ;  

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
   
   {  /* Stages running */  
      tx(clear_screen);                                             //> Clear the screen
      tx(cursor_invisible) ;                                        //> Cursor invisible 
      ui_sticky_banner(BANNER_BOTTOM , BANNER_BOTTOM_STRING) ;      //> Draw  banner at the bottom  
      ui_sticky_banner(BANNER_TOP , BANNER_TOP_STRING);             //> Draw  banner at top  
      __configure_term(INIT) ;                                      //> configure terminal attribute 
   } 

   return OK ;  
}

static int ui_sticky_banner(int side ,  const char * __restrict__ title)
{
   
   int remain_column = columns ;  
   char * buff=(char *)00; 
   int  where = 0 ; 
   
   if(strlen(title) >0 )
        remain_column+=~strlen(title); 

   if(side &  BANNER_TOP ) 
     where = 0 ; //TOP LINE  

   if(side & BANNER_BOTTOM) 
     where=  lines; //bottom lines   

  asprintf(&buff , "%s ?%is\012", title , remain_column);   
  return ui_render(buff ,  where) ; 
}

//ui_display_menulist(const char ** , int , const char * where __algn(struct menulocation_t)) ;  
int ui_display_menulist(const char ** item_list , int highlight_item_pos)    
{
  int selected_item = 0  , 
      proceed  =1 , 
      default_item_selected =0, idx=0; 

  //!coords where the menu item should appear 
  unsigned int  xcol  = columns >> 2 , 
                yline = lines >> 2 ; 

  xcol = (xcol << 8 | yline) ;  
 
  while(proceed) 
  {
    while(*(item_list+idx))  
    {
      tg(cursor_address ,(xcol >> 8),yline); 
      highlight_default_item_at( highlight_item_pos, idx, COLOR_YELLOW+idx); 
      
      proceed^= (selected_item == 0xff && highlight_item_pos == idx );  
      
      //> Refreshing  items display by highlighting 
      //+ the current selected item ... 
      if(highlight_item_pos   == idx)  
      {
        printf("%s %10c\012", *(item_list +idx) , 0x20); 
        tx(exit_attribute_mode) ;  
      }else 
        printf("%s %10c\012", *(item_list +idx) , 0x20); 
      
      idx=-~idx , yline-=~0; 
    } 

    if(!proceed) break ;  

    highlight_item_pos  = ui_menu_interaction(highlight_item_pos , idx) ;
    selected_item = (highlight_item_pos & 0xff);  
    highlight_item_pos>>=8; 
    yline= xcol & 0xff ;  
    idx=0 ; 
  }

  __restor_shell_default_mode() ;  
  return  highlight_item_pos ;  
}

static int ui_menu_interaction(int highlight_item_pos , int total_items)
{
  int selected_code = 0  , ready = 0 ;  
  
  struct pollfd kb_evt = {
    .fd = STDIN_FILENO, 
    .events = POLLIN, 
    0, 
  };


  while(!ready)   
  {
    do 
      ready = poll(&kb_evt ,1 ,~0) ; 
    while(errno == EINTR); 

    if(~0 == ready) 
     {
       perror("poll") ;  
       break ; 
     } 

     if(kb_evt.revents &  POLLIN) 
     {
       char  kb  =getc(stdin) ; 

       /** 
        * move the cursor select 
        * w & s : for up and down 
        * j & k : same thing for thoses who use vim keys 
        **/
       switch((kb & 0xff)) 
       { 
          case  'j':
          case  'w':
            if(highlight_item_pos  <= 0 )  
              highlight_item_pos= total_items-1; 
            else
              highlight_item_pos+=~0;
            break; 

          case  'k':
          case  's': 
            highlight_item_pos-=~0; 
            break ;

            /* Space or Enter to approuve  items  */
          case 0x20:
          case 0x0a:
            selected_code  = 0xff; 
            break;   
       }
     } 

  }
  int item_index =  (abs(highlight_item_pos) %  total_items) ;  
  return (item_index << 8| selected_code) ; 
}

