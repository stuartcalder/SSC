#ifndef BASE_RANDOM_H
#define BASE_RANDOM_H

#include "errors.h"
#include "macros.h"

#if    defined(BASE_OS_MAC) || defined(__NetBSD__) || defined(__Dragonfly__)
#	include "files.h"
#	ifdef BASE_OS_MAC
#		define BASE_RANDOM_DEV "/dev/random"
#	else
#		define BASE_RANDOM_DEV "/dev/urandom"
#	endif
#	define BASE_GET_OS_ENTROPY_IMPL(ptr, size) { \
		Base_File_t dev = Base_open_filepath_or_die(BASE_RANDOM_DEV, true); \
		if (read(dev, ptr, size) != (ssize_t)size) \
			Base_errx("Error: Failed to read from " BASE_RANDOM_DEV "\n"); \
		Base_close_file_or_die(dev); \
	}
#elif  defined(__gnu_linux__) || defined(__FreeBSD__)
#	include <sys/random.h>
#	define BASE_RANDOM_MAX 256
#	define BASE_GET_OS_ENTROPY_IMPL(ptr, size) { \
		while (size > BASE_RANDOM_MAX) { \
			if (getrandom(ptr, BASE_RANDOM_MAX, 0) != (ssize_t)BASE_RANDOM_MAX) \
				Base_errx("Error: getrandom failed.\n"); \
			size -= BASE_RANDOM_MAX; \
			ptr  += BASE_RANDOM_MAX; \
		} \
		if (getrandom(ptr, size, 0) != (ssize_t)size) \
			Base_errx("Error: getrandom failed.\n"); \
	}
#elif  defined(__OpenBSD__)
#	include <unistd.h>
#	define BASE_RANDOM_MAX 256
#	define BASE_GET_OS_ENTROPY_IMPL(ptr, size) { \
		while (size > BASE_RANDOM_MAX) { \
			if (getentropy(ptr, BASE_RANDOM_MAX)) \
				Base_errx("Error: getentropy failed.\n"); \
			size -= BASE_RANDOM_MAX; \
			ptr  += BASE_RANDOM_MAX; \
		} \
		if (getentropy(ptr, size)) \
			Base_errx("Error: getentropy failed.\n"); \
	}
#elif  defined(BASE_OS_WINDOWS)
#	include <windows.h>
#	include <ntstatus.h>
#	include <bcrypt.h>
#	define BASE_GET_OS_ENTROPY_IMPL(ptr, size) { \
		BCRYPT_ALG_HANDLE h; \
		Base_assert_msg(BCryptOpenAlgorithmProvider(&h, L"RNG", NULL, 0) == STATUS_SUCCESS, \
		                "Error: BCryptOpenAlgorithmProvider failed.\n"); \
		Base_assert_msg(BCryptGenRandom(h, ptr, size, 0) == STATUS_SUCCESS, \
		                "Error: BCryptGenRandom failed.\n"); \
		Base_assert_msg(BCryptCloseAlgorithmProvider(h, 0) == STATUS_SUCCESS, \
		                "Error: BCryptCloseAlgorithmProvider failed.\n"); \
	}
#else
#	error "Unsupported OS."
#endif

BASE_BEGIN_DECLS
BASE_API    void Base_get_os_entropy (uint8_t* BASE_RESTRICT, size_t);
BASE_END_DECLS

#endif
