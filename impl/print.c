/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include "print.h"

#define R_(Ptr) Ptr BASE_RESTRICT
void
Base_print_bytes(R_(const void*) vbytes, size_t n_bytes)
{
  BASE_ASSERT(vbytes != BASE_NULL);
  if (n_bytes == 0) return;
  const uint8_t* bytes = (const uint8_t*)vbytes;
  const size_t backtick_one_index = n_bytes - 1;
  #define FORMAT_STR_ "%02hhx"
  for (size_t i = 0; i < backtick_one_index; ++i)
    printf(FORMAT_STR_, bytes[i]);
  printf(FORMAT_STR_, bytes[backtick_one_index]);
} // ~ Base_print_bytes (uint8_t*, size_t )
