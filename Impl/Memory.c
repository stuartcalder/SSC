/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#include "Memory.h"

#ifdef SSC_HAS_GETTOTALSYSTEMMEMORY
size_t
SSC_getTotalSystemMemory(void)
{
  #if   defined(SSC_OS_UNIXLIKE)
  long pages     = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  return (size_t)pages * (size_t)page_size;
  #elif defined(SSC_OS_WINDOWS)
  MEMORYSTATUSEX ms;
  ms.dwLength = sizeof(ms);
  GlobalMemoryStatusEx(&ms);
  return (size_t)ms.ullTotalPhys;
  #else
   #error "Unsupported OS!"
  #endif
}
#endif

#ifdef SSC_HAS_GETAVAILABLESYSTEMMEMORY
size_t
SSC_getAvailableSystemMemory(void)
{
  #if   defined(SSC_OS_UNIXLIKE)
  long pages     = sysconf(_SC_AVPHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  return (size_t)pages * (size_t)page_size;
  #elif defined(SSC_OS_WINDOWS)
  MEMORYSTATUSEX ms;
  ms.dwLength = sizeof(ms);
  GlobalMemoryStatusEx(&ms);
  return (size_t)ms.ullAvailPhys;
  #else
   #error "Unsupported OS!"
  #endif
}
#endif
