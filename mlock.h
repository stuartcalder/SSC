/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_MLOCK_H
#define SHIM_MLOCK_H

#include "macros.h"

#ifdef ENABLE_MEMORYLOCKING_
#	error "ENABLE_MEMORYLOCKING_ already defined."
#endif

#if    (defined (SHIM_OS_UNIXLIKE) && !defined (__OpenBSD__)) || defined (SHIM_OS_WINDOWS)
#	define ENABLE_MEMORYLOCKING_
#endif

#if    defined (ENABLE_MEMORYLOCKING_) && !defined (SHIM_EXT_DISABLE_MEMORYLOCKING)
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


SHIM_BEGIN_DECLS
/* shim_lock_memory (memory_ptr, num_bytes)
 * Lock the memory starting @ memory_ptr from being paged into swap memory.
 * 	No return value. */
SHIM_API void
shim_lock_memory (void * SHIM_RESTRICT, size_t const);
/* shim_unlock_memory (memory_ptr, num_bytes)
 * Unlock the memory starting @ memory_ptr.
 * 	No return value. */
SHIM_API void
shim_unlock_memory (void * SHIM_RESTRICT, size_t const);
SHIM_END_DECLS

#endif /* ~ ENABLE_MEMORYLOCKING_ */
#undef ENABLE_MEMORYLOCKING_


#endif /* ~ SHIM_MLOCK_H */
