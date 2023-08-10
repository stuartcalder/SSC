/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 *
 * In this file, we define procedures for allocating aligned memory,
 * determining the memory page size of the OS.
 * The *OrDie(...) procedures call exit() on failure. */
#ifndef SSC_MEMORY_H
#define SSC_MEMORY_H

#include <stdint.h>
#include <string.h>

#include "Macro.h"
#include "Swap.h"

#if defined(SSC_OS_UNIXLIKE)
 #include <unistd.h>
 #include <stdlib.h>
/* SSC_alignedMalloc */
 #define SSC_ALIGNED_MALLOC_IMPL(Alignment, Size) {\
  void* p;\
  if (posix_memalign(&p, Alignment, Size))\
    return SSC_NULL;\
  return p;\
 }
 /* SSC_alignedFree */
 #define SSC_ALIGNED_FREE_IMPL(Ptr) { free(Ptr); }
 #define SSC_ALIGNED_FREE_IS_POSIX_FREE
 /* SSC_getPageSize */
 #define SSC_GET_PAGE_SIZE_IMPL { return (size_t)sysconf(_SC_PAGESIZE); }
#elif defined(SSC_OS_WINDOWS)
 #include <malloc.h>
 #include <sysinfoapi.h>
 /* SSC_alignedMalloc */
 #define SSC_ALIGNED_MALLOC_IMPL(Alignment, Size) { return _aligned_malloc(Size, Alignment); }
 /* SSC_alignedFree */
 #define SSC_ALIGNED_FREE_IMPL(Ptr) { _aligned_free(Ptr); }
 /* SSC_getPageSize */
 #define SSC_GET_PAGE_SIZE_IMPL {\
  SYSTEM_INFO si;\
  GetSystemInfo(&si);\
  return (size_t)si.dwPageSize;\
 }
#else
 #error "Unsupported."
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/* Return an object pointer to @size heap bytes, aligned to @alignment. */
SSC_INLINE void*
SSC_alignedMalloc(size_t alignment, size_t size)
SSC_ALIGNED_MALLOC_IMPL(alignment, size)
/* On failure, return SSC_NULL. */

/* Return an object pointer to @size heap bytes, aligned to @alignment. */
SSC_INLINE void*
SSC_alignedMallocOrDie(size_t alignment, size_t size)
{
  void* p = SSC_alignedMalloc(alignment, size);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_alignedMallocOrDie died!\n");
  return p;
}

/* Free memory allocated with SSC_alignedMalloc* */
SSC_INLINE void
SSC_alignedFree(void* p)
SSC_ALIGNED_FREE_IMPL(p)

/* Get the size of the OS's virtual memory pages. */
SSC_INLINE size_t
SSC_getPageSize(void)
SSC_GET_PAGE_SIZE_IMPL

/* Allocate @n bytes on the heap successfully, or terminate the program. */
SSC_INLINE void*
SSC_mallocOrDie(size_t n)
{
  void* p = malloc(n);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_mallocOrDie died!\n");
  return p;
}

/* Allocate (@n_elem * @elem_sz) bytes on the heap successfully, or terminate the program. */
SSC_INLINE void*
SSC_callocOrDie(size_t n_elem, size_t elem_sz)
{
  void* p = calloc(n_elem, elem_sz);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_callocOrDie died!\n");
  return p;
}

/* Change the size of @mem to be @n bytes successfully, or terminate the program. */
SSC_INLINE void*
SSC_reallocOrDie(void* R_ mem, size_t n)
{
  void* p = realloc(mem, n);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_reallocOrDie died!\n");
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
 return SSC_swap##Bits(val);\
}
/* Copy all the bytes of @Val to @Ptr. */
#define STORE_NATIVE_IMPL_(Ptr, Val) {\
 memcpy(Ptr, &Val, sizeof(Val));\
}
/* Reverse the byte-order of @Val, and copy (@Bits/8) bytes to @Ptr. */
#define STORE_SWAP_IMPL_(Ptr, Val, Bits) {\
 Val = SSC_swap##Bits(Val);\
 memcpy(Ptr, &Val, sizeof(Val));\
}

