#ifndef BASE_ARGS_H
#define BASE_ARGS_H

#include <stdlib.h>
#include "macros.h"
#include "strings.h"
#define R_(p) p BASE_RESTRICT

enum {
	BASE_ARGTYPE_NONE,
	BASE_ARGTYPE_SHORT,
	BASE_ARGTYPE_LONG
};

/* @wordc: Number words: (--encrypt -iptext --output ctext).
 * @wordv: Word vector.
 * @offset: Offset into the first word to start reading.
 * @state:  Void pointer to data that will be modified by the procedure.
 * ->int(@x) Represents the number of words consumed.
 *            0 meaning "no additional" words,
 *           -1 meaning only 1 char was consumed.
 */
typedef int Base_Arg_Proc_f(const int wordc, R_(char**) wordv, const int offset, R_(void*) state);

enum {
	BASE_ARG_PROC_ONECHAR = -1
};

typedef struct {
	Base_Arg_Proc_f* proc;
	char             ch;
} Base_Arg_Short;
#define BASE_ARG_SHORT_NULL_LITERAL                  (Base_Arg_Short){0}
#define BASE_ARG_SHORT_LITERAL(procedure, character) (Base_Arg_Short){procedure, character}

typedef struct {
	Base_Arg_Proc_f* proc;
	const char*      str;
	size_t           str_n;
} Base_Arg_Long;
#define BASE_ARG_LONG_NULL_LITERAL               (Base_Arg_Long){0}
#define BASE_ARG_LONG_LITERAL(procedure, string) (Base_Arg_Long){procedure, string, (sizeof(string) - 1)}

BASE_BEGIN_DECLS
/* Get the argument type of the string. Short? Long? Neither? */
BASE_API int  Base_argtype(const char*);
/* @argc:  Number of words passed in from main().
 * @argv:  Argument vector.
 * @shortc: Number of short options.
 * @shortv: Short option vector.
 * @longc: Number of long options.
 * @longv: Long option vector.
 * @state: Data to be modified by a registered procedure.
 */
BASE_API void Base_process_args(const int argc, R_(char**)            argv,
				const int shortc, R_(Base_Arg_Short*) shortv,
				const int longc, R_(Base_Arg_Long*)   longv,
				R_(void*) state);
BASE_END_DECLS

#undef R_
#endif /* ! */
