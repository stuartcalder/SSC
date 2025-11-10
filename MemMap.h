/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_MEMMAP_H
#define SSC_MEMMAP_H

#include <stdbool.h>

#include "Error.h"
#include "File.h"
#include "Macro.h"

#if defined(SSC_OS_UNIXLIKE)
 #define SSC_MEMMAP_SYNC_IMPL_FUNCTION  msync
 #define SSC_MEMMAP_SYNC_IMPL(M) { return SSC_MEMMAP_SYNC_IMPL_FUNCTION(M->ptr, M->size, MS_SYNC); }
 #include <sys/mman.h>
#elif defined(SSC_OS_WINDOWS)
 #define SSC_MEMMAP_HAS_WINDOWS_FILEMAP
 #define SSC_MEMMAP_SYNC_IMPL(M) {\
  if (FlushViewOfFile((LPCVOID)M->ptr, M->size))\
    return 0;\
  return -1;\
 }
 #include <memoryapi.h>
 #include <windows.h>
#else
 #error "Unsupported operating system."
#endif /* ~ if defined(SSC_OS_UNIXLIKE) or defined(SSC_OS_WINDOWS) */

#ifdef __gnu_linux__
 #define SSC_MEMMAP_HAS_INITSECRET
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Memory Map */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
typedef struct {
  uint8_t*       ptr;
  size_t         size;
  SSC_File_t     file;
  #ifdef SSC_MEMMAP_HAS_WINDOWS_FILEMAP
  SSC_File_t     windows_filemap;
  #endif
  SSC_BitFlag8_t flags;
} SSC_MemMap;
#ifdef SSC_MEMMAP_HAS_WINDOWS_FILEMAP
 #define SSC_MEMMAP_NULL_LITERAL SSC_STRUCT_LITERAL(SSC_MemMap, SSC_NULL, 0U, SSC_FILE_NULL_LITERAL, SSC_FILE_NULL_LITERAL, 0U)
#else
 #define SSC_MEMMAP_NULL_LITERAL SSC_STRUCT_LITERAL(SSC_MemMap, SSC_NULL, 0U, SSC_FILE_NULL_LITERAL, 0U)
#endif
/*=========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* State Flags
 *    SSC_BitFlag8_t
 *      These flags are stored in the @flags variable of SSC_MemMap structs, and are used in map() and unmap() operations.
 *      Users will typically not need to use these directly.
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
enum {
  /* Is the memory-map readonly? */
  SSC_MEMMAP_FLAG_READONLY = 0x01,
  /* Is the memory-map secret? i.e. created with SSC_MemMap_initSecret() */
  SSC_MEMMAP_FLAG_SECRET   = 0x02,
};

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Initialization Flags
 *     SSC_BitFlag_t
 *        These flags are not stored inside SSC_MemMap structs, but are merely passed in at runtime for init() operations.
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
enum {
  SSC_MEMMAP_INIT_READONLY =    0x01, /* Is the memory-map readonly? */
  SSC_MEMMAP_INIT_ALLOWSHRINK = 0x02, /* Are we allowed to shrink the size of the file? */
  SSC_MEMMAP_INIT_FORCE_EXIST = 0x04, /* Indicate we will either force existence or non-existence of a file. */
  /* When SSC_MEMMAP_INIT_FORCE_EXIST is on...
   * if SSC_MEMMAP_INIT_FORCE_EXIST_YES is on, enforce that the file already exists.
   * else, enforce that the file DOESN'T already exist. */
  SSC_MEMMAP_INIT_FORCE_EXIST_YES = 0x08,
};

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Initialization Error Codes
 *     SSC_CodeError_t
 *        These codes specify what errors might happen during an init() operation.
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
enum {
  SSC_MEMMAP_INIT_CODE_OK =                   0,
  SSC_MEMMAP_INIT_CODE_ERR_FEXIST_NO =       -1, /* Failure to force non-existence of a file. */
  SSC_MEMMAP_INIT_CODE_ERR_FEXIST_YES =      -2, /* Failure to force existence of a file. */
  SSC_MEMMAP_INIT_CODE_ERR_READONLY =        -3, /* Failure to enforce read-only. */
  SSC_MEMMAP_INIT_CODE_ERR_SHRINK =          -4, /* Attempted to shrink while disallowed */
  SSC_MEMMAP_INIT_CODE_ERR_NOSIZE =          -5, /* Size not provided. */
  SSC_MEMMAP_INIT_CODE_ERR_OPEN_FILEPATH =   -6, /* Failed to open a filepath. */
  SSC_MEMMAP_INIT_CODE_ERR_CREATE_FILEPATH = -7, /* Failed to create a file at a filepath. */
  SSC_MEMMAP_INIT_CODE_ERR_GET_FILE_SIZE =   -8, /* Failed to get a file size. */
  SSC_MEMMAP_INIT_CODE_ERR_SET_FILE_SIZE =   -9, /* Failed to set a file size. */
  SSC_MEMMAP_INIT_CODE_ERR_MAP =            -10, /* Failed to map a file into memory. */
  SSC_MEMMAP_INIT_CODE_ERR_SECRET =         -11, /* Failed to initialize a secret map. */
};

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Open a file at @filepath with constraints passed as init flags in @init_flags,
 * then map the file into memory.
 * If a file does not exist at @filepath, an attempt will be made to create one there. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_CodeError_t
