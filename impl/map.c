#include "map.h"

void
shim_map_memory (Shim_Map * map, bool const readonly) {
#if    defined (SHIM_OS_UNIXLIKE)
	int const rw_flag = (readonly ? PROT_READ : PROT_READ|PROT_WRITE);
	map->ptr = (uint8_t *)mmap( NULL, map->size, rw_flag, MAP_SHARED, map->file, 0 );
	if( map->ptr == MAP_FAILED )
		SHIM_ERRX ("Error: Failed to mmap() the file descriptor %d\n", map->file);
#elif  defined (SHIM_OS_WINDOWS)
	DWORD page_rw_flag = PAGE_READWRITE;
	DWORD map_rw_flag = (FILE_MAP_READ | FILE_MAP_WRITE);
	if( readonly ) {
		page_rw_flag = PAGE_READONLY;
		map_rw_flag = FILE_MAP_READ;
	}
	SHIM_STATIC_ASSERT (CHAR_BIT == 8, "Bytes must be 8 bits.");
	SHIM_STATIC_ASSERT (sizeof(map->size) == 8, "Map's size must be 8 bytes.");
	SHIM_STATIC_ASSERT (sizeof(DWORD) == 4, "DWORD must be 4 bytes.");
	DWORD high_bits = (DWORD)(map->size >> 32);
	DWORD low_bits = (DWORD)map->size;
	map->win_fmapping = CreateFileMappingA( map->file, NULL, page_rw_flag, high_bits, low_bits, NULL );
	if( map->win_fmapping == SHIM_NULL_FILE )
		SHIM_ERRX ("Error: Failed during CreateFileMappingA()\n");
	map->ptr = (uint8_t *)MapViewOfFile( map->win_fmapping, map_rw_flag, 0, 0, map->size );
	if( !map->ptr )
		SHIM_ERRX ("Error: Failed during MapViewOfFile()\n");
#else
#	error "Unsupported operating system."
#endif
}

void
shim_unmap_memory (Shim_Map const * map) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( munmap( map->ptr, map->size ) == -1 )
		SHIM_ERRX ("Error: Failed to munmap()\n");
#elif  defined (SHIM_OS_WINDOWS)
	if( UnmapViewOfFile( (LPCVOID)map->ptr ) == 0 )
		SHIM_ERRX ("Error: Failed to UnmapViewOfFile()\n");
	shim_close_file( map->win_fmapping );
#else
#	error "Unsupported operating system."
#endif
}

void
shim_sync_map (Shim_Map const * map) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( msync( map->ptr, map->size, MS_SYNC ) == -1 )
		SHIM_ERRX ("Error: Failed to msync()\n");
#elif  defined (SHIM_OS_WINDOWS)
	if( FlushViewOfFile( (LPCVOID)map->ptr, map->size ) )
		SHIM_ERRX ("Error: Failed to FlushViewOfFile()\n");
#else
#	error "Unsupported operating system."
#endif
}
