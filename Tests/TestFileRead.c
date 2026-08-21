/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"

static const char Content[] = "The quick brown fox jumps over the lazy dog.";
#define CONTENT_SIZE (sizeof(Content) - 1)

int main(void)
{
  char path[PATH_MAX];
  TestUtil_Path(path, sizeof(path), "read_test.bin");

  /* Create an empty file and verify its size. */
  SSC_File_t f;
  TEST_CHECK(SSC_FilePath_createOrTruncate(path, &f) == SSC_OK);
  TEST_CHECK(SSC_FilePath_exists(path));
  size_t size = (size_t)-1;
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == 0, "new file size = %zu", size);

  /* A full write reports OK with the exact byte count. */
  SSC_ssize_t written = -1;
  TEST_CHECK(SSC_File_write(f, Content, CONTENT_SIZE, &written) == SSC_FILE_WRITE_OK);
  TEST_CHECK_MSG(written == (SSC_ssize_t)CONTENT_SIZE, "written = %" SSC_SSIZE_PRI, written);
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == CONTENT_SIZE, "file size = %zu", size);

  /* Rewind, then a full read reports OK with the exact byte count. */
  TEST_CHECK(SSC_File_seek(f, 0) == SSC_OK);
  uint8_t buf[256];
  memset(buf, 0xAA, sizeof(buf));
  SSC_ssize_t n = -1;
  TEST_CHECK(SSC_File_read(f, buf, CONTENT_SIZE, &n) == SSC_FILE_READ_OK);
  TEST_CHECK_MSG(n == (SSC_ssize_t)CONTENT_SIZE, "read count = %" SSC_SSIZE_PRI, n);
  TEST_CHECK(memcmp(buf, Content, CONTENT_SIZE) == 0);

  /* Reading at end-of-file returns EOF with a zero partial count. */
  TEST_CHECK(SSC_File_read(f, buf, 16, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 0, "read count = %" SSC_SSIZE_PRI, n);

  /* A read that overruns end-of-file returns EOF with the partial count. */
  TEST_CHECK(SSC_File_seek(f, 0) == SSC_OK);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_read(f, buf, CONTENT_SIZE + 8, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == (SSC_ssize_t)CONTENT_SIZE, "read count = %" SSC_SSIZE_PRI, n);
  TEST_CHECK(memcmp(buf, Content, CONTENT_SIZE) == 0);

  /* Reading an empty file returns EOF with a zero partial count. */
  char epath[PATH_MAX];
  TestUtil_Path(epath, sizeof(epath), "empty.bin");
  SSC_File_t ef;
  TEST_CHECK(SSC_FilePath_createOrTruncate(epath, &ef) == SSC_OK);
  TEST_CHECK(SSC_File_read(ef, buf, 4, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 0, "read count = %" SSC_SSIZE_PRI, n);

  /* Error paths. */
  char mpath[PATH_MAX];
  TestUtil_Path(mpath, sizeof(mpath), "missing.bin");
  TEST_CHECK(!SSC_FilePath_exists(mpath));
  SSC_File_t mf;
  TEST_CHECK(SSC_FilePath_open(mpath, true, &mf) == SSC_ERR);

  /* Writing through a read-only handle fails. */
  SSC_File_t ro;
  TEST_CHECK(SSC_FilePath_open(path, true, &ro) == SSC_OK);
  written = -1;
  TEST_CHECK(SSC_File_write(ro, "x", 1, &written) == SSC_FILE_WRITE_ERR);

  SSC_File_close(f);
  SSC_File_close(ef);
  SSC_File_close(ro);
  unlink(path);
  unlink(epath);
  TestUtil_Cleanup();
  return TestUtil_Summary("TestFileRead");
}
