#include "operations.h"

#define R_(ptr) ptr BASE_RESTRICT

#define XOR_8_(u8p, cu8p, start) \
  u8p[(0 + start)] ^= cu8p[(0 + start)]; \
  u8p[(1 + start)] ^= cu8p[(1 + start)]; \
  u8p[(2 + start)] ^= cu8p[(2 + start)]; \
  u8p[(3 + start)] ^= cu8p[(3 + start)]; \
  u8p[(4 + start)] ^= cu8p[(4 + start)]; \
  u8p[(5 + start)] ^= cu8p[(5 + start)]; \
  u8p[(6 + start)] ^= cu8p[(6 + start)]; \
  u8p[(7 + start)] ^= cu8p[(7 + start)]
#define XOR_16_(u8p, cu8p, start) \
  XOR_8_(u8p, cu8p, (0 + start)); \
  XOR_8_(u8p, cu8p, (8 + start))
#define XOR_32_(u8p, cu8p, start) \
  XOR_16_(u8p, cu8p, (0  + start)); \
  XOR_16_(u8p, cu8p, (16 + start))
#define XOR_64_(u8p, cu8p, start) \
  XOR_32_(u8p, cu8p, (0  + start)); \
  XOR_32_(u8p, cu8p, (32 + start))
#define XOR_128_(u8p, cu8p, start) \
  XOR_64_(u8p, cu8p, (0  + start)); \
  XOR_64_(u8p, cu8p, (64 + start))

void
Base_xor_16
(R_(void*) base_, R_(const void*) add_)
{
  uint8_t*       base = (uint8_t*)      base_;
  const uint8_t* add  = (const uint8_t*)add_;
  XOR_16_(base, add, 0);
}

void
Base_xor_32
(R_(void*) base_, R_(const void*) add_)
{
  uint8_t*       base = (uint8_t*)      base_;
  const uint8_t* add  = (const uint8_t*)add_;
  XOR_32_(base, add, 0);
}

void
Base_xor_64
(R_(void*) base_, R_(const void*) add_)
{
  uint8_t*       base = (uint8_t*)      base_;
  const uint8_t* add  = (const uint8_t*)add_;
  XOR_64_(base, add, 0);
}

void
Base_xor_128
(R_(void*) base_, R_(const void*) add_)
{
  uint8_t*       base = (uint8_t*)      base_;
  const uint8_t* add  = (const uint8_t*)add_;
  XOR_128_(base, add, 0);
}

size_t Base_ctime_memdiff (R_(const void*) v_0, R_(const void*) v_1, size_t size) {
  BASE_ASSERT(v_0 && v_1);
  const uint8_t* const u8_0 = (const uint8_t*)v_0;
  const uint8_t* const u8_1 = (const uint8_t*)v_1;
  /* Count the number of byte indices of @u8_0 and @u8_1 that are unequal. */
  size_t unequal_count = 0;
  for (size_t i = 0; i < size; ++i) {
    uint8_t u8_bits = u8_0[i] ^ u8_1[i];
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
     * This should take the same amount of time in either case.
     */
    unequal_count += (u8_bits & UINT8_C(0x01));
  }
  return unequal_count;
}
bool Base_is_zero (R_(const void*) mem, size_t n_bytes) {
	BASE_ASSERT(mem);
	const uint8_t* b = (const uint8_t*)mem;
	for (size_t i = 0; i < n_bytes; ++i)
	  if (b[i])
	    return false;
	return true;
}
bool Base_ctime_is_zero (R_(const void*) mem, size_t n_bytes) {
	BASE_ASSERT(mem);
	const uint8_t* cu8p = (const uint8_t*)mem;
	uint8_t zero_test = 0;
	for (size_t i = 0; i < n_bytes; ++i) {
	  /* @zero_test absorbs all the 1 bits. */
	  zero_test |= cu8p[i];
	}
	/* If any 1 bits were absorbed, the memory range
	 * was not all zero'd.
	 */
	return !zero_test;
}
