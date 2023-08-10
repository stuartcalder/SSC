/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_OPERATION_H
#define SSC_OPERATION_H

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Error.h"
#include "Macro.h"
#include "Memory.h"
#include "Random.h"
#include "Swap.h"

#if defined(SSC_LANG_CPP) && (SSC_LANG_CPP >= SSC_CPP_20)
 /* C++20 provides functions for bitwise rotation. */
 #define SSC_OPERATIONS_USE_CPP20_ROT 1
#else
 #define SSC_OPERATIONS_USE_CPP20_ROT 0
#endif

#if SSC_OPERATIONS_USE_CPP20_ROT
 #include <bit>
 #define SSC_ROT_LEFT_(Value, Count, Bits)  std::rotl<uint##Bits##_t>(Value, Count)
 #define SSC_ROT_RIGHT_(Value, Count, Bits) std::rotr<uint##Bits##_t>(Value, Count)
#else
 #if (CHAR_BIT != 8)
  #error "We need 8-bit chars!"
 #endif
 #define SSC_ROT_IMPL_UNSIGNED_MASK_(Bits)       ((uint##Bits##_t)(sizeof(uint##Bits##_t) * CHAR_BIT) - 1)
 #define SSC_ROT_IMPL_MASKED_COUNT_(Bits, Count) (SSC_ROT_IMPL_UNSIGNED_MASK_(Bits) & Count)
 #define SSC_ROT_LEFT_(Value, Count, Bits) \
  ( (Value << SSC_ROT_IMPL_MASKED_COUNT_(Bits, Count)) | (Value >> ((-SSC_ROT_IMPL_MASKED_COUNT_(Bits, Count)) & SSC_ROT_IMPL_UNSIGNED_MASK_(Bits))) )
 #define SSC_ROT_RIGHT_(Value, Count, Bits) \
  ( (Value >> SSC_ROT_IMPL_MASKED_COUNT_(Bits, Count)) | (Value << ((-SSC_ROT_IMPL_MASKED_COUNT_(Bits, Count)) & SSC_ROT_IMPL_UNSIGNED_MASK_(Bits))) )
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS
/* Use inline functions instead of macros directly to do bitwise rotation. */

SSC_INLINE uint16_t
SSC_rotateLeft16(uint16_t val, int count)
{
  return SSC_ROT_LEFT_(val, count, 16);
}

SSC_INLINE uint32_t
SSC_rotateLeft32(uint32_t val, int count)
{
  return SSC_ROT_LEFT_(val, count, 32);
}

SSC_INLINE uint64_t
SSC_rotateLeft64(uint64_t val, int count)
{
  return SSC_ROT_LEFT_(val, count, 64);
}

SSC_INLINE uint16_t
SSC_rotateRight16(uint16_t val, int count)
{
  return SSC_ROT_RIGHT_(val, count, 16);
}

SSC_INLINE uint32_t
SSC_rotateRight32(uint32_t val, int count)
{
  return SSC_ROT_RIGHT_(val, count, 32);
}

SSC_INLINE uint64_t
SSC_rotateRight64(uint64_t val, int count)
{
  return SSC_ROT_RIGHT_(val, count, 64);
}

/* Cleanup implementation macros. */
#undef SSC_ROT_LEFT_
#undef SSC_ROT_RIGHT_
#undef SSC_ROT_IMPL_MASKED_COUNT_
#undef SSC_ROT_IMPL_UNSIGNED_MASK_

/* SSC_xor_n(write_to, read_from)
 * XOR n bytes beginning at both addresses, and store in @write_to. */
SSC_API void
SSC_xor16(void* R_ writeto, const void* R_ readfrom);

SSC_API void
SSC_xor32(void* R_ writeto, const void* R_ readfrom);

SSC_API void
SSC_xor64(void* R_ writeto, const void* R_ readfrom);

SSC_API void
SSC_xor128(void* R_ writeto, const void* R_ readfrom);

/* When possible, use C23's memset_explicit() to securely
 * zero over memory without optimizations; otherwise fall back
 * to OS-specific methods of secure zeroing. */
#include <string.h>
#if defined(SSC_LANG_C) && (SSC_LANG_C >= SSC_C_23)
 #define SECUREZERO_IMPL_(Ptr, Size) { memset_explicit(Ptr, 0, Size); }
#elif defined(SSC_OS_MAC)
 #if !defined(__STDC_WANT_LIB_EXT1__)
  #error "__STDC_WANT_LIB_EXT1__ not defined!"
 #elif __STDC_WANT_LIB_EXT1__ != 1
  #error "__STDC_WANT_LIB_EXT1__ != 1!"
 #endif
 #define SECUREZERO_IMPL_(Ptr, Size) { memset_s(Ptr, Size, 0, Size); }
#elif defined(__NetBSD__)
 #define SECUREZERO_IMPL_(Ptr, Size) { explicit_memset(Ptr, 0, Size); }
#elif defined(SSC_OS_UNIXLIKE)
 #include <strings.h>
 #define SECUREZERO_IMPL_(Ptr, Size) { explicit_bzero(Ptr, Size); }
#elif defined(SSC_OS_WINDOWS)
 #include <windows.h>
 #define SECUREZERO_IMPL_(Ptr, Size) { SecureZeroMemory(Ptr, Size); }
#else
 #error "Unsupported!"
#endif

#undef  R_
#define R_ SSC_RESTRICT

/* Zero over the memory @mem with @n zero bytes.
 * Do not optimize away the zeroing. */
SSC_INLINE void
SSC_secureZero(void* R_ mem, size_t n)
SECUREZERO_IMPL_(mem, n)

/* Compare the first @size bytes of @mem0 and @mem1.
 * Do the comparison in constant (worst case) time. */
SSC_API size_t
SSC_constTimeMemDiff(const void* R_ mem0, const void* R_ mem1, size_t size);
/* -> The number of bytes that differed between @mem0 and @mem1. */

/* Compare the first @size bytes of @mem with 0. */
SSC_API bool
SSC_isZero(const void* R_ mem, size_t size);
/* -> true : All bytes of @mem were zero.
 * -> false: At least one byte of @mem was not zero.
 * Return immediately on detection of nonzero. */

/* Compare the first @size bytes of @mem with 0.
 * This comparison is constant time. All @size bytes will be scanned. */
SSC_API bool
SSC_constTimeIsZero(const void* R_ mem, size_t size);
/* -> true : All bytes of @mem were zero.
 * -> false: At least one byte of @mem was nonzero.
 * Don't return until all @size bytes of @mem are scanned. */

SSC_END_C_DECLS
#undef SECUREZERO_IMPL_
#undef R_

#endif /* ~ ifndef SSC_OPERATION_H */
