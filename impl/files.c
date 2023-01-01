/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include "files.h"

#define R_(Ptr) Ptr BASE_RESTRICT /* Restrict pointer aliasing if we can. */

#if   defined(BASE_OS_UNIXLIKE)
typedef struct stat   Stat_t;
#elif defined(BASE_OS_WINDOWS)
typedef LARGE_INTEGER LargeInt_t;
typedef DWORD         Dw32_t;
#endif

int Base_get_file_size(Base_File_t file, R_(size_t*) size_p)
{
#if    defined(BASE_OS_UNIXLIKE)
  Stat_t s;
  if (fstat(file, &s))
    return -1;
  *size_p = (size_t)s.st_size;
#elif  defined(BASE_OS_WINDOWS)
  LargeInt_t li;
  if (!GetFileSizeEx(file, &li))
    return -1;
  *size_p = (size_t)li.QuadPart;
#else
 #error "Unsupported operating system."
#endif
  return 0;
}

int Base_get_filepath_size(R_(const char*) fpath, R_(size_t*) size_p)
{
#ifdef BASE_OS_UNIXLIKE
  Stat_t s;
  if (stat(fpath, &s))
    return -1;
  *size_p = (size_t)s.st_size;
  return 0;
#else /* Any other OS. */
  Base_File_t f;
  if (Base_open_filepath(fpath, true, &f))
    return -1;
  if (Base_get_file_size(f, size_p)) {
    Base_close_file(f);
    return -1;
  }
  return Base_close_file(f);
#endif
}

bool Base_filepath_exists(const char* filepath)
{
  bool exists = false;
  FILE* test = fopen(filepath, "r");
  if (test) {
    fclose(test);
    exists = true;
  }
  return exists;
}

void Base_force_filepath_existence_or_die(R_(const char*) filepath, bool force_to_exist)
{
  if (force_to_exist)
    Base_assert_msg(Base_filepath_exists(filepath) , "Error: The filepath %s does not seem to exist.\n", filepath);
  else
    Base_assert_msg(!Base_filepath_exists(filepath), "Error: The filepath %s seems to already exist.\n", filepath);
}

int Base_open_filepath(R_(const char*) filepath, bool readonly, R_(Base_File_t*) file)
{
#if    defined(BASE_OS_UNIXLIKE)
  const int read_write_rights = readonly ? O_RDONLY : O_RDWR;
  *file = open(filepath, read_write_rights, (mode_t)0600);
#elif  defined(BASE_OS_WINDOWS)
  BASE_ANY_ASSERT(sizeof(Dw32_t) == 4, "Dw32_t not 4 bytes!");
  const Dw32_t read_write_rights = readonly ? GENERIC_READ : (GENERIC_READ|GENERIC_WRITE);
  *file = CreateFileA(filepath, read_write_rights, 0, BASE_NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, BASE_NULL);
#else
 #error "Unsupported operating system."
#endif
  return (*file != BASE_FILE_NULL_LITERAL) ? 0 : -1;
}

int Base_create_filepath(R_(const char*) filepath, R_(Base_File_t*) file)
{
#if    defined(BASE_OS_UNIXLIKE)
  *file = open(filepath, (O_RDWR|O_TRUNC|O_CREAT), (mode_t)0600);
#elif  defined(BASE_OS_WINDOWS)
  *file = CreateFileA(filepath, (GENERIC_READ|GENERIC_WRITE), 0, BASE_NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, BASE_NULL);
#else
 #error "Unsupported operating system."
#endif
  return (*file != BASE_FILE_NULL_LITERAL) ? 0 : -1;
}

#ifndef BASE_SET_FILE_SIZE_INLINE
int
Base_set_file_size(Base_File_t file, size_t size)
BASE_SET_FILE_SIZE_IMPL(file, size)
#endif /* ~ BASE_INLINE_SET_FILE_SIZE */
