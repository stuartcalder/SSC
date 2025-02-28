/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "MemLock.h"
#include "Memory.h"
#include "Error.h"
#ifdef SSC_MEMLOCK_H /* When memorylocking is disabled, the entire header is discarded. */
#if defined(SSC_OS_UNIXLIKE)
 #include <sys/types.h>
 #include <sys/time.h>
 #include <sys/resource.h>
 #include <sys/mman.h>
 #ifndef RLIMIT_MEMLOCK
  #error "RLIMIT_MEMLOCK not defined!"
 #endif
#elif defined(SSC_OS_WINDOWS)
 #include "File.h"
 #include <windows.h>
 #include <memoryapi.h>
#else
 #error "Unsupported."
#endif

SSC_MemLock SSC_MemLock_Global;

#define R_ SSC_RESTRICT

/* Static prototypes. */

/* Calculate the number of bytes locked, when we lock
 * @n many virtual memory pages of size @page_size. */
static uint64_t num_locked_bytes_(uint64_t n, uint64_t page_size);

/* Implementations. */
SSC_Error_t
SSC_MemLock_init(SSC_MemLock* ml)
{
  ml->page_size = (uint64_t)SSC_getPageSize();
#if defined(SSC_OS_UNIXLIKE)
  {
    struct rlimit rl;
    if (getrlimit(RLIMIT_MEMLOCK, &rl))
      return SSC_MEMLOCK_ERR_GET_LIMIT;
    if (rl.rlim_max > rl.rlim_cur) {
      rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_MEMLOCK, &rl))
      return SSC_MEMLOCK_ERR_SET_LIMIT;
    }
    ml->limit = (uint64_t)rl.rlim_cur;
  }
#elif defined(SSC_OS_WINDOWS)
  {
    SSC_File_t proc = GetCurrentProcess();
    SIZE_T minimum, maximum;
    if (!GetProcessWorkingSetSize(proc, &minimum, &maximum))
      return SSC_MEMLOCK_ERR_GET_LIMIT;
    /* FIXME: The win32 documentation is vague about how much memory
     * precisely is allowed to be locked at once. */
    ml->limit = (uint64_t)minimum - ml->page_size;
  }
#else
 #error "Unsupported."
#endif
  ml->n = 0;
  #ifdef SSC_EXTERN_MEMLOCK_THREADSAFE
  if (pthread_mutex_init(&ml->n_mtx, SSC_NULL))
    return SSC_MEMLOCK_ERR_MTX_INIT;
  #endif
  return 0;
}

#undef  ERR_
#define ERR_ "Error: SSC_MemLock_initHandled: %s\n"

void
SSC_MemLock_initHandled(SSC_MemLock* ml)
{
  const int c = SSC_MemLock_init(ml);
  switch (c) {
    case 0:
      return;
    case SSC_MEMLOCK_ERR_GET_LIMIT:
      SSC_errx(ERR_, "Failed to get memory limit.");
      return; /* This return will never happen but supresses compiler warnings. */
    #if SSC_MEMLOCK_INIT_MAYRETURN_ERR_SET_LIMIT
    case SSC_MEMLOCK_ERR_SET_LIMIT:
      SSC_errx(ERR_, "Failed to increase memory limit.");
      return; /* This return will never happen but supresses compiler warnings. */
    #endif
    #if SSC_MEMLOCK_INIT_MAYRETURN_ERR_MTX_INIT
    case SSC_MEMLOCK_ERR_MTX_INIT:
      SSC_errx(ERR_, "Failed to initialize n mutex.");
      return; /* This return will never happen but supresses compiler warnings. */
    #endif
  }
  SSC_errx(ERR_, "Invalid SSC_MemLock_init return code.");
}

/* When enabled externally, these macros lock the
 * counter mutex of the SSC_MemLock pointer @Ml_Ptr.
 * Must be used in a procedure returning int for error codes.*/
#ifdef SSC_EXTERN_MLOCK_THREADSAFE
 #define LOCK_N_MTX_(Ml_Ptr) do {\
  if (pthread_mutex_lock(&Ml_Ptr->n_mtx))\
   return SSC_MEMLOCK_ERR_MTX_OP;\
 } while (0)
 #define UNLOCK_N_MTX_(Ml_Ptr) do {\
  if (pthread_mutex_unlock(&Ml_Ptr->n_mtx))\
   return SSC_MEMLOCK_ERR_MTX_OP;\
 } while (0)
#else
 #define LOCK_N_MTX_(Ml_Ptr)   /* Nil */
 #define UNLOCK_N_MTX_(Ml_Ptr) /* Nil */
#endif

