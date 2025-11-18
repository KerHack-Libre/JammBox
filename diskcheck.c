//SPDX-Lincense-Identifier: GPL-3.0 

#include "diskcheck.h" 

#include <stdio.h> 
#include <unistd.h> 
#include <stdint.h> 
#include <assert.h>
#include <errno.h> 

mbr_t mbr ; 
char overhead[MBR_TS] = {0} ;  


char  * diskload(char const * dosimg) 
{ 
   size_t br =0; 
   FILE *fp  = fopen(dosimg , "rb") ;  
   if(!fp) 
   { 
     disk_err(-EIO) ; 
     return (char *)0 ; 
   }
   
   br = fread(overhead ,1 , MBR_TS ,fp) ;  
   assert(!(br ^ sizeof(overhead))) ; 
   fclose(fp);

   return  overhead ; 

}

uint8_t  has_boot_signature(mbr_t *restrict  mbr ) 
{
  return  !mbr->_boot_sig ^ MBR_BS ; 
}


__pte *  active_partition(__pte* restrict current_partition) 
{
  if((current_partition->_boot_flag & 0x80))  
    return current_partition ; 
  
   return (__pte*)00 ;  
} 


global_chs_t * decode_chsbytes(__pte *  restrict active_partition) 
{
  uint32_t chs_value=  *(uint32_t *) active_partition->_chs_begin; 
  chs_value&=0xfffff; 
  
  global_chs_t *  chs =  malloc(sizeof(*chs)) ; 
  if(!chs) 
  { 
    disk_err(-ENOMEM); 
    return (global_chs_t *) 00;  
  }
  
  //Starting CHS 
  chs->_start._cylinder=0 ,  chs->_start._cylinder|=(chs_value >> 0x10)  ; 
  chs->_start._head    =(chs_value & 0xff) , chs->_start._head-=~0; 
  chs->_start._sector  =(chs_value >> 8  & 0xff); 
  
  //End CHS 
  
  chs_value&=~chs_value , chs_value= *(uint32_t*) active_partition->_chs_end & 0xffffff ;    

  chs->_end._cylinder=0 , chs->_end._cylinder|=((chs_value  >> 0x10 )& 0x3ff); 
  chs->_end._head    =(chs_value  & 0xff); 
  chs->_end._sector  =(chs_value >> 8  & 0xff) ; 
 

  chs->_end._cylinder |= (chs->_end._sector  & 0xc0)<< 2 ;    
  chs->_end._sector &= 0x3f ; 

 
  return chs ; 

}
