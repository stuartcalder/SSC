/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_MACRO_H
#define SSC_MACRO_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/* Endianness. */
#define SSC_ENDIAN_NONE   0 /* Endianness unestablished. */
#define SSC_ENDIAN_LITTLE 1 /* Least-Significant-Byte-First Ordering. */
#define SSC_ENDIAN_BIG    2 /* Most-Significant-Byte-First Ordering. */
#define SSC_ENDIAN_DEFAULT SSC_ENDIAN_LITTLE /* For now just assume little endian if unspecified. */
#define SSC_ENDIAN_ISVALID(Endian) ((Endian) == SSC_ENDIAN_LITTLE || (Endian) == SSC_ENDIAN_BIG) /* NONE is invalid. */
/* How are we determining endianness? */
#define SSC_ENDIAN_SRC_NONE     0 /* Endian source unestablished. */
#define SSC_ENDIAN_SRC_EXTERN   1 /* Endianness was specified externally, in SSC_EXTERN_ENDIAN. */
#define SSC_ENDIAN_SRC_ISA      2 /* Endianness was determined by the ISA of the CPU. */
#define SSC_ENDIAN_SRC_COMPILER 3 /* Endianness was specified by the compiler. */
#define SSC_ENDIAN_SRC_DEFAULT  4 /* Endianness was set to defaults. */
#define SSC_ENDIAN_SRC_ISVALID(EndSrc) ((EndSrc) >= SSC_ENDIAN_SRC_EXTERN && (EndSrc) <= SSC_ENDIAN_SRC_DEFAULT) /* NONE is invalid. */
/* What is the compiler? */
#define SSC_COMPILER_UNKNOWN 0 /* Unrecognized compiler. */
#define SSC_COMPILER_GCC     1 /* GNU C/C++ compiler. */
#define SSC_COMPILER_CLANG   2 /* Clang C/C++ compiler. */
#define SSC_COMPILER_MSVC    3 /* Microsoft Visual C/C++ compiler. */
#define SSC_COMPILER_ISVALID(Comp) ((Comp) >= SSC_COMPILER_UNKNOWN && (Comp) <= SSC_COMPILER_MSVC) /* UNKNOWN is valid. */
/* Pointer-aliasing restrict support bits. */
#define SSC_RESTRICT_IMPL_C    0x01 /* C99 restrict. */
#define SSC_RESTRICT_IMPL_CPP  0x02 /* C++/MSVC restrict. */

/* Which compiler are we using? */
#if   defined(__clang__)
 #define SSC_COMPILER   SSC_COMPILER_CLANG
#elif defined(_MSC_VER)
 #define SSC_COMPILER   SSC_COMPILER_MSVC
 #define SSC_COMPILER_V _MSC_VER
#elif defined(__GNUC__)
 #define SSC_COMPILER   SSC_COMPILER_GCC
#else
 #define SSC_COMPILER SSC_COMPILER_UNKNOWN
 #warning "SSC_COMPILER unknown!"
#endif

/* Is our compiler a gcc-compatible compiler? */
#define SSC_COMPILER_IS_GCC_COMPATIBLE ((SSC_COMPILER == SSC_COMPILER_GCC) || (SSC_COMPILER == SSC_COMPILER_CLANG))

/* Operating system macros */
#if defined(__APPLE__) && defined(__MACH__)
 #define SSC_OS_MAC
#endif /* ! #if defined (__APPLE__) and defined (__MACH__) */

/* More unixlikes could go here without too much code change, probably. */
#if (defined(SSC_OS_MAC)    ||\
     defined(__ANDROID__)   ||\
     defined(__Dragonfly__) ||\
     defined(__FreeBSD__)   ||\
     defined(__gnu_linux__) ||\
     defined(__NetBSD__)    ||\
     defined(__OpenBSD__))
 #define SSC_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif defined(_WIN32) || defined(__CYGWIN__)
 #define SSC_OS_WINDOWS
 #ifdef _WIN64
  #define SSC_OS_WIN64
 #else
  #define SSC_OS_WIN32
 #endif /* ! #ifdef _WIN64 */
#else
 #error "Unsupported."
#endif /* ! #if defined (unixlike os's ...) */

