/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/* Endianness. */
#define BASE_ENDIAN_NONE   0 /* Endianness unestablished. */
#define BASE_ENDIAN_LITTLE 1 /* Least-Significant-Byte-First Ordering. */
#define BASE_ENDIAN_BIG    2 /* Most-Significant-Byte-First Ordering. */
#define BASE_ENDIAN_DEFAULT BASE_ENDIAN_LITTLE /* For now just assume little endian if unspecified. */
#define BASE_ENDIAN_ISVALID(Endian) ((Endian) == BASE_ENDIAN_LITTLE || (Endian) == BASE_ENDIAN_BIG) /* NONE is invalid. */
/* How are we determining endianness? */
#define BASE_ENDIAN_SRC_NONE     0 /* Endian source unestablished. */
#define BASE_ENDIAN_SRC_EXTERN   1 /* Endianness was specified externally, in BASE_EXTERN_ENDIAN. */
#define BASE_ENDIAN_SRC_ISA      2 /* Endianness was determined by the ISA of the CPU. */
#define BASE_ENDIAN_SRC_COMPILER 3 /* Endianness was specified by the compiler. */
#define BASE_ENDIAN_SRC_DEFAULT  4 /* Endianness was set to defaults. */
#define BASE_ENDIAN_SRC_ISVALID(EndSrc) ((EndSrc) >= BASE_ENDIAN_SRC_EXTERN && (EndSrc) <= BASE_ENDIAN_SRC_DEFAULT) /* NONE is invalid. */
/* What is the compiler? */
#define BASE_COMPILER_UNKNOWN 0 /* Unrecognized compiler. */
#define BASE_COMPILER_GCC     1 /* GNU C/C++ compiler. */
#define BASE_COMPILER_CLANG   2 /* Clang C/C++ compiler. */
#define BASE_COMPILER_MSVC    3 /* Microsoft Visual C/C++ compiler. */
#define BASE_COMPILER_ISVALID(Comp) ((Comp) >= BASE_COMPILER_UNKNOWN && (Comp) <= BASE_COMPILER_MSVC) /* UNKNOWN is valid. */
/* Pointer-aliasing restrict support bits. */
#define BASE_RESTRICT_IMPL_C    0x01 /* C99 restrict. */
#define BASE_RESTRICT_IMPL_CPP  0x02 /* C++/MSVC restrict. */

/* Which compiler are we using? */
#if   defined(__clang__)
 #define BASE_COMPILER   BASE_COMPILER_CLANG
#elif defined(_MSC_VER)
 #define BASE_COMPILER   BASE_COMPILER_MSVC
 #define BASE_COMPILER_V _MSC_VER
#elif defined(__GNUC__)
 #define BASE_COMPILER   BASE_COMPILER_GCC
#else
 #define BASE_COMPILER BASE_COMPILER_UNKNOWN
 #warning "BASE_COMPILER unknown!"
#endif

/* Is our compiler a gcc-compatible compiler? */
#define BASE_COMPILER_IS_GCC_COMPATIBLE ((BASE_COMPILER == BASE_COMPILER_GCC) || (BASE_COMPILER == BASE_COMPILER_CLANG))

/* Operating system macros */
#if defined(__APPLE__) && defined(__MACH__)
 #define BASE_OS_MAC
#endif /* ! #if defined (__APPLE__) and defined (__MACH__) */

/* More unixlikes could go here without too much code change, probably. */
#if (defined(BASE_OS_MAC)   ||\
     defined(__Dragonfly__) ||\
     defined(__FreeBSD__)   ||\
     defined(__gnu_linux__) ||\
     defined(__NetBSD__)    ||\
     defined(__OpenBSD__))
 #define BASE_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif defined(_WIN32) || defined(__CYGWIN__)
 #define BASE_OS_WINDOWS
 #ifdef _WIN64
  #define BASE_OS_WIN64
 #else
  #define BASE_OS_WIN32
 #endif /* ! #ifdef _WIN64 */
#else
 #error "Unsupported."
#endif /* ! #if defined (unixlike os's ...) */

#define BASE_2023 202312L /* Not released yet. */
/* C language standards. */
#define BASE_C_89 199409L
#define BASE_C_99 199901L
#define BASE_C_11 201112L
#define BASE_C_17 201710L
#define BASE_C_23 BASE_2023
/* C++ language standards. */
#define BASE_CPP_11 201103L
#define BASE_CPP_14 201402L
#define BASE_CPP_17 201703L
#define BASE_CPP_20 202002L
#define BASE_CPP_23 202302L

