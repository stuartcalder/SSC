/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_PRINT_H
#define SSC_PRINT_H

#include <stdint.h>
#include <stdio.h>
#include "Error.h"
#include "Macro.h"

SSC_BEGIN_C_DECLS

/* Print bytes in hexadecimal format. */
SSC_API void
SSC_printBytes(const void* SSC_RESTRICT bytes, size_t nbytes);

SSC_END_C_DECLS

#endif // ~ SSC_PRINT_H
