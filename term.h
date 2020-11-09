/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_TERM_H
#define SHIM_TERM_H
#include "macros.h"
#include "operations.h"

#if    defined (SHIM_OS_UNIXLIKE)
#	ifdef __NetBSD__
#		include <ncurses/ncurses.h>
#	else
#		include <ncurses.h>
#	endif
#elif  defined (SHIM_OS_WINDOWS)
#	include "errors.h"
#	include <windows.h>
#	include <conio.h>
#else
#	error "Unsupported OS."
#endif

SHIM_BEGIN_DECLS

#if    defined (SHIM_OS_UNIXLIKE)
/* On a Unixlike, do not inline term_init or term_end, since
 * doing so will mean needing to link directly with ncurses,
 * which is not necessary.
 */
SHIM_API void
shim_term_init ();

SHIM_API void
shim_term_end ();
#elif  defined (SHIM_OS_WINDOWS)
/* On Windows, inline term_init and term_end, since they're
 * just there to clear the screen.
 */
static inline void
shim_term_init () {
	system( "cls" );
}

static inline void
shim_term_end () {
	system( "cls" );
}
#else
#	error "Unsupported OS."
#endif

SHIM_API int
shim_term_get_secret_string (uint8_t * SHIM_RESTRICT    buffer,
			     char const * SHIM_RESTRICT prompt,
			     int const                  buffer_size);

SHIM_API int
shim_term_obtain_password (uint8_t *    SHIM_RESTRICT buffer,
			   char const * SHIM_RESTRICT prompt,
			   int const                  min_pw_size,
			   int const                  max_pw_size,
			   int const                  buffer_size);

SHIM_API int
shim_term_obtain_password_checked (uint8_t * SHIM_RESTRICT    password_buffer,
				   uint8_t * SHIM_RESTRICT    check_buffer,
				   char const * SHIM_RESTRICT entry_prompt,
				   char const * SHIM_RESTRICT reentry_prompt,
				   int const                  min_pw_size,
				   int const                  max_pw_size,
				   int const                  buffer_size);

SHIM_API void
shim_term_notify (char const *);

SHIM_END_DECLS

#endif /* ~ SHIM_TERM_H */
