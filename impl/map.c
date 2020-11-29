#include "map.h"

int
shim_map_memory (Shim_Map * map, bool const readonly) {
#if    defined (SHIM_OS_UNIXLIKE)
	int const rw_flag = (readonly ? PROT_READ : (PROT_READ|PROT_WRITE));
	map->ptr = (uint8_t *)mmap( NULL, map->size, rw_flag, MAP_SHARED, map->file, 0 );
	if( map->ptr == MAP_FAILED )
		return -1;
#elif  defined (SHIM_OS_WINDOWS)
	DWORD page_rw   = PAGE_READWRITE;
	DWORD map_rw    = (FILE_MAP_READ|FILE_MAP_WRITE);
	DWORD high_bits = (DWORD)(map->size >> 32);
	DWORD low_bits  = (DWORD)map->size;
	if( readonly ) {
		page_rw = PAGE_READONLY;
		map_rw  = FILE_MAP_READ;
	}

	SHIM_STATIC_ASSERT (CHAR_BIT == 8, "Bytes must be 8 bits.");
	SHIM_STATIC_ASSERT (sizeof(map->size) == 8, "Map's size must be 8 bytes.");
	SHIM_STATIC_ASSERT (sizeof(DWORD) == 4, "DWORD must be 4 bytes.");

	map->win_fmapping = CreateFileMappingA( map->file, NULL, page_rw, high_bits, low_bits, NULL );
	if( map->win_fmapping == SHIM_NULL_FILE )
		return -1;
	map->ptr = (uint8_t *)MapViewOfFile( map->win_fmapping, map_rw, 0, 0, map->size );
	if( !map->ptr ) {
		(void)shim_close_file( map->win_fmapping );
		return -1;
	}

#else
#	error "Unsupported operating system."
#endif
	return 0;
}

void
shim_enforce_map_memory (Shim_Map * map, bool const readonly) {
	if( shim_map_memory( map, readonly ) )
		SHIM_ERRX ("Error: shim_enforce_map_memory failed!\n");
}

int
shim_unmap_memory (Shim_Map * map) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( munmap( map->ptr, map->size ) == -1 )
		return -1;
	map->ptr = NULL;
#elif  defined (SHIM_OS_WINDOWS)
	if( UnmapViewOfFile( (LPCVOID)map->ptr ) == 0 )
		return -1;
	map->ptr = NULL;
	int status = shim_close_file( map->win_fmapping );
	if( status )
		return status;
	map->win_fmapping = SHIM_NULL_FILE;
#else
#	error "Unsupported operating system."
#endif
	return 0;
}

void
shim_enforce_unmap_memory (Shim_Map * map) {
	if( shim_unmap_memory( map ) )
		SHIM_ERRX ("Error: shim_enforce_unmap_memory failed!\n");
}

int
shim_sync_map (Shim_Map const * map) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( msync( map->ptr, map->size, MS_SYNC ) == -1 )
		return -1;
#elif  defined (SHIM_OS_WINDOWS)
	if( FlushViewOfFile( (LPCVOID)map->ptr, map->size ) )
		return -1;
#else
#	error "Unsupported operating system."
#endif
	return 0;
}

void
shim_enforce_sync_map (Shim_Map const * map) {
	if( shim_sync_map( map ) )
		SHIM_ERRX ("Error: shim_enforce_sync_map failed!\n");
}
