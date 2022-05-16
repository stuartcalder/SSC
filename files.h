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
# define BASE_CLOSE_FILE_IMPL_F close
# define BASE_CLOSE_FILE_IMPL(f) { return BASE_CLOSE_FILE_IMPL_F(f); }
# define BASE_CLOSE_FILE_INLINE
# define BASE_SET_FILE_SIZE_IMPL_F ftruncate
# define BASE_SET_FILE_SIZE_IMPL(f, n) { return BASE_SET_FILE_SIZE_IMPL_F(f, n); }
# define BASE_SET_FILE_SIZE_INLINE
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
/* On Unix-like systems, files are managed through handles, represented by integers. */
typedef int Base_File_t;
# define BASE_FILE_NULL_LITERAL (-1) /* -1 is typically returned on failure. */
# define BASE_NULL_FILE         BASE_FILE_NULL_LITERAL /* FIXME: Deprecated. */

#elif defined(BASE_OS_WINDOWS)
# define BASE_CLOSE_FILE_IMPL(f) { if (CloseHandle(f)) return 0; return -1; }
# define BASE_SET_FILE_SIZE_IMPL(f, n) { \
     LARGE_INTEGER i; i.QuadPart = n; \
     if (!SetFilePointerEx(f, i, BASE_NULL, FILE_BEGIN) || !SetEndOfFile(f)) \
       return -1; \
     return 0; \
   }
#  include <windows.h>
/* On Windows systems, files are managed through HANDLEs. */
typedef HANDLE Base_File_t;
# define BASE_FILE_NULL_LITERAL (INVALID_HANDLE_VALUE)
# define BASE_NULL_FILE         BASE_FILE_NULL_LITERAL /* FIXME: Deprecated. */

#else
#  error "Unsupported operating system."
#endif /* ~ if defined (BASE_OS_UNIXLIKE) or defined (BASE_OS_WINDOWS) */

#define R_(Ptr) Ptr BASE_RESTRICT /* Restrict pointers from aliasing, if supported. */
BASE_BEGIN_C_DECLS

BASE_API    int         Base_get_file_size (Base_File_t, R_(size_t*));
BASE_API    size_t      Base_get_file_size_or_die (Base_File_t);
BASE_API    int         Base_get_filepath_size (R_(const char*), R_(size_t*));
BASE_API    size_t      Base_get_filepath_size_or_die (const char*);
BASE_API    bool        Base_filepath_exists (const char*);
BASE_API    void        Base_force_filepath_existence_or_die (R_(const char*), bool);
BASE_API    int         Base_open_filepath (R_(const char*), bool, R_(Base_File_t*));
BASE_API    Base_File_t Base_open_filepath_or_die (R_(const char*), bool);
BASE_API    int         Base_create_filepath (R_(const char*), R_(Base_File_t*));
BASE_API    Base_File_t Base_create_filepath_or_die (const char*);
#ifdef BASE_CLOSE_FILE_INLINE
BASE_INLINE int Base_close_file (Base_File_t f) BASE_CLOSE_FILE_IMPL(f)
#else
BASE_API    int Base_close_file (Base_File_t);
#endif
BASE_API   void Base_close_file_or_die (Base_File_t);
#ifdef BASE_SET_FILE_SIZE_INLINE
BASE_INLINE int Base_set_file_size (Base_File_t f, size_t s) BASE_SET_FILE_SIZE_IMPL(f, s)
#else
BASE_API    int Base_set_file_size (Base_File_t, size_t);
#endif
BASE_API   void Base_set_file_size_or_die (Base_File_t, size_t);

BASE_END_C_DECLS
#undef R_

#endif /* ~ BASE_FILES_H */
