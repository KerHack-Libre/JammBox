//SPDX-License-Identifier: GPL-3.0 

/*  mbr.h 
 *  Representation structurelle  du Master Boot Record. 
 *  En vue d'en extraire les informations utile provenant 
 *  de l'image du jeu jouable sur dosbox. 
 *
 *  Copyright(c) 2025, Umar Ba <jUmarB@protonmail.com>   
 * */


#if !defined(DOSMBR_H) 
#define DOSMBR_H 

#include <stdint.h> 
#include "attr.h"

#define  MBR_BS      ((0x55) | (0xAA << 8))                       /* Master Boot Record Boot Signature      */
#define  MBR_PE      4                                            /* Master Boot Record Partion Entry       */ 
#define  MBR_BC      446                                          /* Master Boot Record Boot Code size      */
#define  MBR_BS_SIZE 2                                            /* Master Boot Record Boot Signature size */

#define  MBR_TS      (MBR_BC + (MBR_PE * 16) + MBR_BS_SIZE)              /* Master Boot Record Total Size          */

typedef struct __mbr_t mbr_t  ;  
typedef struct __chs_t  chs_t ;
typedef struct __global_chs_t  global_chs_t ;  

struct __partition_table_entry{ 
  uint8_t  _boot_flag;  
  uint8_t  _chs_begin[3] ; 
  uint8_t  _partype_desc ; 
  uint8_t  _chs_end[3] ; 
  uint32_t _ss /*staring sector */ , _partsize ; 
};
typedef struct __partition_table_entry   __pte ; 
typedef struct __partition_table_entry   __ptes[MBR_PE];  

struct __mbr_t { 
  uint8_t  _boot_code[446] ; 
  __ptes ptabs;  
  uint16_t _boot_sig; 
}__packed;

struct __chs_t 
{
   uint16_t  _cylinder; 
   uint8_t   _head ; 
   uint8_t   _sector;
}__packed;   

struct __global_chs_t { 
  struct __chs_t  _start  ;
  struct __chs_t  _end ; 
}; 



#endif /* DOSMBR_H */
