/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_OPERATIONS_H
#define SHIM_OPERATIONS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "macros.h"
#include "errors.h"

#if    defined (SHIM_OS_UNIXLIKE)
#	include <unistd.h>
#	if    defined (__OpenBSD__)
#		include <endian.h>
#	elif  defined (__FreeBSD__)
#		include <sys/endian.h>
#	elif  defined (__NetBSD__)
#		include <sys/types.h>
#		include <machine/bswap.h>
#		include <string.h>
#		include <sys/param.h>
#	elif  defined (__gnu_linux__)
#		include <byteswap.h>
#	elif  defined (SHIM_OS_OSX)
#		if   !defined (__STDC_WANT_LIB_EXT1__) || (__STDC_WANT_LIB_EXT1__ != 1)
#			error "The macro __STDC_WANT_LIB_EXT1__ msut be #defined to 1 for access to memset_s."
#		endif
#		include "files.h"
#		include <string.h>
#	else
#		error "Unsupported unix-like OS."
#	endif
#elif  defined (SHIM_OS_WINDOWS)
#	include <windows.h>
#	include <ntstatus.h>
#	include <bcrypt.h>
#	include <stdlib.h>
#else
#	error "Unsupported operating system."
#endif

#define UNSIGNED_MASK_T_(bits) \
	uint##bits_t

#define UNSIGNED_MASK_(bits) \
	((UNSIGNED_MASK_T_ (bits))((CHAR_BIT * sizeof(UNSIGNED_MASK_T_ (bits))) - 1))

#define MASKED_COUNT_(bits,count) \
	((UNSIGNED_MASK_ (bits)) & count)

#define SHIM_ROTATE_LEFT(value, count, bits) \
	static_assert (CHAR_BIT == 8, "Assume 8-bits bytes." ); \
	static_assert (bits == 16 || bits == 32 || bits == 64, "Invalid `bits` parameter."); \
	static_assert (count >= 1, "Cannot shift by less than 1"); \
	((value << MASK_COUNT_ (bits,count)) | (value >> ((-MASK_COUNT_ (bits,count)) & UNSIGNED_MASK_ (bits))))

#define SHIM_ROTATE_RIGHT(value, count, bits) \
	static_assert (CHAR_BIT == 8, "Assume 8-bits bytes." ); \
	static_assert (bits == 16 || bits == 32 || bits == 64, "Invalid `bits` parameter."); \
	static_assert (count >= 1, "Cannot shift by less than 1"); \
	((value >> MASK_COUNT_ (bits,count)) | (value << ((-MASK_COUNT_ (bits,count)) & UNSIGNED_MASK_ (bits))))

#undef MASKED_COUNT_
#undef UNSIGNED_MASK_
#undef UNSIGNED_MASK_T_

#define SHIM_XOR_BLOCK(block_p, add_p, block_bits) \
	static_assert (block_bits % CHAR_BIT == 0, "block_bits must be divisible into bytes."); \
	for( int unique_i = 0; i < (block_bits / CHAR_BIT); ++i ) \
		SHIM_REINTERPRET_CAST_VALUE_TO (block_p, uint8_t*      )[ unique_i ] ^= \
		SHIM_REINTERPRET_CAST_VALUE_TO (add_p  , uint8_t const*)[ unique_i ]

SHIM_BEGIN_DECLS

extern inline void
shim_obtain_os_entropy (uint8_t *buffer, size_t num_bytes) {
#if    defined (SHIM_OS_OSX) || (defined (__NetBSD__) && (__NetBSD_Version__ < 1000000000))
#	if    defined (SHIM_OS_OSX)
#		define DEV_RANDOM_ "/dev/random"
#	elif  defined (__NetBSD__)
#		define DEV_RANDOM_ "/dev/urandom"
#	else
#		error "This should be impossible."
#	endif
	Shim_File_t random_dev = shim_open_existing_filepath( DEV_RANDOM_, true );
	if( read( random_dev, buffer, num_bytes ) != SHIM_STATIC_CAST_VALUE_TO (num_bytes, ssize_t) )
		SHIM_ERRX ("Error: Failed to read from " DEV_RANDOM_ "\n");
	shim_close_file( random_dev );
#	undef DEV_RANDOM_
#elif  defined (SHIM_OS_UNIXLIKE)
#	define MAX_BYTES_ 256
	while( num_bytes > MAX_BYTES_ ) {
		if( getentropy( buffer, MAX_BYTES_ ) != 0 )
			SHIM_ERRX ("Error: Failed to getentropy()\n");
		num_bytes -= MAX_BYTES_;
		buffer    += MAX_BYTES_;
	}
	if( getentropy( buffer, num_bytes ) != 0 )
		SHIM_ERRX ("Error: Failed to getentropy()\n");
#	undef MAX_BYTES_
#elif  defined (SHIM_OS_WINDOWS)
	BCRYPT_ALG_HANDLE cng_provider_handle;
	if( BCryptOpenAlgorithmProvider( &cng_provider_handle, L"RNG", NULL, 0 ) != STATUS_SUCCESS )
		SHIM_ERRX ("Error: BCryptOpenAlgorithmProvider() failed\n");
	if( BCryptGenRandom( cng_provider_handle, buffer, num_bytes, 0 ) != STATUS_SUCCESS )
		SHIM_ERRX ("Error: BCryptGenRandom() failed\n");
	if( BCryptCloseAlgorithmProvider( cng_provider_handle, 0 ) != STATUS_SUCCESS )
		SHIM_ERRX ("Error: BCryptCloseAlgorithmProvider() failed\n");
#else
#	error "Unsupported operating system."
#endif
} // ~ shim_obtain_os_entropy(...)

