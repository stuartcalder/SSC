/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "operations.h"

#undef  R_
#define R_ BASE_RESTRICT

#define XOR_8_(U8p, Cu8p, Start) \
  U8p[(0 + Start)] ^= Cu8p[(0 + Start)];\
  U8p[(1 + Start)] ^= Cu8p[(1 + Start)];\
  U8p[(2 + Start)] ^= Cu8p[(2 + Start)];\
  U8p[(3 + Start)] ^= Cu8p[(3 + Start)];\
  U8p[(4 + Start)] ^= Cu8p[(4 + Start)];\
  U8p[(5 + Start)] ^= Cu8p[(5 + Start)];\
  U8p[(6 + Start)] ^= Cu8p[(6 + Start)];\
  U8p[(7 + Start)] ^= Cu8p[(7 + Start)]
#define XOR_16_(U8p, Cu8p, Start) \
  XOR_8_(U8p, Cu8p, (0 + Start));\
  XOR_8_(U8p, Cu8p, (8 + Start))
#define XOR_32_(U8p, Cu8p, Start) \
  XOR_16_(U8p, Cu8p, (0  + Start));\
  XOR_16_(U8p, Cu8p, (16 + Start))
#define XOR_64_(U8p, Cu8p, Start) \
  XOR_32_(U8p, Cu8p, (0  + Start));\
  XOR_32_(U8p, Cu8p, (32 + Start))
#define XOR_128_(U8p, Cu8p, Start) \
  XOR_64_(U8p, Cu8p, (0  + Start));\
  XOR_64_(U8p, Cu8p, (64 + Start))

void
Base_xor_16(void* R_ writeto, const void* R_ readfrom)
{
  uint8_t*       first  = (uint8_t*)      writeto;
  const uint8_t* second = (const uint8_t*)readfrom;
  XOR_16_(first, second, 0);
}

void
Base_xor_32(void* R_ writeto, const void* R_ readfrom)
{
  uint8_t*       first  = (uint8_t*)      writeto;
  const uint8_t* second = (const uint8_t*)readfrom;
  XOR_32_(first, second, 0);
}

void
Base_xor_64(void* R_ writeto, const void* R_ readfrom)
{
  uint8_t*       first  = (uint8_t*)      writeto;
  const uint8_t* second = (const uint8_t*)readfrom;
  XOR_64_(first, second, 0);
}

void
Base_xor_128 (void* R_ writeto, const void* R_ readfrom)
{
  uint8_t*       first  = (uint8_t*)      writeto;
  const uint8_t* second = (const uint8_t*)readfrom;
  XOR_128_(first, second, 0);
}

size_t
Base_ctime_memdiff (const void* R_ v_0, const void* R_ v_1, size_t size)
{
  BASE_ASSERT(v_0 && v_1);
  const uint8_t* const mem0 = (const uint8_t*)v_0;
  const uint8_t* const mem1 = (const uint8_t*)v_1;
  /* Count the number of byte indices of @mem0 and @mem1 that are unequal. */
  size_t unequal_count = 0;
  for (size_t i = 0; i < size; ++i) {
    uint8_t u8_bits = mem0[i] ^ mem1[i];
    /* Bit-index 0 absorbs 1 bits from bit-indices 1 through 7. */
    u8_bits |= ((u8_bits >> 7)|
                (u8_bits >> 6)|
	        (u8_bits >> 5)|
	        (u8_bits >> 4)|
	        (u8_bits >> 3)|
	        (u8_bits >> 2)|
	        (u8_bits >> 1));
    /* We do integer addition.
     * If bit-index 0 was 1, this will increment @unequal_count,
     * else the bytes were equal.
     * This should take the same amount of time in either case. */
    unequal_count += (u8_bits & UINT8_C(0x01));
  }
  return unequal_count;
}

bool
Base_is_zero (const void* R_ v, size_t n_bytes)
{
  BASE_ASSERT(v);
  const uint8_t* mem = (const uint8_t*)v;
  for (size_t i = 0; i < n_bytes; ++i)
    if (mem[i])
      return false;
  return true;
}

bool
Base_ctime_is_zero (const void* R_ v, size_t n_bytes)
{
  BASE_ASSERT(v);
  const uint8_t* mem = (const uint8_t*)v;
  uint8_t zero_test = 0;
  for (size_t i = 0; i < n_bytes; ++i) {
    /* @zero_test absorbs all the 1 bits. */
    zero_test |= mem[i];
  }
  /* If any 1 bits were absorbed, the memory range
   * was not all zeroes. */
  return !zero_test;
}
