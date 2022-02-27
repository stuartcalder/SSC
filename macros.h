#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/* Flags to indicate support for restricting pointers. */
#define BASE_RESTRICT_IMPL_C    0x01
#define BASE_RESTRICT_IMPL_CPP  0x02
/* Endianness. */
#define BASE_ENDIAN_LITTLE  0
#define BASE_ENDIAN_BIG     1
#define BASE_ENDIAN_DEFAULT BASE_ENDIAN_LITTLE
#define BASE_ENDIAN_ISVALID(Endian) (Endian == BASE_ENDIAN_LITTLE || Endian == BASE_ENDIAN_BIG)

/* Try to detect the compiler. */
#if defined(__clang__)
  /* We need to test Clang first because it also defines __GNUC__, and may also define _MSC_VER. */
# define BASE_COMPILER_CLANG
#elif defined(_MSC_VER)
  /* We can test _MSC_VER as BASE_COMPILER_MSVC for features later. */
# define BASE_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
# define BASE_COMPILER_GCC
#else
# warning "Failed to detect the compiler."
# define BASE_COMPILER_UNKNOWN
#endif

/* Operating System Macros */
#if defined(__APPLE__) && defined(__MACH__)
# define BASE_OS_MAC
#endif /* ! #if defined (__APPLE__) and defined (__MACH__) */

/* More unixlikes could go here without too much code change, probably. */
#if (defined(BASE_OS_MAC)   || \
     defined(__Dragonfly__) || \
     defined(__FreeBSD__)   || \
     defined(__gnu_linux__) || \
     defined(__NetBSD__)    || \
     defined(__OpenBSD__))
# define BASE_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif defined(_WIN32) || defined(__CYGWIN__)
# define BASE_OS_WINDOWS
# ifdef _WIN64
#  define BASE_OS_WIN64
# else
#  define BASE_OS_WIN32
# endif /* ! #ifdef _WIN64 */
#else
# error "Unsupported."
#endif /* ! #if defined (unixlike os's ...) */

/* C language standards. */
#define BASE_C_89 199409L
#define BASE_C_99 199901L
#define BASE_C_11 201112L
#define BASE_C_17 201710L
/* C++ language standards. */
#define BASE_CPP_11 201103L
#define BASE_CPP_14 201402L
#define BASE_CPP_17 201703L
#define BASE_CPP_20 202002L
#define BASE_CPP_23 202312L /* Hasn't released yet. Set this to December of 2023 for now. */

/* BASE_ENDIAN
 * int
 *   Defines the native byte order.
 *   May be defined outside by the BASE_EXTERN_ENDIAN macro.
 *   Must equal to BASE_ENDIAN_LITTLE, or BASE_ENDIAN_BIG.
 */
#if !defined(BASE_ENDIAN) && defined(BASE_EXTERN_ENDIAN)
# if !BASE_ENDIAN_ISVALID(BASE_EXTERN_ENDIAN)
#  error "BASE_EXTERN_ENDIAN is an invalid endianness!"
# endif
  /* External definition trumps all endian detection methods. */
# define BASE_ENDIAN BASE_EXTERN_ENDIAN
# define BASE_ENDIAN_IS_EXTERN
#endif

/* GCC/Clang provide __BYTE_ORDER__ for us to check endian directly. Use this when possible. */
#if (!defined(BASE_ENDIAN) && defined(__GNUC__) && defined(__BYTE_ORDER__) && \
     defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__))
# if   (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define BASE_ENDIAN BASE_ENDIAN_LITTLE
# elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#  define BASE_ENDIAN BASE_ENDIAN_BIG
# else
#  error "Invalid __BYTE_ORDER__!"
# endif
# define BASE_ENDIAN_IS_GNUC
#endif

/* BASE_ISA
 * int
 *   The instruction set architecture of the system.
 *   If not explicitly defined in this file,
 *   BASE_ISA will default to BASE_ISA_UNKNOWN.
 */

