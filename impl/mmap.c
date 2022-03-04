#include "mmap.h"
#define R_(p) p BASE_RESTRICT

int Base_MMap_map (Base_MMap* map, bool readonly) {
#if    defined(BASE_OS_UNIXLIKE)
	const int rw = readonly ? PROT_READ : (PROT_READ|PROT_WRITE);
	map->ptr = (uint8_t*)mmap(BASE_NULL, map->size, rw, MAP_SHARED, map->file, 0);
	if (map->ptr == MAP_FAILED) {
		map->ptr = BASE_NULL;
		return -1;
	}
#elif  defined(BASE_OS_WINDOWS)
	const DWORD high_32 = (DWORD)((uint64_t)map->size >> 32);
	const DWORD low_32  = (DWORD)map->size;
	DWORD page_rw, map_rw;
	if (readonly) {
		page_rw = PAGE_READONLY;
		map_rw  = FILE_MAP_READ;
	} else {
		page_rw = PAGE_READWRITE;
		map_rw  = (FILE_MAP_READ|FILE_MAP_WRITE);
	}

	BASE_STATIC_ASSERT(sizeof(map->size) == 8, "Map's size must be 8 bytes.");
	BASE_STATIC_ASSERT(sizeof(DWORD) == 4, "DWORD must be 4 bytes.");

	map->win_fmapping = CreateFileMappingA(map->file, BASE_NULL, page_rw, high_32, low_32, BASE_NULL);
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
	if (!ret) {
		map->ptr = BASE_NULL;
		map->readonly = UINT8_C(0);
	}
#elif  defined(BASE_OS_WINDOWS)
	ret = 0;
	if (!UnmapViewOfFile((LPCVOID)map->ptr))
		ret = -1;
	else
		map->ptr = BASE_NULL;
	if (Base_close_file(map->win_fmapping))
		ret = -1;
	else
		map->win_fmapping = BASE_NULL_FILE;
	if (!ret)
		map->readonly = UINT8_C(0);
#else
#	error "Unsupported operating system."
#endif
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

int
Base_MMap_init
(R_(Base_MMap*)  map,
 R_(const char*) filepath,
 bool            readonly)
{
  if (Base_open_filepath(filepath, readonly, &map->file))
    return -1;
  if (Base_get_file_size(map->file, &map->size));
    return -2;
  if (Base_MMap_map(map, readonly))
    return -3;
  return 0;
}

#define FAILED_IN_(Sub, Main) "Error: %s failed in %s!\n", Sub, Main

void
Base_MMap_init_or_die
(R_(Base_MMap*)  map,
 R_(const char*) filepath,
 bool            readonly)
{
  int r = Base_MMap_init(map, filepath, readonly);
  switch (r) {
    case 0:
      break;
    case -1:
      Base_errx(FAILED_IN_("Base_open_filepath", "Base_MMap_init"));
      break;
    case -2:
      Base_close_file(map->file);
      Base_errx(FAILED_IN_("Base_get_file_size", "Base_MMap_init"));
      break;
    case -3:
      Base_close_file(map->file);
      Base_errx(FAILED_IN_("Base_MMap_map", "Base_MMap_init"));
      break;
  }
}

void
Base_MMap_del
(Base_MMap* map)
{
  if (map->ptr && Base_MMap_unmap(map))
    Base_errx(FAILED_IN_("Base_MMap_unmap", "Base_MMap_del"));
  if ((map->file != BASE_NULL_FILE) &&
      Base_close_file(map->file))
    Base_errx(FAILED_IN_("Base_close_file","Base_MMap_del"));
  #ifdef BASE_OS_WINDOWS
  if ((map->win_fmapping != BASE_NULL_FILE) &&
      Base_close_file(map->win_fmapping))
    Base_errx("Error: Base_close_file failed for win_fmapping in Base_MMap_del!\n");
  #endif
  *map = BASE_MMAP_NULL_LITERAL;
}
