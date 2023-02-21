/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_SWAP_H
#define BASE_SWAP_H
/* This header implements byte-swapping; reversing the order of bytes in
 * unsigned integers. */

#include "errors.h"
#include "macros.h"
#include <stdlib.h>

/* Which implementation will we use? */
#define BASE_SWAP_IMPL_RAW           0 /* We will implement it ourselves. */
#define BASE_SWAP_IMPL_GNUC          1 /* We will use __builtin_bswap* functions. */
#define BASE_SWAP_IMPL_CPP23         2 /* We will use std::byteswap functions. */
#define BASE_SWAP_IMPL_OS_PROVIDED   3 /* We will use some OS provided byteswap functionality. */

/* Choose GNUC's builtin swap first. */
#if !defined(BASE_SWAP_IMPL) && defined(__GNUC__)
 #define BASE_SWAP_IMPL BASE_SWAP_IMPL_GNUC
#endif
/* If using C++23 or higher, next prefer std::byteswap. */
#if !defined(BASE_SWAP_IMPL) && defined(BASE_LANG_CPP) && (BASE_LANG_CPP >= BASE_CPP_23)
 #define BASE_SWAP_IMPL BASE_SWAP_IMPL_CPP23
#endif
/* On OSX we will use the provided OSByteOrder.h
 * to byteswap, if it is available and if the compiler
 * supports __has_include.
 */
#if !defined(BASE_SWAP_IMPL) && defined(BASE_OS_MAC)
 #ifdef __has_include
  #if __has_include(<libkern/OSByteOrder.h>)
   #include <libkern/OSByteOrder.h>
   #define BASE_SWAP_IMPL BASE_SWAP_IMPL_OS_PROVIDED
  #else /* Otherwise, we will use raw. */
   #define BASE_SWAP_IMPL BASE_SWAP_IMPL_RAW
  #endif
 #else /* We don't have __has_include. Don't assume we can use OSByteOrder.*/
  #define BASE_SWAP_IMPL BASE_SWAP_IMPL_RAW
 #endif /* ! ifdef __has_include */
#endif

/* Next, prefer functions natively available on some operating system. */
#if (!defined(BASE_SWAP_IMPL) &&\
     (defined(__OpenBSD__) ||\
      defined(__FreeBSD__) ||\
      defined(__Dragonfly__) ||\
      defined(__NetBSD__) ||\
      defined(__gnu_linux__) ||\
      defined(BASE_OS_WINDOWS)))
 #define BASE_SWAP_IMPL BASE_SWAP_IMPL_OS_PROVIDED
#endif

/* If no swap implementation has been chosen yet,
 * we will implement the byteswapping ourselves.
 */
#ifndef BASE_SWAP_IMPL
 #define BASE_SWAP_IMPL BASE_SWAP_IMPL_RAW
#endif
#if (BASE_SWAP_IMPL == BASE_SWAP_IMPL_RAW)
 #define BASE_SWAP_DONT_INLINE
#endif

#if (BASE_SWAP_IMPL == BASE_SWAP_IMPL_GNUC)
 #define BASE_SWAP_16_IMPL(VarU16) { return __builtin_bswap16(VarU16); }
 #define BASE_SWAP_32_IMPL(VarU32) { return __builtin_bswap32(VarU32); }
 #define BASE_SWAP_64_IMPL(VarU64) { return __builtin_bswap64(VarU64); }
#elif (BASE_SWAP_IMPL == BASE_SWAP_IMPL_CPP23)
 #include <bit>
 #define BASE_SWAP_16_IMPL(VarU16) { return std::byteswap<uint16_t>(VarU16); }
 #define BASE_SWAP_32_IMPL(VarU32) { return std::byteswap<uint32_t>(VarU32); }
 #define BASE_SWAP_64_IMPL(VarU64) { return std::byteswap<uint64_t>(VarU64); }