/* BASE_ENDIAN
 * int
 *   Defines the native byte order.
 *   May be defined outside by the BASE_EXTERN_ENDIAN macro.
 *   Must equal to BASE_ENDIAN_LITTLE, or BASE_ENDIAN_BIG. */
#if !defined(BASE_ENDIAN) && defined(BASE_EXTERN_ENDIAN)
 #if !BASE_ENDIAN_ISVALID(BASE_EXTERN_ENDIAN)
  #error "BASE_EXTERN_ENDIAN is an invalid endianness!"
 #endif
  /* External definition trumps all endian detection methods. */
 #define BASE_ENDIAN     BASE_EXTERN_ENDIAN
 #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_EXTERN
#endif

/* GCC/Clang provide __BYTE_ORDER__ for us to check byte endianness directly. Use this when possible. */
#if (!defined(BASE_ENDIAN) && defined(__GNUC__) && defined(__BYTE_ORDER__) &&\
     defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))
 #if   (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  #define BASE_ENDIAN BASE_ENDIAN_LITTLE
 #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  #define BASE_ENDIAN BASE_ENDIAN_BIG
 #else
  #error "Invalid __BYTE_ORDER__!"
 #endif
 #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_COMPILER
#endif

/* BASE_ISA
 * int
 *   The instruction set architecture of the system.
 *   If not explicitly defined in this file,
 *   BASE_ISA will default to BASE_ISA_UNKNOWN. */
#define BASE_ISA_UNKNOWN 0
#define BASE_ISA_AMD64   1
#define BASE_ISA_RISCV   2
#define BASE_ISA_ARM64   3
#define BASE_ISA_X86     4
#define BASE_ISA_ARMV7   5
#define BASE_ISA_ISVALID(Isa) ((Isa) >= BASE_ISA_UNKNOWN && (Isa) <= BASE_ISA_ARMV7) /* UNKNOWN arch is valid. */

/* Architecture macros. */
#if (defined(__amd64)  || defined(__amd64__)  ||\
     defined(__x86_64) || defined(__x86_64__) ||\
     defined(_M_X64)   || defined(_M_AMD64))
 #define BASE_ISA BASE_ISA_AMD64
 #ifndef BASE_ENDIAN
   /* AMD64 is little endian. */
  #define BASE_ENDIAN     BASE_ENDIAN_LITTLE
  #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_ISA
 #endif
#elif defined(__riscv)
 #define BASE_ISA BASE_ISA_RISCV
 #ifndef BASE_ENDIAN
   /* RISCV is little endian. */
  #define BASE_ENDIAN     BASE_ENDIAN_LITTLE
  #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_ISA
 #endif
#elif (defined(__aarch64__) || defined(_M_ARM64))
 #define BASE_ISA BASE_ISA_ARM64
 #ifndef BASE_ENDIAN
  #if (BASE_COMPILER == BASE_COMPILER_MSVC) /* Assume little-endian mode when used with MSVC. */
   #define BASE_ENDIAN     BASE_ENDIAN_LITTLE
   #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_ISA
  #else
   #if   (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_LITTLE)
    #warning "Aarch64 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Little)."
   #elif (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_BIG)
    #warning "Aarch64 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Big)."
   #else
    #error "BASE_ENDIAN_DEFAULT is invalid!"
   #endif
   #define BASE_ENDIAN     BASE_ENDIAN_DEFAULT
   #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_DEFAULT
  #endif /* ! if (BASE_COMPILER == BASE_COMPILER_MSVC) */
 #endif /* ! ifndef BASE_ENDIAN */
#elif (defined(__i386__) || defined(_M_IX86))
 #define BASE_ISA BASE_ISA_X86
 #ifndef BASE_ENDIAN
   /* X86 is little endian. */
  #define BASE_ENDIAN     BASE_ENDIAN_LITTLE
  #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_ISA
 #endif /* ! ifndef BASE_ENDIAN */
