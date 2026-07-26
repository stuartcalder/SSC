/* Copyright (C) 2020-2025 Stuart Calder
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
 int posix_memalign(void **, size_t, size_t);
 #define SSC_ALIGNED_FREE_IS_POSIX_FREE
 #ifdef _SC_PHYS_PAGES
  #define SSC_HAS_GETTOTALSYSTEMMEMORY
 #endif
 #ifdef _SC_AVPHYS_PAGES
  #define SSC_HAS_GETAVAILABLESYSTEMMEMORY
 #endif
#elif defined(SSC_OS_WINDOWS)
 #include <malloc.h>
 #include <sysinfoapi.h>
 #define SSC_HAS_GETTOTALSYSTEMMEMORY
 #define SSC_HAS_GETAVAILABLESYSTEMMEMORY
#else
 #error "Unsupported."
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Allocate @size bytes of heap memory aligned to @alignment (must be a power of two).
 * Uses posix_memalign() on Unixlike systems or _aligned_malloc() on Windows.
 *
 * Parameters:
 *   @alignment:  The alignment requirement (must be >= size of largest object type and a power of two).
 *                Typically 16, 32, or larger depending on platform requirements.
 *   @size:       The number of bytes to allocate.
 *
 * Returns:
 *   - Pointer to the allocated memory block if successful.
 *   - SSC_NULL (NULL) if allocation fails.
 *
 * Notes:
 *   - Caller is responsible for freeing with SSC_alignedFree().
 *   - @alignment must be a power of two and >= sizeof(largest object).
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE void*
SSC_alignedMalloc(size_t alignment, size_t size)
{
  #if   defined(SSC_OS_UNIXLIKE)
  void* p;
  if (posix_memalign(&p, alignment, size))
    return SSC_NULL;
  return p;
  #elif defined(SSC_OS_WINDOWS)
  return _aligned_malloc(size, alignment);
  #else
   #error "Unsupported OS!"
  #endif
}
/* -> Pointer to allocated memory (SSC_NULL on failure). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Allocate @size bytes of heap memory aligned to @alignment (must be a power of two).
 * Calls exit() on allocation failure via SSC_assertMsg().
 *
 * Parameters:
 *   @alignment:  The alignment requirement (must be >= size of largest object type and a power of two).
 *                Typically 16, 32, or larger depending on platform requirements.
 *   @size:       The number of bytes to allocate.
 *
 * Returns:
 *   - Pointer to the allocated memory block if successful.
 *
 * Behavior:
 *   - Uses SSC_alignedMalloc() internally.
 *   - If allocation fails (returns SSC_NULL), calls exit(1) via SSC_assertMsg().
 *
 * Notes:
 *   - Caller is responsible for freeing with SSC_alignedFree().
 *   - @alignment must be a power of two and >= sizeof(largest object).
 *   - This function does not return on failure; the program terminates.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE void*
SSC_alignedMallocOrDie(size_t alignment, size_t size)
{
  void* p = SSC_alignedMalloc(alignment, size);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_alignedMallocOrDie died!\n");
  return p;
}
/* -> Pointer to allocated memory (never returns on failure). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Free memory previously allocated with SSC_alignedMalloc() or SSC_alignedMallocOrDie().
 * Uses free() on Unixlike systems or _aligned_free() on Windows.
 *
 * Parameters:
 *   @p:  Pointer to the memory block to free (must have been allocated by SSC_alignedMalloc*).
 *        Passing SSC_NULL is safe and has no effect.
 *
 * Behavior:
 *   - On Unixlike systems with POSIX-aligned-free support, calls free().
 *   - On Windows, calls _aligned_free() for proper aligned memory deallocation.
 *   - If @p is SSC_NULL (NULL), does nothing (safe to call).
 *
 * Notes:
 *   - Must be called on memory allocated by SSC_alignedMalloc* functions.
 *   - Memory must have been properly aligned when allocated.
 *   - No error checking performed; caller responsible for ensuring valid pointer.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE void
SSC_alignedFree(void* p)
{
  #if   defined(SSC_OS_UNIXLIKE) && defined(SSC_ALIGNED_FREE_IS_POSIX_FREE)
  free(p);
  #elif defined(SSC_OS_WINDOWS)
  _aligned_free(p);
  #else
   #error "Unsupported OS!"
  #endif
}
/* -> None (void). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Return the size of the operating system's virtual memory pages in bytes.
 *
 * Behavior:
 *   - Unixlike systems: Uses sysconf(_SC_PAGESIZE) to query the page size (typically 4096 bytes).
 *   - Windows: Uses GetSystemInfo() and retrieves dwPageSize from SYSTEM_INFO structure.
 *
 * Returns:
 *   - The OS page size in bytes (e.g., 4096 on most modern systems, 8192 on some servers).
 *
 * Notes:
 *   - Page size is typically a power of two (commonly 4KB or 8KB).
 *   - This value is used for memory alignment and mapping operations.
 *   - The result may vary between different OS versions and architectures.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE size_t
SSC_getPageSize(void)
{
  #if defined(SSC_OS_UNIXLIKE)
  return (size_t)sysconf(_SC_PAGESIZE);
  #elif defined(SSC_OS_WINDOWS)
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return (size_t)si.dwPageSize;
  #else
   #error "Unsupported OS!"
  #endif
}
/* -> Size of memory page in bytes. */

