/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_TERMINAL_H
#define SSC_TERMINAL_H

#include "Macro.h"
#include "Operation.h"

#if defined(SSC_OS_UNIXLIKE)
 #ifdef __NetBSD__
  #include <ncurses/ncurses.h>
 #else
  #include <ncurses.h>
 #endif
#elif defined(SSC_OS_WINDOWS)
 #include <conio.h>
 #include <windows.h>
 #include "Error.h"
#else
 #error "Unsupported OS."
#endif

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

#if defined(SSC_OS_UNIXLIKE)
 /* On a Unixlike, do not inline term_init or term_end, since
  * doing so will mean needing to link directly with ncurses,
  * which is not necessary. */
 #define TERM_API_  SSC_API
 #define TERM_IMPL_ ;
#elif defined(SSC_OS_WINDOWS)
 /* On Windows, inline term_init and term_end, since they're
  * just there to clear the screen. */
 #define TERM_API_  SSC_INLINE
 #define TERM_IMPL_ { system("cls"); }
#else
 #error "Unsupported."
#endif

TERM_API_ void
SSC_Terminal_init(void)
TERM_IMPL_

TERM_API_ void
SSC_Terminal_end(void)
TERM_IMPL_

#undef TERM_API_
#undef TERM_IMPL_

/* From the command-line, prompt the user for "secret" input
 * characters of size @buffer_size, storing the characters
 * at @buffer. */
SSC_API int
SSC_Terminal_getSecretString(uint8_t* R_ buffer, const char* R_ prompt, const int buffer_size);
/* -> Size of the secret string. */

/* From the command-line, prompt the user to input a password with @prompt,
 * of size at least @min_pw_size and at most @max_pw_size in a buffer of size @buffer_size,
 * at the memory location @buffer.*/
SSC_API int
SSC_Terminal_getPassword(
 uint8_t*    R_ buffer,
 const char* R_ prompt,
 const int      min_pw_size,
 const int      max_pw_size,
 const int      buffer_size);
/* -> Size of the password. */

/* Obtains 2 passwords, the first stored at @password_buffer the next stored at @check_buffer.
 * Prompt the user to input the same password twice to ensure they entered it correctly.
 * @entry_prompt: String to prompt the user for input.
 * @reentry_prompt: String to prompt the user to re-enter the input.
 * @min_pw_size: The minimum size the password is allowed to be.*/
SSC_API int
SSC_Terminal_getPasswordChecked(
 uint8_t*    R_ password_buffer,
 uint8_t*    R_ check_buffer,
 const char* R_ entry_prompt,
 const char* R_ reentry_prompt,
 const int      min_pw_size,
 const int      max_pw_size,
 const int      buffer_size);
/* -> Size of the password. */

/* Notify the user with the string @notify_prompt. */
SSC_API void
SSC_Terminal_notify(const char* notify_prompt);

SSC_END_C_DECLS
#undef R_

#endif /* ~ SSC_TERMINAL_H */
