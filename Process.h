#ifndef SSC_PROCESS_H
#define SSC_PROCESS_H
#include "Macro.h"

#define SSC_GETEXECUTABLEPATH_IMPL_NONE    0
#define SSC_GETEXECUTABLEPATH_IMPL_LINUX   1
#define SSC_GETEXECUTABLEPATH_IMPL_WINDOWS 2

#if   defined(__gnu_linux__)
 #define SSC_GETEXECUTABLEPATH_IMPL SSC_GETEXECUTABLEPATH_IMPL_LINUX
#elif defined(SSC_OS_WINDOWS)
 #define SSC_GETEXECUTABLEPATH_IMPL SSC_GETEXECUTABLEPATH_IMPL_WINDOWS
#else
 #define SSC_GETEXECUTABLEPATH_IMPL SSC_GETEXECUTABLEPATH_IMPL_NONE
#endif

#if SSC_GETEXECUTABLEPATH_IMPL != SSC_GETEXECUTABLEPATH_IMPL_NONE
 #define SSC_HAS_GETEXECUTABLEPATH
#endif

SSC_BEGIN_C_DECLS

#ifdef SSC_HAS_GETEXECUTABLEPATH
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Determine the filepath of the currently running executable. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API char*
SSC_getExecutablePath(size_t* exec_path_size);
/*==========================================================================================*/
#endif

SSC_END_C_DECLS

#endif
