#include "operations.h"

#define GENERIC_SHIM_XOR_(bytes) \
	void SHIM_PUBLIC \
	shim_xor_##bytes (void * SHIM_RESTRICT mem, void const * SHIM_RESTRICT add) { \
		for( int i = 0; i < bytes; ++i ) { \
			((uint8_t *)      mem)[ i ] ^= \
			((uint8_t const *)add)[ i ]; \
		} \
	}

GENERIC_SHIM_XOR_ (16)
GENERIC_SHIM_XOR_ (32)
GENERIC_SHIM_XOR_ (64)
GENERIC_SHIM_XOR_ (128)

void SHIM_PUBLIC * 
shim_checked_malloc (size_t size) {
	void * mem = malloc( size );
	if( !mem )
		SHIM_ERRX (SHIM_ERR_STR_ALLOC_FAILURE);
	return mem;
}

#ifndef SHIM_OPERATIONS_INLINE_OBTAIN_OS_ENTROPY
void SHIM_PUBLIC
shim_obtain_os_entropy (uint8_t * SHIM_RESTRICT mem, size_t size)
	SHIM_OPERATIONS_OBTAIN_OS_ENTROPY_IMPL (mem, size)
#endif

#ifdef SHIM_OPERATIONS_NO_INLINE_SWAP_FUNCTIONS
uint16_t SHIM_PUBLIC
shim_swap_16 (uint16_t u16)
	SHIM_OPERATIONS_SWAP_16_IMPL (u16)
uint32_t SHIM_PUBLIC
shim_swap_32 (uint32_t u32)
	SHIM_OPERATIONS_SWAP_32_IMPL (u32)
uint64_t SHIM_PUBLIC
shim_swap_64 (uint64_t u64)
	SHIM_OPERATIONS_SWAP_64_IMPL (u64)
#endif

int SHIM_PUBLIC
shim_ctime_memcmp (void const * SHIM_RESTRICT mem_0,
		   void const * SHIM_RESTRICT mem_1,
		   size_t const               size)
{
#define ONE_ UINT8_C (0x01)
	int unequal_count = 0;
	for( size_t i = 0; i < size; ++i ) {
		uint8_t const b = ((uint8_t const *)mem_0)[ i ] ^
				  ((uint8_t const *)mem_1)[ i ];
		unequal_count += (
				((b >> 7)       ) |
				((b >> 6) & ONE_) |
				((b >> 5) & ONE_) |
				((b >> 4) & ONE_) |
				((b >> 3) & ONE_) |
				((b >> 2) & ONE_) |
				((b >> 1) & ONE_) |
				((b     ) & ONE_)
				);
	}
	return unequal_count;
}
