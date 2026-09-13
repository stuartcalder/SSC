/* Copyright (C) 2020-2026 Stuart Calder
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
#include "Typedef.h"

#define SSC_FILE_DEFAULT_NEWFILE_SIZE 0

#if defined(SSC_OS_UNIXLIKE)
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 /* On Unix-like systems, files are managed through integer handles, "file descriptors". */
 typedef int SSC_File_t;
 #define SSC_FILE_IS_INT
 #define SSC_FILE_NULL_LITERAL (-1) /* -1 is an invalid file descriptor representing failure. */
 #ifdef __linux__
  /* Assume that memfd_secret() is supported if no Linux kernel version is specified. */
  #if !SSC_LINUX_VERSION_VALUE_ISDEFINED || (SSC_LINUX_VERSION_VALUE >= SSC_LINUX_VERSION(5, 14, 0))
   #define SSC_FILE_HAS_CREATESECRET
  #endif
 #endif
#elif defined(SSC_OS_WINDOWS)
 #include <windows.h>
 #include <direct.h>
 /* On Windows systems, files are managed through HANDLEs. */
 typedef HANDLE SSC_File_t;
 #define SSC_FILE_NULL_LITERAL INVALID_HANDLE_VALUE
#else
 #error "Unsupported operating system."
#endif /* ~ if defined (SSC_OS_UNIXLIKE) or defined (SSC_OS_WINDOWS) */

/* Differentiate between paths that reference
 * files, paths that reference directories, and
 * altogether invalid paths.
 */
typedef enum {
  SSC_PATH_NONE,
  SSC_PATH_FILE,
  SSC_PATH_DIR
} SSC_PathType_t;

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
  SSC_assertMsg(SSC_File_getSize(file, &s) == SSC_OK, "Error: SSC_File_getSize() failed to store the size of file %d at %p!", file, (void*)&s);
  #else
  SSC_assertMsg(SSC_File_getSize(file, &s) == SSC_OK, "Error: SSC_File_getSize() failed to store the size of a file at %p!", (void*)&s);
  #endif
  return s;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Determine whether a path specifies a file, directory, or nothing. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_PathType_t
SSC_Path_getType(const char* path);
/* ->SSC_PATH_NONE: Neither a file nor a directory exists at @path.
 * ->SSC_PATH_FILE: A file exists at @path.
 * ->SSC_PATH_DIR:  A directory exists at @path. */
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
  SSC_assertMsg(SSC_FilePath_getSize(fpath, &s) == SSC_OK, "Error: SSC_FilePath_getSize() failed to obtain the size of %s!\n", fpath);
  return s;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Is there a file (and not a directory) at a specified filepath? */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API bool
SSC_FilePath_exists(const char* fpath);
/* ->true : There is a file.
 * ->false: There is not a file (the path does not exist, or it is a directory). */
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Is there a directory at a specified filepath? */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API bool
SSC_DirPath_exists(const char* fpath);
/* ->true : There is a directory.
 * ->false: There is not a directory (the path does not exist, or it is not a directory). */
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
   SSC_FilePath_open(fpath, ronly, &f) == SSC_OK,
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
  SSC_assertMsg(SSC_FilePath_create(fpath, &f) == SSC_OK, "Error: SSC_FilePath_create() failed to create %s!\n", fpath);
  return f;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Open an existing file for appending. The file pointer is positioned at end-of-file so that all writes occur after existing content, without affecting other readers or writers (on systems that support atomic append). */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_FilePath_openAppend(const char* R_ fpath, SSC_File_t* R_ storefile);