#ifndef SSC_ENDIAN
 #error "SSC_ENDIAN undefined!"
#elif !SSC_ENDIAN_ISVALID(SSC_ENDIAN)
 #error "SSC_ENDIAN is invalid!"
#elif SSC_ENDIAN == SSC_ENDIAN_LITTLE
 #define STORE_LE_IMPL_(Ptr, Val, Bits_) STORE_NATIVE_IMPL_(Ptr, Val)
 #define LOAD_LE_IMPL_(Ptr, Bits)        LOAD_NATIVE_IMPL_(Ptr, Bits)
 #define STORE_BE_IMPL_(Ptr, Val, Bits)  STORE_SWAP_IMPL_(Ptr, Val, Bits)
 #define LOAD_BE_IMPL_(Ptr, Bits)        LOAD_SWAP_IMPL_(Ptr, Bits)
#elif SSC_ENDIAN == SSC_ENDIAN_BIG
 #define STORE_BE_IMPL_(Ptr, Val, Bits_) STORE_NATIVE_IMPL_(Ptr, Val)
 #define LOAD_BE_IMPL_(Ptr, Bits)        LOAD_NATIVE_IMPL_(Ptr, Bits)
 #define STORE_LE_IMPL_(Ptr, Val, Bits)  STORE_SWAP_IMPL_(Ptr, Val, Bits)
 #define LOAD_LE_IMPL_(Ptr, Bits)        LOAD_SWAP_IMPL_(Ptr, Bits)
#else
 #error "SSC_ENDIAN is an invalid byte order!"
#endif

/* Little and big endian stores. */
SSC_INLINE void SSC_storeLittleEndian16(void* R_ mem, uint16_t val) STORE_LE_IMPL_(mem, val, 16)
SSC_INLINE void SSC_storeBigEndian16(void* R_ mem, uint16_t val)    STORE_BE_IMPL_(mem, val, 16)

SSC_INLINE void SSC_storeLittleEndian32(void* R_ mem, uint32_t val) STORE_LE_IMPL_(mem, val, 32)
SSC_INLINE void SSC_storeBigEndian32(void* R_ mem, uint32_t val)    STORE_BE_IMPL_(mem, val, 32)

SSC_INLINE void SSC_storeLittleEndian64(void* R_ mem, uint64_t val) STORE_LE_IMPL_(mem, val, 64)
SSC_INLINE void SSC_storeBigEndian64(void* R_ mem, uint64_t val)    STORE_BE_IMPL_(mem, val, 64)
/* Little and big endian loads. */
SSC_INLINE uint16_t SSC_loadLittleEndian16(const void* mem) LOAD_LE_IMPL_(mem, 16)
SSC_INLINE uint16_t SSC_loadBigEndian16(const void* mem)    LOAD_BE_IMPL_(mem, 16)

SSC_INLINE uint32_t SSC_loadLittleEndian32(const void* mem) LOAD_LE_IMPL_(mem, 32)
SSC_INLINE uint32_t SSC_loadBigEndian32(const void* mem)    LOAD_BE_IMPL_(mem, 32)

SSC_INLINE uint64_t SSC_loadLittleEndian64(const void* mem) LOAD_LE_IMPL_(mem, 64)
SSC_INLINE uint64_t SSC_loadBigEndian64(const void* mem)    LOAD_BE_IMPL_(mem, 64)

#undef LOAD_LE_IMPL_
#undef LOAD_BE_IMPL_
#undef STORE_LE_IMPL_
#undef STORE_BE_IMPL_
#undef LOAD_NATIVE_IMPL_
#undef LOAD_SWAP_IMPL_
#undef STORE_NATIVE_IMPL_
#undef STORE_SWAP_IMPL_

SSC_END_C_DECLS
#undef R_

#endif /* ! */
