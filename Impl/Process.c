#include "Process.h"
#include "Error.h"

#if   defined(__gnu_linux__)
 #include <limits.h>
 #include <stdlib.h>
 #include <string.h>
#elif defined(SSC_OS_WINDOWS)
 #include <windows.h>
 #include <string.h>
#endif

#ifdef SSC_HAS_GETEXECUTABLEPATH
char* SSC_getExecutablePath(
 size_t* exec_path_size)
{
 #if   defined(__gnu_linux__)
  char* exec_path = realpath("/proc/self/exe", SSC_NULL);
  if (exec_path == SSC_NULL)
    return SSC_NULL;
  if (exec_path_size != SSC_NULL)
    *exec_path_size = strlen(exec_path);
  return exec_path;
 #elif defined(SSC_OS_WINDOWS)
  const size_t BUF_SIZE = MAX_PATH + 1;
  wchar_t wide_path [BUF_SIZE] = {0};
  char*   exec_path = (char*)malloc(BUF_SIZE);
  if (exec_path == SSC_NULL)
    return SSC_NULL;
  memset(exec_path, 0, BUF_SIZE);
  DWORD len = GetModuleFileNameW(SSC_NULL, wide_path, MAX_PATH);
  if (len == 0) {
    free(exec_path);
    return SSC_NULL;
  }
  if (exec_path_size != SSC_NULL)
    *exec_path_size = (size_t)len;
  snprintf(exec_path, BUF_SIZE, "%ls", wide_path);
  return exec_path;
 #else
  #error "Invalid implementation!"
 #endif
}
#endif /* ! ifdef SSC_HAS_GETEXECUTABLEPATH */

#ifdef SSC_HAS_GETNUMBERPROCESSORS
int SSC_getNumberProcessors(void)
{
 #if   defined(__gnu_linux__) && defined(_SC_NPROCESSORS_ONLN)
  return (int)sysconf(_SC_NPROCESSORS_ONLN);
 #elif defined(SSC_OS_WINDOWS)
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return (int)si.dwNumberOfProcessors;
 #else
  #error "Invalid implementation!"
 #endif
}
#endif
