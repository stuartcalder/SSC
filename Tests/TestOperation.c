/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "Operation.h"

int main(void)
{
  /* --- Bitwise rotation: known values. --- */
  TEST_CHECK(SSC_rotateLeft16(0x8000, 1) == 0x0001);
  TEST_CHECK(SSC_rotateRight16(0x0001, 1) == 0x8000);
  TEST_CHECK(SSC_rotateLeft32(0x80000000u, 1) == 0x00000001u);
  TEST_CHECK(SSC_rotateRight32(0x00000001u, 1) == 0x80000000u);
  TEST_CHECK(SSC_rotateLeft64(UINT64_C(0x8000000000000000), 1) == UINT64_C(1));
  TEST_CHECK(SSC_rotateRight64(UINT64_C(1), 1) == UINT64_C(0x8000000000000000));

  /* Byte-pattern values rotate whole bytes. */
  TEST_CHECK(SSC_rotateLeft32(0x01020304u, 8) == 0x02030401u);
  TEST_CHECK(SSC_rotateRight32(0x01020304u, 8) == 0x04010203u);
  TEST_CHECK(SSC_rotateLeft64(UINT64_C(0x0102030405060708), 16) == UINT64_C(0x0304050607080102));
  TEST_CHECK(SSC_rotateRight64(UINT64_C(0x0102030405060708), 40) == UINT64_C(0x0405060708010203));

  /* Counts are masked to the value width: zero, full-width, and
   * multiples of the width are all identity. */
  TEST_CHECK(SSC_rotateLeft16(0xA5A5, 0) == 0xA5A5);
  TEST_CHECK(SSC_rotateRight16(0xA5A5, 0) == 0xA5A5);
  TEST_CHECK(SSC_rotateLeft32(0xDEADBEEFu, 0) == 0xDEADBEEFu);
  TEST_CHECK(SSC_rotateRight64(UINT64_C(0xCAFEBABEDEADBEEF), 0) == UINT64_C(0xCAFEBABEDEADBEEF));
  TEST_CHECK(SSC_rotateLeft16(0xA5A5, 16) == 0xA5A5);
  TEST_CHECK(SSC_rotateRight32(0xDEADBEEFu, 32) == 0xDEADBEEFu);
  TEST_CHECK(SSC_rotateLeft64(UINT64_C(0xCAFEBABEDEADBEEF), 64) == UINT64_C(0xCAFEBABEDEADBEEF));
  TEST_CHECK(SSC_rotateRight16(0xA5A5, 32) == 0xA5A5);

  /* Counts wrap modulo the width, including negative counts. */
  TEST_CHECK(SSC_rotateLeft16(0x8000, 17) == SSC_rotateLeft16(0x8000, 1));
  TEST_CHECK(SSC_rotateRight32(0x01020304u, 40) == SSC_rotateRight32(0x01020304u, 8));
  TEST_CHECK(SSC_rotateLeft64(UINT64_C(0x0102030405060708), 72) == SSC_rotateLeft64(UINT64_C(0x0102030405060708), 8));
  TEST_CHECK(SSC_rotateRight16(0x0001, -1) == SSC_rotateLeft16(0x0001, 1));
  TEST_CHECK(SSC_rotateLeft32(0x01020304u, -8) == SSC_rotateRight32(0x01020304u, 8));

  /* Round-trip: rotating left then right by the same count restores the value. */
  {
    static const uint16_t v16[] = {0x0000, 0xFFFF, 0xA5A5, 0x8000, 0x0001};
    static const uint32_t v32[] = {0u, 0xFFFFFFFFu, 0x01020304u, 0xDEADBEEFu, 0x80000000u};
    static const uint64_t v64[] = {UINT64_C(0), UINT64_MAX, UINT64_C(0x0102030405060708), UINT64_C(0xFEDCBA9876543210), UINT64_C(0x8000000000000000)};
    static const int counts[] = {1, 3, 7, 15};
    size_t i, j;
    for (i = 0; i < sizeof(v16) / sizeof(v16[0]); ++i) {
      for (j = 0; j < sizeof(counts) / sizeof(counts[0]); ++j) {
        uint16_t rt = SSC_rotateRight16(SSC_rotateLeft16(v16[i], counts[j]), counts[j]);
        TEST_CHECK_MSG(rt == v16[i], "rotl/rotr16 round-trip: 0x%04X n=%d -> 0x%04X", (unsigned)v16[i], counts[j], (unsigned)rt);
      }
    }
    for (i = 0; i < sizeof(v32) / sizeof(v32[0]); ++i) {
      for (j = 0; j < sizeof(counts) / sizeof(counts[0]); ++j) {
        uint32_t rt = SSC_rotateRight32(SSC_rotateLeft32(v32[i], counts[j]), counts[j]);
        TEST_CHECK_MSG(rt == v32[i], "rotl/rotr32 round-trip: 0x%08X n=%d -> 0x%08X", (unsigned)v32[i], counts[j], (unsigned)rt);
      }
    }
    for (i = 0; i < sizeof(v64) / sizeof(v64[0]); ++i) {
      for (j = 0; j < sizeof(counts) / sizeof(counts[0]); ++j) {
        uint64_t rt = SSC_rotateRight64(SSC_rotateLeft64(v64[i], counts[j]), counts[j]);
        TEST_CHECK_MSG(rt == v64[i], "rotl/rotr64 round-trip: 0x%16llX n=%d -> 0x%16llX", (unsigned long long)v64[i], counts[j], (unsigned long long)rt);
      }
    }
  }

  /* --- XOR: SSC_xorN XORs the first N bytes of @writeto with @readfrom. --- */
  {
    uint8_t orig[128], key[128], key_orig[128], a[128], expected[128];
    size_t i;
    for (i = 0; i < sizeof(orig); ++i) {
      orig[i] = (uint8_t)(i * 7 + 3);
      key[i]  = (uint8_t)(0xFF - i);
    }
    memcpy(key_orig, key, sizeof(key));

    /* Each variant touches only its own byte range; the rest is untouched. */
    memcpy(a, orig, sizeof(a));
    memcpy(expected, a, sizeof(a));
    for (i = 0; i < 16; ++i) expected[i] ^= key[i];
    SSC_xor16(a, key);
    TEST_CHECK_MSG(memcmp(a, expected, sizeof(a)) == 0, "xor16: first 16 bytes XORed, rest untouched");

    memcpy(a, orig, sizeof(a));
    memcpy(expected, a, sizeof(a));
    for (i = 0; i < 32; ++i) expected[i] ^= key[i];
    SSC_xor32(a, key);
    TEST_CHECK_MSG(memcmp(a, expected, sizeof(a)) == 0, "xor32: first 32 bytes XORed, rest untouched");

    memcpy(a, orig, sizeof(a));
    memcpy(expected, a, sizeof(a));
    for (i = 0; i < 64; ++i) expected[i] ^= key[i];
    SSC_xor64(a, key);
    TEST_CHECK_MSG(memcmp(a, expected, sizeof(a)) == 0, "xor64: first 64 bytes XORed, rest untouched");

    memcpy(a, orig, sizeof(a));
    memcpy(expected, a, sizeof(a));
    for (i = 0; i < 128; ++i) expected[i] ^= key[i];
    SSC_xor128(a, key);
    TEST_CHECK_MSG(memcmp(a, expected, sizeof(a)) == 0, "xor128: all 128 bytes XORed");

    /* @readfrom is never modified. */
    TEST_CHECK_MSG(memcmp(key, key_orig, sizeof(key)) == 0, "xor: readfrom unmodified");

    /* An all-zero key leaves the buffer unchanged. */
    {
      uint8_t zkey[128];
      memset(zkey, 0, sizeof(zkey));
      memcpy(a, orig, sizeof(a));
      SSC_xor64(a, zkey);
      TEST_CHECK_MSG(memcmp(a, orig, sizeof(a)) == 0, "xor with zero key: unchanged");
    }

    /* An all-ones key inverts each byte. */
    {
      uint8_t okey[128];
      memset(okey, 0xFF, sizeof(okey));
      memcpy(a, orig, sizeof(a));
      SSC_xor32(a, okey);
      for (i = 0; i < 32; ++i) expected[i] = (uint8_t)(~orig[i]);
      TEST_CHECK_MSG(memcmp(a, expected, 32) == 0, "xor with all-ones key: bytes inverted");
    }
  }

  /* --- SSC_secureZero. --- */
  {
    uint8_t buf[64];
    size_t i;
    for (i = 0; i < sizeof(buf); ++i) buf[i] = 0xA5;
    SSC_secureZero(buf, sizeof(buf));
    TEST_CHECK_MSG(SSC_isZero(buf, sizeof(buf)), "secureZero: full buffer zeroed");

    for (i = 0; i < sizeof(buf); ++i) buf[i] = (uint8_t)i;
    SSC_secureZero(buf, 16);
    TEST_CHECK_MSG(SSC_isZero(buf, 16), "secureZero: first 16 bytes zeroed");
    TEST_CHECK_MSG(buf[16] == 16 && buf[63] == 63, "secureZero: bytes past @n untouched");

    for (i = 0; i < sizeof(buf); ++i) buf[i] = 0x7F;
    SSC_secureZero(buf, 0); /* Zero-length call must be a no-op. */
    TEST_CHECK_MSG(!SSC_isZero(buf, sizeof(buf)), "secureZero: size 0 is a no-op");
  }

  /* --- SSC_constTimeMemDiff: counts differing bytes. --- */
  {
    uint8_t m0[32], m1[32];
    size_t i;
    for (i = 0; i < sizeof(m0); ++i) {
      m0[i] = (uint8_t)i;
      m1[i] = (uint8_t)i;
    }
    TEST_CHECK(SSC_constTimeMemDiff(m0, m1, sizeof(m0)) == 0);

    for (i = 0; i < sizeof(m1); ++i) m1[i] ^= 0xFF; /* Every byte differs. */
    TEST_CHECK_MSG(SSC_constTimeMemDiff(m0, m1, sizeof(m0)) == sizeof(m0), "all bytes differ");

    memcpy(m1, m0, sizeof(m1));
    m1[0] ^= 1;
    m1[31] ^= 1;
    TEST_CHECK_MSG(SSC_constTimeMemDiff(m0, m1, sizeof(m0)) == 2, "two bytes differ");

    /* Only the first @size bytes are compared. */
    memcpy(m1, m0, sizeof(m1));
    m1[31] = 0xEE;
    TEST_CHECK(SSC_constTimeMemDiff(m0, m1, 8) == 0);

    TEST_CHECK(SSC_constTimeMemDiff(m0, m1, 0) == 0);
  }

  /* --- SSC_isZero / SSC_constTimeIsZero. --- */
  {
    uint8_t buf[32];
    memset(buf, 0, sizeof(buf));
    TEST_CHECK(SSC_isZero(buf, sizeof(buf)));
    TEST_CHECK(SSC_constTimeIsZero(buf, sizeof(buf)));

    /* A single nonzero byte anywhere makes the range non-zero. */
    {
      static const size_t positions[] = {0, 1, 15, 31};
      size_t i;
      for (i = 0; i < sizeof(positions) / sizeof(positions[0]); ++i) {
        memset(buf, 0, sizeof(buf));
        buf[positions[i]] = 0x01;
        TEST_CHECK_MSG(!SSC_isZero(buf, sizeof(buf)), "isZero: nonzero at byte %zu", positions[i]);
        TEST_CHECK_MSG(!SSC_constTimeIsZero(buf, sizeof(buf)), "constTimeIsZero: nonzero at byte %zu", positions[i]);
      }
    }

    /* Zero-length ranges are trivially all-zero. */
    memset(buf, 0x5A, sizeof(buf));
    TEST_CHECK(SSC_isZero(buf, 0));
    TEST_CHECK(SSC_constTimeIsZero(buf, 0));
  }

  return TestUtil_Summary("TestOperation");
}
