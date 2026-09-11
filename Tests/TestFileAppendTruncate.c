/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"

int main(void)
{
  char path[PATH_MAX];
  TestUtil_Path(path, sizeof(path), "append_truncate.bin");

  uint8_t buf[64];
  SSC_ssize_t n = -1;
  size_t size = (size_t)-1;

  /* createOrTruncate creates a new empty file. */
  TEST_CHECK(!SSC_FilePath_exists(path));
  SSC_File_t f;
  TEST_CHECK(SSC_FilePath_createOrTruncate(path, &f) == SSC_OK);
  TEST_CHECK(SSC_FilePath_exists(path));
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == 0, "file size = %zu", size);

  /* Write initial content. */
  SSC_ssize_t written = -1;
  TEST_CHECK(SSC_File_write(f, "Hello, ", 7, &written) == SSC_FILE_WRITE_OK);
  TEST_CHECK_MSG(written == 7, "written = %" SSC_SSIZE_PRI, written);
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == 7, "file size = %zu", size);
  TEST_CHECK(SSC_File_close(f) == SSC_OK);

  /* openAppend writes after existing content, even if the handle is seeked first. */
  SSC_File_t a;
  TEST_CHECK(SSC_FilePath_openAppend(path, &a) == SSC_OK);
  TEST_CHECK(SSC_File_seek(a, 0) == SSC_OK);
  TEST_CHECK(SSC_File_write(a, "World!", 6, &written) == SSC_FILE_WRITE_OK);
  TEST_CHECK_MSG(written == 6, "written = %" SSC_SSIZE_PRI, written);
  TEST_CHECK_MSG(SSC_File_getSize(a, &size) == SSC_OK && size == 13, "file size = %zu", size);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(a, 0, buf, 13, &n) == SSC_FILE_READ_OK);
  TEST_CHECK(memcmp(buf, "Hello, World!", 13) == 0);
  TEST_CHECK(SSC_File_close(a) == SSC_OK);

  /* createOrTruncate truncates an existing non-empty file to zero bytes. */
  TEST_CHECK(SSC_FilePath_createOrTruncate(path, &f) == SSC_OK);
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == 0, "file size = %zu", size);
  TEST_CHECK(SSC_File_write(f, "Bye", 3, &written) == SSC_FILE_WRITE_OK);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(f, 0, buf, 3, &n) == SSC_FILE_READ_OK);
  TEST_CHECK(memcmp(buf, "Bye", 3) == 0);
  TEST_CHECK(SSC_File_close(f) == SSC_OK);

  /* openAppend requires the file to exist. */
  char mpath[PATH_MAX];
  TestUtil_Path(mpath, sizeof(mpath), "missing_append.bin");
  SSC_File_t ma;
  TEST_CHECK(SSC_FilePath_openAppend(mpath, &ma) == SSC_ERR);

  TEST_CHECK(SSC_FilePath_delete(path) == SSC_OK);
  TestUtil_Cleanup();
  return TestUtil_Summary("TestFileAppendTruncate");
}
