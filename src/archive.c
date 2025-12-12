//SPDX-License-Identifier:GPL-3.0 

#include "archive.h" 
#include <fcntl.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h> 


#ifdef _USE_ZIP_ARCHIVE  
zip_t *za = (void *)0 ;  
zip_error_t *zerr =(void*)0 ; 

int archive_open(const char  *restrict archive_file) 
{
  int zerrno =0 ; 
  za = zip_open(archive_file , 0x10,&zerrno ) ; 
  if(!za)
  {
    ZIP_ERR(zip_open , zerrno ,"fail to open archive file"); 
    return zerrno; 
  } 
  return 0 ; 
}

int archive_scan(zip_t * za ,  const char  *lookup_file) 
{ 
  zip_int64_t total_entries=0;  
  zip_stat_t   zstbuff ; 

  total_entries = zip_get_num_entries(za , ZIP_FL_UNCHANGED);  
  if(!(~0 ^  total_entries))  
    return ~0 ; //ARCHIVE_ERR_ZIP_ENTRIES 

  unsigned int  entry =~0 ;
  while(++entry   < total_entries) 
  {
     if(zip_stat_index(za ,entry, 0  ,  &zstbuff))
       continue ;  //!NOTE : silent ! 
    
     if(!archive_populate(za, &zstbuff)) 
       continue ; 
  }


  return 0 ; 
} 


static  int archive_populate(zip_t* _Nonnull za, zip_stat_t * _Nonnull   zip_entry_file_stat) 
{
  
  zip_file_t * target_file= (zip_file_t *) 00 ; 
  unsigned int fd = ~0 ;    
  
  target_file = zip_fopen_index(za ,  zip_entry_file_stat->index ,0 ); 
  if(!target_file) 
    return 0; 
  
  fd ^= open(zip_entry_file_stat->name ,O_CREAT| O_EXCL | O_RDWR, 
      /*TODO : Appliquer les meme permission que le ficher zip source (en entre) */
      S_IRUSR | S_IWUSR);  

  if(!fd)  
  { 
    if(0x11 != errno)   
      return ~0 ;
    //* NOTE : ignore si le fichier existe deja */ 
    errno = 0 ;  
  }
  
  fd=~fd ; 
  
  char *content_buffer = (char*) malloc(zip_entry_file_stat->size) ;
  if (!content_buffer) 
    return  0 ; 
  
  int64_t ziprb   = zip_fread(target_file , content_buffer , zip_entry_file_stat->size) ;  
  if(0 >= ziprb) 
  {
    fprintf(stderr , "fail to read content \012") ; 
    free(content_buffer) ; 
  }

  write(fd , content_buffer , zip_entry_file_stat->size) ; 

  free(content_buffer) , content_buffer =(void *) 0 ; 
  free(target_file)    , target_file= (void *)0 ; 
  close(fd) ; 
  
  return zip_entry_file_stat->size ;   
} 
#endif 

