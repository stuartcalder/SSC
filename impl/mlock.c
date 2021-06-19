#include "mlock.h"
#include "mem.h"
#include "error.h"
#ifdef BASE_HAS_MEMORYLOCKING
#if    defined(BASE_OS_UNIXLIKE)
#  include <sys/types.h>
#  include <sys/time.h>
#  include <sys/resource.h>
#  include <sys/mman.h>
#  ifndef RLIMIT_MEMLOCK
#    error "RLIMIT_MEMLOCK not defined!"
#  endif
#elif  defined(BASE_OS_WINDOWS)
#  include "files.h"
#  include <windows.h>
#  include <memoryapi.h>
#else
#  error "Unsupported."
#endif

Base_MLock Base_MLock_g;

#define R_(p) p BASE_RESTRICT

static uint64_t num_locked_bytes_ (uint64_t, uint64_t);

int Base_MLock_init (Base_MLock* ml) {
	ml->page_size = (uint64_t)Base_get_pagesize();
#if    defined(BASE_OS_UNIXLIKE)
	{
		struct rlimit rl;
		if (getrlimit(RLIMIT_MEMLOCK, &rl))
			return BASE_MLOCK_ERR_GET_LIMIT;
		if (rl.rlim_max > rl.rlim_cur) {
			rl.rlim_cur = rl.rlim_max;
			if (setrlimit(RLIMIT_MEMLOCK, &rl))
				return BASE_MLOCK_ERR_SET_LIMIT;
		}
		ml->limit = (uint64_t)rl.rlim_cur;
	}
#elif  defined(BASE_OS_WINDOWS)
	{
		Base_File_t proc = GetCurrentProcess();
		SIZE_T minimum, maximum;
		if (!GetProcessWorkingSetSize(proc, &minimum, &maximum))
			return BASE_MLOCK_ERR_GET_LIMIT;
		ml->limit = (uint64_t)minimum - ml->page_size;
	}
#else
#  error "Unsupported."
#endif
	ml->n = 0;
#ifdef BASE_EXTERN_MLOCK_THREADSAFE
	if (pthread_mutex_init(&ml->n_mtx, NULL))
		return BASE_MLOCK_ERR_MTX_INIT;
#endif
	return 0;
}

#undef  ERR_
#define ERR_ "Error: Base_MLock_init_handled: %s\n"

void Base_MLock_init_handled (Base_MLock* ml) {
	const int c = Base_MLock_init(ml);
	switch (c) {
	case 0:
		return;
	case BASE_MLOCK_ERR_GET_LIMIT:
		Base_errx(ERR_, "Failed to get memory limit.");
		return;
#ifdef BASE_OS_UNIXLIKE
	case BASE_MLOCK_ERR_SET_LIMIT:
		Base_errx(ERR_, "Failed to increase memory limit.");
		return;
#endif
#ifdef BASE_EXTERN_MLOCK_THREADSAFE
	case BASE_MLOCK_ERR_MTX_INIT:
		Base_errx(ERR_, "Failed to initialize n mutex.");
		return;
#endif
	}
	Base_errx(ERR_, "Invalid Base_MLock_init return code.");
}

#ifdef BASE_EXTERN_MLOCK_THREADSAFE
#  define LOCK_N_MTX_(ml_p) do { \
	if (pthread_mutex_lock(&ml_p->n_mtx)) \
		return BASE_MLOCK_ERR_MTX_OP; \
	} while (0)
#  define UNLOCK_N_MTX_(ml_p) do { \
	if (pthread_mutex_unlock(&ml_p->n_mtx)) \
		return BASE_MLOCK_ERR_MTX_OP; \
	} while (0)
#else
#  define LOCK_N_MTX_(ml_p)		/* Nil */
#  define UNLOCK_N_MTX_(ml_p)	/* Nil */
#endif

uint64_t num_locked_bytes_ (uint64_t n, uint64_t page_size) {
	uint64_t locked = n / page_size;
	if (n % page_size)
		++locked;
	return locked * page_size;
}

