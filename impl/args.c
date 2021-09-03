#include <string.h>
#include "args.h"
#include "errors.h"

#define R_(p) p BASE_RESTRICT

typedef Base_Arg_Short Short_t;
typedef Base_Arg_Long  Long_t;

static int short_match_(const int, R_(const Short_t*), char);
static int long_match_(const int, R_(const Long_t*), const size_t, R_(char*));
static int process_shorts_(const int, R_(char**), const int, R_(const Short_t*), R_(void*));
static int process_long_(const int, R_(char**), const int, R_(const Long_t*), R_(void*));

int Base_argtype(const char* s) {
	int n_hyphens = 0;
	if (s[0] == '-')
		++n_hyphens;
	else
		goto skip;
	if (s[1] == '-')
		++n_hyphens;
skip:
	switch (n_hyphens) {
		case 1: return BASE_ARGTYPE_SHORT;
		case 2: return BASE_ARGTYPE_LONG;
	}
	return BASE_ARGTYPE_NONE;
}

enum { NOMATCH_ = -1 };

int short_match_(const int shortc, R_(const Base_Arg_Short*) shortv, char ch) {
	for (int shorti = 0; shorti < shortc; ++shorti)
		if (shortv[shorti].ch == ch)
			return shorti;
	return NOMATCH_;
}

int long_match_(const int    longc, R_(const Base_Arg_Long*) longv,
		const size_t str_n, R_(char*)                str)
{
	for (int longi = 0; longi < longc; ++longi) {
		if (str_n != longv[longi].str_n)
			continue;
		const int cmp_res = memcmp(str, longv[longi].str, str_n);
		if (cmp_res > 0)
			continue;
		else if (cmp_res == 0)
			return longi;
		else /* Implicitly: (cmp_res < 0) */
			Base_errx("Error: Invalid long option %s!\n", str);
	}
	return NOMATCH_;
}

int process_shorts_(const int argc,   R_(char**)                argv,
		    const int shortc, R_(const Base_Arg_Short*) shortv,
		    R_(void*) state)
{
	if (!argc) return 0; /* Are there arguments? */
	const char* const s = argv[0];
	const int len = (int)strlen(s);
	if (len < 2) return 0; /* Long enough to process? */
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

int process_long_(const int argc,  R_(char**)               argv,
		  const int longc, R_(const Base_Arg_Long*) longv,
		  R_(void*) state)
{
	if (!argc) return 0; /* Are there arguments? */
	const int len = (int)strlen(argv[0]);
	if (len < 3) return 0; /* Long enough to process? */
	int long_i;
	if ((long_i = long_match_(longc, longv, len - 2, argv[0] + 2)) == NOMATCH_)
		Base_errx("Error: Invalid long option: %s!\n", argv[0]);
	return (longv[long_i].proc)(argc, argv, len, state);
}

void Base_process_args(const int argc,   R_(char**)                argv,
		       const int shortc, R_(const Base_Arg_Short*) shortv,
		       const int longc,  R_(const Base_Arg_Long*)  longv,
		       R_(void*) state)
{
	for (int arg_i = 0; arg_i < argc; ++arg_i) {
		const int arg_left_c = argc - arg_i;
		char**    arg_left_v = argv + arg_i;
		const int typ = Base_argtype(arg_left_v[0]);
		switch (typ) {
			case BASE_ARGTYPE_SHORT: {
				arg_i += process_shorts_(arg_left_c, arg_left_v, shortc, shortv, state);
			} break;
			case BASE_ARGTYPE_LONG: {
				arg_i += process_long_(arg_left_c, arg_left_v, longc, longv, state);
			} break;
			case BASE_ARGTYPE_NONE: {
				Base_errx("Error: Invalid argument: %s!\n", arg_left_v[0]);
			} break;
		}
	}
}

void Base_Arg_Parser_init(R_(Base_Arg_Parser*) ctx,  R_(char*)  start,
			  const int            argc, R_(char**) argv)
{
	ctx->consumed = 0;
	if (*start) {
	/* @start points to a non-null character, so we can start consuming data from here.*/
		ctx->to_read = start;
		ctx->size = strlen(start);
	} else {
	/* @start points to '\0'. If there is any input to read, it will be in the next word.*/
		if (argc >= 2) {
		/* There is a next word. Assume this next word has the input we want. */
			ctx->to_read = argv[1];
			ctx->size = strlen(ctx->to_read);
			ctx->consumed = 1;
		} else ctx->to_read = NULL;
	}
}
