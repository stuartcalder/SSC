#include "operations.h"

#define R_(ptr) ptr BASE_RESTRICT

#define XOR_(v, cv, i)	((uint8_t*)v)[i] ^= ((const uint8_t*)cv)[i]

#define XOR_8_(v, cv, i) \
	XOR_(v, cv, (i + 0)); \
	XOR_(v, cv, (i + 1)); \
	XOR_(v, cv, (i + 2)); \
	XOR_(v, cv, (i + 3)); \
	XOR_(v, cv, (i + 4)); \
	XOR_(v, cv, (i + 5)); \
	XOR_(v, cv, (i + 6)); \
	XOR_(v, cv, (i + 7))
#define  XOR_16_(v, cv, i)  XOR_8_(v, cv, (i + 0));  XOR_8_(v, cv, (i +  8))
#define  XOR_32_(v, cv, i) XOR_16_(v, cv, (i + 0)); XOR_16_(v, cv, (i + 16))
#define  XOR_64_(v, cv, i) XOR_32_(v, cv, (i + 0)); XOR_32_(v, cv, (i + 32))
#define XOR_128_(v, cv, i) XOR_64_(v, cv, (i + 0)); XOR_64_(v, cv, (i + 64))

void  Base_xor_16  (R_(void*) base, R_(const void*) add) {  XOR_16_(base, add, 0); }
void  Base_xor_32  (R_(void*) base, R_(const void*) add) {  XOR_32_(base, add, 0); }
void  Base_xor_64  (R_(void*) base, R_(const void*) add) {  XOR_64_(base, add, 0); }
void  Base_xor_128 (R_(void*) base, R_(const void*) add) { XOR_128_(base, add, 0); }
void* Base_malloc_or_die (size_t num_bytes) {
	void* mem = malloc(num_bytes);
	Base_assert_msg(mem, BASE_ERR_STR_ALLOC_FAILURE);
	return mem;
}
void* Base_calloc_or_die (size_t n_elem, size_t elem_size) {
	void* mem = calloc(n_elem, elem_size);
	Base_assert_msg(mem, BASE_ERR_STR_ALLOC_FAILURE);
	return mem;
}
void* Base_realloc_or_die (R_(void*) ptr, size_t size) {
	void* mem = realloc(ptr, size);
	Base_assert_msg(mem, BASE_ERR_STR_ALLOC_FAILURE);
	return mem;
}

size_t Base_ctime_memdiff (R_(const void*) m_0, R_(const void*) m_1, size_t size) {
	BASE_ASSERT(m_0 && m_1);
	size_t unequal_count = 0;
	for (size_t i = 0; i < size; ++i) {
		uint8_t b = ((const uint8_t*)m_0)[i] ^ ((const uint8_t*)m_1)[i];
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
bool Base_is_zero (R_(const void*) mem, size_t n_bytes) {
	BASE_ASSERT(mem);
	for (size_t i = 0; i < n_bytes; ++i)
		if (((const uint8_t*)mem)[i])
			return false;
	return true;
}
bool Base_ctime_is_zero (R_(const void*) mem, size_t n_bytes) {
	BASE_ASSERT(mem);
	uint8_t zero_test = 0;
	for (size_t i = 0; i < n_bytes; ++i)
		zero_test |= ((const uint8_t*)mem)[i];
	return !zero_test;
}
