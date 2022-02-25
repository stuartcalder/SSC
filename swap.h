#ifndef BASE_SWAP_H
#define BASE_SWAP_H

#include <stdlib.h>
#include "errors.h"
#include "macros.h"

#define BASE_SWAP_IMPL_RAW           0
#define BASE_SWAP_IMPL_GNUC          1
#define BASE_SWAP_IMPL_OS_PROVIDED   2

#if !defined(BASE_SWAP_IMPL) && defined(__GNUC__)
# define BASE_SWAP_IMPL BASE_SWAP_IMPL_GNUC
#endif

#if !defined(BASE_SWAP_IMPL) && defined(BASE_OS_MAC)
# ifdef __has_include
#  if __has_include(<libkern/OSByteOrder.h>)
#   include <libkern/OSByteOrder.h>
#   define BASE_SWAP_IMPL BASE_SWAP_IMPL_OS_PROVIDED
#  else
#   define BASE_SWAP_IMPL BASE_SWAP_IMPL_RAW
#  endif
# else /* We don't have __has_include. FIXME: Don't assume we can use OSByteOrder for now.*/
#  define BASE_SWAP_IMPL BASE_SWAP_IMPL_RAW
# endif /* ! __has_include */
#endif

#if (!defined(BASE_SWAP_IMPL) && \
     (defined(__OpenBSD__) || \
      defined(__FreeBSD__) || \
      defined(__Dragonfly__) || \
      defined(__NetBSD__) || \
      defined(__gnu_linux__) || \
      defined(BASE_OS_WINDOWS)))
# define BASE_SWAP_IMPL BASE_SWAP_IMPL_OS_PROVIDED
#endif

#ifndef BASE_SWAP_IMPL
# define BASE_SWAP_IMPL BASE_SWAP_IMPL_RAW
#endif
#if (BASE_SWAP_IMPL == BASE_SWAP_IMPL_RAW)
# define BASE_SWAP_DONT_INLINE
#endif

