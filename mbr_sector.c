//SPDX-Lincense-Identifier:  XXX 

/** 
 * Ce programme a pour but d'analyser les informations de l'image du jeu
 * comme l'en-tete le cylindre et les pistes de secteur. 
 * Copyright(c), 2025 Umar Ba <jUmarB@protonmail.com> 
 * */

#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <err.h> 
#include <errno.h> 
#include <string.h> 
#include <stdint.h> 
#include <assert.h> 

#if __glibc_has_attribute(packed) 
# define  __packed  __attribute__((packed)) 
#else 
# define  __packed 
#endif 

#if __glibc_has_attribute(aligned)
# define __algn(type_t) __attribute__((aligned(__alignof__(sizeof(type_t))))) 
#else 
# define __align(type_t) 
#endif 


#define  MBR_BOOTMAGIC ((0x55) | (0xAA << 8)) 
#define  MBR_PARTAB 4 

#define  mbr_err(...) \
  EXIT_FAILURE;do err(EXIT_FAILURE,  __VA_ARGS__); while(0) 


typedef struct __mbr_sector_t  mbr_sector_t ; 

struct __partition_table_t{ 
  uint8_t  _boot_indicator;  
  uint8_t  _chs_begin[3] ; 
  uint8_t  _partition_type_decriptor; 
  uint8_t  _chs_end[3] ; 
  uint32_t _ss /*staring sector */ , _partsize ; 
}; 

typedef struct __partition_table_t  dospartions[MBR_PARTAB];  

struct __mbr_sector_t { 
  uint8_t  _boot_code[446] ; 
  dospartions  ptabs;  
  uint16_t _boot_sig; 
}__packed;


uint8_t  mbr_sector_has_boot_signature(struct __mbr_sector_t * __restrict__);  


/* l'utilite de ce main function est de tester */
int main(int ac , char *const *av)
{
  unsigned int pstatus= EXIT_SUCCESS, 
               mbrfd  = ~0; 
  const char *dosimg= (char*)00; 
  mbr_sector_t * mbr_sector = 0 ; 
  char *mbraw  __algn(*mbr_sector) =(char *)00 ; 

  if(!(ac &~(1))) 
  {
    errno=0x16; 
    err( (pstatus^=1),"Require DOS/MBR image")  ; 
    goto _eplg ; 
  }

  dosimg = *(av+1); 

  mbrfd^=open(dosimg , O_RDONLY) ; 
  
  if(!mbrfd) 
  {
     pstatus^=mbr_err("opening"); 
     goto _eplg ; 
  } 
  mbrfd=~mbrfd ;

  mbraw = malloc(sizeof(*mbr_sector)) ; 
  if(!mbraw)
  {
     pstatus^=mbr_err("mallocing") ; 
     close(mbrfd) ; 
     goto  _eplg ; 
  } 
  

  size_t rb = read(mbrfd , mbraw , sizeof(*mbr_sector)); 
  close(mbrfd) ; 
  assert(!(rb^sizeof(*mbr_sector))) ;  
  mbr_sector = (mbr_sector_t*)mbraw; 
  
  if(mbr_sector_has_boot_signature(mbr_sector)) 
  {
    pstatus^=mbr_err("No boot signature found  on this disk"); 
    free(mbraw) , mbraw=00; 
    goto _eplg ;  
  } 
  

  //! get cylinder size 
  
  int i = 0;
  while(i <3 ) 
  {
    printf(" -> %i \012" ,mbr_sector->ptabs[0]._chs_end[i]) ;  
    i++ ;
  }
  

_eplg: 
  return pstatus ; 
}



uint8_t  mbr_sector_has_boot_signature(mbr_sector_t* restrict  mbr ) 
{
  return (mbr->_boot_sig ^ MBR_BOOTMAGIC) ;  
}
