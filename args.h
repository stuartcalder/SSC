/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef BASE_ARGS_H
#define BASE_ARGS_H

#include <stdbool.h>
#include <stdlib.h>
#include "macros.h"
#define R_(ptr) ptr BASE_RESTRICT
/* Processing arguments... A series of C strings terminate by null. abc, def, ghi, NULL
 * 	How to handle some specific argument: a handler that can change the remaining arguments, and somehow change
 * 	the rest of the program.
 */

/* Argument Handlers modify some state as per received arguments.
 * 	No return value. */
typedef void Base_Arg_Handler_f (char**, const int, R_(void*));
/* Argument Parsers determine what argument handlers will be dispatched for some string. 
 * 	If there is no appropriate handler, return a NULL pointer. */
typedef Base_Arg_Handler_f* Base_Arg_Parser_f (const char*);
/* Argument Processors determine what Argument Parsers will be dispatched for some string.
 * 	If there is no valid parser, return a NULL pointer. */
typedef Base_Arg_Parser_f* Base_Arg_Processor_f (const char*, R_(void *));

enum {
	BASE_ARGTYPE_NONE,
	BASE_ARGTYPE_SHORT,
	BASE_ARGTYPE_LONG
};

BASE_BEGIN_DECLS
/* Base_argtype (argument string)
 * 	Return whether the argument qualifies as a short or long option, or as neither. */
BASE_API int Base_argtype (const char*);
/* Base_process_args (argument_count, arg_vector, processor_func_ptr, state_ptr)
 * Process arguments and classify them by their type (short option, long option, neither),
 * then dispatch the appropriate argument parsers.
 * 	No return value.
 */
BASE_API void Base_process_args (const int argc, char** argv, Base_Arg_Processor_f* processor, R_(void*) state);
BASE_END_DECLS
		   
#undef R_
#endif /* ~ BASE_ARGS_H */
