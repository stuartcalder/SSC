/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_FILES_H
#define SHIM_FILES_H

#ifdef __cplusplus
#	define NO_DISCARD_API_ [[nodiscard]] SHIM_API
#else
#	define NO_DISCARD_API_ SHIM_API
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
/* On Unix-like systems, files are managed through handles, represented by integers. */
typedef int	Shim_File_t;
#	define SHIM_NULL_FILE -1
#elif  defined (SHIM_OS_WINDOWS)
#	include <windows.h>
/* On Windows systems, files are managed through HANDLEs. */
typedef HANDLE	Shim_File_t;
#	define SHIM_NULL_FILE INVALID_HANDLE_VALUE
#else
#	error "Unsupported operating system."
#endif // ~ SHIM_OS_UNIXLIKE

SHIM_BEGIN_DECLS
/* shim_file_size (Shim_File_t)
 *	Return the size of the file; assumes there is such a file and we may access it. */
NO_DISCARD_API_ size_t
shim_file_size (Shim_File_t const);
/* shim_filepath_size (filepath_str)
 * 	Return the size of the file at the path; assumes there is such a file and we may access it. */
NO_DISCARD_API_ size_t
shim_filepath_size (char const *);
/* shim_filepath_exists (filepath_str)
 * 	Return true if there is a file at the path; false otherwise. */
NO_DISCARD_API_ bool
shim_filepath_exists (char const *);
/* shim_enforce_filepath_existence (filepath_str, force)
 * Kill the program when existence or non-existence of a file is not satisfied when required. */
SHIM_API void
shim_enforce_filepath_existence (char const * SHIM_RESTRICT,
				 bool const);
/* shim_open_existing_filepath (filepath_str, readonly)
 * 	Return a Shim_File_t, that manages the file at the path. Assumes file exists there. */
NO_DISCARD_API_ Shim_File_t
shim_open_existing_filepath (char const * SHIM_RESTRICT,
			     bool const);
/* shim_create_filepath (filepath_str)
 * 	Return a Shim_File_t, that manages a newly created file at the path. Assumes there is not a file already there. */
NO_DISCARD_API_ Shim_File_t
shim_create_filepath (char const *);
/* shim_close_file (Shim_File_t)
 * Close the file managed by the Shim_File_t. */
SHIM_API void 
shim_close_file (Shim_File_t const);
/* shim_set_file_size (Shim_File_t, desired_size)
 * Extends or truncates the file managed by Shim_File_t. */
SHIM_API void 
shim_set_file_size (Shim_File_t const, size_t const);
SHIM_END_DECLS

#undef NO_DISCARD_API_

#endif // ~ SHIM_FILES_H
