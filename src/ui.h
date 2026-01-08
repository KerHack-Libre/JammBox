//SPDX-License-Identifier: GPL-3.0  
/*
 * ui.h
 *
 * Interface d'utilisateur sur le terminal  (TUI) 
 * Permet a l'utilisateur  d'interagir avec le launcher 
 * via un menu. 
 * 
 * Copyright(c) 2025, Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(UI)
#define UI 

#include <curses.h> 
#include <term.h> 
#include "attr.h" 

#define tx(xcap)\
  tputs(xcap , 1,  putchar)   

#define tp(...)\
  tx(tparm(__VA_ARGS__))  

#define tg(...)\
  tx(tgoto(__VA_ARGS__))   


#define  BANNER_TOP     (1 << 0)  
#define  BANNER_BOTTOM  (1 << 1) 

//!TODO : Move this  to meson config 
#define  BANNER_TOP_STRING "Jammbox version 1.0 By KerHack-Libre" 
#define  BANNER_BOTTOM_STRING "Jammbox PlayGround" 



/* Initialise  les termcap disponible  
 * du terminal 
 */
int  ui_init(void) ; 
static int ui_sticky_banner(int side, const char * __restrict__ _Nullable)  ; 
static int ui_render(char * , int) ; 
int ui_display_menulist(const char ** __item_list , int highlight_npos)  ; 

static inline int  highlight_default_item_at(int position  , int target_index , int default_color )  
{
   
  if((position ^ target_index)) 
    return 0 ; 

  tp(set_a_background , default_color) ; 
  return default_color << 8 |  position ;  
}

int ui_menu_interaction(int  hlg ,  int  total_items) ;  
#endif //!UI 
