/* Copyright (C) 2020-2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "File.h"
#include <errno.h>

#if defined(__linux__) && defined(SSC_FILE_HAS_CREATESECRET)
 #include <sys/syscall.h>
 #include <unistd.h>
#endif

#if defined(SSC_OS_UNIXLIKE) && !defined(O_CLOEXEC)
 #warning "O_CLOEXEC was NOT defined!"
 #define O_CLOEXEC (0)
#endif

#define R_ SSC_RESTRICT

SSC_Error_t
SSC_File_getSize(SSC_File_t file, size_t* R_ storesize)
{
#if    defined(SSC_OS_UNIXLIKE)
  struct stat s;
  if (fstat(file, &s))
    return SSC_ERR;
  *storesize = (size_t)s.st_size;
#elif  defined(SSC_OS_WINDOWS)
  LARGE_INTEGER li;
  if (!GetFileSizeEx(file, &li))
    return SSC_ERR;
  *storesize = (size_t)li.QuadPart;
#else
 #error "Unsupported operating system."
#endif
  return SSC_OK;
}

SSC_Error_t
SSC_FilePath_getSize(const char* R_ fpath, size_t* R_ storesize)
{
#ifdef SSC_OS_UNIXLIKE
  struct stat s;
  if (stat(fpath, &s))
    return SSC_ERR;
  *storesize = (size_t)s.st_size;
  return SSC_OK;
#else /* Any other OS. */
  SSC_File_t f;
  if (SSC_FilePath_open(fpath, true, &f) != SSC_OK)
    return SSC_ERR;
  if (SSC_File_getSize(f, storesize) != SSC_OK) {
    SSC_File_close(f);
    return SSC_ERR;
  }
  return SSC_File_close(f);
#endif
}

bool
SSC_FilePath_exists(const char* filepath)
{
  bool exists = false;
#if   defined(SSC_OS_UNIXLIKE)
  struct stat s;
  if (stat(filepath, &s) == 0)
    exists = true;
#elif defined(SSC_OS_WINDOWS)
  const DWORD attrib = GetFileAttributesA(filepath);
  /* (The file exists and it is not a directory.) */
  if (attrib != INVALID_FILE_ATTRIBUTES &&
      !(attrib & FILE_ATTRIBUTE_DIRECTORY))
    exists = true;
#else /* In practice, this codepath is presently unreachable. */
  FILE* test = fopen(filepath, "r");
  if (test != SSC_NULL) {
    fclose(test);
    exists = true;
  }
#endif
  return exists;
}

void
SSC_FilePath_forceExistOrDie(const char* R_ filepath, bool force_to_exist)
{
  if (force_to_exist)
    SSC_assertMsg(SSC_FilePath_exists(filepath) , "Error: The filepath %s does not seem to exist.\n", filepath);
  else
    SSC_assertMsg(!SSC_FilePath_exists(filepath), "Error: The filepath %s seems to already exist.\n", filepath);
}

#if   defined(SSC_OS_UNIXLIKE)
 /* Files can only be accessed by the creator by default.
  */
 #define UNIX_MODE_ ((mode_t)0600)
#elif defined(SSC_OS_WINDOWS)
 /* When creating files on Windows we provide all the FILE_SHARE_* bits
  * to CreateFileA() so behavior is the same between Win32 and POSIX.
  */
 #define WIN_SHARE_MODE_ (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE)
 #define WIN_READONLY_   (GENERIC_READ)
 #define WIN_READWRITE_  (GENERIC_READ | GENERIC_WRITE)
#endif

SSC_Error_t
SSC_FilePath_open(const char* R_ filepath, bool readonly, SSC_File_t* R_ storefile)
{
#if    defined(SSC_OS_UNIXLIKE)
  *storefile = open(filepath, ((readonly ? O_RDONLY : O_RDWR) | O_CLOEXEC), UNIX_MODE_);
#elif  defined(SSC_OS_WINDOWS)
  const DWORD rw = readonly ? WIN_READONLY_ : WIN_READWRITE_;
  *storefile = CreateFileA(
    filepath,
    rw,
    WIN_SHARE_MODE_,
    SSC_NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    SSC_NULL
  );
#else
 #error "Unsupported operating system."
#endif
  return (*storefile != SSC_FILE_NULL_LITERAL) ? SSC_OK : SSC_ERR;
}