#if (BASE_SWAP_IMPL == BASE_SWAP_IMPL_GNUC)
# define BASE_SWAP_16_IMPL(u16_v) { return __builtin_bswap16(u16_v); }
# define BASE_SWAP_32_IMPL(u32_v) { return __builtin_bswap32(u32_v); }
# define BASE_SWAP_64_IMPL(u64_v) { return __builtin_bswap64(u64_v); }
#elif (BASE_SWAP_IMPL == BASE_SWAP_IMPL_OS_PROVIDED)
# if defined(__OpenBSD__)
#  include <endian.h>
#  define BASE_SWAP_16_IMPL(u16_v) { return swap16(u16_v); }
#  define BASE_SWAP_32_IMPL(u32_v) { return swap32(u32_v); }
#  define BASE_SWAP_64_IMPL(u64_v) { return swap64(u64_v); }
# elif defined(__FreeBSD__) || defined(__Dragonfly__) || defined(__NetBSD__)
#  ifdef __NetBSD__
#   include <machine/bswap.h>
#  else
#   include <sys/endian.h>
#  endif
#  define BASE_SWAP_16_IMPL(u16_v) { return bswap16(u16_v); }
#  define BASE_SWAP_32_IMPL(u32_v) { return bswap32(u32_v); }
#  define BASE_SWAP_64_IMPL(u64_v) { return bswap64(u64_v); }
# elif defined(__gnu_linux__)
#  include <byteswap.h>
#  define BASE_SWAP_16_IMPL(u16_v) { return bswap_16(u16_v); }
#  define BASE_SWAP_32_IMPL(u32_v) { return bswap_32(u32_v); }
#  define BASE_SWAP_64_IMPL(u64_v) { return bswap_64(u64_v); }
# elif defined(BASE_OS_WINDOWS)
#  include <stdlib.h>
#  define BASE_SWAP_16_IMPL(u16_v) { return _byteswap_ushort(u16_v); }
#  define BASE_SWAP_32_IMPL(u32_v) { return _byteswap_ulong(u32_v); }
#  define BASE_SWAP_64_IMPL(u64_v) { return _byteswap_uint64(u64_v); }
# elif defined(BASE_OS_MAC)
#  if   (BASE_ENDIAN == BASE_ENDIAN_LITTLE)
#   define BASE_SWAP_IMPL_HOST_TO(Var, Bits) (uint##Bits##_t)OSSwapHostToBigInt##Bits(Var)
#  elif (BASE_ENDIAN == BASE_ENDIAN_BIG)
#   define BASE_SWAP_IMPL_HOST_TO(Var, Bits) (uint##Bits##_t)OSSwapHostToLittleInt##Bits(Var)
#  else
#   error "BASE_ENDIAN is invalid!"
#  endif
#  define BASE_SWAP_16_IMPL(u16_v) { return BASE_SWAP_IMPL_HOST_TO(u16_v, 16); }
#  define BASE_SWAP_32_IMPL(u32_v) { return BASE_SWAP_IMPL_HOST_TO(u32_v, 32); }
#  define BASE_SWAP_64_IMPL(u64_v) { return BASE_SWAP_IMPL_HOST_TO(u64_v, 64); }
# else
#  error "BASE_SWAP is OS provided, but OS does not provide a swap impl!"
# endif
#elif (BASE_SWAP_IMPL == BASE_SWAP_IMPL_RAW)
# if (CHAR_BIT != 8)
#  error "We need 8-bit bytes!"
# endif
# define BASE_SWAP_16_IMPL(u16v) { \
  return ( (u16v & UINT16_C(0xff00)) >> 8 | \
           (u16v & UINT16_C(0x00ff)) << 8 ); \
}
# define BASE_SWAP_32_IMPL(u32v) { \
  return ( (u32v & UINT32_C(0xff000000)) >> (8 * 3) | \
           (u32v & UINT32_C(0x00ff0000)) >> (8 * 1) | \
	   (u32v & UINT32_C(0x0000ff00)) << (8 * 1) | \
	   (u32v & UINT32_C(0x000000ff)) << (8 * 3) ); \
}
# define BASE_SWAP_64_IMPL(u64v) { \
  return ( (u64v & UINT64_C(0xff00000000000000)) >> (8 * 7) | \
           (u64v & UINT64_C(0x00ff000000000000)) >> (8 * 5) | \
	   (u64v & UINT64_C(0x0000ff0000000000)) >> (8 * 3) | \
	   (u64v & UINT64_C(0x000000ff00000000)) >> (8 * 1) | \
	   (u64v & UINT64_C(0x00000000ff000000)) << (8 * 1) | \
	   (u64v & UINT64_C(0x0000000000ff0000)) << (8 * 3) | \
	   (u64v & UINT64_C(0x000000000000ff00)) << (8 * 5) | \
	   (u64v & UINT64_C(0x00000000000000ff)) << (8 * 7) ); \
}
#else
# error "Invalid BASE_SWAP_IMPL!"
#endif

#ifdef BASE_SWAP_DONT_INLINE
BASE_API    uint16_t Base_swap_16 (uint16_t);
BASE_API    uint32_t Base_swap_32 (uint32_t);
BASE_API    uint64_t Base_swap_64 (uint64_t);
#else
BASE_INLINE uint16_t Base_swap_16 (uint16_t u16_v) BASE_SWAP_16_IMPL(u16_v)
BASE_INLINE uint32_t Base_swap_32 (uint32_t u32_v) BASE_SWAP_32_IMPL(u32_v)
BASE_INLINE uint64_t Base_swap_64 (uint64_t u64_v) BASE_SWAP_64_IMPL(u64_v)
#endif /* ! ifdef BASE_SWAP_DONT_INLINE */

#endif /* ~ BASE_SWAP_H */
