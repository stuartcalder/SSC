#ifndef BASE_ARGS_H
#define BASE_ARGS_H

#include <stdlib.h>
#include "macros.h"
#include "strings.h"
#define R_(p) p BASE_RESTRICT
BASE_BEGIN_C_DECLS

enum {
  BASE_ARGTYPE_NONE  = 0,
  BASE_ARGTYPE_SHORT = 1,
  BASE_ARGTYPE_LONG  = 2
};
typedef int_fast8_t Base_ArgType_t;

/* @wordc: Number words: (--encrypt -iptext --output ctext).
 * @wordv: Word vector.
 * @offset: Offset into the first word to start reading.
 * @state:  Void pointer to data that will be modified by the procedure.
 * ->int(@x) Represents the number of words consumed.
 *            0 meaning "no additional" words,
 *           -1 meaning only 1 char was consumed.
 */
typedef int Base_Arg_Proc_f(const int wordc, R_(char**) wordv, const int offset, R_(void*) state);

/* Return BASE_ARG_PROC_ONECHAR when processing
 * short options, and we consume only 1 char
 * during processing.
 */
enum { BASE_ARG_PROC_ONECHAR = -1 };

/* Base_Arg_Short
 *   Associate a Base_Arg_Proc_f function pointer
 *   with a single character.
 */
typedef struct {
	Base_Arg_Proc_f* proc;
	char             ch;
} Base_Arg_Short;
#define BASE_ARG_SHORT_NULL_LITERAL      BASE_COMPOUND_LITERAL(Base_Arg_Short, 0)
#define BASE_ARG_SHORT_LITERAL(proc, ch) BASE_COMPOUND_LITERAL(Base_Arg_Short, proc, ch)

/* Base_Arg_Long
 *   Associate a Base_Arg_Proc_f function pointer
 *   with a null-terminated string.
 *   Store also the length of the string (null-terminator not included).
 */
typedef struct {
	Base_Arg_Proc_f* proc;
	const char*      str;
	size_t           str_n;
} Base_Arg_Long;
#define BASE_ARG_LONG_NULL_LITERAL           BASE_COMPOUND_LITERAL(Base_Arg_Long, 0)
#define BASE_ARG_LONG_LITERAL(proc, str_lit) BASE_COMPOUND_LITERAL(Base_Arg_Long, proc, str_lit, (sizeof(str_lit) - 1))
/* Don't use BASE_ARG_LONG_LITERAL with any string other than a string literal "like this". */

typedef struct {
	char*  to_read;
	size_t size;
	int    consumed;
} Base_Arg_Parser;
#define BASE_ARG_PARSER_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_Arg_Parser, 0)

#define BASE_ARG_PARSER_FLAG_NONE           0x00 /* Passing no flags. */
#define BASE_ARG_PARSER_FLAG_EQUALS_ISVALID 0x01 /* '=' is a valid character, not indicative of assignment. */
typedef int Base_Arg_Parser_Flag_t;

/* Get the argument type of the string. Short? Long? Neither? */
BASE_API Base_ArgType_t
Base_argtype
(const char*);

/* @argc:  Number of words passed in from main().
 * @argv:  Argument vector.
 * @shortc: Number of short options.
 * @shortv: Short option vector.
 * @longc: Number of long options.
 * @longv: Long option vector.
 * @state: Data to be modified by a registered procedure.
 * @alone: Proc function pointer to handle "dashless" args. If NULL, we do not accept "dashless" args.
 */
BASE_API void
Base_process_args
(const int argc,   R_(char**)                argv,
 const int shortc, R_(const Base_Arg_Short*) shortv,
 const int longc,  R_(const Base_Arg_Long*)  longv,
 R_(void*) state,  Base_Arg_Proc_f*          alone);

BASE_API void
Base_Arg_Parser_init
(R_(Base_Arg_Parser*) ctx,
 R_(char*)            start,
 const int            argc,
 R_(char**)           argv);

BASE_INLINE int
Base_1opt
(const char ch)
{ return ch ? BASE_ARG_PROC_ONECHAR : 0; }

BASE_END_C_DECLS

#undef R_
#endif /* ! */
