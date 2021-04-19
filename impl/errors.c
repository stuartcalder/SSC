#include "errors.h"

void
shim_assert_msg (bool b, char const * SHIM_RESTRICT fmt, ...) {
	if (!b) {
		va_list vl;
		va_start(vl, fmt);
		shim_errx_code_list(EXIT_FAILURE, fmt, vl);
	}
}

void
shim_errx_code_vargs (int code, char const * SHIM_RESTRICT fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	shim_errx_code_list(code, fmt, vl);
}

#ifndef SHIM_ERRORS_INLINE_ERRX_CODE_LIST
void
shim_errx_code_list (int code, char const * SHIM_RESTRICT fmt, va_list arg_list)
	SHIM_ERRORS_ERRX_CODE_LIST_IMPL(code, fmt, arg_list)
#endif

void
shim_errx (char const * SHIM_RESTRICT fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	shim_errx_code_list(EXIT_FAILURE, fmt, vl);
}
