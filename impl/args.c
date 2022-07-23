/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include <string.h>
#include "args.h"
#include "errors.h"

#define R_(p) p BASE_RESTRICT

typedef Base_Arg_Short         Short_t;
typedef Base_Arg_Long          Long_t;
typedef Base_Arg_Parser        Parser_t;
typedef Base_Arg_Parser_Flag_t BAP_Flag_t;
#define EQ_ISVALID_ BASE_ARG_PARSER_FLAG_EQUALS_ISVALID

static int short_match_(const int shortc, R_(const Short_t*) shortv, char ch);
static int long_match_flag_(const BAP_Flag_t flag, const int longc, R_(const Long_t*) longv, size_t str_n, R_(char*) str);
BASE_INLINE int long_match_(const int longc, R_(const Long_t*) longv, size_t str_n, R_(char*) str) {
  return long_match_flag_(BASE_ARG_PARSER_FLAG_NONE, longc, longv, str_n, str);
}
static int long_match_(const int, R_(const Long_t*), size_t, R_(char*));
static int process_shorts_(const int, R_(char**), const int, R_(const Short_t*), R_(void*));
static int process_longs_flag_(
 const BAP_Flag_t flag,
 const int argc,
 R_(char**) argv,
 const int longc,
 R_(const Long_t*) longv,
 R_(void*) state
);
BASE_INLINE int process_longs_(
 const int argc,
 R_(char**) argv,
 const int longc,
 R_(const Long_t*) longv,
 R_(void*) state
)
{
  return process_longs_flag_(BASE_ARG_PARSER_FLAG_NONE, argc, argv, longc, longv, state);
}
Base_ArgType_t Base_argtype(const char* s) {
  int n_hyphens = 0;
  if (s[0] == '-')
    ++n_hyphens;
  else
    goto skip;
  if (s[1] == '-')
    ++n_hyphens;
skip:
  switch (n_hyphens) {
    case 1:  return BASE_ARGTYPE_SHORT;
    case 2:  return BASE_ARGTYPE_LONG;
    default: return BASE_ARGTYPE_NONE;
  }
}
enum { NOMATCH_ = -1 };
int short_match_(const int shortc, R_(const Short_t*) shortv, char ch) {
  for (int shorti = 0; shorti < shortc; ++shorti)
    if (shortv[shorti].ch == ch)
      return shorti;
  return NOMATCH_;
}
enum { EQ_NOT_FOUND_ = -1 };
static int eq_strlen_(const char* s) {
  int i = 0;
  while (1) {
    if (s[i] == '\0')
      return EQ_NOT_FOUND_;
    if (s[i] == '=')
      return i;
    ++i;
  }
}
/* Long matching relies upon all Base_Arg_Long structs
 * being in alphabetical order.
 */
int long_match_flag_(
 const BAP_Flag_t  flag,
 const int longc,
 R_(const Long_t*) longv,
 const size_t str_n,
 R_(char*) str
)
{
  for (int longi = 0; longi < longc; ++longi) {
    const int cmp_res = memcmp(str, longv[longi].str, longv[longi].str_n);
    if (cmp_res > 0)
      continue;
    else if (cmp_res == 0)
      return longi;
    else /* Implicitly: (cmp_res < 0) */
      Base_errx("Error: Invalid long option %s!\n", str);
  } /* ! for (int longi = 0; longi < longc; ++longi) */
  return NOMATCH_;
}
int process_shorts_(
 const int argc,
 R_(char**) argv,
 const int shortc,
 R_(const Short_t*) shortv,
 R_(void*) state
)
{
  if (argc == 0)
    return 0; /* Are there arguments? */
  const char* const s = argv[0];
  const int len = (int)strlen(s);
  if (len < 2)
    return 0; /* Long enough to process? */
  for (int i = 1; i < len; ++i) {
    int short_i;
    if ((short_i = short_match_(shortc, shortv, s[i])) == NOMATCH_)
      Base_errx("Error: Invalid short option: %c!\n", s[i]);
    const int result = (shortv[short_i].proc)(argc, argv, i + 1, state);
    if (result == BASE_ARG_PROC_ONECHAR)
      continue;
    else
      return result;
  }
  /* Every procedure only consumed one char each. */
  return 0;
}
int process_longs_flag_(
 const BAP_Flag_t  flag,
 const int         argc,
 R_(char**)        argv,
 const int         longc,
 R_(const Long_t*) longv,
 R_(void*)         state
)
{
  if (argc == 0)
    return 0; /* Stop if there are 0 args. */
  const int len = (int)strlen(argv[0]);
  if (len < 3)
    return 0;
  int long_i;
  if ((long_i = long_match_flag_(flag, longc, longv, len - 1, argv[0] + 2)) == NOMATCH_)
    Base_errx("Error: Invalid long option %s!\n", argv[0]);
  /* If '=' is invalid it signifies assignment. If we're doing assignment,
   * then we need to scan and find a '=', if any, and start reading from there
   * instead of the end of the word, to trigger reading the next word.
   */
  int start;
  if (!(flag & EQ_ISVALID_))
    if ((start = eq_strlen_(argv[0])) == EQ_NOT_FOUND_)
      start = len;
    else
      ++start;
  else
    start = len;
  return (longv[long_i].proc)(argc, argv, start, state);
}
void Base_process_args(
 const int argc,   R_(char**)         argv,
 const int shortc, R_(const Short_t*) shortv,
 const int longc,  R_(const Long_t*)  longv,
 R_(void*) state,  Base_Arg_Proc_f*   alone
)
{
  for (int arg_i = 0; arg_i < argc; ++arg_i) {
    const int            argc_left = argc - arg_i;
    char**               argv_left = argv + arg_i;
    /* Classify the string. */
    const Base_ArgType_t typ = Base_argtype(*argv_left);
    switch (typ) {
      case BASE_ARGTYPE_SHORT: {
        /* The string was a short option. Process all the short options of this word. */
        arg_i += process_shorts_(argc_left, argv_left, shortc, shortv, state);
      } break;
      case BASE_ARGTYPE_LONG: {
        /* The string was a long option. Process it, and any arguments if necessary. */
        arg_i += process_longs_(argc_left, argv_left, longc, longv, state);
      } break;
      case BASE_ARGTYPE_NONE: {
        /* The string did not appear to be an option. */
        if (alone)
	  arg_i += alone(argc_left, argv_left, 0, state);
	else
	  Base_errx("Error: Invalid argument: %s!\n", *argv_left);
      } break;
    } /* ! switch (typ) */
  } /* ! for (int arg_i = 0; arg_i < argc; ++arg_i) */
} /* ! Base_process_args */
void Base_Arg_Parser_init(
 R_(Parser_t*) ctx,
 R_(char*) start,
 const int argc,
 R_(char**) argv
)
{
  ctx->consumed = 0;
  if (*start != '\0') {
    ctx->to_read = start;
    ctx->size = strlen(ctx->to_read);
  } else {
    /* *start == '\0' */
    if (argc >= 2) {
      /* There is a next word. Assume this next word has the input we want. */
      ctx->to_read = argv[1];
      ctx->size = strlen(ctx->to_read);
      ctx->consumed = 1;
    } else
      ctx->to_read = BASE_NULL;
  }
}