SSC_INLINE SSC_File_t
SSC_FilePath_openAppendOrDie(const char* R_ fpath)
{
  SSC_File_t f;
  SSC_assertMsg(SSC_FilePath_openAppend(fpath, &f) == SSC_OK, "Error: SSC_FilePath_openAppend() failed to open %s for appending!\n", fpath);
  return f;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Create a new file at @fpath if it does not already exist. If the file already exists, truncate it to zero bytes. Always opens read-write. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_FilePath_createOrTruncate(const char* R_ fpath, SSC_File_t* R_ storefile);

SSC_INLINE SSC_File_t
SSC_FilePath_createOrTruncateOrDie(const char* R_ fpath)
{
  SSC_File_t f;
  SSC_assertMsg(SSC_FilePath_createOrTruncate(fpath, &f) == SSC_OK, "Error: SSC_FilePath_createOrTruncate() failed to create or truncate %s!\n", fpath);
  return f;
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Delete the file at the filepath @fpath. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_FilePath_delete(const char* fpath);

SSC_INLINE void
SSC_FilePath_deleteOrDie(const char* fpath)
{
  SSC_assertMsg(SSC_FilePath_delete(fpath) == SSC_OK, "Error: SSC_FilePath_delete() failed to delete the filepath %s!\n", fpath);
}
/*==========================================================================================*/

#ifdef SSC_FILE_HAS_CREATESECRET
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Create a "secret" file, with more protections than usually afforded by RAM-backed
 * filesytems. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_File_createSecret(SSC_File_t* file);
/*==========================================================================================*/
#endif /* ! SSC_FILE_HAS_CREATESECRET */

SSC_API bool
SSC_File_createSecretIsAvailable(void);

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Close the file associed with a specified file handle. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_File_close(SSC_File_t file);

SSC_INLINE void
SSC_File_closeOrDie(SSC_File_t file)
{
  #ifdef SSC_FILE_IS_INT
  SSC_assertMsg(SSC_File_close(file) == SSC_OK, "Error: SSC_File_close() failed to close file %d!\n", file);
  #else
  SSC_assertMsg(SSC_File_close(file) == SSC_OK, SSC_ERR_S_FAILED_IN("SSC_File_close()"));
  #endif
}
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Set the size of a file in bytes. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_File_setSize(SSC_File_t file, size_t size);

SSC_INLINE void
SSC_File_setSizeOrDie(SSC_File_t file, size_t size)
{
  #ifdef SSC_FILE_IS_INT
  SSC_assertMsg(SSC_File_setSize(file, size) == SSC_OK, "Error: SSC_File_setSize() failed to set file %d to size %zu!\n", file, size);
  #else
  SSC_assertMsg(SSC_File_setSize(file, size) == SSC_OK, "Error: SSC_File_setSize() failed to set a file to size %zu!\n", size);
  #endif
}
/*==========================================================================================*/

enum {
  /* Read */
  SSC_FILE_READ_OK      =   0, /* All @count bytes read successfully. */
  SSC_FILE_READ_EOF     =  -1, /* EOF reached before all bytes transferred; *stored_count holds partial count. */
  SSC_FILE_READ_ERR     =  -2, /* I/O error; *stored_count may hold a partial count of bytes transferred before the failure. */
  /* Write */
  SSC_FILE_WRITE_OK      =   0, /* All @count bytes written successfully. */
  SSC_FILE_WRITE_PARTIAL =  -1, /* Partial transfer then failure; *stored_count holds the number of bytes written before the error. */
  SSC_FILE_WRITE_ERR     =  -2, /* Write failure with nothing transferred; *stored_count is zero. */
  /* Seek */
  SSC_FILE_SEEK_ERR      =  -3, /* lseek/SetFilePointerEx failed (no read or write attempted); *stored_count is zero. */
};

/* Seek to end-of-file. */
#define SSC_FILE_SEEK_END ((SSC_ssize_t)-1)

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Read up to @count bytes from @file into @buf, looping until @count bytes are
 * transferred or EOF is reached. Returns:
 *   SSC_FILE_READ_OK      : All @count bytes successfully read.
 *   SSC_FILE_READ_EOF     : EOF reached before all bytes transferred; *stored_count holds partial count.
 *   SSC_FILE_READ_ERR     : I/O error; *stored_count may hold a partial count of bytes transferred before the failure. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_CodeError_t
SSC_File_read(SSC_File_t file, void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count);
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Write up to @count bytes from @buf to @file, looping until all bytes are
 * transferred or an error occurs. Returns:
 *   SSC_FILE_WRITE_OK     : All @count bytes successfully written.
 *   SSC_FILE_WRITE_PARTIAL: Partial transfer then failure; *stored_count holds the number of bytes written before the error.
 *   SSC_FILE_WRITE_ERR    : Write failure with nothing transferred; *stored_count is zero. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_CodeError_t
SSC_File_write(SSC_File_t file, const void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count);
/*==========================================================================================*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Seek to @offset within @file. Pass SSC_FILE_SEEK_END (or any negative offset) to seek to true end-of-file: position == file size, one past the last byte; a subsequent read returns EOF immediately. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_File_seek(SSC_File_t file, SSC_ssize_t offset);
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Seek to @offset within @file and read up to @count bytes into @buf. Returns:
 *   SSC_FILE_READ_OK      : All @count bytes successfully read after seek.
 *   SSC_FILE_READ_EOF     : EOF reached before all bytes transferred; *stored_count holds partial count.
 *   SSC_FILE_READ_ERR     : I/O error; *stored_count may hold a partial count of bytes transferred before the failure.
 *   SSC_FILE_SEEK_ERR     : Seek failed (no read attempted); *stored_count is zero. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_CodeError_t
SSC_File_seekRead(SSC_File_t file, SSC_ssize_t offset, void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count);
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Seek to @offset within @file and write up to @count bytes from @buf. Returns:
 *   SSC_FILE_WRITE_OK     : All @count bytes successfully written after seek.
 *   SSC_FILE_WRITE_PARTIAL: Partial transfer then failure; *stored_count holds the number of bytes written before the error.
 *   SSC_FILE_WRITE_ERR    : Write failure with nothing transferred; *stored_count is zero.
 *   SSC_FILE_SEEK_ERR     : Seek failed (no write attempted); *stored_count is zero. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_CodeError_t
SSC_File_seekWrite(SSC_File_t file, SSC_ssize_t offset, const void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count);
/*==========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Change the current working directory to @path. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_chdir(const char* path);
/*==========================================================================================*/

SSC_END_C_DECLS
#undef R_

#endif /* ~ SSC_FILE_H */
