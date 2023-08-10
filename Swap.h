/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_SWAP_H
#define SSC_SWAP_H
/* This header implements byte-swapping; reversing the order of bytes in
 * unsigned integers. */

#include "Error.h"
#include "Macro.h"
#include <stdlib.h>

/* Which implementation will we use? */
#define SSC_SWAP_IMPL_RAW           0 /* We will implement it ourselves. */
#define SSC_SWAP_IMPL_GNUC          1 /* We will use __builtin_bswap* functions. */
#define SSC_SWAP_IMPL_CPP23         2 /* We will use std::byteswap functions. */
#define SSC_SWAP_IMPL_OS_PROVIDED   3 /* We will use some OS provided byteswap functionality. */

/* Choose GNUC's builtin swap first. */
#if !defined(SSC_SWAP_IMPL) && defined(__GNUC__)
 #define SSC_SWAP_IMPL SSC_SWAP_IMPL_GNUC
#endif
/* If using C++23 or higher, next prefer std::byteswap. */
#if !defined(SSC_SWAP_IMPL) && defined(SSC_LANG_CPP) && (SSC_LANG_CPP >= SSC_CPP_23)
 #define SSC_SWAP_IMPL SSC_SWAP_IMPL_CPP23
#endif
/* On OSX we will use the provided OSByteOrder.h
 * to byteswap, if it is available and if the compiler
 * supports __has_include. */
#if !defined(SSC_SWAP_IMPL) && defined(SSC_OS_MAC)
 #ifdef __has_include
  #if __has_include(<libkern/OSByteOrder.h>)
   #include <libkern/OSByteOrder.h>
   #define SSC_SWAP_IMPL SSC_SWAP_IMPL_OS_PROVIDED
  #else /* Otherwise, we will use raw. */
   #define SSC_SWAP_IMPL SSC_SWAP_IMPL_RAW
  #endif
 #else /* We don't have __has_include. Don't assume we can use OSByteOrder.*/
  #define SSC_SWAP_IMPL SSC_SWAP_IMPL_RAW
 #endif /* ! ifdef __has_include */
#endif

/* Next, prefer functions natively available on some operating system. */
#if (!defined(SSC_SWAP_IMPL) &&\
     (defined(__OpenBSD__) ||\
      defined(__FreeBSD__) ||\
      defined(__Dragonfly__) ||\
      defined(__NetBSD__) ||\
      defined(__gnu_linux__) ||\
      defined(SSC_OS_WINDOWS)))
 #define SSC_SWAP_IMPL SSC_SWAP_IMPL_OS_PROVIDED
#endif

/* If no swap implementation has been chosen yet,
 * we will implement the byteswapping ourselves. */
#ifndef SSC_SWAP_IMPL
 #define SSC_SWAP_IMPL SSC_SWAP_IMPL_RAW
#endif
#if SSC_SWAP_IMPL == SSC_SWAP_IMPL_RAW
 #define SSC_SWAP_DONT_INLINE
#endif

#if SSC_SWAP_IMPL == SSC_SWAP_IMPL_GNUC
 #define SSC_SWAP16_IMPL(VarU16) { return __builtin_bswap16(VarU16); }
 #define SSC_SWAP32_IMPL(VarU32) { return __builtin_bswap32(VarU32); }
 #define SSC_SWAP64_IMPL(VarU64) { return __builtin_bswap64(VarU64); }
#elif SSC_SWAP_IMPL == SSC_SWAP_IMPL_CPP23
 #include <bit>
 #define SSC_SWAP16_IMPL(VarU16) { return std::byteswap<uint16_t>(VarU16); }
 #define SSC_SWAP32_IMPL(VarU32) { return std::byteswap<uint32_t>(VarU32); }
 #define SSC_SWAP64_IMPL(VarU64) { return std::byteswap<uint64_t>(VarU64); }
