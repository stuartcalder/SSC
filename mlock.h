/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_MLOCK_H
#define BASE_MLOCK_H

#include "macros.h"

#define BASE_MLOCK_ERR_LOCK       (-1) /* Failed a memory lock or unlock operation. */
#define BASE_MLOCK_ERR_MTX_LOCK   (-2) /* Failed a mutex lock or unlock operation. */
#define BASE_MLOCK_ERR_MISCOUNT   (-3) /* Tried to unlock more memory than was ever locked. */
#define BASE_MLOCK_ERR_GET_LIMIT  (-4) /* Failed to obtain the memory lock limit. */
#define BASE_MLOCK_ERR_SET_LIMIT  (-5) /* Failed to set the memory lock limit. */
#define BASE_MLOCK_ERR_MTX_INIT   (-6) /* Failed to initialize a mutex. */

/* OpenBSD encrypts swap by default. */
#if !defined(__OpenBSD__) && !defined(BASE_EXTERN_DISABLE_MEMORYLOCKING)
#  define BASE_HAS_MEMORYLOCKING
#  if    defined(__gnu_linux__) || defined(__FreeBSD__) || defined (__NetBSD__) || defined(__Dragonfly__) || defined(BASE_OS_MAC)
#    include <sys/types.h>
#    include <sys/time.h>
#    include <sys/resource.h>
#    define BASE_MLOCK_HAS_LIMITS
#  elif  defined(BASE_OS_WINDOWS)
#    include <windows.h>
#    include <memoryapi.h>
#    define BASE_MLOCK_HAS_LIMITS
#  endif /* ~ Platform */

#  ifdef BASE_MLOCK_HAS_LIMITS
#    define BASE_MLOCK_CURRENT_MAX_IMPL() { \
		BASE_MLOCK_MTX_LOCK(&mem_locked_mtx); \
		const bool size_ok = mem_locked < mem_limit; \
		if (size_ok) { \
			const size_t n = mem_limit - mem_locked ; \
			BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
			/* Return the maximum number of bytes that can be further locked, considering that
			 * we must lock entire pages at a time.
			 */ \
			return (n / mem_page_size) * mem_page_size; \
		} \
		BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
		return 0; \
	}
#    ifdef BASE_EXTERN_LUA_MLOCK_THREADSAFE
#      include <pthread.h>
#      define BASE_MLOCK_MTX_LOCK(m)  do { if (pthread_mutex_lock(m))   return BASE_MLOCK_ERR_MTX_LOCK; } while (0)
#      define BASE_MLOCK_MTX_ULOCK(m) do { if (pthread_mutex_unlock(m)) return BASE_MLOCK_ERR_MTX_LOCK; } while (0)
#      define BASE_MLOCK_CURRENT_MAX_MAYFAIL_MTXLOCK
#      define BASE_MLOCK_MAYFAIL_MTXLOCK
#      define BASE_MUNLOCK_MAYFAIL_MTXLOCK
#      define BASE_MLOCK_INIT_MAYFAIL_MTXINIT
#    else /* Thread safety not needed. */
#      define BASE_MLOCK_MTX_LOCK(m)  /* Nil */
#      define BASE_MLOCK_MTX_ULOCK(m) /* Nil */
#    endif /* ~ ifdef BASE_EXTERN_LUA_MLOCK_THREADSAFE */
#  else /* No memory locking limits. */
#    define BASE_MLOCK_CURRENT_MAX_IMPL() { return SIZE_MAX; }
#    define BASE_MLOCK_CURRENT_MAX_INLINE
#  endif /* ~ ifdef BASE_MLOCK_HAS_LIMITS */

#  include <stdint.h>
#  include <stdlib.h>
#  include "errors.h"
#  define BASE_MLOCK_MAYFAIL_LOCK
#  define BASE_MUNLOCK_MAYFAIL_LOCK
#  if    defined(BASE_OS_UNIXLIKE)
#    include <unistd.h>
#    ifdef BASE_MLOCK_HAS_LIMITS
#      define BASE_MLOCK_IMPL(p, n)   { \
		if (mlock(p, n)) return BASE_MLOCK_ERR_LOCK; \
		BASE_MLOCK_MTX_LOCK(&mem_locked_mtx); \
		mem_locked += (n + (n % mem_page_size)); \
		BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
		return 0; \
	}
#      define BASE_MUNLOCK_IMPL(p, n) { \
		if (munlock(p, n)) return BASE_MLOCK_ERR_LOCK; \
		const size_t total = n + (n % mem_page_size); \
		BASE_MLOCK_MTX_LOCK(&mem_locked_mtx); \
		if (total > mem_locked) { \
			BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
			return BASE_MLOCK_ERR_MISCOUNT; \
		} \
		mem_locked -= total; \
		BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
		return 0; \
	}
