/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include <ctype.h>
#include "errors.h"
#include "strings.h"
#include "operations.h"

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
