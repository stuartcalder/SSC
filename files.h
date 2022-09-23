/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
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
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 /* On Unix-like systems, files are managed through handles, represented by integers. */
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
 #include <windows.h>
 /* On Windows systems, files are managed through HANDLEs. */
 typedef HANDLE Base_File_t;
 #define BASE_FILE_NULL_LITERAL INVALID_HANDLE_VALUE
#else
 #error "Unsupported operating system."
#endif /* ~ if defined (BASE_OS_UNIXLIKE) or defined (BASE_OS_WINDOWS) */

#define R_(Ptr) Ptr BASE_RESTRICT /* Restrict pointers from aliasing, if supported. */
BASE_BEGIN_C_DECLS

/* @fhandle: The Base_File_t we want to know the size of.
 * @sizeptr: Where the size will be stored if successful.
 * ->0   : Successfully stored the size of @fhandle in @sizeptr.
 * ->(-1): Failed. */
BASE_API int         Base_get_file_size(Base_File_t fhandle, R_(size_t*) sizeptr);
BASE_API size_t      Base_get_file_size_or_die(Base_File_t fhandle);
BASE_API int         Base_get_filepath_size(R_(const char*) fpath, R_(size_t*) sizeptr);
BASE_API size_t      Base_get_filepath_size_or_die(const char* fpath);
BASE_API bool        Base_filepath_exists(const char* fpath);
BASE_API void        Base_force_filepath_existence_or_die(R_(const char*) fpath, bool control);
BASE_API int         Base_open_filepath(R_(const char*) fpath, bool ronly, R_(Base_File_t*) fhptr);
BASE_API Base_File_t Base_open_filepath_or_die(R_(const char*) fpath, bool ronly);
BASE_API int         Base_create_filepath(R_(const char*) fpath, R_(Base_File_t*) fhptr);
BASE_API Base_File_t Base_create_filepath_or_die(const char* fpath);

BASE_INLINE int
Base_close_file(Base_File_t fhandle) BASE_CLOSE_FILE_IMPL(fhandle)

BASE_API void
Base_close_file_or_die(Base_File_t fhandle);

#ifdef BASE_SET_FILE_SIZE_INLINE
 #define API_       BASE_INLINE
 #define IMPL_(...) BASE_SET_FILE_SIZE_IMPL(__VA_ARGS__)
#else
 #define API_       BASE_API
 #define IMPL_(...) ;
#endif
API_ int Base_set_file_size(Base_File_t fhandle, size_t size) IMPL_(fhandle, size)
#undef  API_
#undef IMPL_
BASE_API void Base_set_file_size_or_die(Base_File_t fhandle, size_t size);

BASE_END_C_DECLS
#undef R_

#endif /* ~ BASE_FILES_H */
