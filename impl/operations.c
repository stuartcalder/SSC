#include "errors.h"
#include "operations.h"
#include <stdlib.h>

#define AS_U8_PTR_(v)   ((uint8_t *)v)
#define AS_U8C_PTR_(cv) ((uint8_t const *)cv)

#define XOR_(v_ptr, vc_ptr, i) \
	AS_U8_PTR_(v_ptr)[i] ^= AS_U8C_PTR_(vc_ptr)[i]

#define XOR_8_(v_ptr, vc_ptr, i) \
	XOR_(v_ptr, vc_ptr, (i + 0)); \
	XOR_(v_ptr, vc_ptr, (i + 1)); \
	XOR_(v_ptr, vc_ptr, (i + 2)); \
	XOR_(v_ptr, vc_ptr, (i + 3)); \
	XOR_(v_ptr, vc_ptr, (i + 4)); \
	XOR_(v_ptr, vc_ptr, (i + 5)); \
	XOR_(v_ptr, vc_ptr, (i + 6)); \
	XOR_(v_ptr, vc_ptr, (i + 7))

#define XOR_16_(v_ptr, vc_ptr, i) \
	XOR_8_(v_ptr, vc_ptr, (i + 0)); \
	XOR_8_(v_ptr, vc_ptr, (i + 8))

#define XOR_32_(v_ptr, vc_ptr, i) \
	XOR_16_(v_ptr, vc_ptr, (i +  0)); \
	XOR_16_(v_ptr, vc_ptr, (i + 16))

#define XOR_64_(v_ptr, vc_ptr, i) \
	XOR_32_(v_ptr, vc_ptr, (i +  0)); \
	XOR_32_(v_ptr, vc_ptr, (i + 32))

#define XOR_128_(v_ptr, vc_ptr, i) \
	XOR_64_(v_ptr, vc_ptr, (i +  0)); \
	XOR_64_(v_ptr, vc_ptr, (i + 64))

void
shim_xor_16 (void * SHIM_RESTRICT base, void const * SHIM_RESTRICT add) {
	XOR_16_(base, add, 0);
}

void
shim_xor_32 (void * SHIM_RESTRICT base, void const * SHIM_RESTRICT add) {
	XOR_32_(base, add, 0);
}

void
shim_xor_64 (void * SHIM_RESTRICT base, void const * SHIM_RESTRICT add) {
	XOR_64_(base, add, 0);
}

void
shim_xor_128 (void * SHIM_RESTRICT base, void const * SHIM_RESTRICT add) {
	XOR_128_(base, add, 0);
}

void *
shim_enforce_malloc (size_t num_bytes) {
	void * mem = malloc(num_bytes);
	if (!mem)
		shim_errx(SHIM_ERR_STR_ALLOC_FAILURE);
	return mem;
}

void *
shim_enforce_calloc (size_t num_elements, size_t element_size) {
	void * mem = calloc(num_elements, element_size);
	if (!mem)
		shim_errx(SHIM_ERR_STR_ALLOC_FAILURE);
	return mem;
}

void *
shim_enforce_realloc (void * SHIM_RESTRICT ptr, size_t size) {
	void * mem = realloc(ptr, size);
	if (!mem)
		shim_errx(SHIM_ERR_STR_ALLOC_FAILURE);
	return mem;
}

#ifndef SHIM_OPERATIONS_INLINE_OBTAIN_OS_ENTROPY
void
shim_obtain_os_entropy (uint8_t * SHIM_RESTRICT mem, size_t size)
	SHIM_OPERATIONS_OBTAIN_OS_ENTROPY_IMPL(mem, size)
#endif /* ~ ifndef SHIM_OPERATIONS_INLINE_OBTAIN_OS_ENTROPY */

#ifdef SHIM_OPERATIONS_NO_INLINE_SWAP_FUNCTIONS
uint16_t
shim_swap_16 (uint16_t u16)
	SHIM_OPERATIONS_SWAP_16_IMPL(u16)
uint32_t
shim_swap_32 (uint32_t u32)
	SHIM_OPERATIONS_SWAP_32_IMPL(u32)
uint64_t
shim_swap_64 (uint64_t u64)
	SHIM_OPERATIONS_SWAP_64_IMPL(u64)
#endif /* ~ ifdef SHIM_OPERATIONS_NO_INLINE_SWAP_FUNCTIONS */

ssize_t
shim_ctime_memdiff (void const * SHIM_RESTRICT mem_0,
		    void const * SHIM_RESTRICT mem_1,
		    ssize_t const              size)
{
	SHIM_ASSERT(mem_0 && mem_1);
	ssize_t unequal_count = 0;
	for (ssize_t i = 0; i < size; ++i) {
		uint8_t b = AS_U8C_PTR_(mem_0)[i] ^ AS_U8C_PTR_(mem_1)[i];
		b |= ((b >> 7)|
		      (b >> 6)|
		      (b >> 5)|
		      (b >> 4)|
		      (b >> 3)|
		      (b >> 2)|
		      (b >> 1));
		unequal_count += (b & 0x01);

	}
	return unequal_count;
}

bool
shim_iszero (void const * SHIM_RESTRICT mem,
             ssize_t const              num_bytes)
{
	SHIM_ASSERT(mem);
	for (ssize_t i = 0; i < num_bytes; ++i) {
		if (AS_U8C_PTR_(mem)[i])
			return false;
	}
	return true;
}

bool
shim_ctime_iszero (void const * SHIM_RESTRICT mem,
		   ssize_t const              num_bytes)
{
	SHIM_ASSERT(mem);
	uint8_t zero_test = 0;
	for (ssize_t i = 0; i < num_bytes; ++i) {
		zero_test |= ((uint8_t const *)mem)[i];
	}
	return !zero_test;
}