#elif defined(__arm__) || defined(_M_ARM)
 #define BASE_ISA BASE_ISA_ARMV7
 #ifndef BASE_ENDIAN
  #if (BASE_COMPILER == BASE_COMPILER_MSVC) /* Assume little-endian mode when used with MSVC. */
   #define BASE_ENDIAN     BASE_ENDIAN_LITTLE
   #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_COMPILER
  #else
   #if   (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_LITTLE)
    #warning "Armv7 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Little)."
   #elif (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_BIG)
    #warning "Armv7 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Big)."
   #else
    #error "BASE_ENDIAN is invalid!"
   #endif
   #define BASE_ENDIAN     BASE_ENDIAN_DEFAULT
   #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_DEFAULT
  #endif /* ! ifdef BASE_COMPILER_MSVC */
 #endif /* ! ifndef BASE_ENDIAN */
#else
 #warning "Failed to detect ISA."
 #define BASE_ISA BASE_ISA_UNKNOWN
 #ifndef BASE_ENDIAN
  #if   (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_LITTLE)
   #warning "BASE_ISA is UNKNOWN. Will use default (little) endianness and hope for the best."
  #elif (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_BIG)
   #warning "BASE_ISA is UNKNOWN. Will use default (big) endianness and hope for the best."
  #else
   #error "BASE_ENDIAN_DEFAULT is invalid!"
  #endif
  #warning "To specify endianness, define BASE_EXTERN_ENDIAN to BASE_ENDIAN_LITTLE or BASE_ENDIAN_BIG!"
  #define BASE_ENDIAN     BASE_ENDIAN_DEFAULT
  #define BASE_ENDIAN_SRC BASE_ENDIAN_SRC_DEFAULT
 #endif
#endif

/* Sanity assertions. */
#if   !defined(BASE_COMPILER)
 #error "BASE_COMPILER is not defined!"
#elif !BASE_COMPILER_ISVALID(BASE_COMPILER)
 #error "BASE_COMPILER is an invalid compiler!"
#elif !defined(BASE_ENDIAN)
 #error "BASE_ENDIAN is not defined!"
#elif !BASE_ENDIAN_ISVALID(BASE_ENDIAN)
 #error "BASE_ENDIAN is an invalid endianness!"
#elif !defined(BASE_ENDIAN_SRC)
 #error "BASE_ENDIAN_SRC is not defined!"
#elif !BASE_ENDIAN_SRC_ISVALID(BASE_ENDIAN_SRC)
 #error "BASE_ENDIAN_SRC is an invalid endian source!"
#elif !defined(BASE_ISA)
 #error "BASE_ISA is not defined!"
#elif !BASE_ISA_ISVALID(BASE_ISA)
 #error "BASE_ISA is an invalid ISA!"
#endif

/* C/C++ Interoperability Macros */
#ifdef __cplusplus
 #define BASE_LANG_CPP __cplusplus
 #define BASE_LANG     BASE_LANG_CPP
 #define BASE_NULL     nullptr
/* C++ doesn't support "restrict". Use the non-standard "__restrict" compiler extension. */
 #ifndef BASE_RESTRICT_IMPL
  #define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_CPP
 #endif
/* Use C function name mangling. */
 #define BASE_BEGIN_C_DECLS extern "C" {
 #define BASE_END_C_DECLS   }
/* If we're using C++11 or above, support "static_assert", "alignas", and "alignof". */
 #if (BASE_LANG_CPP < BASE_CPP_11)
  #error "When compiled as C++, Base is C++11 or higher only!"
 #endif
 #define BASE_STATIC_ASSERT(Boolean, Message) static_assert(Boolean, Message)
 #define BASE_ALIGNAS(V)                      alignas(V)
 #define BASE_ALIGNOF(V)                      alignof(V)
 #if (BASE_LANG_CPP >= BASE_CPP_17)
  #define BASE_STATIC_ASSERT_1(Boolean)       static_assert(Boolean)
 #else
  #define BASE_STATIC_ASSERT_1(Boolean_)
  #define BASE_STATIC_ASSERT_1_IS_NIL
 #endif
  /* consteval is similar to, but distinct from, constexpr.
   * Be aware of that when using BASE_CONSTEVAL or BASE_CONSTEXPR in the first place. */
 #if (BASE_LANG_CPP >= BASE_CPP_20)
  #define BASE_CONSTEVAL consteval
 #else
  #define BASE_CONSTEVAL constexpr
  #define BASE_CONSTEVAL_IS_CONSTEXPR /* We have constexpr but not consteval. Assume constexpr is a `downgrade` from consteval.*/
 #endif
 #define BASE_COMPOUND_LITERAL(Type, ...) Type{__VA_ARGS__} /* C++ literal syntax. */
 #define BASE_CONSTEXPR constexpr
