/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"

static const char Content[] = "0123456789abcdef"; /* 16 bytes. */

int main(void)
{
  char path[PATH_MAX];
  TestUtil_Path(path, sizeof(path), "seek_test.bin");

  SSC_File_t f;
  TEST_CHECK(SSC_FilePath_createOrTruncate(path, &f) == SSC_OK);
  SSC_ssize_t written = -1;
  TEST_CHECK(SSC_File_write(f, Content, sizeof(Content) - 1, &written) == SSC_FILE_WRITE_OK);

  uint8_t buf[64];
  SSC_ssize_t n = -1;

  /* seek(0) rewinds to the start of the file. */
  TEST_CHECK(SSC_File_seek(f, 0) == SSC_OK);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_read(f, buf, 16, &n) == SSC_FILE_READ_OK);
  TEST_CHECK(memcmp(buf, Content, 16) == 0);

  /* A mid-file seek reads the remaining bytes and stops at EOF. */
  TEST_CHECK(SSC_File_seek(f, 8) == SSC_OK);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_read(f, buf, 16, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 8, "read count = %" SSC_SSIZE_PRI, n);
  TEST_CHECK(memcmp(buf, "89abcdef", 8) == 0);

  /* Seeking past end-of-file is legal; a read there returns EOF immediately. */
  TEST_CHECK(SSC_File_seek(f, 100) == SSC_OK);
  TEST_CHECK(SSC_File_read(f, buf, 4, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 0, "read count = %" SSC_SSIZE_PRI, n);

  /* A negative offset seeks to true end-of-file: position == file size. */
  TEST_CHECK(SSC_File_seek(f, SSC_FILE_SEEK_END) == SSC_OK);
  TEST_CHECK(SSC_File_read(f, buf, 8, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 0, "read count = %" SSC_SSIZE_PRI, n);

  /* Any negative offset means end-of-file. */
  TEST_CHECK(SSC_File_seek(f, -42) == SSC_OK);
  TEST_CHECK(SSC_File_read(f, buf, 8, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 0, "read count = %" SSC_SSIZE_PRI, n);

  /* seekRead combines a seek and a read. */
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(f, 4, buf, 8, &n) == SSC_FILE_READ_OK);
  TEST_CHECK(memcmp(buf, "456789ab", 8) == 0);

  /* seekRead that overruns end-of-file returns the partial count. */
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(f, 12, buf, 8, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 4, "read count = %" SSC_SSIZE_PRI, n);
  TEST_CHECK(memcmp(buf, "cdef", 4) == 0);

  /* seekRead at end-of-file returns EOF with a zero partial count. */
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(f, SSC_FILE_SEEK_END, buf, 8, &n) == SSC_FILE_READ_EOF);
  TEST_CHECK_MSG(n == 0, "read count = %" SSC_SSIZE_PRI, n);

  /* seekWrite overwrites in place. */
  TEST_CHECK(SSC_File_seekWrite(f, 4, "WXYZ", 4, &written) == SSC_FILE_WRITE_OK);
  size_t size = (size_t)-1;
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == 16, "file size = %zu", size);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(f, 0, buf, 16, &n) == SSC_FILE_READ_OK);
  TEST_CHECK(memcmp(buf, "0123WXYZ89abcdef", 16) == 0);

  /* seekWrite at end-of-file (negative offset) appends. */
  TEST_CHECK(SSC_File_seekWrite(f, -1, "!?", 2, &written) == SSC_FILE_WRITE_OK);
  TEST_CHECK_MSG(written == 2, "written = %" SSC_SSIZE_PRI, written);
  TEST_CHECK_MSG(SSC_File_getSize(f, &size) == SSC_OK && size == 18, "file size = %zu", size);
  memset(buf, 0xAA, sizeof(buf));
  TEST_CHECK(SSC_File_seekRead(f, 0, buf, 18, &n) == SSC_FILE_READ_OK);
  TEST_CHECK(memcmp(buf, "0123WXYZ89abcdef!?", 18) == 0);

  SSC_File_close(f);
  TEST_CHECK(SSC_FilePath_delete(path) == SSC_OK);
  TestUtil_Cleanup();
  return TestUtil_Summary("TestFileSeek");
}
