#ifndef SHIM_ARGS_H
#define SHIM_ARGS_H
#include <stdlib.h>
#include "macros.h"
/* Processing arguments... A series of C strings terminate by null. abc, def, ghi, NULL
 * 	How to handle some specific argument: a handler that can change the remaining arguments, and somehow change
 * 	the rest of the program.
 */

typedef void Shim_Arg_Handler_t (char **, int const, void * SHIM_RESTRICT);
typedef Shim_Arg_Handler_t * Shim_Arg_Parser_t (char const *);

void SHIM_PUBLIC
shim_process_args (int argc, char ** argv,
		   Shim_Arg_Parser_t * const short_parser,
		   Shim_Arg_Parser_t * const long_parser,
		   Shim_Arg_Parser_t * const floating_parser,
		   void * SHIM_RESTRICT state_modifier);
#endif /* ~ SHIM_ARGS_H */
