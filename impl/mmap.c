#include "errors.h"
#include "files.h"
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
	map->readonly = (uint_fast8_t)readonly;
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
		map->readonly = 0;
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
		map->readonly = 0;
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

typedef enum {
  INIT_CODE_OK = 0,
  INIT_CODE_ERR_NOCREATE = -1,
  INIT_CODE_ERR_READONLY = -2,
  INIT_CODE_ERR_SHRINK = -3,
  INIT_CODE_ERR_NOSIZE = -4,
  INIT_CODE_ERR_OPEN_FILEPATH = -5,
  INIT_CODE_ERR_CREATE_FILEPATH = -6,
  INIT_CODE_ERR_GET_FILE_SIZE = -7,
  INIT_CODE_ERR_SET_FILE_SIZE = -8,
  INIT_CODE_ERR_MAP = -9,
} Init_Code_t;

#define RONLY_       BASE_MMAP_INIT_READONLY
#define NOCREATE_    BASE_MMAP_INIT_NOCREATE
#define ALLOWSHRINK_ BASE_MMAP_INIT_ALLOWSHRINK
typedef Base_MMap_Init_t  Init_t;

Init_Code_t
Base_MMap_init
(R_(Base_MMap*)  map,
 R_(const char*) filepath,
 size_t          size,
 Init_t          flags)
{
  bool exists, set_size;

  exists = Base_filepath_exists(filepath);
  /* We will set the size when the filepath
   * doesn't exist, and when it does exist and a size has been requested.
   */
  set_size = !(flags & RONLY_) && (!exists || (size > 0));
  if (exists) {
    if (Base_open_filepath(filepath, flags & RONLY_, &map->file))
      return INIT_CODE_ERR_OPEN_FILEPATH;
    if (Base_get_file_size(map->file, &map->size))
      return INIT_CODE_ERR_GET_FILE_SIZE;
    if (!(flags & RONLY_)) {
      if (map->size > size) {
        if (!(flags & ALLOWSHRINK_))
	  return INIT_CODE_ERR_SHRINK;
      } else if (map->size == size)
        set_size = false;
    }
  } else {
    if (flags & NOCREATE_)
      return INIT_CODE_ERR_NOCREATE;
    if (flags & RONLY_)
      return INIT_CODE_ERR_READONLY;
    if (!size)
      return INIT_CODE_ERR_NOSIZE;
    if (Base_create_filepath(filepath, &map->file))
      return INIT_CODE_ERR_CREATE_FILEPATH;
  }
  if (set_size) {
    map->size = size;
    if (Base_set_file_size(map->file, map->size))
      return INIT_CODE_ERR_SET_FILE_SIZE;
  }
  /* When we create a new file, it implicitly
   * is readwrite, not readonly.
   */
  if (Base_MMap_map(map, exists && (flags & RONLY_)))
    return INIT_CODE_ERR_MAP;
  return INIT_CODE_OK;
}

void
Base_MMap_init_or_die
(R_(Base_MMap*)  map,
 R_(const char*) filepath,
 size_t          size,
 Init_t          flags)
{
  Init_Code_t ic = Base_MMap_init(map, filepath, size, flags);
  const char* err_str;
  switch (ic) {
    case INIT_CODE_OK:
      return;
    case INIT_CODE_ERR_NOCREATE:
      err_str = "File creation disabled";
      break;
    case INIT_CODE_ERR_READONLY:
      err_str = "File creation prevented by readonly";
      break;
    case INIT_CODE_ERR_SHRINK:
      err_str = "File shrinking disallowed";
      break;
    case INIT_CODE_ERR_NOSIZE:
      err_str = "Size not provided";
      break;
    case INIT_CODE_ERR_OPEN_FILEPATH:
      err_str = "Base_open_filepath failed";
      break;
    case INIT_CODE_ERR_CREATE_FILEPATH:
      err_str = "Base_create_filepath failed";
      break;
    case INIT_CODE_ERR_GET_FILE_SIZE:
      err_str = "Base_get_file_size failed";
      break;
    case INIT_CODE_ERR_SET_FILE_SIZE:
      err_str = "Base_set_file_size failed";
      break;
    case INIT_CODE_ERR_MAP:
      err_str = "Base_MMap_map failed";
      break;
    default:
      Base_errx(BASE_ERR_S_IN("Invalid Init_Code_t"));
      break;
  }
  Base_errx(BASE_ERR_S_IN(err_str));
}

#define FAILED_IN_(Sub, Main) "Error: %s failed in %s!\n", Sub, Main

void
Base_MMap_del
(Base_MMap* map)
{
  if (map->ptr && Base_MMap_unmap(map))
    Base_errx(FAILED_IN_("Base_MMap_unmap", "Base_MMap_del"));
  if ((map->file != BASE_NULL_FILE) &&
      Base_close_file(map->file))
    Base_errx(FAILED_IN_("Base_close_file","Base_MMap_del"));
  #ifdef BASE_MMAP_HAS_WIN_FMAPPING
  if ((map->win_fmapping != BASE_NULL_FILE) &&
      Base_close_file(map->win_fmapping))
    Base_errx("Error: Base_close_file failed for win_fmapping in Base_MMap_del!\n");
  #endif
  *map = BASE_MMAP_NULL_LITERAL;
}
