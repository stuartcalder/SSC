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

size_t Base_ctime_memdiff (R_(const void*) m_0, R_(const void*) m_1, size_t size) {
  BASE_ASSERT(m_0 && m_1);
  const uint8_t* b_0 = (const uint8_t*)m_0;
  const uint8_t* b_1 = (const uint8_t*)m_1;
  size_t unequal_count = 0;
  for (size_t i = 0; i < size; ++i) {
    uint8_t b = b_0[i] ^ b_1[i];
    b |= ((b >> 7)|
          (b >> 6)|
	  (b >> 5)|
	  (b >> 4)|
	  (b >> 3)|
	  (b >> 2)|
	  (b >> 1));
    unequal_count += (b & UINT8_C(0x01));
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
	const uint8_t* b = (const uint8_t*)mem;
	uint8_t zero_test = 0;
	for (size_t i = 0; i < n_bytes; ++i)
	  zero_test |= b[i];
	return !zero_test;
}