SSC_MemMap_init(
 SSC_MemMap* R_ map,
 const char* R_ filepath,
 size_t         size,
 SSC_BitFlag_t  init_flags);

SSC_API void
SSC_MemMap_initOrDie(
 SSC_MemMap* R_ map,
 const char* R_ filepath,
 size_t         size,
 SSC_BitFlag_t  init_flags);
/*=========================================================================================*/

#ifdef SSC_MEMMAP_HAS_INITSECRET
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Create a RAM-backed MemoryMap to serve as a more secure memory buffer. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_CodeError_t
SSC_MemMap_initSecret(
 SSC_MemMap* map,
 size_t      size);
/*=========================================================================================*/
#endif

#if defined(SSC_FILE_IS_INT)
 #define MEMMAP_DUMP_ \
  "Error: %s. Dump:\n"\
  "map->ptr      = %p.\n"\
  "map->size     = %zu.\n"\
  "map->file     = %d.\n"\
  "map->flags    = %u.\n"
 #define MEMMAP_DUMP_ARGS_(Map, Err) \
  Err,\
  (void*)Map->ptr,\
  Map->size,\
  Map->file,\
  (unsigned)Map->flags
#else
 #define MEMMAP_DUMP_ \
  "Error: %s. Dump:\n"\
  "map->ptr      = %p.\n"\
  "map->size     = %zu.\n"\
  "map->flags    = %u.\n"
 #define MEMMAP_DUMP_ARGS_(Map, Err) \
  Err,\
  (void*)Map->ptr,\
  Map->size,\
  (unsigned)Map->flags
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Assuming @map->file, @map->size, and @map->windows_filemap
 * (if applicable) fields of @map are initialized, attempt to map the
 * file into memory. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_MemMap_map(SSC_MemMap* map, bool readonly);

SSC_INLINE void
SSC_MemMap_mapOrDie(SSC_MemMap* map, bool readonly)
{
  SSC_assertMsg(
   !SSC_MemMap_map(map, readonly),
   MEMMAP_DUMP_,
   MEMMAP_DUMP_ARGS_(map, "SSC_MemMap_map() failed to map a file into memory"));
}
/*=========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Attempt to unmap @map->file from memory. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_MemMap_unmap(SSC_MemMap* map);

SSC_INLINE void
SSC_MemMap_unmapOrDie(SSC_MemMap* map)
{
  SSC_assertMsg(
   !SSC_MemMap_unmap(map),
   MEMMAP_DUMP_,
   MEMMAP_DUMP_ARGS_(map, "SSC_MemMap_unmap() failed to unmap a file from memory"));
}
/*=========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Synchronize mapped memory with the filesystem. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_MemMap_sync(const SSC_MemMap* map);

SSC_INLINE void
SSC_MemMap_syncOrDie(const SSC_MemMap* map)
{
  SSC_assertMsg(
   !SSC_MemMap_sync(map),
   MEMMAP_DUMP_,
   MEMMAP_DUMP_ARGS_(map, "SSC_MemMap_sync() failed to synchronize a memory-mapped file"));
}
/*=========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Unmap memory and close opened files. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API void
SSC_MemMap_del(SSC_MemMap* map);
/*=========================================================================================*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Resize the underlying file. Unmap the file if it's currently mapped, and remap it. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API SSC_Error_t
SSC_MemMap_resize(SSC_MemMap* map, size_t new_size);
/*=========================================================================================*/

SSC_END_C_DECLS
#undef MEMMAP_DUMP_
#undef MEMMAP_DUMP_ARGS_
#undef R_

#endif
