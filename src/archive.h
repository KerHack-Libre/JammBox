//SPDX-License-Identifier:GPL-3.0 
/* archive.h 
 *
 * Archive  manipulation. Load, Detect, and uncompress 
 * Format supported: 
 *       - zip [ok]
 *       - (may be in near future or never ... ) 
 * Copyright(c) 2025,  Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(ARCHIVE) 
#define  ARCHIVE  

#include  <sys/stat.h> 
#include  <stdlib.h> 
#include  "attr.h" 
#if _USE_ZIP_ARCHIVE   
# define USE_ZIP_ARCHIVE 
# include <zip.h>  
  
  /* Archive header finger print */
# define  ZIP_ARCHIVE_FINGERPRINT (0x50|0x4b << 8) 
# define  NON_EMPTY_ARCHIVE 0x403 
# define  EMPTY_ARCHIVE     0x605 
# define  SPANNED_ARCHIVE   0x807 

#define ZIP_ERR(fcall,errcode , ...)                            \ 
  do{                                                           \
    char *s =(char *)0;                                         \
    zip_error_init_with_code(zerr,errcode);                     \
    asprintf(&s ,  __VA_ARGS__);                                \
    fprintf(stderr,"%s : %s\012", zip_error_strerror(zerr),s);  \
    free(s); zip_error_fini(zerr);                              \
  }while(0)


extern zip_t * za ; 
extern zip_error_t *zerr; 

typedef struct  __unzip_t  unzip_t  ; 
struct __unzip_t {
  char * _filename ; 
  size_t _size ; 
}; 

#if defined(__cplusplus) 
extern "C" { 
#endif 

static char * archive_get_dirent_path_location(const char *  __restrict__  _Nonnull); 
static struct __unzip_t * archive_populate(zip_t* _Nonnull, zip_stat_t * _Nonnull, unsigned int archive_permode) ;
static __always_inline mode_t  __archive_get_stat(unsigned int  raw_archive_fd) 
{ 
  struct stat sb ; 
  if(fstat(raw_archive_fd , &sb)) 
    return ~0 ; 

  return sb.st_mode & 0x1ff ;  
} 

/* Avoid to populate the archive if the uncompressed file already exists and contains some data 
 * inside. By default this function is enable to desable it should specify the flag
 * -DARCHIVE_FORCE_POPULATE. 
 */
static __always_inline _Bool __archive_auto_cancel_propagation(zip_stat_t  * restrict  st_archive , 
    int check_code)   
{
  return (0 < st_archive->size && !(0x11^check_code));  
} 

/* Archive  basic routines operations  such as :open  check and scaning ... */  
unsigned int archive_open(const char * __restrict__ _Nonnull); 
void archive_close(zip_t * _Nonnull) ; 

unsigned int archive_check(const char * __restrict__ _Nonnull); 
char * archive_scan(zip_t * _Nonnull za , unsigned int archive_status_mode) __algn(struct __unzip_t) ; 

#else 
  # warning  "LibZip not Found!"
#endif /*! _USE_ZIP_ARCHIVE */ 

#if defined(__cplusplus) 
}
#endif 
#endif //!ARCHIVE  
