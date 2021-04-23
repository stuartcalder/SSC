/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_STRINGS_H
#define SHIM_STRINGS_H

#include "errors.h"
#include "macros.h"
#include "types.h"

SHIM_BEGIN_DECLS

SHIM_API int 
shim_shift_left_digits (char * SHIM_RESTRICT, int const);

typedef struct {
	char *  c_str;
	ssize_t size;
} Shim_String;

SHIM_API int
shim_string_init (Shim_String *, char * SHIM_RESTRICT, ssize_t);

SHIM_API void
shim_string_init_enforced (Shim_String *, char * SHIM_RESTRICT, ssize_t);

SHIM_API void
shim_string_del (Shim_String *);

SHIM_END_DECLS

#endif
