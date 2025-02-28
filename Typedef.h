#ifndef SSC_TYPEDEF_H
#define SSC_TYPEDEF_H

#include <stdint.h>

/* For functions that return SSC_OK on success and SSC_ERR on failure,
 * use the SSC_Error_t typedef, so it is unambiguous why
 * we are returning an integer. */
typedef enum
{
  SSC_OK =   0,
  SSC_ERR = -1
} SSC_Error_t;

/* For functions that return a field of bits to indicate
 * discrete errors use the SSC_BitError_t typedef. */
typedef unsigned int SSC_BitError_t;
typedef uint8_t      SSC_BitError8_t;
typedef uint16_t     SSC_BitError16_t;
typedef uint32_t     SSC_BitError32_t;
typedef uint64_t     SSC_BitError64_t;

/* For functions that return 0 on success or a range of integers
 * depending on a specific error use the following typedefs. */
typedef int     SSC_CodeError_t;
typedef int32_t SSC_CodeError32_t;
typedef int64_t SSC_CodeError64_t;

/* For functions that pass bit fields as flags use the
 * following typedefs. */
typedef unsigned int SSC_BitFlag_t;
typedef uint8_t      SSC_BitFlag8_t;
typedef uint16_t     SSC_BitFlag16_t;
typedef uint32_t     SSC_BitFlag32_t;
typedef uint64_t     SSC_BitFlag64_t;

#endif
