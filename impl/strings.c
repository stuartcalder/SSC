/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include <ctype.h>
#include "errors.h"
#include "strings.h"
#include "operations.h"

#undef  R_
#define R_ BASE_RESTRICT
typedef Base_String_Size_t Size_t;

int
Base_String_init(
 char** R_      ctxp, const Size_t size,
 const char* R_ cstr, const Size_t cstr_len)
{
  char* ctx;

  /* If we're initializing from a C string, we need to have enough space. */
  Base_assert_msg(cstr == BASE_NULL || cstr_len <= size, "Base_String_init: cstr_len > size!\n");
  *ctxp = (char*)malloc(size);
  ctx = *ctxp;
  if (ctx == BASE_NULL)
    return -1;
  /* Copy in the total number of bytes allocated. */
  memcpy(ctx, &size, sizeof(size));
  ctx += sizeof(size);
  /* If we were given a C-string, get that data. */
  if (cstr != BASE_NULL) {
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
Base_String_del_flag(char* R_ ctx, int flag)
{
  if (ctx == BASE_NULL)
    return;
  if (flag & BASE_STRING_DEL_SECUREZERO) {
    Size_t sz = Base_String_getbufsize(ctx);
    Base_secure_zero(ctx, sz);
  }
  free(ctx);
}

int
Base_String_make_cstr(char* ctx)
{
  Size_t bufsize = Base_String_getbufsize(ctx);
  Size_t strsize = Base_String_getstrsize(ctx);
  /* Is there enough room to null terminate what is there? */
  if (bufsize - (BASE_STRING_PREFIXBYTES * 2) >= strsize + 1) {
    char* p = Base_String_getdata(ctx) + strsize;
    *p = '\0';
    return 0;
  }
  return -1;
}

void
Base_String_make_cstr_or_die(char* ctx)
{
  Base_assert_msg(!Base_String_make_cstr(ctx), BASE_ERR_S_IN("Base_String_make_cstr failed"));
}

int Base_shift_left_digits(char* R_ str, const int size)
{
  BASE_ASSERT(str);
  int index = 0;
  for (int i = 0; i < size; ++i)
    if (isdigit((unsigned char)str[i]))
      str[index++] = str[i];
  if (index <= size)
    str[index] = '\0';
  return index;
}
