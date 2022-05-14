/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include "term.h"
#include "macros.h"

#if    defined(BASE_OS_UNIXLIKE)
#	define NEWLINE_	"\n"
#	ifdef __NetBSD__
#		include <ncurses/ncurses.h>
#	else
#		include <ncurses.h>
#	endif
#elif  defined(BASE_OS_WINDOWS)
#	define NEWLINE_	"\n\r"
#	include <windows.h>
#else
#	error "Unsupported OS."
#endif

#define OS_PROMPT_ "> " NEWLINE_

#ifdef BASE_OS_UNIXLIKE
/* On Windows these functions are inlined. */
void Base_term_init (void) { initscr(); clear(); }
void Base_term_end  (void) { endwin();           }
#endif /* ~ BASE_OS_UNIXLIKE */

#define SECRET_STR_MIN_BUFSIZE_ 2
#define R_(Ptr) Ptr BASE_RESTRICT
int Base_term_get_secret_string (R_(uint8_t*)    buffer,
                                 R_(const char*) prompt,
				 const int       buffer_size)
#if    defined(BASE_OS_UNIXLIKE)
{ /* Unixlike impl */
	#if 0
	Base_assert_msg(
	 buffer_size >= SECRET_STR_MIN_BUFSIZE_,
	 "Error: Buffer in Base_term_get_secret_string has buffer size less than " BASE_STRINGIFY(SECRET_STR_MIN_BUFSIZE_) ".\n"
	);
	const int max_pw_size = buffer_size - 1;
	int index = 0;
	bool outer, inner;
	WINDOW* w;
	enum {
	  STATE_0_, /* .   */
	  STATE_1_, /*  .  */
	  STATE_2_  /*   . */
	}; typedef int_fast8_t State_t;
	State_t state = STATE_0_;

	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	w = newwin(5, max_pw_size + 10, 0, 0);
	keypad(w, TRUE);
	outer = true;
	while (outer) {
	  memset(buffer, 0, buffer_size);
	  wclear(w);
	  wmove(w, 1, 0);
	  waddstr(w, prompt);
	  inner = true;
	  while (inner) {
	    int ch = wgetch(w);
	    switch (ch) {
	    case 127:
	    case KEY_DC:
	    case KEY_LEFT:
	    case KEY_BACKSPACE:
	      if (index > 0) {
	        int y, x;
		getyx(w, y, x);
		wdelch(w);
		wmove(w, y, (x - 1));
		buffer[--index] = 0;
	      }
	      break;
	    case '\n':
	    case KEY_ENTER:
	      inner = false;
	      break;
	    default:
	      if (index < max_pw_size) {
	        waddch(w, '*');
		wrefresh(w);
		buffer[index++] = (uint8_t)ch;
	      }
	    }
	  } /* ~ while(inner) */
	  outer = false;
	} /* ~ while(outer) */
	const int pw_size = strlen((char*)buffer);
	delwin(w);
	return pw_size;
	#else
	BASE_ASSERT(buffer && prompt);
	Base_assert_msg(buffer_size >= SECRET_STR_MIN_BUFSIZE_,
		"Error: Buffer in Base_term_get_secret_string has buffer size less than " BASE_STRINGIFY(SECRET_STR_MIN_BUFSIZE_) ".\n");
	const int max_pw_size = buffer_size - 1;
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	int index = 0;
	WINDOW* w = newwin(5, max_pw_size + 10, 0, 0);
	keypad(w, TRUE);
	bool outer, inner;
	outer = true;
	while (outer) {
		memset(buffer, 0, buffer_size);
		wclear(w);
		wmove(w, 1, 0);
		waddstr(w, prompt);
		inner = true;
		while (inner) {
			int ch = wgetch(w);
			switch(ch) {
				/* Delete */
				case 127:
				case KEY_DC:
				case KEY_LEFT:
				case KEY_BACKSPACE: {
					if (index > 0) {
						int y, x;
						getyx(w, y, x);
						wdelch(w);
						wmove(w, y, (x - 1));
						wrefresh(w);
						buffer[--index] = 0;
					}
				} break;
				/* Return */
				case '\n':
				case KEY_ENTER: {
					inner = false;
				} break;
				default: {
					if (index < max_pw_size) {
						waddch(w, '*');
						wrefresh(w);
						buffer[index++] = (uint8_t)ch;
					}
				} break;
			} /* ~ switch(ch) */
		} /* ~ while(inner) */
		outer = false;
	} /* ~ while(outer) */
	const int pw_size = strlen((char *)buffer);
	delwin(w);
	return pw_size;
	#endif
} /* ~ Unixlike impl */
#elif  defined(BASE_OS_WINDOWS)
{ /* Windows impl */
	BASE_ASSERT(buffer && prompt);
	Base_assert_msg(buffer_size >= SECRET_STR_MIN_BUFSIZE_,
		"Error: Buffer in Base_term_get_secret_string has buffer size less than " BASE_STRINGIFY(SECRET_STR_MIN_BUFSIZE_) ".\n");
	const int max_pw_size = buffer_size - 1;
	int index = 0;
	bool repeat_ui, repeat_input;
	repeat_ui = true;
	while (repeat_ui) {
		memset(buffer, 0, buffer_size);
		system("cls");
		Base_assert_msg(!_cputs(prompt), "Failed to _cputs().\n");
		repeat_input = true;
		while (repeat_input) {
			int ch = _getch();
			switch (ch) {
				case '\b': {
					if (index > 0) {
						Base_assert_msg(!_cputs("\b \b"), "Failed to _cputs().\n");
						buffer[--index] = 0;
					}
				} break;
				case '\r': {
					repeat_input = false;
				} break;
				default: {
					if ((index < buffer_size) && (ch >= 32) && (ch <= 126)) {
						Base_assert_msg(_putch('*') != EOF, "Failed to _putch().\n");
						buffer[index++] = (uint8_t)ch;
					}
				} break;
			} /* ~ switch(ch) */
		} /* ~ while(repeat_input) */
		repeat_ui = false;
	} /* ~ while(repeat_ui) */
	const int pw_size = strlen((char *)buffer);
	system("cls");
	return pw_size;
} /* ~ Windows impl */
#else
#	error "Unsupported OS."
#endif