#elif SSC_SWAP_IMPL == SSC_SWAP_IMPL_OS_PROVIDED
 #if defined(__OpenBSD__)
  #include <endian.h>
  #define SSC_SWAP16_IMPL(VarU16) { return swap16(VarU16); }
  #define SSC_SWAP32_IMPL(VarU32) { return swap32(VarU32); }
  #define SSC_SWAP64_IMPL(VarU64) { return swap64(VarU64); }
 #elif defined(__FreeBSD__) || defined(__Dragonfly__) || defined(__NetBSD__)
  #ifdef __NetBSD__
   #include <machine/bswap.h>
  #else
   #include <sys/endian.h>
  #endif
  #define SSC_SWAP16_IMPL(VarU16) { return bswap16(VarU16); }
  #define SSC_SWAP32_IMPL(VarU32) { return bswap32(VarU32); }
  #define SSC_SWAP64_IMPL(VarU64) { return bswap64(VarU64); }
 #elif defined(__gnu_linux__)
  #include <byteswap.h>
  #define SSC_SWAP16_IMPL(VarU16) { return bswap_16(VarU16); }
  #define SSC_SWAP32_IMPL(VarU32) { return bswap_32(VarU32); }
  #define SSC_SWAP64_IMPL(VarU64) { return bswap_64(VarU64); }
 #elif defined(SSC_OS_WINDOWS)
  #include <stdlib.h>
  #define SSC_SWAP16_IMPL(VarU16) { return _byteswap_ushort(VarU16); }
  #define SSC_SWAP32_IMPL(VarU32) { return _byteswap_ulong(VarU32); }
  #define SSC_SWAP64_IMPL(VarU64) { return _byteswap_uint64(VarU64); }
 #elif defined(SSC_OS_MAC)
  /* We just want to byteswap here. OSX provides functions to convert from host to alternative endians,
   * which is the same as a byteswap when endianness differs. */
  #if   SSC_ENDIAN == SSC_ENDIAN_LITTLE
   #define SSC_SWAP_IMPL_HOST_TO(Var, Bits) (uint##Bits##_t)OSSwapHostToBigInt##Bits(Var)
  #elif SSC_ENDIAN == SSC_ENDIAN_BIG
   #define SSC_SWAP_IMPL_HOST_TO(Var, Bits) (uint##Bits##_t)OSSwapHostToLittleInt##Bits(Var)
  #else
   #error "SSC_ENDIAN is invalid!"
  #endif
  #define SSC_SWAP16_IMPL(VarU16) { return SSC_SWAP_IMPL_HOST_TO(VarU16, 16); }
  #define SSC_SWAP32_IMPL(VarU32) { return SSC_SWAP_IMPL_HOST_TO(VarU32, 32); }
  #define SSC_SWAP64_IMPL(VarU64) { return SSC_SWAP_IMPL_HOST_TO(VarU64, 64); }
 #else
  #error "SSC_SWAP is OS provided, but OS does not provide a swap impl!"
 #endif
#elif SSC_SWAP_IMPL == SSC_SWAP_IMPL_RAW
 #if CHAR_BIT != 8
  #error "We need 8-bit bytes!"
 #endif
 #define SSC_SWAP16_IMPL(VarU16) {\
  return (VarU16 >> 8) | (VarU16 << 8);\
 }
 #define SSC_SWAP32_IMPL(VarU32) {\
  return ( (VarU32                       ) >> (8 * 3) |\
           (VarU32 & UINT32_C(0x00ff0000)) >> (8 * 1) |\
           (VarU32 & UINT32_C(0x0000ff00)) << (8 * 1) |\
           (VarU32                       ) << (8 * 3) );\
  }
 #define SSC_SWAP64_IMPL(VarU64) {\
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
 #error "Invalid SSC_SWAP_IMPL!"
#endif /* ~ Defining swap implementation. */

#ifdef SSC_SWAP_DONT_INLINE
 #define API_         SSC_API
 #define IMPL16_(U16) ;
 #define IMPL32_(U32) ;
 #define IMPL64_(U64) ;
#else
 #define API_         SSC_INLINE
 #define IMPL16_(U16) SSC_SWAP16_IMPL(U16)
 #define IMPL32_(U32) SSC_SWAP32_IMPL(U32)
 #define IMPL64_(U64) SSC_SWAP64_IMPL(U64)
#endif

SSC_BEGIN_C_DECLS

API_ uint16_t
SSC_swap16(uint16_t u16)
IMPL16_(u16)

API_ uint32_t
SSC_swap32(uint32_t u32)
IMPL32_(u32)

API_ uint64_t
SSC_swap64(uint64_t u64)
IMPL64_(u64)

SSC_END_C_DECLS
#undef API_
#undef IMPL16_
#undef IMPL32_
#undef IMPL64_

#endif /* ~ SSC_SWAP_H */
