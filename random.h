/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_RANDOM_H
#define BASE_RANDOM_H

#include "errors.h"
#include "macros.h"

#if defined(BASE_OS_MAC) || defined(__NetBSD__) || defined(__Dragonfly__)
 #include "files.h"
 #ifdef BASE_OS_MAC
  #define BASE_RANDOM_DEV "/dev/random"
 #else
  #define BASE_RANDOM_DEV "/dev/urandom"
 #endif
 #define BASE_GET_OS_ENTROPY_IMPL(Ptr, Size) {\
  Base_File_t dev = Base_open_filepath_or_die(BASE_RANDOM_DEV, true);\
  Base_assert_msg((read(dev, Ptr, Size) == (ssize_t)Size), BASE_ERR_S_FAILED("Base_get_os_entropy: read"));\
  Base_close_file_or_die(dev);\
 }
#elif defined(__gnu_linux__) || defined(__FreeBSD__)
 #include <sys/random.h>
 #define BASE_RANDOM_MAX 256
 #define BASE_GET_OS_ENTROPY_IMPL(Ptr, Size) { \
 uint8_t* p = (uint8_t*)Ptr; \
 while (Size > BASE_RANDOM_MAX) { \
   if (getrandom(p, BASE_RANDOM_MAX, 0) != (ssize_t)BASE_RANDOM_MAX) \
     Base_errx(BASE_ERR_S_FAILED("getrandom")); \
   Size -= BASE_RANDOM_MAX; \
   p    += BASE_RANDOM_MAX; \
 } \
 if (getrandom(p, Size, 0) != (ssize_t)Size) \
   Base_errx(BASE_ERR_S_FAILED("getrandom")); \
}
#elif defined(__OpenBSD__)
# include <unistd.h>
# define BASE_RANDOM_MAX 256
# define BASE_GET_OS_ENTROPY_IMPL(Ptr, Size) { \
 uint8_t* p = (uint8_t*)Ptr; \
 while (Size > BASE_RANDOM_MAX) { \
   if (getentropy(p, BASE_RANDOM_MAX)) \
     Base_errx(BASE_ERR_S_FAILED("getentropy")); \
   Size -= BASE_RANDOM_MAX; \
   p    += BASE_RANDOM_MAX; \
 } \
 if (getentropy(p, size)) \
   Base_errx(BASE_ERR_S_FAILED("getentropy")); \
}
#elif defined(BASE_OS_WINDOWS)
# include <windows.h>
# include <ntstatus.h>
# include <bcrypt.h>
# define BASE_GET_OS_ENTROPY_IMPL(Ptr, Size) { \
  BCRYPT_ALG_HANDLE h; \
  Base_assert_msg(BCryptOpenAlgorithmProvider(&h, L"RNG", BASE_NULL, 0) == STATUS_SUCCESS, \
   BASE_ERR_S_FAILED("BCryptOpenAlgorithmProvider")); \
  Base_assert_msg(BCryptGenRandom(h, Ptr, Size, 0) == STATUS_SUCCESS, \
   BASE_ERR_S_FAILED("BCryptGenRandom")); \
  Base_assert_msg(BCryptCloseAlgorithmProvider(h, 0) == STATUS_SUCCESS, \
   BASE_ERR_S_FAILED("BCryptCloseAlgorithmProvider")); \
}
#else
#	error "Unsupported OS."
#endif

BASE_BEGIN_C_DECLS
BASE_API void Base_get_os_entropy(void* BASE_RESTRICT, size_t);
BASE_END_C_DECLS

#endif