#ifdef SSC_HAS_GETTOTALSYSTEMMEMORY
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Return the total amount of physical system memory (RAM) in bytes.
 *
 * Behavior:
 *   - Unixlike systems with _SC_PHYS_PAGES: Uses sysconf(_SC_PHYS_PAGES) to get page count,
 *     multiplies by SSC_getPageSize() to compute total physical memory.
 *   - Windows: Uses GetPhysicallyInstalledSystemMemory() API from sysinfoapi.h.
 *
 * Returns:
 *   - Total physical memory size in bytes (e.g., 8GB = 8589934592).
 *   - May return 0 on systems where this information is unavailable.
 *
 * Notes:
 *   - This function returns the actual installed RAM, not available memory.
 *   - The SSC_HAS_GETTOTALSYSTEMMEMORY macro guards compilation when the API is unavailable.
 *   - Memory size may vary depending on system architecture and OS implementation.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API size_t
SSC_getTotalSystemMemory(void);
/* -> Total physical RAM in bytes (or 0 if unavailable). */
#endif

#ifdef SSC_HAS_GETAVAILABLESYSTEMMEMORY
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Return the amount of available (free) physical system memory in bytes.
 *
 * Behavior:
 *   - Unixlike systems with _SC_AVPHYS_PAGES: Uses sysconf(_SC_AVPHYS_PAGES) to get available
 *     page count, multiplies by SSC_getPageSize() to compute available memory.
 *   - Windows: Uses GetFreeSystemMemory() API from sysinfoapi.h.
 *
 * Returns:
 *   - Available physical memory size in bytes (e.g., 2GB = 2147483648).
 *   - May return 0 on systems where this information is unavailable.
 *
 * Notes:
 *   - This function returns currently free RAM, not total installed RAM.
 *   - Available memory fluctuates as programs allocate and release memory.
 *   - The SSC_HAS_GETAVAILABLESYSTEMMEMORY macro guards compilation when the API is unavailable.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API size_t
SSC_getAvailableSystemMemory(void);
/* -> Available physical RAM in bytes (or 0 if unavailable). */
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Allocate @n bytes of heap memory using malloc(). Calls exit() on failure via SSC_assertMsg().
 *
 * Parameters:
 *   @n:  Number of bytes to allocate.
 *
 * Returns:
 *   - Pointer to the allocated memory block if successful.
 *
 * Behavior:
 *   - Uses standard malloc() for heap allocation.
 *   - If allocation fails (returns SSC_NULL), calls exit(1) via SSC_assertMsg().
 *
 * Notes:
 *   - Caller is responsible for freeing with free().
 *   - This function does not return on failure; the program terminates.
 *   - Use SSC_callocOrDie() if zero-initialization is required.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE void*
SSC_mallocOrDie(size_t n)
{
  void* p = malloc(n);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_mallocOrDie died!\n");
  return p;
}
/* -> Pointer to allocated memory (never returns on failure). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Allocate (@n_elem * @elem_sz) bytes of heap memory using calloc() with zero-initialization.
 * Calls exit() on failure via SSC_assertMsg().
 *
 * Parameters:
 *   @n_elem:    Number of elements to allocate.
 *   @elem_sz:   Size in bytes of each element.
 *
 * Returns:
 *   - Pointer to the allocated memory block if successful.
 *
 * Behavior:
 *   - Uses standard calloc() for heap allocation with automatic zero-initialization.
 *   - If allocation fails (returns SSC_NULL), calls exit(1) via SSC_assertMsg().
 *
 * Notes:
 *   - Caller is responsible for freeing with free().
 *   - This function does not return on failure; the program terminates.
 *   - Use this when zero-initialization of allocated memory is required.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE void*
SSC_callocOrDie(size_t n_elem, size_t elem_sz)
{
  void* p = calloc(n_elem, elem_sz);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_callocOrDie died!\n");
  return p;
}
/* -> Pointer to zero-initialized memory (never returns on failure). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Change the size of @mem to be @n bytes using realloc(). Calls exit() on failure via SSC_assertMsg().
 *
 * Parameters:
 *   @mem:  Pointer to previously allocated memory (can be SSC_NULL for new allocation).
 *          Must have been allocated by malloc(), calloc(), or previous calls to realloc().
 *   @n:    New size in bytes.
 *
 * Returns:
 *   - Pointer to the reallocated memory block if successful.
 *
 * Behavior:
 *   - Uses standard realloc() for resizing heap allocation.
 *   - If @mem is SSC_NULL (NULL), equivalent to calling malloc(n).
 *   - If @n is 0, returns a unique pointer whose value is unspecified but not NULL.
 *   - If reallocation fails (returns SSC_NULL), calls exit(1) via SSC_assertMsg().
 *
 * Notes:
 *   - Caller is responsible for freeing with free() after use.
 *   - This function does not return on failure; the program terminates.
 *   - The original memory block may be freed and reallocated, so update all pointers to @mem.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE void*
SSC_reallocOrDie(void* R_ mem, size_t n)
{
  void* p = realloc(mem, n);
  SSC_assertMsg(p != SSC_NULL, "Error: SSC_reallocOrDie died!\n");
  return p;
}
/* -> Pointer to reallocated memory (never returns on failure). */

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
