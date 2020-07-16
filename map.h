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

extern inline void 
shim_map_memory (Shim_Map *, bool const);

extern inline void
shim_unmap_memory (Shim_Map const *);

extern inline void
shim_sync_map (Shim_Map const *);

extern inline void
shim_nullify_map (Shim_Map *);

SHIM_END_DECLS

void
shim_map_memory (Shim_Map *shim_map, bool const readonly) {
#if    defined (SHIM_OS_UNIXLIKE)
	int const read_write_flag = (readonly ? PROT_READ : PROT_READ|PROT_WRITE);
	shim_map->ptr = (uint8_t *)mmap( NULL, shim_map->size, read_write_flag, MAP_SHARED, shim_map->shim_file, 0 );
	if( shim_map->ptr == MAP_FAILED )
		SHIM_ERRX ("Error: Failed to mmap() the file descriptor %d\n", shim_map->shim_file );
#elif  defined (SHIM_OS_WINDOWS)
	DWORD page_readwrite_flag;
	DWORD map_readwrite_flag;
	if( readonly ) {
		page_readwrite_flag = PAGE_READONLY;
		map_readwrite_flag = FILE_MAP_READ;
	} else {
		page_readwrite_flag = PAGE_READWRITE;
		map_readwrite_flag = FILE_MAP_READ|FILE_MAP_WRITE;
	}

	static_assert (sizeof(shim_map->size) == 8, "Shim_Map's size must be 8 bytes.");
	static_assert (sizeof(DWORD) == 4, "DWORD must be 4 bytes.");
	DWORD high_bits = (DWORD)(shim_map->size >> 32);
	DWORD low_bits  = (DWORD)(shim_map->size)
	shim_map->windows_filemapping = CreateFileMappingA( shim_map->shim_file, NULL, page_readwrite_flag, high_bits, low_bits, NULL );
	if( !shim_map->windows_filemapping )
		SHIM_ERRX ("Error: Failed during CreateFileMappingA()\n");
	shim_map->ptr = (uint8_t *)MapViewOfFile( shim_map->windows_filemapping, map_readwrite_flag, 0, 0, shim_map->size );
	if( !shim_map->ptr )
		SHIM_ERRX ("Error: Failed during MapViewOfFile()\n");
#else
#	error "Unsupported operating system."
#endif
} // ~ shim_map_memory(...)

void
shim_unmap_memory (Shim_Map const *shim_map) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( munmap( shim_map->ptr, shim_map->size ) == -1 )
		SHIM_ERRX ("Error: Failed to munmap()\n");
#elif  defined (SHIM_OS_WINDOWS)
	if( UnmapViewOfFile( (LPCVOID)shim_map->ptr ) == 0 )
		SHIM_ERRX ("Error: Failed to UnmapViewOfFile()\n");
	shim_close_file( shim_map->windows_filemapping );
#else
#	error "Unsupported operating system."
#endif
} // ~ shim_unmap_memory(...)

void
shim_sync_map (Shim_Map const *shim_map) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( msync( shim_map->ptr, shim_map->size, MS_SYNC ) == -1 )
		SHIM_ERRX ("Error: Failed to msync()\n");
#elif  defined (SHIM_OS_WINDOWS)
	if( FlushViewOfFile( (LPCVOID)shim_map->ptr, shim_map->size ))
		SHIM_ERRX ("Error: Failed to FlushViewOfFile()\n");
#else
#	error "Unsupported operating system."
#endif
}

void
shim_nullify_map (Shim_Map *shim_map) {
	shim_map->ptr = NULL;
	shim_map->size = 0;
	shim_map->shim_file = SHIM_NULL_FILE;
#ifdef SHIM_OS_WINDOWS
	shim_map->windows_filemapping = SHIM_NULL_FILE;
#endif
}

#endif // ~ SHIM_MAP_H




