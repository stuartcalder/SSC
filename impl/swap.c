#include "swap.h"

#ifdef BASE_SWAP_DONT_INLINE
uint16_t Base_swap_16 (uint16_t u16) BASE_SWAP_16_IMPL(u16)
uint32_t Base_swap_32 (uint32_t u32) BASE_SWAP_32_IMPL(u32)
uint64_t Base_swap_64 (uint64_t u64) BASE_SWAP_64_IMPL(u64)
#endif
