/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_MAP_H
#define SHIM_MAP_H

#include "errors.h"
#include "files.h"
#include "macros.h"
#include <stdint.h>

#if    defined (SHIM_OS_UNIXLIKE)
#	define SHIM_MAP_INLINE_SYNC_MAP
#	include <sys/mman.h>
#elif  defined (SHIM_OS_WINDOWS)
#	include <memoryapi.h>
#	include <windows.h>
#else
#	error "Unsupported operating system."
#endif /* ~ if defined (SHIM_OS_UNIXLIKE) or defined (SHIM_OS_WINDOWS) */

typedef struct {
	uint8_t *    ptr;
	uint64_t    size;
	Shim_File_t file;
#ifdef SHIM_OS_WINDOWS
	Shim_File_t win_fmapping;
#endif /* ~ ifdef SHIM_OS_WINDOWS */
} Shim_Map;

SHIM_BEGIN_DECLS

SHIM_API void
shim_enforce_map_memory (Shim_Map *, bool const);

SHIM_API void
shim_enforce_sync_map (Shim_Map const *);

SHIM_API void
shim_enforce_unmap_memory (Shim_Map *);

SHIM_API int
shim_map_memory (Shim_Map *, bool const);

static inline void
shim_nullify_map (Shim_Map * map) {
	map->ptr = NULL;
	map->size = 0;
	map->file = SHIM_NULL_FILE;
#ifdef SHIM_OS_WINDOWS
	map->win_fmapping = SHIM_NULL_FILE;
#endif /* ~ ifdef SHIM_OS_WINDOWS */
}

#ifdef SHIM_MAP_INLINE_SYNC_MAP
#	define API_ static inline
#else
#	define API_ SHIM_API
#endif

API_ int
shim_sync_map (Shim_Map const *);

#undef API_

SHIM_API int
shim_unmap_memory (Shim_Map *);

SHIM_END_DECLS

#if    defined (SHIM_OS_UNIXLIKE)
#	define SHIM_MAP_SYNC_MAP_IMPL(map_ptr) { \
		return msync( map_ptr->ptr, \
			      map_ptr->size, \
			      MS_SYNC ); \
	}
#elif  defined (SHIM_OS_WINDOWS)
#	define SHIM_MAP_SYNC_MAP_IMPL(map_ptr) { \
		if( FlushViewOfFile( (LPCVOID)map_ptr->ptr, map_ptr->size ) ) \
			return 0; \
		return -1; \
	}
#else
#	error "Unsupported operating system."
#endif

#ifdef SHIM_MAP_INLINE_SYNC_MAP
int
shim_sync_map (Shim_Map const * map)
	SHIM_MAP_SYNC_MAP_IMPL (map)
#endif /* ~ SHIM_MAP_INLINE_SYNC_MAP */

#endif /* ~ SHIM_MAP_H */
