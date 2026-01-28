//SPDX-License-Identifier: GPL-3.0  
/*
 * ui.h
 *
 * Basic Terminal user interface (TUI)  
 * Allows the user  to interact with the launcher 
 * via a basic simple interactive menu. 
 * 
 * Copyright(c) 2025, Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(UI)
#define UI 

#include <stdlib.h> 
#include <curses.h> 
#include <term.h> 
#include <termios.h> 
#include <string.h> 

#include "attr.h" 

#define tx(xcap)\
  tputs(xcap , 1,  putchar)   

#define tp(...)\
  tx(tparm(__VA_ARGS__))  

#define tg(...)\
  tx(tgoto(__VA_ARGS__))   


#define  BANNER_TOP     (1 << 0)  
#define  BANNER_BOTTOM  (1 << 1) 

/* Designed for termios terminal configuration 
 * needed by __configure_term(mode) */
#define INIT   1 
#define BACKUP 2 


//!TODO : Move this  to meson config 
#define  BANNER_TOP_STRING "Jammbox version 1.0 By KerHack-Libre" 
#define  BANNER_BOTTOM_STRING "Jammbox PlayGround" 

/* Used to restor  the default behavior of the terminal */
extern  struct  termios *__backup_tcios;

static inline  int __configure_term(int mode)
{
  unsigned int status = 0 ; 
  if(mode & INIT) 
   {
     struct  termios tcios[2] = {0} ; 
     status = tcgetattr(0 , tcios ) | tcgetattr(0 , (tcios+1));
     if(!__backup_tcios) 
     {
        __backup_tcios = malloc(sizeof(*__backup_tcios));  
        if(!__backup_tcios)  
          return  ~0 ; 
        
        /* The first item is used to restore  or backup the initial 
         * state of the terminal 
         */
        memcpy(__backup_tcios ,  (tcios+0) ,  sizeof(*(tcios))) ; 
     } 

     //This dummy configuration is enought ... 
     (tcios+1)->c_lflag &=~(ICANON | ECHO);  
     status|=tcsetattr(0 , TCSANOW ,(tcios+1)) ;  
   } 
  if(mode & BACKUP) 
  {
     if(!__backup_tcios)
       return ~0 ; 
    
     status  |= tcsetattr(1, TCSANOW, __backup_tcios)  ; 
     free(__backup_tcios) , __backup_tcios = 0 ; 
  }

  return status ; 
}

static inline int  highlight_default_item_at(int position  , int target_index , int default_color )  
{
  if((position ^ target_index)) 
    return 0 ; 

  tp(set_a_background , default_color) ; 
  return position  ; 
}

static inline int  __restor_shell_default_mode(void) 
{
   __configure_term(BACKUP) ; 
   tx(exit_attribute_mode);  
   tx(cursor_visible) ;  
} 


static int ui_sticky_banner(int side, const char * __restrict__ _Nullable)  ; 
static int ui_render(char * , int) ; 
static int ui_menu_interaction(int  hlg ,  int total_items) ;  

extern int  ui_init(void) ; 
extern int  ui_display_menulist(const char ** _Nonnull __item_list , int highlight_npos)  ; 

#endif /* UI */ 
