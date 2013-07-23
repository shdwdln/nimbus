
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

#ifndef __GLUX_GL_SUN_triangle_list__
#define __GLUX_GL_SUN_triangle_list__

GLUX_NEW_PLUGIN(GL_SUN_triangle_list);
// --------------------------------------------------------
//           Extension conditions
// --------------------------------------------------------
// --------------------------------------------------------
//           Extension defines
// --------------------------------------------------------
#ifndef GL_RESTART_SUN
#  define GL_RESTART_SUN 0x0001
#endif
#ifndef GL_REPLACE_MIDDLE_SUN
#  define GL_REPLACE_MIDDLE_SUN 0x0002
#endif
#ifndef GL_REPLACE_OLDEST_SUN
#  define GL_REPLACE_OLDEST_SUN 0x0003
#endif
#ifndef GL_TRIANGLE_LIST_SUN
#  define GL_TRIANGLE_LIST_SUN 0x81D7
#endif
#ifndef GL_REPLACEMENT_CODE_SUN
#  define GL_REPLACEMENT_CODE_SUN 0x81D8
#endif
#ifndef GL_REPLACEMENT_CODE_ARRAY_SUN
#  define GL_REPLACEMENT_CODE_ARRAY_SUN 0x85C0
#endif
#ifndef GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN
#  define GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN 0x85C1
#endif
#ifndef GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN
#  define GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN 0x85C2
#endif
#ifndef GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN
#  define GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN 0x85C3
#endif
#ifndef GL_R1UI_V3F_SUN
#  define GL_R1UI_V3F_SUN 0x85C4
#endif
#ifndef GL_R1UI_C4UB_V3F_SUN
#  define GL_R1UI_C4UB_V3F_SUN 0x85C5
#endif
#ifndef GL_R1UI_C3F_V3F_SUN
#  define GL_R1UI_C3F_V3F_SUN 0x85C6
#endif
#ifndef GL_R1UI_N3F_V3F_SUN
#  define GL_R1UI_N3F_V3F_SUN 0x85C7
#endif
#ifndef GL_R1UI_C4F_N3F_V3F_SUN
#  define GL_R1UI_C4F_N3F_V3F_SUN 0x85C8
#endif
#ifndef GL_R1UI_T2F_V3F_SUN
#  define GL_R1UI_T2F_V3F_SUN 0x85C9
#endif
#ifndef GL_R1UI_T2F_N3F_V3F_SUN
#  define GL_R1UI_T2F_N3F_V3F_SUN 0x85CA
#endif
#ifndef GL_R1UI_T2F_C4F_N3F_V3F_SUN
#  define GL_R1UI_T2F_C4F_N3F_V3F_SUN 0x85CB
#endif
// --------------------------------------------------------
//           Extension gl function typedefs
// --------------------------------------------------------
#ifndef __GLUX__GLFCT_glReplacementCodeuiSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEUISUNPROC) (GLuint code);
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeusSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEUSSUNPROC) (GLushort code);
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeubSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEUBSUNPROC) (GLubyte code);
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeuivSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEUIVSUNPROC) (const GLuint *code);
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeusvSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEUSVSUNPROC) (const GLushort *code);
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeubvSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEUBVSUNPROC) (const GLubyte *code);
#endif
#ifndef __GLUX__GLFCT_glReplacementCodePointerSUN
typedef void (APIENTRYP PFNGLUXREPLACEMENTCODEPOINTERSUNPROC) (GLenum type, GLsizei stride, const GLvoid* *pointer);
#endif
// --------------------------------------------------------
//           Extension gl functions
// --------------------------------------------------------
namespace glux {
#ifndef __GLUX__GLFCT_glReplacementCodeuiSUN
extern PFNGLUXREPLACEMENTCODEUISUNPROC glReplacementCodeuiSUN;
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeusSUN
extern PFNGLUXREPLACEMENTCODEUSSUNPROC glReplacementCodeusSUN;
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeubSUN
extern PFNGLUXREPLACEMENTCODEUBSUNPROC glReplacementCodeubSUN;
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeuivSUN
extern PFNGLUXREPLACEMENTCODEUIVSUNPROC glReplacementCodeuivSUN;
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeusvSUN
extern PFNGLUXREPLACEMENTCODEUSVSUNPROC glReplacementCodeusvSUN;
#endif
#ifndef __GLUX__GLFCT_glReplacementCodeubvSUN
extern PFNGLUXREPLACEMENTCODEUBVSUNPROC glReplacementCodeubvSUN;
#endif
#ifndef __GLUX__GLFCT_glReplacementCodePointerSUN
extern PFNGLUXREPLACEMENTCODEPOINTERSUNPROC glReplacementCodePointerSUN;
#endif
} // namespace glux
// --------------------------------------------------------
#endif
// --------------------------------------------------------