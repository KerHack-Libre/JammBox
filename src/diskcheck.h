//SPDX-License-Identifier:GPL-3.0  
/*
 * diskcheck.h 
 *
 * Routine for verifying the disk image at the  
 * boot partition level in order to extract the information
 *
 * Copyright(c)  2025 , Umar Ba <jUmarB@protonmail.com>  
 */ 

#if !defined(DOSBOX_DC) 
#define  DOSBOX_DC 

#include "mbr.h" 

#include <stdlib.h> 
#include <err.h> 
#include <errno.h> 
#include "attr.h"

#define  dc_err(...) err(EXIT_FAILURE, __VA_ARGS__); 
#define  dc_warn(...) warn(__VA_ARGS__)  

#define err_expr(__statement)\
  EXIT_FAILURE;do{__statement;}while(0)

#define  disk_err(__errno) \
  errno=__errno  

enum { 
  INVDIM=~1, 
#define  MESG_INVDIM  "Invalid disk image\012"
};

#define GETMESG(ERROR) \
  fprintf(stderr,MESG_##ERROR)


#if defined(__cpluscplus) 
# define   DBOX extern "C" 
#else 
# define   DBOX  
#endif 

void disk_mesg_err(void) ; 
extern  mbr_t mbr; 

/* !NOTE: load the boot sector in  buffer memory for recurrent io access */  
DBOX uint8_t  diskload(char const * __restrict__ _Nonnull ) ; 
DBOX static uint8_t has_boot_signature(struct  __mbr_t * __restrict__  _Nonnull); 
DBOX struct __partition_table_entry * active_partition(struct __partition_table_entry * __restrict__ _Nonnull) ; 
DBOX struct __global_chs_t * decode_chsbytes(struct __partition_table_entry  * __restrict__  _Nonnull);  


#endif /* DOSBOX_DC */ 
