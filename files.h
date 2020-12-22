/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_FILES_H
#define SHIM_FILES_H

#include "macros.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#if    defined (SHIM_OS_UNIXLIKE)
#	define SHIM_FILES_INLINE_CLOSE_FILE
#	define SHIM_FILES_INLINE_SET_FILE_SIZE
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <unistd.h>
#	include <fcntl.h>
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
#endif // ~ SHIM_OS_UNIXLIKE

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
#	define CLOSE_API_ static inline
#else
#	define CLOSE_API_ SHIM_API
#endif

CLOSE_API_ int
shim_close_file (Shim_File_t const);

#undef CLOSE_API_

SHIM_API void
shim_enforce_close_file (Shim_File_t const);

#ifdef SHIM_FILES_INLINE_SET_FILE_SIZE
#	define SET_API_ static inline
#else
#	define SET_API_ SHIM_API
#endif

SET_API_ int
shim_set_file_size (Shim_File_t const, size_t const);

#undef SET_API_

SHIM_API void
shim_enforce_set_file_size (Shim_File_t const, size_t const);

SHIM_END_DECLS

#if    defined (SHIM_OS_UNIXLIKE)
/* shim_close_file implementation. */
#	define SHIM_FILES_CLOSE_FILE_IMPL(file_const) { \
		return close( file_const ); \
	}
#	define SHIM_FILES_SET_FILE_SIZE_IMPL(file, new_size) { \
		return ftruncate( file, new_size ); \
	}
#elif  defined (SHIM_OS_WINDOWS)
/* shim_close_file implementation. */
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
#endif

#ifdef SHIM_FILES_INLINE_CLOSE_FILE
int
shim_close_file (Shim_File_t const file)
	SHIM_FILES_CLOSE_FILE_IMPL (file)
#endif
#ifdef SHIM_FILES_INLINE_SET_FILE_SIZE
int
shim_set_file_size (Shim_File_t const file, size_t const new_size)
	SHIM_FILES_SET_FILE_SIZE_IMPL (file, new_size)
#endif

#endif /* ~ SHIM_FILES_H */
