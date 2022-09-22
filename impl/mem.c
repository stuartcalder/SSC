/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include "errors.h"
#include "mem.h"

#define R_(Ptr) Ptr BASE_RESTRICT

void* Base_aligned_malloc_or_die(size_t alignment, size_t size)
{
  void* p = Base_aligned_malloc(alignment, size);
  Base_assert_msg(p != BASE_NULL, "Error: Base_aligned_malloc failed.\n");
  return p;
}

void* Base_malloc_or_die(size_t num_bytes)
{
  void* mem = malloc(num_bytes);
  Base_assert_msg(mem != BASE_NULL, BASE_ERR_STR_ALLOC_FAILURE);
  return mem;
}

void* Base_calloc_or_die(size_t n_elem, size_t elem_size)
{
  void* mem = calloc(n_elem, elem_size);
  Base_assert_msg(mem != BASE_NULL, BASE_ERR_STR_ALLOC_FAILURE);
  return mem;
}

void* Base_realloc_or_die(R_(void*) ptr, size_t size)
{
  void* mem = realloc(ptr, size);
  Base_assert_msg(mem != BASE_NULL, BASE_ERR_STR_ALLOC_FAILURE);
  return mem;
}
