/* Copyright (C) 2020-2025 Stuart Calder
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
#define SSC_ERR_S_FAILED(Str)	    "Error: %s failed!\n", Str
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

/* Internal utility function used by the library to report errors and exit with a specific return code.
 * Parameters:
 *   @code: The integer exit status code to be returned upon termination.
 *   @format: A format string describing the error context.
 * Behavior: Parses variadic arguments, formats them into an error message (typically via err or direct vfprintf),
 *           and then calls exit(code).
 */
SSC_API
void SSC_errxCodeVargs(const int code, const char* R_ format, ...);

#ifdef SSC_ERRX_CODE_LIST_INLINE
 #define  API_      SSC_INLINE
 #define IMPL_(...) SSC_ERRX_CODE_LIST_IMPL(__VA_ARGS__)
#else
 #define  API_      SSC_API
 #define IMPL_(...) ;
#endif
/* Internal core function that handles the actual formatting of error messages and program termination.
 * Parameters:
 *   @code: The exit status code to return after logging the error.
 *   @format: The format string for the error message.
 *   @args: A variadic argument list (va_list) initialized by the caller (via va_start).
 * Behavior: Dispatches the formatted error output either through the system's err function
 *           (if available on Unix-like systems) or directly to stderr. It subsequently calls exit(code)
 *           to terminate the process.
 */
API_ void
SSC_errxCodeList(int const code, const char* R_ format, va_list args)
IMPL_(code, format, args)
#undef  API_
#undef IMPL_

/* Reports a generic fatal error and terminates the program with status code EXIT_FAILURE.
 * Parameters:
 *   @format: A format string describing the failure and any associated data.
 * Behavior: Treats the provided arguments as an error description, prints them to standard error,
 *           and exits the application immediately.
 */
SSC_API void
SSC_errx(const char* R_ format, ...);

/* Issues an assertion error and terminates execution if the provided boolean condition evaluates to false.
 * Parameters:
 *   @condition: The boolean expression that must be true; otherwise, an error occurs.
 *   @format:    A format string (similar to printf) containing details about the failure and any additional variables.
 * Behavior: If condition is false, the function prints the formatted message to standard error and immediately
 *           exits the program with status code EXIT_FAILURE.
 */
SSC_API void
SSC_assertMsg(bool condition, const char* R_ format, ...);

/* Issues a non-fatal warning if the provided boolean condition evaluates to false.
 * Parameters:
 *   @condition: The boolean expression that must be true; otherwise, a warning is generated.
 *   @fmt: A format string containing details about the issue and any additional variables.
 * Behavior: If condition is false, the function prints the formatted message to standard error (stderr).
 *           Unlike an assertion, execution continues after printing the warning.
 */
SSC_API void
SSC_warnMsg(bool condition, const char* R_ format, ...);

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
