/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_COMMANDLINEARG_H
#define SSC_COMMANDLINEARG_H

#include <stdlib.h>
#include "Macro.h"
#include "String.h"

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/*##############################################################################*/
/* Are we processing a short or long argument?
 * Or neither? */
/*##############################################################################*/
enum {
  SSC_ARGTYPE_NONE  = 0,/* Neither a short argtype nor a long argtype. */
  SSC_ARGTYPE_SHORT = 1,/* Short argtype i.e. -e -f -g, etc. */
  SSC_ARGTYPE_LONG  = 2 /* Long argtype i.e. --encrypt --force --guard, etc. */
};
typedef int SSC_ArgType_t;
/*==============================================================================*/

/*##############################################################################*/
/* Return SSC_ARG_PROC_ONECHAR when processing
 * short options, and we consume only 1 char
 * during processing. */
/*##############################################################################*/
enum {
  SSC_ARG_PROC_ONECHAR = -1
};
/*==============================================================================*/

/*##############################################################################*/
/* @wordc: Number words: (--encrypt -iptext --output ctext).
 * @wordv: Word vector.
 * @offset: Offset into the first word to start reading.
 * @state:  Void pointer to data that will be modified by the procedure.
 * ->int(@x) Represents the number of words consumed.
 *            0, meaning "no additional" words (i.e. 1 word was consumed),
 *            SSC_ARG_PROC_ONECHAR, meaning only 1 char of 1 word was consumed. */
/*##############################################################################*/
typedef int SSC_ArgProc_f(const int wordc, char** R_ wordv, const int offset, void* R_ state);
/*==============================================================================*/

/*##############################################################################*/
/* SSC_ArgShort
 *   Associate a SSC_ArgProc_f function pointer
 *   with a single character. */
/*##############################################################################*/
typedef struct {
  SSC_ArgProc_f* proc;
  char           ch;
} SSC_ArgShort;
#define SSC_ARGSHORT_NULL_LITERAL      SSC_COMPOUND_LITERAL(SSC_ArgShort, 0)
#define SSC_ARGSHORT_LITERAL(Proc, Ch) SSC_COMPOUND_LITERAL(SSC_ArgShort, Proc, Ch)
/*==============================================================================*/

/*##############################################################################*/
/* SSC_ArgLong
 *   Associate a SSC_ArgProc_f function pointer
 *   with a null-terminated string.
 *   Store also the length of the string (null-terminator not included). */
/*##############################################################################*/
typedef struct {
  SSC_ArgProc_f* proc;
  const char*    str;
  size_t         str_n;
} SSC_ArgLong;
/* Don't use SSC_ARGLONG_LITERAL with any string
 * other than a string literal "like this", or the
 * use of sizeof is going to be a problem. */
#define SSC_ARGLONG_NULL_LITERAL               SSC_COMPOUND_LITERAL(SSC_ArgLong, 0)
#define SSC_ARGLONG_LITERAL(Proc, Str_Literal) SSC_COMPOUND_LITERAL(SSC_ArgLong, Proc, Str_Literal, (sizeof(Str_Literal) - 1))
/*==============================================================================*/

/*##############################################################################*/
/* SSC_ArgParser {}
 *     This struct is intended to help parse through command line argument strings.
 *     When initialized @to_read either points to a C-string to read input from or
 *     assumes a value of NULL when invalid. @size refers to the number of non-null
 *     bytes of @to_read. When more than one C-string gets processed at once, @consumed
 *     equals the number of additional C-strings consumed beyond the first. */
/*##############################################################################*/
typedef struct {
  char*  to_read;
  size_t size;
  int    consumed;
} SSC_ArgParser;
#define SSC_ARGPARSER_NULL_LITERAL SSC_COMPOUND_LITERAL(SSC_ArgParser, 0)
/*==============================================================================*/

enum {
  SSC_ARGPARSER_FLAG_EQUALS_ISVALID = 0x01  /* '=' is a valid character, not indicative of assignment. */
};
typedef int SSC_ArgParserFlag_t;

/*##############################################################################*/
/* SSC_ArgProc_Processor_f ()*
 *     Pointers to functions of this signature are intended to be passed to
 *     SSC_ArgParser_process(), where they implement the logic of processing
 *     command line strings that supply parameters should valid parameters to
 *     be found in SSC_ArgParser.to_read */
/*##############################################################################*/
typedef SSC_Error_t SSC_ArgProc_Processor_f(SSC_ArgParser* R_ parser, void* R_ data);
/*==============================================================================*/

/*##############################################################################*/
/* Get the argument type of the string. Short? Long? Neither? */
/*##############################################################################*/
SSC_API SSC_ArgType_t
SSC_getArgType(const char* arg);
/*==============================================================================*/

/*##############################################################################*/
/* SSC_processCommandLineArgs ()
 *     This procedure is typically called from main().
 *     Pass the traditional main @argc and @argv, as well as the
 *     short and long option vectors, @state pointer, as well as an SSC_ArgProc_f
 *     for handling long arguments, i.e. arguments not preceded by dashed words like
 *     -i or --input. */
/*##############################################################################*/
SSC_API void
SSC_processCommandLineArgs(
 const int              argc,   /* Number of words passed in from main(). */
 char** R_              argv,   /* Argument vector. */
 const int              shortc, /* Number of short options. */
 const SSC_ArgShort* R_ shortv, /* Short option vector. */
 const int              longc,  /* Number of long options. */
 const SSC_ArgLong* R_  longv,  /* Long option vector. */
 void* R_               state,  /* Pointer to data, to be modified and read by registered procedures. */
 SSC_ArgProc_f*         alone); /* Function pointer to handle "dashless" arguments. If SSC_NULL, we do not accept "dashless" arguments. */
/*==============================================================================*/

/*##############################################################################*/
/* Initialize the SSC_ArgParser context @ctx.
 * Begin reading at @start. */
/*##############################################################################*/
SSC_API void SSC_ArgParser_init(
 SSC_ArgParser* R_ ctx,
 char* R_          start,
 const int         argc,
 char** R_         argv);
/*==============================================================================*/

/*##############################################################################*/
/* SSC_ArgParser_process ()
 *     Process the @argc C-strings pointed to by @argv.
 *     Begin processing at @argv[@offset].
 *     Pass @ctx and @data to the supplied SSC_ArgProc_Processor_f
 *     function pointer and store the result in @processor_status, if that
 *     pointer is not NULL. */
/*##############################################################################*/
SSC_API int SSC_ArgParser_process(
 SSC_ArgParser* R_        ctx,
 const int                argc,
 char** R_                argv,
 const int                offset,
 void* R_                 data,
 SSC_Error_t* R_          processor_status,
 SSC_ArgProc_Processor_f* processor);
/*==============================================================================*/

/*##############################################################################*/
/* When @ch is NOT zero, return SSC_ARG_PROC_ONECHAR else return zero. */
/*##############################################################################*/
SSC_INLINE int
SSC_1opt(const char ch)
{
  return ch ? SSC_ARG_PROC_ONECHAR : 0;
}
/*==============================================================================*/

SSC_END_C_DECLS
#undef R_

#endif /* ! */