SSC_Error_t
SSC_FilePath_create(const char* R_ filepath, SSC_File_t* R_ storefile)
{
#if    defined(SSC_OS_UNIXLIKE)
  *storefile = open(filepath, (O_RDWR|O_CREAT|O_EXCL|O_CLOEXEC), UNIX_MODE_);
#elif  defined(SSC_OS_WINDOWS)
  *storefile = CreateFileA(
    filepath,
    WIN_READWRITE_,
    WIN_SHARE_MODE_,
    SSC_NULL,
    CREATE_NEW,
    FILE_ATTRIBUTE_NORMAL,
    SSC_NULL
  );
#else
 #error "Unsupported operating system."
#endif
  return (*storefile != SSC_FILE_NULL_LITERAL) ? SSC_OK : SSC_ERR;
}

SSC_Error_t
SSC_FilePath_openAppend(const char* R_ filepath, SSC_File_t* R_ storefile)
{
#if   defined(SSC_OS_UNIXLIKE)
  *storefile = open(filepath, (O_RDWR|O_APPEND|O_CLOEXEC), UNIX_MODE_);
#elif defined(SSC_OS_WINDOWS)
  *storefile = CreateFileA(
    filepath,
    GENERIC_READ | FILE_APPEND_DATA,
    WIN_SHARE_MODE_,
    SSC_NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    SSC_NULL
  );
#else
 #error "Unsupported operating system."
#endif
  return (*storefile != SSC_FILE_NULL_LITERAL) ? SSC_OK : SSC_ERR;
}

SSC_Error_t
SSC_FilePath_createOrTruncate(const char* R_ filepath, SSC_File_t* R_ storefile)
{
#if   defined(SSC_OS_UNIXLIKE)
  *storefile = open(filepath, (O_RDWR|O_CREAT|O_TRUNC|O_CLOEXEC), UNIX_MODE_);
#elif defined(SSC_OS_WINDOWS)
  *storefile = CreateFileA(
    filepath,
    WIN_READWRITE_,
    WIN_SHARE_MODE_,
    SSC_NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    SSC_NULL
  );
#else
 #error "Unsupported operating system."
#endif
  return (*storefile != SSC_FILE_NULL_LITERAL) ? SSC_OK : SSC_ERR;
}

#ifdef SSC_FILE_HAS_CREATESECRET
SSC_Error_t
SSC_File_createSecret(SSC_File_t* storefile)
{
 #ifdef __linux__
 int f = syscall(SYS_memfd_secret, 0U);
 if (f == -1)
  return SSC_ERR;
 *storefile = f;
 #else
  #error "Unsupported OS!"
 #endif
 return SSC_OK;
}
#endif /* ! SSC_FILE_HAS_CREATESECRET */

bool
SSC_File_createSecretIsAvailable(void)
{
  #ifdef SSC_FILE_HAS_CREATESECRET
  SSC_File_t  f;
  if (SSC_File_createSecret(&f) == SSC_OK &&
      SSC_File_close(f) == SSC_OK)
    return true;
  #endif
  return false;
}

SSC_Error_t
SSC_File_close(SSC_File_t file)
{
  #if   defined(SSC_OS_UNIXLIKE)
  return close(file) == 0 ? SSC_OK : SSC_ERR;
  #elif defined(SSC_OS_WINDOWS)
  return CloseHandle(file) != 0 ? SSC_OK : SSC_ERR;
  #else
   #error "Unsupported OS!"
  #endif
}

#ifdef SSC_OS_UNIXLIKE
int ftruncate(int, off_t);
#endif

SSC_Error_t
SSC_File_setSize(SSC_File_t file, size_t size)
{
  #if   defined(SSC_OS_UNIXLIKE)
  return ftruncate(file, size) == 0 ? SSC_OK : SSC_ERR;
  #elif defined(SSC_OS_WINDOWS)
  LARGE_INTEGER i;
  i.QuadPart = (LONGLONG)size;
  if (!SetFilePointerEx(file, i, SSC_NULL, FILE_BEGIN) || !SetEndOfFile(file))
    return SSC_ERR;
  return SSC_OK;
  #else
   #error "Unsupported OS!"
  #endif
}

