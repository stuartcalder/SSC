/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "File.h"

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
    return -1;
  *storesize = (size_t)s.st_size;
#elif  defined(SSC_OS_WINDOWS)
  LargeInt_t li;
  if (!GetFileSizeEx(file, &li))
    return -1;
  *storesize = (size_t)li.QuadPart;
#else
 #error "Unsupported operating system."
#endif
  return 0;
}

SSC_Error_t
SSC_FilePath_getSize(const char* R_ fpath, size_t* R_ storesize)
{
#ifdef SSC_OS_UNIXLIKE
  Stat_t s;
  if (stat(fpath, &s))
    return -1;
  *storesize = (size_t)s.st_size;
  return 0;
#else /* Any other OS. */
  SSC_File_t f;
  if (SSC_FilePath_open(fpath, true, &f))
    return -1;
  if (SSC_File_getSize(f, storesize)) {
    SSC_File_close(f);
    return -1;
  }
  return SSC_File_close(f);
#endif
}

bool
SSC_FilePath_exists(const char* filepath)
{
  bool exists = false;
  FILE* test = fopen(filepath, "r");
  if (test) {
    fclose(test);
    exists = true;
  }
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

SSC_Error_t
SSC_FilePath_open(const char* R_ filepath, bool readonly, SSC_File_t* R_ storefile)
{
#if    defined(SSC_OS_UNIXLIKE)
  *storefile = open(filepath, (readonly ? O_RDONLY : O_RDWR), (mode_t)0600);
#elif  defined(SSC_OS_WINDOWS)
  const Dw32_t read_write_rights = readonly ? GENERIC_READ : (GENERIC_READ|GENERIC_WRITE);
  *storefile = CreateFileA(filepath, read_write_rights, 0, SSC_NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, SSC_NULL);
#else
 #error "Unsupported operating system."
#endif
  return (*storefile != SSC_FILE_NULL_LITERAL) ? 0 : -1;
}

SSC_Error_t
SSC_FilePath_create(const char* R_ filepath, SSC_File_t* R_ storefile)
{
#if    defined(SSC_OS_UNIXLIKE)
  *storefile = open(filepath, (O_RDWR|O_TRUNC|O_CREAT), (mode_t)0600);
#elif  defined(SSC_OS_WINDOWS)
  *storefile = CreateFileA(filepath, (GENERIC_READ|GENERIC_WRITE), 0, SSC_NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, SSC_NULL);
#else
 #error "Unsupported operating system."
#endif
  return (*storefile != SSC_FILE_NULL_LITERAL) ? 0 : -1;
}

#ifndef SSC_FILE_SETSIZE_INLINE
SSC_Error_t
SSC_File_setSize(SSC_File_t file, size_t size)
SSC_FILE_SETSIZE_IMPL(file, size)
#endif /* ~ SSC_FILE_SETSIZE_INLINE */
