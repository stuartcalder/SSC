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

SHIM_API int
shim_get_file_size (Shim_File_t const, size_t *);

SHIM_API size_t
shim_enforce_get_file_size (Shim_File_t const);

SHIM_API int
shim_get_filepath_size (char const * SHIM_RESTRICT, size_t * SHIM_RESTRICT);

SHIM_API size_t
shim_enforce_get_filepath_size (char const *);

SHIM_API bool
shim_filepath_exists (char const *);

SHIM_API void
shim_enforce_filepath_existence (char const * SHIM_RESTRICT, bool const);

SHIM_API int
shim_open_filepath (char const * SHIM_RESTRICT, bool const, Shim_File_t * SHIM_RESTRICT);

SHIM_API Shim_File_t
shim_enforce_open_filepath (char const * SHIM_RESTRICT, bool const);

SHIM_API int
shim_create_filepath (char const * SHIM_RESTRICT, Shim_File_t * SHIM_RESTRICT);

SHIM_API Shim_File_t
shim_enforce_create_filepath (char const *);

SHIM_API int
shim_close_file (Shim_File_t const);

SHIM_API void
shim_enforce_close_file (Shim_File_t const);

SHIM_API int
shim_set_file_size (Shim_File_t const, size_t const);

SHIM_API void
shim_enforce_set_file_size (Shim_File_t const, size_t const);

SHIM_END_DECLS

#undef NO_DISCARD_API_

#endif // ~ SHIM_FILES_H
