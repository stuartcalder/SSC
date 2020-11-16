/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_MAP_H
#define SHIM_MAP_H

#include "macros.h"
#include "errors.h"
#include "files.h"
#include <stdint.h>

#if    defined (SHIM_OS_UNIXLIKE)
#	include <sys/mman.h>
#elif  defined (SHIM_OS_WINDOWS)
#	include <windows.h>
#	include <memoryapi.h>
#else
#	error "Unsupported operating system."
#endif

typedef struct {
	uint8_t *    ptr;
	uint64_t    size;
	Shim_File_t file;
#ifdef SHIM_OS_WINDOWS
	Shim_File_t win_fmapping;
#endif
} Shim_Map;

SHIM_BEGIN_DECLS

SHIM_API int
shim_map_memory (Shim_Map *, bool const);

SHIM_API void
shim_enforce_map_memory (Shim_Map *, bool const);

SHIM_API int
shim_unmap_memory (Shim_Map *);

SHIM_API void
shim_enforce_unmap_memory (Shim_Map *);

SHIM_API int
shim_sync_map (Shim_Map const *);

SHIM_API void
shim_enforce_sync_map (Shim_Map const *);

static inline void
shim_nullify_map (Shim_Map * map) {
	memset( map, 0, sizeof(*map) );
#if SHIM_NULL_FILE != 0
	map->file = SHIM_NULL_FILE;
#	ifdef SHIM_OS_WINDOWS
	map->win_fmapping = SHIM_NULL_FILE;
#	endif
#endif
}

SHIM_END_DECLS

#endif /* ~ SHIM_MAP_H */
