#include "operations.h"

#define GENERIC_SHIM_XOR_(bytes) \
	void SHIM_PUBLIC \
	shim_xor_##bytes (void * SHIM_RESTRICT mem, void const * SHIM_RESTRICT add) { \
		for( int i = 0; i < bytes; ++i ) { \
			((uint8_t *)      mem)[ i ] ^= \
			((uint8_t const *)add)[ i ]; \
		} \
	}

GENERIC_SHIM_XOR_ (16)
GENERIC_SHIM_XOR_ (32)
GENERIC_SHIM_XOR_ (64)
GENERIC_SHIM_XOR_ (128)

#ifndef SHIM_OPERATIONS_INLINE_OBTAIN_OS_ENTROPY
void SHIM_PUBLIC
shim_obtain_os_entropy (uint8_t * SHIM_RESTRICT mem, size_t size)
{
#if    defined (SHIM_OS_OSX) || \
      (defined (__NetBSD__) && (__NetBSD_Version__ < 1000000000)) || \
       defined (__Dragonfly__)
#	if    defined (SHIM_OS_OSX)
#		define DEV_RANDOM_ "/dev/random"
#	else
#		define DEV_RANDOM_ "/dev/urandom"
#	endif
	Shim_File_t random_dev = shim_open_existing_filepath( DEV_RANDOM_, true );
	if( read( random_dev, mem, size ) != ((ssize_t)size) )
		SHIM_ERRX ("Error: Failed to read from " DEV_RANDOM_ "\n");
	shim_close_file( random_dev );
#elif  defined (SHIM_OS_UNIXLIKE)
#	define MAX_ 256
	while( size > MAX_ ) {
		if( getentropy( mem, MAX_ ) != 0 )
			SHIM_ERRX ("Error: Failed to getentropy()\n");
		size -= MAX_;
		mem  += MAX_;
	}
	if( getentropy( mem, size ) != 0 )
		SHIM_ERRX ("Error: Failed to getentropy()\n");
#elif  defined (SHIM_OS_WINDOWS)
	BCRYPT_ALG_HANDLE balg_handle;
	if( BCryptOpenAlgorithmProvider( &balg_handle, L"RNG", NULL, 0 ) != STATUS_SUCCESS )
		SHIM_ERRX ("Error: BCryptOpenAlgorithmProvider() failed\n");
	if( BCryptGenRandom( balg_handle, mem, size, 0 ) != STATUS_SUCCESS )
		SHIM_ERRX ("Error: BCryptGenRandom() failed\n");
	if( BCryptCloseAlgorithmProvider( balg_handle, 0 ) != STATUS_SUCCESS )
		SHIM_ERRX ("Error: BCryptCloseAlgorithmProvider() failed\n");
#else
#	error "Unsupported OS."
#endif
}
#endif /* ~ SHIM_OPERATIONS_INLINE_OBTAIN_OS_ENTROPY */
int SHIM_PUBLIC
shim_ctime_memcmp (void const * SHIM_RESTRICT mem_0,
		   void const * SHIM_RESTRICT mem_1,
		   size_t const               size)
{
#define ONE_ UINT8_C (0x01)
	int unequal_count = 0;
	for( size_t i = 0; i < size; ++i ) {
		uint8_t const b = ((uint8_t const *)mem_0)[ i ] ^
				  ((uint8_t const *)mem_1)[ i ];
		unequal_count += (
				((b >> 7)       ) |
				((b >> 6) & ONE_) |
				((b >> 5) & ONE_) |
				((b >> 4) & ONE_) |
				((b >> 3) & ONE_) |
				((b >> 2) & ONE_) |
				((b >> 1) & ONE_) |
				((b     ) & ONE_)
				);
	}
	return unequal_count;
}

#ifdef SHIM_OPERATIONS_NO_INLINE_SWAP_FUNCTIONS

#define SWAP_F_(size, value) \
	SWAP_F_IMPL_ (size, value)
#if    defined (__OpenBSD__)
#	define SWAP_F_IMPL_(size, value) \
		swap##size( value )
#elif  defined (__FreeBSD__) || defined (__NetBSD__) || defined (__Dragonfly__)
#	define SWAP_F_IMPL_(size, value) \
		bswap##size( value )
#elif  defined (__gnu_linux__)
#	define SWAP_F_IMPL_(size, value) \
		bswap_##size( value )
#elif  defined (SHIM_OS_WINDOWS)
#	define SWAP_F_IMPL_(size, value) \
		_byteswap_##size( value )
#elif !defined (SHIM_OS_OSX)
#	error "Unsupported OS."
#endif

#if    defined (SHIM_OS_UNIXLIKE)
#	define SIZE_(unixlike, windows) unixlike
#elif  defined (SHIM_OS_WINDOWS)
#	define SIZE_(unixlike, windows) windows
#else
#	error "Unsupported OS."
#endif


uint16_t SHIM_PUBLIC
shim_swap_16 (uint16_t val)
{
#ifdef SHIM_OS_OSX
	return (val >> 8) | (val << 8);
#else
	return SWAP_F_ (SIZE_ (16, ushort), val);
#endif
}
uint32_t SHIM_PUBLIC
shim_swap_32 (uint32_t val)
{
#ifdef SHIM_OS_OSX
	return (
		 (val >> (3*8)) |
		((val >>    8) & UINT32_C (0x0000ff00)) |
		((val <<    8) & UINT32_C (0x00ff0000)) |
		 (val << (3*8))
	       );
#else
	return SWAP_F_ (SIZE_ (32, ulong), val);
#endif
}
uint64_t SHIM_PUBLIC
shim_swap_64 (uint64_t val)
{
#ifdef SHIM_OS_OSX
	return (
		 (val >> (7*8))                                  |
		((val >> (5*8)) & UINT64_C (0x000000000000ff00)) |
		((val >> (3*8)) & UINT64_C (0x0000000000ff0000)) |
		((val >>    8 ) & UINT64_C (0x00000000ff000000)) |
		((val <<    8 ) & UINT64_C (0x000000ff00000000)) |
		((val << (3*8)) & UINT64_C (0x0000ff0000000000)) |
		((val << (5*8)) & UINT64_C (0x00ff000000000000)) |
		 (val << (7*8))
	       );
#else
	return SWAP_F_ (SIZE_ (64, uint64), val);
#endif
}

#endif /* ~ SHIM_OPERATIONS_NO_INLINE_SWAP_FUNCTIONS */
