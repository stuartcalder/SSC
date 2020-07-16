/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_PRINT_H
#define SHIM_PRINT_H
#include "macros.h"
#include <stdio.h>
#include <stdint.h>

SHIM_BEGIN_DECLS

extern inline void
shim_print_byte_buffer (uint8_t * SHIM_RESTRICT bytes, size_t const num_bytes) {
	if( num_bytes == 0 )
		return;
	uint8_t const *alias = bytes;
	size_t const backtick_one_index = num_bytes - 1;
#define FORMAT_STR_ "%02hhx"
	for( size_t i = 0; i < backtick_one_index; ++i )
		printf( FORMAT_STR_, alias[ i ] );
	printf( FORMAT_STR_, alias[ backtick_one_index ] );
#undef FORMAT_STR_
} // ~ shim_print_byte_buffer (uint8_t*, size_t const)

SHIM_END_DECLS

#endif // ~ SHIM_PRINT_H
