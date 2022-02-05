/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#include "macros.h"
#include "types.h"

typedef struct {
	char*  c_str;
	size_t size;
} Base_String;
#define BASE_STRING_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_String, NULL, 0)

#define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS
BASE_API int  Base_shift_left_digits (R_(char*), int);
BASE_API int  Base_String_init (Base_String*, R_(char*), size_t);
BASE_API void Base_String_init_or_die (Base_String*, R_(char*), size_t);
BASE_API void Base_String_del (Base_String*);
BASE_END_C_DECLS
#undef R_

#endif
