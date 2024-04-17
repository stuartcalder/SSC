/* Copyright (c) 2020-2024 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_STRING_H
#define SSC_STRING_H

#include "Macro.h"
#include "Memory.h"

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/* SSC_String's cannot exceed (2^32) - 1 bytes. */
#define SSC_STRING_PREFIXBITS  32
#define SSC_STRING_PREFIXBYTES 4
/* SSC_String's use the first 8 bytes to store the amount of memory allocated and
 * the string's current length. */
#define SSC_STRING_MAXSTRLEN   (UINT32_MAX - (SSC_STRING_PREFIXBYTES * 2))
#define SSC_STRING_MAXBUFSIZE  UINT32_MAX
typedef uint32_t SSC_StringSize_t;

/* Memory: [total memory allocated ][current string length  ][string bytes]
 * Size:   [SSC_STRING_PREFIXBYTES][SSC_STRING_PREFIXBYTES][current string length] */

/* @ctxp: We will store a new pointer from malloc at *@ctxp.
 * @size: The SSC_String will occupy @size bytes in total.
 * @cstr: If not NULL, copy the characters of this C-string as initialization data.
 * @cstr_len: The length of the C-string to copy in. Ignored if @cstr is NULL. */
SSC_API int
SSC_String_init(
 char** R_              ctxp,
 const SSC_StringSize_t size,
 const char* R_         cstr,
 const SSC_StringSize_t cstr_len);
/* ->0   : Success.
 * ->(-1): Failure. */

SSC_INLINE void
SSC_String_initOrDie(
 char** R_              ctxp,
 const SSC_StringSize_t size,
 const char* R_         cstr,
 const SSC_StringSize_t cstr_len)
{
  SSC_assertMsg(!SSC_String_init(ctxp, size, cstr, cstr_len), "SSC_String_initOrDie failed!\n");
}


/* Get the size of the SSC_String buffer. */
SSC_INLINE SSC_StringSize_t
SSC_String_getBufSize(char* ctx)
{
  SSC_StringSize_t sz;
  memcpy(&sz, ctx, sizeof(sz));
  return sz;
}

/* Get the size of the SSC_String data. */
SSC_INLINE SSC_StringSize_t
SSC_String_getStrSize(char* ctx)
{
  SSC_StringSize_t sz;
  memcpy(&sz, ctx + SSC_STRING_PREFIXBYTES, sizeof(sz));
  return sz;
}

/* Get the SSC_String data. */
SSC_INLINE char*
SSC_String_getData(char* ctx)
{
  return ctx + (SSC_STRING_PREFIXBYTES * 2);
}

#define SSC_STRING_DEL_SECUREZERO 0x01

/* Delete the string, and pass optional flags
 * obviated by the shorthand inline function below. */
SSC_API void
SSC_String_delFlag(char* R_ ctx, int flag);

/* If there is enough room in the data segment
 * to make the data a C-string, we will null the byte
 * just past the data segment and return 0.
 * Otherwise return -1. */
SSC_API SSC_Error_t
SSC_String_makeCstr(char* ctx);
/* ->0   : Success
 * ->(-1): Failure. */

/* TODO */
SSC_API void
SSC_String_makeCstrOrDie(char* ctx);

/* TODO */
SSC_INLINE void
SSC_String_del(char* ctx)
{
  SSC_String_delFlag(ctx, 0);
}

/* @cstr: Pointer to Null-Terminated String.
 * @size: The length of the string pointed to by @cstr.
 * Scan through @size many bytes of @cstr, shifting all the digit
 * characters encountered to the beginning of @cstr. */
SSC_API int
SSC_Cstr_shiftDigitsToFront(char* R_ cstr, int size);
/* -> The number of digits shifted. */

SSC_END_C_DECLS
#undef R_

#endif