/* GNU/Linux Specific. */
#ifdef __gnu_linux__
 #define SSC_LINUX_VERSION_MAJOR(Num)    ((Num) * 1000000ULL)
 #define SSC_LINUX_VERSION_MINOR(Num)    ((Num) * 1000ULL)
 #define SSC_LINUX_VERSION_REVISION(Num) ((Num) * 1ULL)

 #define SSC_LINUX_VERSION(Major, Minor, Revision) (\
  SSC_LINUX_VERSION_MAJOR(Major) +\
  SSC_LINUX_VERSION_MINOR(Minor) +\
  SSC_LINUX_VERSION_REVISION(Revision))

 #ifdef SSC_EXTERN_LINUX_VERSION_MAJOR
  #define SSC_LINUX_VERSION_MAJOR_VALUE_ISDEFINED 1
  #define SSC_LINUX_VERSION_MAJOR_VALUE \
   SSC_LINUX_VERSION_MAJOR(SSC_EXTERN_LINUX_VERSION_MAJOR)
 #else
  #define SSC_LINUX_VERSION_MAJOR_VALUE_ISDEFINED 0
  #define SSC_LINUX_VERSION_MAJOR_VALUE (0ULL)
 #endif

 #ifdef SSC_EXTERN_LINUX_VERSION_MINOR
  #define SSC_LINUX_VERSION_MINOR_VALUE_ISDEFINED 1
  #define SSC_LINUX_VERSION_MINOR_VALUE \
   SSC_LINUX_VERSION_MINOR(SSC_EXTERN_LINUX_VERSION_MINOR)
 #else
  #define SSC_LINUX_VERSION_MINOR_VALUE_ISDEFINED 0
  #define SSC_LINUX_VERSION_MINOR_VALUE (0ULL)
 #endif

 #ifdef SSC_EXTERN_LINUX_VERSION_REVISION
  #define SSC_LINUX_VERSION_REVISION_VALUE_ISDEFINED 1
  #define SSC_LINUX_VERSION_REVISION_VALUE \
   SSC_LINUX_VERSION_REVISION(SSC_EXTERN_LINUX_VERSION_REVISION)
 #else
  #define SSC_LINUX_VERSION_REVISION_VALUE_ISDEFINED 0
  #define SSC_LINUX_VERSION_REVISION_VALUE (0ULL)
 #endif

 #define SSC_LINUX_VERSION_VALUE \
  (SSC_LINUX_VERSION_MAJOR_VALUE +\
   SSC_LINUX_VERSION_MINOR_VALUE +\
   SSC_LINUX_VERSION_REVISION_VALUE)
 #define SSC_LINUX_VERSION_VALUE_ISDEFINED \
  (SSC_LINUX_VERSION_MAJOR_VALUE_ISDEFINED && SSC_LINUX_VERSION_MINOR_VALUE_ISDEFINED)
#endif /* ! ifdef __gnu_linux__ */

/* C language standards. */
#define SSC_C_89 199409L
#define SSC_C_99 199901L
#define SSC_C_11 201112L
#define SSC_C_17 201710L
#define SSC_C_23 202311L
/* C++ language standards. */
#define SSC_CPP_11 201103L
#define SSC_CPP_14 201402L
#define SSC_CPP_17 201703L
#define SSC_CPP_20 202002L
#define SSC_CPP_23 202302L

/* SSC_ENDIAN
 * int
 *   Defines the native byte order.
 *   May be defined outside by the SSC_EXTERN_ENDIAN macro.
 *   Must equal to SSC_ENDIAN_LITTLE, or SSC_ENDIAN_BIG. */
#if !defined(SSC_ENDIAN) && defined(SSC_EXTERN_ENDIAN)
 #if !SSC_ENDIAN_ISVALID(SSC_EXTERN_ENDIAN)
  #error "SSC_EXTERN_ENDIAN is an invalid endianness!"
 #endif
  /* External definition trumps all endian detection methods. */
 #define SSC_ENDIAN     SSC_EXTERN_ENDIAN
 #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_EXTERN
#endif

/* GCC/Clang provide __BYTE_ORDER__ for us to check byte endianness directly. Use this when possible. */
#if (!defined(SSC_ENDIAN) && defined(__GNUC__) && defined(__BYTE_ORDER__) &&\
     defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))
 #if   __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define SSC_ENDIAN SSC_ENDIAN_LITTLE
 #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  #define SSC_ENDIAN SSC_ENDIAN_BIG
 #else
  #error "Invalid __BYTE_ORDER__!"
 #endif
 #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_COMPILER
#endif

