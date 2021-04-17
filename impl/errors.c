#include "errors.h"

void
shim_assert_msg (bool b, char const * SHIM_RESTRICT msg) {
	if (!b)
		SHIM_ERRX(msg);
}
