/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_ERRORS_H
#define BASE_ERRORS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"

#define BASE_ERR_STR_ALLOC_FAILURE "Error: Generic Allocation Failure.\n"

#ifdef BASE_OS_UNIXLIKE
#	include <err.h>
#	define BASE_ERRX_CODE_LIST_IMPL(code, fmt, arg_list) { verrx(code, fmt, arg_list); }
#	define BASE_ERRX_CODE_LIST_INLINE
#else
#	define BASE_ERRX_CODE_LIST_IMPL(code, fmt, arg_list) { \
		vfprintf(stderr, fmt, arg_list); \
		va_end(arg_list); \
		exit(code); \
	}
#endif

#define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_DECLS
BASE_API    void Base_errx_code_vargs (int, R_(const char*), ...);
#ifdef BASE_ERRX_CODE_LIST_INLINE
BASE_INLINE void Base_errx_code_list (int code, R_(const char*) fmt, va_list args) BASE_ERRX_CODE_LIST_IMPL(code, fmt, args)
#else
BASE_API    void Base_errx_code_list (int code, R_(const char*) fmt, va_list args);
#endif
BASE_API    void Base_errx (R_(const char*) fmt, ...);
BASE_API    void Base_assert_msg (bool, R_(const char*), ...);
BASE_INLINE void Base_assert (bool b) { Base_assert_msg(b, "ERROR: Base_assert failed.\n"); }
BASE_END_DECLS
#undef R_

#ifdef BASE_EXTERN_DEBUG
#	define BASE_ASSERT(boolean) Base_assert(boolean)
#	define BASE_ASSERT_MSG(...) Base_assert_msg(__VA_ARGS__)
#else
#	define BASE_ASSERT(boolean) /* Nil */
#	define BASE_ASSERT_MSG(...) /* Nil */
#endif

#endif /* ~ BASE_ERRORS_H */
