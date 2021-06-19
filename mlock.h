#ifndef BASE_MLOCK_H
#define BASE_MLOCK_H

#include "macros.h"

/* OpenBSD encrypts swap by default. */
#if !defined(__OpenBSD__) && !defined(BASE_EXTERN_DISABLE_MEMORYLOCKING)
#  define BASE_HAS_MEMORYLOCKING
#endif

#ifdef BASE_HAS_MEMORYLOCKING
#  ifdef BASE_EXTERN_MLOCK_THREADSAFE
#    include <pthread.h> /*TODO: Needs testing. */
#  endif
typedef struct {
	uint64_t        page_size;
	uint64_t        limit;
	uint64_t        n;
#  ifdef BASE_EXTERN_MLOCK_THREADSAFE
	pthread_mutex_t n_mtx;
#  endif
} Base_MLock;

BASE_API extern Base_MLock Base_MLock_g;

enum {
	BASE_MLOCK_ERR_LOCK_OP       = -1,
	BASE_MLOCK_ERR_MTX_OP        = -2,
	BASE_MLOCK_ERR_MTX_INIT      = -3,
	BASE_MLOCK_ERR_OVER_MEMLIMIT = -4,
	BASE_MLOCK_ERR_UNDER_MEMMIN  = -5,
	BASE_MLOCK_ERR_GET_LIMIT     = -6,
	BASE_MLOCK_ERR_SET_LIMIT     = -7,
	BASE_MLOCK_ERR_UNINITIALIZED = -8
};

#  define BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL	(0x01u)
#  define BASE_MLOCK_F_GRACEFUL_LOCK_FAIL			(0x02u)
#  define BASE_MLOCK_F_GRACEFUL_UNLOCK_FAIL			(0x02u)

#  define R_(p) p BASE_RESTRICT
BASE_BEGIN_DECLS
/* Initialize a Base_MLock struct. */
BASE_API    int  Base_MLock_init (Base_MLock*);
BASE_INLINE int  Base_MLock_g_init (void) { return Base_MLock_init(&Base_MLock_g); }
BASE_API    void Base_MLock_init_handled (Base_MLock*);
BASE_INLINE void Base_MLock_g_init_handled (void) { Base_MLock_init_handled(&Base_MLock_g); }
/* Attempt to lock the memory `p` with requested number of bytes `n`, with the
 * MemoryLock struct pointer `ctx`. */
BASE_API    int  Base_mlock_ctx (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx);
/* Lock the memory `p` with number bytes `n` using the default global context. */
BASE_INLINE int  Base_mlock (R_(void*) p, uint64_t n) { return Base_mlock_ctx(p, n, &Base_MLock_g); }
/* Lock the memory `p` with number bytes `n` using context `ctx` with the handle flags `f`. */
#  define BASE_MLOCK_CTX_HANDLED_F_DEFAULT BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL
BASE_API    void Base_mlock_ctx_handled (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, unsigned f);
/* Lock the memory `p` with number bytes `n` using context `ctx` with a handle flag of `BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL`. */
BASE_INLINE void Base_mlock_ctx_or_die (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx) { Base_mlock_ctx_handled(p, n, ctx, BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL); }
/* Lock the memory `p` with number bytes `n` using the default global context, with a handle flag of `f`. */
BASE_INLINE void Base_mlock_handled (R_(void*) p, uint64_t n, unsigned f) { Base_mlock_ctx_handled(p, n, &Base_MLock_g, f); }
/* Lock the memory `p` with number bytes `n` using the default global context, with a handle flag of `BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL`. */
BASE_INLINE void Base_mlock_or_die (R_(void*) p, uint64_t n) { Base_mlock_handled(p, n, BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL); }
/* Unlock the memory `p` with number bytes `n` using the context `ctx`. */
BASE_API    int  Base_munlock_ctx (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx);
/* Unlock the memory `p` with number bytes `n` using the default global context. */
BASE_INLINE int  Base_munlock (R_(void*) p, uint64_t n) { return Base_munlock_ctx(p, n, &Base_MLock_g); }
/* Unlock the memory `p` with number bytes `n` using the context `ctx`, with a handle flag of `f`. */
BASE_API    void Base_munlock_ctx_handled (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, unsigned f);
/* Unlock the memory `p` with number bytes `n` using the context `ctx`, with a handle flag of 0. */
BASE_INLINE void Base_munlock_ctx_or_die (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx) { Base_munlock_ctx_handled(p, n, ctx, 0u); }
/* Unlock the memory `p` with number bytes `n` using the default global context, with a handle flag of `f`. */
BASE_INLINE void Base_munlock_handled (R_(void*) p, uint64_t n, unsigned f) { Base_munlock_ctx_handled(p, n, &Base_MLock_g, f); }
/* Unlock the memory `p` with number bytes `n` using the default global context, with a handle flag of 0. */
BASE_INLINE void Base_munlock_or_die (R_(void*) p, uint64_t n) { Base_munlock_handled(p, n, 0u); }
BASE_END_DECLS
#  undef R_

#endif /* ~ ifdef BASE_HAS_MEMORYLOCKING */
#endif /* ~ ifndef BASE_MLOCK_H */
