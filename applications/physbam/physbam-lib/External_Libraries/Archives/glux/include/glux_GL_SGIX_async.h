
// --------------------------------------------------------
// Generated by glux perl script (Wed Mar 31 17:19:36 2004)
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

#ifndef __GLUX_GL_SGIX_async__
#define __GLUX_GL_SGIX_async__

GLUX_NEW_PLUGIN(GL_SGIX_async);
// --------------------------------------------------------
//           Extension conditions
// --------------------------------------------------------
// --------------------------------------------------------
//           Extension defines
// --------------------------------------------------------
#ifndef GL_ASYNC_MARKER_SGIX
#  define GL_ASYNC_MARKER_SGIX 0x8329
#endif
// --------------------------------------------------------
//           Extension gl function typedefs
// --------------------------------------------------------
#ifndef __GLUX__GLFCT_glAsyncMarkerSGIX
typedef void (APIENTRYP PFNGLUXASYNCMARKERSGIXPROC) (GLuint marker);
#endif
#ifndef __GLUX__GLFCT_glFinishAsyncSGIX
typedef GLint (APIENTRYP PFNGLUXFINISHASYNCSGIXPROC) (GLuint *markerp);
#endif
#ifndef __GLUX__GLFCT_glPollAsyncSGIX
typedef GLint (APIENTRYP PFNGLUXPOLLASYNCSGIXPROC) (GLuint *markerp);
#endif
#ifndef __GLUX__GLFCT_glGenAsyncMarkersSGIX
typedef GLuint (APIENTRYP PFNGLUXGENASYNCMARKERSSGIXPROC) (GLsizei range);
#endif
#ifndef __GLUX__GLFCT_glDeleteAsyncMarkersSGIX
typedef void (APIENTRYP PFNGLUXDELETEASYNCMARKERSSGIXPROC) (GLuint marker, GLsizei range);
#endif
#ifndef __GLUX__GLFCT_glIsAsyncMarkerSGIX
typedef GLboolean (APIENTRYP PFNGLUXISASYNCMARKERSGIXPROC) (GLuint marker);
#endif
// --------------------------------------------------------
//           Extension gl functions
// --------------------------------------------------------
namespace glux {
#ifndef __GLUX__GLFCT_glAsyncMarkerSGIX
extern PFNGLUXASYNCMARKERSGIXPROC glAsyncMarkerSGIX;
#endif
#ifndef __GLUX__GLFCT_glFinishAsyncSGIX
extern PFNGLUXFINISHASYNCSGIXPROC glFinishAsyncSGIX;
#endif
#ifndef __GLUX__GLFCT_glPollAsyncSGIX
extern PFNGLUXPOLLASYNCSGIXPROC glPollAsyncSGIX;
#endif
#ifndef __GLUX__GLFCT_glGenAsyncMarkersSGIX
extern PFNGLUXGENASYNCMARKERSSGIXPROC glGenAsyncMarkersSGIX;
#endif
#ifndef __GLUX__GLFCT_glDeleteAsyncMarkersSGIX
extern PFNGLUXDELETEASYNCMARKERSSGIXPROC glDeleteAsyncMarkersSGIX;
#endif
#ifndef __GLUX__GLFCT_glIsAsyncMarkerSGIX
extern PFNGLUXISASYNCMARKERSGIXPROC glIsAsyncMarkerSGIX;
#endif
} // namespace glux
// --------------------------------------------------------
#endif
// --------------------------------------------------------
