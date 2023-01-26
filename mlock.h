/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */

/* The environment must externally define BASE_EXTERN_MLOCK to enable
 * this header. */
#if !defined(BASE_MLOCK_H) && defined(BASE_EXTERN_MLOCK)
#define BASE_MLOCK_H
#define BASE_HAS_MEMORYLOCKING /* TODO: Remove references to this macro, as it is now only here for compatability. */

#ifdef BASE_EXTERN_MLOCK_THREADSAFE
 #include <pthread.h>
#endif
#include "macros.h"
#include "errors.h"

#define R_(p) p BASE_RESTRICT
BASE_BEGIN_C_DECLS

typedef struct {
  uint64_t page_size; /* The size of virtual memory pages, in bytes. */
  uint64_t limit;     /* The memory locking limit, in bytes. */
  uint64_t n;         /* The number of locked bytes. */
  #ifdef BASE_EXTERN_MLOCK_THREADSAFE
  pthread_mutex_t n_mtx; /* Mutex limiting access to @n. */
  #endif
} Base_MLock;
#define BASE_MLOCK_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_MLock, 0)

BASE_API extern Base_MLock Base_MLock_g; /* Global Base_MLock structure. */

#define BASE_MLOCK_ERR_LOCK_OP       (-1) /* Failure in a lock operation. */
#define BASE_MLOCK_ERR_MTX_OP        (-2) /* Failure in a mutex operation. */
#define BASE_MLOCK_ERR_MTX_INIT      (-3) /* Failure in initializing a mutex. */
#define BASE_MLOCK_ERR_OVER_MEMLIMIT (-4) /* Going over the allowed memory limit. */
#define BASE_MLOCK_ERR_UNDER_MEMMIN  (-5) /* Under the memory minimum. */
#define BASE_MLOCK_ERR_GET_LIMIT     (-6) /* Failure in getting the memory limit. */
#define BASE_MLOCK_ERR_SET_LIMIT     (-7) /* Failure in setting the memory limit. */
#define BASE_MLOCK_ERR_UNINITIALIZED (-8)

#define BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL	(0x01)
#define BASE_MLOCK_F_GRACEFUL_LOCK_FAIL		(0x02)
#define BASE_MLOCK_F_GRACEFUL_UNLOCK_FAIL	(0x02)
typedef int Base_MLock_F_t;

#ifdef BASE_OS_UNIXLIKE
 #define BASE_MLOCK_INIT_MAYRETURN_ERR_SET_LIMIT 1
#else
 #define BASE_MLOCK_INIT_MAYRETURN_ERR_SET_LIMIT 0
#endif
#ifdef BASE_EXTERN_MLOCK_THREADSAFE
 #define BASE_MLOCK_INIT_MAYRETURN_ERR_MTX_INIT 1
#else
 #define BASE_MLOCK_INIT_MAYRETURN_ERR_MTX_INIT 0
#endif

BASE_API Base_Error_t Base_MLock_init(
/* Initialize a Base_MLock struct at @ml.
 * This must be called on a Base_MLock before using it in
 * the other procedures. */
 Base_MLock* ml);

BASE_INLINE Base_Error_t Base_MLock_g_init(void)
/* Initialize, if not already initialized. */
{
  if (!Base_MLock_g.page_size)
    return Base_MLock_init(&Base_MLock_g);
  return 0;
}

BASE_API void Base_MLock_init_handled(
/* Initialize @ctx. Die on failure. */
 Base_MLock* ctx);

BASE_INLINE void Base_MLock_g_init_handled(void)
/* Initialize the global variable Base_MLock_g, the default
 * memory locking structure when none is specified.
 * Die on failure. */
{
  if (!Base_MLock_g.page_size)
    Base_MLock_init_handled(&Base_MLock_g);
}

BASE_API Base_Error_t Base_mlock_ctx
/* Attempt to lock the memory @p with requested number of bytes @n, with the
 * MemoryLock struct pointer @ctx. */
(R_(void*) p, uint64_t n, R_(Base_MLock*) ctx);

BASE_INLINE Base_Error_t Base_mlock(
/* Lock the memory @p with number bytes @n using the default global context. */
 R_(void*) p, uint64_t n)
{
  return Base_mlock_ctx(p, n, &Base_MLock_g);
}

BASE_API void Base_mlock_ctx_handled(
/* Lock the memory @p with number bytes @n using context
 * @ctx with the handle flags @f. */
 R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, Base_MLock_F_t f);

BASE_INLINE void Base_mlock_ctx_or_die(
/* Lock the memory @p with number bytes @n using context @ctx. */
 R_(void*) p, uint64_t n, R_(Base_MLock*) ctx)
{
  Base_mlock_ctx_handled(p, n, ctx, 0);
}

BASE_INLINE void Base_mlock_handled(
/* Lock the memory @p with number bytes @n using the default global context, with a handle flag of @f. */
 R_(void*) p, uint64_t n, Base_MLock_F_t f)
{
  Base_mlock_ctx_handled(p, n, &Base_MLock_g, f);
}

BASE_INLINE void Base_mlock_or_die(
/* Lock the memory @p with number bytes @n using the default global context. */
 R_(void*) p, uint64_t n)
{
  Base_mlock_handled(p, n, 0);
}

BASE_API Base_Error_t Base_munlock_ctx(
/* Unlock the memory @p with number bytes @n using the context @ctx. */
 R_(void*) p, uint64_t n, R_(Base_MLock*) ctx);

BASE_INLINE Base_Error_t Base_munlock(
/* Unlock the memory @p with number bytes @n using the default global context. */
 R_(void*) p, uint64_t n)
{
  return Base_munlock_ctx(p, n, &Base_MLock_g);
}

BASE_API void Base_munlock_ctx_handled(
/* Unlock the memory @p with number bytes @n using the context @ctx, with a handle flag of @f. */
 R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, Base_MLock_F_t f);

BASE_INLINE void Base_munlock_ctx_or_die(
/* Unlock the memory @p with number bytes @n using the context @ctx, with a handle flag of 0. */
 R_(void*) p, uint64_t n, R_(Base_MLock*) ctx)
{
  Base_munlock_ctx_handled(p, n, ctx, 0);
}

BASE_INLINE void Base_munlock_handled(
/* Unlock the memory @p with number bytes @n using the default global context, with a handle flag of @f. */
 R_(void*) p, uint64_t n, Base_MLock_F_t f)
{
  Base_munlock_ctx_handled(p, n, &Base_MLock_g, f);
}

BASE_INLINE void Base_munlock_or_die(
/* Unlock the memory @p with number bytes @n using the
 * default global context, with a handle flag of 0. */
 R_(void*) p, uint64_t n)
{
  Base_munlock_handled(p, n, 0);
}

BASE_END_C_DECLS
#undef R_

#endif /* ~ if !defined(BASE_MLOCK_H) && defined(BASE_EXTERN_MLOCK) */
