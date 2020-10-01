/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_FILES_H
#define SHIM_FILES_H

#ifdef __cplusplus
#	define NO_DISCARD_ [[nodiscard]]
#else
#	define NO_DISCARD_ /* null macro */
#endif

#include "macros.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#if    defined (SHIM_OS_UNIXLIKE)
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <unistd.h>
#	include <fcntl.h>
#elif  defined (SHIM_OS_WINDOWS)
#	include <windows.h>
#else
#	error "Unsupported operating system."
#endif // ~ SHIM_OS_UNIXLIKE

#if    defined (SHIM_OS_UNIXLIKE)
typedef int	Shim_File_t;
#	define SHIM_NULL_FILE -1

#elif  defined (SHIM_OS_WINDOWS)
typedef HANDLE	Shim_File_t;
#	define SHIM_NULL_FILE INVALID_HANDLE_VALUE

#else
#	error "Unsupported operating system."
#endif // ~ #if defined (SHIM_OS_UNIXLIKE) ...

SHIM_BEGIN_DECLS

NO_DISCARD_ size_t
shim_file_size (Shim_File_t const);

NO_DISCARD_ bool
shim_filepath_exists (char const *);

void
shim_enforce_filepath_existence (char const * SHIM_RESTRICT,
				 bool const);

NO_DISCARD_ Shim_File_t
shim_open_existing_filepath (char const * SHIM_RESTRICT,
			     bool const);

NO_DISCARD_ Shim_File_t
shim_create_filepath (char const *);

void
shim_close_file (Shim_File_t const);

void
shim_set_file_size (Shim_File_t const, size_t const);

SHIM_END_DECLS

#undef NO_DISCARD_

#endif // ~ SHIM_FILES_H
