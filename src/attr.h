//SPDX-License-Identifier: GPL-3.0 

/* attr.h 
 *
 * attributs ou directives provenant de la glibc pour 
 * le compilateur sur certains operation.  
 * 
 * Copyright(c) 2025 , Umar Ba <jUmarB@protonmail.com> 
 */ 

#if defined(__has_feature) 
# if  !__has_feature(nullability) 
  # define _Nonnull 
  # define _Nullable 
# endif 
# else 
  # define _Nonnull 
  # define _Nullable 
#endif 

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

#if __glibc_has_attribute(constructor) 
# define __ctor   __attribute((constructor)) 
#else 
# define __ctor  /* NOTHING  */ 
#endif 
