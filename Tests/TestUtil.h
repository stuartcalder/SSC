/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_TESTS_TEST_UTIL_H
#define SSC_TESTS_TEST_UTIL_H

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int TestUtil_Checks = 0;
static int TestUtil_Failures = 0;
static char TestUtil_TempDirPath[64] = {0};

#define TEST_CHECK_MSG(Cond, Fmt, ...) \
 do { \
  ++TestUtil_Checks; \
  if (!(Cond)) { \
   ++TestUtil_Failures; \
   fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
   fprintf(stderr, Fmt, ##__VA_ARGS__); \
   fputc('\n', stderr); \
  } \
 } while (0)

#define TEST_CHECK(Cond) TEST_CHECK_MSG((Cond), "%s", #Cond)

static inline int TestUtil_Summary(const char* name)
{
  if (TestUtil_Failures == 0) {
    printf("PASS %s (%d checks)\n", name, TestUtil_Checks);
    return 0;
  }
  fprintf(stderr, "FAIL %s: %d of %d checks failed\n", name, TestUtil_Failures, TestUtil_Checks);
  return 1;
}

static inline const char* TestUtil_TempDir(void)
{
  if (TestUtil_TempDirPath[0] == '\0') {
    snprintf(TestUtil_TempDirPath, sizeof(TestUtil_TempDirPath), "/tmp/ssc_test_XXXXXX");
    if (mkdtemp(TestUtil_TempDirPath) == NULL) {
      fprintf(stderr, "Error: mkdtemp() failed!\n");
      exit(2);
    }
  }
  return TestUtil_TempDirPath;
}

static inline void TestUtil_Cleanup(void)
{
  /* Files must already be unlinked by the caller. */
  rmdir(TestUtil_TempDir());
}

static inline void TestUtil_Path(char* out, size_t outsize, const char* name)
{
  snprintf(out, outsize, "%s/%s", TestUtil_TempDir(), name);
}

#endif /* ! SSC_TESTS_TEST_UTIL_H */
