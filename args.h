/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef BASE_ARGS_H
#define BASE_ARGS_H

#include <stdlib.h>
#include "macros.h"
#include "strings.h"

#define R_ BASE_RESTRICT
BASE_BEGIN_C_DECLS

/* Are we processing a short or long argument?
 * Or neither? */
enum {
  BASE_ARGTYPE_NONE  = 0,  /* Neither a short argtype nor a long argtype. */
  BASE_ARGTYPE_SHORT = 1, /* Short argtype i.e. -e -f -g, etc. */
  BASE_ARGTYPE_LONG  = 2 /*  Long argtype i.e. --encrypt --force --guard, etc. */
}; typedef int_fast8_t Base_ArgType_t;

/* Return BASE_ARG_PROC_ONECHAR when processing
 * short options, and we consume only 1 char
 * during processing. */
enum { BASE_ARG_PROC_ONECHAR = -1 };

/* @wordc: Number words: (--encrypt -iptext --output ctext).
 * @wordv: Word vector.
 * @offset: Offset into the first word to start reading.
 * @state:  Void pointer to data that will be modified by the procedure.
 * ->int(@x) Represents the number of words consumed.
 *            0, meaning "no additional" words (i.e. 1 word was consumed),
 *            BASE_ARG_PROC_ONECHAR, meaning only 1 char of 1 word was consumed. */
typedef int Base_Arg_Proc_f(const int wordc, char** R_ wordv, const int offset, void* R_ state);

/* Base_Arg_Short
 *   Associate a Base_Arg_Proc_f function pointer
 *   with a single character. */
typedef struct {
  Base_Arg_Proc_f* proc;
  char             ch;
} Base_Arg_Short;
#define BASE_ARG_SHORT_NULL_LITERAL      BASE_COMPOUND_LITERAL(Base_Arg_Short, 0)
#define BASE_ARG_SHORT_LITERAL(Proc, Ch) BASE_COMPOUND_LITERAL(Base_Arg_Short, Proc, Ch)

/* Base_Arg_Long
 *   Associate a Base_Arg_Proc_f function pointer
 *   with a null-terminated string.
 *   Store also the length of the string (null-terminator not included). */
typedef struct {
  Base_Arg_Proc_f* proc;
  const char*      str;
  size_t           str_n;
} Base_Arg_Long;
#define BASE_ARG_LONG_NULL_LITERAL               BASE_COMPOUND_LITERAL(Base_Arg_Long, 0)
/* Don't use BASE_ARG_LONG_LITERAL with any string
 * other than a string literal "like this", or the
 * use of sizeof is going to be a problem. */
#define BASE_ARG_LONG_LITERAL(Proc, Str_Literal) BASE_COMPOUND_LITERAL(Base_Arg_Long, Proc, Str_Literal, (sizeof(Str_Literal) - 1))

typedef struct {
  char*  to_read;
  size_t size;
  int    consumed;
} Base_Arg_Parser;
#define BASE_ARG_PARSER_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_Arg_Parser, 0)

enum {
  BASE_ARG_PARSER_FLAG_NONE =           0x00, /* Passing no flags. */
  BASE_ARG_PARSER_FLAG_EQUALS_ISVALID = 0x01  /* '=' is a valid character, not indicative of assignment. */
}; typedef int Base_Arg_Parser_Flag_t;

/* Get the argument type of the string. Short? Long? Neither? */
BASE_API Base_ArgType_t
Base_argtype(const char* arg);

BASE_API void
Base_process_args(
 const int                argc,   /* Number of words passed in from main(). */
 char** R_                argv,   /* Argument vector. */
 const int                shortc, /* Number of short options. */
 const Base_Arg_Short* R_ shortv, /* Short option vector. */
 const int                longc,  /* Number of long options. */
 const Base_Arg_Long* R_  longv,  /* Long option vector. */
 void* R_                 state,  /* Pointer to data, to be modified and read by registered procedures. */
 Base_Arg_Proc_f*         alone); /* Function pointer to handle "dashless" arguments. If BASE_NULL, we do not accept "dashless" arguments. */

/* Initialize the Base_Arg_Parser context @ctx.
 * Begin reading at @start. */
BASE_API void
Base_Arg_Parser_init(
 Base_Arg_Parser* R_ ctx,
 char* R_            start,
 const int           argc,
 char** R_           argv);

/* When @ch is NOT zero, return BASE_ARG_PROC_ONECHAR else return zero. */
BASE_INLINE int
Base_1opt(const char ch)
{
  return ch ? BASE_ARG_PROC_ONECHAR : 0;
}

BASE_END_C_DECLS
#undef R_

#endif /* ! */
