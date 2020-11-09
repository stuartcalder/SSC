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
/* shim_map_memory (map_ptr, readonly)
 * Maps the Shim_File_t into memory for reading and writing with pointers.
 * 	No return value. */
SHIM_API void
shim_map_memory (Shim_Map *, bool const);
/* shim_unmap_memory (map_ptr)
 * Unmaps the Shim_File_t from memory.
 * 	No Return value. */
SHIM_API void
shim_unmap_memory (Shim_Map const *);
/* shim_sync_map (map_ptr)
 * Synchronize the memory map, so that the stored file reflects what's in memory.
 * 	No Return value. */
SHIM_API void
shim_sync_map (Shim_Map const *);
/* shim_nullify_map (map_ptr)
 * Zero out the map data, and set the Shim_File_t'(s) to SHIM_NULL_FILE.
 * 	No return value. */
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

#endif // ~ SHIM_MAP_H




