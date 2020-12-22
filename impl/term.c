#include "term.h"

#if    defined (SHIM_OS_UNIXLIKE)
#	define NEWLINE_	"\n"
#	ifdef __NetBSD__
#		include <ncurses/ncurses.h>
#	else
#		include <ncurses.h>
#	endif
#elif  defined (SHIM_OS_WINDOWS)
#	define NEWLINE_	"\n\r"
#	include <windows.h>
#else
#	error "Unsupported OS."
#endif

#define OS_PROMPT_ "> " NEWLINE_

#ifdef SHIM_OS_UNIXLIKE
/* On Windows these functions are inlined. */
void
shim_term_init () {
	initscr();
	clear();
}
void
shim_term_end () {
	endwin();
}
#endif /* ~ SHIM_OS_UNIXLIKE */

#define STR_IMPL_(text) #text
#define STR_(text)	STR_IMPL_ (text)
#define SECRET_STR_MIN_BUFSIZE_ 2
int
shim_term_get_secret_string (uint8_t *    SHIM_RESTRICT buffer,
			     char const * SHIM_RESTRICT prompt,
			     int const                  buffer_size)
#if    defined (SHIM_OS_UNIXLIKE)
{ /* Unixlike impl */
	if( buffer_size < SECRET_STR_MIN_BUFSIZE_ )
		SHIM_ERRX ("Error: Buffer in shim_term_get_secret_string has buffer size less than " STR_ (SECRET_STR_MIN_BUFSIZE_) ".\n");
	int const max_pw_size = buffer_size - 1;
	cbreak();
	noecho();
	keypad( stdscr, TRUE );
	int index = 0;
	WINDOW * w = newwin( 5, max_pw_size + 10, 0, 0 );
	keypad( w, TRUE );
	bool outer, inner;
	outer = true;
	while( outer ) {
		memset( buffer, 0, buffer_size );
		wclear( w );
		wmove( w, 1, 0 );
		waddstr( w, prompt );
		inner = true;
		while( inner ) {
			int ch = wgetch( w );
			switch( ch ) {
				/* Delete */
				case 127:
				case KEY_DC:
				case KEY_LEFT:
				case KEY_BACKSPACE: {
					if( index > 0 ) {
						int y, x;
						getyx( w, y, x );
						wdelch( w );
						wmove( w, y, x - 1 );
						wrefresh( w );
						buffer[ --index ] = UINT8_C (0);
					}
				} break;
				/* Return */
				case '\n':
				case KEY_ENTER: {
					inner = false;
				} break;
				default: {
					if( index < max_pw_size ) {
						waddch( w, '*' );
						wrefresh( w );
						buffer[ index++ ] = (uint8_t)ch;
					}
				} break;
			} /* ~ switch(ch) */
		} /* ~ while(inner) */
		outer = false;
	} /* ~ while(outer) */
	int const pw_size = strlen( (char *)buffer );
	delwin( w );
	return pw_size;
} /* ~ Unixlike impl */
#elif  defined (SHIM_OS_WINDOWS)
{ /* Windows impl */
	if( buffer_size < SECRET_STR_MIN_BUFSIZE_ )
		SHIM_ERRX ("Error: Buffer in shim_term_get_secret_string has buffer size less than " STR_ (SECRET_STR_MIN_BUFSIZE_) ".\n");
	int const max_pw_size = buffer_size - 1;
	int index = 0;
	bool repeat_ui, repeat_input;
	repeat_ui = true;
	while( repeat_ui ) {
		memset( buffer, 0, buffer_size );
		system( "cls" );
		if( _cputs( prompt ) != 0 )
			SHIM_ERRX ("Failed to _cputs()!\n");
		repeat_input = true;
		while( repeat_input ) {
			int ch = _getch();
			switch( ch ) {
				case '\b': {
					if( index > 0 ) {
						if( _cputs( "\b \b" ) != 0 )
							SHIM_ERRX ("Failed to _cputs()!\n");
						buffer[ --index ] = UINT8_C (0);
					}
				} break;
				case '\r': {
					repeat_input = false;
				} break;
				default: {
					if( (index < buffer_size) && (ch >= 32) && (ch <= 126) ) {
						if( _putch( "*" ) == EOF )
							SHIM_ERRX ("Failed to _putch()!\n");
						buffer[ index++ ] = (uint8_t)ch;
					}
				} break;
			} /* ~ switch(ch) */
		} /* ~ while(repeat_input) */
		repeat_ui = false;
	} /* ~ while(repeat_ui) */
	int const pw_size = strlen( (char *)buffer );
	system( "cls" );
	return pw_size;
} /* ~ Windows impl */
#else
#	error "Unsupported OS."
#endif

int
shim_term_obtain_password (uint8_t *    SHIM_RESTRICT password_buf,
			   char const * SHIM_RESTRICT entry_prompt,
			   int const                  min_pw_size,
			   int const                  max_pw_size,
			   int const                  buffer_size)
{
	if( buffer_size < (max_pw_size + 1) )
		SHIM_ERRX ("Error: Buffer size in shim_term_obtain_password too small (%d) for max password size (%d).\n",
			   buffer_size, max_pw_size);
	int size;
	while( 1 ) {
		size = shim_term_get_secret_string( password_buf, entry_prompt, buffer_size );
		if( size < min_pw_size )
			shim_term_notify( "Password is not long enough." NEWLINE_ );
		else if( size > max_pw_size )
			shim_term_notify( "Password is too long." NEWLINE_ );
		else
			break;
	}
	return size;
}

int
shim_term_obtain_password_checked (uint8_t *    SHIM_RESTRICT password_buf,
				   uint8_t *    SHIM_RESTRICT check_buf,
				   char const * SHIM_RESTRICT entry_prompt,
				   char const * SHIM_RESTRICT reentry_prompt,
				   int const                  min_pw_size,
				   int const                  max_pw_size,
				   int const                  buffer_size)
{
	int size;
	while( 1 ) {
		size = shim_term_get_secret_string( password_buf, entry_prompt, buffer_size );
		if( size < min_pw_size ) {
			shim_term_notify( "Password is not long enough." NEWLINE_ );
			continue;
		} else if( size > max_pw_size ) {
			shim_term_notify( "Password is too long." NEWLINE_ );
			continue;
		} else if( shim_term_get_secret_string( check_buf, reentry_prompt, buffer_size ) != size ) {
			shim_term_notify( "Second password not the same size as the first." NEWLINE_ );
			continue;
		}
		if( shim_ctime_memdiff( password_buf, check_buf, buffer_size ) == 0 )
			break;
		shim_term_notify( "Passwords do not match." NEWLINE_ );
	}
	return size;
}

void
shim_term_notify (char const * notice) {
#if    defined (SHIM_OS_UNIXLIKE)
	WINDOW * w = newwin( 1, strlen( notice ) + 1, 0, 0 );
	wclear( w );
	wmove( w, 0, 0 );
	waddstr( w, notice );
	wrefresh( w );
	wgetch( w );
	delwin( w );
#elif  defined (SHIM_OS_WINDOWS)
	system( "cls" );
	if( _cputs( notice ) != 0 )
		SHIM_ERRX ("Error: Failed to _cputs()\n");
	system( "pause" );
	system( "cls" );
#else
#	error "Unsupported OS."
#endif
}
