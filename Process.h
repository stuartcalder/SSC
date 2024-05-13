#ifndef SSC_PROCESS_H
#define SSC_PROCESS_H
#include "Macro.h"

#if defined(__gnu_linux__) || defined(SSC_OS_WINDOWS)
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
