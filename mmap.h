/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef BASE_MMAP_H
#define BASE_MMAP_H

#include <stdbool.h>
#include "errors.h"
#include "files.h"
#include "macros.h"
#if defined(BASE_OS_UNIXLIKE)
 #define BASE_MMAP_SYNC_IMPL_FUNCTION  msync
 #define BASE_MMAP_SYNC_IMPL(M) { return BASE_MMAP_SYNC_IMPL_FUNCTION(M->ptr, M->size, MS_SYNC); }
 #include <sys/mman.h>
#elif defined(BASE_OS_WINDOWS)
 #define BASE_MMAP_HAS_WIN_FMAPPING
 #define BASE_MMAP_SYNC_IMPL(M) { if (FlushViewOfFile((LPCVOID)M->ptr, M->size)) return 0; return -1; }
 #include <memoryapi.h>
 #include <windows.h>
#else
 #error "Unsupported operating system."
#endif /* ~ if defined(BASE_OS_UNIXLIKE) or defined(BASE_OS_WINDOWS) */

#define R_ BASE_RESTRICT
BASE_BEGIN_C_DECLS

typedef struct {
  uint8_t*    ptr;
  size_t      size;
  Base_File_t file;
  #ifdef BASE_MMAP_HAS_WIN_FMAPPING /* We only need another Base_File_t on Windows systems. The Win32 API sucks. */
  Base_File_t win_fmapping;
  #endif
  bool        readonly;
} Base_MMap;

#ifdef BASE_MMAP_HAS_WIN_FMAPPING
 #define BASE_MMAP_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_MMap, BASE_NULL, 0, BASE_FILE_NULL_LITERAL, BASE_FILE_NULL_LITERAL, false)
#else
 #define BASE_MMAP_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_MMap, BASE_NULL, 0, BASE_FILE_NULL_LITERAL, false)
#endif

/* Initialization flags. */
enum {
  BASE_MMAP_INIT_READONLY =    0x01, /* Is the memory-map readonly? */
  BASE_MMAP_INIT_ALLOWSHRINK = 0x02, /* Are we allowed to shrink the size of the file? */
  BASE_MMAP_INIT_FORCE_EXIST = 0x04, /* Indicate we will either force existence or non-existence of a file. */
  /* When BASE_MMAP_INIT_FORCE_EXIST is on...
   * if BASE_MMAP_INIT_FORCE_EXIST_YES is on, enforce that the file already exists.
   * else, enforce that the file DOESN'T already exist. */
  BASE_MMAP_INIT_FORCE_EXIST_YES = 0x08,
}; typedef uint_fast8_t Base_MMap_Init_t;
/* Initialization error codes. */
enum {
  BASE_MMAP_INIT_CODE_OK =                   0,
  BASE_MMAP_INIT_CODE_ERR_FEXIST_NO =       -1, /* Failure to force non-existence of a file. */
  BASE_MMAP_INIT_CODE_ERR_FEXIST_YES =      -2, /* Failure to force existence of a file. */
  BASE_MMAP_INIT_CODE_ERR_READONLY =        -3, /* Failure to enforce read-only. */
  BASE_MMAP_INIT_CODE_ERR_SHRINK =          -4, /* Attempted to shrink while disallowed */
  BASE_MMAP_INIT_CODE_ERR_NOSIZE =          -5, /* Size not provided. */
  BASE_MMAP_INIT_CODE_ERR_OPEN_FILEPATH =   -6, /* Failed to open a filepath. */
  BASE_MMAP_INIT_CODE_ERR_CREATE_FILEPATH = -7, /* Failed to create a file at a filepath. */
  BASE_MMAP_INIT_CODE_ERR_GET_FILE_SIZE =   -8, /* Failed to get a file size. */
  BASE_MMAP_INIT_CODE_ERR_SET_FILE_SIZE =   -9, /* Failed to set a file size. */
  BASE_MMAP_INIT_CODE_ERR_MAP =            -10, /* Failed to map a file into memory. */
}; typedef int_fast8_t Base_MMap_Init_Code_t;

/* Open a file at @filepath with constraints passed as init flags in @flags,
 * then map the file into memory.
 * If a file does not exist at @filepath, an attempt will be made to create one there. */
BASE_API Base_MMap_Init_Code_t
Base_MMap_init(
 Base_MMap*  R_   map,
 const char* R_   filepath,
 size_t           size,
 Base_MMap_Init_t flags);

BASE_API void
Base_MMap_init_or_die(
 Base_MMap*  R_   map,
 const char* R_   filepath,
 size_t           size,
 Base_MMap_Init_t flags);

/* Assuming @map->file, @map->size, and @map->win_fmapping
 * (if applicable) fields of @map are initialized, attempt to map the
 * file into memory. */
BASE_API Base_Error_t
Base_MMap_map(Base_MMap* map, bool readonly);

BASE_INLINE void
Base_MMap_map_or_die(Base_MMap* map, bool readonly)
{
  Base_assert_msg(!Base_MMap_map(map, readonly), BASE_ERR_S_FAILED_IN("Base_MMap_map()"));
}

/* Attempt to unmap @map->file from memory. */
BASE_API Base_Error_t
Base_MMap_unmap(Base_MMap* map);

BASE_INLINE void
Base_MMap_unmap_or_die(Base_MMap* map)
{
  Base_assert_msg(!Base_MMap_unmap(map), BASE_ERR_S_FAILED_IN("Base_MMap_unmap()"));
}

/* Synchronize mapped memory with the filesystem. */
BASE_INLINE Base_Error_t
Base_MMap_sync(const Base_MMap* map)
BASE_MMAP_SYNC_IMPL(map)

BASE_INLINE void
Base_MMap_sync_or_die(const Base_MMap* map)
{
  Base_assert_msg(!Base_MMap_sync(map), BASE_ERR_S_FAILED_IN("Base_MMap_sync()"));
}

/* Unmap memory and close opened files. */
BASE_API void
Base_MMap_del(Base_MMap* map);

BASE_END_C_DECLS
#undef R_
#endif /* ~ BASE_MMAP_H */