#define BASE_ISA_UNKNOWN 0
#define BASE_ISA_AMD64   1
#define BASE_ISA_RISCV   2
#define BASE_ISA_ARM64   3
#define BASE_ISA_X86     4
#define BASE_ISA_ARMV7   5

/* Architecture macros. */
#if (defined(__amd64)  || defined(__amd64__)  || \
     defined(__x86_64) || defined(__x86_64__) || \
     defined(_M_X64)   || defined(_M_AMD64))
# define BASE_ISA BASE_ISA_AMD64
# ifndef BASE_ENDIAN
   /* AMD64 is little endian. */
#  define BASE_ENDIAN BASE_ENDIAN_LITTLE
#  define BASE_ENDIAN_IS_ISA
# endif
#elif defined(__riscv)
# define BASE_ISA BASE_ISA_RISCV
# ifndef BASE_ENDIAN
   /* RISCV is little endian. */
#  define BASE_ENDIAN BASE_ENDIAN_LITTLE
#  define BASE_ENDIAN_IS_ISA
# endif
#elif (defined(__aarch64__) || defined(_M_ARM64))
# define BASE_ISA BASE_ISA_ARM64
# ifndef BASE_ENDIAN
#  ifdef BASE_COMPILER_MSVC /* Assume little-endian mode when used with MSVC. */
#   define BASE_ENDIAN BASE_ENDIAN_LITTLE
#   define BASE_ENDIAN_IS_COMPILER
#  else
#   if   (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_LITTLE)
#    warning "Aarch64 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Little)."
#   elif (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_BIG)
#    warning "Aarch64 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Big)."
#   else
#    error "BASE_ENDIAN is invalid!"
#   endif
#   define BASE_ENDIAN BASE_ENDIAN_DEFAULT
#   define BASE_ENDIAN_IS_DEFAULT
#  endif /* ! ifdef BASE_COMPILER_MSVC */
# endif /* ! ifndef BASE_ENDIAN */
#elif (defined(__i386__) || defined(_M_IX86))
# define BASE_ISA BASE_ISA_X86
# ifndef BASE_ENDIAN
   /* X86 is little endian. */
#  define BASE_ENDIAN BASE_ENDIAN_LITTLE
#  define BASE_ENDIAN_IS_ISA
# endif /* ! ifndef BASE_ENDIAN */
#elif defined(__arm__) || defined(_M_ARM)
# define BASE_ISA BASE_ISA_ARMV7
# ifndef BASE_ENDIAN
#  ifdef BASE_COMPILER_MSVC /* Assume little-endian mode when used with MSVC. */
#   define BASE_ENDIAN BASE_ENDIAN_LITTLE
#   define BASE_ENDIAN_IS_COMPILER
#  else
#   if   (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_LITTLE)
#    warning "Armv7 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Little)."
#   elif (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_BIG)
#    warning "Armv7 is bi-endian, and BASE_ENDIAN is still not yet defined! Using default endianness (Big)."
#   else
#    error "BASE_ENDIAN is invalid!"
#   endif
#   define BASE_ENDIAN BASE_ENDIAN_DEFAULT
#   define BASE_ENDIAN_IS_DEFAULT
#  endif /* ! ifdef BASE_COMPILER_MSVC */
# endif /* ! ifndef BASE_ENDIAN */
#else
# warning "Failed to detect ISA."
# define BASE_ISA BASE_ISA_UNKNOWN
# ifndef BASE_ENDIAN
#  if   (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_LITTLE)
#   warning "BASE_ISA is UNKNOWN. Will use default (little) endianness and hope for the best."
#  elif (BASE_ENDIAN_DEFAULT == BASE_ENDIAN_BIG)
#   warning "BASE_ISA is UNKNOWN. Will use default (big) endianness and hope for the best."
#  else
#   error "BASE_ENDIAN_DEFAULT is invalid!"
#  endif
#  warning "To specify endianness, define BASE_EXTERN_ENDIAN to BASE_ENDIAN_LITTLE or BASE_ENDIAN_BIG!"
#  define BASE_ENDIAN BASE_ENDIAN_DEFAULT
#  define BASE_ENDIAN_IS_DEFAULT
# endif
#endif
#ifndef BASE_ENDIAN
# error "BASE_ENDIAN is not defined!"
#elif !BASE_ENDIAN_ISVALID(BASE_ENDIAN)
# error "BASE_ENDIAN is an invalid endianness!"
#endif