/* SSC_ISA
 * int
 *   The instruction set architecture of the system.
 *   If not explicitly defined in this file,
 *   SSC_ISA will default to SSC_ISA_UNKNOWN. */
#define SSC_ISA_UNKNOWN 0
#define SSC_ISA_AMD64   1
#define SSC_ISA_RISCV   2
#define SSC_ISA_ARM64   3
#define SSC_ISA_X86     4
#define SSC_ISA_ARMV7   5
#define SSC_ISA_ISVALID(Isa) ((Isa) >= SSC_ISA_UNKNOWN && (Isa) <= SSC_ISA_ARMV7) /* UNKNOWN arch is valid. */

/* Architecture macros. */
#if (defined(__amd64)  || defined(__amd64__)  ||\
     defined(__x86_64) || defined(__x86_64__) ||\
     defined(_M_X64)   || defined(_M_AMD64))
 #define SSC_ISA SSC_ISA_AMD64
 #ifndef SSC_ENDIAN
   /* AMD64 is little endian. */
  #define SSC_ENDIAN     SSC_ENDIAN_LITTLE
  #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_ISA
 #endif
#elif defined(__riscv)
 #define SSC_ISA SSC_ISA_RISCV
 #ifndef SSC_ENDIAN
   /* RISCV is little endian. */
  #define SSC_ENDIAN     SSC_ENDIAN_LITTLE
  #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_ISA
 #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
 #define SSC_ISA SSC_ISA_ARM64
 #ifndef SSC_ENDIAN
  #if SSC_COMPILER == SSC_COMPILER_MSVC /* Assume little-endian mode when used with MSVC. */
   #define SSC_ENDIAN     SSC_ENDIAN_LITTLE
   #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_ISA
  #else
   #if   SSC_ENDIAN_DEFAULT == SSC_ENDIAN_LITTLE
    #warning "Aarch64 is bi-endian, and SSC_ENDIAN is still not yet defined! Using default endianness (Little)."
   #elif SSC_ENDIAN_DEFAULT == SSC_ENDIAN_BIG
    #warning "Aarch64 is bi-endian, and SSC_ENDIAN is still not yet defined! Using default endianness (Big)."
   #else
    #error "SSC_ENDIAN_DEFAULT is invalid!"
   #endif
   #define SSC_ENDIAN     SSC_ENDIAN_DEFAULT
   #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_DEFAULT
  #endif /* ! if (SSC_COMPILER == SSC_COMPILER_MSVC) */
 #endif /* ! ifndef SSC_ENDIAN */
#elif defined(__i386__) || defined(_M_IX86)
 #define SSC_ISA SSC_ISA_X86
 #ifndef SSC_ENDIAN
   /* X86 is little endian. */
  #define SSC_ENDIAN     SSC_ENDIAN_LITTLE
  #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_ISA
 #endif /* ! ifndef SSC_ENDIAN */
#elif defined(__arm__) || defined(_M_ARM)
 #define SSC_ISA SSC_ISA_ARMV7
 #ifndef SSC_ENDIAN
  #if SSC_COMPILER == SSC_COMPILER_MSVC /* Assume little-endian mode when used with MSVC. */
   #define SSC_ENDIAN     SSC_ENDIAN_LITTLE
   #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_COMPILER
  #else
   #if   SSC_ENDIAN_DEFAULT == SSC_ENDIAN_LITTLE
    #warning "Armv7 is bi-endian, and SSC_ENDIAN is still not yet defined! Using default endianness (Little)."
   #elif SSC_ENDIAN_DEFAULT == SSC_ENDIAN_BIG
    #warning "Armv7 is bi-endian, and SSC_ENDIAN is still not yet defined! Using default endianness (Big)."
   #else
    #error "SSC_ENDIAN is invalid!"
   #endif
   #define SSC_ENDIAN     SSC_ENDIAN_DEFAULT
   #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_DEFAULT
  #endif /* ! ifdef SSC_COMPILER_MSVC */
 #endif /* ! ifndef SSC_ENDIAN */
