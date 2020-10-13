#ifndef SHIM_ARGS_H
#define SHIM_ARGS_H
#include <stdlib.h>
#include "macros.h"
/* Processing arguments... A series of C strings terminate by null. abc, def, ghi, NULL
 * 	How to handle some specific argument: a handler that can change the remaining arguments, and somehow change
 * 	the rest of the program.
 */

typedef void Shim_Arg_Handler_t (char **, void *);
typedef Shim_Arg_Handler_t * Shim_Arg_Parser_t (char *);

void SHIM_PUBLIC
shim_process_args (int argc, char ** argv,
		   Shim_Arg_Parser_t * const parser,
		   void * state_modifier);

#endif /* ~ SHIM_ARGS_H */
