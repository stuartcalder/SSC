#include "strings.h"
#include <ctype.h>

int SHIM_PUBLIC
shim_shift_left_digits (char * SHIM_RESTRICT str, int const size) {
	int index = 0;
	for( int i = 0; i < size; ++i )
		if( isdigit( (unsigned char)str[ i ] ) )
			str[ index++ ] = str[ i ];
	if( index <= size )
		str[ index ] = '\0';
	return index;
}
