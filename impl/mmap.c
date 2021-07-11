#include "mmap.h"

int Base_MMap_map (Base_MMap* map, bool readonly) {
#if    defined(BASE_OS_UNIXLIKE)
	const int rw = readonly ? PROT_READ : (PROT_READ|PROT_WRITE);
	map->ptr = (uint8_t*)mmap(NULL, map->size, rw, MAP_SHARED, map->file, 0);
	if (map->ptr == MAP_FAILED)
		return -1;
#elif  defined(BASE_OS_WINDOWS)
	const DWORD high_32 = (DWORD)((uint64_t)map->size >> 32);
	const DWORD low_32  = (DWORD)map->size;
	DWORD page_rw = PAGE_READWRITE;
	DWORD map_rw  = (FILE_MAP_READ|FILE_MAP_WRITE);
	if (readonly) {
		page_rw = PAGE_READONLY;
		map_rw  = FILE_MAP_READ;
	}

	BASE_STATIC_ASSERT(sizeof(map->size) == 8, "Map's size must be 8 bytes.");
	BASE_STATIC_ASSERT(sizeof(DWORD) == 4, "DWORD must be 4 bytes.");

	map->win_fmapping = CreateFileMappingA(map->file, NULL, page_rw, high_32, low_32, NULL);
	if (map->win_fmapping == BASE_NULL_FILE)
		return -1;

	map->ptr = (uint8_t*)MapViewOfFile(map->win_fmapping, map_rw, 0, 0, map->size);
	if (!map->ptr) {
		Base_close_file(map->win_fmapping);
		map->win_fmapping = BASE_NULL_FILE;
		return -1;
	}
#else
#	error "Unsupported operating system."
#endif
	map->readonly = (uint8_t)readonly;
	return 0;
}

void Base_MMap_map_or_die (Base_MMap* map, bool readonly) {
	Base_assert_msg(!Base_MMap_map(map, readonly), "Error: Base_MMap_map failed!\n");
}

int Base_MMap_unmap (Base_MMap* map) {
	int ret;
#if    defined(BASE_OS_UNIXLIKE)
	ret = munmap(map->ptr, map->size);
	if (!ret)
		map->ptr = NULL;
#elif  defined(BASE_OS_WINDOWS)
	ret = 0;
	if (!UnmapViewOfFile((LPCVOID)map->ptr))
		ret = -1;
	else
		map->ptr = NULL;
	if (Base_close_file(map->win_fmapping))
		ret = -1;
	else
		map->win_fmapping = BASE_NULL_FILE;
#else
#	error "Unsupported operating system."
#endif
	if (!ret)
		map->readonly = UINT8_C(0);
	return ret;
}

void Base_MMap_unmap_or_die (Base_MMap* map) {
	Base_assert_msg(!Base_MMap_unmap(map), "Error: Base_MMap_unmap failed!\n");
}

#ifndef BASE_MMAP_SYNC_INLINE
int Base_MMap_sync (const Base_MMap* map) BASE_MMAP_SYNC_IMPL(map)
#endif /* ~ BASE_MMAP_SYNC_INLINE */

void Base_MMap_sync_or_die (const Base_MMap* map) {
	Base_assert_msg(!Base_MMap_sync(map), "Error: Base_MMap_sync failed!\n");
}
