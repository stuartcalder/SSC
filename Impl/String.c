/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include <ctype.h>
#include "Error.h"
#include "String.h"
#include "Operation.h"

#undef  R_
#define R_ SSC_RESTRICT
typedef SSC_StringSize_t Size_t;

int
SSC_String_init(
 char** R_      ctxp,
 const Size_t   size,
 const char* R_ cstr,
 const Size_t   cstr_len)
{
  char* ctx;

  /* If we're initializing from a C string, we need to have enough space. */
  SSC_assertMsg(cstr == SSC_NULL || cstr_len <= size, "SSC_String_init: cstr_len > size!\n");
  *ctxp = (char*)malloc(size);
  ctx = *ctxp;
  if (ctx == SSC_NULL)
    return -1;
  /* Copy in the total number of bytes allocated. */
  memcpy(ctx, &size, sizeof(size));
  ctx += sizeof(size);
  /* If we were given a C-string, get that data. */
  if (cstr != SSC_NULL) {
    memcpy(ctx, &cstr_len, sizeof(cstr_len));
    ctx += sizeof(cstr_len);
    memcpy(ctx, cstr, cstr_len);
  } else {
    memset(ctx, 0x00, sizeof(Size_t));
    /* The rest is undefined. */
  }

  return 0;
}

void
SSC_String_delFlag(char* R_ ctx, int flag)
{
  if (ctx == SSC_NULL)
    return;
  if (flag & SSC_STRING_DEL_SECUREZERO) {
    Size_t sz = SSC_String_getBufSize(ctx);
    SSC_secureZero(ctx, sz);
  }
  free(ctx);
}

SSC_Error_t
SSC_String_makeCstr(char* ctx)
{
  Size_t bufsize = SSC_String_getBufSize(ctx);
  Size_t strsize = SSC_String_getStrSize(ctx);
  /* Is there enough room to null terminate what is there? */
  if (bufsize >= strsize + (SSC_STRING_PREFIXBYTES * 2) + 1) {
    char* p = SSC_String_getData(ctx) + strsize;
    *p = '\0';
    return SSC_OK;
  }
  return SSC_ERR;
}

void
SSC_String_makeCstrOrDie(char* ctx)
{
  SSC_assertMsg(!SSC_String_makeCstr(ctx), SSC_ERR_S_IN("SSC_String_make_cstr failed"));
}

int
SSC_Cstr_shiftDigitsToFront(char* R_ str, const int size)
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
