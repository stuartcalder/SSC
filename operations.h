/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef BASE_OPERATIONS_H
#define BASE_OPERATIONS_H

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "macros.h"
#include "mem.h"
#include "random.h"
#include "swap.h"

#if defined(BASE_LANG_CPP) && (BASE_LANG_CPP >= BASE_CPP_20)
 /* C++20 provides functions for bitwise rotation. */
 #define BASE_OPERATIONS_USE_CPP20_ROT 1
#else
 #define BASE_OPERATIONS_USE_CPP20_ROT 0
#endif

#if BASE_OPERATIONS_USE_CPP20_ROT
 #include <bit>
 #define BASE_ROT_LEFT_(Value, Count, Bits)  std::rotl<uint##Bits##_t>(Value, Count)
 #define BASE_ROT_RIGHT_(Value, Count, Bits) std::rotr<uint##Bits##_t>(Value, Count)
#else
 #if (CHAR_BIT != 8)
  #error "We need 8-bit chars!"
 #endif
 #define BASE_ROT_IMPL_UNSIGNED_MASK_(Bits)       ((uint##Bits##_t)(sizeof(uint##Bits##_t) * CHAR_BIT) - 1)
 #define BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count) (BASE_ROT_IMPL_UNSIGNED_MASK_(Bits) & Count)
 #define BASE_ROT_LEFT_(Value, Count, Bits) \
  ( (Value << BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) | (Value >> ((-BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) & BASE_ROT_IMPL_UNSIGNED_MASK_(Bits))) )
 #define BASE_ROT_RIGHT_(Value, Count, Bits) \
  ( (Value >> BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) | (Value << ((-BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) & BASE_ROT_IMPL_UNSIGNED_MASK_(Bits))) )
#endif

#define R_ BASE_RESTRICT /* Shorthand. */
BASE_BEGIN_C_DECLS
/* Use inline functions instead of macros directly to do bitwise rotation. */
BASE_INLINE uint16_t
Base_rotl_16(uint16_t val, int count)
{
  return BASE_ROT_LEFT_(val, count, 16);
}
BASE_INLINE uint32_t
Base_rotl_32(uint32_t val, int count)
{
  return BASE_ROT_LEFT_(val, count, 32);
}
BASE_INLINE uint64_t
Base_rotl_64(uint64_t val, int count)
{
  return BASE_ROT_LEFT_(val, count, 64);
}
BASE_INLINE uint16_t
Base_rotr_16(uint16_t val, int count)
{
  return BASE_ROT_RIGHT_(val, count, 16);
}
BASE_INLINE uint32_t
Base_rotr_32(uint32_t val, int count)
{
  return BASE_ROT_RIGHT_(val, count, 32);
}
BASE_INLINE uint64_t
Base_rotr_64(uint64_t val, int count)
{
  return BASE_ROT_RIGHT_(val, count, 64);
}
/* Cleanup implementation macros. */
#undef BASE_ROT_LEFT_
#undef BASE_ROT_RIGHT_
#undef BASE_ROT_IMPL_MASKED_COUNT_
#undef BASE_ROT_IMPL_UNSIGNED_MASK_

/* Base_xor_n(write_to, read_from)
 * XOR n bytes beginning at both addresses, and store in @write_to. */
BASE_API void
Base_xor_16(void* R_ writeto, const void* R_ readfrom);

BASE_API void
Base_xor_32(void* R_ writeto, const void* R_ readfrom);

BASE_API void
Base_xor_64(void* R_ writeto, const void* R_ readfrom);

BASE_API void
Base_xor_128(void* R_ writeto, const void* R_ readfrom);

/* When possible, use C23's memset_explicit() to securely
 * zero over memory without optimizations; otherwise fall back
 * to OS-specific methods of secure zeroing. */
#include <string.h>
#if defined(BASE_LANG_C) && (BASE_LANG_C >= BASE_C_23)
 #define SECURE_ZERO_IMPL_(Ptr, Size) { memset_explicit(Ptr, 0, Size); }
#elif defined(BASE_OS_MAC)
 #if !defined(__STDC_WANT_LIB_EXT1__)
  #error "__STDC_WANT_LIB_EXT1__ not defined!"
 #elif __STDC_WANT_LIB_EXT1__ != 1
  #error "__STDC_WANT_LIB_EXT1__ != 1!"
 #endif
 #define SECURE_ZERO_IMPL_(Ptr, Size) { memset_s(Ptr, Size, 0, Size); }
#elif defined(__NetBSD__)
 #define SECURE_ZERO_IMPL_(Ptr, Size) { explicit_memset(Ptr, 0, Size); }
#elif defined(BASE_OS_UNIXLIKE)
 #include <strings.h>
 #define SECURE_ZERO_IMPL_(Ptr, Size) { explicit_bzero(Ptr, Size); }
#elif defined(BASE_OS_WINDOWS)
 #include <windows.h>
 #define SECURE_ZERO_IMPL_(Ptr, Size) { SecureZeroMemory(Ptr, Size); }
#else
 #error "Unsupported!"
#endif

#undef  R_
#define R_ BASE_RESTRICT

/* Zero over the memory @mem with @n zero bytes.
 * Do not optimize away the zeroing. */
BASE_INLINE void
Base_secure_zero(void* R_ mem, size_t n)
SECURE_ZERO_IMPL_(mem, n)

/* Compare the first @size bytes of @mem0 and @mem1.
 * Do the comparison in constant (worst case) time. */
BASE_API size_t
Base_ctime_memdiff(const void* R_ mem0, const void* R_ mem1, size_t size);
/* -> The number of bytes that differed between @mem0 and @mem1. */

/* Compare the first @size bytes of @mem with 0. */
BASE_API bool
Base_is_zero(const void* R_ mem, size_t size);
/* -> true : All bytes of @mem were zero.
 * -> false: At least one byte of @mem was not zero.
 * Return immediately on detection of nonzero. */

/* Compare the first @size bytes of @mem with 0.
 * This comparison is constant time. All @size bytes will be scanned. */
BASE_API bool
Base_ctime_is_zero(const void* R_ mem, size_t size);
/* -> true : All bytes of @mem were zero.
 * -> false: At least one byte of @mem was nonzero.
 * Don't return until all @size bytes of @mem are scanned. */

BASE_END_C_DECLS
#undef SECURE_ZERO_IMPL_
#undef R_

#endif /* ~ ifndef BASE_OPERATIONS_H */
