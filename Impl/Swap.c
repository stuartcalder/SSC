/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "Swap.h"

#ifdef SSC_SWAP_DONT_INLINE
uint16_t
SSC_swap16(uint16_t u16)
SSC_SWAP16_IMPL(u16)

uint32_t
SSC_swap32(uint32_t u32)
SSC_SWAP32_IMPL(u32)

uint64_t
SSC_swap64(uint64_t u64)
SSC_SWAP64_IMPL(u64)
#endif