#      define BASE_MLOCK_SETLIMIT() do { \
		struct rlimit rl; \
		if (getrlimit(RLIMIT_MEMLOCK, &rl)) return BASE_MLOCK_ERR_GET_LIMIT; \
		if (rl.rlim_max > rl.rlim_cur) { \
			rl.rlim_cur = rl.rlim_max; \
			if (setrlimit(RLIMIT_MEMLOCK, &rl)) return BASE_MLOCK_ERR_SET_LIMIT; \
		} \
		mem_limit = (size_t)rl.rlim_cur; \
	} while (0)
#      define BASE_MLOCK_INIT_MAYFAIL_GETLIMIT
#      define BASE_MLOCK_INIT_MAYFAIL_SETLIMIT
#      define BASE_MUNLOCK_MAYFAIL_MISCOUNT
#    else
#      define BASE_MLOCK_IMPL(p, n) { return mlock(p, n); }
#      define BASE_MLOCK_INLINE
#      define BASE_MUNLOCK_IMPL(p, n) { return munlock(p, n); }
#      define BASE_MUNLOCK_INLINE
#    endif
#    include <sys/mman.h>
#  elif  defined(BASE_OS_WINDOWS)
#    include <windows.h>
#    include <memoryapi.h>
#    ifdef BASE_MLOCK_HAS_LIMITS
#      define BASE_MLOCK_IMPL(p, n)   { \
		if (!VirtualLock((LPCVOID)p, (SIZE_T)n)) return BASE_MLOCK_ERR_LOCK; \
		BASE_MLOCK_MTX_LOCK(&mem_locked_mtx); \
		mem_locked += (n + (n % mem_page_size)); \
		BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
		return 0; \
	}
#      define BASE_MUNLOCK_IMPL(p, n) { \
		if (!VirtualUnlock((LPCVOID)p, (SIZE_T)n)) return BASE_MLOCK_ERR_LOCK; \
		const size_t total = n + (n % mem_page_size); \
		BASE_MLOCK_MTX_LOCK(&mem_locked_mtx); \
		if (total > mem_locked) { \
			BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
			return BASE_MLOCK_ERR_MISCOUNT; \
		} \
		mem_locked -= total; \
		BASE_MLOCK_MTX_ULOCK(&mem_locked_mtx); \
		return 0; \
	}
#      define BASE_MLOCK_SETLIMIT() do { \
		Base_File_t proc = GetCurrentProcess(); \
		SIZE_T minimum, maximum; \
		if (!GetProcessWorkingSetSize(proc, &minimum, &maximum)) return BASE_MLOCK_ERR_GET_LIMIT; \
		mem_limit = (size_t)minimum; \
	} while (0)
#      define BASE_MLOCK_INIT_MAYFAIL_GETLIMIT
#      define BASE_MUNLOCK_MAYFAIL_MISCOUNT
#    else /* There are no memory limits. */
#      define BASE_MLOCK_IMPL(p, n)   { if (!VirtualLock(p, n))   return BASE_MLOCK_ERR_LOCK; return 0; }
#      define BASE_MUNLOCK_IMPL(p, n) { if (!VirtualUnlock(p, n)) return BASE_MLOCK_ERR_LOCK; return 0; }
#    endif /* ~ ifdef BASE_MLOCK_HAS_LIMITS */
#  else
#    error "Unsupported operating system."
#  endif /* ~ if defined (BASE_OS_UNIXLIKE) elif defined (BASE_OS_WINDOWS) */

#  define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_DECLS
#  ifdef BASE_MLOCK_INLINE
BASE_INLINE int Base_mlock (R_(void*) p, size_t n) BASE_MLOCK_IMPL(p, n)
#  else
BASE_API    int Base_mlock (R_(void*), size_t);
#  endif
BASE_API void Base_mlock_or_die (R_(void*), size_t);
#  ifdef BASE_MUNLOCK_INLINE
BASE_INLINE int Base_munlock (R_(void*) p, size_t n) BASE_MUNLOCK_IMPL(p, n)
#  else
BASE_API    int Base_munlock (R_(void*), size_t);
#  endif
BASE_API void   Base_munlock_or_die (R_(void*), size_t);
BASE_API int    Base_mlock_init (void);
BASE_API void   Base_mlock_init_or_die (void);
#  ifdef BASE_MLOCK_CURRENT_MAX_INLINE
BASE_INLINE size_t Base_mlock_current_max (void) BASE_MLOCK_CURRENT_MAX_IMPL()
#  else
BASE_API    size_t Base_mlock_current_max (void);
#  endif
BASE_END_DECLS
#  undef R_
#endif /* ~ if defined (ENABLE_MEMORYLOCKING_) and !defined (BASE_EXTERN_DISABLE_MEMORYLOCKING) */

#endif /* ~ ifndef BASE_MLOCK_H */
