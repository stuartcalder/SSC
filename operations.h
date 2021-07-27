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

#define BASE_ROT_IMPL_UNSIGNED_MASK_TYPE(bits)	uint##bits##_t
#define BASE_ROT_IMPL_UNSIGNED_MASK(bits) \
	((BASE_ROT_IMPL_UNSIGNED_MASK_TYPE(bits))(sizeof(BASE_ROT_IMPL_UNSIGNED_MASK_TYPE(bits)) * CHAR_BIT) - 1)
#define BASE_ROT_IMPL_MASKED_COUNT(bits, count) \
	((BASE_ROT_IMPL_UNSIGNED_MASK(bits)) & count)
#define BASE_ROT_LEFT(value, count, bits) \
	((value << BASE_ROT_IMPL_MASKED_COUNT(bits, count)) | (value >> ((-BASE_ROT_IMPL_MASKED_COUNT(bits, count)) & BASE_ROT_IMPL_UNSIGNED_MASK(bits))))
#define BASE_ROT_RIGHT(value, count, bits) \
	((value >> BASE_ROT_IMPL_MASKED_COUNT(bits, count)) | (value << ((-BASE_ROT_IMPL_MASKED_COUNT(bits, count)) & BASE_ROT_IMPL_UNSIGNED_MASK(bits))))
#define BASE_BIT_CAST_OP(ptr, type_t, tmp_name, statement) \
	do { \
		type_t tmp_name; \
		memcpy(&tmp_name, ptr, sizeof(tmp_name)); \
		statement; \
		memcpy(ptr, &tmp_name, sizeof(tmp_name)); \
	} while (0)

#define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_DECLS
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
BASE_API    bool    Base_is_zero (R_(const void*), size_t);
BASE_API    bool    Base_ctime_is_zero (R_(const void*), size_t);
BASE_END_DECLS
#undef R_

#endif /* ~ ifndef BASE_OPERATIONS_H */