#else /* Not C++. We are using C. */
 #define BASE_LANG BASE_LANG_C
 #define BASE_NULL NULL
 #define BASE_BEGIN_C_DECLS
 #define BASE_BEGIN_C_DECLS_IS_NIL
 #define BASE_END_C_DECLS
 #define BASE_END_C_DECLS_IS_NIL
 #ifdef __STDC_VERSION__
  #define BASE_LANG_C __STDC_VERSION__
  #if (BASE_LANG_C < BASE_C_99)
   #error "When compiled as C, Base is C99 or higher only!"
  #endif
  #include <stdint.h>   /* We use stdint.h typedefs everywhere. */
  #include <inttypes.h> /* We want accesss to PRI macros. */
  #ifndef BASE_RESTRICT_IMPL
   #define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_C
  #endif
  #if (BASE_LANG_C >= BASE_C_11)
   #define BASE_STATIC_ASSERT(Boolean, Msg) _Static_assert(Boolean, Msg)
   #define BASE_ALIGNAS(As)                 _Alignas(As)
   #define BASE_ALIGNOF(Of)                 _Alignof(Of)
  #else
   #define BASE_STATIC_ASSERT(Boolean_, Msg_)
   #define BASE_STATIC_ASSERT_IS_NIL
   #define BASE_ALIGNAS(As_)
   #define BASE_ALIGNAS_IS_NIL
   #define BASE_ALIGNOF(Of_)
   #define BASE_ALIGNOF_IS_NIL
  #endif
  #if (BASE_LANG_C >= BASE_C_23)
   #define BASE_STATIC_ASSERT_1(Boolean) _Static_assert(Boolean)
  #else
   #define BASE_STATIC_ASSERT_1(Boolean_)
   #define BASE_STATIC_ASSERT_1_IS_NIL
  #endif
 #else  /* __STDC_VERSION__ not defined. */
  /* We use C99 features, so if we can't detect a C version just pray
   * everything doesn't break. */
  #define BASE_LANG_C 0L
  #ifndef BASE_RESTRICT_IMPL
   #define BASE_RESTRICT_IMPL 0
  #endif
  #define BASE_STATIC_ASSERT(Boolean_, Msg_)
  #define BASE_STATIC_ASSERT_IS_NIL
  #define BASE_STATIC_ASSERT_1(Boolean_)
  #define BASE_STATIC_ASSERT_1_IS_NIL
  #define BASE_ALIGNAS(As_)
  #define BASE_ALIGNAS_IS_NIL
  #define BASE_ALIGNOF(Of_)
  #define BASE_ALIGNOF_IS_NIL
 #endif /* !#i fdef __STDC_VERSION__ */
 #define BASE_COMPOUND_LITERAL(Type, ...) (Type){__VA_ARGS__} /* C99 literal syntax. */
 #define BASE_CONSTEXPR
 #define BASE_CONSTEXPR_IS_NIL
 #define BASE_CONSTEVAL
 #define BASE_CONSTEVAL_IS_NIL
#endif /* ! #ifdef __cplusplus */

/* If we can do a compile-time assertion, BASE_ANY_ASSERT()
 * is equivalent to BASE_STATIC_ASSERT(). If we cannot, the assertion
 * is runtime. Only pass compile-time boolean expressions as @Bool and
 * compile-time strings as @Msg. */
#ifdef BASE_STATIC_ASSERT_IS_NIL
 #define BASE_ANY_ASSERT(Bool, Msg) Base_assert_msg(Bool, Msg)
#else
 #define BASE_ANY_ASSERT(Bool, Msg) BASE_STATIC_ASSERT(Bool, Msg)
#endif

/* If we can do a compile-time assertion, BASE_ANY_ASSERT_1()
 * is equivalent to BASE_STATIC_ASSERT_1(). If we cannot, the assertion
 * is runtime. Only pass compile-time boolean expressions as @Bool. */
#ifdef BASE_STATIC_ASSERT_1_IS_NIL
 #define BASE_ANY_ASSERT_1(Bool) Base_assert(Bool)
#else
 #define BASE_ANY_ASSERT_1(Bool) BASE_STATIC_ASSERT_1(Bool)
