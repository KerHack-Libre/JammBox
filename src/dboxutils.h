//SPDX-License-Identifier:GPL-3.0 
/* 
 * dboxutils.h
 * Operation routine that is only used for dosbox
 *
 * Copyright(c) 2025, Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(DBUTILS)
#define DBUTILS

#include "archive.h" 
#include "attr.h" 

#define EMULNAME(target_emulator)  # target_emulator

enum DOSBOX_PART_DRIVE {c,d,a}; 
#define  DBOX_DRIVE(PART) \
    #PART

enum DOSBOX_DIRECTIVES { 
  IMGMOUNT , 
  /* More to come... */
}; 

#define  FMT_IMGMOUNT\
  "%s %c %s -size %li,%i,%i,%i ? %c: " 

#define CMDFMT(__directive)\
  FMT_##__directive

#define CMDIR(__directive) \
  #__directive

struct dosbox_entry_t  
{ 
  enum DOSBOX_DIRECTIVES doscmd_directive ; 
  const char *game_path ;
  ssize_t byte_sector; 
#ifdef MBRCHS
  struct __chs_t *end ;
#endif   
};
typedef  typeof(int (char ** )) *sandbox_ctx; 

extern char * dbox_emulator; 
extern FILE * memrecord_ptr ; 

/*! Check if dosbox emulator is available on the host */
int dbox_available(void) ;
#define has_dosbox dbox_available  

int dbox_extract(char ** stream_memory_dump,  
                        char * _Nonnull fmt_cmd_payload[static 0x3e8]); 

int dbox_automount(const char * __restrict__ _Nullable part_drive, 
                   struct dosbox_entry_t * __restrict__  _Nonnull  payload); 

int dbox_autorun(const char * __restrict__ internal_start_prog, 
                 sandbox_ctx sandbox_context , void ** cmdmem_dump  ) ;

char ** dbox_games(const char * __restrict__  _Nullable dosbox_game_path)  ;
static int dbox_game_location_filter(const struct dirent * _Nonnull dirent);  

#endif /* DBUTILS */
