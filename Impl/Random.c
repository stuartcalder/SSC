/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "Random.h"

#if defined(SSC_OS_MAC) || defined(__NetBSD__) || defined(__Dragonfly__)
 #include "File.h"
 #ifdef SSC_OS_MAC
  #define SSC_RANDOM_DEV_ "/dev/random"
 #else
  #define SSC_RANDOM_DEV_ "/dev/urandom"
 #endif
 #define SSC_GETENTROPY_IMPL_(Ptr, Size) {\
  SSC_File_t dev = SSC_FilePath_openOrDie(SSC_RANDOM_DEV_, true);\
  SSC_assertMsg((read(dev, Ptr, Size) == (ssize_t)Size), SSC_ERR_S_FAILED("SSC_get_os_entropy: read"));\
  SSC_File_closeOrDie(dev);\
 }
#elif defined(__gnu_linux__) || defined(__FreeBSD__)
 #include <sys/random.h>
 #define SSC_RANDOM_MAX_ 256
 #define SSC_GETENTROPY_IMPL_(Ptr, Size) {\
  uint8_t* p = (uint8_t*)Ptr;\
  while (Size > SSC_RANDOM_MAX_) {\
    SSC_assertMsg(getrandom(p, SSC_RANDOM_MAX_, 0) == SSC_RANDOM_MAX_,\
     SSC_ERR_S_FAILED_IN("getrandom()"));\
    Size -= SSC_RANDOM_MAX_;\
    p    += SSC_RANDOM_MAX_;\
  }\
  SSC_assertMsg(getrandom(p, Size, 0) == Size, SSC_ERR_S_FAILED_IN("getrandom()"));\
 }
#elif defined(__OpenBSD__)
 #include <unistd.h>
 #define SSC_RANDOM_MAX_ 256
 #define SSC_GETENTROPY_IMPL_(Ptr, Size) {\
  uint8_t* p = (uint8_t*)Ptr;\
  while (Size > SSC_RANDOM_MAX_) {\
    SSC_assertMsg(!getentropy(p, SSC_RANDOM_MAX_), SSC_ERR_S_FAILED_IN("getentropy()"));\
    Size -= SSC_RANDOM_MAX_;\
    p    += SSC_RANDOM_MAX_;\
  }\
  SSC_assertMsg(!getentropy(p, Size), SSC_ERR_S_FAILED_IN("getentropy()"));\
 }
#elif defined(SSC_OS_WINDOWS)
 #include <windows.h>
 #include <ntstatus.h>
 #include <bcrypt.h>
 #define SSC_GETENTROPY_IMPL_(Ptr, Size) {\
  BCRYPT_ALG_HANDLE h;\
  SSC_assertMsg(BCryptOpenAlgorithmProvider(&h, L"RNG", SSC_NULL, 0) == STATUS_SUCCESS,\
   SSC_ERR_S_FAILED("BCryptOpenAlgorithmProvider"));\
  SSC_assertMsg(BCryptGenRandom(h, Ptr, Size, 0) == STATUS_SUCCESS,\
   SSC_ERR_S_FAILED("BCryptGenRandom"));\
  SSC_assertMsg(BCryptCloseAlgorithmProvider(h, 0) == STATUS_SUCCESS,\
   SSC_ERR_S_FAILED("BCryptCloseAlgorithmProvider"));\
 }
#else
 #error "Unsupported OS."
#endif

void SSC_getEntropy(void* SSC_RESTRICT ptr, size_t size)
SSC_GETENTROPY_IMPL_(ptr, size)
