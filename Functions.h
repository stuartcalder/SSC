/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#ifndef SSC_FUNCS_H
#define SSC_FUNCS_H

/* In C, you should not cast between void* and function pointers.
 * Use SSC_Fptr_t as a generic function pointer, that can be
 * cast to specific function pointer types. */
typedef void SSC_Func_f();
typedef SSC_Func_f* SSC_Fptr_t;

typedef union {
  SSC_Fptr_t func;/* Function pointer. */
  void*      obj; /* Object pointer. */
} SSC_Ptr;

#endif /* ~ifndef SSC_FUNCS */
