//SPDX-License-Identifier:GPL-3.0 

#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h> 
#include <fcntl.h>
#include <string.h> 
#include <assert.h> 

#include "diskcheck.h"  

static int scan_pte(mbr_t *) ;   

int main(int ac , char *const *av)
{
  unsigned int pstatus= EXIT_SUCCESS ; 
  const char *dosimg= (char*)00, 
             *data  =  0 ; 
  
  if(!(ac &~(1))) 
  {
    disk_err(-EINVAL) ; 
    err( (pstatus^=1),"Require DOS/MBR image")  ; 
    goto _eplg ; 
  }
  
  dosimg =  *(av+1) ; 
  data = diskload(dosimg); 
  if(!data) 
  {
    err((pstatus^=1),"Fail to load image disk") ;
    goto _eplg; 
  }
  
  mbr =*(mbr_t*) data ; 

  if(!has_boot_signature(&mbr)) 
  {
     err((pstatus^=1) , "Invalid Disk") ; 
     goto _eplg; 
  } 

   int apartno = scan_pte(&mbr) ;  
   if(~0 == apartno) 
   {
     err((pstatus^=1) , "No Active partition found") ; 
     goto _eplg ; 
   }
   __pte *active_boot_partition = (mbr.ptabs+apartno); 
  
   global_chs_t * chsbytes = decode_chsbytes(active_boot_partition) ; 
   if(!chsbytes) 
   {
     err((pstatus^=1) , "Disk Check fail to decode CHS start and end"); 
     goto _eplg ;  
   }

  
_eplg: 
  return pstatus ; 
}


static int scan_pte(mbr_t  * mbr)  
{
  unsigned int  idx =~0  ,
                missed=~0 ; 
  while(++idx < MBR_PE) 
  {
    if(active_partition( (mbr->ptabs+idx) ))  
      break ; 
    
    missed-=~0; 
    
  }
  if(!(missed &~ idx))  
  {
     disk_err(122) ; 
     return ~0  ; 
  }

return  idx; 
}
