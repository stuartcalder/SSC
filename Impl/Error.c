/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "Error.h"

#define R_ SSC_RESTRICT

void SSC_assertMsg(bool b, const char* R_ fmt, ...)
{
  if (!b) {
    va_list vl;
    va_start(vl, fmt);
    SSC_errxCodeList(EXIT_FAILURE, fmt, vl);
  }
}

void SSC_errxCodeVargs(int code, const char* R_ fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  SSC_errxCodeList(code, fmt, vl);
}

#ifndef SSC_ERRX_CODE_LIST_INLINE
void
SSC_errxCodeList(int code, const char* R_ fmt, va_list arg_list)
SSC_ERRX_CODE_LIST_IMPL(code, fmt, arg_list)
#endif

void SSC_errx(const char* R_ fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  SSC_errxCodeList(EXIT_FAILURE, fmt, vl);
}
