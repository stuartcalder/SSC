#include "args.h"
#include <stdbool.h>

int
shim_argtype (char const * str) {
	int num_hyphens = 0;
	for( int i = 0; i < 2; ++i ) {
		if( str[ i ] == '-' )
			++num_hyphens;
		else
			break;
	}
	switch( num_hyphens ) {
		case 1:
			return SHIM_ARGTYPE_SHORT;
		case 2:
			return SHIM_ARGTYPE_LONG;
		default:
			return SHIM_ARGTYPE_NONE;
	}
}

void
shim_process_args (int const argc, char ** argv,
		   Shim_Arg_Processor_t * const processor,
		   void * SHIM_RESTRICT state)
{
	/* We start processing with argument index 1, skipping the name of the binary. */
	for( int i = 1; i < argc; ++i ) {
		/* We get a pointer to the string @ (argv + i). */
		char ** p = argv + i;
		/* If the string we're pointing at is NOT null */
		if( *p ) {
			/* determine what kind of parser to use on the string. May modify state. */
			Shim_Arg_Parser_t * parser = processor( *p, state );
			/* If the parser is NOT null */
			if( parser ) {
				/* Determine how to handle the string, based on how we just parsed it. */
				Shim_Arg_Handler_t * handler = parser( *p );
				/* If the handler is NOT null */
				if( handler )
					/* execute the handler function. May modify state. */
					handler( p, argc - i, state );
			}
		}
	}
}
