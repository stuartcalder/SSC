/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "File.h"
#include "MemMap.h"

#ifdef SSC_MEMMAP_HAS_INITSECRET

#define SECRET_SIZE 256U

static void TestMemMap_FillPattern(uint8_t* buf, size_t n)
{
  size_t i;
  for (i = 0; i < n; ++i)
    buf[i] = (uint8_t)(i * 31U + 7U);
}

int main(void)
{
  if (!SSC_File_createSecretIsAvailable()) {
    printf("SKIP TestMemMapSecret: secret files are not available on this system\n");
    return 0;
  }

  /* A secret map is RAM-backed, read-write, and flagged as secret. */
  SSC_MemMap m = SSC_MEMMAP_NULL_LITERAL;
  TEST_CHECK(SSC_MemMap_initSecret(&m, SECRET_SIZE) == SSC_MEMMAP_INIT_CODE_OK);
  TEST_CHECK_MSG(m.size == SECRET_SIZE, "map size = %zu", m.size);
  TEST_CHECK(m.ptr != SSC_NULL);
  TEST_CHECK((m.flags & SSC_MEMMAP_FLAG_SECRET) != 0);
  TEST_CHECK((m.flags & SSC_MEMMAP_FLAG_READONLY) == 0);

  /* Write/read round-trip within the mapping. */
  uint8_t buf[SECRET_SIZE];
  TestMemMap_FillPattern(buf, SECRET_SIZE);
  memcpy(m.ptr, buf, SECRET_SIZE);
  uint8_t back[SECRET_SIZE];
  memset(back, 0xAA, sizeof(back));
  memcpy(back, m.ptr, SECRET_SIZE);
  TEST_CHECK(memcmp(back, buf, SECRET_SIZE) == 0);

  /* Secret maps are RAM-backed and never written out; sync() is a no-op success. */
  TEST_CHECK(SSC_MemMap_sync(&m) == SSC_OK);

  /* del() zeroes the secret buffer and resets the struct. */
  SSC_MemMap_del(&m);
  TEST_CHECK(m.ptr == SSC_NULL);
  TEST_CHECK(m.file == SSC_FILE_NULL_LITERAL);
  TEST_CHECK_MSG(m.size == 0, "size after del = %zu", m.size);
  TEST_CHECK(m.flags == 0U);

  return TestUtil_Summary("TestMemMapSecret");
}

#else /* ! SSC_MEMMAP_HAS_INITSECRET */

int main(void)
{
  printf("SKIP TestMemMapSecret: not supported on this platform\n");
  return 0;
}

#endif /* ~ if defined(SSC_MEMMAP_HAS_INITSECRET) */