#else
 #warning "Failed to detect ISA."
 #define SSC_ISA SSC_ISA_UNKNOWN
 #ifndef SSC_ENDIAN
  #if   SSC_ENDIAN_DEFAULT == SSC_ENDIAN_LITTLE
   #warning "SSC_ISA is UNKNOWN. Will use default (little) endianness and hope for the best."
  #elif SSC_ENDIAN_DEFAULT == SSC_ENDIAN_BIG
   #warning "SSC_ISA is UNKNOWN. Will use default (big) endianness and hope for the best."
  #else
   #error "SSC_ENDIAN_DEFAULT is invalid!"
  #endif
  #warning "To specify endianness, define SSC_EXTERN_ENDIAN to SSC_ENDIAN_LITTLE or SSC_ENDIAN_BIG!"
  #define SSC_ENDIAN     SSC_ENDIAN_DEFAULT
  #define SSC_ENDIAN_SRC SSC_ENDIAN_SRC_DEFAULT
 #endif
#endif

/* Sanity assertions. */
#if   !defined(SSC_COMPILER)
 #error "SSC_COMPILER is not defined!"
#elif !SSC_COMPILER_ISVALID(SSC_COMPILER)
 #error "SSC_COMPILER is an invalid compiler!"
#elif !defined(SSC_ENDIAN)
 #error "SSC_ENDIAN is not defined!"
#elif !SSC_ENDIAN_ISVALID(SSC_ENDIAN)
 #error "SSC_ENDIAN is an invalid endianness!"
#elif !defined(SSC_ENDIAN_SRC)
 #error "SSC_ENDIAN_SRC is not defined!"
#elif !SSC_ENDIAN_SRC_ISVALID(SSC_ENDIAN_SRC)
 #error "SSC_ENDIAN_SRC is an invalid endian source!"
#elif !defined(SSC_ISA)
 #error "SSC_ISA is not defined!"
#elif !SSC_ISA_ISVALID(SSC_ISA)
 #error "SSC_ISA is an invalid ISA!"
#endif

/* C/C++ Interoperability Macros */
#ifdef __cplusplus
 #define SSC_LANG_CPP __cplusplus
 #define SSC_LANG     SSC_LANG_CPP
 #define SSC_NULL     nullptr
/* C++ doesn't support "restrict". Use the non-standard "__restrict" compiler extension. */
 #ifndef SSC_RESTRICT_IMPL
  #define SSC_RESTRICT_IMPL SSC_RESTRICT_IMPL_CPP
 #endif
/* Use C function name mangling. */
 #define SSC_BEGIN_C_DECLS extern "C" {
 #define SSC_END_C_DECLS   }
/* If we're using C++11 or above, support "static_assert", "alignas", and "alignof". */
 #if SSC_LANG_CPP < SSC_CPP_11
  #error "When compiled as C++, SSC is C++11 or higher only!"
 #endif
 #define SSC_STATIC_ASSERT(Boolean, Message) static_assert(Boolean, Message)
 #define SSC_ALIGNAS(V)                      alignas(V)
 #define SSC_ALIGNOF(V)                      alignof(V)
 #if SSC_LANG_CPP >= SSC_CPP_17
  #define SSC_STATIC_ASSERT_1(Boolean)       static_assert(Boolean)
 #else
  #define SSC_STATIC_ASSERT_1(Boolean_)
  #define SSC_STATIC_ASSERT_1_IS_NIL
 #endif
  /* consteval is similar to, but distinct from, constexpr.
   * Be aware of that when using SSC_CONSTEVAL or SSC_CONSTEXPR in the first place. */
 #if SSC_LANG_CPP >= SSC_CPP_20
  #define SSC_CONSTEVAL consteval
 #else
  #define SSC_CONSTEVAL constexpr
  #define SSC_CONSTEVAL_IS_CONSTEXPR /* We have constexpr but not consteval. Assume constexpr is a `downgrade` from consteval.*/
 #endif
 #define SSC_COMPOUND_LITERAL(Type, ...) Type{__VA_ARGS__} /* C++ literal syntax. */
 #define SSC_CONSTEXPR constexpr
