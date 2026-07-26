/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "Macro.h"
#if !defined(SSC_TERMINAL_H) && !defined(__ANDROID__)
#define SSC_TERMINAL_H

#include "Operation.h"

#if defined(SSC_OS_UNIXLIKE)
  #ifdef __has_include
    #if __has_include(<ncurses.h>)
      #include <ncurses.h>
    #elif __has_include(<ncurses/ncurses.h>)
      #include <ncurses/ncurses.h>
    #else
      #error "Unable to find an ncurses.h file to include!"
    #endif
  #else
    #ifdef __NetBSD__
      #include <ncurses/ncurses.h>
    #else
      #include <ncurses.h>
    #endif
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
 /* On a Unixlike, do not inline SSC_Terminal_init or SSC_Terminal_end, since
  * doing so will mean needing to link directly with ncurses,
  * which is not necessary. */
 #define SSC_TERMINAL_INIT_IMPL { initscr(); clear(); }
 #define SSC_TERMINAL_END_IMPL  { endwin(); }
 #define TERM_API_  SSC_API
 #define TERM_IMPL_ ;
#elif defined(SSC_OS_WINDOWS)
 #define SSC_TERMINAL_INIT_INLINE
 #define SSC_TERMINAL_END_INLINE
 /* On Windows, inline SSC_Terminal_init and SSC_Terminal_end, since they're
  * just there to clear the screen. */
 #define TERM_API_  SSC_INLINE
 #define TERM_IMPL_ { system("cls"); }
