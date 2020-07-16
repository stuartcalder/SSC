/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_ERRORS_H
#define SHIM_ERRORS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "macros.h"

#define SHIM_ERR_STR_ALLOC_FAILURE "Error: Generic Allocation Failure!\n"

#if    defined (SHIM_OS_UNIXLIKE)
/* Pretty sure all the Unixlike platforms have err.h
 */
#	include <err.h>

/*	SHIM_ERRX_CODE(exit_code, message_string, additional params...)
 */
#	define SHIM_ERRX_CODE(code, ...) \
		errx( code, __VA_ARGS__ )

#elif  defined (SHIM_OS_WINDOWS)
#	include <windows.h>

#	define SHIM_ERRX_CODE(code, ...) \
		SHIM_MACRO_SHIELD \
			fprintf( stderr, __VA_ARGS__ ); \
			exit( code ); \
		SHIM_MACRO_SHIELD_EXIT

#else
#	error "Unsupported operating system."
#endif // ~ #if defined (SHIM_OS_UNIXLIKE) ...

#define SHIM_ERRX(...) \
	SHIM_ERRX_CODE (EXIT_FAILURE, __VA_ARGS__)

#endif // ~ SHIM_ERRORS_H
