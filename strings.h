/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#include "macros.h"
#include "mem.h"

#undef  R_
#define R_(Ptr) Ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS

typedef struct {
	char*  c_str; /* -> [@size+1] */
	size_t size;
} Base_String;
#define BASE_STRING_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_String, BASE_NULL, 0)

/* @str: Pointer to Null-Terminated String.
 * @size: The length of the string pointed to by @str.
 * Scan through @size many bytes of @str, shifting all the digit
 * characters encountered to the beginning of @str.
 * Return the number of digits shifted. */
BASE_API int Base_shift_left_digits(R_(char*) str, int size);

/* @ctx: Pointer to a Base_String, as context.
 * @str: Null-terminated string, or NULL.
 *
 * Initialize @ctx.
 * Allocate @size + 1 bytes of memory to store the string.
 * If @str is not NULL...
 *   Copy all the bytes including the null terminator.
 * else...
 *   Zero @size + 1 bytes.
 * Return 0 on success, -1 on failure. */
BASE_API int  Base_String_init (Base_String* ctx, R_(const char*) str, size_t size);

/* Try to initialize @ctx. On Failure, call exit(EXIT_FAILURE). */
BASE_API void Base_String_init_or_die (Base_String* ctx, R_(const char*) str, size_t size);

/* TODO
 */
BASE_API void Base_String_del (Base_String*);

#define BASE_LPFX_PREFIXBITS  16
#define BASE_LPFX_PREFIXBYTES 2
#define BAES_LPFX_ENDIAN      BASE_ENDIAN_LITTLE
#define BASE_LPFX_PFX_T       uint16_t
typedef BASE_LPFX_PFX_T       Base_Lpfx_Pfx_t;

BASE_API Base_Lpfx_Pfx_t Base_lpfx_getsize(const char* lpstr);//TODO

BASE_API int Base_lpfx_strcmp(const char* first, const char* second);//TODO

BASE_API int Base_lpfx_strcpy(R_(char*) writeto, R_(const char*) readfrom);//TODO

BASE_API size_t Base_lpfx_strlen(const char* lpstr);//TODO

BASE_END_C_DECLS
#undef R_

#endif
