/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include <ctype.h>
#include "errors.h"
#include "strings.h"
#include "operations.h"

#undef  R_
#define R_(ptr) ptr BASE_RESTRICT

int Base_shift_left_digits(R_(char*) str, const int size)
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

int Base_String_init(Base_String* ctx, R_(const char*) str, size_t size)
{
  BASE_ASSERT(ctx);
  if (!(ctx->c_str = (char*)calloc(size + 1, 1)))
    return -1;
  if (str) {
    size_t len = strlen(str);
    memcpy(ctx->c_str, str, size + 1);
    ctx->size = len;
  } else {
    memset(ctx->c_str, 0, size + 1);
    ctx->size = size;
  }
  return 0;
}

void Base_String_init_or_die (Base_String* base_s, R_(const char*) cstr, size_t num_chars)
{
  Base_assert_msg(!Base_String_init(base_s, cstr, num_chars), BASE_ERR_S_FAILED("Base_String_init"));
}

void Base_String_del (Base_String* s)
{
  BASE_ASSERT(s);
  free(s->c_str);
  *s = BASE_STRING_NULL_LITERAL;
}

/* POST_END_: Postfix Endian. */
#if   (BASE_LPFX_ENDIAN == BASE_ENDIAN_LITTLE)
# define POST_END_ le
#elif (BASE_LPFX_ENDIAN == BASE_ENDIAN_BIG)
# define POST_END_ be
#else
# error "BASE_LPFX_ENDIAN is an invalid value!"
#endif

#define LOAD_SZ_IMPL_(Ptr, PfxBits, PostEnd) Base_load_##PostEnd##PfxBits(Ptr)
#define LOAD_SZ_(Ptr)               LOAD_SZ_IMPL_(Ptr, BASE_LPFX_PREFIXBITS, POST_END_)

Base_Lpfx_Pfx_t Base_lpfx_getsize(const char* lpstr)
{
  Base_Lpfx_Pfx_t size = LOAD_SZ_(lpstr);
  return size;
}

int Base_lpfx_strcmp(const char* first, const char* second)
{
//TODO: FIXME: Finish.
  Base_Lpfx_Pfx_t size;
  result = memcmp(first, second, BASE_LPFX_PREFIXBYTES);
  if (result <= 0) {
    size = Base_lpfx_getsize(first);
  } else {
    size = Base_lpfx_getsize(second);
  }
  if (result < 0)
    size = Base_lpfx_getsize(f);
    memcpy(&size, f, sizeof(size));
  else if (result > 0)
    memcpy(&size, s, sizeof(size));
  else
    memcpy(&size, f, sizeof(size));
  f += BASE_LPFX_PREFIXBYTES;
  s += BASE_LPFX_PREFIXBYTES;
  for (Base_Lpfx_Pfx_t i = 0; i < size; ++i) {
    if (f[i] < s[i])
      return -1;
    else if (f[i] > s[i])
      return 1;
  }
  return 0;
}
