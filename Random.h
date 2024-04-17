/* Copyright (c) 2020-2024 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_RANDOM_H
#define SSC_RANDOM_H

#include "Error.h"
#include "Macro.h"

SSC_BEGIN_C_DECLS

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Obtain @size bytes of pseudorandom data from the
 * operating system, and store them at @ptr. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API void
SSC_getEntropy(void* SSC_RESTRICT ptr, size_t size);
/*===================================================*/

SSC_END_C_DECLS

#endif
