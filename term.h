#ifndef SHIM_TERM_H
#define SHIM_TERM_H
#include "macros.h"
#include "operations.h"

#define SHIM_TERM_MAX_PW_SIZE	120
#define SHIM_TERM_BUFFER_SIZE	(SHIM_TERM_MAX_PW_SIZE + 1)
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

void SHIM_PUBLIC
shim_term_init ();

void SHIM_PUBLIC
shim_term_end ();

int SHIM_PUBLIC
shim_term_get_secret_string (uint8_t * SHIM_RESTRICT,
			     char const *    SHIM_RESTRICT);

int SHIM_PUBLIC
shim_term_obtain_password (uint8_t *    SHIM_RESTRICT,
			   char const * SHIM_RESTRICT,
			   int const,
			   int const);

int SHIM_PUBLIC
shim_term_obtain_password_checked (uint8_t * SHIM_RESTRICT,
				   uint8_t * SHIM_RESTRICT,
				   char const * SHIM_RESTRICT,
				   char const * SHIM_RESTRICT,
				   int const,
				   int const);

void SHIM_PUBLIC
shim_term_notify (char const *);

SHIM_END_DECLS

#endif /* ~ SHIM_TERM_H */
