#if !defined(BASE_MLOCK_H) && defined(BASE_EXTERN_MLOCK)
#define BASE_MLOCK_H
#define BASE_HAS_MEMORYLOCKING /* TODO: Remove references to this macro, as it is now only here for compatability. */

#ifdef BASE_EXTERN_MLOCK_THREADSAFE
#  include <pthread.h>
#endif
#include "macros.h"

typedef struct {
	uint64_t page_size;
	uint64_t limit;
	uint64_t n;
#ifdef BASE_EXTERN_MLOCK_THREADSAFE
	pthread_mutex_t n_mtx;
#endif
} Base_MLock;
#define BASE_MLOCK_NULL_LITERAL (Base_MLock){0}

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

#define BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL	(0x01u)
#define BASE_MLOCK_F_GRACEFUL_LOCK_FAIL		(0x02u)
#define BASE_MLOCK_F_GRACEFUL_UNLOCK_FAIL	(0x02u)

#define R_(p) p BASE_RESTRICT
BASE_BEGIN_C_DECLS
#ifdef BASE_OS_UNIXLIKE
#  define BASE_MLOCK_INIT_MAYRETURN_ERR_SET_LIMIT
#endif
/* Initialize a Base_MLock struct. */
BASE_API int Base_MLock_init (Base_MLock*);

BASE_INLINE int Base_MLock_g_init (void) {
	if (!Base_MLock_g.page_size)
		return Base_MLock_init(&Base_MLock_g);
	return 0;
}

BASE_API void Base_MLock_init_handled (Base_MLock*);

BASE_INLINE void Base_MLock_g_init_handled (void) {
	if (!Base_MLock_g.page_size)
		Base_MLock_init_handled(&Base_MLock_g);
}

/* Attempt to lock the memory `p` with requested number of bytes `n`, with the
 * MemoryLock struct pointer `ctx`. */
BASE_API int Base_mlock_ctx (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx);

/* Lock the memory `p` with number bytes `n` using the default global context. */
BASE_INLINE int Base_mlock (R_(void*) p, uint64_t n) {
	return Base_mlock_ctx(p, n, &Base_MLock_g);
}

/* Lock the memory `p` with number bytes `n` using context `ctx` with the handle flags `f`. */
BASE_API void Base_mlock_ctx_handled (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, unsigned f);

/* Lock the memory `p` with number bytes `n` using context `ctx` with a handle flag of `BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL`. */
BASE_INLINE void Base_mlock_ctx_or_die (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx) {
	Base_mlock_ctx_handled(p, n, ctx, 0u);
}

/* Lock the memory `p` with number bytes `n` using the default global context, with a handle flag of `f`. */
BASE_INLINE void Base_mlock_handled (R_(void*) p, uint64_t n, unsigned f) {
	Base_mlock_ctx_handled(p, n, &Base_MLock_g, f);
}

/* Lock the memory `p` with number bytes `n` using the default global context, with a handle flag of `BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL`. */
BASE_INLINE void Base_mlock_or_die (R_(void*) p, uint64_t n) {
	Base_mlock_handled(p, n, 0u);
}

/* Unlock the memory `p` with number bytes `n` using the context `ctx`. */
BASE_API int Base_munlock_ctx (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx);

/* Unlock the memory `p` with number bytes `n` using the default global context. */
BASE_INLINE int Base_munlock (R_(void*) p, uint64_t n) {
	return Base_munlock_ctx(p, n, &Base_MLock_g);
}

/* Unlock the memory `p` with number bytes `n` using the context `ctx`, with a handle flag of `f`. */
BASE_API void Base_munlock_ctx_handled (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, unsigned f);

/* Unlock the memory `p` with number bytes `n` using the context `ctx`, with a handle flag of 0. */
BASE_INLINE void Base_munlock_ctx_or_die (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx) {
	Base_munlock_ctx_handled(p, n, ctx, 0u);
}

/* Unlock the memory `p` with number bytes `n` using the default global context, with a handle flag of `f`. */
BASE_INLINE void Base_munlock_handled (R_(void*) p, uint64_t n, unsigned f) {
	Base_munlock_ctx_handled(p, n, &Base_MLock_g, f);
}

/* Unlock the memory `p` with number bytes `n` using the default global context, with a handle flag of 0. */
BASE_INLINE void Base_munlock_or_die (R_(void*) p, uint64_t n) {
	Base_munlock_handled(p, n, 0u);
}

BASE_END_C_DECLS
#undef R_

#endif /* ~ if !defined(BASE_MLOCK_H) && defined(BASE_EXTERN_MLOCK) */
