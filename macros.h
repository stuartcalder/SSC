/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SHIM_MACROS_H
#define SHIM_MACROS_H

/* Flags to indicate support for restricting pointers. */
#define SHIM_IMPL_C_RESTRICT_FLAG	0b00000001U
#define SHIM_IMPL_CPP_RESTRICT_FLAG	0b00000010U

/* Operating System Macros */
#if    defined (__APPLE__) && defined (__MACH__)
#	define SHIM_OS_MAC
#endif /* ~ if defined (__APPLE__) and defined (__MACH__) */

/* Define the BSDs, GNU/Linux, and MacOS as UNIX-like operating systems. */
#if    defined (__Dragonfly__) || \
       defined (__FreeBSD__)   || \
       defined (__NetBSD__)    || \
       defined (__OpenBSD__)   || \
       defined (__gnu_linux__) || \
       defined (SHIM_OS_MAC)
#	define SHIM_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif  defined (_WIN32)
#	define SHIM_OS_WINDOWS
#	ifndef SHIM_IMPL_RESTRICT
#		define SHIM_IMPL_RESTRICT SHIM_IMPL_CPP_RESTRICT_FLAG
#	endif
#	ifdef _WIN64
#		define SHIM_OS_WIN64
#	else
#		define SHIM_OS_WIN32
#	endif /* ~ ifdef _WIN64 */
#else
#	error "Unsupported OS"
#endif /* ~ if defined (unixlike os's ...) */

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
#	include <unistd.h>
#	include "errors.h"

#	define SHIM_OPENBSD_PLEDGE(promises, execpromises) \
		if( pledge( promises, execpromises ) ) \
			SHIM_ERRX ("Failed to pledge()\n")
#	define SHIM_OPENBSD_UNVEIL(path, permissions) \
		if( unveil( path, permissions ) ) \
			SHIM_ERRX ("Failed to unveil()\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
#	define SHIM_OPENBSD_PLEDGE(promises, execpromises) /* Nil */
#	define SHIM_OPENBSD_UNVEIL(path    , permissions ) /* Nil */
#endif /* ~ ifdef __OpenBSD__ */

/* Simplification Macros */

#ifdef __cplusplus
/* C++ doesn't support "restrict". Use the non-standard "__restrict" compiler extension. */
#	ifndef SHIM_IMPL_RESTRICT
#		define SHIM_IMPL_RESTRICT SHIM_IMPL_CPP_RESTRICT_FLAG
#	endif
/* Use C function name mangling. */
#	define SHIM_BEGIN_DECLS extern "C" {
#	define SHIM_END_DECLS   }
/* If we're using C++11 or above, support "static_assert", "alignas", and "alignof". */
#	if    (__cplusplus >= 201100L)
#		define SHIM_HAS_STATIC_ASSERT
#		define SHIM_HAS_ALIGNAS
#		define SHIM_HAS_ALIGNOF
#		define SHIM_STATIC_ASSERT(boolean, message) static_assert(boolean, message)
#		define SHIM_ALIGNAS(align_as) alignas(align_as)
#		define SHIM_ALIGNOF(align_of) alignof(align_of)
#	else
#		define SHIM_STATIC_ASSERT(boolean, message)	/* Nil */
#		define SHIM_ALIGNAS(align_as)			/* Nil */
#		define SHIM_ALIGNOF(align_of)			/* Nil */
#	endif
#else
#	define SHIM_BEGIN_DECLS /* Nil */
#	define SHIM_END_DECLS   /* Nil */
#	ifdef __STDC_VERSION__
		/* We need at least C99 to support the "restrict" qualifier . */
#		if    (__STDC_VERSION__ >= 199901L)
#			ifndef SHIM_IMPL_RESTRICT
#				define SHIM_IMPL_RESTRICT SHIM_IMPL_C_RESTRICT_FLAG
#			endif
#		else
#			ifndef SHIM_IMPL_RESTRICT
#				define SHIM_IMPL_RESTRICT 0
#			endif
#		endif
#		if    (__STDC_VERSION__ >= 201112L)
#			include <inttypes.h>
#			include <stdalign.h>
#			define SHIM_HAS_STATIC_ASSERT
#			define SHIM_HAS_ALIGNAS
#			define SHIM_HAS_ALIGNOF
#			define SHIM_STATIC_ASSERT(boolean, msg) _Static_assert(boolean, msg)
#			define SHIM_ALIGNAS(align_as) _Alignas(align_as)
#			define SHIM_ALIGNOF(align_of) _Alignof(align_of)
#		else
#			define SHIM_STATIC_ASSERT(boolean, msg)	/* Nil */
#			define SHIM_ALIGNAS(align_as)		/* Nil */
#			define SHIM_ALIGNOF(align_of)		/* Nil */
#		endif
#	else
#		ifndef SHIM_IMPL_RESTRICT
#			define SHIM_IMPL_RESTRICT 0
#		endif
#		define SHIM_STATIC_ASSERT(boolean, msg) /* Nil */
#		define SHIM_ALIGNAS(align_as)		/* Nil */
#		define SHIM_ALIGNOF(align_of)		/* Nil */
#	endif /* ~ #ifdef __STDC_VERSION__ */
#endif

#ifndef SHIM_IMPL_RESTRICT
#	error "SHIM_IMPL_RESTRICT undefined!"
#endif
#if    (SHIM_IMPL_RESTRICT & SHIM_IMPL_CPP_RESTRICT_FLAG)
#	define SHIM_HAS_RESTRICT
#	define SHIM_RESTRICT __restrict
#elif  (SHIM_IMPL_RESTRICT & SHIM_IMPL_C_RESTRICT_FLAG)
#	define SHIM_HAS_RESTRICT
#	define SHIM_RESTRICT restrict
#else
#	define SHIM_RESTRICT /* Nil */
#endif

/* Symbol Visibility, Export/Import Macros */
#if    defined (SHIM_OS_UNIXLIKE)
#	if    defined (__GNUC__) && (__GNUC__ >= 4)
#		define SHIM_EXPORT_SYMBOL __attribute__ ((visibility ("default")))
#		define SHIM_IMPORT_SYMBOL SHIM_EXPORT_SYMBOL
#	else
#		define SHIM_EXPORT_SYMBOL /* Nil */
#		define SHIM_IMPORT_SYMBOL /* Nil */
#	endif /* ~ if defined (__GNUC__) and (__GNUC__ >= 4) */
#elif  defined (SHIM_OS_WINDOWS) || defined (__CYGWIN__)
#	ifdef __GNUC__
#		define SHIM_EXPORT_SYMBOL __attribute__ ((dllexport))
#		define SHIM_IMPORT_SYMBOL __attribute__ ((dllimport))
#	else
#		define SHIM_EXPORT_SYMBOL __declspec(dllexport)
#		define SHIM_IMPORT_SYMBOL __declspec(dllimport)
#	endif /* ~ ifdef __GNUC__ */
#else
#	error "Unsupported operating system."
#endif

#ifdef SHIM_EXT_STATIC_LIB
#	define SHIM_API /* Nil */
#else
#	ifdef SHIM_EXT_BUILD_DYNAMIC_LIB
#		define SHIM_API SHIM_EXPORT_SYMBOL
#	else /* Assume that Shim is being imported as a dynamic library. */
#		define SHIM_API SHIM_IMPORT_SYMBOL
#	endif /* ~ ifdef SHIM_EXT_BUILD_DYNAMIC_LIB */
#endif /* ~ ifdef SHIM_EXT_STATIC_LIB */

#endif /* ~ ifndef SHIM_MACROS_H */
