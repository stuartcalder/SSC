/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_MMAP_H
#define BASE_MMAP_H

#include "errors.h"
#include "files.h"
#include "macros.h"
#include "types.h"
#if    defined(BASE_OS_UNIXLIKE)
#	define BASE_MMAP_SYNC_IMPL_F msync
#	define BASE_MMAP_SYNC_IMPL(m) { return BASE_MMAP_SYNC_IMPL_F(m->ptr, m->size, MS_SYNC); }
#	define BASE_MMAP_SYNC_INLINE
#	include <sys/mman.h>
#elif  defined(BASE_OS_WINDOWS)
#	define BASE_MMAP_SYNC_IMPL(m) { if (FlushViewOfFile((LPCVOID)m->ptr, m->size)) return 0; return -1; }
#	include <memoryapi.h>
#	include <windows.h>
#else
#	error "Unsupported operating system."
#endif /* ~ if defined(BASE_OS_UNIXLIKE) or defined(BASE_OS_WINDOWS) */

typedef struct {
	uint8_t*    ptr;
	size_t      size;
	Base_File_t file;
#ifdef BASE_OS_WINDOWS
	Base_File_t win_fmapping;
#endif /* ~ ifdef BASE_OS_WINDOWS */
} Base_MMap;

BASE_BEGIN_DECLS
BASE_API    int  Base_MMap_map (Base_MMap*, bool);
BASE_API    void Base_MMap_map_or_die (Base_MMap*, bool);
BASE_API    int  Base_MMap_unmap (Base_MMap*);
BASE_API    void Base_MMap_unmap_or_die (Base_MMap*);
#ifdef BASE_MMAP_SYNC_INLINE
BASE_INLINE int  Base_MMap_sync (const Base_MMap* m) BASE_MMAP_SYNC_IMPL(m)
#else
BASE_API    int  Base_MMap_sync (const Base_MMap*);
#endif
BASE_API    void Base_MMap_sync_or_die (const Base_MMap*);
BASE_END_DECLS

#endif /* ~ BASE_MMAP_H */
