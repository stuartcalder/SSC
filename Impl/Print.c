/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "Print.h"
#define R_ SSC_RESTRICT
#define HEX_FORMAT_STR_ "%02hhx"

static void
print_hex_(const uint8_t* R_ bytes, size_t n_bytes, SSC_BitFlag8_t flags)
{
  if (flags & SSC_PRINT_PREFIX)
    printf("0x");
  for (size_t i = 0; i < n_bytes; ++i)
    printf(HEX_FORMAT_STR_, bytes[i]);
  if (flags & SSC_PRINT_NEWLINE)
    putchar('\n');
}

static void
print_bin_(const uint8_t* R_ bytes, size_t n_bytes, SSC_BitFlag8_t flags)
{
  static const char* const nibble_table[16] = {
    "0000", /* 0x0 */
    "0001", /* 0x1 */
    "0010", /* 0x2 */
    "0011", /* 0x3 */
    "0100", /* 0x4 */
    "0101", /* 0x5 */
    "0110", /* 0x6 */
    "0111", /* 0x7 */
    "1000", /* 0x8 */
    "1001", /* 0x9 */
    "1010", /* 0xA */
    "1011", /* 0xB */
    "1100", /* 0xC */
    "1101", /* 0xD */
    "1110", /* 0xE */
    "1111"  /* 0xF */
  };

  if (flags & SSC_PRINT_PREFIX)
    printf("0b");
  for (size_t i = 0; i < n_bytes; ++i) {
    const uint8_t upper = bytes[i] >> 4;
    const uint8_t lower = bytes[i] &  UINT8_C(0xF);
    printf("%s%s", nibble_table[upper], nibble_table[lower]);
  }
  if (flags & SSC_PRINT_NEWLINE)
    putchar('\n');
}

void
SSC_printBytesMode(const void* R_ vbytes, size_t n_bytes, SSC_BitFlag8_t flags)
{
  SSC_ASSERT(vbytes != SSC_NULL);
  if (n_bytes == 0)
    return;
  const uint8_t* bytes = (const uint8_t*)vbytes;

  if (flags & SSC_PRINT_BIN) {
    print_bin_(bytes, n_bytes, flags);
  } else { /* Just assume HEX mode if it's not BIN mode. */
    print_hex_(bytes, n_bytes, flags);
  }
}
