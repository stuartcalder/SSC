#ifndef BASE_FUNCS_H
#define BASE_FUNCS_H

/* In C, you should not cast between void* and function pointers.
 * Use Base_Fptr_t as a generic function pointer, that can be
 * cast to specific function pointer types.
 */
typedef void (*Base_Fptr_t) ();

typedef union {
  Base_Fptr_t func;
  void*       obj; /* Object pointer. */
} Base_Ptr;

#endif /* ~ifndef BASE_FUNCS */
