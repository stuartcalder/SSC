/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_MLOCK_H
#define SHIM_MLOCK_H

#include "macros.h"

#ifdef ENABLE_MEMORYLOCKING_
#	error "ENABLE_MEMORYLOCKING_ already defined"
#endif

#if    (defined (SHIM_OS_UNIXLIKE) && !defined (__OpenBSD__)) || defined (SHIM_OS_WINDOWS)
#	define ENABLE_MEMORYLOCKING_
#endif

#if    defined (ENABLE_MEMORYLOCKING_) && !defined (SHIM_EXT_DISABLE_MEMORYLOCKING)
#	undef ENABLE_MEMORYLOCKING_
#	ifdef SHIM_FEATURE_MEMORYLOCKING
#		error "SHIM_FEATURE_MEMORYLOCKING already defined"
#	endif
#	define SHIM_FEATURE_MEMORYLOCKING

#	include <stdlib.h>
#	include <stdint.h>
#	include "errors.h"

#	if    defined (SHIM_OS_UNIXLIKE)
#		include <sys/mman.h>
#	elif  defined (SHIM_OS_WINDOWS)
#		include <windows.h>
#		include <memoryapi.h>
#	else
#		error "Unsupported operating system."
#	endif

#	ifdef __cplusplus
#		define CONST_CAST_(value, type) const_cast<type>(value)
#	else
#		define CONST_CAST_(value, type) ((type)value)
#	endif

#	if    defined (SHIM_OS_UNIXLIKE)
#		define LOCK_CONST_ const
#	elif  defined (SHIM_OS_WINDOWS)
#		define LOCK_CONST_ /*null macro*/
#	else
#		error "Unsupported operating system."
#	endif

SHIM_BEGIN_DECLS

static inline void
shim_lock_memory (void LOCK_CONST_ *address, size_t const length) {
#	if    defined (SHIM_OS_UNIXLIKE)
	if( mlock( address, length ) != 0 )
		SHIM_ERRX ("Error: Failed to mlock()\n");
#	elif  defined (SHIM_OS_WINDOWS)
	if( VirtualLock( ((void *)address), length ) == 0 )
		SHIM_ERRX ("Error: Failed to VirtualLock()\n");
#	else
#		error "Unsupported operating system."
#	endif
}

static inline void
shim_unlock_memory (void LOCK_CONST_ *address, size_t const length) {
#	if    defined (SHIM_OS_UNIXLIKE)
	if( munlock( address, length ) != 0 )
		SHIM_ERRX ("Error: Failed to munlock()\n");
#	elif  defined (SHIM_OS_WINDOWS)
	if( VirtualUnlock( ((void *)address), length ) == 0 )
		SHIM_ERRX ("Error: Failed to VirtualUnlock()\n");
#	else
#		error "Unsupported operating system."
#	endif
}

#	undef LOCK_CONST_
#	undef CONST_CAST_

SHIM_END_DECLS

#endif // ~ ENABLE_MEMORYLOCKING_


#endif // ~ SHIM_MLOCK_H
