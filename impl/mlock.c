#include "mlock.h"

#ifdef SHIM_HAS_MEMORYLOCKING

int
shim_lock_memory (void * SHIM_RESTRICT address, size_t const size) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( mlock( address, size ) != 0 )
		return -1;
#elif  defined (SHIM_OS_WINDOWS)
	if( VirtualLock( address, size  ) == 0 )
		return -1;
#else
#	error "Unsupported operating system."
#endif
	return 0;
}

void
shim_enforce_lock_memory (void * SHIM_RESTRICT address, size_t const size) {
	if( shim_lock_memory( address, size ) )
		SHIM_ERRX ("Error: shim_enforce_lock_memory failed!\n");
}

int
shim_unlock_memory (void * SHIM_RESTRICT address, size_t const size) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( munlock( address, size ) != 0 )
		return -1;
#elif  defined (SHIM_OS_WINDOWS)
	if( VirtualUnlock( address, size ) == 0 )
		return -1;
#else
#	error "Unsupported operating system."
#endif
	return 0;
}

void
shim_enforce_unlock_memory (void * SHIM_RESTRICT address, size_t const size) {
	if( shim_unlock_memory( address, size ) )
		SHIM_ERRX ("Error: shim_enforce_unlock_memory failed!\n");
}

#endif /* ~ ifdef SHIM_HAS_MEMORYLOCKING */
