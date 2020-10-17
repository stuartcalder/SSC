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

typedef struct SHIM_PUBLIC {
	uint8_t     *ptr;
	uint64_t    size;
	Shim_File_t shim_file;
#ifdef SHIM_OS_WINDOWS
	Shim_File_t windows_filemapping;
#endif
} Shim_Map;

SHIM_BEGIN_DECLS

void SHIM_PUBLIC
shim_map_memory (Shim_Map *, bool const);

void SHIM_PUBLIC
shim_unmap_memory (Shim_Map const *);

void SHIM_PUBLIC
shim_sync_map (Shim_Map const *);

static inline void 
shim_nullify_map (Shim_Map * map) {
	memset( map, 0, sizeof(*map) );
#if SHIM_NULL_FILE != 0
	map->shim_file = SHIM_NULL_FILE;
	#ifdef SHIM_OS_WINDOWS
	map->windows_filemapping = SHIM_NULL_FILE;
	#endif
#endif
}

SHIM_END_DECLS

#endif // ~ SHIM_MAP_H