#else /* Not C++. We are using C. */
 #define SSC_LANG SSC_LANG_C
 #define SSC_BEGIN_C_DECLS
 #define SSC_BEGIN_C_DECLS_IS_NIL
 #define SSC_END_C_DECLS
 #define SSC_END_C_DECLS_IS_NIL
 #ifdef __STDC_VERSION__
  #define SSC_LANG_C __STDC_VERSION__
  #if SSC_LANG_C < SSC_C_99
   #error "When compiled as C, SSC is C99 or higher only!"
  #endif
  #include <stdint.h>   /* We use stdint.h typedefs everywhere. */
  #include <inttypes.h> /* We want accesss to PRI macros. */
  #ifndef SSC_RESTRICT_IMPL
   #define SSC_RESTRICT_IMPL SSC_RESTRICT_IMPL_C
  #endif
  #if SSC_LANG_C >= SSC_C_11
   #define SSC_STATIC_ASSERT(Boolean, Msg) _Static_assert(Boolean, Msg)
   #define SSC_ALIGNAS(As)                 _Alignas(As)
   #define SSC_ALIGNOF(Of)                 _Alignof(Of)
  #else
   #define SSC_STATIC_ASSERT(Boolean_, Msg_)
   #define SSC_STATIC_ASSERT_IS_NIL
   #define SSC_ALIGNAS(As_)
   #define SSC_ALIGNAS_IS_NIL
   #define SSC_ALIGNOF(Of_)
   #define SSC_ALIGNOF_IS_NIL
  #endif
  #if SSC_LANG_C >= SSC_C_23
   #define SSC_STATIC_ASSERT_1(Boolean) _Static_assert(Boolean)
   #define SSC_NULL nullptr
  #else
   #define SSC_STATIC_ASSERT_1(Boolean_)
   #define SSC_STATIC_ASSERT_1_IS_NIL
   #define SSC_NULL NULL
  #endif
 #else  /* __STDC_VERSION__ not defined. */
  /* We use C99 features, so if we can't detect a C version just pray
   * everything doesn't break. */
  #define SSC_LANG_C 0L
  #ifndef SSC_RESTRICT_IMPL
   #define SSC_RESTRICT_IMPL 0
  #endif
  #define SSC_STATIC_ASSERT(Boolean_, Msg_)
  #define SSC_STATIC_ASSERT_IS_NIL
  #define SSC_STATIC_ASSERT_1(Boolean_)
  #define SSC_STATIC_ASSERT_1_IS_NIL
  #define SSC_ALIGNAS(As_)
  #define SSC_ALIGNAS_IS_NIL
  #define SSC_ALIGNOF(Of_)
  #define SSC_ALIGNOF_IS_NIL
 #endif /* !#ifdef __STDC_VERSION__ */
 #define SSC_COMPOUND_LITERAL(Type, ...) (Type){__VA_ARGS__} /* C99 literal syntax. */
 #define SSC_CONSTEXPR
 #define SSC_CONSTEXPR_IS_NIL
 #define SSC_CONSTEVAL
 #define SSC_CONSTEVAL_IS_NIL
#endif /* ! #ifdef __cplusplus */

/* If we can do a compile-time assertion, SSC_ANY_ASSERT()
 * is equivalent to SSC_STATIC_ASSERT(). If we cannot, the assertion
 * is runtime. Only pass compile-time boolean expressions as @Bool and
 * compile-time strings as @Msg. */
#ifdef SSC_STATIC_ASSERT_IS_NIL
 #define SSC_ANY_ASSERT(Bool, Msg) SSC_assertMsg(Bool, Msg)
#else
 #define SSC_ANY_ASSERT(Bool, Msg) SSC_STATIC_ASSERT(Bool, Msg)
#endif

/* If we can do a compile-time assertion, SSC_ANY_ASSERT_1()
 * is equivalent to SSC_STATIC_ASSERT_1(). If we cannot, the assertion
 * is runtime. Only pass compile-time boolean expressions as @Bool. */
#ifdef SSC_STATIC_ASSERT_1_IS_NIL
 #define SSC_ANY_ASSERT_1(Bool) SSC_assert(Bool)
#else
 #define SSC_ANY_ASSERT_1(Bool) SSC_STATIC_ASSERT_1(Bool)
#endif

/* Can we restrict pointers? C++ or C99 style? */
#ifndef SSC_RESTRICT_IMPL
 #define SSC_RESTRICT_IMPL 0
#endif
#if (SSC_RESTRICT_IMPL & SSC_RESTRICT_IMPL_CPP) ||\
     ((SSC_COMPILER == SSC_COMPILER_MSVC) && (defined(SSC_COMPILER_V) && (SSC_COMPILER_V >= 1900)))
 #define SSC_RESTRICT __restrict /* C++/MSVC compatible restrict. */
#elif SSC_RESTRICT_IMPL & SSC_RESTRICT_IMPL_C
 #define SSC_RESTRICT restrict   /* C99-specified restrict. */
#else
 #define SSC_RESTRICT /* We don't have restrict. */
 #define SSC_RESTRICT_IS_NIL
