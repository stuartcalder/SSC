/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_STRING_H
#define SSC_STRING_H

#include "Macro.h"
#include "Memory.h"
#include "Error.h"

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

#define SSC_STRING_TODO 1

#define SSC_STRING_FIELD_BYTES 4
#define SSC_STRING_FIELD_COUNT 3 /* There are 3 "fields" before the string payload. */

#define SSC_STRING_METADATA_BYTES (SSC_STRING_FIELD_BYTES * SSC_STRING_FIELD_COUNT)
#define SSC_STRING_MIN_BUFLEN     (SSC_STRING_METADATA_BYTES + 2)
#define SSC_STRING_MAX_BUFLEN     UINT32_MAX
#define SSC_STRING_MAX_STRLEN     (UINT32_MAX - (SSC_STRING_METADATA_BYTES + 1)))

typedef uint32_t SSC_StringSize_t;
typedef char*    SSC_String_t; /* Make it clear we're accessing pointers to SSC_String_t structured memory. */
typedef char*    SSC_Cstr_t;   /* Make it clear we're accessing pointers to normal C strings. */
enum {
  SSC_STRING_FLAG_SECURE = 0x01, /* Securely zero over string memory when we're done with it. */
  SSC_STRING_FLAG_CSTR   = 0x02, /* Ensure the string is always null-terminated. */
};
/* Memory: [total memory allocated][current string length ][bit flags             ][...payload...]
 *         [SSC_STRING_FIELD_BYTES][SSC_STRING_FIELD_BYTES][SSC_STRING_FIELD_BYTES][.............] */


/* Memory: [total memory allocated ][current string length  ][string bytes         ]
 * Size:   [SSC_STRING_PREFIX_BYTES][SSC_STRING_PREFIX_BYTES][current string length] */

/* @len     : The SSC_String_t will occupy @len bytes in total.
 * @cstr    : If not NULL, copy the characters of this C-string as initialization data.
 * @cstr_len: The length of the C-string to copy in. Ignored if @cstr is NULL.
 * &flags   : Bit-flags changing the behavior of the String. */
SSC_API SSC_String_t
SSC_String_init(
 const SSC_StringSize_t len,
 const SSC_Cstr_t       cstr,
 const SSC_StringSize_t cstr_len,
 const SSC_BitFlag_t    flags);
/* ->0   : Success.
 * ->(-1): Failure. */

SSC_INLINE SSC_String_t
SSC_String_initOrDie(
 const SSC_StringSize_t len,
 const SSC_Cstr_t       cstr,
 const SSC_StringSize_t cstr_len,
 const SSC_BitFlag_t    flags)
{
  SSC_String_t s = SSC_String_init(len, cstr, cstr_len, flags);
  SSC_assertMsg(s != SSC_NULL, "SSC_String_init() failed!\n");
  return s;
}

#if !SSC_STRING_TODO
SSC_API SSC_Error_t
SSC_String_appendString(
  SSC_String_t R_       dest,
  const SSC_String_t R_ src,
  const SSC_Size_t      len);

SSC_API SSC_Error_t
SSC_String_appendCstr(
  SSC_String_t R_        dest,
  const SSC_Cstr_t R_    src,
  const SSC_StringSize_t len)
#endif

SSC_INLINE SSC_StringSize_t*
SSC_String_getBufSizePtr(SSC_String_t ctx)
{
  return (SSC_StringSize_t*)ctx;
}

/* Get the size of the entire SSC_String_t buffer. */
SSC_INLINE SSC_StringSize_t
SSC_String_getBufSize(SSC_String_t ctx)
{
  SSC_StringSize_t sz;
  memcpy(&sz, SSC_String_getBufSizePtr(ctx), sizeof(sz));
  return sz;
}

SSC_INLINE SSC_StringSize_t*
SSC_String_getStrSizePtr(SSC_String_t ctx)
{
  return (SSC_StringSize_t*)(ctx + SSC_STRING_FIELD_BYTES);
}

/* Get the size of the SSC_String_t data. */
SSC_INLINE SSC_StringSize_t
SSC_String_getStrSize(SSC_String_t ctx)
{
  SSC_StringSize_t sz;
  memcpy(&sz, SSC_String_getStrSizePtr(ctx), sizeof(sz));
  return sz;
}

SSC_INLINE SSC_StringSize_t*
SSC_String_getFlagsPtr(SSC_String_t ctx)
{
  return (SSC_StringSize_t*)(ctx + (SSC_STRING_FIELD_BYTES * 2));
}

/* Get the bit flags of the SSC_String_t. */
SSC_INLINE SSC_BitFlag_t
SSC_String_getFlags(SSC_String_t ctx)
{
  SSC_StringSize_t sz;
  memcpy(&sz, SSC_String_getFlagsPtr(ctx), sizeof(sz));
  return (SSC_BitFlag_t)sz;
}

/* Get the SSC_String_t data. */
SSC_INLINE char*
SSC_String_getData(SSC_String_t ctx)
{
  return (char*)(ctx + (SSC_STRING_FIELD_BYTES * 3));
}

SSC_INLINE bool
SSC_String_isEmpty(SSC_String_t ctx)
{
  //TODO
  return true;
}

SSC_API void
SSC_String_del(SSC_String_t ctx);

/* If there is enough room in the data segment
 * to make the data a C-string, we will null the byte
 * just past the data segment and return SSC_OK.
 * Otherwise return SSC_ERR. */
SSC_API SSC_Error_t
SSC_String_makeCstr(SSC_String_t ctx);

SSC_API void
SSC_String_makeCstrOrDie(SSC_String_t ctx);

/* @cstr: Pointer to Null-Terminated String.
 * @size: The length of the string pointed to by @cstr.
 * Scan through @size many bytes of @cstr, shifting all the digit
 * characters encountered to the beginning of @cstr. */
SSC_API int
SSC_Cstr_shiftDigitsToFront(SSC_Cstr_t cstr, int size);
/* -> The number of digits shifted. */

SSC_END_C_DECLS
#undef R_

#endif
