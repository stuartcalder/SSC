/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_MACROS_H
#define SHIM_MACROS_H

/* Operating System Macros */

#if    defined (__APPLE__) && defined (__MACH__)
#	ifdef SHIM_OS_OSX
#		error 'SHIM_OS_OSX already defined'
#	endif
#	define SHIM_OS_OSX
#endif

/* Define the BSDs, GNU/Linux, and Mac OSX as UNIX-like operating systems. */
#if    defined (__OpenBSD__)   || \
       defined (__FreeBSD__)   || \
       defined (__NetBSD__)    || \
       defined (__gnu_linux__) || \
       defined (SHIM_OS_OSX)   || \
       defined (__Dragonfly__)
#	ifdef SHIM_OS_UNIXLIKE
#		error 'SHIM_OS_UNIXLIKE already defined'
#	endif
#	define SHIM_OS_UNIXLIKE
/* Define MS Windows, naming scheme consistent with the above. */
#elif  defined (_WIN32)
#	ifdef SHIM_OS_WINDOWS
#		error 'SHIM_OS_WINDOWS already defined'
#	endif
#	define SHIM_OS_WINDOWS
#	ifdef _WIN64
#		ifdef SHIM_OS_WIN64
#			error 'SHIM_OS_WIN64 already defined'
#		endif
#		define SHIM_OS_WIN64
#	else
#		ifdef SHIM_OS_WIN32
#			error 'SHIM_OS_WIN32 already defined'
#		endif
#		define SHIM_OS_WIN32
#	endif
#else
#	error 'Unsupported OS'
#endif // ~ #if defined (unixlike_os's...)

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
#	if    defined (SHIM_OPENBSD_UNVEIL)
#		error 'SHIM_OPENBSD_UNVEIL already defined'
#	elif  defined (SHIM_OPENBSD_PLEDGE)
#		error 'SHIM_OPENBSD_PLEDGE already defined'
#	endif

#	include <unistd.h>
#	include "errors.h"

#	define SHIM_OPENBSD_UNVEIL(path, permissions) \
		if( unveil( path, permissions ) != 0 ) \
			SHIM_ERRX ("Failed to unveil()\n")
#	define SHIM_OPENBSD_PLEDGE(promises, execpromises) \
		if( pledge( promises, execpromises ) != 0 ) \
			SHIM_ERRX ("Failed to pledge()\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems. */
#	define SHIM_OPENBSD_UNVEIL(path, permission)		/* Nil */
#	define SHIM_OPENBSD_PLEDGE(promises, execpromises)	/* Nil */
#endif // ~ #ifdef __OpenBSD__

/* Simplification Macros */

#ifdef __cplusplus
#	if    (__cplusplus < 201100L)
#		error "Need at least C++11"
#	endif
#	define SHIM_BEGIN_DECLS extern "C" {
#	define SHIM_END_DECLS   }
#	define SHIM_STATIC_ASSERT(boolean, message) \
		static_assert (boolean, message)
#	define SHIM_ALIGNAS(align_to) \
		alignas(align_to)
#	define SHIM_ALIGNOF(align_of) \
		alignof(align_of)
#	define SHIM_RESTRICT __restrict
#else
#	define SHIM_BEGIN_DECLS /* null macro */
#	define SHIM_END_DECLS   /* null macro */
#	if   !defined (__STDC_VERSION__)
#		error "C standard not detected with the __STDC_VERSION__ macro."
#	else
#		if    (__STDC_VERSION__ < 201112L)
#			error "At minimum, we need C11."
#		else
#			include <inttypes.h>
#			include <stdalign.h>
#			define SHIM_STATIC_ASSERT(boolean, message) \
				_Static_assert (boolean, message)
#			define SHIM_ALIGNAS(align_to) \
				_Alignas(align_to)
#			define SHIM_ALIGNOF(align_of) \
				_Alignof(align_of)
#			define SHIM_RESTRICT restrict
#		endif
#	endif
#endif

/* Symbol Visibility, Export/Import Macros */
#if    defined (SHIM_OS_WINDOWS) || defined (__CYGWIN__)
#	ifdef __GNUC__
#		define SHIM_EXPORT_SYMBOL __attribute__ ((dllexport))
#		define SHIM_IMPORT_SYMBOL __attribute__ ((dllimport))
#	else
#		define SHIM_EXPORT_SYMBOL __declspec(dllexport)
#		define SHIM_IMPORT_SYMBOL __declspec(dllimport)
#	endif
#elif  defined (SHIM_OS_UNIXLIKE)
#	if    defined (__GNUC__) && (__GNUC__ >= 4)
#		define SHIM_EXPORT_SYMBOL __attribute__ ((visibility ("default")))
#		define SHIM_IMPORT_SYMBOL __attribute__ ((visibility ("default")))
#	else
#		define SHIM_EXPORT_SYMBOL /* Nil */
#		define SHIM_IMPORT_SYMBOL /* Nil */
#	endif
#else
#	error "Unsupported operating system."
#endif

#ifdef SHIM_EXT_BUILD
#	define SHIM_API SHIM_EXPORT_SYMBOL
#else
#	define SHIM_API SHIM_IMPORT_SYMBOL
#endif


#endif // ~ SHIM_MACROS_H
