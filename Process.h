#ifndef SSC_PROCESS_H
#define SSC_PROCESS_H
#include "Macro.h"

#if   defined(__gnu_linux__)
 #define SSC_HAS_GETEXECUTABLEPATH
 #include <unistd.h>
 #ifdef _SC_NPROCESSORS_ONLN
  #define SSC_HAS_GETNUMBERPROCESSORS
 #endif
#elif defined(SSC_OS_WINDOWS)
 #define SSC_HAS_GETEXECUTABLEPATH
 #define SSC_HAS_GETNUMBERPROCESSORS
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

#ifdef SSC_HAS_GETNUMBERPROCESSORS
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Return the number of logical processors on the system. */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
SSC_API int
SSC_getNumberProcessors(void);
/*==========================================================================================*/
#endif

SSC_END_C_DECLS

#endif
