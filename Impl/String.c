/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include <ctype.h>
#include "Error.h"
#include "String.h"
#include "Operation.h"

#undef  R_
#define R_ SSC_RESTRICT
/* Pointers */
typedef SSC_String_t     String_t;
typedef SSC_Cstr_t       Cstr_t;
/* Integers */
typedef SSC_StringSize_t Size_t;
typedef SSC_BitFlag_t    Flag_t;

String_t
SSC_String_init(
 const Size_t len,
 const Cstr_t cstr,
 const Size_t cstr_len,
 const Flag_t flags)
{
  String_t ctx;

  /* If we're initializing from a C string, we need to have enough space. */
  if (cstr != SSC_NULL && cstr_len > len)
    return SSC_NULL;
  const Size_t sz = len + SSC_STRING_METADATA_BYTES;
  if (sz < SSC_STRING_MIN_BUFLEN)
    return SSC_NULL;

  ctx = (String_t)malloc(sz);
  if (ctx == SSC_NULL)
    return SSC_NULL;

  String_t p = ctx;
  /* Copy in the total number of bytes allocated. */
  memcpy(p, &sz, sizeof(sz));
  p += sizeof(sz);
  /* If we were given a C-string, get that data. */
  if (cstr != SSC_NULL) {
    memcpy(p, &cstr_len, sizeof(cstr_len));
    p += sizeof(cstr_len);
    memcpy(p, cstr, cstr_len);
  } else {
    memset(p, 0x00, sizeof(Size_t));
    /* The rest is left undefined. */
  }

  return ctx;
}

void
SSC_String_del(String_t ctx)
{
  if (ctx != SSC_NULL) {
    Flag_t flags = SSC_String_getFlags(ctx);
    if (flags & SSC_STRING_FLAG_SECURE) {
      Size_t len = SSC_String_getBufSize(ctx);
      SSC_secureZero(ctx, len);
    }
    free(ctx);
  }
}

SSC_Error_t
SSC_String_makeCstr(String_t ctx)
{
  Size_t bufsize = SSC_String_getBufSize(ctx);
  Size_t strsize = SSC_String_getStrSize(ctx);
  /* Is there enough room to null terminate what is there? */
  if (bufsize >= strsize + (SSC_STRING_FIELD_BYTES * 2) + 1) {
    char* p = SSC_String_getData(ctx) + strsize;
    *p = '\0';
    return SSC_OK;
  }
  return SSC_ERR;
}

void
SSC_String_makeCstrOrDie(String_t ctx)
{
  SSC_assertMsg(!SSC_String_makeCstr(ctx), SSC_ERR_S_IN("SSC_String_make_cstr failed"));
}

int
SSC_Cstr_shiftDigitsToFront(Cstr_t str, const int size)
{
  SSC_ASSERT(str);
  int index = 0;
  for (int i = 0; i < size; ++i)
    if (isdigit((unsigned char)str[i]))
      str[index++] = str[i];
  if (index <= size)
    str[index] = '\0';
  return index;
}
