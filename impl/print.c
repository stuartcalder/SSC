#include "print.h"

#define R_(ptr) ptr BASE_RESTRICT
void Base_print_byte_buffer (R_(uint8_t*) bytes, size_t n_bytes) {
	BASE_ASSERT(bytes);
	if (!n_bytes) return;
	const uint8_t* alias = bytes;
	const size_t backtick_one_index = n_bytes - 1;
#define FORMAT_STR_ "%02hhx"
	for (size_t i = 0; i < backtick_one_index; ++i)
		printf(FORMAT_STR_, alias[i]);
	printf(FORMAT_STR_, alias[backtick_one_index]);
} // ~ Base_print_byte_buffer (uint8_t*, size_t const)
