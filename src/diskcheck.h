//SPDX-License-Identifier:GPL-3.0  
/*
 * diskcheck.c 
 * Routine de verification de l'image disk au niveau  
 * de la partition boot enfin d'en extraire les infos
 *
 * Copyright(c)  2025 , Umar Ba <jUmarB@protonmail.com>  
 */ 

#if !defined(DOSBOX_DC) 
#define  DOSBOX_DC 

#include "mbr.h" 

#include <stdlib.h> 
#include <err.h> 
#include <errno.h> 

#define   dc_err(...) \
  EXIT_FAILURE;do err(EXIT_FAILURE, __VA_ARGS__);  while(0) 

#if defined(__cpluscplus) 
# define   DBOX extern "C" 
#else 
# define   DBOX  
#endif 


#define  disk_err(__errno) \
  errno=abs(__errno)  


extern  mbr_t mbr; 
extern char overhead[MBR_TS] __algn(mbr) ;   

//! load the boot sector in  buffer memory for recurrent io access 
DBOX char *  diskload(char const * __restrict__ ) ; 
DBOX uint8_t has_boot_signature(struct  __mbr_t * __restrict__); 
DBOX struct __partition_table_entry * active_partition(struct __partition_table_entry * __restrict__) ; 
DBOX struct __global_chs_t * decode_chsbytes(struct __partition_table_entry  * __restrict__); 


#endif /* DOSBOX_DC */ 
