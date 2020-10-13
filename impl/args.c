#include "args.h"

void SHIM_PUBLIC
shim_process_args (int argc, char ** argv,
		   Shim_Arg_Parser_t * const parser,
		   void * state_modifier)
{
	for( int i = 1; i < argc; ++i ) {
		char ** ptr = argv + i;
		if( ptr ) {
			/* How do we handle the current argument string? */
			Shim_Arg_Handler_t * handler = parser( *ptr );
			/* If there is a valid way to handle it, then handle it. */
			if( handler )
				handler( ptr, state_modifier );
		}
	}
}
