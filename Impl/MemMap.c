/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "MemMap.h"
#define R_ SSC_RESTRICT

#if   defined(SSC_OS_UNIXLIKE)
 #define MAP_FAIL_ ((uint8_t*)MAP_FAILED)
#elif defined(SSC_OS_WINDOWS)
 #define MAP_FAIL_ ((uint8_t*)SSC_NULL)
 typedef DWORD Dw32_t;
#else
 #error "Unsupported."
#endif

SSC_Error_t SSC_MemMap_map(SSC_MemMap* map, bool readonly) 
{
#if    defined(SSC_OS_UNIXLIKE)
  const int rw = readonly ? PROT_READ : (PROT_READ|PROT_WRITE);
  map->ptr = (uint8_t*)mmap(SSC_NULL, map->size, rw, MAP_SHARED, map->file, 0);
  if (map->ptr == MAP_FAIL_) {
    map->ptr = SSC_NULL;
    return -1;
  }
#elif  defined(SSC_OS_WINDOWS)
  Dw32_t high, low, page_rw, map_rw;

  high = (Dw32_t)(((uint64_t)map->size & UINT64_C(0xffffffff00000000)) >> 32);
  low  = (Dw32_t)(((uint64_t)map->size & UINT64_C(0x00000000ffffffff))      );
  if (readonly) {
    page_rw = PAGE_READONLY;
    map_rw  = FILE_MAP_READ;
  }
  else {
    page_rw = PAGE_READWRITE;
    map_rw  = (FILE_MAP_READ|FILE_MAP_WRITE);
  }
  map->windows_filemap = CreateFileMappingA(map->file, SSC_NULL, page_rw, high, low, SSC_NULL);
  if (map->windows_filemap == SSC_FILE_NULL_LITERAL)
    return -1;
  map->ptr = (uint8_t*)MapViewOfFile(map->windows_filemap, map_rw, 0, 0, map->size);
  if (map->ptr == MAP_FAIL_) {
    if (!SSC_File_close(map->windows_filemap))
      map->windows_filemap = SSC_FILE_NULL_LITERAL;
    return -1;
  }
#else
 #error "Unsupported operating system."
#endif
  map->readonly = readonly;
  return 0;
}

SSC_Error_t SSC_MemMap_unmap(SSC_MemMap* map)
{
  SSC_Error_t ret;
#if defined(SSC_OS_UNIXLIKE)
  ret = munmap(map->ptr, map->size);
  if (!ret) {
    map->ptr = SSC_NULL;
    map->readonly = false;
  }
#elif defined(SSC_OS_WINDOWS)
  ret = 0;
  if (!UnmapViewOfFile((LPCVOID)map->ptr))
    ret = -1;
  else
    map->ptr = SSC_NULL;
  if (SSC_File_close(map->windows_filemap))
    ret = -1;
  else
    map->windows_filemap = SSC_FILE_NULL_LITERAL;
  if (!ret)
    map->readonly = false;
#else
 #error "Unsupported operating system."
#endif
  return ret;
}

#define RONLY_       SSC_MEMMAP_INIT_READONLY
#define ALLOWSHRINK_ SSC_MEMMAP_INIT_ALLOWSHRINK
#define FEXIST_      SSC_MEMMAP_INIT_FORCE_EXIST
#define FEXIST_Y_    SSC_MEMMAP_INIT_FORCE_EXIST_YES

#define OK_                  SSC_MEMMAP_INIT_CODE_OK
#define ERR_FEXIST_NO_       SSC_MEMMAP_INIT_CODE_ERR_FEXIST_NO
#define ERR_FEXIST_YES_      SSC_MEMMAP_INIT_CODE_ERR_FEXIST_YES
#define ERR_READONLY_        SSC_MEMMAP_INIT_CODE_ERR_READONLY
#define ERR_SHRINK_          SSC_MEMMAP_INIT_CODE_ERR_SHRINK
#define ERR_NOSIZE_          SSC_MEMMAP_INIT_CODE_ERR_NOSIZE
#define ERR_OPEN_FILEPATH_   SSC_MEMMAP_INIT_CODE_ERR_OPEN_FILEPATH
#define ERR_CREATE_FILEPATH_ SSC_MEMMAP_INIT_CODE_ERR_CREATE_FILEPATH
#define ERR_GET_FILE_SIZE_   SSC_MEMMAP_INIT_CODE_ERR_GET_FILE_SIZE
#define ERR_SET_FILE_SIZE_   SSC_MEMMAP_INIT_CODE_ERR_SET_FILE_SIZE
#define ERR_MAP_             SSC_MEMMAP_INIT_CODE_ERR_MAP

