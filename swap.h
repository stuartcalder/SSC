#ifndef BASE_SWAP_H
#define BASE_SWAP_H

#include <stdlib.h>
#include "errors.h"
#include "macros.h"

#if    defined(__OpenBSD__)
#	include <endian.h>
#elif  defined(__FreeBSD__) || defined(__Dragonfly__)
#	include <sys/endian.h>
#elif  defined(__NetBSD__)
#	include <machine/bswap.h>
#elif  defined(__gnu_linux__)
#   if 0
#	include <byteswap.h>
#   endif
#elif  defined(BASE_OS_MAC)
#	define BASE_SWAP_DONT_INLINE
#	define BASE_SWAP_NO_NATIVE_FUNCTIONS
#elif  defined(BASE_OS_WINDOWS)
#	include <stdlib.h>
#endif

#if    defined(__OpenBSD__)
#	define BASE_SWAP_16_IMPL(u16) { return swap16(u16); }
#	define BASE_SWAP_32_IMPL(u32) { return swap32(u32); }
#	define BASE_SWAP_64_IMPL(u64) { return swap64(u64); }
#elif  defined(__FreeBSD__) || defined(__NetBSD__) || defined(__Dragonfly__)
#	define BASE_SWAP_16_IMPL(u16) { return bswap16(u16); }
#	define BASE_SWAP_32_IMPL(u32) { return bswap32(u32); }
#	define BASE_SWAP_64_IMPL(u64) { return bswap64(u64); }
#elif  defined(__gnu_linux__)
#	define BASE_SWAP_16_IMPL(u16) { return __builtin_bswap16(u16); }
#	define BASE_SWAP_32_IMPL(u32) { return __builtin_bswap32(u32); }
#	define BASE_SWAP_64_IMPL(u64) { return __builtin_bswap64(u64); }
#elif  defined(BASE_OS_WINDOWS)
#	define BASE_SWAP_16_IMPL(u16) { return _byteswap_ushort(u16); }
#	define BASE_SWAP_32_IMPL(u32) { return _byteswap_ulong(u32); }
#	define BASE_SWAP_64_IMPL(u64) { return _byteswap_uint64(u64); }
#elif  defined(BASE_SWAP_NO_NATIVE_FUNCTIONS)
#	define BASE_SWAP_16_IMPL(u16) { \
		return ( (u16 & UINT16_C(0xff00)) >> 8 | \
                         (u16 & UINT16_C(0x00ff)) << 8 ); \
	}
#	define BASE_SWAP_32_IMPL(u32) { \
		return ( (u32 & UINT32_C(0xff000000)) >> (8 * 3) | \
		         (u32 & UINT32_C(0x00ff0000)) >> (8 * 1) | \
			 (u32 & UINT32_C(0x0000ff00)) << (8 * 1) | \
			 (u32 & UINT32_C(0x000000ff)) << (8 * 3) ); \
	}
#	define BASE_SWAP_64_IMPL(u64) { \
		return ( (u64 & UINT64_C(0xff00000000000000)) >> (8 * 7) | \
		         (u64 & UINT64_C(0x00ff000000000000)) >> (8 * 5) | \
			 (u64 & UINT64_C(0x0000ff0000000000)) >> (8 * 3) | \
			 (u64 & UINT64_C(0x000000ff00000000)) >> (8 * 1) | \
			 (u64 & UINT64_C(0x00000000ff000000)) << (8 * 1) | \
			 (u64 & UINT64_C(0x0000000000ff0000)) << (8 * 3) | \
			 (u64 & UINT64_C(0x000000000000ff00)) << (8 * 5) | \
			 (u64 & UINT64_C(0x00000000000000ff)) << (8 * 7) ); \
	}
#else
#	error "Unsupported."
#endif

#ifdef BASE_SWAP_DONT_INLINE
BASE_API    uint16_t Base_swap_16 (uint16_t);
BASE_API    uint32_t Base_swap_32 (uint32_t);
BASE_API    uint64_t Base_swap_64 (uint64_t);
#else
BASE_INLINE uint16_t Base_swap_16 (uint16_t u16) BASE_SWAP_16_IMPL(u16)
BASE_INLINE uint32_t Base_swap_32 (uint32_t u32) BASE_SWAP_32_IMPL(u32)
BASE_INLINE uint64_t Base_swap_64 (uint64_t u64) BASE_SWAP_64_IMPL(u64)
#endif

#endif /* ~ BASE_SWAP_H */
