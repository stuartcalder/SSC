/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
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
# define BASE_OPERATIONS_USE_CPP20_ROT 1
#else
# define BASE_OPERATIONS_USE_CPP20_ROT 0
#endif

#if BASE_OPERATIONS_USE_CPP20_ROT
# include <bit>
# define BASE_ROT_LEFT_(Value, Count, Bits)  std::rotl<uint##Bits##_t>(Value, Count)
# define BASE_ROT_RIGHT_(Value, Count, Bits) std::rotr<uint##Bits##_t>(Value, Count)
#else
# if (CHAR_BIT != 8)
#  error "We need 8-bit chars!"
# endif
# define BASE_ROT_IMPL_UNSIGNED_MASK_(Bits) \
         ((uint##Bits##_t)(sizeof(uint##Bits##_t) * CHAR_BIT) - 1)
# define BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count) \
	 (BASE_ROT_IMPL_UNSIGNED_MASK_(Bits) & Count)
# define BASE_ROT_LEFT_(Value, Count, Bits) \
	 ( (Value << BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) | (Value >> ((-BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) & BASE_ROT_IMPL_UNSIGNED_MASK_(Bits))) )
# define BASE_ROT_RIGHT_(Value, Count, Bits) \
	 ( (Value >> BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) | (Value << ((-BASE_ROT_IMPL_MASKED_COUNT_(Bits, Count)) & BASE_ROT_IMPL_UNSIGNED_MASK_(Bits))) )
#endif

#define R_(Ptr) Ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS
/* Use inline functions instead of macros directly to do bitwise rotation. */
BASE_INLINE uint16_t Base_rotl_16(uint16_t val, int count) { return BASE_ROT_LEFT_(val, count, 16); }
BASE_INLINE uint32_t Base_rotl_32(uint32_t val, int count) { return BASE_ROT_LEFT_(val, count, 32); }
BASE_INLINE uint64_t Base_rotl_64(uint64_t val, int count) { return BASE_ROT_LEFT_(val, count, 64); }
BASE_INLINE uint16_t Base_rotr_16(uint16_t val, int count) { return BASE_ROT_RIGHT_(val, count, 16); }
BASE_INLINE uint32_t Base_rotr_32(uint32_t val, int count) { return BASE_ROT_RIGHT_(val, count, 32); }
BASE_INLINE uint64_t Base_rotr_64(uint64_t val, int count) { return BASE_ROT_RIGHT_(val, count, 64); }
#undef BASE_ROT_LEFT_
#undef BASE_ROT_RIGHT_
#undef BASE_ROT_IMPL_MASKED_COUNT_
#undef BASE_ROT_IMPL_UNSIGNED_MASK_

/* Base_xor_n(write_to, read_from)
 * XOR n bytes beginning at both addresses, and store in @write_to. */
BASE_API void Base_xor_16  (R_(void*) writeto, R_(const void*) readfrom);
BASE_API void Base_xor_32  (R_(void*) writeto, R_(const void*) readfrom);
BASE_API void Base_xor_64  (R_(void*) writeto, R_(const void*) readfrom);
BASE_API void Base_xor_128 (R_(void*) writeto, R_(const void*) readfrom);

#if defined(BASE_OS_MAC)
# if (!defined(__STDC_WANT_LIB_EXT1__) || (__STDC_WANT_LIB_EXT1__ != 1))
#  error "We needed __STDC_WANT_LIB_EXT1__ defined to 1, for access to memset_s!"
# endif
# include <string.h>
# define SECURE_ZERO_IMPL_(Ptr, Size) { memset_s(Ptr, Size, 0, Size); }
#elif defined(__NetBSD__)
# include <string.h>
# define SECURE_ZERO_IMPL_(Ptr, Size) { explicit_memset(Ptr, 0, Size); }
#elif defined(BASE_OS_UNIXLIKE)
# include <strings.h>
# define SECURE_ZERO_IMPL_(Ptr, Size) { explicit_bzero(Ptr, Size); }
#elif defined(BASE_OS_WINDOWS)
# include <windows.h>
# define SECURE_ZERO_IMPL_(Ptr, Size) { SecureZeroMemory(Ptr, Size); }
#else
# error "Unsupported!"
#endif

#undef  R_
#define R_(Ptr) Ptr BASE_RESTRICT

/* Base_secure_zero(mem, n)
 * Zero over the memory @mem with @n zero bytes.
 * Do not optimize away the zeroing. */
BASE_INLINE void Base_secure_zero(R_(void*) mem, size_t n) SECURE_ZERO_IMPL_(mem, n)
#undef SECURE_ZERO_IMPL_
/* Base_ctime_memdiff(m_0, m_1, num_bytes)
 * Compare the first @num_bytes bytes of @m_0 and @m_1.
 * Return the number of bytes that differed between @m_0 and @m_1.
 * Do the comparison in constant (worst case) time.
 */
BASE_API size_t Base_ctime_memdiff(R_(const void*) mem0, R_(const void*) mem1, size_t size);
/* Base_is_zero(mem, num_bytes)
 * Compare the first @num_bytes bytes of @mem with 0.
 * Return false immediately upon detecting a non-zero byte.
 * Return true if all bytes are zero.
 */
BASE_API bool Base_is_zero(R_(const void*) mem, size_t size);
/* Base_ctime_is_zero(mem, num_bytes)
 * Compare the first @num_bytes bytes of @mem with 0.
 * This comparison is constant time. All @num_bytes bytes will be scanned,
 * before determining whether they were all zero or not.
 * Return false if one or more of the @mem bytes were non-zero.
 * Return true if all of the @mem bytes were zero.
 */
BASE_API bool Base_ctime_is_zero(R_(const void*) mem, size_t size);
BASE_END_C_DECLS
#undef R_

#endif /* ~ ifndef BASE_OPERATIONS_H */
