/* Copyright (c) 2020-2024 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_FILE_H
#define SSC_FILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Macro.h"
#include "Error.h"

#define SSC_FILE_DEFAULT_NEWFILE_SIZE 0

#if defined(SSC_OS_UNIXLIKE)
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 /* On Unix-like systems, files are managed through integer handles, "file descriptors". */
 typedef int SSC_File_t;
 #define SSC_FILE_IS_INT
 #define SSC_FILE_CLOSE_IMPL_FUNCTION   close
 #define SSC_FILE_SETSIZE_IMPL_FUNCTION ftruncate
 #define SSC_CHDIR_IMPL_FUNCTION        chdir
 #define SSC_FILE_SETSIZE_INLINE
 #define SSC_FILE_NULL_LITERAL (-1) /* -1 is an invalid file descriptor representing failure. */
 #ifdef __gnu_linux__
  /* Assume that memfd_secret() is supported if no Linux kernel version is specified. */
  #if !SSC_LINUX_VERSION_VALUE_ISDEFINED || (SSC_LINUX_VERSION_VALUE >= SSC_LINUX_VERSION(5, 14, 0))
   #define SSC_HAS_FILE_CREATESECRET
  #endif
 #endif
#elif defined(SSC_OS_WINDOWS)
 #include <windows.h>
 #include <direct.h>
 /* On Windows systems, files are managed through HANDLEs. */
 typedef HANDLE SSC_File_t;
 #define SSC_CHDIR_IMPL_FUNCTION _chdir
 #define SSC_FILE_CLOSE_IMPL(File) {\
  if (CloseHandle(File))\
    return 0;\
  return -1;\
 }
 #define SSC_FILE_SETSIZE_IMPL(File, Size) {\
  LARGE_INTEGER i;\
  i.QuadPart = Size;\
  if (!SetFilePointerEx(File, i, SSC_NULL, FILE_BEGIN) || !SetEndOfFile(File))\
    return -1;\
  return 0;\
 }
 #define SSC_FILE_NULL_LITERAL INVALID_HANDLE_VALUE
#else
 #error "Unsupported operating system."
#endif /* ~ if defined (SSC_OS_UNIXLIKE) or defined (SSC_OS_WINDOWS) */

#if defined(SSC_FILE_CLOSE_IMPL_FUNCTION)
 #define SSC_FILE_CLOSE_IMPL(File) { return SSC_FILE_CLOSE_IMPL_FUNCTION(File); }
#elif !defined(SSC_FILE_CLOSE_IMPL)
 #error "SSC_FILE_CLOSE_IMPL undefined!"
#endif

#if defined(SSC_FILE_SETSIZE_IMPL_FUNCTION)
 #define SSC_FILE_SETSIZE_IMPL(File, Size) { return SSC_FILE_SETSIZE_IMPL_FUNCTION(File, Size); }
#elif !defined(SSC_FILE_SETSIZE_IMPL)
 #error "SSC_FILE_SETSIZE_IMPL undefined!"
#endif

#if defined(SSC_CHDIR_IMPL_FUNCTION)
 #define SSC_CHDIR_IMPL(Path) { return SSC_CHDIR_IMPL_FUNCTION(Path); }
#elif !defined(SSC_CHDIR_IMPL)
 #error "SSC_CHDIR_IMPL undefined!"
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Get the size of a file in bytes. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_File_getSize(SSC_File_t file, size_t* R_ storesize);

