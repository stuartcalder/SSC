/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"
#include "MemMap.h"

#define PATTERN_SIZE 64U

static void TestMemMap_FillPattern(uint8_t* buf, size_t n)
{
  size_t i;
  for (i = 0; i < n; ++i)
    buf[i] = (uint8_t)(i * 31U + 7U);
}

/* init() leaves map->file open on some error paths; close it if so. */
static void TestMemMap_CloseFile(SSC_MemMap* map)
{
  if (map->file != SSC_FILE_NULL_LITERAL) {
    TEST_CHECK(SSC_File_close(map->file) == SSC_OK);
    map->file = SSC_FILE_NULL_LITERAL;
  }
}

int main(void)
{
  char path[PATH_MAX];
  TestUtil_Path(path, sizeof(path), "memmap_init.bin");
  uint8_t pattern[PATTERN_SIZE];
  size_t size = (size_t)-1;
  TestMemMap_FillPattern(pattern, PATTERN_SIZE);

  /* A new file is created at the requested size and mapped read-write. */
  SSC_MemMap m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, PATTERN_SIZE, 0U) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == PATTERN_SIZE, "map size = %zu", m.size);
  TEST_CHECK(m.ptr != SSC_NULL);
  TEST_CHECK((m.flags & SSC_MEMMAP_FLAG_READONLY) == 0);
  TEST_CHECK_MSG(SSC_FilePath_getSize(path, &size) == SSC_OK && size == PATTERN_SIZE, "file size = %zu", size);

  /* Writes through the mapping persist to disk after sync and del. */
  memcpy(m.ptr, pattern, PATTERN_SIZE);
  TEST_CHECK(SSC_MemMap_sync(&m) == SSC_OK);
  SSC_MemMap_del(&m);
  TEST_CHECK(m.ptr == SSC_NULL);
  TEST_CHECK(m.file == SSC_FILE_NULL_LITERAL);
  TEST_CHECK_MSG(m.size == 0, "size after del = %zu", m.size);
  TEST_CHECK(m.flags == 0U);

  SSC_File_t f;
  uint8_t disk[PATTERN_SIZE];
  memset(disk, 0xAA, sizeof(disk));
  SSC_ssize_t n = -1;
  TEST_CHECK(SSC_FilePath_open(path, false, &f) == SSC_OK);
  TEST_CHECK(SSC_File_read(f, disk, PATTERN_SIZE, &n) == SSC_FILE_READ_OK);
  TEST_CHECK_MSG(n == (SSC_ssize_t)PATTERN_SIZE, "read count = %" SSC_SSIZE_PRI, n);
  TEST_CHECK(memcmp(disk, pattern, PATTERN_SIZE) == 0);
  TEST_CHECK(SSC_File_close(f) == SSC_OK);

  /* An existing file smaller than the requested size is grown; content preserved. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, PATTERN_SIZE * 2U, 0U) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == PATTERN_SIZE * 2U, "map size = %zu", m.size);
  TEST_CHECK(memcmp(m.ptr, pattern, PATTERN_SIZE) == 0);
  uint8_t zeros[PATTERN_SIZE] = {0};
  TEST_CHECK(memcmp(m.ptr + PATTERN_SIZE, zeros, PATTERN_SIZE) == 0);
  TEST_CHECK_MSG(SSC_FilePath_getSize(path, &size) == SSC_OK && size == PATTERN_SIZE * 2U, "file size = %zu", size);
  SSC_MemMap_del(&m);

  /* Requesting the current size of an existing file is a no-op. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, PATTERN_SIZE * 2U, 0U) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == PATTERN_SIZE * 2U, "map size = %zu", m.size);
  TEST_CHECK(memcmp(m.ptr, pattern, PATTERN_SIZE) == 0);
  SSC_MemMap_del(&m);

  /* An existing file opened read-only is mapped at its current size. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, 0U, SSC_MEMMAP_INIT_READONLY) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == PATTERN_SIZE * 2U, "map size = %zu", m.size);
  TEST_CHECK((m.flags & SSC_MEMMAP_FLAG_READONLY) != 0);
  SSC_MemMap_del(&m);

  /* Shrinking an existing file is refused without the allow-shrink flag. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, PATTERN_SIZE, 0U) == SSC_MEMMAP_INIT_CODE_ERR_SHRINK);
  TEST_CHECK(m.ptr == SSC_NULL);
  TestMemMap_CloseFile(&m);
  TEST_CHECK_MSG(SSC_FilePath_getSize(path, &size) == SSC_OK && size == PATTERN_SIZE * 2U, "file size = %zu", size);

  /* ... and allowed with it. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, PATTERN_SIZE, SSC_MEMMAP_INIT_ALLOWSHRINK) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == PATTERN_SIZE, "map size = %zu", m.size);
  TEST_CHECK(memcmp(m.ptr, pattern, PATTERN_SIZE) == 0);
  TEST_CHECK_MSG(SSC_FilePath_getSize(path, &size) == SSC_OK && size == PATTERN_SIZE, "file size = %zu", size);
  SSC_MemMap_del(&m);

  /* Force-existence flags. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, 0U, SSC_MEMMAP_INIT_FORCE_EXIST) == SSC_MEMMAP_INIT_CODE_ERR_FEXIST_NO);
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(
   SSC_MemMap_init(&m, path, 0U, (SSC_MEMMAP_INIT_FORCE_EXIST|SSC_MEMMAP_INIT_FORCE_EXIST_YES)) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == PATTERN_SIZE, "map size = %zu", m.size);
  SSC_MemMap_del(&m);

  char mpath[PATH_MAX];
  TestUtil_Path(mpath, sizeof(mpath), "memmap_missing.bin");
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(
   SSC_MemMap_init(&m, mpath, 0U, (SSC_MEMMAP_INIT_FORCE_EXIST|SSC_MEMMAP_INIT_FORCE_EXIST_YES)) == SSC_MEMMAP_INIT_CODE_ERR_FEXIST_YES);
  TEST_CHECK(!SSC_FilePath_exists(mpath));
  /* Forcing non-existence of a missing file simply creates it. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, mpath, 32U, SSC_MEMMAP_INIT_FORCE_EXIST) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == 32U, "map size = %zu", m.size);
  TEST_CHECK(SSC_FilePath_exists(mpath));
  SSC_MemMap_del(&m);

  /* A new file requires a size. */
  char npath[PATH_MAX];
  TestUtil_Path(npath, sizeof(npath), "memmap_nosize.bin");
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, npath, 0U, 0U) == SSC_MEMMAP_INIT_CODE_ERR_NOSIZE);
  TEST_CHECK(!SSC_FilePath_exists(npath));

  /* A filepath whose parent directory does not exist cannot be created. */
  char badpath[PATH_MAX];
  snprintf(badpath, sizeof(badpath), "%s/nodir/memmap.bin", TestUtil_TempDir());
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, badpath, 16U, 0U) == SSC_MEMMAP_INIT_CODE_ERR_CREATE_FILEPATH);

  /* Mapping a zero-size file fails. */
  char epath[PATH_MAX];
  TestUtil_Path(epath, sizeof(epath), "memmap_empty.bin");
  TEST_CHECK(SSC_FilePath_createOrTruncate(epath, &f) == SSC_OK);
  TEST_CHECK(SSC_File_close(f) == SSC_OK);
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, epath, 0U, 0U) == SSC_MEMMAP_INIT_CODE_ERR_MAP);
  TEST_CHECK(m.ptr == SSC_NULL);
  TestMemMap_CloseFile(&m);

  /* Direct map()/sync()/unmap() on a manually prepared struct. */
  m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_FilePath_open(path, false, &m.file) == SSC_OK);
  TEST_CHECK(SSC_File_getSize(m.file, &m.size) == SSC_OK);
  TEST_CHECK_MSG(m.size > 0, "size = %zu", m.size);
  TEST_CHECK(SSC_MemMap_map(&m, false) == SSC_OK);
  TEST_CHECK(m.ptr != SSC_NULL);
  TEST_CHECK((m.flags & SSC_MEMMAP_FLAG_READONLY) == 0);
  TEST_CHECK(SSC_MemMap_sync(&m) == SSC_OK);
  /* Note: the Unix branch of SSC_MemMap_unmap() does not clear map->ptr on
   * success, so only the return code is checked here. */
  TEST_CHECK(SSC_MemMap_unmap(&m) == SSC_OK);
  /* Remapping read-only sets the readonly flag. */
  TEST_CHECK(SSC_MemMap_map(&m, true) == SSC_OK);
  TEST_CHECK(m.ptr != SSC_NULL);
  TEST_CHECK((m.flags & SSC_MEMMAP_FLAG_READONLY) != 0);
  TEST_CHECK(SSC_MemMap_unmap(&m) == SSC_OK);
  SSC_MemMap_del(&m);
  TEST_CHECK(m.ptr == SSC_NULL);
  TEST_CHECK(m.file == SSC_FILE_NULL_LITERAL);
  TEST_CHECK_MSG(m.size == 0, "size after del = %zu", m.size);
  TEST_CHECK(m.flags == 0U);

  unlink(path);
  unlink(mpath);
  unlink(epath);
  TestUtil_Cleanup();
  return TestUtil_Summary("TestMemMapInit");
}
