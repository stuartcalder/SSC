/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef BASE_FILES_H
#define BASE_FILES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "macros.h"

#define BASE_FILES_DEFAULT_NEWFILE_SIZE 0

#if defined(BASE_OS_UNIXLIKE)
 #define BASE_CLOSE_FILE_IMPL_F  close
 #define BASE_CLOSE_FILE_IMPL(F) { return BASE_CLOSE_FILE_IMPL_F(F); }
 #define BASE_SET_FILE_SIZE_IMPL_F     ftruncate
 #define BASE_SET_FILE_SIZE_IMPL(F, N) { return BASE_SET_FILE_SIZE_IMPL_F(F, N); }
 #define BASE_SET_FILE_SIZE_INLINE
 #define BASE_CHDIR_IMPL_F     chdir
 #define BASE_CHDIR_IMPL(Path) { return BASE_CHDIR_IMPL_F(Path); }
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 /* On Unix-like systems, files are managed through integer handles, "file descriptors". */
 typedef int Base_File_t;
 #define BASE_FILE_NULL_LITERAL (-1) /* -1 is an invalid file descriptor, often representing failure. */
#elif defined(BASE_OS_WINDOWS)
 #define BASE_CLOSE_FILE_IMPL(F) { if (CloseHandle(F)) return 0; return -1; }
 #define BASE_SET_FILE_SIZE_IMPL(F, N) {\
  LARGE_INTEGER i; i.QuadPart = N;\
  if (!SetFilePointerEx(F, i, BASE_NULL, FILE_BEGIN) || !SetEndOfFile(F))\
    return -1;\
  return 0;\
 }
 #define BASE_CHDIR_IMPL_F     _chdir
 #define BASE_CHDIR_IMPL(Path) { return BASE_CHDIR_IMPL_F(Path); }
 #include <windows.h>
 #include <direct.h>
 /* On Windows systems, files are managed through HANDLEs. */
 typedef HANDLE Base_File_t;
 #define BASE_FILE_NULL_LITERAL INVALID_HANDLE_VALUE
#else
 #error "Unsupported operating system."
#endif /* ~ if defined (BASE_OS_UNIXLIKE) or defined (BASE_OS_WINDOWS) */

#define R_ BASE_RESTRICT /* Shorthand. */
BASE_BEGIN_C_DECLS

/* Get the size of a file in bytes. */
BASE_API Base_Error_t
Base_get_file_size(Base_File_t fhandle, size_t* R_ sizeptr);

BASE_INLINE size_t
Base_get_file_size_or_die(Base_File_t fhandle)
{
  size_t s;
  Base_assert_msg(!Base_get_file_size(fhandle, &s), BASE_ERR_S_FAILED_IN("Base_get_file_size()"));
  return s;
}

/* Get the size of a file at a specified filepath in bytes. */
BASE_API Base_Error_t
Base_get_filepath_size(const char* R_ fpath, size_t* R_ sizeptr);

BASE_INLINE size_t
Base_get_filepath_size_or_die(const char* fpath)
{
  size_t s;
  Base_assert_msg(!Base_get_filepath_size(fpath, &s), BASE_ERR_S_FAILED_IN("Base_get_filepath_size()"));
  return s;
}

/* Is there a file at a specified filepath? */
BASE_API bool
Base_filepath_exists(const char* fpath);
/* ->true : There is a file.
 * ->false: There is not a file. */

/* If @control is true, force a file to exist at @fpath; otherwise force a file to NOT exist at @fpath. */
BASE_API void
Base_force_filepath_existence_or_die(const char* R_ fpath, bool control);

/* Open the file at a specified filepath. */
BASE_API Base_Error_t
Base_open_filepath(const char* R_ fpath, bool ronly, Base_File_t* R_ fhptr);

BASE_INLINE Base_File_t
Base_open_filepath_or_die(const char* R_ fpath, bool ronly)
{
  Base_File_t f;
  Base_assert_msg(!Base_open_filepath(fpath, ronly, &f), BASE_ERR_S_FAILED_IN("Base_open_filepath()"));
  return f;
}

/* Create a file at a specified filepath. */
BASE_API Base_Error_t
Base_create_filepath(const char* R_ fpath, Base_File_t* R_ fhptr);

BASE_INLINE Base_File_t
Base_create_filepath_or_die(const char* fpath)
{
  Base_File_t f;
  Base_assert_msg(!Base_create_filepath(fpath, &f), BASE_ERR_S_FAILED_IN("Base_create_filepath()"));
  return f;
}

/* Close the file associed with a specified file handle. */
BASE_INLINE Base_Error_t
Base_close_file(Base_File_t fhandle)
BASE_CLOSE_FILE_IMPL(fhandle)

BASE_INLINE void
Base_close_file_or_die(Base_File_t fhandle)
{
  Base_assert_msg(!Base_close_file(fhandle), BASE_ERR_S_FAILED_IN("Base_close_file()"));
}

#ifdef BASE_SET_FILE_SIZE_INLINE
 #define  API_      BASE_INLINE
 #define IMPL_(...) BASE_SET_FILE_SIZE_IMPL(__VA_ARGS__)
#else
 #define  API_      BASE_API
 #define IMPL_(...) ;
#endif
/* Set the size of a file in bytes. Fill the void with zeroes. */
API_ Base_Error_t
Base_set_file_size(Base_File_t fhandle, size_t size)
IMPL_(fhandle, size)
#undef  API_
#undef IMPL_

BASE_INLINE void
Base_set_file_size_or_die(Base_File_t fhandle, size_t size)
{
  Base_assert_msg(!Base_set_file_size(fhandle, size), BASE_ERR_S_FAILED_IN("Base_set_file_size()"));
}

/* Change the current working directory to @path. */
BASE_INLINE Base_Error_t
Base_chdir(const char* path)
BASE_CHDIR_IMPL(path)

BASE_END_C_DECLS
#undef R_

#endif /* ~ BASE_FILES_H */
