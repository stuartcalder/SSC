/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include <ctype.h>
#include "errors.h"
#include "strings.h"
#include "operations.h"

#define R_(ptr) ptr BASE_RESTRICT

/* @str:	A null-terminated string.
 * @size:	The number of non-null characters in the string @str.
 * returns:	The number of digit characters shifted to the left. */
int
Base_shift_left_digits
(R_(char*) str, const int size)
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

int
Base_String_init
(Base_String* base_s, R_(const char*) c_s, size_t num_chars)
{
  BASE_ASSERT(base_s);
  if (!(base_s->c_str = (char*)malloc(num_chars + 1)))
    return -1;
  if (c_s)
    strcpy(base_s->c_str, c_s);
  else
    memset(base_s->c_str, 0, num_chars + 1);
  base_s->size = num_chars;
  return 0;
}
void Base_String_init_or_die (Base_String* base_s, R_(const char*) cstr, size_t num_chars) {
	Base_assert_msg(!Base_String_init(base_s, cstr, num_chars), BASE_ERR_S_FAILED("Base_String_init"));
}
void Base_String_del (Base_String* s) {
	BASE_ASSERT(s);
	free(s->c_str);
	*s = BASE_STRING_NULL_LITERAL;
}
