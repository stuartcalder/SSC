/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#include "macros.h"
#include "mem.h"

#define R_(Ptr) Ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS

/* Base_String's cannot exceed (2^32) - 1 bytes. */
#define BASE_STRING_PREFIXBITS  32
#define BASE_STRING_PREFIXBYTES 4
/* Base_String's use the first 8 bytes to store the amount of memory allocated and
 * the string's current length. */
#define BASE_STRING_MAXSTRLEN   (UINT32_MAX - (BASE_STRING_PREFIXBYTES * 2))
#define BASE_STRING_MAXBUFSIZE  UINT32_MAX
typedef uint32_t Base_String_Size_t;

/* Memory: [total memory allocated ][current string length  ][string bytes]
 * Size:   [BASE_STRING_PREFIXBYTES][BASE_STRING_PREFIXBYTES][current string length] */

/* @ctxp: We will store a new pointer from malloc at *@ctxp.
 * @size: The Base_String will occupy @size bytes in total.
 * @cstr: If not NULL, copy the characters of this C-string as initialization data.
 * @cstr_len: The length of the C-string to copy in. Ignored if @cstr is NULL. */
BASE_API int
Base_String_init(
 R_(char**) ctxp, const Base_String_Size_t size,
 R_(const char*) cstr, const Base_String_Size_t cstr_len);

BASE_INLINE void
Base_String_init_or_die(
 R_(char**) ctxp, const Base_String_Size_t size,
 R_(const char*) cstr, const Base_String_Size_t cstr_len)
{
  Base_assert_msg(!Base_String_init(ctxp, size, cstr, cstr_len), "Base_String_init_or_die failed!\n");
}


BASE_INLINE Base_String_Size_t
Base_String_getbufsize(char* ctx)
{
  Base_String_Size_t sz;
  memcpy(&sz, ctx, sizeof(sz));
  return sz;
}

BASE_INLINE Base_String_Size_t
Base_String_getstrsize(char* ctx)
{
  Base_String_Size_t sz;
  memcpy(&sz, ctx + BASE_STRING_PREFIXBYTES, sizeof(sz));
  return sz;
}

BASE_INLINE char*
Base_String_getdata(char* ctx)
{
  return ctx + (BASE_STRING_PREFIXBYTES * 2);
}

#define BASE_STRING_DEL_SECUREZERO 0x01

/* Delete the string, and pass optional flags
 * obviated by the shorthand inline function below. */
BASE_API void
Base_String_del_flag(R_(char*) ctx, int flag);

/* If there is enough room in the data segment
 * to make the data a C-string, we will null the byte
 * just past the data segment and return 0.
 * Otherwise return -1. */
BASE_API int
Base_String_make_cstr(char* ctx);

BASE_API void
Base_String_make_cstr_or_die(char* ctx);

BASE_INLINE void
Base_String_del(char* ctx)
{ Base_String_del_flag(ctx, 0); }

/* @cstr: Pointer to Null-Terminated String.
 * @size: The length of the string pointed to by @cstr.
 * Scan through @size many bytes of @cstr, shifting all the digit
 * characters encountered to the beginning of @cstr.
 * Return the number of digits shifted. */
BASE_API int
Base_shift_left_digits(R_(char*) cstr, int size);

BASE_END_C_DECLS
#undef R_

#endif
