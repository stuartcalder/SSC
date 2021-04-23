#ifndef SHIM_TYPES_H
#define SHIM_TYPES_H

#include "macros.h"
#include <stdint.h>
#include <limits.h>

#ifdef SHIM_OS_UNIXLIKE
#	include <sys/types.h>
#else
#	ifndef SSIZE_MAX
		typedef int64_t ssize_t;
#		define SSIZE_MAX INT64_MAX
#	endif
#endif

#endif /* ~ifndef SHIM_TYPES_H */
