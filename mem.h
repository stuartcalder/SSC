#ifndef BASE_MEM_H
#define BASE_MEM_H

#include "macros.h"

#if defined(BASE_OS_UNIXLIKE)
#  include <unistd.h>
#  include <stdlib.h>
/* Base_aligned_malloc */
#  define BASE_ALIGNED_MALLOC_IMPL(alignment, size) { \
     void* p; \
     if (posix_memalign(&p, alignment, size)) \
       return NULL; \
     return p; \
   }
/* Base_aligned_free */
#  define BASE_ALIGNED_FREE_IMPL(ptr) { free(ptr); }
/* Base_get_pagesize */
#  define BASE_GET_PAGESIZE_IMPL { return (size_t)sysconf(_SC_PAGESIZE); }
#  define BASE_GET_PAGESIZE_INLINE

#elif defined(BASE_OS_WINDOWS)
#  include <malloc.h>
#  include <sysinfoapi.h>
/* Base_aligned_malloc */
#  define BASE_ALIGNED_MALLOC_IMPL(alignment, size) { return _aligned_malloc(size, alignment); }
#  define BASE_ALIGNED_MALLOC_INLINE
/* Base_aligned_free */
#  define BASE_ALIGNED_FREE_IMPL(ptr) { _aligned_free(ptr); }
/* Base_get_pagesize */
#  define BASE_GET_PAGESIZE_IMPL { \
     SYSTEM_INFO si; \
     GetSystemInfo(&si); \
     return (size_t)si.dwPageSize; \
   }
#else
#  error "Unsupported."
#endif

#define R_(p) p BASE_RESTRICT
BASE_BEGIN_DECLS
#ifdef BASE_ALIGNED_MALLOC_INLINE
BASE_INLINE void*  Base_aligned_malloc (size_t alignment, size_t size) BASE_ALIGNED_MALLOC_IMPL(alignment, size)
#else
BASE_API    void*  Base_aligned_malloc (size_t alignment, size_t size);
#endif
BASE_API    void*  Base_aligned_malloc_or_die (size_t alignment, size_t size);

BASE_INLINE void   Base_aligned_free (void* p) BASE_ALIGNED_FREE_IMPL(p)

#ifdef BASE_GET_PAGESIZE_INLINE
BASE_INLINE size_t Base_get_pagesize (void) BASE_GET_PAGESIZE_IMPL
#else
BASE_API    size_t Base_get_pagesize (void);
#endif

BASE_API    void* Base_malloc_or_die  (size_t);
BASE_API    void* Base_calloc_or_die  (size_t num_elements, size_t element_size);
BASE_API    void* Base_realloc_or_die (R_(void*), size_t);
BASE_END_DECLS
#undef R_

#endif /* ! */