#endif

/* Can we restrict pointers? C++ or C99 style? */
#ifndef BASE_RESTRICT_IMPL
 #define BASE_RESTRICT_IMPL 0
#endif
#if (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_CPP) ||\
     ((BASE_COMPILER == BASE_COMPILER_MSVC) && (defined(BASE_COMPILER_V) && (BASE_COMPILER_V >= 1900)))
 #define BASE_RESTRICT __restrict /* C++/MSVC compatible restrict. */
#elif (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_C)
 #define BASE_RESTRICT restrict   /* C99-specified restrict. */
#else
 #define BASE_RESTRICT /* We don't have restrict. */
 #define BASE_RESTRICT_IS_NIL
#endif
/* Cleanup restrict implementation. */
#undef BASE_RESTRICT_IMPL_C
#undef BASE_RESTRICT_IMPL_CPP
#undef BASE_RESTRICT_IMPL

/* Symbol Visibility, Export/Import Macros */
#if !defined(BASE_COMPILER)
 #error "BASE_COMPILER undefined!"
#elif !BASE_COMPILER_ISVALID(BASE_COMPILER)
 #error "BASE_COMPILER is invalid!"
#elif (BASE_COMPILER == BASE_COMPILER_UNKNOWN)
 #warning "Compiler unknown: NIL-ing import/export attributes!"
 #define BASE_EXPORT
 #define BASE_EXPORT_IS_NIL
 #define BASE_IMPORT
 #define BASE_IMPORT_IS_NIL
#elif BASE_COMPILER_IS_GCC_COMPATIBLE
 #if defined(BASE_OS_UNIXLIKE)
  #define BASE_EXPORT __attribute__ ((visibility ("default")))
  #define BASE_IMPORT BASE_EXPORT
 #elif defined(BASE_OS_WINDOWS)
  #define BASE_EXPORT __attribute__ ((dllexport))
  #define BASE_IMPORT __attribute__ ((dllimport))
 #else
  #error "GCC/Clang support Unixlikes and Windows."
 #endif
#elif (BASE_COMPILER == BASE_COMPILER_MSVC)
 #ifndef BASE_OS_WINDOWS
  #error "This is only for Windows OS's."
 #endif
 #define BASE_EXPORT __declspec(dllexport)
 #define BASE_IMPORT __declspec(dllimport)
#else
 #error "Unaccounted for compiler."
#endif

/* Currenly in Base all inline functions are static inline. */
#define BASE_INLINE		  static inline
#define BASE_STRINGIFY_IMPL(Text) #Text
#define BASE_STRINGIFY(Text)      BASE_STRINGIFY_IMPL(Text)

#ifdef BASE_EXTERN_STATIC_LIB /* Base is being built, or imported as a static library. */
 #define BASE_API /* Nil */
 #define BASE_API_IS_NIL
#else
 #ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB
  #define BASE_API BASE_EXPORT
  #define BASE_API_IS_EXPORT
  #ifdef BASE_EXPORT_IS_NIL
   #define BASE_API_IS_NIL
  #endif
 #else /* Assume that Base is being imported as a dynamic library. */
  #define BASE_API BASE_IMPORT
  #define BASE_API_IS_IMPORT
  #ifdef BASE_IMPORT_IS_NIL
   #define BASE_API_IS_NIL
  #endif
 #endif /* ! #ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB */
#endif /* ! #ifdef BASE_EXTERN_STATIC_LIB */

/* We conditionally include `errors.h' below, but `errors.h' depends upon some macros defined here in `macros.h'.
 * The following macros must remain at the end of `macros.h' to avoid circular dependence problems.*/

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
 #include <unistd.h>
 #include "errors.h"
 #define BASE_OPENBSD_PLEDGE(Promises, Execpromises) Base_assert_msg(!pledge(Promises, Execpromises), "Failed to pledge()!\n")
 #define BASE_OPENBSD_UNVEIL(Path    , Permissions)  Base_assert_msg(!unveil(Path    , Permissions ), "Failed to unveil()!\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
 #define BASE_OPENBSD_PLEDGE(Promises_, Execpromises_) /* Nil */
 #define BASE_OPENBSD_UNVEIL(Path_    , Permissions_)  /* Nil */
#endif /* ! #ifdef __OpenBSD__ */

#endif /* ! #ifndef BASE_MACROS_H */
