/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_ERRORS_H
#define SHIM_ERRORS_H

#include "macros.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef SHIM_OS_UNIXLIKE
#	include <err.h>
#	define SHIM_ERRORS_INLINE_ERRX_CODE_LIST
#endif

#define SHIM_ERR_STR_ALLOC_FAILURE "Error: Generic Allocation Failure!\n"

SHIM_API void
shim_errx_code_vargs (int, char const * SHIM_RESTRICT, ...);

#ifdef SHIM_ERRORS_INLINE_ERRX_CODE_LIST
#	define API_ static inline
#else
#	define API_ SHIM_API
#endif

API_ void
shim_errx_code_list (int, char const * SHIM_RESTRICT, va_list);

#undef API_

#ifdef SHIM_OS_UNIXLIKE
#	define SHIM_ERRORS_ERRX_CODE_LIST_IMPL(code, fmt, arg_list) { \
		verrx(code, fmt, arg_list); \
	}
#else
#	define SHIM_ERRORS_ERRX_CODE_LIST_IMPL(code, fmt, arg_list) { \
		vfprintf(stderr, fmt, arg_list); \
		va_end(arg_list); \
		exit(code); \
	}
#endif

#ifdef SHIM_ERRORS_INLINE_ERRX_CODE_LIST
void
shim_errx_code_list (int code, char const * SHIM_RESTRICT fmt, va_list arg_list)
	SHIM_ERRORS_ERRX_CODE_LIST_IMPL(code, fmt, arg_list)
#endif

SHIM_API void
shim_errx (char const * SHIM_RESTRICT fmt, ...);

SHIM_API void
shim_assert_msg (bool, char const * SHIM_RESTRICT, ...);

static inline void
shim_assert (bool b) {
	shim_assert_msg(b, "ERROR: shim_assert failed!\n");
}

#ifdef SHIM_EXT_DEBUG
#	define SHIM_ASSERT(boolean)		 shim_assert(boolean)
#	define SHIM_ASSERT_MSG(boolean, msg_ptr) shim_assert_msg(boolean, msg_ptr)
#else
#	define SHIM_ASSERT(boolean)		 /* Nil */
#	define SHIM_ASSERT_MSG(boolean, msg_ptr) /* Nil */
#endif

#endif /* ~ SHIM_ERRORS_H */
