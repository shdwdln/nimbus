// --------------------------------------------------------
#ifndef WIN32

// --------------------------------------------------------
// Generated by glux perl script (Wed Mar 31 17:19:35 2004)
// 
// Sylvain Lefebvre - 2002 - Sylvain.Lefebvre@imag.fr
// --------------------------------------------------------
#include "glux_no_redefine.h"
#include "glux_ext_defs.h"
#include "gluxLoader.h"
#include "gluxPlugin.h"
// --------------------------------------------------------
//         Plugin creation
// --------------------------------------------------------

#ifndef __GLUX_GLX_SGI_video_sync__
#define __GLUX_GLX_SGI_video_sync__

GLUX_NEW_PLUGIN(GLX_SGI_video_sync);
// --------------------------------------------------------
//           Extension conditions
// --------------------------------------------------------
// --------------------------------------------------------
//           Extension defines
// --------------------------------------------------------
// --------------------------------------------------------
//           Extension gl function typedefs
// --------------------------------------------------------
#ifndef __GLUX__GLFCT_glXGetVideoSyncSGI
typedef int (APIENTRYP PFNXGLUXGETVIDEOSYNCSGIPROC) (unsigned int *count);
#endif
#ifndef __GLUX__GLFCT_glXWaitVideoSyncSGI
typedef int (APIENTRYP PFNXGLUXWAITVIDEOSYNCSGIPROC) (int divisor, int remainder, unsigned int *count);
#endif
// --------------------------------------------------------
//           Extension gl functions
// --------------------------------------------------------
namespace glux {
#ifndef __GLUX__GLFCT_glXGetVideoSyncSGI
extern PFNXGLUXGETVIDEOSYNCSGIPROC glXGetVideoSyncSGI;
#endif
#ifndef __GLUX__GLFCT_glXWaitVideoSyncSGI
extern PFNXGLUXWAITVIDEOSYNCSGIPROC glXWaitVideoSyncSGI;
#endif
} // namespace glux
// --------------------------------------------------------
#endif
// --------------------------------------------------------
#endif
// --------------------------------------------------------
