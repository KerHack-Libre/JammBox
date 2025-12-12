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

#ifdef _USE_ZIP_ARCHIVE  
# include <zip.h> 

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

int archive_open(const char *__restrict__ archive_filename) ;  
int archive_scan(zip_t * _Nonnull za , const char * _Nullable  lookup_file); 
static  int archive_populate(zip_t* _Nonnull za, zip_stat_t * _Nonnull   zip_entry_file_stat) ;  

#endif /*! _USE_ZIP_ARCHIVE */ 



#if defined(__cplusplus) 
}
#endif 


#endif //!ARCHIVE  

