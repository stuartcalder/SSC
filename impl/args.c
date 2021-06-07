#include <stdbool.h>
#include "args.h"
#include "errors.h"

#define R_(ptr) ptr BASE_RESTRICT

int Base_argtype (const char* str) {
	BASE_ASSERT(str);
	int num_hyphens = 0;
	if (str[0] == '-')
		++num_hyphens;
	else
		goto SKIP_;
	if (str[1] == '-')
		++num_hyphens;
SKIP_:
	switch (num_hyphens) {
		case 1:
			return BASE_ARGTYPE_SHORT;
		case 2:
			return BASE_ARGTYPE_LONG;
	}
	return BASE_ARGTYPE_NONE;
}

void Base_process_args (const int argc, char** argv, Base_Arg_Processor_f* processor, R_(void*) state) {
	BASE_ASSERT(argv && processor && state);
	/* We start processing with argument index 1, skipping the name of the binary. */
	for (int i = 1; i < argc; ++i) {
		/* We get a pointer to the string @ (argv + i). */
		char** p = argv + i;
		/* If the string we're pointing at is NOT null */
		if (*p) {
			/* determine what kind of parser to use on the string. May modify state. */
			Base_Arg_Parser_f* parser = processor(*p, state);
			/* If the parser is NOT null */
			if (parser) {
				/* Determine how to handle the string, based on how we just parsed it. */
				Base_Arg_Handler_f* handler = parser(*p);
				/* If the handler is NOT null */
				if (handler)
					/* execute the handler function. May modify state. */
					handler(p, argc - i, state);
			}
		}
	}
}
