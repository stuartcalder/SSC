/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_PRINT_H
#define BASE_PRINT_H

#include <stdint.h>
#include <stdio.h>
#include "errors.h"
#include "macros.h"

BASE_BEGIN_DECLS
BASE_API void Base_print_bytes (const void* BASE_RESTRICT, size_t);
BASE_END_DECLS

#endif // ~ BASE_PRINT_H
