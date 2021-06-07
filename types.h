#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <limits.h>
#include <stdint.h>
#include "macros.h"

#ifdef BASE_OS_UNIXLIKE
#	include <sys/types.h>
#else
#	ifndef SSIZE_MAX
#		if !defined(INT64_MAX) || !defined(INT64_MIN)
#			error "We need these defined!"
#		endif
		typedef int64_t ssize_t;
#		define SSIZE_MAX INT64_MAX
#		define SSIZE_MIN INT64_MIN
#		define PRINT_SSIZE PRIi64
#	endif
#endif

#endif /* ~ifndef BASE_TYPES_H */
