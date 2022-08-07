/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include "errors.h"
#include "files.h"
#include "mmap.h"
#define R_(Ptr) Ptr BASE_RESTRICT

#if   defined(BASE_OS_UNIXLIKE)
# define MAP_FAIL_ ((uint8_t*)MAP_FAILED)
#elif defined(BASE_OS_WINDOWS)
# define MAP_FAIL_ ((uint8_t*)BASE_NULL)
  typedef DWORD Dw32_t;
#else
# error "Unsupported."
#endif

int Base_MMap_map(Base_MMap* map, bool readonly) 
{
#if    defined(BASE_OS_UNIXLIKE)
  const int rw = readonly ? PROT_READ : (PROT_READ|PROT_WRITE);
  if ((map->ptr = (uint8_t*)mmap(BASE_NULL, map->size, rw, MAP_SHARED, map->file, 0)) == MAP_FAIL_) {
    map->ptr = BASE_NULL;
    return -1;
  }
#elif  defined(BASE_OS_WINDOWS)
# ifdef BASE_STATIC_ASSERT_IS_NIL
  if (CHAR_BIT != 8)       Base_errx("CHAR_BIT must be 8 on line %d of file %s!", __LINE__, __FILE__);
  if (sizeof(Dw32_t) != 4) Base_errx("Dw32_t should be 4 bytes on line %d of file %s!", __LINE__, __FILE__);
# else
  BASE_STATIC_ASSERT(CHAR_BIT == 8, "CHAR_BIT must be 8!");
  BASE_STATIC_ASSERT(sizeof(Dw32_t) == 4, "Dw32_t should be 4 bytes!");
# endif
  Dw32_t high, low, page_rw, map_rw;

  high = (Dw32_t)(((uint_fast64_t)map->size & UINT64_C(0xffffffff00000000)) >> 32);
  low  = (Dw32_t)(((uint_fast64_t)map->size & UINT64_C(0x00000000ffffffff))      );
  if (readonly) {
    page_rw = PAGE_READONLY;
    map_rw  = FILE_MAP_READ;
  } else {
    page_rw = PAGE_READWRITE;
    map_rw  = (FILE_MAP_READ|FILE_MAP_WRITE);
  }
  if ((map->win_fmapping = CreateFileMappingA(map->file, BASE_NULL, page_rw, high, low, BASE_NULL)) == BASE_FILE_NULL_LITERAL)
    return -1;
  if ((map->ptr = (uint8_t*)MapViewOfFile(map->win_fmapping, map_rw, 0, 0, map->size)) == MAP_FAIL_) {
    if (!Base_close_file(map->win_fmapping))
      map->win_fmapping = BASE_FILE_NULL_LITERAL;
    return -1;
  }
#else
# error "Unsupported operating system."
#endif
  map->readonly = readonly;
  return 0;
}

void Base_MMap_map_or_die(Base_MMap* map, bool readonly)
{
 Base_assert_msg(!Base_MMap_map(map, readonly), "Error: Base_MMap_map failed!\n");
}

int Base_MMap_unmap(Base_MMap* map) {
  int ret;
#if defined(BASE_OS_UNIXLIKE)
  ret = munmap(map->ptr, map->size);
  if (!ret) {
    map->ptr = BASE_NULL;
    map->readonly = false;
  }
#elif defined(BASE_OS_WINDOWS)
  ret = 0;
  if (!UnmapViewOfFile((LPCVOID)map->ptr))
    ret = -1;
  else
    map->ptr = BASE_NULL;
  if (Base_close_file(map->win_fmapping))
    ret = -1;
  else
    map->win_fmapping = BASE_FILE_NULL_LITERAL;
  if (!ret)
    map->readonly = false;
#else
# error "Unsupported operating system."
#endif
  return ret;
}

void Base_MMap_unmap_or_die(Base_MMap* map)
{
  Base_assert_msg(!Base_MMap_unmap(map), "Error: Base_MMap_unmap failed!\n");
}

#ifndef BASE_MMAP_SYNC_INLINE
int Base_MMap_sync(const Base_MMap* map) BASE_MMAP_SYNC_IMPL(map)
#endif /* ~ BASE_MMAP_SYNC_INLINE */

void Base_MMap_sync_or_die(const Base_MMap* map)
{
  Base_assert_msg(!Base_MMap_sync(map), "Error: Base_MMap_sync failed!\n");
}

#define RONLY_           BASE_MMAP_INIT_READONLY
#define ALLOWSHRINK_     BASE_MMAP_INIT_ALLOWSHRINK
#define FEXIST_          BASE_MMAP_INIT_FORCE_EXIST
#define FEXIST_Y_        BASE_MMAP_INIT_FORCE_EXIST_YES
typedef Base_MMap_Init_t Init_t;

