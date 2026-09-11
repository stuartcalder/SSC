/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"
#include "MemMap.h"

#define INITIAL_SIZE 32U

static void TestMemMap_FillPattern(uint8_t* buf, size_t n)
{
  size_t i;
  for (i = 0; i < n; ++i)
    buf[i] = (uint8_t)(i * 31U + 7U);
}

int main(void)
{
  char path[PATH_MAX];
  TestUtil_Path(path, sizeof(path), "memmap_resize.bin");
  uint8_t pattern[INITIAL_SIZE];
  size_t size = (size_t)-1;
  TestMemMap_FillPattern(pattern, INITIAL_SIZE);

  /* A NULL map and a struct with no open file are rejected. */
  TEST_CHECK(SSC_MemMap_resize(SSC_NULL, 16U) == SSC_ERR);
  SSC_MemMap n = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_resize(&n, 16U) == SSC_ERR);

  /* Initialize a map with known content. */
  SSC_MemMap m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&m, path, INITIAL_SIZE, 0U) == SSC_MEMMAP_INIT_CODE_OK);
  memcpy(m.ptr, pattern, INITIAL_SIZE);

  /* A readonly map cannot be resized and remains mapped. */
  SSC_MemMap ro = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_init(&ro, path, 0U, SSC_MEMMAP_INIT_READONLY) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK((ro.flags & SSC_MEMMAP_FLAG_READONLY) != 0);
  TEST_CHECK(SSC_MemMap_resize(&ro, INITIAL_SIZE * 2U) == SSC_ERR);
  TEST_CHECK(ro.ptr != SSC_NULL);
  TEST_CHECK_MSG(ro.size == INITIAL_SIZE, "size = %zu", ro.size);
  SSC_MemMap_del(&ro);

  /* Growing preserves existing content and zero-fills the new region. */
  TEST_CHECK(SSC_MemMap_resize(&m, INITIAL_SIZE * 2U) == SSC_OK);
  TEST_CHECK_MSG(m.size == INITIAL_SIZE * 2U, "size = %zu", m.size);
  TEST_CHECK(m.ptr != SSC_NULL);
  TEST_CHECK(memcmp(m.ptr, pattern, INITIAL_SIZE) == 0);
  uint8_t zeros[INITIAL_SIZE] = {0};
  TEST_CHECK(memcmp(m.ptr + INITIAL_SIZE, zeros, INITIAL_SIZE) == 0);
  TEST_CHECK_MSG(SSC_FilePath_getSize(path, &size) == SSC_OK && size == INITIAL_SIZE * 2U, "file size = %zu", size);

  /* Shrinking preserves the leading content. */
  TEST_CHECK(SSC_MemMap_resize(&m, INITIAL_SIZE / 2U) == SSC_OK);
  TEST_CHECK_MSG(m.size == INITIAL_SIZE / 2U, "size = %zu", m.size);
  TEST_CHECK(memcmp(m.ptr, pattern, INITIAL_SIZE / 2U) == 0);
  TEST_CHECK_MSG(SSC_FilePath_getSize(path, &size) == SSC_OK && size == INITIAL_SIZE / 2U, "file size = %zu", size);

  /* Resizing to the current size is a no-op. */
  TEST_CHECK(SSC_MemMap_resize(&m, INITIAL_SIZE / 2U) == SSC_OK);
  TEST_CHECK_MSG(m.size == INITIAL_SIZE / 2U, "size = %zu", m.size);
  TEST_CHECK(memcmp(m.ptr, pattern, INITIAL_SIZE / 2U) == 0);

  SSC_MemMap_del(&m);
  TEST_CHECK(m.ptr == SSC_NULL);
  TEST_CHECK(m.file == SSC_FILE_NULL_LITERAL);
  TEST_CHECK_MSG(m.size == 0, "size after del = %zu", m.size);
  TEST_CHECK(m.flags == 0U);

  TEST_CHECK(SSC_FilePath_delete(path) == SSC_OK);
  TestUtil_Cleanup();
  return TestUtil_Summary("TestMemMapResize");
}
