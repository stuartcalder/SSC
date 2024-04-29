#include "Process.h"
#include "Error.h"
#if   SSC_GETEXECUTABLEPATH_IMPL == SSC_GETEXECUTABLEPATH_IMPL_LINUX
 #include <limits.h>
 #include <stdlib.h>
 #include <string.h>
#elif SSC_GETEXECUTABLEPATH_IMPL == SSC_GETEXECUTABLEPATH_IMPL_WINDOWS
 #include <windows.h>
 #include <string.h>
#endif

#ifdef SSC_HAS_GETEXECUTABLEPATH
char* SSC_getExecutablePath(
 size_t* exec_path_size)
{
 #if   SSC_GETEXECUTABLEPATH_IMPL == SSC_GETEXECUTABLEPATH_IMPL_LINUX
  char* exec_path = realpath("/proc/self/exe", SSC_NULL);
  if (exec_path_size)
    *exec_path_size = strlen(exec_path);
  return exec_path;
 #elif SSC_GETEXECUTABLEPATH_IMPL == SSC_GETEXECUTABLEPATH_IMPL_WINDOWS
  wchar_t wide_path [MAX_PATH + 1] = {0};
  char*   path = (char*)malloc(MAX_PATH + 1);
  SSC_assertMsg(path, "Error: SSC_getExecutablePath(): malloc(MAX_PATH + 1) failed!\n");
  memset(path, 0, MAX_PATH + 1);
  DWORD len = GetModuleFileNameW(SSC_NULL, wide_path, MAX_PATH);
  SSC_assertMsg(len != 0, "Error: SSC_getExecutablePath(): GetModuleFileNameW failed!\n");
  sprintf(path, "%ls", wide_path);
  return path;
 #else
  #error "Invalid implementation!"
 #endif
}
#endif /* ! ifdef SSC_HAS_GETEXECUTABLEPATH */
