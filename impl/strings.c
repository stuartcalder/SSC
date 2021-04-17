#include "strings.h"
#include "operations.h"
#include <ctype.h>

/* @str:	A null-terminated string.
 * @size:	The number of non-null characters in the string @str.
 * returns:	The number of digit characters shifted to the left. */
int
shim_shift_left_digits (char * SHIM_RESTRICT str, int const size) {
	SHIM_ASSERT(str);
	int index = 0;
	for (int i = 0; i < size; ++i)
		if (isdigit((unsigned char)str[i]))
			str[index++] = str[i];
	if (index <= size)
		str[index] = '\0';
	return index;
}

int
shim_string_init (Shim_String * shim_s, char * SHIM_RESTRICT c_str, size_t size) {
	SHIM_ASSERT(shim_s);
	shim_s->c_str = (char *)malloc(size + 1);
	if (!shim_s->c_str)
		return -1;
	if (c_str)
		strcpy(shim_s->c_str, c_str);
	else
		memset(shim_s->c_str, 0, size);
	shim_s->size = size;
	return 0;
}

void
shim_string_init_enforced (Shim_String * shim_s, char * SHIM_RESTRICT c_str, size_t size) {
	if (shim_string_init(shim_s, c_str, size))
		SHIM_ERRX("ERROR: shim_string_init failed!\n");
}

void
shim_string_del (Shim_String * s) {
	SHIM_ASSERT(s);
	shim_secure_zero(s->c_str, s->size);
	free(s->c_str);
	*s = (Shim_String){0};
}
