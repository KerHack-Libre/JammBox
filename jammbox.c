// SPDX-License-Identifier : GPL-2.0+

/* Ce fichier a pour but de trouver des informations 
 * apropos de l'image du jeu  comme: 
 * l'en-tete (header) et le Cylindre et les pistes de secteur. 
 * Copyright(c) , 2025   Umar Ba <jUmarB@protonmail.com> 
 */

#include <errno.h> 
#include <fcntl.h> 
#include <linux/hdreg.h> 
#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <sys/ioctl.h> 
#include <unistd.h> 

#define jammbox_err(_anotation , ...) \
  do{puts(#_anotation);fprintf(stderr , __VA_ARGS__);}while(0) 

#define errloc strerror(*__errno_location()) 

int main(int ac , char * const *av) 
{

  struct hd_geometry  hdgeo ; 
  unsigned  int hdfd= 0 ;  
  if(!(ac &~(1))) 
  {
     jammbox_err(argument_error ,"Require img file to proceed\012") ; 
     return EXIT_FAILURE ; 
  }

  const char *  imgfile= *(av+1);

  hdfd=open(imgfile , O_RDONLY) ;
  if(!(~0 ^hdfd))  
  {
    jammbox_err(open ,"%s\012",errloc) ; 
    return EXIT_FAILURE ; 
  }

  if(ioctl(hdfd , HDIO_GETGEO , &hdgeo))  
  {
     jammbox_err(image_geometry_error, "Fail to get information about this file : %s \012",errloc) ;
     close(hdfd); 
     return EXIT_FAILURE ; 
  }

  

  return EXIT_SUCCESS ; 
}