SSC_CodeError_t SSC_MemMap_init(
 SSC_MemMap* R_ map,
 const char* R_ filepath,
 size_t         size,
 SSC_BitFlag_t  flags)
{
  bool exists, readonly, allowshrink, setsize;

  exists = SSC_FilePath_exists(filepath);
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
    }
    else {
    /* We are forcing non-existence. */
      if (exists)
        return ERR_FEXIST_NO_;
    }
  }
  /* Does the file exist? */

  /* It does exist. */
  if (exists) {
    /* It does. Open it and store the SSC_File_t in @map. */
    if (SSC_FilePath_open(filepath, readonly, &map->file))
      return ERR_OPEN_FILEPATH_;
    /* Store the size of the file in @map->size. */
    if (SSC_File_getSize(map->file, &map->size))
      return ERR_GET_FILE_SIZE_;
    /* When not readonly and a size has been requested by the caller... */
    if (!readonly && size > 0) {
      /* ... and the stored size in @map exceeds the size requested by the caller... */
      if (map->size > size) {
        /* ... only allow it when we are allowing shrinkage. */
        if (!allowshrink)
	        return ERR_SHRINK_;
      }
      /* ... and the stored size in @map equals the size requested by the caller ... */
      else if (map->size == size)
        setsize = false; /* ... no size change is necessary. */
    }
  }
  /* The file didn't exist. */
  else {
    /* Since it didn't exist requesting readonly makes no sense. */
    if (readonly)
      return ERR_READONLY_;
    /* Since it didn't exist a size must be provided by the caller. */
    if (size == 0)
      return ERR_NOSIZE_;
    /* Create the file at the @filepath and store the SSC_File_t in @map. */
    if (SSC_FilePath_create(filepath, &map->file))
      return ERR_CREATE_FILEPATH_;
  }
  if (setsize) {
    /* Set the size according to that specified by the caller. */
    map->size = size;
    if (SSC_File_setSize(map->file, map->size))
      return ERR_SET_FILE_SIZE_;
  }
  /* When we create a new file, it's implicitly readwrite, not readonly. */
  if (SSC_MemMap_map(map, readonly))
    return ERR_MAP_;
  return OK_;
}

void SSC_MemMap_initOrDie(
 SSC_MemMap*  R_ map,
 const char* R_  filepath,
 size_t          size,
 SSC_BitFlag_t   flags)
{
  SSC_CodeError_t ce = SSC_MemMap_init(map, filepath, size, flags);
  const char* err_str;
  switch (ce) {
    case OK_:
      return;
    case ERR_FEXIST_NO_:
      SSC_errx("Error:SSC_MemMap_initOrDie: Filepath ``%s'' exists, when it shouldn't!\n", filepath);
      break;
    case ERR_FEXIST_YES_:
      SSC_errx("Error:SSC_MemMap_initOrDie: Filepath ``%s'' does not exist, when it should!\n", filepath);
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
      err_str = "SSC_Filepath_open failed";
      break;
    case ERR_CREATE_FILEPATH_:
      err_str = "SSC_FilePath_create failed";
      break;
    case ERR_GET_FILE_SIZE_:
      err_str = "SSC_File_getSize failed";
      break;
    case ERR_SET_FILE_SIZE_:
      err_str = "SSC_File_setSize failed";
      break;
    case ERR_MAP_:
      err_str = "SSC_MemMap_map failed";
      break;
    default:
      err_str = "Invalid SSC_CodeError_t";
      break;
  }
  SSC_errx(SSC_ERR_S_IN(err_str));
}

void SSC_MemMap_del(SSC_MemMap* map)
{
  if (map->ptr && SSC_MemMap_unmap(map))
    SSC_errx(SSC_ERR_S_FAILED_IN("SSC_MemMap_unmap()"));
  if ((map->file != SSC_FILE_NULL_LITERAL) && SSC_File_close(map->file))
    SSC_errx(SSC_ERR_S_FAILED_IN("SSC_File_close()"));
  #ifdef SSC_MEMMAP_HAS_WINDOWS_FILEMAP
  if ((map->windows_filemap != SSC_FILE_NULL_LITERAL) && SSC_File_close(map->windows_filemap))
    SSC_errx(SSC_ERR_S_FAILED_IN("SSC_File_close()"));
  #endif
  *map = SSC_MEMMAP_NULL_LITERAL;
}

SSC_Error_t SSC_MemMap_sync(const SSC_MemMap* map)
SSC_MEMMAP_SYNC_IMPL(map)