#elif (BASE_SWAP_IMPL == BASE_SWAP_IMPL_OS_PROVIDED)
 #if defined(__OpenBSD__)
  #include <endian.h>
  #define BASE_SWAP_16_IMPL(VarU16) { return swap16(VarU16); }
  #define BASE_SWAP_32_IMPL(VarU32) { return swap32(VarU32); }
  #define BASE_SWAP_64_IMPL(VarU64) { return swap64(VarU64); }
 #elif defined(__FreeBSD__) || defined(__Dragonfly__) || defined(__NetBSD__)
  #ifdef __NetBSD__
   #include <machine/bswap.h>
  #else
   #include <sys/endian.h>
  #endif
  #define BASE_SWAP_16_IMPL(VarU16) { return bswap16(VarU16); }
  #define BASE_SWAP_32_IMPL(VarU32) { return bswap32(VarU32); }
  #define BASE_SWAP_64_IMPL(VarU64) { return bswap64(VarU64); }
 #elif defined(__gnu_linux__)
  #include <byteswap.h>
  #define BASE_SWAP_16_IMPL(VarU16) { return bswap_16(VarU16); }
  #define BASE_SWAP_32_IMPL(VarU32) { return bswap_32(VarU32); }
  #define BASE_SWAP_64_IMPL(VarU64) { return bswap_64(VarU64); }
 #elif defined(BASE_OS_WINDOWS)
  #include <stdlib.h>
  #define BASE_SWAP_16_IMPL(VarU16) { return _byteswap_ushort(VarU16); }
  #define BASE_SWAP_32_IMPL(VarU32) { return _byteswap_ulong(VarU32); }
  #define BASE_SWAP_64_IMPL(VarU64) { return _byteswap_uint64(VarU64); }
 #elif defined(BASE_OS_MAC)
   /* We just want to byteswap here. OSX provides functions to convert from host to alternative endians,
    * which is the same as a byteswap when endianness differs.
    */
  #if   (BASE_ENDIAN == BASE_ENDIAN_LITTLE)
   #define BASE_SWAP_IMPL_HOST_TO(Var, Bits) (uint##Bits##_t)OSSwapHostToBigInt##Bits(Var)
  #elif (BASE_ENDIAN == BASE_ENDIAN_BIG)
   #define BASE_SWAP_IMPL_HOST_TO(Var, Bits) (uint##Bits##_t)OSSwapHostToLittleInt##Bits(Var)
  #else
   #error "BASE_ENDIAN is invalid!"
  #endif
  #define BASE_SWAP_16_IMPL(VarU16) { return BASE_SWAP_IMPL_HOST_TO(VarU16, 16); }
  #define BASE_SWAP_32_IMPL(VarU32) { return BASE_SWAP_IMPL_HOST_TO(VarU32, 32); }
  #define BASE_SWAP_64_IMPL(VarU64) { return BASE_SWAP_IMPL_HOST_TO(VarU64, 64); }
 #else
  #error "BASE_SWAP is OS provided, but OS does not provide a swap impl!"
 #endif
#elif (BASE_SWAP_IMPL == BASE_SWAP_IMPL_RAW)
 #if (CHAR_BIT != 8)
  #error "We need 8-bit bytes!"
 #endif
 #define BASE_SWAP_16_IMPL(VarU16) {\
  return (VarU16 >> 8) | (VarU16 << 8);\
 }
 #define BASE_SWAP_32_IMPL(VarU32) {\
  return ( (VarU32                       ) >> (8 * 3) |\
           (VarU32 & UINT32_C(0x00ff0000)) >> (8 * 1) |\
	   (VarU32 & UINT32_C(0x0000ff00)) << (8 * 1) |\
	   (VarU32                       ) << (8 * 3) );\
  }
 #define BASE_SWAP_64_IMPL(VarU64) {\
  return ( (VarU64                               ) >> (8 * 7) |\
           (VarU64 & UINT64_C(0x00ff000000000000)) >> (8 * 5) |\
	   (VarU64 & UINT64_C(0x0000ff0000000000)) >> (8 * 3) |\
	   (VarU64 & UINT64_C(0x000000ff00000000)) >> (8 * 1) |\
	   (VarU64 & UINT64_C(0x00000000ff000000)) << (8 * 1) |\
	   (VarU64 & UINT64_C(0x0000000000ff0000)) << (8 * 3) |\
	   (VarU64 & UINT64_C(0x000000000000ff00)) << (8 * 5) |\
	   (VarU64                               ) << (8 * 7) );\
  }
#else
 #error "Invalid BASE_SWAP_IMPL!"
#endif /* ~ Defining swap implementation. */

#ifdef BASE_SWAP_DONT_INLINE
BASE_API    uint16_t Base_swap_16 (uint16_t);
BASE_API    uint32_t Base_swap_32 (uint32_t);
BASE_API    uint64_t Base_swap_64 (uint64_t);
#else
BASE_INLINE uint16_t Base_swap_16 (uint16_t u16v) BASE_SWAP_16_IMPL(u16v)
BASE_INLINE uint32_t Base_swap_32 (uint32_t u32v) BASE_SWAP_32_IMPL(u32v)
BASE_INLINE uint64_t Base_swap_64 (uint64_t u64v) BASE_SWAP_64_IMPL(u64v)
#endif /* ! ifdef BASE_SWAP_DONT_INLINE */

#endif /* ~ BASE_SWAP_H */
