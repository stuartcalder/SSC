/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include <string.h>
#include "CommandLineArg.h"
#include "Error.h"

#define R_ SSC_RESTRICT

typedef SSC_ArgShort        Short_t;
typedef SSC_ArgLong         Long_t;
typedef SSC_ArgParser       Parser_t;
typedef SSC_ArgParserFlag_t Flag_t;
#define EQ_ISVALID_ SSC_ARGPARSER_FLAG_EQUALS_ISVALID

enum {
  NOMATCH_ = -1,     /* For all the matching functions, return NOMATCH_ when no match is found. */
};

/*##############################################################################*/
/* TODO: Document. */
/*##############################################################################*/
static int
shortMatch_(const int shortc, const Short_t* R_ shortv, char ch);
/*==============================================================================*/

/*##############################################################################*/
/* TODO: Document. */
/*##############################################################################*/
static int
longMatchFlag_(
 const Flag_t     flag,
 const int        longc,
 const Long_t* R_ longv,
 size_t           str_n,
 char* R_         str);

SSC_INLINE int
longMatch_(
 const int        longc, /* Number of longs to process. */
 const Long_t* R_ longv, /* Long vector. */
 size_t           str_n, /* Length of the string to check. */
 char* R_         str)   /* The string to check. */
{
  return longMatchFlag_(0, longc, longv, str_n, str);
}
/*==============================================================================*/

/*##############################################################################*/
/* TODO: Document. */
/*##############################################################################*/
static int
longMatch_(const int, const Long_t* R_, size_t, char* R_);
/*==============================================================================*/

/*##############################################################################*/
/* TODO: Document. */
/*##############################################################################*/
static int
processShorts_(const int, char** R_, const int, const Short_t* R_, void* R_);
/*==============================================================================*/

/*##############################################################################*/
/* TODO: Document. */
/*##############################################################################*/
static int
processLongsFlag_(
 const Flag_t     flag,
 const int        argc,
 char** R_        argv,
 const int        longc,
 const Long_t* R_ longv,
 void* R_         state);

SSC_INLINE int
processLongs_(
 const int        argc,
 char** R_        argv,
 const int        longc,
 const Long_t* R_ longv,
 void* R_         state)
{
  return processLongsFlag_(0, argc, argv, longc, longv, state);
}
/*==============================================================================*/

/*##############################################################################*/
/* The same a strlen(), but '=' also terminates as well as NULL does.
 * Return EQ_NOT_FOUND_ when '=' is not detected.*/
/*##############################################################################*/
static int
eq_strlen_(const char* s);
enum {
  EQ_NOT_FOUND_ = -1
};
/*==============================================================================*/

SSC_ArgType_t
SSC_getArgType(const char* s)
{
  int n_hyphens = 0;
  if (s[0] == '-')
    ++n_hyphens;
  else
    goto skip;
  if (s[1] == '-')
    ++n_hyphens;
  skip:
  switch (n_hyphens) {
    case 1:  return SSC_ARGTYPE_SHORT;
    case 2:  return SSC_ARGTYPE_LONG;
    default: return SSC_ARGTYPE_NONE;
  }
}


int
shortMatch_(
 const int         shortc,
 const Short_t* R_ shortv,
 char              ch)
{
  for (int shorti = 0; shorti < shortc; ++shorti)
    if (shortv[shorti].ch == ch)
      return shorti;
  return NOMATCH_;
}

int
eq_strlen_(const char* s)
{
  int i = 0;
  while (1) {
    switch (s[i]) {
      case '\0':
        return EQ_NOT_FOUND_;
      case '=':
        return i;
      default:
        ++i;
    }
  }
}

/* Long matching relies upon all SSC_ArgLong structs
 * being in alphanumeric order. */
int
longMatchFlag_(
 const Flag_t     flag,
 const int        longc,
 const Long_t* R_ longv,
 const size_t     str_n,
 char* R_         str)
{
  for (int longi = 0; longi < longc; ++longi) {
    const int cmp_res = memcmp(str, longv[longi].str, longv[longi].str_n);
    if (cmp_res > 0)
      continue;
    else if (cmp_res == 0)
      return longi;
    else /* Implicitly: (cmp_res < 0) */
      SSC_errx("Error: Invalid long option %s!\n", str);
  } /* ! for (int longi = 0; longi < longc; ++longi) */
  return NOMATCH_;
}