SSC_CodeError_t
SSC_File_read(SSC_File_t file, void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count)
{
#if   defined(SSC_OS_UNIXLIKE)
  SSC_ssize_t total = 0;
  size_t remaining = count;
  while (remaining > 0) {
    SSC_ssize_t r = read(file, ((uint8_t*)buf) + total, remaining);
    if (r < 0) {
      if (errno == EINTR) continue;
      return SSC_FILE_READ_ERR;
    }
    if (r == 0 || total + r == (SSC_ssize_t)count) break;
    total += r;
    remaining -= (size_t)r;
  }
  *stored_count = total;
  if (total < (SSC_ssize_t)count) return SSC_FILE_READ_EOF;
  return SSC_FILE_READ_OK;
#elif defined(SSC_OS_WINDOWS)
  DWORD br;
  if (!ReadFile(file, buf, (DWORD)count, &br, SSC_NULL)) {
    if (GetLastError() == ERROR_HANDLE_EOF) {
      *stored_count = 0;
      return SSC_FILE_READ_EOF;
    }
    return SSC_FILE_READ_ERR;
  }
  *stored_count = (SSC_ssize_t)br;
  return ((size_t)br == count) ? SSC_FILE_READ_OK : SSC_FILE_READ_EOF;
#else
 #error "Unsupported OS!"
#endif
}

SSC_CodeError_t
SSC_File_write(SSC_File_t file, const void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count)
{
#if   defined(SSC_OS_UNIXLIKE)
  SSC_ssize_t written = 0;
  size_t remaining = count;
  while (remaining > 0) {
    SSC_ssize_t r = write(file, ((const uint8_t*)buf) + written, remaining);
    if (r < 0) {
      if (errno == EINTR) continue;
      return SSC_FILE_WRITE_ERR;
    }
    written += r;
    remaining -= (size_t)r;
  }
  *stored_count = written;
  return SSC_FILE_WRITE_OK;
#elif defined(SSC_OS_WINDOWS)
  DWORD written_total = 0;
  while (written_total < (DWORD)count) {
    DWORD bw = 0;
    BOOL result = WriteFile(
     file,
     (const uint8_t*)buf + written_total,
     (DWORD)count - written_total,
     &bw,
     SSC_NULL
    );
    if (!result)
      return SSC_FILE_WRITE_ERR;
    written_total += bw;
    if (bw == 0) break;
  }
  *stored_count = (SSC_ssize_t)written_total;
  return ((size_t)written_total == count) ? SSC_FILE_WRITE_OK : SSC_FILE_WRITE_PARTIAL;
#else
 #error "Unsupported OS!"
#endif
}

//TODO: Make offset SSC_ssize_t and use -1 to specify seeking to the end.
SSC_Error_t
SSC_File_seek(SSC_File_t file, size_t offset)
{
#if   defined(SSC_OS_UNIXLIKE)
  if (lseek(file, (off_t)offset, SEEK_SET) < 0)
    return SSC_ERR;
  return SSC_OK;
#elif defined(SSC_OS_WINDOWS)
  LARGE_INTEGER li;
  li.QuadPart = (LONGLONG)offset;
  if (!SetFilePointerEx(file, li, SSC_NULL, FILE_BEGIN))
    return SSC_ERR;
  return SSC_OK;
#else
 #error "Unsupported OS!"
#endif
}

SSC_CodeError_t
SSC_File_seekRead(SSC_File_t file, size_t offset, void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count)
{
  if (SSC_File_seek(file, offset) != SSC_OK)
    return SSC_FILE_SEEK_ERR;
  return SSC_File_read(file, buf, count, stored_count);
}

SSC_CodeError_t
SSC_File_seekWrite(SSC_File_t file, size_t offset, const void* R_ buf, size_t count, SSC_ssize_t* R_ stored_count)
{
  if (SSC_File_seek(file, offset) != SSC_OK)
    return SSC_FILE_SEEK_ERR;
  return SSC_File_write(file, buf, count, stored_count);
}

SSC_Error_t
SSC_chdir(const char* path)
{
  #if   defined(SSC_OS_UNIXLIKE)
  return chdir(path) == 0 ? SSC_OK : SSC_ERR;
  #elif defined(SSC_OS_WINDOWS)
  return _chdir(path) == 0 ? SSC_OK : SSC_ERR;
  #else
   #error "Unsupported OS!"
  #endif
}
