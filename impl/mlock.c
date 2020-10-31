#include "mlock.h"

#ifdef SHIM_FEATURE_MEMORYLOCKING

void
shim_lock_memory (void * address, size_t const size) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( mlock( address, size ) != 0 )
		SHIM_ERRX ("Error: Failed to mlock()\n");
#elif  defined (SHIM_OS_WINDOWS)
	if( VirtualLock( (void *)address, size ) == 0 )
		SHIM_ERRX ("Error: Failed to VirtualLock()\n");
#else
#	error "Unsupported OS."
#endif
}

void
shim_unlock_memory (void * address, size_t const size) {
#if    defined (SHIM_OS_UNIXLIKE)
	if( munlock( address, size ) != 0 )
		SHIM_ERRX ("Error: Faield to munlock()\n");
#elif  defined (SHIM_OS_WINDOWS)
	if( VirtualUnlock( address, size ) == 0 )
		SHIM_ERRX ("Error: Failed to VirtualUnlock()\n");
#else
#	error "Unsupported OS."
#endif
}

#endif /* SHIM_FEATURE_MEMORYLOCKING */
