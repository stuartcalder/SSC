#include "args.h"
#include <stdbool.h>

enum {
	OP_TYPE_FLOATING_,
	OP_TYPE_SHORT_,		/* i.e. -h */
	OP_TYPE_LONG_		/* i.e. --help */
};

static int
get_op_type_ (char const * str) {
	char const * p = str;
	int number_hyphens = 0;
	while( *p++ == '-' )
		++number_hyphens;
	switch( number_hyphens ) {
		case 1:
			return OP_TYPE_SHORT_;
		case 2:
			return OP_TYPE_LONG_;
		default:
			return OP_TYPE_FLOATING_;
	}
}

void SHIM_PUBLIC
shim_process_args (int argc, char ** argv,
		   Shim_Arg_Parser_t * const short_parser,
		   Shim_Arg_Parser_t * const long_parser,
		   Shim_Arg_Parser_t * const floating_parser,
		   void * SHIM_RESTRICT state_modifier)
{
	for( int i = 1; i < argc; ++i ) {
		char ** ptr = argv + i;
		if( *ptr ) {
			int const op_type = get_op_type_( *ptr );
			Shim_Arg_Handler_t * handler;
			switch( op_type ) {
				case OP_TYPE_SHORT_:
					handler = short_parser( *ptr );
					break;
				case OP_TYPE_LONG_:
					handler = long_parser( *ptr );
					break;
				case OP_TYPE_FLOATING_:
				default:
					handler = floating_parser( *ptr );
					break;
			}
			if( handler )
				handler( ptr, argc - i, state_modifier );
			*ptr = NULL;
		}
	}
}