uint64_t
num_locked_bytes_(uint64_t n, uint64_t page_size)
{
  uint64_t locked = n / page_size; /* Floor division. The number of whole pages. */
  if (n % page_size) /* If n is not evenly divisible into pages. */
    ++locked; /* We lock all pages along the covered boundaries. */
  SSC_ASSERT_MSG((locked * page_size) > locked, "Overflow!\n");
  return locked * page_size; /* Number pages x Size of each page in bytes. */
}

SSC_Error_t
SSC_MemLock_lockContext(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx)
{
  const uint64_t locked = num_locked_bytes_(n, ctx->page_size); /* How many bytes would be locked? */
  LOCK_N_MTX_(ctx); /* Lock mutex. */
  if ((locked + ctx->n) > ctx->limit) {
    /* Went over the limit. */
    UNLOCK_N_MTX_(ctx);
    return SSC_MEMLOCK_ERR_OVER_MEMLIMIT;
  }
  #if defined(SSC_OS_UNIXLIKE)
  if (mlock(p, n)) {
    UNLOCK_N_MTX_(ctx);
    return SSC_MEMLOCK_ERR_LOCK_OP  ;
  }
  #elif defined(SSC_OS_WINDOWS)
  if (!VirtualLock((LPVOID)p, (SIZE_T)n)) {
    UNLOCK_N_MTX_(ctx);
    return SSC_MEMLOCK_ERR_LOCK_OP;
  }
  #else
   #error "Unsupported."
  #endif
  ctx->n += locked;
  UNLOCK_N_MTX_(ctx); /* Unlock mutex. */
  return 0;
}

#undef  ERR_
#define ERR_ "Error: SSC_MemLock_lockContextHandled: %s\n"

void
SSC_MemLock_lockContextHandled(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx, SSC_MemLockFlag_t f)
{
  const int c = SSC_MemLock_lockContext(p, n, ctx);
  switch (c) {
    case 0:
      return;
    #ifdef SSC_EXTERN_MLOCK_THREADSAFE
    case SSC_MEMLOCK_ERR_MTX_OP:
      SSC_errx(ERR_, "Failed to lock or unlock mutex.");
      return;
    #endif
    case SSC_MEMLOCK_ERR_LOCK_OP:
      if (f & SSC_MEMLOCKFLAG_GRACEFUL_LOCK_FAIL)
        return;
      SSC_errx(ERR_, "Failed to memlock or memunlock.");
      return;
    case SSC_MEMLOCK_ERR_OVER_MEMLIMIT:
      if (f & SSC_MEMLOCKFLAG_GRACEFUL_OVERMEMLIMIT_FAIL)
        return;
      SSC_errx(ERR_, "Went over memory locking limit.");
      return;
  }
  SSC_errx(ERR_, "Invalid SSC_MemLock_lockContext return code.");
}

SSC_Error_t
SSC_MemLock_unlockContext(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx)
{
  const uint64_t locked = num_locked_bytes_(n, ctx->page_size);
  LOCK_N_MTX_(ctx);
  if (locked > ctx->n) {
    UNLOCK_N_MTX_(ctx);
    return SSC_MEMLOCK_ERR_UNDER_MEMMIN;
  }
  #if defined(SSC_OS_UNIXLIKE)
  if (munlock(p, n)) {
    UNLOCK_N_MTX_(ctx);
    return SSC_MEMLOCK_ERR_LOCK_OP;
  }
  #elif defined(SSC_OS_WINDOWS)
  if (!VirtualUnlock((LPVOID)p, (SIZE_T)n)) {
    UNLOCK_N_MTX_(ctx);
    return SSC_MEMLOCK_ERR_LOCK_OP;
  }
  #else
   #error "Unsupported."
  #endif
  ctx->n -= locked;
  UNLOCK_N_MTX_(ctx);
  return 0;
}

#undef  ERR_
#define ERR_ "Error: SSC_MemLock_lockContextHandled %s\n"

void
SSC_MemLock_unlockContextHandled(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx, SSC_MemLockFlag_t f)
{
  const int c = SSC_MemLock_unlockContext(p, n, ctx);
  switch (c) {
    case 0:
      return;
    #ifdef SSC_EXTERN_MLOCK_THREADSAFE
    case SSC_MEMLOCK_ERR_MTX_OP:
      SSC_errx(ERR_, "Failed to lock or unlock mutex.");
      return;
    #endif
    case SSC_MEMLOCK_ERR_LOCK_OP:
      if (f & SSC_MEMLOCKFLAG_GRACEFUL_UNLOCK_FAIL)
        return;
      SSC_errx(ERR_, "Failed to memlock or memunlock.");
      return;
    case SSC_MEMLOCK_ERR_UNDER_MEMMIN:
      SSC_errx(ERR_, "Went under minimum memory locking limit.");
      return;
  }
  SSC_errx(ERR_, "Invalid SSC_MemLock_unlockContext return code.");
}

#endif /* ~ ifdef SSC_MEMLOCK_H */