#endif
/* Cleanup restrict implementation. */
#undef SSC_RESTRICT_IMPL_C
#undef SSC_RESTRICT_IMPL_CPP
#undef SSC_RESTRICT_IMPL

/* Symbol Visibility, Export/Import Macros */
#if !defined(SSC_COMPILER)
 #error "SSC_COMPILER undefined!"
#elif !SSC_COMPILER_ISVALID(SSC_COMPILER)
 #error "SSC_COMPILER is invalid!"
#elif SSC_COMPILER == SSC_COMPILER_UNKNOWN
 /* When the compiler is unknown, NIL all the import and export
  * macros. If we don't know how the compiler decides what to
  * export/what to import prefer to simply do nothing.
  */
 #warning "Compiler unknown: NIL-ing import/export attributes!"
 #define SSC_EXPORT
 #define SSC_EXPORT_IS_NIL
 #define SSC_IMPORT
 #define SSC_IMPORT_IS_NIL
#elif SSC_COMPILER_IS_GCC_COMPATIBLE
 #if defined(SSC_OS_UNIXLIKE)
  /* UNIX exports all symbols by default. Make the visibility of exported
   * and imported symbols as default.
   */
  #define SSC_EXPORT __attribute__((visibility ("default")))
  #define SSC_IMPORT SSC_EXPORT
 #elif defined(SSC_OS_WINDOWS)
 /* MinGW supports dllexport and dllimport attributes similar to MSVC's __declspec. */
  #define SSC_EXPORT __attribute__((dllexport))
  #define SSC_IMPORT __attribute__((dllimport))
 #else
  #error "GCC/Clang support Unixlikes and Windows."
 #endif
#elif SSC_COMPILER == SSC_COMPILER_MSVC
 #ifndef SSC_OS_WINDOWS
  #error "This is only for Windows OS's."
 #endif
 #define SSC_EXPORT __declspec(dllexport)
 #define SSC_IMPORT __declspec(dllimport)
#else
 #error "Unaccounted for compiler."
#endif

#if   SSC_COMPILER_IS_GCC_COMPATIBLE
 #define SSC_INLINE __attribute__((always_inline)) inline
#elif SSC_COMPILER == SSC_COMPILER_MSVC
 #define SSC_INLINE __forceinline inline
#else
 #define SSC_INLINE static inline
#endif

#define SSC_STRINGIFY_IMPL(Text) #Text
#define SSC_STRINGIFY(Text)      SSC_STRINGIFY_IMPL(Text)

/* If you want to build or import SSC as a static library you must externally define
 * the macro SSC_EXTERN_STATIC_LIB, otherwise it is assumed SSC is a shared
 * object/dynamically linked library.
 */
#ifdef SSC_EXTERN_STATIC_LIB
 #define SSC_API /* Nil */
 #define SSC_API_IS_NIL
#else
 #ifdef SSC_EXTERN_BUILD_DYNAMIC_LIB
  #define SSC_API SSC_EXPORT
  #define SSC_API_IS_EXPORT
  #ifdef SSC_EXPORT_IS_NIL
   #define SSC_API_IS_NIL
  #endif
 #else /* Assume that SSC is being imported as a dynamically linked library. */
  #define SSC_API SSC_IMPORT
  #define SSC_API_IS_IMPORT
  #ifdef SSC_IMPORT_IS_NIL
   #define SSC_API_IS_NIL
  #endif
 #endif /* ! #ifdef SSC_EXTERN_BUILD_DYNAMIC_LIB */
#endif /* ! #ifdef SSC_EXTERN_STATIC_LIB */

/* We conditionally include `Error.h' below, but `Error.h' depends upon some macros defined here in `Macro.h'.
 * The following macros must remain at the end of `Macro.h' to avoid circular dependence problems.*/

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
 #include <unistd.h>
 #include "Error.h"
 #define SSC_OPENBSD_PLEDGE(Promises, Execpromises) SSC_assertMsg(!pledge(Promises, Execpromises), "Failed to pledge()!\n")
 #define SSC_OPENBSD_UNVEIL(Path    , Permissions)  SSC_assertMsg(!unveil(Path    , Permissions ), "Failed to unveil()!\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
 #define SSC_OPENBSD_PLEDGE(Promises_, Execpromises_) /* Nil */
 #define SSC_OPENBSD_UNVEIL(Path_    , Permissions_)  /* Nil */
#endif /* ! #ifdef __OpenBSD__ */

#endif /* ! #ifndef SSC_MACRO_H */
