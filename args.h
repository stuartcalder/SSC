#ifndef SHIM_ARGS_H
#define SHIM_ARGS_H
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"
/* Processing arguments... A series of C strings terminate by null. abc, def, ghi, NULL
 * 	How to handle some specific argument: a handler that can change the remaining arguments, and somehow change
 * 	the rest of the program.
 */

typedef void Shim_Arg_Handler_t (char **, int const, void * SHIM_RESTRICT);
typedef Shim_Arg_Handler_t * Shim_Arg_Parser_t (char const *);
typedef Shim_Arg_Parser_t * Shim_Arg_Processor_t (char const *, void * SHIM_RESTRICT);

enum {
	SHIM_ARGTYPE_NONE,
	SHIM_ARGTYPE_SHORT,
	SHIM_ARGTYPE_LONG
};

int SHIM_PUBLIC
shim_argtype (char const *);

void SHIM_PUBLIC
shim_process_args (int argc, char ** argv,
		   Shim_Arg_Processor_t * processor,
		   void * SHIM_RESTRICT state);
		   
#endif /* ~ SHIM_ARGS_H */
