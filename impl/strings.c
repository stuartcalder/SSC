#include <ctype.h>
#include "strings.h"
#include "operations.h"

#define R_(ptr) ptr BASE_RESTRICT

/* @str:	A null-terminated string.
 * @size:	The number of non-null characters in the string @str.
 * returns:	The number of digit characters shifted to the left. */
int Base_shift_left_digits (R_(char*) str, const int size) {
	BASE_ASSERT(str);
	int index = 0;
	for (int i = 0; i < size; ++i)
		if (isdigit((unsigned char)str[i]))
			str[index++] = str[i];
	if (index <= size)
		str[index] = '\0';
	return index;
}
int Base_String_init (Base_String* base_s, R_(char*) c_s, size_t size) {
	BASE_ASSERT(base_s);
	base_s->c_str = (char*)malloc(size + 1);
	if (!base_s->c_str) return -1;
	if (c_s)
		strcpy(base_s->c_str, c_s);
	else
		memset(base_s->c_str, 0, size);
	base_s->size = size;
	return 0;
}
void Base_String_init_enforced (Base_String* base_s, R_(char*) c_s, size_t size) {
	Base_assert_msg(!Base_String_init(base_s, c_s, size), "Error: Base_String_init failed!\n");
}
void Base_String_del (Base_String* s) {
	BASE_ASSERT(s);
	Base_secure_zero(s->c_str, s->size);
	free(s->c_str);
	*s = (Base_String){0};
}