int Base_term_obtain_password (R_(uint8_t*)    password_buf,
                               R_(const char*) entry_prompt,
			       const int       min_pw_size,
			       const int       max_pw_size,
			       const int       buffer_size)
{
	BASE_ASSERT(password_buf && entry_prompt);
	BASE_ASSERT(min_pw_size >= 1);
	Base_assert_msg(buffer_size >= (max_pw_size + 1),
		"Error: Buffer size in Base_term_obtain_password too small (%d) for max password size (%d).\n", buffer_size, max_pw_size);
	int size;
	while (1) {
		size = Base_term_get_secret_string(password_buf, entry_prompt, buffer_size);
		if (size < min_pw_size) { Base_term_notify("Password is not long enough." NEWLINE_); continue; }
		if (size > max_pw_size) { Base_term_notify("password is too long." NEWLINE_); continue; }
		break;
	}
	return size;
}

int Base_term_obtain_password_checked (R_(uint8_t*)    password_buf,
                                       R_(uint8_t*)    check_buf,
				       R_(const char*) entry_prompt,
				       R_(const char*) reentry_prompt,
				       const int       min_pw_size,
				       const int       max_pw_size,
				       const int       buffer_size)
{
	BASE_ASSERT(password_buf && check_buf && entry_prompt && reentry_prompt);
	int size;
	while (1) {
		size = Base_term_get_secret_string(password_buf, entry_prompt, buffer_size);
		if (size < min_pw_size) { Base_term_notify("Password is not long enough." NEWLINE_); continue; }
		if (size > max_pw_size) { Base_term_notify("Password is too long." NEWLINE_); continue; }
		if (Base_term_get_secret_string(check_buf, reentry_prompt, buffer_size) != size) {
			Base_term_notify("Second password is not the same size as the first." NEWLINE_);
			continue;
		}
		if (Base_ctime_memdiff(password_buf, check_buf, buffer_size)) {
			Base_term_notify("Passwords do not match." NEWLINE_);
			continue;
		}
		break;
	}
	return size;
}

void Base_term_notify (const char* notice) {
	BASE_ASSERT(notice);
#if    defined (BASE_OS_UNIXLIKE)
	WINDOW* w = newwin(1, strlen(notice) + 1, 0, 0);
	wclear(w);
	wmove(w, 0, 0);
	waddstr(w, notice);
	wrefresh(w);
	wgetch(w);
	delwin(w);
#elif  defined (BASE_OS_WINDOWS)
	system("cls");
	Base_assert_msg(!_cputs(notice), "Error: Failed to _cputs().\n");
	system("pause");
	system("cls");
#else
#	error "Unsupported OS."
#endif
}
