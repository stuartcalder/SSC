#include "Process.h"
#include "Error.h"

#if   defined(SSC_OS_UNIXLIKE)
 #include <limits.h>
 #include <stdlib.h>
 #include <string.h>
#elif defined(SSC_OS_WINDOWS)
 #include <windows.h>
 #include <string.h>
#endif

#ifdef SSC_HAS_GETEXECUTABLEPATH
char* SSC_getExecutablePath(size_t* exec_path_size)
{
 #if   defined(__linux__)
  char* exec_path = realpath("/proc/self/exe", SSC_NULL);
  if (exec_path == SSC_NULL)
    return SSC_NULL;
  if (exec_path_size != SSC_NULL)
    *exec_path_size = strlen(exec_path);
  return exec_path;
 #elif defined(SSC_OS_WINDOWS)
  #define BUF_SIZE_ (MAX_PATH + 1)
  wchar_t wide_path [BUF_SIZE_] = {0};
  char*   exec_path = (char*)malloc(BUF_SIZE_);
  if (exec_path == SSC_NULL)
    return SSC_NULL; /* Return a null pointer if malloc fails. */
  memset(exec_path, 0, BUF_SIZE_);

  DWORD len = GetModuleFileNameW(SSC_NULL, wide_path, MAX_PATH);
  if (len == 0) {
    free(exec_path);
    return SSC_NULL;
  }

  if (exec_path_size != SSC_NULL)
    *exec_path_size = (size_t)len;

  snprintf(exec_path, BUF_SIZE_, "%ls", wide_path);
  return exec_path;
  #undef BUF_SIZE_
 #else
  #error "Invalid implementation!"
 #endif
}
#endif /* ! ifdef SSC_HAS_GETEXECUTABLEPATH */

#ifdef SSC_HAS_GETNUMBERPROCESSORS
int SSC_getNumberProcessors(void)
{
 #if   defined(SSC_OS_UNIXLIKE) && defined(_SC_NPROCESSORS_ONLN)
  return (int)sysconf(_SC_NPROCESSORS_ONLN);
 #elif defined(SSC_OS_WINDOWS)
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return (int)si.dwNumberOfProcessors;
 #else
  #error "Invalid implementation!"
 #endif
}
#endif /* ! ifdef SSC_HAS_GETNUMBERPROCESSORS */
