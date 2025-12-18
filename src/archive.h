//SPDX-License-Identifier:GPL-3.0 
/* archive.h 
 *
 * Compression et Decompression  des archives. 
 * Formats supportes: 
 *       - zip (en cours)
 *       - ... 
 * Copyright(c) 2025,  Umar Ba <jUmarB@protonmail.com> 
 */


#if !defined(ARCHIVE) 
#define  ARCHIVE  

#include  <stdlib.h> 
#include  "attr.h" 

#if defined(__cplusplus) 
extern "C" { 
#endif 

#if _USE_ZIP_ARCHIVE   
# define USE_ZIP_ARCHIVE 
# include <zip.h> 

# define  ZIP_ARCHIVE_FINGERPRINT (0x50|0x4b << 8) 
# define  NON_EMPTY_ARCHIVE 0x403 
# define  EMPTY_ARCHIVE   0x605 
# define  SPANNED_ARCHIVE 0x807 

extern zip_t * za ; 
extern zip_error_t *zerr; 

/*TODO :use function instead */ 
#define ZIP_ERR(fcall,errcode , ...) \
  do{\
    char *s =(char *)0; \
    zip_error_init_with_code(zerr,errcode);\
    asprintf(&s ,  __VA_ARGS__);\
    fprintf(stderr,"%s : %s\012", zip_error_strerror(zerr),s);\
    free(s); zip_error_fini(zerr);\
  }while(0)

int archive_open(const char *__restrict__ archive_filename); 
static int archive_check(const char * __restrict__ archive_filename) ; 
int archive_scan(zip_t * _Nonnull za , char * _Nonnull  lookup_file); 
static  int archive_populate(zip_t* _Nonnull za, zip_stat_t * _Nonnull   zip_entry_file_stat) ;  
static char * archive_get_dirent_path_location(const char *  __restrict__  _Nonnull archive_file);  

#else 
  # warning  "LibZip not Found!"
#endif /*! _USE_ZIP_ARCHIVE */ 


#if defined(__cplusplus) 
}
#endif 



#endif //!ARCHIVE  

