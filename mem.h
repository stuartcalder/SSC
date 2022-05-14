/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_MEM_H
#define BASE_MEM_H

#include <stdint.h>
#include <string.h>
#include "macros.h"
#include "swap.h"

#if defined(BASE_OS_UNIXLIKE)
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
/* Base_aligned_malloc */
# define BASE_ALIGNED_MALLOC_IMPL(Alignment, Size) { \
 void* p; \
 if (posix_memalign(&p, Alignment, Size)) \
   return BASE_NULL; \
 return p; \
}
/* Base_aligned_free */
# define BASE_ALIGNED_FREE_IMPL(Ptr) { free(Ptr); }
# define BASE_ALIGNED_FREE_IS_POSIX_FREE
/* Base_get_pagesize */
# define BASE_GET_PAGESIZE_IMPL { return (size_t)sysconf(_SC_PAGESIZE); }
# define BASE_GET_PAGESIZE_INLINE

#elif defined(BASE_OS_WINDOWS)
# include <malloc.h>
# include <sysinfoapi.h>
/* Base_aligned_malloc */
# define BASE_ALIGNED_MALLOC_IMPL(Alignment, Size) { return _aligned_malloc(Size, Alignment); }
# define BASE_ALIGNED_MALLOC_INLINE
/* Base_aligned_free */
# define BASE_ALIGNED_FREE_IMPL(Ptr) { _aligned_free(Ptr); }
/* Base_get_pagesize */
# define BASE_GET_PAGESIZE_IMPL { \
 SYSTEM_INFO si; \
 GetSystemInfo(&si); \
 return (size_t)si.dwPageSize; \
}
#else
# error "Unsupported."
#endif

#define R_(p) p BASE_RESTRICT
BASE_BEGIN_C_DECLS

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
BASE_API    void* Base_calloc_or_die  (size_t n_elem, size_t elem_sz);
BASE_API    void* Base_realloc_or_die (R_(void*), size_t);

/* A native load is just memcpy'ing bytes into an automatic variable and returning it. */
#define BASE_LOAD_NATIVE_IMPL(Ptr, Bits) { \
 uint##Bits##_t val; \
 memcpy(&val, Ptr, sizeof(val)); \
 return val; \
}
/* A swap load is memcpy'ing bytes into an automatic variable, byte-swapping it, and returning it. */
#define BASE_LOAD_SWAP_IMPL(Ptr, Bits) { \
 uint##Bits##_t val; \
 memcpy(&val, Ptr, sizeof(val)); \
 return Base_swap_##Bits(val); \
}
/* A native store is merely a memcpy call. */
#define BASE_STORE_NATIVE_IMPL(Ptr, Val) { \
 memcpy(Ptr, &Val, sizeof(Val)); \
}
/* A swap store byte-swaps the bytes before memcpy. */
#define BASE_STORE_SWAP_IMPL(Ptr, Val, Bits) { \
 Val = Base_swap_##Bits(Val); \
 memcpy(Ptr, &Val, sizeof(Val)); \
}

#ifndef BASE_ENDIAN
# error "BASE_ENDIAN undefined!"
#elif (BASE_ENDIAN == BASE_ENDIAN_LITTLE)
# define BASE_STORE_LE_IMPL(Ptr, Val, Bits_) BASE_STORE_NATIVE_IMPL(Ptr, Val)
# define BASE_LOAD_LE_IMPL(Ptr, Bits)        BASE_LOAD_NATIVE_IMPL(Ptr, Bits)
# define BASE_STORE_BE_IMPL(Ptr, Val, Bits)  BASE_STORE_SWAP_IMPL(Ptr, Val, Bits)
# define BASE_LOAD_BE_IMPL(Ptr, Bits)        BASE_LOAD_SWAP_IMPL(Ptr, Bits)
#elif (BASE_ENDIAN == BASE_ENDIAN_BIG)
# define BASE_STORE_BE_IMPL(Ptr, Val, Bits_) BASE_STORE_NATIVE_IMPL(Ptr, Val)
# define BASE_LOAD_BE_IMPL(Ptr, Bits)        BASE_LOAD_NATIVE_IMPL(Ptr, Bits)
# define BASE_STORE_LE_IMPL(Ptr, Val, Bits)  BASE_STORE_SWAP_IMPL(Ptr, Val, Bits)
# define BASE_LOAD_LE_IMPL(Ptr, Bits)        BASE_LOAD_SWAP_IMPL(Ptr, Bits)
#else
# error "Invalid byte-order!"
#endif

BASE_INLINE void Base_store_le16(R_(void*) mem, uint16_t val) BASE_STORE_LE_IMPL(mem, val, 16)
BASE_INLINE void Base_store_le32(R_(void*) mem, uint32_t val) BASE_STORE_LE_IMPL(mem, val, 32)
BASE_INLINE void Base_store_le64(R_(void*) mem, uint64_t val) BASE_STORE_LE_IMPL(mem, val, 64)
BASE_INLINE void Base_store_be16(R_(void*) mem, uint16_t val) BASE_STORE_BE_IMPL(mem, val, 16)
BASE_INLINE void Base_store_be32(R_(void*) mem, uint32_t val) BASE_STORE_BE_IMPL(mem, val, 32)
BASE_INLINE void Base_store_be64(R_(void*) mem, uint64_t val) BASE_STORE_BE_IMPL(mem, val, 64)

BASE_INLINE uint16_t Base_load_le16(const void* mem) BASE_LOAD_LE_IMPL(mem, 16)
BASE_INLINE uint32_t Base_load_le32(const void* mem) BASE_LOAD_LE_IMPL(mem, 32)
BASE_INLINE uint64_t Base_load_le64(const void* mem) BASE_LOAD_LE_IMPL(mem, 64)
BASE_INLINE uint16_t Base_load_be16(const void* mem) BASE_LOAD_BE_IMPL(mem, 16)
BASE_INLINE uint32_t Base_load_be32(const void* mem) BASE_LOAD_BE_IMPL(mem, 32)
BASE_INLINE uint64_t Base_load_be64(const void* mem) BASE_LOAD_BE_IMPL(mem, 64)

#undef BASE_LOAD_LE_IMPL
#undef BASE_LOAD_BE_IMPL
#undef BASE_LOAD_NATIVE_IMPL
#undef BASE_LOAD_SWAP_IMPL
#undef BASE_STORE_LE_IMPL
#undef BASE_STORE_BE_IMPL
#undef BASE_STORE_NATIVE_IMPL
#undef BASE_STORE_SWAP_IMPL

BASE_END_C_DECLS
#undef R_

#endif /* ! */
