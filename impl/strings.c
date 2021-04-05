#include "strings.h"
#include <ctype.h>

/* @str:	A null-terminated string.
 * @size:	The number of non-null characters in the string @str.
 * returns:	The number of digit characters shifted to the left. */
int
shim_shift_left_digits (char * SHIM_RESTRICT str, int const size) {
	int index = 0;
	for( int i = 0; i < size; ++i )
		if( isdigit( (unsigned char)str[ i ] ) )
			str[ index++ ] = str[ i ];
	if( index <= size )
		str[ index ] = '\0';
	return index;
}
