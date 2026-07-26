/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_COMMANDLINEARG_H
#define SSC_COMMANDLINEARG_H

#include <stdlib.h>
#include "Macro.h"
#include "Error.h"

#define R_ SSC_RESTRICT
SSC_BEGIN_C_DECLS

/*##############################################################################*/
/* Are we processing a short or long argument?
 * Or neither? */
/*##############################################################################*/
typedef enum {
  SSC_ARGTYPE_NONE  = 0,/* Neither a short argtype nor a long argtype. */
  SSC_ARGTYPE_SHORT = 1,/* Short argtype i.e. -e -f -g, etc. */
  SSC_ARGTYPE_LONG  = 2 /* Long argtype i.e. --encrypt --force --guard, etc. */
} SSC_ArgType_t;
/*==============================================================================*/

/*##############################################################################*/
/* Return SSC_ARGPROC_ONECHAR when processing
 * short options, and we consume only 1 char
 * during processing. */
/*##############################################################################*/
enum {
  SSC_ARGPROC_ONECHAR = -1
};
/*==============================================================================*/

/*##############################################################################*/
/* @wordc: Number words: (--encrypt -iptext --output ctext).
 * @wordv: Word vector.
 * @offset: Offset into the first word to start reading.
 * @state:  Void pointer to data that will be modified by the procedure.
 * ->int(@x) Represents the number of words consumed.
 *            0, meaning "no additional" words (i.e. 1 word was consumed),
 *            SSC_ARGPROC_ONECHAR, meaning only 1 char of 1 word was consumed. */
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
#define SSC_ARGSHORT_NULL_LITERAL      SSC_STRUCT_LITERAL(SSC_ArgShort, 0)
#define SSC_ARGSHORT_LITERAL(Proc, Ch) SSC_STRUCT_LITERAL(SSC_ArgShort, Proc, Ch)
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
#define SSC_ARGLONG_NULL_LITERAL               SSC_STRUCT_LITERAL(SSC_ArgLong, 0)
#define SSC_ARGLONG_LITERAL(Proc, Str_Literal) SSC_STRUCT_LITERAL(SSC_ArgLong, Proc, Str_Literal, (sizeof(Str_Literal) - 1))
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
#define SSC_ARGPARSER_NULL_LITERAL SSC_STRUCT_LITERAL(SSC_ArgParser, 0)
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
 *     be found in SSC_ArgParser.to_read.
 *
 * Behavior:
 *   - Processes input from parser->to_read until exhausted or error occurs.
 *   - Parser tracks position via ->consumed (additional words consumed beyond first).
 *   - Returns SSC_ERROR if processing fails, otherwise returns 0 for success.
 *
 * Parameters:
 *   @parser: Pointer to the argument parser context containing input string.
 *   @data:    User-provided data pointer passed through to the processor function.
 *
 * Returns:
 *   - SSC_ERROR on failure (e.g., invalid parameter, malformed input).
 *   - 0 on success.
 *
 * Notes:
 *   - Processor functions must handle their own error reporting via SSC_errx().
 *   - Parser->to_read may be NULL if no valid input is available.
 *##############################################################################*/
typedef SSC_Error_t SSC_ArgProc_Processor_f(SSC_ArgParser* R_ parser, void* R_ data);

/*##############################################################################*/
/* Get the argument type of a command-line string: short (-x), long (--long-opt),
 * or neither (regular argument). */
/*##############################################################################*/
SSC_API SSC_ArgType_t
SSC_getArgType(const char* arg);

/*##############################################################################*/
/* SSC_processCommandLineArgs ()
 *     Parse and process all command-line arguments, handling short options (-x),
 *     long options (--long-opt), and standalone arguments. Typically called from main().
 *
 * Behavior:
 *   - Iterates through @argv[@arg_i] for arg_i in [0, argc).
 *   - Classifies each argument via SSC_getArgType() and routes to appropriate handler.
 *   - Short options are processed character-by-character; long options may consume
 *     additional arguments based on processor requirements.
 *   - Standalone arguments (no dash prefix) invoke @alone if provided, else error.
 *
 * Parameters:
 *   @argc:    Number of words passed to main().
 *   @argv:    Argument vector from main().
 *   @shortc:  Number of registered short options.
 *   @shortv:  Short option vector (array of SSC_ArgShort).
 *   @longc:   Number of registered long options.
 *   @longv:   Long option vector (array of SSC_ArgLong).
 *   @state:   User data pointer; modified by registered processors. Must outlive this call.
 *   @alone:   Pointer to function handling standalone arguments. If SSC_NULL, standalone args error.
 *
 * Notes:
 *   - Modifies @argv in-place as it consumes options and their arguments.
 *   - Uses @state for cross-option state sharing between processors.
 *##############################################################################*/
