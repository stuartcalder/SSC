/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef BASE_ERRORS_H
#define BASE_ERRORS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"

#define BASE_ERR_STR_ALLOC_FAILURE "Error: Generic Allocation Failure.\n"
#define BASE_ERR_S(Str)            "Error: %s!\n", Str
#define BASE_ERR_S_GENERIC(Str)	   BASE_ERR_S(Str) /* TODO: Remove me. */
#define BASE_ERR_S_IN(Str)         "Error: %s in function %s!\n", Str, __func__
#define BASE_ERR_S_FAILED(Str)	   "Error: %s failed!\n", Str
#define BASE_ERR_S_FAILED_IN(Str)  "Error: %s failed in function %s!\n", Str, __func__

/* For functions that return 0 on success and (-1) on failure,
 * use the Base_Error_t typedef, so it is unambiguous why
 * we are returning an integer. */
typedef int Base_Error_t;

#define BASE_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList) {\
 vfprintf(stderr, Fmt, ArgList);\
 va_end(ArgList);\
 exit(Code);\
}
#define BASE_ERRX_CODE_LIST_IMPL_UNIXLIKE(Code, Fmt, ArgList) {\
 verrx(Code, Fmt, ArgList);\
}

#ifdef BASE_OS_UNIXLIKE
 #ifdef __has_include
  #if __has_include(<err.h>)
   #include <err.h>
   #define BASE_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList) BASE_ERRX_CODE_LIST_IMPL_UNIXLIKE(Code, Fmt, ArgList)
   #define BASE_ERRX_CODE_LIST_INLINE
  #else /* Unixlike, but we don't have <err.h>. */
   #define BASE_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList) BASE_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList)
  #endif
 #else /* We don't have __has_include. */
  #include <err.h>
  #define BASE_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList)  BASE_ERRX_CODE_LIST_IMPL_UNIXLIKE(Code, Fmt, ArgList)
  #define BASE_ERRX_CODE_LIST_INLINE
 #endif
#else
 #define BASE_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList) BASE_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList)
#endif

#define R_ BASE_RESTRICT
BASE_BEGIN_C_DECLS

/* TODO: Document. */
BASE_API
void Base_errx_code_vargs(int code, const char* R_ fmt, ...);

#ifdef BASE_ERRX_CODE_LIST_INLINE
 #define  API_      BASE_INLINE
 #define IMPL_(...) BASE_ERRX_CODE_LIST_IMPL(__VA_ARGS__)
#else
 #define  API_      BASE_API
 #define IMPL_(...) ;
#endif
/* TODO: Document. */
API_ void
Base_errx_code_list(int code, const char* R_ fmt, va_list args)
IMPL_(code, fmt, args)
#undef  API_
#undef IMPL_

/* TODO: Document. */
BASE_API void
Base_errx(const char* R_ fmt, ...);

/* TODO: Document. */
BASE_API void
Base_assert_msg(bool, const char* R_, ...);

/* TODO: Document. */
BASE_INLINE void
Base_assert(bool b)
{
  Base_assert_msg(b, BASE_ERR_S_FAILED("Base_assert"));
}

BASE_END_C_DECLS
#undef R_

#ifdef BASE_EXTERN_DEBUG
 #define BASE_ASSERT(Bool)    Base_assert(Bool)
 #define BASE_ASSERT_MSG(...) Base_assert_msg(__VA_ARGS__)
#else
 #define BASE_ASSERT(Boolean) /* Nil */
 #define BASE_ASSERT_MSG(...) /* Nil */
#endif

#endif /* ! BASE_ERRORS_H */
