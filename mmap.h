/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_MMAP_H
#define BASE_MMAP_H

#include <stdbool.h>
#include "errors.h"
#include "files.h"
#include "macros.h"
#include "types.h"
#if defined(BASE_OS_UNIXLIKE)
# define BASE_MMAP_SYNC_IMPL_F msync
# define BASE_MMAP_SYNC_IMPL(m) { return BASE_MMAP_SYNC_IMPL_F(m->ptr, m->size, MS_SYNC); }
# define BASE_MMAP_SYNC_INLINE
# include <sys/mman.h>
#elif defined(BASE_OS_WINDOWS)
# define BASE_MMAP_HAS_WIN_FMAPPING
# define BASE_MMAP_SYNC_IMPL(m) { if (FlushViewOfFile((LPCVOID)m->ptr, m->size)) return 0; return -1; }
# include <memoryapi.h>
# include <windows.h>
#else
# error "Unsupported operating system."
#endif /* ~ if defined(BASE_OS_UNIXLIKE) or defined(BASE_OS_WINDOWS) */
BASE_BEGIN_C_DECLS

/* Initialization flags. */
#define BASE_MMAP_INIT_READONLY    0x01u /* Are files readonly? */
#define BASE_MMAP_INIT_NOCREATE    0x02u /* Should we refrain from creating new files? */
#define BASE_MMAP_INIT_ALLOWSHRINK 0x04u /* Are we allowed to decrease the size of files? */
typedef uint_fast8_t Base_MMap_Init_t;

typedef struct {
  uint8_t*     ptr;
  size_t       size;
  Base_File_t  file;
  #ifdef BASE_MMAP_HAS_WIN_FMAPPING
  Base_File_t  win_fmapping;
  #endif /* ~ ifdef BASE_OS_WINDOWS */
  uint_fast8_t readonly;
} Base_MMap;

#ifdef BASE_MMAP_HAS_WIN_FMAPPING
# define BASE_MMAP_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_MMap, BASE_NULL, 0, BASE_NULL_FILE, BASE_NULL_FILE, 0)
#else
# define BASE_MMAP_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_MMap, BASE_NULL, 0, BASE_NULL_FILE, 0)
#endif

BASE_API int
Base_MMap_map
(Base_MMap* ctx, bool readonly);

BASE_API void
Base_MMap_map_or_die
(Base_MMap* ctx, bool readonly);

BASE_API int
Base_MMap_unmap
(Base_MMap* ctx);

BASE_API void
Base_MMap_unmap_or_die
(Base_MMap* ctx);

#ifdef BASE_MMAP_SYNC_INLINE
BASE_INLINE int
Base_MMap_sync
(const Base_MMap* ctx) BASE_MMAP_SYNC_IMPL(ctx)
#else
BASE_API int
Base_MMap_sync
(const Base_MMap* ctx);
#endif

BASE_API void
Base_MMap_sync_or_die
(const Base_MMap* ctx);

BASE_API int
Base_MMap_init
(Base_MMap* BASE_RESTRICT  ctx,
 const char* BASE_RESTRICT filepath,
 size_t                    size,
 Base_MMap_Init_t          flags);

BASE_API
void Base_MMap_init_or_die
(Base_MMap* BASE_RESTRICT  ctx,
 const char* BASE_RESTRICT filepath,
 size_t                    size,
 Base_MMap_Init_t          flags);

BASE_API
void Base_MMap_del
(Base_MMap* ctx);

BASE_END_C_DECLS
#endif /* ~ BASE_MMAP_H */
