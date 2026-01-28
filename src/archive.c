//SPDX-License-Identifier:GPL-3.0 

#include "archive.h"

#include <fcntl.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h> 


#ifdef USE_ZIP_ARCHIVE  
zip_t *za = (void *)0 ;  
zip_error_t *zerr =(void*)0 ; 

char * archive_file_root_dir=(void *)0 ; 

unsigned int  archive_open(const char  *restrict archive_file) 
{ 

  int zerrno =0 ; 
  unsigned int check_status =  archive_check(archive_file) ; 
  if(~0 == check_status)
    return  ~0;  

  za = zip_open(archive_file , 0x10,&zerrno);  
  if(!za)
  {
    ZIP_ERR(zip_open , zerrno ,"fail to open archive file"); 
    return zerrno; 
  } 
  
  archive_file_root_dir = archive_get_dirent_path_location(archive_file); 
  return check_status ;  
}

unsigned int archive_check(const char * archive_filename)
{
  uint32_t signature= 0x0; 
  mode_t regusr_mode = 0 ;
  unsigned int check_status = ~0 ; 

  int fd  = open(archive_filename ,  O_RDONLY) ; 
  if(!(~0 ^ fd)) 
    return ~0; 

  regusr_mode = __archive_get_stat(fd) ;  

  read(fd ,  &signature , sizeof(signature)) ; 
  close(fd) ; 

  if((signature & 0xffff) ^ (ZIP_ARCHIVE_FINGERPRINT))  
    return ~0 ;  //!ZIP_ARCHIVE_BAD_SIGNATURE  

  switch( (signature >> 0x10))  
  {
    case NON_EMPTY_ARCHIVE:
      check_status = 1 ;break;     
    case EMPTY_ARCHIVE: 
      check_status=  2 ;break; 
    case SPANNED_ARCHIVE:
      check_status = 4 ; break;  
  }
  if(~0 ==check_status) 
    return ~0 ; 

  return (check_status << 0x10) | regusr_mode ; 

}
static char  * archive_get_dirent_path_location(const char * restrict archive_file) 
{
  char *dirent_location = strrchr(archive_file , 0x2f); 

  if(!dirent_location) 
    return (char *)00 ; 
  
  ssize_t len  = dirent_location - archive_file ; 
  if(0 == len) 
    return (char *)00; 

  return  strndup(archive_file , len);  
}

char * archive_scan(zip_t * za , unsigned  int status_mode) 
{ 
  zip_int64_t total_entries=0;  
  zip_stat_t   zstbuff ;  

  //> The uncompressed_data is aligned to fit with struct __unzip_t  data structure  
  //+ Please see how __unzip_t is defined  @ archive.h header .... 
  //-------------------------------------------------------------------------------
  char *uncompressed_data  __algn(struct __unzip_t) = (char *)00 ; 

  total_entries = zip_get_num_entries(za , ZIP_FL_UNCHANGED);  
  if( 0 == total_entries)  
    return (void *)~0  ; //ARCHIVE_ERR_ZIP_ENTRIES 

  unsigned int  entry =~0 ;
  while(++entry   < total_entries) 
  {
     int rc  = zip_stat_index(za ,entry, 0  ,  &zstbuff) ; 
     if(rc) 
       continue ;  //!NOTE : silent ! 
 
     //!FIXME : I supposed that, the archive should have only 1 entry ...  
     uncompressed_data = strdup((char *) archive_populate(za, &zstbuff,  status_mode)) ;
     
     if(!uncompressed_data ||  (void *)~0  == uncompressed_data) 
       continue ; 
  }

  free(archive_file_root_dir)  , archive_file_root_dir =0 ; 
  return uncompressed_data; 
} 

static unzip_t * archive_populate(zip_t* za, zip_stat_t *  zip_entry_file_stat , unsigned int permode) 
{
  
  unzip_t  * unzip = (unzip_t *) 00 ; 
  zip_file_t * target_file= (zip_file_t *) 00 ; 
  unsigned int fd = ~0 ,  pops = 0 ;  
  char * path =  (char*)00 ;  

  target_file = zip_fopen_index(za ,  zip_entry_file_stat->index ,0 ); 
  if(!target_file) 
    return 0; 

  asprintf(&path, "%s/%s", archive_file_root_dir , zip_entry_file_stat->name) ; 
  fd ^= open(path ,O_CREAT| O_EXCL | O_RDWR,permode)    ; 

  free(archive_file_root_dir); 
  if(!fd)  
  { 
    if(0x11 != errno)   
      return (void *)~0; 

    pops = errno,errno = 0 ;  
  }
  
  fd=~fd ; 
  unzip=  malloc(sizeof(*unzip)) ; 
  if(!unzip) 
    goto __free_target_file ;  

  unzip->_filename = strdup(path) ; 
  unzip->_size = zip_entry_file_stat->size;  
  free(path) , path=0 ; 

  /* > By default this flag is not defined. And the default
   * + behavior  is to skip  re-populate the contains of
   * + archive file. When this flag is set a  compile time, that's mean 
   * + every time you execute the code it'll propagate the file(s)
   * + from compressed archive! 
   * */
#if !defined(ARCHIVE_FORCE_POPULATE) 
  if(__archive_auto_cancel_propagation(zip_entry_file_stat,  pops))
    goto __free_target_file ;
#endif 
  
  char *content_buffer = (char*) malloc(zip_entry_file_stat->size);
  if (!content_buffer) 
    goto __free_target_file ;

  int64_t ziprb   = zip_fread(target_file , content_buffer , zip_entry_file_stat->size) ;  
  if(0 >= ziprb) 
  {
    fprintf(stderr , "fail to read content \012") ; 
    goto __free_content_buffer ;  
  }

  write(fd , content_buffer , zip_entry_file_stat->size) ; 

__free_content_buffer: 
  free(content_buffer) , content_buffer =(void *) 0 ; 

__free_target_file: 
  free(target_file)    , target_file= (void *)0 ; 


  close(fd) ; 
 
 

  return unzip ; 
} 
#endif 

