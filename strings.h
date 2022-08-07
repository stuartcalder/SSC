/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#include "macros.h"

#define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS

typedef struct {
	char*  c_str; /* -> [@size+1] */
	size_t size;
} Base_String;
#define BASE_STRING_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_String, BASE_NULL, 0)

/* Scan through @size many bytes of @str, shifting all the digit
 * characters encountered to the beginning of @str.
 * Return the number of digits shifted. */
BASE_API int Base_shift_left_digits(R_(char*) str, int size);

/* Initialize @ctx.
 * Allocate @size + 1 bytes of memory to store the string.
 * If @str is not NULL...
 *   Copy all the bytes including the null terminator.
 * else...
 *   Zero @size + 1 bytes.
 * Return 0 on success, -1 on failure. */
BASE_API int  Base_String_init (Base_String* ctx, R_(const char*) str, size_t size);

/* Try to initialize @ctx. On Failure, call exit(EXIT_FAILURE). */
BASE_API void Base_String_init_or_die (Base_String* ctx, R_(const char*) str, size_t size);

/*
 */
BASE_API void Base_String_del (Base_String*);

BASE_END_C_DECLS
#undef R_

#endif