SSC_API void
SSC_processCommandLineArgs(
 const int              argc,   /* Number of words passed from main(). */
 char** R_              argv,   /* Argument vector; modified during processing. */
 const int              shortc, /* Count of registered short options. */
 const SSC_ArgShort* R_ shortv, /* Short option vector array. */
 const int              longc,  /* Count of registered long options. */
 const SSC_ArgLong* R_  longv,  /* Long option vector array. */
 void* R_               state,  /* User data; modified by processors. Must survive this call. */
 SSC_ArgProc_f*         alone); /* Handle standalone args. SSC_NULL = error on standalone arg. */

/*##############################################################################*/
/* Initialize the argument parser context @ctx for reading starting at @start.
 *
 * Behavior:
 *   - If *@start != '\0': Sets ctx->to_read to @start, size to strlen(@start).
 *   - If *@start == '\0' and argc >= 2: Reads next word argv[1].
 *   - Otherwise (argc < 2): Sets ctx->to_read to NULL.
 *
 * Parameters:
 *   @ctx:    Pointer to parser context to initialize.
 *   @start:  Pointer to C-string to read from, or '\0' sentinel for next word.
 *   @argc:   Total argument count (needed if @start == '\0').
 *   @argv:   Argument vector; used when reading next word.
 *
 * Notes:
 *   - After initialization, ctx->consumed indicates words consumed beyond first.
 *##############################################################################*/
SSC_API void SSC_ArgParser_init(
 SSC_ArgParser* R_ ctx,    /* Parser context to initialize. */
 char* R_          start,  /* Start string or '\0' sentinel for next word. */
 const int         argc,   /* Total arg count (used if start == '\0'). */
 char** R_         argv);  /* Argument vector (used if reading next word). */

/*##############################################################################*/
/* Process @argc C-strings from @argv[@offset] using the registered processor.
 *
 * Behavior:
 *   - Reinitializes parser context starting at argv[0] + offset.
 *   - Invokes @processor(ctx, data) and stores result in *processor_status if provided.
 *   - Returns ctx->consumed (words consumed beyond first).
 *
 * Parameters:
 *   @ctx:        Parser context to use for processing.
 *   @argc:       Number of C-strings to process from argv.
 *   @argv:       Argument vector; offset indicates starting position.
 *   @offset:     Offset into argv[0] where reading begins (for options like --input).
 *   @data:       User data pointer passed to processor function.
 *   @processor_status: Pointer to error status storage, if not NULL.
 *   @processor:    Function pointer to process the input string.
 *
 * Returns:
 *   - Number of additional words consumed beyond argv[0].
 *##############################################################################*/
SSC_API int SSC_ArgParser_process(
 SSC_ArgParser* R_        ctx,        /* Parser context. */
 const int                argc,       /* Number of C-strings in argv. */
 char** R_                argv,       /* Argument vector; offset into argv[0]. */
 const int                offset,     /* Offset in argv[0] to start reading. */
 void* R_                 data,       /* User data for processor function. */
 SSC_Error_t* R_          processor_status,  /* Output error status (optional). */
 SSC_ArgProc_Processor_f* processor); /* Function to process input string. */

/*##############################################################################*/
/* Helper: Returns SSC_ARGPROC_ONECHAR if @ch is non-zero, else 0. Used by short option
 * processors that consume exactly one character per invocation. */
/*##############################################################################*/
SSC_INLINE int
SSC_1opt(const char ch)
{
  return ch ? SSC_ARGPROC_ONECHAR : 0;
}


SSC_END_C_DECLS
#undef R_

#endif /* ! */
