#include "errors.h"
#include "mem.h"

#define R_(p) p BASE_RESTRICT

#ifndef BASE_ALIGNED_MALLOC_INLINE
void* Base_aligned_malloc (size_t alignment, size_t size) BASE_ALIGNED_MALLOC_IMPL(alignment, size)
#endif

void* Base_aligned_malloc_or_die (size_t alignment, size_t size) {
	void* p = Base_aligned_malloc(alignment, size);
	Base_assert_msg(p != NULL, "Error: Base_aligned_malloc failed.\n");
	return p;
}

#ifndef BASE_GET_PAGESIZE_INLINE
size_t Base_get_pagesize (void) BASE_GET_PAGESIZE_IMPL
#endif

void* Base_malloc_or_die (size_t num_bytes) {
	void* mem = malloc(num_bytes);
	Base_assert_msg(mem != NULL, BASE_ERR_STR_ALLOC_FAILURE);
	return mem;
}
void* Base_calloc_or_die (size_t n_elem, size_t elem_size) {
	void* mem = calloc(n_elem, elem_size);
	Base_assert_msg(mem != NULL, BASE_ERR_STR_ALLOC_FAILURE);
	return mem;
}
void* Base_realloc_or_die (R_(void*) ptr, size_t size) {
	void* mem = realloc(ptr, size);
	Base_assert_msg(mem != NULL, BASE_ERR_STR_ALLOC_FAILURE);
	return mem;
}
