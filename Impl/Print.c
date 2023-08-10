/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "Print.h"

#define FORMAT_STR_ "%02hhx"
void
SSC_printBytes(const void* SSC_RESTRICT vbytes, size_t n_bytes)
{
  SSC_ASSERT(vbytes != SSC_NULL);
  if (n_bytes == 0)
    return;
  const uint8_t* bytes = (const uint8_t*)vbytes;
  const size_t backtick_one_index = n_bytes - 1;
  for (size_t i = 0; i < backtick_one_index; ++i)
    printf(FORMAT_STR_, bytes[i]);
  printf(FORMAT_STR_, bytes[backtick_one_index]);
}
