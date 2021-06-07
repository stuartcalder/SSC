/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_TERM_H
#define BASE_TERM_H

#include "macros.h"
#include "operations.h"

#if    defined(BASE_OS_UNIXLIKE)
#	ifdef __NetBSD__
#		include <ncurses/ncurses.h>
#	else
#		include <ncurses.h>
#	endif
#elif  defined(BASE_OS_WINDOWS)
#	include <conio.h>
#	include <windows.h>
#	include "errors.h"
#else
#	error "Unsupported OS."
#endif

BASE_BEGIN_DECLS
#if    defined(BASE_OS_UNIXLIKE)
/* On a Unixlike, do not inline term_init or term_end, since
 * doing so will mean needing to link directly with ncurses,
 * which is not necessary.
 */
BASE_API void Base_term_init (void);
BASE_API void Base_term_end (void);
#elif  defined(BASE_OS_WINDOWS)
/* On Windows, inline term_init and term_end, since they're
 * just there to clear the screen.
 */
BASE_INLINE void Base_term_init (void) { system("cls"); }
BASE_INLINE void Base_term_end  (void) { system("cls"); }
#else
#	error "Unsupported."
#endif
#define R_(ptr) ptr BASE_RESTRICT
BASE_API int Base_term_get_secret_string (R_(uint8_t*)    buffer,
                                          R_(const char*) prompt,
					  const int       buffer_size);
BASE_API int Base_term_obtain_password (R_(uint8_t*)    buffer,
                                        R_(const char*) prompt,
				        const int       min_pw_size,
				        const int       max_pw_size,
				        const int       buffer_size);
BASE_API int Base_term_obtain_password_checked (R_(uint8_t*)    password_buffer,
                                                R_(uint8_t*)    check_buffer,
					        R_(const char*) entry_prompt,
					        R_(const char*) reentry_prompt,
					        const int       min_pw_size,
					        const int       max_pw_size,
					        const int       buffer_size);
BASE_API void Base_term_notify (char const *);
BASE_END_DECLS
#undef R_

#endif /* ~ BASE_TERM_H */