int Base_mlock_ctx (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx) {
	const uint64_t locked = num_locked_bytes_(n, ctx->page_size);
	LOCK_N_MTX_(ctx);
	if ((locked + ctx->n) > ctx->limit) {
		UNLOCK_N_MTX_(ctx);
		return BASE_MLOCK_ERR_OVER_MEMLIMIT;
	}
#if   defined(BASE_OS_UNIXLIKE)
	if (mlock(p, n)) {
		UNLOCK_N_MTX_(ctx);
		return BASE_MLOCK_ERR_LOCK_OP;
	}
#elif defined(BASE_OS_WINDOWS)
	if (!VirtualLock((LPVOID)p, (SIZE_T)n)) {
		UNLOCK_N_MTX_(ctx);
		return BASE_MLOCK_ERR_LOCK_OP;
	}
#else
#  error "Unsupported."
#endif
	ctx->n += locked;
	UNLOCK_N_MTX_(ctx);
	return 0;
}

#undef  ERR_
#define ERR_ "Error: Base_mlock_ctx_handled: %s\n"

void Base_mlock_ctx_handled (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, unsigned f) {
	const int c = Base_mlock_ctx(p, n, ctx);
	switch (c) {
	case 0:
		return;
#ifdef BASE_EXTERN_MLOCK_THREADSAFE
	case BASE_MLOCK_ERR_MTX_OP:
		Base_errx(ERR_, "Failed to lock or unlock mutex.");
		return;
#endif
	case BASE_MLOCK_ERR_LOCK_OP:
		if (f & BASE_MLOCK_F_GRACEFUL_LOCK_FAIL)
			return;
		Base_errx(ERR_, "Failed to memlock or memunlock.");
		return;
	case BASE_MLOCK_ERR_OVER_MEMLIMIT:
		if (f & BASE_MLOCK_F_GRACEFUL_OVERMEMLIMIT_FAIL)
			return;
		Base_errx(ERR_, "Went over memory locking limit.");
		return;
	}
	Base_errx(ERR_, "Invalid Base_mlock_ctx return code.");
}

int Base_munlock_ctx (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx) {
	const uint64_t locked = num_locked_bytes_(n, ctx->page_size);
	LOCK_N_MTX_(ctx);
	if (locked > ctx->n) {
		UNLOCK_N_MTX_(ctx);
		return BASE_MLOCK_ERR_UNDER_MEMMIN;
	}
#if    defined(BASE_OS_UNIXLIKE)
	if (munlock(p, n)) {
		UNLOCK_N_MTX_(ctx);
		return BASE_MLOCK_ERR_LOCK_OP;
	}
#elif  defined(BASE_OS_WINDOWS)
	if (!VirtualUnlock((LPVOID)p, (SIZE_T)n)) {
		UNLOCK_N_MTX_(ctx);
		return BASE_MLOCK_ERR_LOCK_OP;
	}
#else
#  error "Unsupported."
#endif
	ctx->n -= locked;
	UNLOCK_N_MTX_(ctx);
	return 0;
}

#undef  ERR_
#define ERR_ "Error: Base_munlock_ctx_handled: %s\n"

void Base_munlock_ctx_handled (R_(void*) p, uint64_t n, R_(Base_MLock*) ctx, unsigned f) {
	const int c = Base_munlock_ctx(p, n, ctx);
	switch (c) {
	case 0:
		return;
#ifdef BASE_EXTERN_MLOCK_THREADSAFE
	case BASE_MLOCK_ERR_MTX_OP:
		Base_errx(ERR_, "Failed to lock or unlock mutex.");
		return;
#endif
	case BASE_MLOCK_ERR_LOCK_OP:
		if (f & BASE_MLOCK_F_GRACEFUL_UNLOCK_FAIL)
			return;
		Base_errx(ERR_, "Failed to memlock or memunlock.");
		return;
	case BASE_MLOCK_ERR_UNDER_MEMMIN:
		Base_errx(ERR_, "Went under minimum memory locking limit.");
		return;
	}
	Base_errx(ERR_, "Invalid Base_munlock_ctx return code.");
}

#endif /* ~ ifdef BASE_HAS_MEMORYLOCKING */
