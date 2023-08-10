/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_ERROR_H
#define SSC_ERROR_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Macro.h"
#include "Typedef.h"

#define SSC_ERR_STR_ALLOC_FAILURE "Error: Generic Allocation Failure.\n"
#define SSC_ERR_S(Str)            "Error: %s!\n", Str
#define SSC_ERR_S_GENERIC(Str)	   SSC_ERR_S(Str) /* TODO: Remove me. */
#define SSC_ERR_S_IN(Str)         "Error: %s in function %s!\n", Str, __func__
#define SSC_ERR_S_FAILED(Str)	  "Error: %s failed!\n", Str
#define SSC_ERR_S_FAILED_IN(Str)  "Error: %s failed in function %s!\n", Str, __func__

#define SSC_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList) {\
 vfprintf(stderr, Fmt, ArgList);\
 va_end(ArgList);\
 exit(Code);\
}
#define SSC_ERRX_CODE_LIST_IMPL_UNIXLIKE(Code, Fmt, ArgList) {\
 verrx(Code, Fmt, ArgList);\
}

#ifdef SSC_OS_UNIXLIKE
 #ifdef __has_include
  #if __has_include(<err.h>)
   #include <err.h>
   #define SSC_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList) SSC_ERRX_CODE_LIST_IMPL_UNIXLIKE(Code, Fmt, ArgList)
   #define SSC_ERRX_CODE_LIST_INLINE
  #else /* Unixlike, but we don't have <err.h>. */
   #define SSC_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList) SSC_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList)
  #endif
 #else /* We don't have __has_include. */
  #define SSC_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList)  SSC_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList)
 #endif
#else
 #define SSC_ERRX_CODE_LIST_IMPL(Code, Fmt, ArgList) SSC_ERRX_CODE_LIST_IMPL_GENERIC(Code, Fmt, ArgList)
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/* TODO: Document. */
SSC_API
void SSC_errxCodeVargs(int const, const char* R_ fmt, ...);

#ifdef SSC_ERRX_CODE_LIST_INLINE
 #define  API_      SSC_INLINE
 #define IMPL_(...) SSC_ERRX_CODE_LIST_IMPL(__VA_ARGS__)
#else
 #define  API_      SSC_API
 #define IMPL_(...) ;
#endif
/* TODO: Document. */
API_ void
SSC_errxCodeList(int const code, const char* R_ fmt, va_list args)
IMPL_(code, fmt, args)
#undef  API_
#undef IMPL_

/* TODO: Document. */
SSC_API void
SSC_errx(const char* R_ fmt, ...);

/* TODO: Document. */
SSC_API void
SSC_assertMsg(bool, const char* R_, ...);

/* TODO: Document. */
SSC_INLINE void
SSC_assert(bool b)
{
  SSC_assertMsg(b, SSC_ERR_S_FAILED("SSC_assert"));
}

SSC_END_C_DECLS
#undef R_

#ifdef SSC_EXTERN_DEBUG
 #define SSC_ASSERT(Bool)    SSC_assert(Bool)
 #define SSC_ASSERT_MSG(...) SSC_assertMsg(__VA_ARGS__)
#else
 #define SSC_ASSERT(Boolean) /* Nil */
 #define SSC_ASSERT_MSG(...) /* Nil */
#endif

#endif /* ! SSC_ERROR_H */
