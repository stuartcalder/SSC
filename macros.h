#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#include <stdarg.h>

/* Flags to indicate support for restricting pointers. */
#define BASE_RESTRICT_IMPL_C	(1u << 0) /*   restrict */
#define BASE_RESTRICT_IMPL_CPP	(1u << 1) /* __restrict */

/* Operating System Macros */
#if defined(__APPLE__) && defined(__MACH__)
#  define BASE_OS_MAC
#endif /* ~ if defined (__APPLE__) and defined (__MACH__) */

/* Define the BSDs, GNU/Linux, and MacOS as UNIX-like operating systems. */
#if defined(BASE_OS_MAC)   || \
    defined(__Dragonfly__) || \
    defined(__FreeBSD__)   || \
    defined(__gnu_linux__) || \
    defined(__NetBSD__)    || \
    defined(__OpenBSD__)
#  define BASE_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif defined(_WIN32) || defined(__CYGWIN__)
#  define BASE_OS_WINDOWS
#  ifndef BASE_RESTRICT_IMPL
#    define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_CPP
#  endif
#  ifdef _WIN64
#    define BASE_OS_WIN64
#  else
#    define BASE_OS_WIN32
#  endif /* ~ ifdef _WIN64 */
#else
#	error "Unsupported."
#endif /* ~ if defined (unixlike os's ...) */

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
#  include <unistd.h>
#  include "errors.h"
#  define BASE_OPENBSD_PLEDGE(promises, execpromises) Base_assert_msg(!pledge(promises, execpromises), "Failed to pledge()\n")
#  define BASE_OPENBSD_UNVEIL(path    , permissions)  Base_assert_msg(!unveil(path    , permissions ), "Failed to unveil()\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
#  define BASE_OPENBSD_PLEDGE(promises, execpromises) /* Nil */
#  define BASE_OPENBSD_UNVEIL(path    , permissions)  /* Nil */
#endif /* ~ ifdef __OpenBSD__ */

#define BASE_C_89        199409L
#define BASE_C_99        199901L
#define BASE_C_11        201112L
#define BASE_C_17        201710L
#define BASE_CPP_11      201103L
#define BASE_CPP_14      201402L
#define BASE_CPP_17      201703L
#define BASE_CPP_20      202002L

/* Simplification Macros */
#ifdef __cplusplus
#  define BASE_LANG_CPP __cplusplus
#  define BASE_LANG     BASE_LANG_CPP
/* C++ doesn't support "restrict". Use the non-standard "__restrict" compiler extension. */
#  ifndef BASE_RESTRICT_IMPL
#    define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_CPP
#  endif
/* Use C function name mangling. */
#  define BASE_BEGIN_DECLS extern "C" {
#  define BASE_END_DECLS   }
/* If we're using C++11 or above, support "static_assert", "alignas", and "alignof". */
#  if (BASE_LANG_CPP < BASE_CPP_11)
#    error "Base is C++11 or higher only!"
#  endif
#  define BASE_STATIC_ASSERT(boolean, message) static_assert(boolean, message)
#  define BASE_ALIGNAS(v) alignas(v)
#  define BASE_ALIGNOF(v) alignof(v)
#else /* Not C++. We are using C. */
#  define BASE_LANG BASE_LANG_C
#  define BASE_BEGIN_DECLS /* Nil */
#  define BASE_BEGIN_DECLS_IS_NIL
#  define BASE_END_DECLS   /* Nil */
#  define BASE_END_DECLS_IS_NIL
#  ifdef __STDC_VERSION__
#    define BASE_LANG_C __STDC_VERSION__
#    if (BASE_LANG_C >= BASE_C_99)
#      include <stdint.h>
#      include <inttypes.h>
       /* We need at least C99 to support the "restrict" qualifier . */
#      ifndef BASE_RESTRICT_IMPL
#        define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_C
#      endif
#    else
#      error "Base is C99 or higher only!"
#    endif
#    if (BASE_LANG_C >= BASE_C_11)
#      define BASE_STATIC_ASSERT(boolean, msg)	_Static_assert(boolean, msg)
#      define BASE_ALIGNAS(align_as)		_Alignas(align_as)
#      define BASE_ALIGNOF(align_of)		_Alignof(align_of)
#    else
       /* Nil macros. */
#      define BASE_STATIC_ASSERT(boolean, msg)
#      define BASE_STATIC_ASSERT_IS_NIL
#      define BASE_ALIGNAS(align_as)
#      define BASE_ALIGNAS_IS_NIL
#      define BASE_ALIGNOF(align_of)
#      define BASE_ALIGNOF_IS_NIL
#    endif
#  else
   /* We use C99 features, so if we can't detect a C version just pray
    * everything doesn't break.
    */
#    define BASE_LANG_C	0L
#    ifndef BASE_RESTRICT_IMPL
#      define BASE_RESTRICT_IMPL 0u
#    endif
     /* Nil macros. */
#    define BASE_STATIC_ASSERT(boolean, msg)
#    define BASE_STATIC_ASSERT_IS_NIL
#    define BASE_ALIGNAS(align_as)
#    define BASE_ALIGNAS_IS_NIL
#    define BASE_ALIGNOF(align_of)
#    define BASE_ALIGNOF_IS_NIL
#  endif /* ~ #ifdef __STDC_VERSION__ */
#endif

#ifndef BASE_RESTRICT_IMPL
#  error "BASE_RESTRICT_IMPL undefined!"
#endif
#if   (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_CPP)
#  define BASE_RESTRICT __restrict
#elif (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_C)
#  define BASE_RESTRICT restrict
#else
#  define BASE_RESTRICT /* Nil */
#  define BASE_RESTRICT_IS_NIL
#endif

/* Symbol Visibility, Export/Import Macros */
#if defined(BASE_OS_UNIXLIKE)
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define BASE_EXPORT __attribute__ ((visibility ("default")))
#    define BASE_IMPORT BASE_EXPORT
#  else
#    define BASE_EXPORT /* Nil */
#    define BASE_EXPORT_IS_NIL
#    define BASE_IMPORT /* Nil */
#    define BASE_IMPORT_IS_NIL
#  endif /* ~ if defined (__GNUC__) and (__GNUC__ >= 4) */
#elif defined(BASE_OS_WINDOWS)
#  ifdef __GNUC__
#    define BASE_EXPORT __attribute__ ((dllexport))
#    define BASE_IMPORT __attribute__ ((dllimport))
#  else
#    define BASE_EXPORT __declspec(dllexport)
#    define BASE_IMPORT __declspec(dllimport)
#  endif /* ~ ifdef __GNUC__ */
#else
#	error "Unsupported operating system."
#endif

#define BASE_INLINE static inline
#define BASE_STRINGIFY_IMPL(s) #s
#define BASE_STRINGIFY(s)      BASE_STRINGIFY_IMPL(s)

#ifdef BASE_EXTERN_STATIC_LIB
#  define BASE_API /* Nil */
#  define BSAE_API_IS_NIL
#else
#  ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB
#    define BASE_API BASE_EXPORT
#    define BASE_API_IS_EXPORT
#    ifdef BASE_EXPORT_IS_NIL
#      define BASE_API_IS_NIL
#    endif
#  else /* Assume that Base is being imported as a dynamic library. */
#    define BASE_API BASE_IMPORT
#    define BASE_API_IS_IMPORT
#    ifdef BASE_IMPORT_IS_NIL
#      define BASE_API_IS_NIL
#    endif
#  endif /* ~ ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB */
#endif /* ~ ifdef BASE_EXTERN_STATIC_LIB */

#endif /* ~ ifndef BASE_MACROS_H */
