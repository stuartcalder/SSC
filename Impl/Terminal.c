/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "Terminal.h"

#define R_ SSC_RESTRICT

#if defined(SSC_OS_UNIXLIKE)
 #define NEWLINE_ "\n"
 #ifdef __has_include
  #if __has_include(<ncurses.h>)
   #include <ncurses.h>
  #elif __has_include(<ncurses/ncurses.h>)
   #include <ncurses/ncurses.h>
  #endif
 #else
  #ifdef __NetBSD__
   #include <ncurses/ncurses.h>
  #else
   #include <ncurses.h>
  #endif
 #endif
#elif defined(SSC_OS_WINDOWS)
 #define NEWLINE_ "\n\r"
 #include <windows.h>
#else
 #error "Unsupported OS."
#endif

#define OS_PROMPT_ "> " NEWLINE_

#ifdef SSC_OS_UNIXLIKE
/* On Windows these functions are inlined. */
void SSC_Terminal_init(void) { initscr(); clear(); }
void SSC_Terminal_end(void)  { endwin(); }
#endif /* ~ SSC_OS_UNIXLIKE */

#define SECRET_STR_MIN_BUFSIZE_ 2
int
SSC_Terminal_getSecretString(
 uint8_t*    R_ buffer,
 const char* R_ prompt,
 const int      buffer_size)
#if defined(SSC_OS_UNIXLIKE)
{ /* Unixlike impl */
  SSC_assertMsg(
   buffer_size >= SECRET_STR_MIN_BUFSIZE_,
   "Error: Buffer in SSC_Terminal_getSecretString has buffer size less than " SSC_STRINGIFY(SECRET_STR_MIN_BUFSIZE_) ".\n");
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
  const int pw_size = strlen((char*)buffer);
  delwin(w);
  return pw_size;
} /* ~ Unixlike impl */
#elif  defined(SSC_OS_WINDOWS)
{ /* Windows impl */
  SSC_assertMsg(
   buffer_size >= SECRET_STR_MIN_BUFSIZE_,
   "Error: Buffer in SSC_Terminal_getSecretString has buffer size less than " SSC_STRINGIFY(SECRET_STR_MIN_BUFSIZE_) ".\n");
  const int max_pw_size = buffer_size - 1;
  int index = 0;
  bool repeat_ui, repeat_input;
  repeat_ui = true;
  while (repeat_ui) {
    memset(buffer, 0, buffer_size);
    system("cls");
    SSC_assertMsg(!_cputs(prompt), "Failed to _cputs().\n");
    repeat_input = true;
    while (repeat_input) {
      int ch = _getch();
      switch (ch) {
        case '\b': {
          if (index > 0) {
            SSC_assertMsg(!_cputs("\b \b"), "Failed to _cputs().\n");
            buffer[--index] = 0;
          }
        } break;
        case '\r': {
          repeat_input = false;
        } break;
        default: {
          if ((index < buffer_size) && (ch >= 32) && (ch <= 126)) {
            SSC_assertMsg(_putch('*') != EOF, "Failed to _putch().\n");
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
 #error "Unsupported OS."
#endif

int
SSC_Terminal_getPassword(
 uint8_t*    R_ password_buf,
 const char* R_ entry_prompt,
 const int      min_pw_size,
 const int      max_pw_size,
 const int      buffer_size)
{
  SSC_assertMsg(
   buffer_size >= (max_pw_size + 1),
   "Error: Buffer size in SSC_Terminal_getPassword too small (%d) for max password size (%d).\n", buffer_size, max_pw_size);
  int size;
  while (1) {
    size = SSC_Terminal_getSecretString(password_buf, entry_prompt, buffer_size);
    if (size < min_pw_size) { SSC_Terminal_notify("Password is not long enough." NEWLINE_); continue; }
    if (size > max_pw_size) { SSC_Terminal_notify("password is too long." NEWLINE_); continue; }
    break;
  }
  return size;
}

int
SSC_Terminal_getPasswordChecked(
 uint8_t*    R_ password_buf,
 uint8_t*    R_ check_buf,
 const char* R_ entry_prompt,
 const char* R_ reentry_prompt,
 const int      min_pw_size,
 const int      max_pw_size,
 const int      buffer_size)
{
  int size;
  while (1) {
    size = SSC_Terminal_getSecretString(password_buf, entry_prompt, buffer_size);
    if (size < min_pw_size) { SSC_Terminal_notify("Password is not long enough." NEWLINE_); continue; }
    if (size > max_pw_size) { SSC_Terminal_notify("Password is too long." NEWLINE_); continue; }
    if (SSC_Terminal_getSecretString(check_buf, reentry_prompt, buffer_size) != size) {
      SSC_Terminal_notify("Second password is not the same size as the first." NEWLINE_);
      continue;
    }
    if (SSC_constTimeMemDiff(password_buf, check_buf, buffer_size)) {
      SSC_Terminal_notify("Passwords do not match." NEWLINE_);
      continue;
    }
    break;
  }
  return size;
}

void
SSC_Terminal_notify(const char* notice)
{
#if defined(SSC_OS_UNIXLIKE)
  WINDOW* w = newwin(1, strlen(notice) + 1, 0, 0);
  wclear(w);
  wmove(w, 0, 0);
  waddstr(w, notice);
  wrefresh(w);
  wgetch(w);
  delwin(w);
#elif defined(SSC_OS_WINDOWS)
  system("cls");
  SSC_assertMsg(!_cputs(notice), "Error: Failed to _cputs().\n");
  system("pause");
  system("cls");
#else
 #error "Unsupported OS."
#endif
}
