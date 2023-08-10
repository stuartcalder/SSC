/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */

/* The environment must externally define SSC_EXTERN_MEMLOCK to enable
 * this header. */
#if !defined(SSC_MEMLOCK_H) && defined(SSC_EXTERN_MEMLOCK)
#define SSC_MEMLOCK_H

#ifdef SSC_EXTERN_MEMLOCK_THREADSAFE
 #include <pthread.h>
#endif
#include "Macro.h"
#include "Error.h"

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

typedef struct {
  uint64_t page_size; /* The size of virtual memory pages, in bytes. */
  uint64_t limit;     /* The memory locking limit, in bytes. */
  uint64_t n;         /* The number of locked bytes. */
  #ifdef SSC_EXTERN_MEMLOCK_THREADSAFE
  pthread_mutex_t n_mtx; /* Mutex limiting access to @n. */
  #endif
} SSC_MemLock;
#define SSC_MEMLOCK_NULL_LITERAL SSC_COMPOUND_LITERAL(SSC_MemLock, 0)

SSC_API extern SSC_MemLock SSC_MemLock_Global; /* Global SSC_MemLock structure. */

#define SSC_MEMLOCK_ERR_LOCK_OP       (-1) /* Failure in a lock operation. */
#define SSC_MEMLOCK_ERR_MTX_OP        (-2) /* Failure in a mutex operation. */
#define SSC_MEMLOCK_ERR_MTX_INIT      (-3) /* Failure in initializing a mutex. */
#define SSC_MEMLOCK_ERR_OVER_MEMLIMIT (-4) /* Going over the allowed memory limit. */
#define SSC_MEMLOCK_ERR_UNDER_MEMMIN  (-5) /* Under the memory minimum. */
#define SSC_MEMLOCK_ERR_GET_LIMIT     (-6) /* Failure in getting the memory limit. */
#define SSC_MEMLOCK_ERR_SET_LIMIT     (-7) /* Failure in setting the memory limit. */
#define SSC_MEMLOCK_ERR_UNINITIALIZED (-8)

#define SSC_MEMLOCKFLAG_GRACEFUL_OVERMEMLIMIT_FAIL (0x01)
#define SSC_MEMLOCKFLAG_GRACEFUL_LOCK_FAIL         (0x02)
#define SSC_MEMLOCKFLAG_GRACEFUL_UNLOCK_FAIL       (0x02)
typedef int SSC_MemLockFlag_t;

#ifdef SSC_OS_UNIXLIKE
 #define SSC_MEMLOCK_INIT_MAYRETURN_ERR_SET_LIMIT 1
#else
 #define SSC_MEMLOCK_INIT_MAYRETURN_ERR_SET_LIMIT 0
#endif
#ifdef SSC_EXTERN_MEMLOCK_THREADSAFE
 #define SSC_MEMLOCK_INIT_MAYRETURN_ERR_MTX_INIT 1
#else
 #define SSC_MEMLOCK_INIT_MAYRETURN_ERR_MTX_INIT 0
#endif

/* Initialize a SSC_MemLock struct at @ml.
 * This must be called on a SSC_MemLock before using it in
 * the other procedures. */
SSC_API SSC_Error_t
SSC_MemLock_init(SSC_MemLock* ml);

/* Initialize, if not already initialized. */
SSC_INLINE SSC_Error_t
SSC_MemLock_Global_init(void)
{
  if (!SSC_MemLock_Global.page_size)
    return SSC_MemLock_init(&SSC_MemLock_Global);
  return 0;
}

/* Initialize @ctx. Die on failure. */
SSC_API void SSC_MemLock_initHandled(SSC_MemLock* ctx);

/* Initialize the global variable SSC_MemLock_Global, the default
 * memory locking structure when none is specified.
 * Die on failure. */
SSC_INLINE void SSC_MemLock_Global_initHandled(void)
{
  if (!SSC_MemLock_Global.page_size)
    SSC_MemLock_initHandled(&SSC_MemLock_Global);
}

/* Attempt to lock the memory @p with requested number of bytes @n, with the
 * MemoryLock struct pointer @ctx. */
SSC_API SSC_Error_t
SSC_MemLock_lockContext(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx);

/* Lock the memory @p with number bytes @n using the default global context. */
SSC_INLINE SSC_Error_t
SSC_MemLock_lock(void* R_ p, uint64_t n)
{
  return SSC_MemLock_lockContext(p, n, &SSC_MemLock_Global);
}

/* Lock the memory @p with number bytes @n using context
 * @ctx with the handle flags @f. */
SSC_API void
SSC_MemLock_lockContextHandled(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx, SSC_MemLockFlag_t f);

/* Lock the memory @p with number bytes @n using context @ctx. */
SSC_INLINE void
SSC_MemLock_lockContextOrDie(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx)
{
  SSC_MemLock_lockContextHandled(p, n, ctx, 0);
}

/* Lock the memory @p with number bytes @n using the default global context, with a handle flag of @f. */
SSC_INLINE void
SSC_MemLock_lockHandled(void* R_ p, uint64_t n, SSC_MemLockFlag_t f)
{
  SSC_MemLock_lockContextHandled(p, n, &SSC_MemLock_Global, f);
}

/* Lock the memory @p with number bytes @n using the default global context. */
SSC_INLINE void
SSC_MemLock_lockOrDie(void* R_ p, uint64_t n)
{
  SSC_MemLock_lockHandled(p, n, 0);
}

/* Unlock the memory @p with number bytes @n using the context @ctx. */
SSC_API SSC_Error_t
SSC_MemLock_unlockContext(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx);

/* Unlock the memory @p with number bytes @n using the default global context. */
SSC_INLINE SSC_Error_t SSC_MemLock_unlock(void* R_ p, uint64_t n)
{
  return SSC_MemLock_unlockContext(p, n, &SSC_MemLock_Global);
}

/* Unlock the memory @p with number bytes @n using the context @ctx, with a handle flag of @f. */
SSC_API void
SSC_MemLock_unlockContextHandled(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx, SSC_MemLockFlag_t f);

/* Unlock the memory @p with number bytes @n using the context @ctx, with a handle flag of 0. */
SSC_INLINE void
SSC_MemLock_unlockContextOrDie(void* R_ p, uint64_t n, SSC_MemLock* R_ ctx)
{
  SSC_MemLock_unlockContextHandled(p, n, ctx, 0);
}

/* Unlock the memory @p with number bytes @n using the default global context, with a handle flag of @f. */
SSC_INLINE void
SSC_MemLock_unlockHandled(void* R_ p, uint64_t n, SSC_MemLockFlag_t f)
{
  SSC_MemLock_unlockContextHandled(p, n, &SSC_MemLock_Global, f);
}

/* Unlock the memory @p with number bytes @n using the
 * default global context, with a handle flag of 0. */
SSC_INLINE void
SSC_MemLock_unlockOrDie(void* R_ p, uint64_t n)
{
  SSC_MemLock_unlockHandled(p, n, 0);
}

SSC_END_C_DECLS
#undef R_

#endif /* ~ if !defined(SSC_MEMLOCK_H) && defined(SSC_EXTERN_MEMLOCK) */
