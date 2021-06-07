#include "files.h"
#include "mlock.h"
#include "mem.h"

#ifdef BASE_HAS_MEMORYLOCKING

static size_t          mem_page_size = 0;
#ifdef BASE_MLOCK_HAS_LIMITS
static size_t          mem_limit = 0;
static size_t          mem_locked = 0;
#  ifdef BASE_EXTERN_LUA_MLOCK_THREADSAFE
static pthread_mutex_t mem_locked_mtx;
#  endif
#endif

#define R_(ptr) ptr BASE_RESTRICT

#ifndef BASE_MLOCK_INLINE
int Base_mlock (R_(void*) p, size_t n) BASE_MLOCK_IMPL(p, n)
#endif

void Base_mlock_or_die (R_(void*) address, size_t size) {
	int c = Base_mlock(address, size);
	switch (c) {
	case 0:
		return;
#ifdef BASE_MLOCK_MAYFAIL_LOCK
	case BASE_MLOCK_ERR_LOCK:
		Base_errx("Error: Base_mlock failed.\n");
#endif
#ifdef BASE_MLOCK_MAYFAIL_MTXLOCK
	case BASE_MLOCK_ERR_MTX_LOCK:
		Base_errx("Error: Failed to lock mem_locked_mtx.\n");
#endif
	}
}

#ifndef BASE_MUNLOCK_INLINE
int Base_munlock (R_(void*) p, size_t n) BASE_MUNLOCK_IMPL(p, n)
#endif

void Base_munlock_or_die (R_(void*) address, size_t size) {
	int c = Base_munlock(address, size);
	switch (c) {
	case 0:
		return;
#ifdef BASE_MUNLOCK_MAYFAIL_LOCK
	case BASE_MLOCK_ERR_LOCK:
		Base_errx("Error: Base_munlock failed.\n");
#endif
#ifdef BASE_MUNLOCK_MAYFAIL_MTXLOCK
	case BASE_MLOCK_ERR_MTX_LOCK:
		Base_errx("Error: Failed to unlock mem_locked_ctx.\n");
#endif
#ifdef BASE_MUNLOCK_MAYFAIL_MISCOUNT
	case BASE_MLOCK_ERR_MISCOUNT:
		Base_errx("Error: Attempted to unlock more memory than was ever successfully locked.\n");
#endif
	}
}

int Base_mlock_init (void) {
	if (!mem_page_size) {
		mem_page_size = Base_get_pagesize();
#ifdef BASE_MLOCK_HAS_LIMITS
		mem_locked = 0;
#  ifdef BASE_EXTERN_LUA_MLOCK_THREADSAFE
		if (!pthread_mutex_init(&mem_locked_mtx, NULL))
			return BASE_MLOCK_ERR_MTX_INIT;
#  endif
		BASE_MLOCK_SETLIMIT();
#endif
	}
	return 0;
}

void Base_mlock_init_or_die (void) {
	int c = Base_mlock_init();
	switch (c) {
	case 0:
		return;
#ifdef BASE_MLOCK_INIT_MAYFAIL_MTXINIT:
	case BASE_MLOCK_ERR_MTX_INIT:
		Base_errx("Error: Failed to initialize mem_locked_ctx.\n");
#endif
#ifdef BASE_MLOCK_INIT_MAYFAIL_GETLIMIT:
	case BASE_MLOCK_ERR_GET_LIMIT:
		Base_errx("Error: Failed to get memory locking limit.\n");
#endif
#ifdef BASE_MLOCK_INIT_MAYFAIL_SETLIMIT:
	case BASE_MLOCK_ERR_SET_LIMIT:
		Base_errx("Error: Failed to set memory locking limit.\n");
#endif
	}
}

#ifndef BASE_MLOCK_CURRENT_MAX_INLINE
size_t Base_mlock_current_max (void) BASE_MLOCK_CURRENT_MAX_IMPL()
#endif

#endif /* ~ ifdef BASE_HAS_MEMORYLOCKING */
