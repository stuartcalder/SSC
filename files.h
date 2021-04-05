/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_FILES_H
#define SHIM_FILES_H

#include "errors.h"
#include "macros.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if    defined (SHIM_OS_UNIXLIKE)
#	define SHIM_FILES_INLINE_CLOSE_FILE
#	define SHIM_FILES_INLINE_SET_FILE_SIZE
#	include <fcntl.h>
#	include <unistd.h>
#	include <sys/stat.h>
#	include <sys/types.h>
/* On Unix-like systems, files are managed through handles, represented by integers. */
typedef int	Shim_File_t;
#	define SHIM_NULL_FILE (-1)
#elif  defined (SHIM_OS_WINDOWS)
#	include <windows.h>
/* On Windows systems, files are managed through HANDLEs. */
typedef HANDLE	Shim_File_t;
#	define SHIM_NULL_FILE INVALID_HANDLE_VALUE
#else
#	error "Unsupported operating system."
#endif /* ~ if defined (SHIM_OS_UNIXLIKE) or defined (SHIM_OS_WINDOWS) */

SHIM_BEGIN_DECLS

SHIM_API int
shim_get_file_size (Shim_File_t const, size_t * SHIM_RESTRICT);

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

#ifdef SHIM_FILES_INLINE_CLOSE_FILE
#	define API_ static inline
#else
#	define API_ SHIM_API
#endif

API_ int
shim_close_file (Shim_File_t const);

#undef API_

SHIM_API void
shim_enforce_close_file (Shim_File_t const);

#ifdef SHIM_FILES_INLINE_SET_FILE_SIZE
#	define API_ static inline
#else
#	define API_ SHIM_API
#endif /* ~ ifdef SHIM_FILES_INLINE_SET_FILE_SIZE */

API_ int
shim_set_file_size (Shim_File_t const, size_t const);

#undef API_

SHIM_API void
shim_enforce_set_file_size (Shim_File_t const, size_t const);

SHIM_END_DECLS

/* Inline/non-inline API function implementations */
#if    defined (SHIM_OS_UNIXLIKE)
#	define SHIM_FILES_CLOSE_FILE_IMPL(file_const) { \
		return close( file_const ); \
	}
#	define SHIM_FILES_SET_FILE_SIZE_IMPL(file, new_size) { \
		return ftruncate( file, new_size ); \
	}
#elif  defined (SHIM_OS_WINDOWS)
#	define SHIM_FILES_CLOSE_FILE_IMPL(file_const) { \
		if( !CloseHandle( file_const ) )\
			return -1; \
		return 0; \
	}
#	define SHIM_FILES_SET_FILE_SIZE_IMPL(file, new_size) { \
		LARGE_INTEGER li; \
		li.QuadPart = new_size; \
		if( !SetFilePointerEx( file, li, NULL, FILE_BEGIN ) ) \
			return -1; \
		if( !SetEndOfFile( file ) ) \
			return -1; \
		return 0; \
	}
#else
#	error "Unsupported OS."
#endif /* ~ if defined (SHIM_OS_UNIXLIKE) or defined (SHIM_OS_WINDOWS) */

#ifdef SHIM_FILES_INLINE_CLOSE_FILE
int
shim_close_file (Shim_File_t const file)
	SHIM_FILES_CLOSE_FILE_IMPL (file)
#endif /* ~ ifdef SHIM_FILES_INLINE_CLOSE_FILE */

#ifdef SHIM_FILES_INLINE_SET_FILE_SIZE
int
shim_set_file_size (Shim_File_t const file, size_t const new_size)
	SHIM_FILES_SET_FILE_SIZE_IMPL (file, new_size)
#endif /* ~ ifdef SHIM_FILES_INLINE_SET_FILE_SIZE */

#endif /* ~ SHIM_FILES_H */
