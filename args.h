/* Copyright (c) 2020 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#ifndef SHIM_ARGS_H
#define SHIM_ARGS_H
#include "macros.h"
#include <stdbool.h>
#include <stdlib.h>
/* Processing arguments... A series of C strings terminate by null. abc, def, ghi, NULL
 * 	How to handle some specific argument: a handler that can change the remaining arguments, and somehow change
 * 	the rest of the program.
 */

/* Argument Handlers modify some state as per received arguments.
 * 	No return value. */
typedef void Shim_Arg_Handler_t (char **, int const, void * SHIM_RESTRICT);
/* Argument Parsers determine what argument handlers will be dispatched for some string. 
 * 	If there is no appropriate handler, return a NULL pointer. */
typedef Shim_Arg_Handler_t * Shim_Arg_Parser_t (char const *);
/* Argument Processors determine what Argument Parsers will be dispatched for some string.
 * 	If there is no valid parser, return a NULL pointer. */
typedef Shim_Arg_Parser_t * Shim_Arg_Processor_t (char const *, void * SHIM_RESTRICT);

enum {
	SHIM_ARGTYPE_NONE,
	SHIM_ARGTYPE_SHORT,
	SHIM_ARGTYPE_LONG
};

SHIM_BEGIN_DECLS
/* shim_argtype (argument string)
 * 	Return whether the argument qualifies as a short or long option, or as neither. */
SHIM_API int
shim_argtype (char const *);
/* shim_process_args (argument_count, arg_vector, processor_func_ptr, state_ptr)
 * Process arguments and classify them by their type (short option, long option, neither),
 * then dispatch the appropriate argument parsers.
 * 	No return value.
 */
SHIM_API void
shim_process_args (int const argc, char ** argv,
		   Shim_Arg_Processor_t * const processor,
		   void * SHIM_RESTRICT state);
SHIM_END_DECLS
		   
#endif /* ~ SHIM_ARGS_H */
