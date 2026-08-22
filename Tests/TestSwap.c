/* Copyright (C) 2026 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "TestUtil.h"

#include "Memory.h" /* Also pulls in Swap.h. */

int main(void)
{
  /* SSC_swap16/32/64 always reverse byte order, regardless of host endianness. */
  TEST_CHECK(SSC_swap16(0x0102) == 0x0201);
  TEST_CHECK(SSC_swap16(0xABCD) == 0xCDAB);
  TEST_CHECK(SSC_swap32(0x01020304u) == 0x04030201u);
  TEST_CHECK(SSC_swap32(0xDEADBEEFu) == 0xEFBEADDEu);
  TEST_CHECK(SSC_swap64(0x0102030405060708ULL) == 0x0807060504030201ULL);
  TEST_CHECK(SSC_swap64(0xDEADBEEFCAFEBABEULL) == 0xBEBAFECAEFBEADDEULL);

  /* Edge values: zero, all-ones (a palindrome), and other palindromes are unchanged. */
  TEST_CHECK(SSC_swap16(0x0000) == 0x0000);
  TEST_CHECK(SSC_swap32(0x00000000u) == 0x00000000u);
  TEST_CHECK(SSC_swap64(0x0ULL) == 0x0ULL);
  TEST_CHECK(SSC_swap16(0xFFFF) == 0xFFFF);
  TEST_CHECK(SSC_swap32(0xFFFFFFFFu) == 0xFFFFFFFFu);
  TEST_CHECK(SSC_swap64(UINT64_MAX) == UINT64_MAX);
  TEST_CHECK(SSC_swap16(0x0101) == 0x0101);
  TEST_CHECK(SSC_swap32(0x01000001u) == 0x01000001u);
  TEST_CHECK(SSC_swap64(0x0100000000000001ULL) == 0x0100000000000001ULL);

  /* Single-byte values move to the opposite end. */
  TEST_CHECK(SSC_swap16(0x00FF) == 0xFF00);
  TEST_CHECK(SSC_swap32(0x000000FFu) == 0xFF000000u);
  TEST_CHECK(SSC_swap64(0x00000000000000FFULL) == 0xFF00000000000000ULL);

  /* Round-trip: swapping twice returns the original value. */
  {
    static const uint16_t v16[] = {0x0000, 0xFFFF, 0x0102, 0xA5A5, 0x8000};
    static const uint32_t v32[] = {0u, 0xFFFFFFFFu, 0x01020304u, 0x9ABCDEF0u, 0x80000000u};
    static const uint64_t v64[] = {0ULL, UINT64_MAX, 0x0102030405060708ULL, 0xFEDCBA9876543210ULL, 0x8000000000000000ULL};
    size_t i;
    for (i = 0; i < sizeof(v16) / sizeof(v16[0]); ++i) {
      uint16_t rt = SSC_swap16(SSC_swap16(v16[i]));
      TEST_CHECK_MSG(rt == v16[i], "swap16 round-trip: 0x%04X -> 0x%04X", (unsigned)v16[i], (unsigned)rt);
    }
    for (i = 0; i < sizeof(v32) / sizeof(v32[0]); ++i) {
      uint32_t rt = SSC_swap32(SSC_swap32(v32[i]));
      TEST_CHECK_MSG(rt == v32[i], "swap32 round-trip: 0x%08X -> 0x%08X", (unsigned)v32[i], (unsigned)rt);
    }
    for (i = 0; i < sizeof(v64) / sizeof(v64[0]); ++i) {
      uint64_t rt = SSC_swap64(SSC_swap64(v64[i]));
      TEST_CHECK_MSG(rt == v64[i], "swap64 round-trip: 0x%016llX -> 0x%016llX", (unsigned long long)v64[i], (unsigned long long)rt);
    }
  }

  /* TO_LE/TO_BE macros: identity for the host endianness, swap for the other. */
  {
    const uint16_t a16 = 0x0102;
    const uint32_t a32 = 0x01020304u;
    const uint64_t a64 = 0x0102030405060708ULL;
#if SSC_ENDIAN == SSC_ENDIAN_LITTLE
    TEST_CHECK(SSC_U16_TO_LE(a16) == a16);
    TEST_CHECK(SSC_U32_TO_LE(a32) == a32);
    TEST_CHECK(SSC_U64_TO_LE(a64) == a64);
    TEST_CHECK(SSC_U16_TO_BE(a16) == SSC_swap16(a16));
    TEST_CHECK(SSC_U32_TO_BE(a32) == SSC_swap32(a32));
    TEST_CHECK(SSC_U64_TO_BE(a64) == SSC_swap64(a64));
#else /* Big-endian host. */
    TEST_CHECK(SSC_U16_TO_BE(a16) == a16);
    TEST_CHECK(SSC_U32_TO_BE(a32) == a32);
    TEST_CHECK(SSC_U64_TO_BE(a64) == a64);
    TEST_CHECK(SSC_U16_TO_LE(a16) == SSC_swap16(a16));
    TEST_CHECK(SSC_U32_TO_LE(a32) == SSC_swap32(a32));
    TEST_CHECK(SSC_U64_TO_LE(a64) == SSC_swap64(a64));
#endif
  }

  /* Byte layout in memory is host-endianness independent: TO_BE must yield
   * big-endian bytes and TO_LE little-endian bytes. */
  {
    static const uint8_t be16[] = {0x01, 0x02};
    static const uint8_t le16[] = {0x02, 0x01};
    static const uint8_t be32[] = {0x01, 0x02, 0x03, 0x04};
    static const uint8_t le32[] = {0x04, 0x03, 0x02, 0x01};
    static const uint8_t be64[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    static const uint8_t le64[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint8_t buf[8];
    uint16_t t16;
    uint32_t t32;
    uint64_t t64;

    t16 = SSC_U16_TO_BE(0x0102); memcpy(buf, &t16, 2);
    TEST_CHECK_MSG(memcmp(buf, be16, 2) == 0, "U16_TO_BE layout: %02X %02X", buf[0], buf[1]);
    t32 = SSC_U32_TO_BE(0x01020304u); memcpy(buf, &t32, 4);
    TEST_CHECK_MSG(memcmp(buf, be32, 4) == 0, "U32_TO_BE layout: %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);
    t64 = SSC_U64_TO_BE(0x0102030405060708ULL); memcpy(buf, &t64, 8);
    TEST_CHECK_MSG(memcmp(buf, be64, 8) == 0, "U64_TO_BE layout");

    t16 = SSC_U16_TO_LE(0x0102); memcpy(buf, &t16, 2);
    TEST_CHECK_MSG(memcmp(buf, le16, 2) == 0, "U16_TO_LE layout: %02X %02X", buf[0], buf[1]);
    t32 = SSC_U32_TO_LE(0x01020304u); memcpy(buf, &t32, 4);
    TEST_CHECK_MSG(memcmp(buf, le32, 4) == 0, "U32_TO_LE layout: %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);
    t64 = SSC_U64_TO_LE(0x0102030405060708ULL); memcpy(buf, &t64, 8);
    TEST_CHECK_MSG(memcmp(buf, le64, 8) == 0, "U64_TO_LE layout");
  }

  /* Memory.h endian-aware load/store: the only in-repo consumers of SSC_swap*. */
  {
    static const uint8_t be16[] = {0x01, 0x02};
    static const uint8_t le16[] = {0x02, 0x01};
    static const uint8_t be32[] = {0x01, 0x02, 0x03, 0x04};
    static const uint8_t le32[] = {0x04, 0x03, 0x02, 0x01};
    static const uint8_t be64[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    static const uint8_t le64[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint8_t buf[8];

    SSC_storeLittleEndian16(buf, 0x0102);
    TEST_CHECK_MSG(memcmp(buf, le16, 2) == 0, "storeLE16 layout: %02X %02X", buf[0], buf[1]);
    TEST_CHECK(SSC_loadLittleEndian16(buf) == 0x0102);
    SSC_storeBigEndian16(buf, 0x0102);
    TEST_CHECK_MSG(memcmp(buf, be16, 2) == 0, "storeBE16 layout: %02X %02X", buf[0], buf[1]);
    TEST_CHECK(SSC_loadBigEndian16(buf) == 0x0102);

    SSC_storeLittleEndian32(buf, 0x01020304u);
    TEST_CHECK_MSG(memcmp(buf, le32, 4) == 0, "storeLE32 layout: %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);
    TEST_CHECK(SSC_loadLittleEndian32(buf) == 0x01020304u);
    SSC_storeBigEndian32(buf, 0x01020304u);
    TEST_CHECK_MSG(memcmp(buf, be32, 4) == 0, "storeBE32 layout: %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);
    TEST_CHECK(SSC_loadBigEndian32(buf) == 0x01020304u);

    SSC_storeLittleEndian64(buf, 0x0102030405060708ULL);
    TEST_CHECK_MSG(memcmp(buf, le64, 8) == 0, "storeLE64 layout");
    TEST_CHECK(SSC_loadLittleEndian64(buf) == 0x0102030405060708ULL);
    SSC_storeBigEndian64(buf, 0x0102030405060708ULL);
    TEST_CHECK_MSG(memcmp(buf, be64, 8) == 0, "storeBE64 layout");
    TEST_CHECK(SSC_loadBigEndian64(buf) == 0x0102030405060708ULL);

    /* Round-trips with asymmetric values. */
    SSC_storeLittleEndian16(buf, 0xA5A5);
    TEST_CHECK(SSC_loadLittleEndian16(buf) == 0xA5A5);
    SSC_storeBigEndian32(buf, 0xDEADBEEFu);
    TEST_CHECK(SSC_loadBigEndian32(buf) == 0xDEADBEEFu);
    SSC_storeLittleEndian64(buf, 0xCAFEBABEDEADBEEFULL);
    TEST_CHECK(SSC_loadLittleEndian64(buf) == 0xCAFEBABEDEADBEEFULL);
    SSC_storeBigEndian16(buf, 0x8000);
    TEST_CHECK(SSC_loadBigEndian16(buf) == 0x8000);
  }

  return TestUtil_Summary("TestSwap");
}
