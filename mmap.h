/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_MMAP_H
#define BASE_MMAP_H

#include <stdbool.h>
#include "errors.h"
#include "files.h"
#include "macros.h"
#if defined(BASE_OS_UNIXLIKE)
 #define BASE_MMAP_SYNC_IMPL_F msync
 #define BASE_MMAP_SYNC_IMPL(M) { return BASE_MMAP_SYNC_IMPL_F(M->ptr, M->size, MS_SYNC); }
 #define BASE_MMAP_SYNC_INLINE
 #include <sys/mman.h>
#elif defined(BASE_OS_WINDOWS)
 #define BASE_MMAP_HAS_WIN_FMAPPING
 #define BASE_MMAP_SYNC_IMPL(M) { if (FlushViewOfFile((LPCVOID)M->ptr, M->size)) return 0; return -1; }
 #include <memoryapi.h>
 #include <windows.h>
#else
 #error "Unsupported operating system."
#endif /* ~ if defined(BASE_OS_UNIXLIKE) or defined(BASE_OS_WINDOWS) */

#define R_(Ptr) Ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS

/* Initialization flags. */
#define BASE_MMAP_INIT_READONLY        UINT8_C(0x01) /* Is the memorymap readonly? */
#define BASE_MMAP_INIT_ALLOWSHRINK     UINT8_C(0x02) /* Are we allowed to shrink the size of the file? */
#define BASE_MMAP_INIT_FORCE_EXIST     UINT8_C(0x04)
/* When BASE_MMAP_INIT_FORCE_EXIST is on...
 * if BASE_MMAP_INIT_FORCE_EXIST_YES is on, enforce that the file already exists.
 * else, enforce that the file DOESN'T already exist. */
#define BASE_MMAP_INIT_FORCE_EXIST_YES UINT8_C(0x08)
typedef uint_fast8_t Base_MMap_Init_t;

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
 #define BASE_MMAP_NULL_LITERAL \
  BASE_COMPOUND_LITERAL(Base_MMap, BASE_NULL, 0u, BASE_FILE_NULL_LITERAL, BASE_FILE_NULL_LITERAL, false)
#else
 #define BASE_MMAP_NULL_LITERAL \
  BASE_COMPOUND_LITERAL(Base_MMap, BASE_NULL, 0u, BASE_FILE_NULL_LITERAL, false)
#endif

BASE_API int  Base_MMap_map(Base_MMap* map, bool readonly);
BASE_API void Base_MMap_map_or_die(Base_MMap* map, bool readonly);
BASE_API int  Base_MMap_unmap(Base_MMap* map);
BASE_API void Base_MMap_unmap_or_die(Base_MMap* map);

#ifdef BASE_MMAP_SYNC_INLINE
 #define API_       BASE_INLINE
 #define IMPL_(...) BASE_MMAP_SYNC_IMPL(__VA_ARGS__)
#else
 #define API_ BASE_API
 #define IMPL_(...) ;
#endif
API_ int Base_MMap_sync(const Base_MMap* map) IMPL_(map)
#undef  API_
#undef IMPL_

BASE_API void Base_MMap_sync_or_die(const Base_MMap* map);

/* Initialization error codes. */
#undef  Code_t
#define Code_t Base_MMap_Init_Code_t
enum {
  BASE_MMAP_INIT_CODE_OK =                   0,
  BASE_MMAP_INIT_CODE_ERR_FEXIST_NO =       -1,
  BASE_MMAP_INIT_CODE_ERR_FEXIST_YES =      -2,
  BASE_MMAP_INIT_CODE_ERR_READONLY =        -3,
  BASE_MMAP_INIT_CODE_ERR_SHRINK =          -4,
  BASE_MMAP_INIT_CODE_ERR_NOSIZE =          -5,
  BASE_MMAP_INIT_CODE_ERR_OPEN_FILEPATH =   -6,
  BASE_MMAP_INIT_CODE_ERR_CREATE_FILEPATH = -7,
  BASE_MMAP_INIT_CODE_ERR_GET_FILE_SIZE =   -8,
  BASE_MMAP_INIT_CODE_ERR_SET_FILE_SIZE =   -9,
  BASE_MMAP_INIT_CODE_ERR_MAP =            -10,
}; typedef int_fast8_t Code_t;

BASE_API Code_t Base_MMap_init(
 R_(Base_MMap*)   map,
 R_(const char*)  filepath,
 size_t           size,
 Base_MMap_Init_t flags);

BASE_API void Base_MMap_init_or_die(
 R_(Base_MMap*)   map,
 R_(const char*)  filepath,
 size_t           size,
 Base_MMap_Init_t flags);

BASE_API void Base_MMap_del(Base_MMap* map);

BASE_END_C_DECLS
#undef R_
#undef Code_t
#endif /* ~ BASE_MMAP_H */