SSC_INLINE size_t
SSC_File_getSizeOrDie(SSC_File_t file)
{
  size_t s;
  #ifdef SSC_FILE_IS_INT
  SSC_assertMsg(!SSC_File_getSize(file, &s), "Error: SSC_File_getSize() failed to store the size of file %d at %p!", file, (void*)&s);
  #else
  SSC_assertMsg(!SSC_File_getSize(file, &s), "Error: SSC_File_getSize() failed to store the size of a file at %p!", (void*)&s);
  #endif
  return s;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Get the size of a file at a specified filepath in bytes. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_FilePath_getSize(const char* R_ fpath, size_t* R_ storesize);

SSC_INLINE size_t
SSC_FilePath_getSizeOrDie(const char* fpath)
{
  size_t s;
  SSC_assertMsg(!SSC_FilePath_getSize(fpath, &s), "Error: SSC_FilePath_getSize() failed to obtain the size of %s!\n", fpath);
  return s;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Is there a file at a specified filepath? */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API bool
SSC_FilePath_exists(const char* fpath);
/* ->true : There is a file.
 * ->false: There is not a file. */
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* If @control is true, force a file to exist at @fpath; otherwise force a file to NOT exist at @fpath. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API void
SSC_FilePath_forceExistOrDie(const char* R_ fpath, bool control);
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Open the file at a specified filepath. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_FilePath_open(const char* R_ fpath, bool ronly, SSC_File_t* R_ file);

SSC_INLINE SSC_File_t
SSC_FilePath_openOrDie(const char* R_ fpath, bool ronly)
{
  SSC_File_t f;
  SSC_assertMsg(
   !SSC_FilePath_open(fpath, ronly, &f),
   "Error: SSC_FilePath_open() failed to open %s as %s!\n",
   fpath,
   ronly ? "ReadOnly" : "ReadWrite");
  return f;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Create a file at a specified filepath. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_FilePath_create(const char* R_ fpath, SSC_File_t* R_ file);

SSC_INLINE SSC_File_t
SSC_FilePath_createOrDie(const char* fpath)
{
  SSC_File_t f;
  SSC_assertMsg(!SSC_FilePath_create(fpath, &f), "Error: SSC_FilePath_create() failed to create %s!\n", fpath);
  return f;
}
/*==========================================================================================*/

#ifdef SSC_HAS_FILE_CREATESECRET
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Create a "secret" file, with more protections than usually afforded by RAM-backed
 * filesytems. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_File_createSecret(SSC_File_t* file);
/*==========================================================================================*/
#endif /* ! SSC_HAS_FILE_CREATESECRET */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Close the file associed with a specified file handle. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE SSC_Error_t
SSC_File_close(SSC_File_t file)
SSC_FILE_CLOSE_IMPL(file)

SSC_INLINE void
SSC_File_closeOrDie(SSC_File_t file)
{
  #ifdef SSC_FILE_IS_INT
  SSC_assertMsg(!SSC_File_close(file), "Error: SSC_File_close() failed to close file %d!\n", file);
  #else
  SSC_assertMsg(!SSC_File_close(file), SSC_ERR_S_FAILED_IN("SSC_File_close()"));
  #endif
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Set the size of a file in bytes. Fill the void with zeroes. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#ifdef SSC_FILE_SETSIZE_INLINE
 #define  API_      SSC_INLINE
 #define IMPL_(...) SSC_FILE_SETSIZE_IMPL(__VA_ARGS__)
#else
 #define  API_      SSC_API
 #define IMPL_(...) ;
#endif
API_ SSC_Error_t
SSC_File_setSize(SSC_File_t file, size_t size)
IMPL_(file, size)
#undef  API_
#undef IMPL_

SSC_INLINE void
SSC_File_setSizeOrDie(SSC_File_t file, size_t size)
{
  #ifdef SSC_FILE_IS_INT
  SSC_assertMsg(!SSC_File_setSize(file, size), "Error: SSC_File_setSize() failed to set file %d to size %zu!\n", file, size);
  #else
  SSC_assertMsg(!SSC_File_setSize(file, size), "Error: SSC_File_setSize() failed to set a file to size %zu!\n", size);
  #endif
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Change the current working directory to @path. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_INLINE SSC_Error_t
SSC_chdir(const char* path)
SSC_CHDIR_IMPL(path)
/*==========================================================================================*/

SSC_END_C_DECLS
#undef R_

#endif /* ~ SSC_FILE_H */