int
processShorts_(
 const int         argc,
 char** R_         argv,
 const int         shortc,
 const Short_t* R_ shortv,
 void* R_          state)
{
  if (argc == 0)
    return 0; /* Are there arguments? */
  const char* const s = argv[0];
  const int len = (int)strlen(s);
  if (len < 2)
    return 0; /* Long enough to process? */
  for (int i = 1; i < len; ++i) {
    int short_i;
    if ((short_i = shortMatch_(shortc, shortv, s[i])) == NOMATCH_)
      SSC_errx("Error: Invalid short option: %c!\n", s[i]);
    const int result = (shortv[short_i].proc)(argc, argv, i + 1, state);
    if (result == SSC_ARG_PROC_ONECHAR)
      continue;
    else
      return result;
  }
  /* Every procedure only consumed one char each. */
  return 0;
}

int
processLongsFlag_(
 const Flag_t     flag,
 const int        argc,
 char** R_        argv,
 const int        longc,
 const Long_t* R_ longv,
 void* R_         state)
{
  if (argc == 0)
    return 0; /* Stop if there are 0 args. */
  const int len = (int)strlen(argv[0]);
  if (len < 3)
    return 0;
  int long_i;
  if ((long_i = longMatchFlag_(flag, longc, longv, len - 1, argv[0] + 2)) == NOMATCH_)
    SSC_errx("Error: Invalid long option %s!\n", argv[0]);
  /* If '=' is invalid it signifies assignment. If we're doing assignment,
   * then we need to scan and find a '=', if any, and start reading from there
   * instead of the end of the word, to trigger reading the next word. */
  int start;
  if (!(flag & EQ_ISVALID_)) {
    if ((start = eq_strlen_(argv[0])) == EQ_NOT_FOUND_)
      start = len;
    else
      ++start;
  }
  else
    start = len;
  return (longv[long_i].proc)(argc, argv, start, state);
}

void
SSC_processCommandLineArgs(
 const int argc,   char** R_         argv,
 const int shortc, const Short_t* R_ shortv,
 const int longc,  const Long_t* R_  longv,
 void* R_  state,  SSC_ArgProc_f*    alone)
{
  for (int arg_i = 0; arg_i < argc; ++arg_i) {
    const int argc_left = argc - arg_i;
    char**    argv_left = argv + arg_i;
    /* Classify the string. */
    const SSC_ArgType_t typ = SSC_getArgType(*argv_left);
    switch (typ) {
      case SSC_ARGTYPE_SHORT: {
        /* The string was a short option. Process all the short options of this word. */
        arg_i += processShorts_(argc_left, argv_left, shortc, shortv, state);
      } break;
      case SSC_ARGTYPE_LONG: {
        /* The string was a long option. Process it, and any arguments if necessary. */
        arg_i += processLongs_(argc_left, argv_left, longc, longv, state);
      } break;
      case SSC_ARGTYPE_NONE: {
        /* The string did not appear to be an option. */
        if (alone)
	  arg_i += alone(argc_left, argv_left, 0, state);
	else
	  SSC_errx("Error: Invalid argument: %s!\n", *argv_left);
      } break;
    } /* ! switch (typ) */
  } /* ! for (int arg_i = 0; arg_i < argc; ++arg_i) */
} /* ! SSC_process_args */

void
SSC_ArgParser_init(
 Parser_t* R_ ctx,
 char* R_     start,
 const int    argc,
 char** R_    argv)
{
  ctx->consumed = 0;
  if (*start != '\0') {
    ctx->to_read = start;
    ctx->size = strlen(ctx->to_read);
  }
  else {
    /* *start == '\0' */
    if (argc >= 2) {
      /* There is a next word. Assume this next word has the input we want. */
      ctx->to_read = argv[1];
      ctx->size = strlen(ctx->to_read);
      ctx->consumed = 1;
    }
    else
      ctx->to_read = SSC_NULL;
  }
}
