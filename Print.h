/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_PRINT_H
#define SSC_PRINT_H

#include <stdint.h>
#include <stdio.h>
#include "Error.h"
#include "Macro.h"
#include "Typedef.h"

#define R_ SSC_RESTRICT

/* SSC_BitFlag8_t */
enum {
  SSC_PRINT_HEX    = 0x01, /* Print bytes as hexadecimal. */
  SSC_PRINT_BIN    = 0x02, /* Print bytes in binary.      */
  SSC_PRINT_PREFIX = 0x04, /* Print the "prefix". i.e. 0x or 0b. */
};

SSC_BEGIN_C_DECLS

SSC_API void
SSC_printBytesMode(const void* R_ bytes, size_t nbytes, SSC_BitFlag8_t mode);

SSC_INLINE void
SSC_printBytes(const void* R_ bytes, size_t nbytes)
{
  SSC_printBytesMode(bytes, nbytes, SSC_PRINT_HEX);
}

SSC_END_C_DECLS

#undef R_

#endif // ~ SSC_PRINT_H
