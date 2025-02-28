/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "Memory.h"

#ifdef SSC_HAS_GETTOTALSYSTEMMEMORY
size_t
SSC_getTotalSystemMemory(void)
SSC_GETTOTALSYSTEMMEMORY_IMPL
#endif

#ifdef SSC_HAS_GETAVAILABLESYSTEMMEMORY
size_t
SSC_getAvailableSystemMemory(void)
SSC_GETAVAILABLESYSTEMMEMORY_IMPL
#endif
