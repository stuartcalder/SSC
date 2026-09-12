/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"

int main(void)
{
  /* A missing path is neither a file nor a directory. */
  char mpath[PATH_MAX];
  TestUtil_Path(mpath, sizeof(mpath), "missing");
  TEST_CHECK(!SSC_FilePath_exists(mpath));
  TEST_CHECK(!SSC_DirPath_exists(mpath));

  /* A regular file is a file but not a directory. */
  char fpath[PATH_MAX];
  TestUtil_Path(fpath, sizeof(fpath), "regular.bin");
  SSC_File_t f;
  TEST_CHECK(SSC_FilePath_createOrTruncate(fpath, &f) == SSC_OK);
  SSC_File_close(f);
  TEST_CHECK(SSC_FilePath_exists(fpath));
  TEST_CHECK(!SSC_DirPath_exists(fpath));

  /* A directory is a directory but not a file. */
  char dpath[PATH_MAX];
  TestUtil_Path(dpath, sizeof(dpath), "subdir");
  TEST_CHECK(mkdir(dpath, (mode_t)0700) == 0);
  TEST_CHECK(SSC_DirPath_exists(dpath));
  TEST_CHECK(!SSC_FilePath_exists(dpath));

  /* The temp root itself is a directory. */
  TEST_CHECK(SSC_DirPath_exists(TestUtil_TempDir()));

  /* Cleanup: remove the file and subdirectory before removing the temp root. */
  TEST_CHECK(SSC_FilePath_delete(fpath) == SSC_OK);
  TEST_CHECK(rmdir(dpath) == 0);
  TestUtil_Cleanup();
  return TestUtil_Summary("TestDirPathExists");
}
