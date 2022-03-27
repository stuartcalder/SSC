/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_PRINT_H
#define BASE_PRINT_H

#include <stdint.h>
#include <stdio.h>
#include "errors.h"
#include "macros.h"

BASE_BEGIN_C_DECLS

BASE_API void Base_print_bytes
/* Print bytes in hexadecimal format. */
(const void* BASE_RESTRICT bytes, size_t nbytes);

BASE_END_C_DECLS

#endif // ~ BASE_PRINT_H