/* C/C++ Interoperability Macros */
#ifdef __cplusplus
# define BASE_LANG_CPP __cplusplus
# define BASE_LANG     BASE_LANG_CPP
# define BASE_NULL     nullptr
/* C++ doesn't support "restrict". Use the non-standard "__restrict" compiler extension. */
# ifndef BASE_RESTRICT_IMPL
#  define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_CPP
# endif
/* Use C function name mangling. */
# define BASE_BEGIN_C_DECLS extern "C" {
# define BASE_END_C_DECLS   }
/* If we're using C++11 or above, support "static_assert", "alignas", and "alignof". */
# if (BASE_LANG_CPP < BASE_CPP_11)
#  error "When compiled as C++, Base is C++11 or higher only!"
# endif
# define BASE_STATIC_ASSERT(boolean, message) static_assert(boolean, message)
# define BASE_ALIGNAS(v)                      alignas(v)
# define BASE_ALIGNOF(v)                      alignof(v)
#else /* Not C++. We are using C. */
# define BASE_LANG BASE_LANG_C
# define BASE_NULL NULL
# define BASE_BEGIN_C_DECLS
# define BASE_BEGIN_C_DECLS_IS_NIL
# define BASE_END_C_DECLS
# define BASE_END_C_DECLS_IS_NIL
# ifdef __STDC_VERSION__
#  define BASE_LANG_C __STDC_VERSION__
#  if (BASE_LANG_C < BASE_C_99)
#   error "When compiled as C, Base is C99 or higher only!"
#  endif
#  include <stdint.h>
#  include <inttypes.h>
#  ifndef BASE_RESTRICT_IMPL
#   define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_C
#  endif
#  if (BASE_LANG_C >= BASE_C_11)
#   define BASE_STATIC_ASSERT(boolean, msg)	_Static_assert(boolean, msg)
#   define BASE_ALIGNAS(as)			_Alignas(as)
#   define BASE_ALIGNOF(of)			_Alignof(of)
#  else
    /* Nil macros. */
#   define BASE_STATIC_ASSERT(boolean, msg)
#   define BASE_STATIC_ASSERT_IS_NIL
#   define BASE_ALIGNAS(as)
#   define BASE_ALIGNAS_IS_NIL
#   define BASE_ALIGNOF(of)
#   define BASE_ALIGNOF_IS_NIL
#  endif
# else  /* __STDC_VERSION__ not defined. */
  /* We use C99 features, so if we can't detect a C version just pray
   * everything doesn't break.
   */
#  define BASE_LANG_C	0L
#  ifndef BASE_RESTRICT_IMPL
#   define BASE_RESTRICT_IMPL 0
#  endif
  /* Nil macros. */
#  define BASE_STATIC_ASSERT(boolean, msg)
#  define BASE_STATIC_ASSERT_IS_NIL
#  define BASE_ALIGNAS(as)
#  define BASE_ALIGNAS_IS_NIL
#  define BASE_ALIGNOF(of)
#  define BASE_ALIGNOF_IS_NIL
# endif /* ! #ifdef __STDC_VERSION__ */
#endif /* ! #ifdef __cplusplus */

/* What do compound literals look like? */
#if   defined(BASE_LANG_CPP)
# define BASE_COMPOUND_LITERAL(type, ...) type{__VA_ARGS__}
#elif defined(BASE_LANG_C)
# if (BASE_LANG_C > 0L && BASE_LANG_C < BASE_C_99)
#  error "We need C99 style literals!"
# endif
# define BASE_COMPOUND_LITERAL(type, ...) (type){__VA_ARGS__}
#else
# error "C++ and C both undefined!"
#endif

