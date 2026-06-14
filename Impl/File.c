/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "File.h"

#if defined(__gnu_linux__) && defined(SSC_FILE_HAS_CREATESECRET)
 #include <sys/syscall.h>
 #include <unistd.h>
#endif

#define R_ SSC_RESTRICT

#if   defined(SSC_OS_UNIXLIKE)
 typedef struct stat   Stat_t;
#elif defined(SSC_OS_WINDOWS)
 typedef LARGE_INTEGER LargeInt_t;
 typedef DWORD         Dw32_t;
#endif

SSC_Error_t
SSC_File_getSize(SSC_File_t file, size_t* R_ storesize)
{
#if    defined(SSC_OS_UNIXLIKE)
  Stat_t s;
  if (fstat(file, &s))
    return SSC_ERR;
  *storesize = (size_t)s.st_size;
#elif  defined(SSC_OS_WINDOWS)
  LargeInt_t li;
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
  Stat_t s;
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
  Stat_t s;
  if (stat(filepath, &s) == 0)
    exists = true;
#elif defined(SSC_OS_WINDOWS)
  const Dw32_t attrib = GetFileAttributesA(filepath);
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
  *storefile = open(filepath, (readonly ? O_RDONLY : O_RDWR), UNIX_MODE_);
#elif  defined(SSC_OS_WINDOWS)
  const Dw32_t rw = readonly ? WIN_READONLY_ : WIN_READWRITE_;
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
  *storefile = open(filepath, (O_RDWR|O_CREAT|O_EXCL), UNIX_MODE_);
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

#ifdef SSC_FILE_HAS_CREATESECRET
SSC_Error_t
SSC_File_createSecret(SSC_File_t* storefile)
{
 #ifdef __gnu_linux__
 int f = syscall(SYS_memfd_secret, 0U);
 if (f == -1)
  return SSC_ERR;
 *storefile = f;
 return SSC_OK;
 #else
  #error "Unsupported OS!"
 #endif
}
#endif /* ! SSC_FILE_HAS_CREATESECRET */

bool
SSC_File_createSecretIsAvailable(void)
{
 #ifdef SSC_FILE_HAS_CREATESECRET
  SSC_File_t  f;
  SSC_Error_t result = SSC_File_createSecret(&f);
  if (result == SSC_OK) {
    if (SSC_File_close(f) == SSC_OK)
      return true;
    return false;
  }
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
  LargeInt_t i;
  i.QuadPart = size;
  if (!SetFilePointerEx(file, i, SSC_NULL, FILE_BEGIN) || !SetEndOfFile(file))
    return SSC_ERR;
  return SSC_OK;
  #else
   #error "Unsupported OS!"
  #endif
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