#define OK_                   BASE_MMAP_INIT_CODE_OK
#define ERR_FEXIST_NO_        BASE_MMAP_INIT_CODE_ERR_FEXIST_NO
#define ERR_FEXIST_YES_       BASE_MMAP_INIT_CODE_ERR_FEXIST_YES
#define ERR_READONLY_         BASE_MMAP_INIT_CODE_ERR_READONLY
#define ERR_SHRINK_           BASE_MMAP_INIT_CODE_ERR_SHRINK
#define ERR_NOSIZE_           BASE_MMAP_INIT_CODE_ERR_NOSIZE
#define ERR_OPEN_FILEPATH_    BASE_MMAP_INIT_CODE_ERR_OPEN_FILEPATH
#define ERR_CREATE_FILEPATH_  BASE_MMAP_INIT_CODE_ERR_CREATE_FILEPATH
#define ERR_GET_FILE_SIZE_    BASE_MMAP_INIT_CODE_ERR_GET_FILE_SIZE
#define ERR_SET_FILE_SIZE_    BASE_MMAP_INIT_CODE_ERR_SET_FILE_SIZE
#define ERR_MAP_              BASE_MMAP_INIT_CODE_ERR_MAP
typedef Base_MMap_Init_Code_t Init_Code_t;

Init_Code_t Base_MMap_init
(R_(Base_MMap*)  map,
 R_(const char*) filepath,
 size_t          size,
 Init_t          flags)
{
  bool exists, readonly, allowshrink, setsize;

  exists = Base_filepath_exists(filepath);
  readonly = exists && (flags & RONLY_);
  allowshrink = (flags & ALLOWSHRINK_);
  /* We will set the size when the filepath
   * doesn't exist, and when it does exist and a size has been requested. */
  setsize = !readonly && (!exists || (size > 0));
  if (flags & FEXIST_) {
    if (flags & FEXIST_Y_) {
      /* We are forcing existence. */
      if (!exists)
        return ERR_FEXIST_YES_;
    } else {
      /* We are forcing non-existence. */
      if (exists)
        return ERR_FEXIST_NO_;
    }
  }
  if (exists) {
    if (Base_open_filepath(filepath, readonly, &map->file))
      return ERR_OPEN_FILEPATH_;
    if (Base_get_file_size(map->file, &map->size))
      return ERR_GET_FILE_SIZE_;
    if (!readonly) {
      if (map->size > size) {
        if (!allowshrink)
	  return ERR_SHRINK_;
      } else if (map->size == size)
        setsize = false;
    }
  } else {
    if (readonly)
      return ERR_READONLY_;
    if (!size)
      return ERR_NOSIZE_;
    if (Base_create_filepath(filepath, &map->file))
      return ERR_CREATE_FILEPATH_;
  }
  if (setsize) {
    map->size = size;
    if (Base_set_file_size(map->file, map->size))
      return ERR_SET_FILE_SIZE_;
  }
  /* When we create a new file, it implicitly
   * is readwrite, not readonly.
   */
  if (Base_MMap_map(map, readonly))
    return ERR_MAP_;
  return OK_;
}

void Base_MMap_init_or_die
(R_(Base_MMap*)  map,
 R_(const char*) filepath,
 size_t          size,
 Init_t          flags)
{
  Init_Code_t ic = Base_MMap_init(map, filepath, size, flags);
  const char* err_str;
  switch (ic) {
    case OK_:
      return;
    case ERR_FEXIST_NO_:
      Base_errx("Error:Base_MMap_init_or_die: Filepath ``%s'' exists, when it shouldn't!\n", filepath);
      break;
    case ERR_FEXIST_YES_:
      Base_errx("Error:Base_MMap_init_or_die: Filepath ``%s'' does not exist, when it should!\n", filepath);
      break;
    case ERR_READONLY_:
      err_str = "File creation prevented by readonly";
      break;
    case ERR_SHRINK_:
      err_str = "File shrinking disallowed";
      break;
    case ERR_NOSIZE_:
      err_str = "Size not provided for new filepath";
      break;
    case ERR_OPEN_FILEPATH_:
      err_str = "Base_open_filepath failed";
      break;
    case ERR_CREATE_FILEPATH_:
      err_str = "Base_create_filepath failed";
      break;
    case ERR_GET_FILE_SIZE_:
      err_str = "Base_get_file_size failed";
      break;
    case ERR_SET_FILE_SIZE_:
      err_str = "Base_set_file_size failed";
      break;
    case ERR_MAP_:
      err_str = "Base_MMap_map failed";
      break;
    default:
      err_str = "Invalid Init_t";
      break;
  }
  Base_errx(BASE_ERR_S_IN(err_str));
}

#define FAILED_IN_(Sub, Main) "Error: %s failed in %s!\n", Sub, Main

void Base_MMap_del(Base_MMap* map)
{
  if (map->ptr && Base_MMap_unmap(map))
    Base_errx(FAILED_IN_("Base_MMap_unmap", "Base_MMap_del"));
  if ((map->file != BASE_FILE_NULL_LITERAL) && Base_close_file(map->file))
    Base_errx(FAILED_IN_("Base_close_file","Base_MMap_del"));
  #ifdef BASE_MMAP_HAS_WIN_FMAPPING
  if ((map->win_fmapping != BASE_FILE_NULL_LITERAL) && Base_close_file(map->win_fmapping))
    Base_errx("Error: Base_close_file failed for win_fmapping in Base_MMap_del!\n");
  #endif
  *map = BASE_MMAP_NULL_LITERAL;
}
