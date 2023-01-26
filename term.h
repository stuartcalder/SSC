/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_TERM_H
#define BASE_TERM_H

#include "macros.h"
#include "operations.h"

#if defined(BASE_OS_UNIXLIKE)
 #ifdef __NetBSD__
  #include <ncurses/ncurses.h>
 #else
  #include <ncurses.h>
 #endif
#elif defined(BASE_OS_WINDOWS)
 #include <conio.h>
 #include <windows.h>
 #include "errors.h"
#else
 #error "Unsupported OS."
#endif

#define R_(ptr) ptr BASE_RESTRICT
BASE_BEGIN_C_DECLS

#if defined(BASE_OS_UNIXLIKE)
/* On a Unixlike, do not inline term_init or term_end, since
 * doing so will mean needing to link directly with ncurses,
 * which is not necessary. */
 #define TERM_API_  BASE_API
 #define TERM_IMPL_ ;
#elif defined(BASE_OS_WINDOWS)
/* On Windows, inline term_init and term_end, since they're
 * just there to clear the screen. */
 #define TERM_API_  BASE_INLINE
 #define TERM_IMPL_ { system("cls"); }
#else
 #error "Unsupported."
#endif

TERM_API_ void Base_term_init(void) TERM_IMPL_
TERM_API_ void Base_term_end(void)  TERM_IMPL_

#undef TERM_API_
#undef TERM_IMPL_

BASE_API int Base_term_get_secret_string(
/* From the command-line, prompt the user for "secret" input
 * characters of size @buffer_size, storing the characters
 * at @buffer. */
 R_(uint8_t*) buffer, R_(const char*) prompt, const int buffer_size);

BASE_API int Base_term_obtain_password(
/* From the command-line, prompt the user to input a password with @prompt,
 * of size at least @min_pw_size and at most @max_pw_size in a buffer of size @buffer_size,
 * at the memory location @buffer.*/
 R_(uint8_t*) buffer, R_(const char*) prompt, const int min_pw_size, const int max_pw_size, const int buffer_size);

BASE_API int Base_term_obtain_password_checked(
/* Obtains 2 passwords, the first stored at @password_buffer the next stored at @check_buffer.
 * Prompt the user to input the same password twice to ensure they entered it correctly.
 * @entry_prompt: String to prompt the user for input.
 * @reentry_prompt: String to prompt the user to re-enter the input.
 * @min_pw_size: The minimum size the password is allowed to be.*/
 R_(uint8_t*) password_buffer, R_(uint8_t*) check_buffer, R_(const char*) entry_prompt,
 R_(const char*) reentry_prompt, const int min_pw_size, const int max_pw_size, const int buffer_size);

BASE_API void Base_term_notify(
/* Notify the user with the string @notify_prompt. */
 const char* notify_prompt);

BASE_END_C_DECLS
#undef R_

#endif /* ~ BASE_TERM_H */