#else
 #error "Unsupported."
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Initialize the terminal for secure input operations. */
/* On Unixlike systems, this enables ncurses mode (cbreak/noecho). */
/* On Windows, it clears the screen using 'cls'. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
TERM_API_ void SSC_Terminal_init(void) TERM_IMPL_

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Terminate terminal initialization and restore normal mode. */
/* On Unixlike systems, this disables ncurses mode (endwin). */
/* On Windows, it clears the screen using 'cls'. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
TERM_API_ void SSC_Terminal_end(void)  TERM_IMPL_

#undef TERM_API_
#undef TERM_IMPL_

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Prompt the user for "secret" input (e.g., password) without echoing characters.
 * The input is stored at @buffer and displayed as '*' on Unixlike systems, or '*'
 * per character on Windows.
 *
 * Parameters:
 *   @buffer:    Pointer to a buffer of size @buffer_size where the secret string will be stored.
 *               Must have at least 2 bytes available (min buffer size enforced).
 *   @prompt:    A null-terminated prompt string displayed before input.
 *   @buffer_size: The total size of the buffer in bytes.
 *
 * Behavior:
 *   - Unixlike: Uses ncurses to create an off-screen window for secure input, displaying '*'
 *               characters and allowing backspace/deletion keys. The terminal is restored upon exit.
 *   - Windows: Uses _getch() with conio.h, echoing '*' per character, clearing screen on each attempt.
 *
 * Returns:
 *   - Size of the secret string entered (0 if empty).
 *   - Caller must null-terminate the buffer after use.
 *
 * Notes:
 *   - The function loops until valid input is received or Enter/Return key is pressed.
 *   - Buffer size constraint: minimum 2 bytes required to handle edge cases.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API int SSC_Terminal_getSecretString(uint8_t* R_ buffer, const char* R_ prompt, const int buffer_size);
/* -> Size of the secret string (0 if empty). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Prompt the user to input a password with validation for minimum and maximum length.
 * The function loops until a valid password is entered (within size constraints).
 *
 * Parameters:
 *   @buffer:         Pointer to a buffer of size @buffer_size where the password will be stored.
 *                    Must have at least (max_pw_size + 1) bytes available.
 *   @prompt:         A null-terminated prompt string displayed before input.
 *   @min_pw_size:    The minimum acceptable password length. Passwords shorter than this are rejected.
 *   @max_pw_size:    The maximum acceptable password length. Passwords longer than this are rejected.
 *   @buffer_size:    The total size of the buffer in bytes (must be >= max_pw_size + 1).
 *
 * Behavior:
 *   - Uses SSC_Terminal_getSecretString() internally to capture input without echo.
 *   - If password length is outside [min_pw_size, max_pw_size], displays "Password is not long enough" or
 *     "password is too long" and prompts again.
 *
 * Returns:
 *   - Size of the valid password entered (between min_pw_size and max_pw_size inclusive).
 *
 * Notes:
 *   - The caller must null-terminate the buffer after use.
 *   - Loop continues until a valid password is provided.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API int SSC_Terminal_getPassword(
  uint8_t*    R_ buffer,
  const char* R_ prompt,
  const int      min_pw_size,
  const int      max_pw_size,
  const int      buffer_size);
/* -> Size of the password (validated to be within [min_pw_size, max_pw_size]). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Obtain a validated password by requiring the user to re-enter it. The function loops until:
 *   1) Both passwords have length within [min_pw_size, max_pw_size]
 *   2) Both passwords are of equal length
 *   3) Passwords match (using constant-time memory comparison for side-channel resistance)
 *
 * Parameters:
 *   @password_buffer: Pointer to first buffer where the initial password is stored.
 *                     Must have at least (max_pw_size + 1) bytes available.
 *   @check_buffer:    Pointer to second buffer where the re-entry password is stored.
 *                     Must have at least (max_pw_size + 1) bytes available.
 *   @entry_prompt:    A null-terminated prompt string displayed before initial input.
 *   @reentry_prompt:  A null-terminated prompt string displayed for re-entry.
 *   @min_pw_size:     The minimum acceptable password length. Passwords shorter than this are rejected.
 *   @max_pw_size:     The maximum acceptable password length. Passwords longer than this are rejected.
 *   @buffer_size:     The total size of each buffer in bytes (must be >= max_pw_size + 1).
 *
 * Behavior:
 *   - Prompts user for initial password, storing at @password_buffer.
 *   - If length is invalid (< min_pw_size or > max_pw_size), notifies and re-prompts.
 *   - Prompts user to re-enter the password, storing at @check_buffer.
 *   - If lengths differ, notifies "Second password is not the same size" and re-prompts.
 *   - Uses SSC_constTimeMemDiff() to compare passwords (constant-time comparison prevents timing attacks).
 *   - If mismatch detected, notifies "Passwords do not match" and loops again.
 *
 * Returns:
 *   - Size of the validated password (between min_pw_size and max_pw_size inclusive).
 *
 * Notes:
 *   - Both buffers must be at least (max_pw_size + 1) bytes to handle edge cases.
 *   - The caller must null-terminate both buffers after use.
 *   - Constant-time memory comparison prevents side-channel attacks on password length/contents.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API int SSC_Terminal_getPasswordChecked(
  uint8_t*    R_ password_buffer,
  uint8_t*    R_ check_buffer,
  const char* R_ entry_prompt,
  const char* R_ reentry_prompt,
  const int      min_pw_size,
  const int      max_pw_size,
  const int      buffer_size);
/* -> Size of the validated password (between min_pw_size and max_pw_size inclusive). */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Display a notification message to the user and wait for acknowledgment before returning.
 *
 * Parameters:
 *   @notify_prompt: A null-terminated string displayed as a system notification.
 *
 * Behavior:
 *   - Unixlike: Creates an off-screen ncurses window displaying the prompt, then waits for any keypress
 *               (using wgetch) before returning and cleaning up.
 *   - Windows: Clears screen, displays the message using _cputs(), shows "Press any key to continue",
 *              clears screen again, then returns.
 *
 * Notes:
 *   - Use this function for non-blocking user feedback during password prompts or other operations.
 *   - The caller must ensure @notify_prompt is null-terminated and reasonably sized (< 256 chars recommended).
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API void SSC_Terminal_notify(const char* notify_prompt);

SSC_END_C_DECLS
#undef R_

#endif /* ~ SSC_TERMINAL_H */
