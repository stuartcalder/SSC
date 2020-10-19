#include "args.h"
#include <stdbool.h>

enum Op_Type {
	Op_Type_FLOATING_,
	Op_Type_SHORT_,
	Op_Type_LONG_
};

static int
get_op_type_ (char const * str) {
	char const * p = str;
	int number_hyphens = 0;
	while( p ) {
		if( *p++ == '-' )
			++number_hyphens;
		else
			break;
	}
	switch( number_hyphens ) {
		case 1:
			return Op_Type_SHORT_;
		case 2:
			return Op_Type_LONG_;
		default:
			return Op_Type_FLOATING_;
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
		char ** ptr = argv + 1;
		if( *ptr ) {
			int const op_type = get_op_type_( *ptr );
			Shim_Arg_Handler_t * handler;
			switch( op_type ) {
				case Op_Type_SHORT_:
					handler = short_parser( *ptr );
					break;
				case Op_Type_LONG_:
					handler = long_parser( *ptr );
					break;
				case Op_Type_FLOATING_:
					handler = floating_parser( *ptr );
					break;
				default:
					handler = NULL;
					break;
			}
			if( handler ) {
				handler( ptr, state_modifier );
				*ptr = NULL;
			}
		}
	}
}
