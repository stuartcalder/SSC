/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 *
 * In this file, we define procedures for allocating aligned memory,
 * determining the memory page size of the OS.
 * The *_or_die(...) procedures call exit() on failure. */
#ifndef BASE_MEM_H
#define BASE_MEM_H

#include <stdint.h>
#include <string.h>
#include "macros.h"
#include "swap.h"

#if defined(BASE_OS_UNIXLIKE)
 #include <unistd.h>
 #include <stdlib.h>
/* Base_aligned_malloc */
 #define BASE_ALIGNED_MALLOC_IMPL(Alignment, Size) {\
  void* p;\
  if (posix_memalign(&p, Alignment, Size))\
    return BASE_NULL;\
  return p;\
 }
 /* Base_aligned_free */
 #define BASE_ALIGNED_FREE_IMPL(Ptr) { free(Ptr); }
 #define BASE_ALIGNED_FREE_IS_POSIX_FREE
 /* Base_get_pagesize */
 #define BASE_GET_PAGESIZE_IMPL { return (size_t)sysconf(_SC_PAGESIZE); }
#elif defined(BASE_OS_WINDOWS)
 #include <malloc.h>
 #include <sysinfoapi.h>
 /* Base_aligned_malloc */
 #define BASE_ALIGNED_MALLOC_IMPL(Alignment, Size) { return _aligned_malloc(Size, Alignment); }
 /* Base_aligned_free */
 #define BASE_ALIGNED_FREE_IMPL(Ptr) { _aligned_free(Ptr); }
 /* Base_get_pagesize */
 #define BASE_GET_PAGESIZE_IMPL {\
  SYSTEM_INFO si;\
  GetSystemInfo(&si);\
  return (size_t)si.dwPageSize;\
 }
#else
 #error "Unsupported."
#endif

#define R_ BASE_RESTRICT
BASE_BEGIN_C_DECLS

/* Return an object pointer to @size heap bytes, aligned to @alignment. */
BASE_INLINE void*
Base_aligned_malloc(size_t alignment, size_t size)
BASE_ALIGNED_MALLOC_IMPL(alignment, size)
/* On failure, return BASE_NULL. */

/* Return an object pointer to @size heap bytes, aligned to @alignment. */
BASE_INLINE void*
Base_aligned_malloc_or_die(size_t alignment, size_t size)
{
  void* p = Base_aligned_malloc(alignment, size);
  Base_assert_msg(p != BASE_NULL, "Error: Base_aligned_malloc_or_die died!\n");
  return p;
}

/* Free memory allocated with Base_aligned_malloc* */
BASE_INLINE void
Base_aligned_free(void* p)
BASE_ALIGNED_FREE_IMPL(p)

/* Get the size of the OS's virtual memory pages. */
BASE_INLINE size_t
Base_get_pagesize(void)
BASE_GET_PAGESIZE_IMPL

/* Allocate @n bytes on the heap successfully, or terminate the program. */
BASE_INLINE void*
Base_malloc_or_die(size_t n)
{
  void* p = malloc(n);
  Base_assert_msg(p != BASE_NULL, "Error: Base_malloc_or_die died!\n");
  return p;
}

/* Allocate (@n_elem * @elem_sz) bytes on the heap successfully, or terminate the program. */
BASE_INLINE void*
Base_calloc_or_die(size_t n_elem, size_t elem_sz)
{
  void* p = calloc(n_elem, elem_sz);
  Base_assert_msg(p != BASE_NULL, "Error: Base_calloc_or_die died!\n");
  return p;
}

/* Change the size of @mem to be @n bytes successfully, or terminate the program. */
BASE_INLINE void*
Base_realloc_or_die(void* R_ mem, size_t n)
{
  void* p = realloc(mem, n);
  Base_assert_msg(p != BASE_NULL, "Error: Base_realloc_or_die died!\n");
  return p;
}

