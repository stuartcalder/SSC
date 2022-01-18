/* Copyright (c) 2020 Stuart Steven Calder
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
# include <cstdint>
using std::int16_t , std::int32_t , std::int64_t,
      std::uint16_t, std::uint32_t, std::uint64_t;
# include <bit>
# define BASE_ROT_LEFT(value, count, bits) \
         std::rotl<uint##bits##_t>(value, count)
# define BASE_ROT_RIGHT(value, count, bits) \
         std::rotr<uint##bits##_t>(value, count)
#else /* We're not using C++20 or higher. */
# if (CHAR_BIT != 8)
#  error "We need 8-bit chars."
# endif
# define BASE_ROT_IMPL_UNSIGNED_MASK_TYPE(bits)	uint##bits##_t
# define BASE_ROT_IMPL_UNSIGNED_MASK(bits) \
	 ((BASE_ROT_IMPL_UNSIGNED_MASK_TYPE(bits))(sizeof(BASE_ROT_IMPL_UNSIGNED_MASK_TYPE(bits)) * CHAR_BIT) - 1)
# define BASE_ROT_IMPL_MASKED_COUNT(bits, count) \
	 ((BASE_ROT_IMPL_UNSIGNED_MASK(bits)) & count)
# define BASE_ROT_LEFT(value, count, bits) \
	 ((value << BASE_ROT_IMPL_MASKED_COUNT(bits, count)) | (value >> ((-BASE_ROT_IMPL_MASKED_COUNT(bits, count)) & BASE_ROT_IMPL_UNSIGNED_MASK(bits))))
# define BASE_ROT_RIGHT(value, count, bits) \
	 ((value >> BASE_ROT_IMPL_MASKED_COUNT(bits, count)) | (value << ((-BASE_ROT_IMPL_MASKED_COUNT(bits, count)) & BASE_ROT_IMPL_UNSIGNED_MASK(bits))))
#endif

#define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS
BASE_INLINE uint16_t Base_rotl_16(uint16_t val, int count) { return BASE_ROT_LEFT(val, count, 16); }
BASE_INLINE uint32_t Base_rotl_32(uint32_t val, int count) { return BASE_ROT_LEFT(val, count, 32); }
BASE_INLINE uint64_t Base_rotl_64(uint64_t val, int count) { return BASE_ROT_LEFT(val, count, 64); }
BASE_INLINE uint16_t Base_rotr_16(uint16_t val, int count) { return BASE_ROT_RIGHT(val, count, 16); }
BASE_INLINE uint32_t Base_rotr_32(uint32_t val, int count) { return BASE_ROT_RIGHT(val, count, 32); }
BASE_INLINE uint64_t Base_rotr_64(uint64_t val, int count) { return BASE_ROT_RIGHT(val, count, 64); }
/* Base_xor_n(write_to, read_from)
 * XOR n bytes beginning at both addresses, and store in @write_to.
 */
BASE_API    void    Base_xor_16  (R_(void*), R_(const void*));
BASE_API    void    Base_xor_32  (R_(void*), R_(const void*));
BASE_API    void    Base_xor_64  (R_(void*), R_(const void*));
BASE_API    void    Base_xor_128 (R_(void*), R_(const void*));
BASE_INLINE void    Base_secure_zero (R_(void*) buffer, size_t size) {
#if defined(BASE_OS_MAC)
	memset_s(buffer, size, 0, size);
#elif defined(__NetBSD__)
	explicit_memset(buffer, 0, size);
#elif defined(BASE_OS_UNIXLIKE)
	explicit_bzero(buffer, size);
#elif defined(BASE_OS_WINDOWS)
	SecureZeroMemory(buffer, size);
#else
#  error "Unsupported operating system."
#endif
}/* ~ Base_secure_zero */
BASE_API    size_t  Base_ctime_memdiff (R_(const void*), R_(const void*), size_t);
BASE_API    bool    Base_is_zero       (R_(const void*), size_t);
BASE_API    bool    Base_ctime_is_zero (R_(const void*), size_t);
BASE_END_C_DECLS
#undef R_

#endif /* ~ ifndef BASE_OPERATIONS_H */
