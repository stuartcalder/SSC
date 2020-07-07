/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_MACROS_H
#define SHIM_MACROS_H

#if    defined (SHIM_PUBLIC) || defined (SHIM_PRIVATE)
#	error 'Symbol Macro Already Defined'
#endif
/* Symbol visibility macros */
#if defined (SHIM_BUILD_STATIC) || defined (SHIM_IMPORT_STATIC)
#	define  SHIM_PUBLIC
#	define  SHIM_PRIVATE
#else
#	if    defined (_WIN32) || defined (__CYGWIN__)
#		ifdef	SHIM_BUILD_DLL
#			ifdef	__GNUC__
#				define	SHIM_PUBLIC __attribute__ ((dllexport))
#			else
#				define	SHIM_PRIVATE __declspec(dllexport)
#			endif // ~ #ifdef __GNUC__
#		else
#			ifdef	__GNUC__
#				define	SHIM_PUBLIC __attribute__ ((dllimport))
#			else
#				define	SHIM_PUBLIC __declspec(dllimport)
#			endif // ~ #ifdef __GNUC__
#		endif // ~ #ifdef SHIM_BUILD_DLL
#		define	SHIM_PRIVATE
#	else
#		if    defined (__GNUC__) && (__GNUC__ >= 4)
#			define	SHIM_PUBLIC  __attribute__ ((visibility ("default")))
#			define	SHIM_PRIVATE __attribute__ ((visibility ("hidden")))
#		else
#			define	SHIM_PUBLIC
#			define	SHIM_PRIVATE
#		endif // ~ #if defined (__GNUC__) && (__GNUC__ >= 4)
#	endif // ~ #if defined (_WIN32) || defined (__CYGWIN__)
#endif // ~ #if defined (SHIM_BUILD_STATIC) || defined (SHIM_IMPORT_STATIC)

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
       defined (SHIM_OS_OSX)
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

/* Compile-Time-Constant short-hand macros. */

#ifdef SHIM_RESTRICT
#	error 'SHIM_RESTRICT already defined'
#endif
#ifdef __cplusplus
#	define SHIM_RESTRICT __restrict
#else
#	define SHIM_RESTRICT restrict
#endif

/* OpenBSD-specific mitigations */
#ifdef	__OpenBSD__
#	if    defined (SHIM_OPENBSD_UNVEIL)
#		error 'SHIM_OPENBSD_UNVEIL already defined'
#	elif  defined (SHIM_OPENBSD_PLEDGE)
#		error 'SHIM_OPENBSD_PLEDGE already defined'
#	endif
#	include "errors.h"

#	include <unistd.h>
#	define SHIM_OPENBSD_UNVEIL(path,permissions) \
		if( unveil( path, permissions ) != 0 ) \
			SHIM_ERRX ("Failed to unveil()\n")
#	define SHIM_OPENBSD_PLEDGE(promises,execpromises) \
		if( pledge( promises, execpromises ) != 0 ) \
			SHIM_ERRX ("Failed to pledge()\n")
#else
/* These macros define to nothing on non-OpenBSD operating systems.
 */
#	define SHIM_OPENBSD_UNVEIL(null0,null1)
#	define SHIM_OPENBSD_PLEDGE(null0,null1)
#endif // ~ #ifdef __OpenBSD__

/* Simplification Macros */
#if    defined (SHIM_MACRO_SHIELD)
#	error 'SHIM_MACRO_SHIELD already defined'
#elif  defined (SHIM_MACRO_SHIELD_EXIT)
#	error 'SHIM_MACRO_SHIELD_EXIT already defined'
#endif
#define SHIM_MACRO_SHIELD	do {
#define SHIM_MACRO_SHIELD_EXIT	} while(0)

#ifdef __cplusplus
#	define SHIM_BEGIN_DECLS extern "C" {
#	define SHIM_END_DECLS   }

#	define SHIM_STATIC_CAST_VALUE_TO(value,type)		static_cast<type>(value)
#	define SHIM_REINTERPRET_CAST_VALUE_TO(value,type)	reinterpret_cast<type>(value)
#else
#	define SHIM_BEGIN_DECLS /* null macro */
#	define SHIM_END_DECLS   /* null macro */
#	define SHIM_STATIC_CAST_VALUE_TO(value,type)		((type)value)
#	define SHIM_REINTERPRET_CAST_VALUE_TO(value,type)	((type)value)
#endif

#endif // ~ SHIM_MACROS_H
