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

NO_DISCARD_ static inline size_t
shim_file_size (Shim_File_t const);

NO_DISCARD_ static inline bool
shim_filepath_exists (char const *);

static inline void
shim_enforce_filepath_existence (char const * SHIM_RESTRICT,
				 bool const);

NO_DISCARD_ static inline Shim_File_t
shim_open_existing_filepath (char const * SHIM_RESTRICT,
			     bool const);

NO_DISCARD_ static inline Shim_File_t
shim_create_filepath (char const *);

static inline void
shim_close_file (Shim_File_t const);

static inline void
shim_set_file_size (Shim_File_t const, size_t const);

SHIM_END_DECLS

size_t
shim_file_size (Shim_File_t const shim_file)
{
#if    defined (SHIM_OS_UNIXLIKE)
	struct stat stat_st;
	if( fstat( shim_file, &stat_st ) == -1 )
		SHIM_ERRX ("Error: Unable to fstat file descriptor #%d\n", shim_file);
	return (size_t)stat_st.st_size;
#elif  defined (SHIM_OS_WINDOWS)
	LARGE_INTEGER lg_int;
	if( GetFileSizeEx( shim_file, &lg_int ) == 0 )
		SHIM_ERRX ("Error: GetFileSizeEx() failed\n");
	return (size_t)lg_int.QuadPart;
#else
#	error "Unsupported operating system."
#endif // ~ #if defined (SHIM_OS_UNIXLIKE) ...
}

bool
shim_filepath_exists (char const *filepath)
{
	bool exists = false;
	FILE *test = fopen( filepath, "rb" );
	if( test ) {
		fclose( test );
		exists = true;
	}
	return exists;
}

void
shim_enforce_filepath_existence (char const * SHIM_RESTRICT filepath,
				 bool const                 force_to_exist)
{
	if( shim_filepath_exists( filepath ) ) {
		if( !force_to_exist )
			SHIM_ERRX ("Error: The file %s seems to already exist.\n", filepath);
	} else {
		if( force_to_exist )
			SHIM_ERRX ("Error: The file %s does not seem to exist.\n", filepath);
	}
}

Shim_File_t
shim_open_existing_filepath (char const * SHIM_RESTRICT filepath,
			     bool const                 readonly)
{
	shim_enforce_filepath_existence( filepath, true );
	Shim_File_t shim_file;
#if    defined (SHIM_OS_UNIXLIKE)
	int const read_write_rights = (readonly ? O_RDONLY : O_RDWR);
	if( (shim_file = open( filepath, read_write_rights, (mode_t)0600 )) == -1 )
		SHIM_ERRX ("Error: Unable to open existing file %s with open()\n", filepath);
#elif  defined (SHIM_OS_WINDOWS)
	if( readonly ) {
		if( (shim_file = CreateFileA( filepath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
			SHIM_ERRX ("Error: Unable to open existing file %s with CreateFileA()\n", filepath);
	} else {
		if( (shim_file = CreateFileA( filepath, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
			SHIM_ERRX ("Error: Unable to open existing file %s with CreateFileA()\n", filepath);
	}
#else
#	error "Unsupported operating system."
#endif
	return shim_file;
}

Shim_File_t
shim_create_filepath (char const *filepath)
{
	shim_enforce_filepath_existence( filepath, false );
	Shim_File_t shim_file;
#if    defined (SHIM_OS_UNIXLIKE)
	if( (shim_file = open( filepath, O_RDWR|O_TRUNC|O_CREAT, (mode_t)0600 )) == -1 )
		SHIM_ERRX ("Error: Unable to create new file %s with open()\n", filepath);
#elif  defined (SHIM_OS_WINDOWS)
	if( (shim_file = CreateFileA( filepath, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
		SHIM_ERRX ("Error: Unable to create file %s with CreateFileA()\n", filepath);
#else
#	error "Unsupported operating system."
#endif
	return shim_file;
}

void
shim_close_file (Shim_File_t const shim_file)
{
#if    defined (SHIM_OS_UNIXLIKE)
	if( close( shim_file ) == -1 )
		SHIM_ERRX ("Error: Wasn't able to close file descriptor %d\n", shim_file);
#elif  defined (SHIM_OS_WINDOWS)
	if( CloseHandle( shim_file ) == 0 )
		SHIM_ERRX ("Error: Wasn't able to close file handle\n");
#else
#	error "Unsupported operating system."
#endif
}

void
shim_set_file_size (Shim_File_t const shim_file, size_t const new_size)
{
#if    defined (SHIM_OS_UNIXLIKE)
	if( ftruncate( shim_file, new_size ) == -1 )
		SHIM_ERRX ("Error: Failed to set size of file descriptor %d to %zu\n", shim_file, new_size);
#elif  defined (SHIM_OS_WINDOWS)
	LARGE_INTEGER lg_int;
	lg_int.QuadPart = new_size;
	if( SetFilePointerEx( shim_file, lg_int, NULL, FILE_BEGIN ) == 0 )
		SHIM_ERRX ("Error: Failed to SetFilePointerEx()\n");
	if( SetEndOfFile( shim_file ) == 0 )
		SHIM_ERRX ("Error: Failed to SetEndOfFile()\n");
#else
#	error "Unsupported operating system."
#endif
}

#undef NO_DISCARD_

#endif // ~ SHIM_FILES_H
