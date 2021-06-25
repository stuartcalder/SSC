#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#include <stdarg.h>

/* Flags to indicate support for restricting pointers. */
#define BASE_RESTRICT_IMPL_C	(1u << 0) /*   restrict */
#define BASE_RESTRICT_IMPL_CPP	(1u << 1) /* __restrict */

/* Operating System Macros */
#if    defined(__APPLE__) && defined(__MACH__)
#	define BASE_OS_MAC
#endif /* ~ if defined (__APPLE__) and defined (__MACH__) */

/* Define the BSDs, GNU/Linux, and MacOS as UNIX-like operating systems. */
#if    defined(BASE_OS_MAC)   || \
       defined(__Dragonfly__) || \
       defined(__FreeBSD__)   || \
       defined(__gnu_linux__) || \
       defined(__NetBSD__)    || \
       defined(__OpenBSD__)
#	define BASE_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif  defined(_WIN32)
#	define BASE_OS_WINDOWS
#	ifndef BASE_RESTRICT_IMPL
#		define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_CPP
#	endif
#	ifdef _WIN64
#		define BASE_OS_WIN64
#	else
#		define BASE_OS_WIN32
#	endif /* ~ ifdef _WIN64 */
#else
#	error "Unsupported."
#endif /* ~ if defined (unixlike os's ...) */

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
#	include <unistd.h>
#	include "errors.h"
#	define BASE_OPENBSD_PLEDGE(promises, execpromises) Base_assert_msg(!pledge(promises, execpromises), "Failed to pledge()\n")
#	define BASE_OPENBSD_UNVEIL(path    , permissions)  Base_assert_msg(!unveil(path    , permissions ), "Failed to unveil()\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
#	define BASE_OPENBSD_PLEDGE(promises, execpromises) /* Nil */
#	define BASE_OPENBSD_UNVEIL(path    , permissions)  /* Nil */
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
#	define BASE_LANG_CPP	__cplusplus
#	define BASE_LANG		BASE_LANG_CPP
/* C++ doesn't support "restrict". Use the non-standard "__restrict" compiler extension. */
#	ifndef BASE_RESTRICT_IMPL
#		define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_CPP
#	endif
/* Use C function name mangling. */
#	define BASE_BEGIN_DECLS extern "C" {
#	define BASE_END_DECLS   }
/* If we're using C++11 or above, support "static_assert", "alignas", and "alignof". */
#	if (BASE_LANG_CPP < BASE_CPP_11)
#		error "We need to have C++11 at minimum."
#	endif
#	define BASE_HAS_STATIC_ASSERT
#	define BASE_HAS_ALIGNAS
#	define BASE_HAS_ALIGNOF
#	define BASE_STATIC_ASSERT(boolean, message) static_assert(boolean, message)
#	define BASE_ALIGNAS(v) alignas(v)
#	define BASE_ALIGNOF(v) alignof(v)
#else
#	define BASE_LANG BASE_LANG_C
#	define BASE_BEGIN_DECLS /* Nil */
#	define BASE_END_DECLS   /* Nil */
#	ifdef __STDC_VERSION__
#		define BASE_LANG_C	__STDC_VERSION__
		/* We need at least C99 to support the "restrict" qualifier . */
#		if    (BASE_LANG_C >= BASE_C_99)
#			ifndef BASE_RESTRICT_IMPL
#				define BASE_RESTRICT_IMPL BASE_RESTRICT_IMPL_C
#			endif
#		else
#			ifndef BASE_RESTRICT_IMPL
#				define BASE_RESTRICT_IMPL 0u
#			endif
#		endif
#		if    (BASE_LANG_C >= BASE_C_11)
#			include <inttypes.h>
#			include <stdalign.h>
#			define BASE_HAS_STATIC_ASSERT
#			define BASE_HAS_ALIGNAS
#			define BASE_HAS_ALIGNOF
#			define BASE_STATIC_ASSERT(boolean, msg) _Static_assert(boolean, msg)
#			define BASE_ALIGNAS(align_as) _Alignas(align_as)
#			define BASE_ALIGNOF(align_of) _Alignof(align_of)
#		else
#			define BASE_STATIC_ASSERT(boolean, msg)	/* Nil */
#			define BASE_ALIGNAS(align_as)		/* Nil */
#			define BASE_ALIGNOF(align_of)		/* Nil */
#		endif
#	else
#		define BASE_LANG_C	0L
#		ifndef BASE_RESTRICT_IMPL
#			define BASE_RESTRICT_IMPL 0u
#		endif
#		define BASE_STATIC_ASSERT(boolean, msg) /* Nil */
#		define BASE_ALIGNAS(align_as)		/* Nil */
#		define BASE_ALIGNOF(align_of)		/* Nil */
#	endif /* ~ #ifdef __STDC_VERSION__ */
#endif

#ifndef BASE_RESTRICT_IMPL
#	error "BASE_RESTRICT_IMPL undefined!"
#endif
#if    (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_CPP)
#	define BASE_HAS_RESTRICT
#	define BASE_RESTRICT __restrict
#elif  (BASE_RESTRICT_IMPL & BASE_RESTRICT_IMPL_C)
#	define BASE_HAS_RESTRICT
#	define BASE_RESTRICT restrict
#else
#	define BASE_RESTRICT /* Nil */
#endif

/* Symbol Visibility, Export/Import Macros */
#if    defined(BASE_OS_UNIXLIKE)
#	if    defined(__GNUC__) && (__GNUC__ >= 4)
#		define BASE_EXPORT __attribute__ ((visibility ("default")))
#		define BASE_IMPORT BASE_EXPORT
#	else
#		define BASE_EXPORT /* Nil */
#		define BASE_IMPORT /* Nil */
#	endif /* ~ if defined (__GNUC__) and (__GNUC__ >= 4) */
#elif  defined(BASE_OS_WINDOWS) || defined(__CYGWIN__)
#	ifdef __GNUC__
#		define BASE_EXPORT __attribute__ ((dllexport))
#		define BASE_IMPORT __attribute__ ((dllimport))
#	else
#		define BASE_EXPORT __declspec(dllexport)
#		define BASE_IMPORT __declspec(dllimport)
#	endif /* ~ ifdef __GNUC__ */
#else
#	error "Unsupported operating system."
#endif

#define BASE_INLINE static inline
#define BASE_STRINGIFY_IMPL(s)  #s
#define BASE_STRINGIFY(s)	BASE_STRINGIFY_IMPL(s)

#ifdef BASE_EXTERN_STATIC_LIB
#	define BASE_API /* Nil */
#else
#	ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB
#		define BASE_API BASE_EXPORT
#	else /* Assume that Base is being imported as a dynamic library. */
#		define BASE_API BASE_IMPORT
#	endif /* ~ ifdef BASE_EXTERN_BUILD_DYNAMIC_LIB */
#endif /* ~ ifdef BASE_EXTERN_STATIC_LIB */

#endif /* ~ ifndef BASE_MACROS_H */