extern inline void
shim_secure_zero (void *buffer, size_t num_bytes) {
#if    defined (SHIM_OS_OSX)
	SHIM_STATIC_CAST_VALUE_TO (memset_s( buffer, num_bytes, 0, num_bytes ), void);
#elif  defined (__NetBSD__)
	SHIM_STATIC_CAST_VALUE_TO (explicit_memset( buffer, 0, num_bytes ), void);
#elif  defined (SHIM_OS_UNIXLIKE)
	explicit_bzero( buffer, num_bytes );
#elif  defined (SHIM_OS_WINDOWS)
	SecureZeroMemory( buffer, num_bytes );
#else
#	error "Unsupported operating system."
#endif
} // ~ shim_secure_zero(...)

extern inline int
shim_ctime_memcmp (void const * SHIM_RESTRICT left,
		   void const * SHIM_RESTRICT right,
		   size_t const               size)
{
	int non_equal_bytes = 0;
#define ONE_MASK_ UINT8_C (0x01)
	for( size_t i = 0; i < size; ++i ) {
		uint8_t const b = SHIM_REINTERPRET_CAST_VALUE_TO(left ,uint8_t const*)[ i ] ^
			          SHIM_REINTERPRET_CAST_VALUE_TO(right,uint8_t const*)[ i ];
		non_equal_bytes += (
					((b >> 7)            ) |
					((b >> 6) & ONE_MASK_) |
					((b >> 5) & ONE_MASK_) |
					((b >> 4) & ONE_MASK_) |
					((b >> 3) & ONE_MASK_) |
					((b >> 2) & ONE_MASK_) |
					((b     ) & ONE_MASK_)
				   );
	}
	return non_equal_bytes;
#undef ONE_MASK_
}

#define SWAP_F_(size,u)	SWAP_F_IMPL_ (size,u)

#if    defined (__OpenBSD__)
#	define SWAP_F_IMPL_(size,u)	swap##size( u )
#elif  defined (__FreeBSD__) || defined (__NetBSD__)
#	define SWAP_F_IMPL_(size,u)	bswap##size( u )
#elif  defined (__gnu_linux__)
#	define SWAP_F_IMPL_(size,u)	bswap_##size( u )
#elif  defined (SHIM_OS_WINDOWS)
#	define SWAP_F_IMPL_(size,u)	_byteswap_##size( u )
#elif !defined (SHIM_OS_OSX)
#	error "Unsupported operating system."
#endif

#if    defined (SHIM_OS_UNIXLIKE) && !defined (SHIM_OS_OSX)
#	define SIZE_(unixlike, windows)	unixlike
#elif  defined (SHIM_OS_WINDOWS)
#	define SIZE_(unixlike, windows) windows
#elif !defined (SHIM_OS_OSX)
#	error "Unsupported operating system."
#endif

extern inline uint16_t
shim_swap_16 (uint16_t val) {
#ifdef SHIM_OS_OSX
	return (val >> 8) | (val << 8);
#else
	return SWAP_F_ (SIZE_ (16,ushort),val);
#endif
}

extern inline uint32_t
shim_swap_32 (uint32_t val) {
#ifdef SHIM_OS_OSX
	return (((val >> (3*8))) | \
		((val >> 8    ) & UINT32_C (0x0000ff00)) | \
		((val << 8    ) & UINT32_C (0x00ff0000)) | \
		((val << (3*8))));
#else
	return SWAP_F_ (SIZE_ (32,ulong),val);
#endif
}

extern inline uint64_t
shim_swap_64 (uint64_t val) {
#ifdef SHIM_OS_OSX
	return  ((val >> (7*8)) | \
		 ((val >> (5*8)) & UINT64_C (0x000000000000ff00)) | \
		 ((val >> (3*8)) & UINT64_C (0x0000000000ff0000)) | \
		 ((val >> 8    ) & UINT64_C (0x00000000ff000000)) | \
		 ((val << 8    ) & UINT64_C (0x000000ff00000000)) | \
		 ((val << (3*8)) & UINT64_C (0x0000ff0000000000)) | \
		 ((val << (5*8)) & UINT64_C (0x00ff000000000000)) | \
		 (val << (7*8)));
#else
	return SWAP_F_ (SIZE_ (64,uint64),val);
#endif
}

SHIM_END_DECLS

#undef SIZE_
#undef SWAP_F_IMPL_
#undef SWAP_F_

#endif // ~ SHIM_OPERATIONS_H