/* Copy the @Bits at @Ptr into an unsigned integer type and return. */
#define LOAD_NATIVE_IMPL_(Ptr, Bits) {\
 uint##Bits##_t val;\
 memcpy(&val, Ptr, sizeof(val));\
 return val;\
}
/* Reverse the byte-order of the @Bits at @Ptr and return as an unsigned integer type. */
#define LOAD_SWAP_IMPL_(Ptr, Bits) {\
 uint##Bits##_t val;\
 memcpy(&val, Ptr, sizeof(val));\
 return Base_swap_##Bits(val);\
}
/* Copy all the bytes of @Val to @Ptr. */
#define STORE_NATIVE_IMPL_(Ptr, Val) {\
 memcpy(Ptr, &Val, sizeof(Val));\
}
/* Reverse the byte-order of @Val, and copy (@Bits/8) bytes to @Ptr. */
#define STORE_SWAP_IMPL_(Ptr, Val, Bits) {\
 Val = Base_swap_##Bits(Val);\
 memcpy(Ptr, &Val, sizeof(Val));\
}

#ifndef BASE_ENDIAN
 #error "BASE_ENDIAN undefined!"
#elif !BASE_ENDIAN_ISVALID(BASE_ENDIAN)
 #error "BASE_ENDIAN is invalid!"
#elif (BASE_ENDIAN == BASE_ENDIAN_LITTLE)
 #define STORE_LE_IMPL_(Ptr, Val, Bits_) STORE_NATIVE_IMPL_(Ptr, Val)
 #define LOAD_LE_IMPL_(Ptr, Bits)        LOAD_NATIVE_IMPL_(Ptr, Bits)
 #define STORE_BE_IMPL_(Ptr, Val, Bits)  STORE_SWAP_IMPL_(Ptr, Val, Bits)
 #define LOAD_BE_IMPL_(Ptr, Bits)        LOAD_SWAP_IMPL_(Ptr, Bits)
#elif (BASE_ENDIAN == BASE_ENDIAN_BIG)
 #define STORE_BE_IMPL_(Ptr, Val, Bits_) STORE_NATIVE_IMPL_(Ptr, Val)
 #define LOAD_BE_IMPL_(Ptr, Bits)        LOAD_NATIVE_IMPL_(Ptr, Bits)
 #define STORE_LE_IMPL_(Ptr, Val, Bits)  STORE_SWAP_IMPL_(Ptr, Val, Bits)
 #define LOAD_LE_IMPL_(Ptr, Bits)        LOAD_SWAP_IMPL_(Ptr, Bits)
#else
 #error "BASE_ENDIAN is an invalid byte order!"
#endif

/* Little and big endian stores. */
BASE_INLINE void Base_store_le16(void* R_ mem, uint16_t val) STORE_LE_IMPL_(mem, val, 16)
BASE_INLINE void Base_store_be16(void* R_ mem, uint16_t val) STORE_BE_IMPL_(mem, val, 16)

BASE_INLINE void Base_store_le32(void* R_ mem, uint32_t val) STORE_LE_IMPL_(mem, val, 32)
BASE_INLINE void Base_store_be32(void* R_ mem, uint32_t val) STORE_BE_IMPL_(mem, val, 32)

BASE_INLINE void Base_store_le64(void* R_ mem, uint64_t val) STORE_LE_IMPL_(mem, val, 64)
BASE_INLINE void Base_store_be64(void* R_ mem, uint64_t val) STORE_BE_IMPL_(mem, val, 64)
/* Little and big endian loads. */
BASE_INLINE uint16_t Base_load_le16(const void* mem) LOAD_LE_IMPL_(mem, 16)
BASE_INLINE uint16_t Base_load_be16(const void* mem) LOAD_BE_IMPL_(mem, 16)

BASE_INLINE uint32_t Base_load_le32(const void* mem) LOAD_LE_IMPL_(mem, 32)
BASE_INLINE uint32_t Base_load_be32(const void* mem) LOAD_BE_IMPL_(mem, 32)

BASE_INLINE uint64_t Base_load_le64(const void* mem) LOAD_LE_IMPL_(mem, 64)
BASE_INLINE uint64_t Base_load_be64(const void* mem) LOAD_BE_IMPL_(mem, 64)

#undef LOAD_LE_IMPL_
#undef LOAD_BE_IMPL_
#undef STORE_LE_IMPL_
#undef STORE_BE_IMPL_
#undef LOAD_NATIVE_IMPL_
#undef LOAD_SWAP_IMPL_
#undef STORE_NATIVE_IMPL_
#undef STORE_SWAP_IMPL_

BASE_END_C_DECLS
#undef R_

#endif /* ! */