/* Can we restrict pointers? C++ or C99 style? */
#ifndef BASE_RESTRICT_IMPL
# error "BASE_RESTRICT_IMPL undefined!"
#endif
#if ((BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_CPP) || \
     (defined(BASE_COMPILER_MSVC) && BASE_COMPILER_MSVC >= 1900))
# define BASE_RESTRICT __restrict /* C++/MSVC compatible restrict. */
#elif (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_C)
# define BASE_RESTRICT restrict   /* C99-specified restrict. */
#else
# define BASE_RESTRICT /* We don't have restrict. */
# define BASE_RESTRICT_IS_NIL
#endif
/* Cleanup restrict implementation. */
#undef BASE_RESTRICT_IMPL_C
#undef BASE_RESTRICT_IMPL_CPP
#undef BASE_RESTRICT_IMPL

/* Symbol Visibility, Export/Import Macros */
#if defined(BASE_COMPILER_UNKNOWN)
# warning "Compiler unknown: NIL-ing import/export attributes!"
# define BASE_EXPORT
# define BASE_EXPORT_IS_NIL
# define BASE_IMPORT
# define BASE_IMPORT_IS_NIL
#elif (defined(BASE_COMPILER_GCC) || defined(BASE_COMPILER_CLANG))
# if defined(BASE_OS_UNIXLIKE)
#  define BASE_EXPORT __attribute__ ((visibility ("default")))
#  define BASE_IMPORT BASE_EXPORT
# elif defined(BASE_OS_WINDOWS)
#  define BASE_EXPORT __attribute__ ((dllexport))
#  define BASE_IMPORT __attribute__ ((dllimport))
# else
#  error "GCC/Clang support Unixlikes and Windows."
# endif
#elif defined(BASE_COMPILER_MSVC)
# ifndef BASE_OS_WINDOWS
#  error "This is only for Windows OS's."
# endif
# define BASE_EXPORT __declspec(dllexport)
# define BASE_IMPORT __declspec(dllimport)
#else
# error "Unaccounted for compiler."
#endif

#define BASE_INLINE		static inline
#define BASE_STRINGIFY_IMPL(s)	#s
#define BASE_STRINGIFY(s)	BASE_STRINGIFY_IMPL(s)

#ifdef BASE_EXTERN_STATIC_LIB /* Base is being built, or imported as a static library. */
# define BASE_API /* Nil */
# define BASE_API_IS_NIL
#else
# ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB
#  define BASE_API BASE_EXPORT
#  define BASE_API_IS_EXPORT
#  ifdef BASE_EXPORT_IS_NIL
#   define BASE_API_IS_NIL
#  endif
# else /* Assume that Base is being imported as a dynamic library. */
#  define BASE_API BASE_IMPORT
#  define BASE_API_IS_IMPORT
#  ifdef BASE_IMPORT_IS_NIL
#   define BASE_API_IS_NIL
#  endif
# endif /* ! #ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB */
#endif /* ! #ifdef BASE_EXTERN_STATIC_LIB */

/* We conditionally include `errors.h' below, but `errors.h' depends upon some macros defined here in `macros.h'.
 * The following macros must remain at the end of `macros.h' to avoid circular dependence problems.
 */

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
# include <unistd.h>
# include "errors.h"
# define BASE_OPENBSD_PLEDGE(promises, execpromises) Base_assert_msg(!pledge(promises, execpromises), "Failed to pledge()!\n")
# define BASE_OPENBSD_UNVEIL(path    , permissions)  Base_assert_msg(!unveil(path    , permissions ), "Failed to unveil()!\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
# define BASE_OPENBSD_PLEDGE(promises, execpromises) /* Nil */
# define BASE_OPENBSD_UNVEIL(path    , permissions)  /* Nil */
#endif /* ! #ifdef __OpenBSD__ */

#endif /* ! #ifndef BASE_MACROS_H */
