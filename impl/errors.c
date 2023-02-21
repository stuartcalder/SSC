/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include "errors.h"

#define R_ BASE_RESTRICT

void Base_assert_msg(bool b, const char* R_ fmt, ...)
{
  if (!b) {
    va_list vl;
    va_start(vl, fmt);
    Base_errx_code_list(EXIT_FAILURE, fmt, vl);
  }
}

void Base_errx_code_vargs(int code, const char* R_ fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  Base_errx_code_list(code, fmt, vl);
}

#ifndef BASE_ERRX_CODE_LIST_INLINE
void Base_errx_code_list(int code, const char* R_ fmt, va_list arg_list) BASE_ERRX_CODE_LIST_IMPL(code, fmt, arg_list)
#endif

void Base_errx(const char* R_ fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  Base_errx_code_list(EXIT_FAILURE, fmt, vl);
}
