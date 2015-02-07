/*
* OpenGL - 3D Vision Wrapper V.1.5
* Copyright (c) Helifax 2015
*/

extern "C" {
#include "opengl_3dv.h"
}

#include "opengl32.h"
#include "include\gl_custom.h"
#include "ConfigReader.h"

configReader *g_reader;

 HMODULE hOriginalDll = 0;
 // Shader Related
 funct_glUseProgram_t orig_glUseProgram;
 funct_glCreateProgram_t orig_glCreateProgram;
 funct_glShaderSource_t orig_glShaderSource;
 funct_glGetAttachedShaders_t orig_glGetAttachedShaders;
 funct_glGetShaderSource_t orig_glGetShaderSource;
 funct_glGetShaderiv_t orig_glGetShader;
 func_glGetUniformLocation_t orig_glGetUniformLocation;
 func_glUniform1f_t orig_glUniform1f;
 func_glBindFramebuffer_t orig_glBindFramebuffer;
 func_glLinkProgram_t orig_glLinkProgram;
 func_glGetProgramiv_t orig_glGetProgramiv;
 func_glCompileShader orig_glCompileShader;
 func_glGetShaderiv_t orig_glGetShaderiv;
 func_glGetShaderInfoLog_t orig_glGetShaderInfoLog;
 func_glGetProgramInfoLog_t orig_glGetProgramInfoLog;
 func_glDetachShader_t orig_glDetachShader;
 func_glDeleteShader_t orig_glDeleteShader;
 func_glAttachShader_t orig_glAttachShader;
 func_glGetError_t orig_glGetError;
 func_glCreateShader orig_glCreateShader;

 // ARB Extension
 funct_glUseProgramObjectARB_t orig_glUseProgramObjectARB;
 func_glCompileShaderARB orig_glCompileShaderARB;


 //Other stuff
 func_glActiveTextureARB_t orig_glActiveTextureARB;
 func_glUniformMatrix4fv_t orig_glUniformMatrix4fv;
 func_glActiveTexture_t orig_glActiveTexture;
 func_BindTextureEXT_t orig_BindTextureEXT;
 func_glMultiTexCoord2fARB_t orig_glMultiTexCoord2fARB;
 func_glAccum_t	orig_glAccum;
 func_glAlphaFunc_t	orig_glAlphaFunc;
 func_glAreTexturesResident_t	orig_glAreTexturesResident;
 func_glArrayElement_t	orig_glArrayElement;
 func_glBegin_t	orig_glBegin;
 func_glBindTexture_t	orig_glBindTexture;
 func_glBitmap_t	orig_glBitmap;
 func_glBlendFunc_t	orig_glBlendFunc;
 func_glCallList_t	orig_glCallList;
 func_glCallLists_t	orig_glCallLists;
 func_glClear_t	orig_glClear;
 func_glClearAccum_t	orig_glClearAccum;
 func_glClearColor_t	orig_glClearColor;
 func_glClearDepth_t	orig_glClearDepth;
 func_glClearIndex_t	orig_glClearIndex;
 func_glClearStencil_t	orig_glClearStencil;
 func_glClipPlane_t	orig_glClipPlane;
 func_glColor3b_t	orig_glColor3b;
 func_glColor3bv_t	orig_glColor3bv;
 func_glColor3d_t	orig_glColor3d;
 func_glColor3dv_t	orig_glColor3dv;
 func_glColor3f_t	orig_glColor3f;
 func_glColor3fv_t	orig_glColor3fv;
 func_glColor3i_t	orig_glColor3i;
 func_glColor3iv_t	orig_glColor3iv;
 func_glColor3s_t	orig_glColor3s;
 func_glColor3sv_t	orig_glColor3sv;
 func_glColor3ub_t	orig_glColor3ub;
 func_glColor3ubv_t	orig_glColor3ubv;
 func_glColor3ui_t	orig_glColor3ui;
 func_glColor3uiv_t	orig_glColor3uiv;
 func_glColor3us_t	orig_glColor3us;
 func_glColor3usv_t	orig_glColor3usv;
 func_glColor4b_t	orig_glColor4b;
 func_glColor4bv_t	orig_glColor4bv;
 func_glColor4d_t	orig_glColor4d;
 func_glColor4dv_t	orig_glColor4dv;
 func_glColor4f_t	orig_glColor4f;
 func_glColor4fv_t	orig_glColor4fv;
 func_glColor4i_t	orig_glColor4i;
 func_glColor4iv_t	orig_glColor4iv;
 func_glColor4s_t	orig_glColor4s;
 func_glColor4sv_t	orig_glColor4sv;
 func_glColor4ub_t	orig_glColor4ub;
 func_glColor4ubv_t	orig_glColor4ubv;
 func_glColor4ui_t	orig_glColor4ui;
 func_glColor4uiv_t	orig_glColor4uiv;
 func_glColor4us_t	orig_glColor4us;
 func_glColor4usv_t	orig_glColor4usv;
 func_glColorMask_t	orig_glColorMask;
 func_glColorMaterial_t	orig_glColorMaterial;
 func_glColorPointer_t	orig_glColorPointer;
 func_glCopyPixels_t	orig_glCopyPixels;
 func_glCopyTexImage1D_t	orig_glCopyTexImage1D;
 func_glCopyTexImage2D_t	orig_glCopyTexImage2D;
 func_glCopyTexSubImage1D_t	orig_glCopyTexSubImage1D;
 func_glCopyTexSubImage2D_t	orig_glCopyTexSubImage2D;
 func_glCullFace_t	orig_glCullFace;
 func_glDebugEntry_t	orig_glDebugEntry;
 func_glDeleteLists_t	orig_glDeleteLists;
 func_glDeleteTextures_t	orig_glDeleteTextures;
 func_glDepthFunc_t	orig_glDepthFunc;
 func_glDepthMask_t	orig_glDepthMask;
 func_glDepthRange_t	orig_glDepthRange;
 func_glDisable_t	orig_glDisable;
 func_glDisableClientState_t	orig_glDisableClientState;
 func_glDrawArrays_t	orig_glDrawArrays;
 func_glDrawBuffer_t	orig_glDrawBuffer;
 func_glDrawElements_t	orig_glDrawElements;
 func_glDrawPixels_t	orig_glDrawPixels;
 func_glEdgeFlag_t	orig_glEdgeFlag;
 func_glEdgeFlagPointer_t	orig_glEdgeFlagPointer;
 func_glEdgeFlagv_t	orig_glEdgeFlagv;
 func_glEnable_t	orig_glEnable;
 func_glEnableClientState_t	orig_glEnableClientState;
 func_glEnd_t	orig_glEnd;
 func_glEndList_t	orig_glEndList;
 func_glEvalCoord1d_t	orig_glEvalCoord1d;
 func_glEvalCoord1dv_t	orig_glEvalCoord1dv;
 func_glEvalCoord1f_t	orig_glEvalCoord1f;
 func_glEvalCoord1fv_t	orig_glEvalCoord1fv;
 func_glEvalCoord2d_t	orig_glEvalCoord2d;
 func_glEvalCoord2dv_t	orig_glEvalCoord2dv;
 func_glEvalCoord2f_t	orig_glEvalCoord2f;
 func_glEvalCoord2fv_t	orig_glEvalCoord2fv;
 func_glEvalMesh1_t	orig_glEvalMesh1;
 func_glEvalMesh2_t	orig_glEvalMesh2;
 func_glEvalPoint1_t	orig_glEvalPoint1;
 func_glEvalPoint2_t	orig_glEvalPoint2;
 func_glFeedbackBuffer_t	orig_glFeedbackBuffer;
 func_glFinish_t	orig_glFinish;
 func_glFlush_t	orig_glFlush;
 func_glFogf_t	orig_glFogf;
 func_glFogfv_t	orig_glFogfv;
 func_glFogi_t	orig_glFogi;
 func_glFogiv_t	orig_glFogiv;
 func_glFrontFace_t	orig_glFrontFace;
 func_glFrustum_t	orig_glFrustum;
 func_glGenLists_t	orig_glGenLists;
 func_glGenTextures_t	orig_glGenTextures;
 func_glGetBooleanv_t	orig_glGetBooleanv;
 func_glGetClipPlane_t	orig_glGetClipPlane;
 func_glGetDoublev_t	orig_glGetDoublev;
 func_glGetFloatv_t	orig_glGetFloatv;
 func_glGetIntegerv_t	orig_glGetIntegerv;
 func_glGetLightfv_t	orig_glGetLightfv;
 func_glGetLightiv_t	orig_glGetLightiv;
 func_glGetMapdv_t	orig_glGetMapdv;
 func_glGetMapfv_t	orig_glGetMapfv;
 func_glGetMapiv_t	orig_glGetMapiv;
 func_glGetMaterialfv_t	orig_glGetMaterialfv;
 func_glGetMaterialiv_t	orig_glGetMaterialiv;
 func_glGetPixelMapfv_t	orig_glGetPixelMapfv;
 func_glGetPixelMapuiv_t	orig_glGetPixelMapuiv;
 func_glGetPixelMapusv_t	orig_glGetPixelMapusv;
 func_glGetPointerv_t	orig_glGetPointerv;
 func_glGetPolygonStipple_t	orig_glGetPolygonStipple;
 func_glGetString_t	orig_glGetString;
 func_glGetTexEnvfv_t	orig_glGetTexEnvfv;
 func_glGetTexEnviv_t	orig_glGetTexEnviv;
 func_glGetTexGendv_t	orig_glGetTexGendv;
 func_glGetTexGenfv_t	orig_glGetTexGenfv;
 func_glGetTexGeniv_t	orig_glGetTexGeniv;
 func_glGetTexImage_t	orig_glGetTexImage;
 func_glGetTexLevelParameterfv_t	orig_glGetTexLevelParameterfv;
 func_glGetTexLevelParameteriv_t	orig_glGetTexLevelParameteriv;
 func_glGetTexParameterfv_t	orig_glGetTexParameterfv;
 func_glGetTexParameteriv_t	orig_glGetTexParameteriv;
 func_glHint_t	orig_glHint;
 func_glIndexMask_t	orig_glIndexMask;
 func_glIndexPointer_t	orig_glIndexPointer;
 func_glIndexd_t	orig_glIndexd;
 func_glIndexdv_t	orig_glIndexdv;
 func_glIndexf_t	orig_glIndexf;
 func_glIndexfv_t	orig_glIndexfv;
 func_glIndexi_t	orig_glIndexi;
 func_glIndexiv_t	orig_glIndexiv;
 func_glIndexs_t	orig_glIndexs;
 func_glIndexsv_t	orig_glIndexsv;
 func_glIndexub_t	orig_glIndexub;
 func_glIndexubv_t	orig_glIndexubv;
 func_glInitNames_t	orig_glInitNames;
 func_glInterleavedArrays_t	orig_glInterleavedArrays;
 func_glIsEnabled_t	orig_glIsEnabled;
 func_glIsList_t	orig_glIsList;
 func_glIsTexture_t	orig_glIsTexture;
 func_glLightModelf_t	orig_glLightModelf;
 func_glLightModelfv_t	orig_glLightModelfv;
 func_glLightModeli_t	orig_glLightModeli;
 func_glLightModeliv_t	orig_glLightModeliv;
 func_glLightf_t	orig_glLightf;
 func_glLightfv_t	orig_glLightfv;
 func_glLighti_t	orig_glLighti;
 func_glLightiv_t	orig_glLightiv;
 func_glLineStipple_t	orig_glLineStipple;
 func_glLineWidth_t	orig_glLineWidth;
 func_glListBase_t	orig_glListBase;
 func_glLoadIdentity_t	orig_glLoadIdentity;
 func_glLoadMatrixd_t	orig_glLoadMatrixd;
 func_glLoadMatrixf_t	orig_glLoadMatrixf;
 func_glLoadName_t	orig_glLoadName;
 func_glLogicOp_t	orig_glLogicOp;
 func_glMap1d_t	orig_glMap1d;
 func_glMap1f_t	orig_glMap1f;
 func_glMap2d_t	orig_glMap2d;
 func_glMap2f_t	orig_glMap2f;
 func_glMapGrid1d_t	orig_glMapGrid1d;
 func_glMapGrid1f_t	orig_glMapGrid1f;
 func_glMapGrid2d_t	orig_glMapGrid2d;
 func_glMapGrid2f_t	orig_glMapGrid2f;
 func_glMaterialf_t	orig_glMaterialf;
 func_glMaterialfv_t	orig_glMaterialfv;
 func_glMateriali_t	orig_glMateriali;
 func_glMaterialiv_t	orig_glMaterialiv;
 func_glMatrixMode_t	orig_glMatrixMode;
 func_glMultMatrixd_t	orig_glMultMatrixd;
 func_glMultMatrixf_t	orig_glMultMatrixf;
 func_glNewList_t	orig_glNewList;
 func_glNormal3b_t	orig_glNormal3b;
 func_glNormal3bv_t	orig_glNormal3bv;
 func_glNormal3d_t	orig_glNormal3d;
 func_glNormal3dv_t	orig_glNormal3dv;
 func_glNormal3f_t	orig_glNormal3f;
 func_glNormal3fv_t	orig_glNormal3fv;
 func_glNormal3i_t	orig_glNormal3i;
 func_glNormal3iv_t	orig_glNormal3iv;
 func_glNormal3s_t	orig_glNormal3s;
 func_glNormal3sv_t	orig_glNormal3sv;
 func_glNormalPointer_t	orig_glNormalPointer;
 func_glOrtho_t	orig_glOrtho;
 func_glPassThrough_t	orig_glPassThrough;
 func_glPixelMapfv_t	orig_glPixelMapfv;
 func_glPixelMapuiv_t	orig_glPixelMapuiv;
 func_glPixelMapusv_t	orig_glPixelMapusv;
 func_glPixelStoref_t	orig_glPixelStoref;
 func_glPixelStorei_t	orig_glPixelStorei;
 func_glPixelTransferf_t	orig_glPixelTransferf;
 func_glPixelTransferi_t	orig_glPixelTransferi;
 func_glPixelZoom_t	orig_glPixelZoom;
 func_glPointSize_t	orig_glPointSize;
 func_glPolygonMode_t	orig_glPolygonMode;
 func_glPolygonOffset_t	orig_glPolygonOffset;
 func_glPolygonStipple_t	orig_glPolygonStipple;
 func_glPopAttrib_t	orig_glPopAttrib;
 func_glPopClientAttrib_t	orig_glPopClientAttrib;
 func_glPopMatrix_t	orig_glPopMatrix;
 func_glPopName_t	orig_glPopName;
 func_glPrioritizeTextures_t	orig_glPrioritizeTextures;
 func_glPushAttrib_t	orig_glPushAttrib;
 func_glPushClientAttrib_t	orig_glPushClientAttrib;
 func_glPushMatrix_t	orig_glPushMatrix;
 func_glPushName_t	orig_glPushName;
 func_glRasterPos2d_t	orig_glRasterPos2d;
 func_glRasterPos2dv_t	orig_glRasterPos2dv;
 func_glRasterPos2f_t	orig_glRasterPos2f;
 func_glRasterPos2fv_t	orig_glRasterPos2fv;
 func_glRasterPos2i_t	orig_glRasterPos2i;
 func_glRasterPos2iv_t	orig_glRasterPos2iv;
 func_glRasterPos2s_t	orig_glRasterPos2s;
 func_glRasterPos2sv_t	orig_glRasterPos2sv;
 func_glRasterPos3d_t	orig_glRasterPos3d;
 func_glRasterPos3dv_t	orig_glRasterPos3dv;
 func_glRasterPos3f_t	orig_glRasterPos3f;
 func_glRasterPos3fv_t	orig_glRasterPos3fv;
 func_glRasterPos3i_t	orig_glRasterPos3i;
 func_glRasterPos3iv_t	orig_glRasterPos3iv;
 func_glRasterPos3s_t	orig_glRasterPos3s;
 func_glRasterPos3sv_t	orig_glRasterPos3sv;
 func_glRasterPos4d_t	orig_glRasterPos4d;
 func_glRasterPos4dv_t	orig_glRasterPos4dv;
 func_glRasterPos4f_t	orig_glRasterPos4f;
 func_glRasterPos4fv_t	orig_glRasterPos4fv;
 func_glRasterPos4i_t	orig_glRasterPos4i;
 func_glRasterPos4iv_t	orig_glRasterPos4iv;
 func_glRasterPos4s_t	orig_glRasterPos4s;
 func_glRasterPos4sv_t	orig_glRasterPos4sv;
 func_glReadBuffer_t	orig_glReadBuffer;
 func_glReadPixels_t	orig_glReadPixels;
 func_glRectd_t	orig_glRectd;
 func_glRectdv_t	orig_glRectdv;
 func_glRectf_t	orig_glRectf;
 func_glRectfv_t	orig_glRectfv;
 func_glRecti_t	orig_glRecti;
 func_glRectiv_t	orig_glRectiv;
 func_glRects_t	orig_glRects;
 func_glRectsv_t	orig_glRectsv;
 func_glRenderMode_t	orig_glRenderMode;
 func_glRotated_t	orig_glRotated;
 func_glRotatef_t	orig_glRotatef;
 func_glScaled_t	orig_glScaled;
 func_glScalef_t	orig_glScalef;
 func_glScissor_t	orig_glScissor;
 func_glSelectBuffer_t	orig_glSelectBuffer;
 func_glShadeModel_t	orig_glShadeModel;
 func_glStencilFunc_t	orig_glStencilFunc;
 func_glStencilMask_t	orig_glStencilMask;
 func_glStencilOp_t	orig_glStencilOp;
 func_glTexCoord1d_t	orig_glTexCoord1d;
 func_glTexCoord1dv_t	orig_glTexCoord1dv;
 func_glTexCoord1f_t	orig_glTexCoord1f;
 func_glTexCoord1fv_t	orig_glTexCoord1fv;
 func_glTexCoord1i_t	orig_glTexCoord1i;
 func_glTexCoord1iv_t	orig_glTexCoord1iv;
 func_glTexCoord1s_t	orig_glTexCoord1s;
 func_glTexCoord1sv_t	orig_glTexCoord1sv;
 func_glTexCoord2d_t	orig_glTexCoord2d;
 func_glTexCoord2dv_t	orig_glTexCoord2dv;
 func_glTexCoord2f_t	orig_glTexCoord2f;
 func_glTexCoord2fv_t	orig_glTexCoord2fv;
 func_glTexCoord2i_t	orig_glTexCoord2i;
 func_glTexCoord2iv_t	orig_glTexCoord2iv;
 func_glTexCoord2s_t	orig_glTexCoord2s;
 func_glTexCoord2sv_t	orig_glTexCoord2sv;
 func_glTexCoord3d_t	orig_glTexCoord3d;
 func_glTexCoord3dv_t	orig_glTexCoord3dv;
 func_glTexCoord3f_t	orig_glTexCoord3f;
 func_glTexCoord3fv_t	orig_glTexCoord3fv;
 func_glTexCoord3i_t	orig_glTexCoord3i;
 func_glTexCoord3iv_t	orig_glTexCoord3iv;
 func_glTexCoord3s_t	orig_glTexCoord3s;
 func_glTexCoord3sv_t	orig_glTexCoord3sv;
 func_glTexCoord4d_t	orig_glTexCoord4d;
 func_glTexCoord4dv_t	orig_glTexCoord4dv;
 func_glTexCoord4f_t	orig_glTexCoord4f;
 func_glTexCoord4fv_t	orig_glTexCoord4fv;
 func_glTexCoord4i_t	orig_glTexCoord4i;
 func_glTexCoord4iv_t	orig_glTexCoord4iv;
 func_glTexCoord4s_t	orig_glTexCoord4s;
 func_glTexCoord4sv_t	orig_glTexCoord4sv;
 func_glTexCoordPointer_t	orig_glTexCoordPointer;
 func_glTexEnvf_t	orig_glTexEnvf;
 func_glTexEnvfv_t	orig_glTexEnvfv;
 func_glTexEnvi_t	orig_glTexEnvi;
 func_glTexEnviv_t	orig_glTexEnviv;
 func_glTexGend_t	orig_glTexGend;
 func_glTexGendv_t	orig_glTexGendv;
 func_glTexGenf_t	orig_glTexGenf;
 func_glTexGenfv_t	orig_glTexGenfv;
 func_glTexGeni_t	orig_glTexGeni;
 func_glTexGeniv_t	orig_glTexGeniv;
 func_glTexImage1D_t	orig_glTexImage1D;
 func_glTexImage2D_t	orig_glTexImage2D;
 func_glTexParameterf_t	orig_glTexParameterf;
 func_glTexParameterfv_t	orig_glTexParameterfv;
 func_glTexParameteri_t	orig_glTexParameteri;
 func_glTexParameteriv_t	orig_glTexParameteriv;
 func_glTexSubImage1D_t	orig_glTexSubImage1D;
 func_glTexSubImage2D_t	orig_glTexSubImage2D;
 func_glTranslated_t	orig_glTranslated;
 func_glTranslatef_t orig_glTranslatef;
 func_glVertex2d_t	orig_glVertex2d;
 func_glVertex2dv_t	orig_glVertex2dv;
 func_glVertex2f_t	orig_glVertex2f;
 func_glVertex2fv_t	orig_glVertex2fv;
 func_glVertex2i_t	orig_glVertex2i;
 func_glVertex2iv_t	orig_glVertex2iv;
 func_glVertex2s_t	orig_glVertex2s;
 func_glVertex2sv_t	orig_glVertex2sv;
 func_glVertex3d_t	orig_glVertex3d;
 func_glVertex3dv_t	orig_glVertex3dv;
 func_glVertex3f_t	orig_glVertex3f;
 func_glVertex3fv_t	orig_glVertex3fv;
 func_glVertex3i_t	orig_glVertex3i;
 func_glVertex3iv_t	orig_glVertex3iv;
 func_glVertex3s_t	orig_glVertex3s;
 func_glVertex3sv_t	orig_glVertex3sv;
 func_glVertex4d_t	orig_glVertex4d;
 func_glVertex4dv_t	orig_glVertex4dv;
 func_glVertex4f_t	orig_glVertex4f;
 func_glVertex4fv_t	orig_glVertex4fv;
 func_glVertex4i_t	orig_glVertex4i;
 func_glVertex4iv_t	orig_glVertex4iv;
 func_glVertex4s_t	orig_glVertex4s;
 func_glVertex4sv_t	orig_glVertex4sv;
 func_glVertexPointer_t	orig_glVertexPointer;
 func_glViewport_t	orig_glViewport;
 func_wglChoosePixelFormat_t	orig_wglChoosePixelFormat;
 func_wglCopyContext_t	orig_wglCopyContext;
 func_wglCreateContext_t	orig_wglCreateContext;
 func_wglCreateLayerContext_t	orig_wglCreateLayerContext;
 func_wglDeleteContext_t	orig_wglDeleteContext;
 func_wglDescribeLayerPlane_t	orig_wglDescribeLayerPlane;
 func_wglDescribePixelFormat_t	orig_wglDescribePixelFormat;
 func_wglGetCurrentContext_t	orig_wglGetCurrentContext;
 func_wglGetCurrentDC_t	orig_wglGetCurrentDC;
 func_wglGetDefaultProcAddress_t	orig_wglGetDefaultProcAddress;
 func_wglGetLayerPaletteEntries_t	orig_wglGetLayerPaletteEntries;
 func_wglGetPixelFormat_t	orig_wglGetPixelFormat;
 func_wglGetProcAddress_t	orig_wglGetProcAddress;
 func_wglMakeCurrent_t	orig_wglMakeCurrent;
 func_wglRealizeLayerPalette_t	orig_wglRealizeLayerPalette;
 func_wglSetLayerPaletteEntries_t	orig_wglSetLayerPaletteEntries;
 func_wglSetPixelFormat_t	orig_wglSetPixelFormat;
 func_wglShareLists_t	orig_wglShareLists;
 func_wglSwapBuffers_t	orig_wglSwapBuffers;
 func_wglSwapLayerBuffers_t	orig_wglSwapLayerBuffers;
 func_wglUseFontBitmapsA_t	orig_wglUseFontBitmapsA;
 func_wglUseFontBitmapsW_t	orig_wglUseFontBitmapsW;
 func_wglUseFontOutlinesA_t	orig_wglUseFontOutlinesA;
 func_wglUseFontOutlinesW_t	orig_wglUseFontOutlinesW;

void sys_glAccum (GLenum op,  GLfloat value)
{
	(*orig_glAccum) (op, value);
}

GLboolean sys_glAreTexturesResident (GLsizei n,  const GLuint *textures,  GLboolean *residences)
{
	return (*orig_glAreTexturesResident) (n, textures, residences);
}

void sys_glArrayElement (GLint i)
{
	(*orig_glArrayElement) (i);
}

void sys_glBindTexture (GLenum target,  GLuint texture)
{
	(*orig_glBindTexture) (target, texture);
}

void sys_glCallList (GLuint list)
{
	(*orig_glCallList) (list);
}

void sys_glCallLists (GLsizei n,  GLenum type,  const GLvoid *lists)
{
	(*orig_glCallLists) (n, type, lists);
}


void sys_glClearDepth (GLclampd depth)
{
	(*orig_glClearDepth) (depth);
}

void sys_glClearIndex (GLfloat c)
{
	(*orig_glClearIndex) (c);
}

void sys_glClearStencil (GLint s)
{
	(*orig_glClearStencil) (s);
}

void sys_glClipPlane (GLenum plane,  const GLdouble *equation)
{
	(*orig_glClipPlane) (plane, equation);
}

void sys_glColor3b (GLbyte red,  GLbyte green,  GLbyte blue)
{
	(*orig_glColor3b) (red, green, blue);
}

void sys_glColor3bv (const GLbyte *v)
{
	(*orig_glColor3bv) (v);
}

void sys_glColor3d (GLdouble red,  GLdouble green,  GLdouble blue)
{
	(*orig_glColor3d) (red, green, blue);
}

void sys_glColor3dv (const GLdouble *v)
{
	(*orig_glColor3dv) (v);
}


void sys_glColor3fv (const GLfloat *v)
{
	(*orig_glColor3fv) (v);
}

void sys_glColor3i (GLint red,  GLint green,  GLint blue)
{
	(*orig_glColor3i) (red, green, blue);
}

void sys_glColor3iv (const GLint *v)
{
	(*orig_glColor3iv) (v);
}

void sys_glColor3s (GLshort red,  GLshort green,  GLshort blue)
{
	(*orig_glColor3s) (red, green, blue);
}

void sys_glColor3sv (const GLshort *v)
{
	(*orig_glColor3sv) (v);
}

void sys_glColor3ui (GLuint red,  GLuint green,  GLuint blue)
{
	(*orig_glColor3ui) (red, green, blue);
}

void sys_glColor3uiv (const GLuint *v)
{
	(*orig_glColor3uiv) (v);
}

void sys_glColor3us (GLushort red,  GLushort green,  GLushort blue)
{
	(*orig_glColor3us) (red, green, blue);
}

void sys_glColor3usv (const GLushort *v)
{
	(*orig_glColor3usv) (v);
}

void sys_glColor4b (GLbyte red,  GLbyte green,  GLbyte blue,  GLbyte alpha)
{
	(*orig_glColor4b) (red, green, blue, alpha);
}

void sys_glColor4bv (const GLbyte *v)
{
	(*orig_glColor4bv) (v);
}

void sys_glColor4d (GLdouble red,  GLdouble green,  GLdouble blue,  GLdouble alpha)
{
	(*orig_glColor4d) (red, green, blue, alpha);
}

void sys_glColor4dv (const GLdouble *v)
{
	(*orig_glColor4dv) (v);
}


void sys_glColor4fv (const GLfloat *v)
{
	(*orig_glColor4fv) (v);
}

void sys_glColor4i (GLint red,  GLint green,  GLint blue,  GLint alpha)
{
	(*orig_glColor4i) (red, green, blue, alpha);
}

void sys_glColor4iv (const GLint *v)
{
	(*orig_glColor4iv) (v);
}

void sys_glColor4s (GLshort red,  GLshort green,  GLshort blue,  GLshort alpha)
{
	(*orig_glColor4s) (red, green, blue, alpha);
}

void sys_glColor4sv (const GLshort *v)
{
	(*orig_glColor4sv) (v);
}


void sys_glColor4ubv (const GLubyte *v)
{
	(*orig_glColor4ubv) (v);
}

void sys_glColor4ui (GLuint red,  GLuint green,  GLuint blue,  GLuint alpha)
{
	(*orig_glColor4ui) (red, green, blue, alpha);
}

void sys_glColor4uiv (const GLuint *v)
{
	(*orig_glColor4uiv) (v);
}

void sys_glColor4us (GLushort red,  GLushort green,  GLushort blue,  GLushort alpha)
{
	(*orig_glColor4us) (red, green, blue, alpha);
}

void sys_glColor4usv (const GLushort *v)
{
	(*orig_glColor4usv) (v);
}

void sys_glColorMask (GLboolean red,  GLboolean green,  GLboolean blue,  GLboolean alpha)
{
	(*orig_glColorMask) (red, green, blue, alpha);
}

void sys_glColorMaterial (GLenum face,  GLenum mode)
{
	(*orig_glColorMaterial) (face, mode);
}

void sys_glColorPointer (GLint size,  GLenum type,  GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glColorPointer) (size, type, stride, pointer);
}

void sys_glCopyPixels (GLint x,  GLint y,  GLsizei width,  GLsizei height,  GLenum type)
{
	(*orig_glCopyPixels) (x, y, width, height, type);
}

void sys_glCopyTexImage1D (GLenum target,  GLint level,  GLenum internalFormat,  GLint x,  GLint y,  GLsizei width,  GLint border)
{
	(*orig_glCopyTexImage1D) (target, level, internalFormat, x, y, width, border);
}

void sys_glCopyTexImage2D (GLenum target,  GLint level,  GLenum internalFormat,  GLint x,  GLint y,  GLsizei width,  GLsizei height,  GLint border)
{
	(*orig_glCopyTexImage2D) (target, level, internalFormat, x, y, width, height, border);
}

void sys_glCopyTexSubImage1D (GLenum target,  GLint level,  GLint xoffset,  GLint x,  GLint y,  GLsizei width)
{
	(*orig_glCopyTexSubImage1D) (target, level, xoffset, x, y, width);
}

void sys_glCopyTexSubImage2D (GLenum target,  GLint level,  GLint xoffset,  GLint yoffset,  GLint x,  GLint y,  GLsizei width,  GLsizei height)
{
	(*orig_glCopyTexSubImage2D) (target, level, xoffset, yoffset, x, y, width, height);
}

__declspec(naked) void sys_glDebugEntry(void)
{
	__asm
	{
		jmp [orig_glDebugEntry]
	}
}

void sys_glDeleteLists (GLuint list,  GLsizei range)
{
	(*orig_glDeleteLists) (list, range);
}

void sys_glDisableClientState (GLenum array)
{
	(*orig_glDisableClientState) (array);
}

void sys_glDrawArrays (GLenum mode,  GLint first,  GLsizei count)
{
	(*orig_glDrawArrays) (mode, first, count);
}

void sys_glDrawBuffer (GLenum mode)
{
	(*orig_glDrawBuffer) (mode);
}

void sys_glDrawElements (GLenum mode,  GLsizei count,  GLenum type,  const GLvoid *indices)
{
	(*orig_glDrawElements) (mode, count, type, indices);
}

void sys_glDrawPixels (GLsizei width,  GLsizei height,  GLenum format,  GLenum type,  const GLvoid *pixels)
{
	(*orig_glDrawPixels) (width, height, format, type, pixels);
}

void sys_glEdgeFlag (GLboolean flag)
{
	(*orig_glEdgeFlag) (flag);
}

void sys_glEdgeFlagPointer (GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glEdgeFlagPointer) (stride, pointer);
}

void sys_glEdgeFlagv (const GLboolean *flag)
{
	(*orig_glEdgeFlagv) (flag);
}

void sys_glEnableClientState (GLenum array)
{
	(*orig_glEnableClientState) (array);
}


void sys_glEndList (void)
{
	(*orig_glEndList) ();
}

void sys_glEvalCoord1d (GLdouble u)
{
	(*orig_glEvalCoord1d) (u);
}

void sys_glEvalCoord1dv (const GLdouble *u)
{
	(*orig_glEvalCoord1dv) (u);
}

void sys_glEvalCoord1f (GLfloat u)
{
	(*orig_glEvalCoord1f) (u);
}

void sys_glEvalCoord1fv (const GLfloat *u)
{
	(*orig_glEvalCoord1fv) (u);
}

void sys_glEvalCoord2d (GLdouble u,  GLdouble v)
{
	(*orig_glEvalCoord2d) (u, v);
}

void sys_glEvalCoord2dv (const GLdouble *u)
{
	(*orig_glEvalCoord2dv) (u);
}

void sys_glEvalCoord2f (GLfloat u,  GLfloat v)
{
	(*orig_glEvalCoord2f) (u, v);
}

void sys_glEvalCoord2fv (const GLfloat *u)
{
	(*orig_glEvalCoord2fv) (u);
}

void sys_glEvalMesh1 (GLenum mode,  GLint i1,  GLint i2)
{
	(*orig_glEvalMesh1) (mode, i1, i2);
}

void sys_glEvalMesh2 (GLenum mode,  GLint i1,  GLint i2,  GLint j1,  GLint j2)
{
	(*orig_glEvalMesh2) (mode, i1, i2, j1, j2);
}

void sys_glEvalPoint1 (GLint i)
{
	(*orig_glEvalPoint1) (i);
}

void sys_glEvalPoint2 (GLint i,  GLint j)
{
	(*orig_glEvalPoint2) (i, j);
}

void sys_glFeedbackBuffer (GLsizei size,  GLenum type,  GLfloat *buffer)
{
	(*orig_glFeedbackBuffer) (size, type, buffer);
}

void sys_glFinish (void)
{
	(*orig_glFinish) ();
}

void sys_glFlush (void)
{
	(*orig_glFlush) ();
}

void sys_glFogf (GLenum pname,  GLfloat param)
{
	(*orig_glFogf) (pname, param);
}

void sys_glFogfv (GLenum pname,  const GLfloat *params)
{
	(*orig_glFogfv) (pname, params);
}

void sys_glFogi (GLenum pname,  GLint param)
{
	(*orig_glFogi) (pname, param);
}

void sys_glFogiv (GLenum pname,  const GLint *params)
{
	(*orig_glFogiv) (pname, params);
}

void sys_glFrontFace (GLenum mode)
{
	(*orig_glFrontFace) (mode);
}

GLuint sys_glGenLists (GLsizei range)
{
	return (*orig_glGenLists) (range);
}

void sys_glGenTextures (GLsizei n,  GLuint *textures)
{
	(*orig_glGenTextures) (n, textures);
}

void sys_glGetBooleanv (GLenum pname,  GLboolean *params)
{
	(*orig_glGetBooleanv) (pname, params);
}

void sys_glGetClipPlane (GLenum plane,  GLdouble *equation)
{
	(*orig_glGetClipPlane) (plane, equation);
}

void sys_glGetDoublev (GLenum pname,  GLdouble *params)
{
	(*orig_glGetDoublev) (pname, params);
}

GLenum sys_glGetError (void)
{
	return (*orig_glGetError) ();
}

void sys_glGetFloatv (GLenum pname,  GLfloat *params)
{
	(*orig_glGetFloatv) (pname, params);
}

void sys_glGetIntegerv (GLenum pname,  GLint *params)
{
	(*orig_glGetIntegerv) (pname, params);
}

void sys_glGetLightfv (GLenum light,  GLenum pname,  GLfloat *params)
{
	(*orig_glGetLightfv) (light, pname, params);
}

void sys_glGetLightiv (GLenum light,  GLenum pname,  GLint *params)
{
	(*orig_glGetLightiv) (light, pname, params);
}

void sys_glGetMapdv (GLenum target,  GLenum query,  GLdouble *v)
{
	(*orig_glGetMapdv) (target, query, v);
}

void sys_glGetMapfv (GLenum target,  GLenum query,  GLfloat *v)
{
	(*orig_glGetMapfv) (target, query, v);
}

void sys_glGetMapiv (GLenum target,  GLenum query,  GLint *v)
{
	(*orig_glGetMapiv) (target, query, v);
}

void sys_glGetMaterialfv (GLenum face,  GLenum pname,  GLfloat *params)
{
	(*orig_glGetMaterialfv) (face, pname, params);
}

void sys_glGetMaterialiv (GLenum face,  GLenum pname,  GLint *params)
{
	(*orig_glGetMaterialiv) (face, pname, params);
}

void sys_glGetPixelMapfv (GLenum map,  GLfloat *values)
{
	(*orig_glGetPixelMapfv) (map, values);
}

void sys_glGetPixelMapuiv (GLenum map,  GLuint *values)
{
	(*orig_glGetPixelMapuiv) (map, values);
}

void sys_glGetPixelMapusv (GLenum map,  GLushort *values)
{
	(*orig_glGetPixelMapusv) (map, values);
}

void sys_glGetPointerv (GLenum pname,  GLvoid* *params)
{
	(*orig_glGetPointerv) (pname, params);
}

void sys_glGetPolygonStipple (GLubyte *mask)
{
	(*orig_glGetPolygonStipple) (mask);
}


void sys_glGetTexEnvfv (GLenum target,  GLenum pname,  GLfloat *params)
{
	(*orig_glGetTexEnvfv) (target, pname, params);
}

void sys_glGetTexEnviv (GLenum target,  GLenum pname,  GLint *params)
{
	(*orig_glGetTexEnviv) (target, pname, params);
}

void sys_glGetTexGendv (GLenum coord,  GLenum pname,  GLdouble *params)
{
	(*orig_glGetTexGendv) (coord, pname, params);
}

void sys_glGetTexGenfv (GLenum coord,  GLenum pname,  GLfloat *params)
{
	(*orig_glGetTexGenfv) (coord, pname, params);
}

void sys_glGetTexGeniv (GLenum coord,  GLenum pname,  GLint *params)
{
	(*orig_glGetTexGeniv) (coord, pname, params);
}

void sys_glGetTexImage (GLenum target,  GLint level,  GLenum format,  GLenum type,  GLvoid *pixels)
{
	(*orig_glGetTexImage) (target, level, format, type, pixels);
}

void sys_glGetTexLevelParameterfv (GLenum target,  GLint level,  GLenum pname,  GLfloat *params)
{
	(*orig_glGetTexLevelParameterfv) (target, level, pname, params);
}

void sys_glGetTexLevelParameteriv (GLenum target,  GLint level,  GLenum pname,  GLint *params)
{
	(*orig_glGetTexLevelParameteriv) (target, level, pname, params);
}

void sys_glGetTexParameterfv (GLenum target,  GLenum pname,  GLfloat *params)
{
	(*orig_glGetTexParameterfv) (target, pname, params);
}

void sys_glGetTexParameteriv (GLenum target,  GLenum pname,  GLint *params)
{
	(*orig_glGetTexParameteriv) (target, pname, params);
}

void sys_glHint (GLenum target,  GLenum mode)
{
	(*orig_glHint) (target, mode);
}

void sys_glIndexMask (GLuint mask)
{
	(*orig_glIndexMask) (mask);
}

void sys_glIndexPointer (GLenum type,  GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glIndexPointer) (type, stride, pointer);
}

void sys_glIndexd (GLdouble c)
{
	(*orig_glIndexd) (c);
}

void sys_glIndexdv (const GLdouble *c)
{
	(*orig_glIndexdv) (c);
}

void sys_glIndexf (GLfloat c)
{
	(*orig_glIndexf) (c);
}

void sys_glIndexfv (const GLfloat *c)
{
	(*orig_glIndexfv) (c);
}

void sys_glIndexi (GLint c)
{
	(*orig_glIndexi) (c);
}

void sys_glIndexiv (const GLint *c)
{
	(*orig_glIndexiv) (c);
}

void sys_glIndexs (GLshort c)
{
	(*orig_glIndexs) (c);
}

void sys_glIndexsv (const GLshort *c)
{
	(*orig_glIndexsv) (c);
}

void sys_glIndexub (GLubyte c)
{
	(*orig_glIndexub) (c);
}

void sys_glIndexubv (const GLubyte *c)
{
	(*orig_glIndexubv) (c);
}

void sys_glInitNames (void)
{
	(*orig_glInitNames) ();
}

void sys_glInterleavedArrays (GLenum format,  GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glInterleavedArrays) (format, stride, pointer);
}

GLboolean sys_glIsEnabled (GLenum cap)
{
	return (*orig_glIsEnabled) (cap);
}

GLboolean sys_glIsList (GLuint list)
{
	return (*orig_glIsList) (list);
}

GLboolean sys_glIsTexture (GLuint texture)
{
	return (*orig_glIsTexture) (texture);
}

void sys_glLightModelf (GLenum pname,  GLfloat param)
{
	(*orig_glLightModelf) (pname, param);
}

void sys_glLightModelfv (GLenum pname,  const GLfloat *params)
{
	(*orig_glLightModelfv) (pname, params);
}

void sys_glLightModeli (GLenum pname,  GLint param)
{
	(*orig_glLightModeli) (pname, param);
}

void sys_glLightModeliv (GLenum pname,  const GLint *params)
{
	(*orig_glLightModeliv) (pname, params);
}

void sys_glLightf (GLenum light,  GLenum pname,  GLfloat param)
{
	(*orig_glLightf) (light, pname, param);
}

void sys_glLightfv (GLenum light,  GLenum pname,  const GLfloat *params)
{
	(*orig_glLightfv) (light, pname, params);
}

void sys_glLighti (GLenum light,  GLenum pname,  GLint param)
{
	(*orig_glLighti) (light, pname, param);
}

void sys_glLightiv (GLenum light,  GLenum pname,  const GLint *params)
{
	(*orig_glLightiv) (light, pname, params);
}

void sys_glLineStipple (GLint factor,  GLushort pattern)
{
	(*orig_glLineStipple) (factor, pattern);
}

void sys_glLineWidth (GLfloat width)
{
	(*orig_glLineWidth) (width);
}

void sys_glListBase (GLuint base)
{
	(*orig_glListBase) (base);
}

void sys_glLoadIdentity (void)
{
	(*orig_glLoadIdentity) ();
}

void sys_glLoadMatrixd (const GLdouble *m)
{
	(*orig_glLoadMatrixd) (m);
}

void sys_glLoadMatrixf (const GLfloat *m)
{
	(*orig_glLoadMatrixf) (m);
}

void sys_glLoadName (GLuint name)
{
	(*orig_glLoadName) (name);
}

void sys_glLogicOp (GLenum opcode)
{
	(*orig_glLogicOp) (opcode);
}

void sys_glMap1d (GLenum target,  GLdouble u1,  GLdouble u2,  GLint stride,  GLint order,  const GLdouble *points)
{
	(*orig_glMap1d) (target, u1, u2, stride, order, points);
}

void sys_glMap1f (GLenum target,  GLfloat u1,  GLfloat u2,  GLint stride,  GLint order,  const GLfloat *points)
{
	(*orig_glMap1f) (target, u1, u2, stride, order, points);
}

void sys_glMap2d (GLenum target,  GLdouble u1,  GLdouble u2,  GLint ustride,  GLint uorder,  GLdouble v1,  GLdouble v2,  GLint vstride,  GLint vorder,  const GLdouble *points)
{
	(*orig_glMap2d) (target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

void sys_glMap2f (GLenum target,  GLfloat u1,  GLfloat u2,  GLint ustride,  GLint uorder,  GLfloat v1,  GLfloat v2,  GLint vstride,  GLint vorder,  const GLfloat *points)
{
	(*orig_glMap2f) (target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

void sys_glMapGrid1d (GLint un,  GLdouble u1,  GLdouble u2)
{
	(*orig_glMapGrid1d) (un, u1, u2);
}

void sys_glMapGrid1f (GLint un,  GLfloat u1,  GLfloat u2)
{
	(*orig_glMapGrid1f) (un, u1, u2);
}

void sys_glMapGrid2d (GLint un,  GLdouble u1,  GLdouble u2,  GLint vn,  GLdouble v1,  GLdouble v2)
{
	(*orig_glMapGrid2d) (un, u1, u2, vn, v1, v2);
}

void sys_glMapGrid2f (GLint un,  GLfloat u1,  GLfloat u2,  GLint vn,  GLfloat v1,  GLfloat v2)
{
	(*orig_glMapGrid2f) (un, u1, u2, vn, v1, v2);
}

void sys_glMaterialf (GLenum face,  GLenum pname,  GLfloat param)
{
	(*orig_glMaterialf) (face, pname, param);
}

void sys_glMaterialfv (GLenum face,  GLenum pname,  const GLfloat *params)
{
	(*orig_glMaterialfv) (face, pname, params);
}

void sys_glMateriali (GLenum face,  GLenum pname,  GLint param)
{
	(*orig_glMateriali) (face, pname, param);
}

void sys_glMaterialiv (GLenum face,  GLenum pname,  const GLint *params)
{
	(*orig_glMaterialiv) (face, pname, params);
}

void sys_glMatrixMode (GLenum mode)
{
	(*orig_glMatrixMode) (mode);
}

void sys_glMultMatrixd (const GLdouble *m)
{
	(*orig_glMultMatrixd) (m);
}

void sys_glMultMatrixf (const GLfloat *m)
{
	(*orig_glMultMatrixf) (m);
}

void sys_glNewList (GLuint list,  GLenum mode)
{
	(*orig_glNewList) (list, mode);
}

void sys_glNormal3b (GLbyte nx,  GLbyte ny,  GLbyte nz)
{
	(*orig_glNormal3b) (nx, ny, nz);
}

void sys_glNormal3bv (const GLbyte *v)
{
	(*orig_glNormal3bv) (v);
}

void sys_glNormal3d (GLdouble nx,  GLdouble ny,  GLdouble nz)
{
	(*orig_glNormal3d) (nx, ny, nz);
}

void sys_glNormal3dv (const GLdouble *v)
{
	(*orig_glNormal3dv) (v);
}

void sys_glNormal3f (GLfloat nx,  GLfloat ny,  GLfloat nz)
{
	(*orig_glNormal3f) (nx, ny, nz);
}

void sys_glNormal3fv (const GLfloat *v)
{
	(*orig_glNormal3fv) (v);
}

void sys_glNormal3i (GLint nx,  GLint ny,  GLint nz)
{
	(*orig_glNormal3i) (nx, ny, nz);
}

void sys_glNormal3iv (const GLint *v)
{
	(*orig_glNormal3iv) (v);
}

void sys_glNormal3s (GLshort nx,  GLshort ny,  GLshort nz)
{
	(*orig_glNormal3s) (nx, ny, nz);
}

void sys_glNormal3sv (const GLshort *v)
{
	(*orig_glNormal3sv) (v);
}

void sys_glNormalPointer (GLenum type,  GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glNormalPointer) (type, stride, pointer);
}

void sys_glPassThrough (GLfloat token)
{
	(*orig_glPassThrough) (token);
}

void sys_glPixelMapfv (GLenum map,  GLsizei mapsize,  const GLfloat *values)
{
	(*orig_glPixelMapfv) (map, mapsize, values);
}

void sys_glPixelMapuiv (GLenum map,  GLsizei mapsize,  const GLuint *values)
{
	(*orig_glPixelMapuiv) (map, mapsize, values);
}

void sys_glPixelMapusv (GLenum map,  GLsizei mapsize,  const GLushort *values)
{
	(*orig_glPixelMapusv) (map, mapsize, values);
}

void sys_glPixelStoref (GLenum pname,  GLfloat param)
{
	(*orig_glPixelStoref) (pname, param);
}

void sys_glPixelStorei (GLenum pname,  GLint param)
{
	(*orig_glPixelStorei) (pname, param);
}

void sys_glPixelTransferf (GLenum pname,  GLfloat param)
{
	(*orig_glPixelTransferf) (pname, param);
}

void sys_glPixelTransferi (GLenum pname,  GLint param)
{
	(*orig_glPixelTransferi) (pname, param);
}

void sys_glPixelZoom (GLfloat xfactor,  GLfloat yfactor)
{
	(*orig_glPixelZoom) (xfactor, yfactor);
}

void sys_glPointSize (GLfloat size)
{
	(*orig_glPointSize) (size);
}

void sys_glPolygonMode (GLenum face,  GLenum mode)
{
	(*orig_glPolygonMode) (face, mode);
}

void sys_glPolygonOffset (GLfloat factor,  GLfloat units)
{
	(*orig_glPolygonOffset) (factor, units);
}

void sys_glPolygonStipple (const GLubyte *mask)
{
	(*orig_glPolygonStipple) (mask);
}

void sys_glPopAttrib (void)
{
	(*orig_glPopAttrib) ();
}

void sys_glPopClientAttrib (void)
{
	(*orig_glPopClientAttrib) ();
}

void sys_glPushName (GLuint name)
{
	(*orig_glPushName) (name);
}

void sys_glRasterPos2d (GLdouble x,  GLdouble y)
{
	(*orig_glRasterPos2d) (x, y);
}

void sys_glRasterPos2dv (const GLdouble *v)
{
	(*orig_glRasterPos2dv) (v);
}

void sys_glRasterPos2f (GLfloat x,  GLfloat y)
{
	(*orig_glRasterPos2f) (x, y);
}

void sys_glRasterPos2fv (const GLfloat *v)
{
	(*orig_glRasterPos2fv) (v);
}

void sys_glRasterPos2i (GLint x,  GLint y)
{
	(*orig_glRasterPos2i) (x, y);
}

void sys_glRasterPos2iv (const GLint *v)
{
	(*orig_glRasterPos2iv) (v);
}

void sys_glRasterPos2s (GLshort x,  GLshort y)
{
	(*orig_glRasterPos2s) (x, y);
}

void sys_glRasterPos2sv (const GLshort *v)
{
	(*orig_glRasterPos2sv) (v);
}

void sys_glRasterPos3d (GLdouble x,  GLdouble y,  GLdouble z)
{
	(*orig_glRasterPos3d) (x, y, z);
}

void sys_glRasterPos3dv (const GLdouble *v)
{
	(*orig_glRasterPos3dv) (v);
}

void sys_glRasterPos3f (GLfloat x,  GLfloat y,  GLfloat z)
{
	(*orig_glRasterPos3f) (x, y, z);
}

void sys_glRasterPos3fv (const GLfloat *v)
{
	(*orig_glRasterPos3fv) (v);
}

void sys_glRasterPos3i (GLint x,  GLint y,  GLint z)
{
	(*orig_glRasterPos3i) (x, y, z);
}

void sys_glRasterPos3iv (const GLint *v)
{
	(*orig_glRasterPos3iv) (v);
}

void sys_glRasterPos3s (GLshort x,  GLshort y,  GLshort z)
{
	(*orig_glRasterPos3s) (x, y, z);
}

void sys_glRasterPos3sv (const GLshort *v)
{
	(*orig_glRasterPos3sv) (v);
}

void sys_glRasterPos4d (GLdouble x,  GLdouble y,  GLdouble z,  GLdouble w)
{
	(*orig_glRasterPos4d) (x, y, z, w);
}

void sys_glRasterPos4dv (const GLdouble *v)
{
	(*orig_glRasterPos4dv) (v);
}

void sys_glRasterPos4f (GLfloat x,  GLfloat y,  GLfloat z,  GLfloat w)
{
	(*orig_glRasterPos4f) (x, y, z, w);
}

void sys_glRasterPos4fv (const GLfloat *v)
{
	(*orig_glRasterPos4fv) (v);
}

void sys_glRasterPos4i (GLint x,  GLint y,  GLint z,  GLint w)
{
	(*orig_glRasterPos4i) (x, y, z, w);
}

void sys_glRasterPos4iv (const GLint *v)
{
	(*orig_glRasterPos4iv) (v);
}

void sys_glRasterPos4s (GLshort x,  GLshort y,  GLshort z,  GLshort w)
{
	(*orig_glRasterPos4s) (x, y, z, w);
}

void sys_glRasterPos4sv (const GLshort *v)
{
	(*orig_glRasterPos4sv) (v);
}

void sys_glReadBuffer (GLenum mode)
{
	(*orig_glReadBuffer) (mode);
}

void sys_glReadPixels (GLint x,  GLint y,  GLsizei width,  GLsizei height,  GLenum format,  GLenum type,  GLvoid *pixels)
{
	(*orig_glReadPixels) (x, y, width, height, format, type, pixels);
}

void sys_glRectd (GLdouble x1,  GLdouble y1,  GLdouble x2,  GLdouble y2)
{
	(*orig_glRectd) (x1, y1, x2, y2);
}

void sys_glRectdv (const GLdouble *v1,  const GLdouble *v2)
{
	(*orig_glRectdv) (v1, v2);
}

void sys_glRectf (GLfloat x1,  GLfloat y1,  GLfloat x2,  GLfloat y2)
{
	(*orig_glRectf) (x1, y1, x2, y2);
}

void sys_glRectfv (const GLfloat *v1,  const GLfloat *v2)
{
	(*orig_glRectfv) (v1, v2);
}

void sys_glRecti (GLint x1,  GLint y1,  GLint x2,  GLint y2)
{
	(*orig_glRecti) (x1, y1, x2, y2);
}

void sys_glRectiv (const GLint *v1,  const GLint *v2)
{
	(*orig_glRectiv) (v1, v2);
}

void sys_glRects (GLshort x1,  GLshort y1,  GLshort x2,  GLshort y2)
{
	(*orig_glRects) (x1, y1, x2, y2);
}

void sys_glRectsv (const GLshort *v1,  const GLshort *v2)
{
	(*orig_glRectsv) (v1, v2);
}

GLint sys_glRenderMode (GLenum mode)
{
	return (*orig_glRenderMode) (mode);
}

void sys_glRotated (GLdouble angle,  GLdouble x,  GLdouble y,  GLdouble z)
{
	(*orig_glRotated) (angle, x, y, z);
}

void sys_glScaled (GLdouble x,  GLdouble y,  GLdouble z)
{
	(*orig_glScaled) (x, y, z);
}

void sys_glScalef (GLfloat x,  GLfloat y,  GLfloat z)
{
	(*orig_glScalef) (x, y, z);
}

void sys_glScissor (GLint x,  GLint y,  GLsizei width,  GLsizei height)
{
	(*orig_glScissor) (x, y, width, height);
}

void sys_glSelectBuffer (GLsizei size,  GLuint *buffer)
{
	(*orig_glSelectBuffer) (size, buffer);
}

void sys_glStencilFunc (GLenum func,  GLint ref,  GLuint mask)
{
	(*orig_glStencilFunc) (func, ref, mask);
}

void sys_glStencilMask (GLuint mask)
{
	(*orig_glStencilMask) (mask);
}

void sys_glStencilOp (GLenum fail,  GLenum zfail,  GLenum zpass)
{
	(*orig_glStencilOp) (fail, zfail, zpass);
}

void sys_glTexCoord1d (GLdouble s)
{
	(*orig_glTexCoord1d) (s);
}

void sys_glTexCoord1dv (const GLdouble *v)
{
	(*orig_glTexCoord1dv) (v);
}

void sys_glTexCoord1f (GLfloat s)
{
	(*orig_glTexCoord1f) (s);
}

void sys_glTexCoord1fv (const GLfloat *v)
{
	(*orig_glTexCoord1fv) (v);
}

void sys_glTexCoord1i (GLint s)
{
	(*orig_glTexCoord1i) (s);
}

void sys_glTexCoord1iv (const GLint *v)
{
	(*orig_glTexCoord1iv) (v);
}

void sys_glTexCoord1s (GLshort s)
{
	(*orig_glTexCoord1s) (s);
}

void sys_glTexCoord1sv (const GLshort *v)
{
	(*orig_glTexCoord1sv) (v);
}

void sys_glTexCoord2d (GLdouble s,  GLdouble t)
{
	(*orig_glTexCoord2d) (s, t);
}

void sys_glTexCoord2dv (const GLdouble *v)
{
	(*orig_glTexCoord2dv) (v);
}

void sys_glTexCoord2fv (const GLfloat *v)
{
	(*orig_glTexCoord2fv) (v);
}

void sys_glTexCoord2i (GLint s,  GLint t)
{
	(*orig_glTexCoord2i) (s, t);
}

void sys_glTexCoord2iv (const GLint *v)
{
	(*orig_glTexCoord2iv) (v);
}

void sys_glTexCoord2s (GLshort s,  GLshort t)
{
	(*orig_glTexCoord2s) (s, t);
}

void sys_glTexCoord2sv (const GLshort *v)
{
	(*orig_glTexCoord2sv) (v);
}

void sys_glTexCoord3d (GLdouble s,  GLdouble t,  GLdouble r)
{
	(*orig_glTexCoord3d) (s, t, r);
}

void sys_glTexCoord3dv (const GLdouble *v)
{
	(*orig_glTexCoord3dv) (v);
}

void sys_glTexCoord3f (GLfloat s,  GLfloat t,  GLfloat r)
{
	(*orig_glTexCoord3f) (s, t, r);
}

void sys_glTexCoord3fv (const GLfloat *v)
{
	(*orig_glTexCoord3fv) (v);
}

void sys_glTexCoord3i (GLint s,  GLint t,  GLint r)
{
	(*orig_glTexCoord3i) (s, t, r);
}

void sys_glTexCoord3iv (const GLint *v)
{
	(*orig_glTexCoord3iv) (v);
}

void sys_glTexCoord3s (GLshort s,  GLshort t,  GLshort r)
{
	(*orig_glTexCoord3s) (s, t, r);
}

void sys_glTexCoord3sv (const GLshort *v)
{
	(*orig_glTexCoord3sv) (v);
}

void sys_glTexCoord4d (GLdouble s,  GLdouble t,  GLdouble r,  GLdouble q)
{
	(*orig_glTexCoord4d) (s, t, r, q);
}

void sys_glTexCoord4dv (const GLdouble *v)
{
	(*orig_glTexCoord4dv) (v);
}

void sys_glTexCoord4f (GLfloat s,  GLfloat t,  GLfloat r,  GLfloat q)
{
	(*orig_glTexCoord4f) (s, t, r, q);
}

void sys_glTexCoord4fv (const GLfloat *v)
{
	(*orig_glTexCoord4fv) (v);
}

void sys_glTexCoord4i (GLint s,  GLint t,  GLint r,  GLint q)
{
	(*orig_glTexCoord4i) (s, t, r, q);
}

void sys_glTexCoord4iv (const GLint *v)
{
	(*orig_glTexCoord4iv) (v);
}

void sys_glTexCoord4s (GLshort s,  GLshort t,  GLshort r,  GLshort q)
{
	(*orig_glTexCoord4s) (s, t, r, q);
}

void sys_glTexCoord4sv (const GLshort *v)
{
	(*orig_glTexCoord4sv) (v);
}

void sys_glTexCoordPointer (GLint size,  GLenum type,  GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glTexCoordPointer) (size, type, stride, pointer);
}

void sys_glTexEnvfv (GLenum target,  GLenum pname,  const GLfloat *params)
{
	(*orig_glTexEnvfv) (target, pname, params);
}

void sys_glTexEnvi (GLenum target,  GLenum pname,  GLint param)
{
	(*orig_glTexEnvi) (target, pname, param);
}

void sys_glTexEnviv (GLenum target,  GLenum pname,  const GLint *params)
{
	(*orig_glTexEnviv) (target, pname, params);
}

void sys_glTexGend (GLenum coord,  GLenum pname,  GLdouble param)
{
	(*orig_glTexGend) (coord, pname, param);
}

void sys_glTexGendv (GLenum coord,  GLenum pname,  const GLdouble *params)
{
	(*orig_glTexGendv) (coord, pname, params);
}

void sys_glTexGenf (GLenum coord,  GLenum pname,  GLfloat param)
{
	(*orig_glTexGenf) (coord, pname, param);
}

void sys_glTexGenfv (GLenum coord,  GLenum pname,  const GLfloat *params)
{
	(*orig_glTexGenfv) (coord, pname, params);
}

void sys_glTexGeni (GLenum coord,  GLenum pname,  GLint param)
{
	(*orig_glTexGeni) (coord, pname, param);
}

void sys_glTexGeniv (GLenum coord,  GLenum pname,  const GLint *params)
{
	(*orig_glTexGeniv) (coord, pname, params);
}

void sys_glTexImage1D (GLenum target,  GLint level,  GLint internalformat,  GLsizei width,  GLint border,  GLenum format,  GLenum type,  const GLvoid *pixels)
{
	(*orig_glTexImage1D) (target, level, internalformat, width, border, format, type, pixels);
}

void sys_glTexParameterfv (GLenum target,  GLenum pname,  const GLfloat *params)
{
	(*orig_glTexParameterfv) (target, pname, params);
}

void sys_glTexParameteri (GLenum target,  GLenum pname,  GLint param)
{
	(*orig_glTexParameteri) (target, pname, param);
}

void sys_glTexParameteriv (GLenum target,  GLenum pname,  const GLint *params)
{
	(*orig_glTexParameteriv) (target, pname, params);
}

void sys_glTexSubImage1D (GLenum target,  GLint level,  GLint xoffset,  GLsizei width,  GLenum format,  GLenum type,  const GLvoid *pixels)
{
	(*orig_glTexSubImage1D) (target, level, xoffset, width, format, type, pixels);
}

void sys_glTexSubImage2D (GLenum target,  GLint level,  GLint xoffset,  GLint yoffset,  GLsizei width,  GLsizei height,  GLenum format,  GLenum type,  const GLvoid *pixels)
{
	(*orig_glTexSubImage2D) (target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void sys_glVertex2d (GLdouble x,  GLdouble y)
{
	(*orig_glVertex2d) (x, y);
}

void sys_glVertex2dv (const GLdouble *v)
{
	(*orig_glVertex2dv) (v);
}

void sys_glVertex2fv (const GLfloat *v)
{
	(*orig_glVertex2fv) (v);
}

void sys_glVertex2i (GLint x,  GLint y)
{
	(*orig_glVertex2i) (x, y);
}

void sys_glVertex2iv (const GLint *v)
{
	(*orig_glVertex2iv) (v);
}

void sys_glVertex2s (GLshort x,  GLshort y)
{
	(*orig_glVertex2s) (x, y);
}

void sys_glVertex2sv (const GLshort *v)
{
	(*orig_glVertex2sv) (v);
}

void sys_glVertex3d (GLdouble x,  GLdouble y,  GLdouble z)
{
	(*orig_glVertex3d) (x, y, z);
}

void sys_glVertex3dv (const GLdouble *v)
{
	(*orig_glVertex3dv) (v);
}

void sys_glVertex3i (GLint x,  GLint y,  GLint z)
{
	(*orig_glVertex3i) (x, y, z);
}

void sys_glVertex3iv (const GLint *v)
{
	(*orig_glVertex3iv) (v);
}

void sys_glVertex3s (GLshort x,  GLshort y,  GLshort z)
{
	(*orig_glVertex3s) (x, y, z);
}

void sys_glVertex3sv (const GLshort *v)
{
	(*orig_glVertex3sv) (v);
}

void sys_glVertex4d (GLdouble x,  GLdouble y,  GLdouble z,  GLdouble w)
{
	(*orig_glVertex4d) (x, y, z, w);
}

void sys_glVertex4dv (const GLdouble *v)
{
	(*orig_glVertex4dv) (v);
}

void sys_glVertex4f (GLfloat x,  GLfloat y,  GLfloat z,  GLfloat w)
{
	(*orig_glVertex4f) (x, y, z, w);
}

void sys_glVertex4fv (const GLfloat *v)
{
	(*orig_glVertex4fv) (v);
}

void sys_glVertex4i (GLint x,  GLint y,  GLint z,  GLint w)
{
	(*orig_glVertex4i) (x, y, z, w);
}

void sys_glVertex4iv (const GLint *v)
{
	(*orig_glVertex4iv) (v);
}

void sys_glVertex4s (GLshort x,  GLshort y,  GLshort z,  GLshort w)
{
	(*orig_glVertex4s) (x, y, z, w);
}

void sys_glVertex4sv (const GLshort *v)
{
	(*orig_glVertex4sv) (v);
}

void sys_glVertexPointer (GLint size,  GLenum type,  GLsizei stride,  const GLvoid *pointer)
{
	(*orig_glVertexPointer) (size, type, stride, pointer);
}

__declspec(naked) void sys_wglChoosePixelFormat(void)
{
	__asm
	{
		jmp [orig_wglChoosePixelFormat]
	}
}

__declspec(naked) void sys_wglCopyContext(void)
{
	__asm
	{
		jmp [orig_wglCopyContext]
	}
}

__declspec(naked) void sys_wglCreateContext(void)
{
	__asm
	{
		jmp [orig_wglCreateContext]
	}
}

__declspec(naked) void sys_wglCreateLayerContext(void)
{
	__asm
	{
		jmp [orig_wglCreateLayerContext]
	}
}

__declspec(naked) void sys_wglDeleteContext(void)
{
	__asm
	{
		jmp [orig_wglDeleteContext]
	}
}

__declspec(naked) void sys_wglDescribeLayerPlane(void)
{
	__asm
	{
		jmp [orig_wglDescribeLayerPlane]
	}
}

__declspec(naked) void sys_wglDescribePixelFormat(void)
{
	__asm
	{
		jmp [orig_wglDescribePixelFormat]
	}
}

__declspec(naked) void sys_wglGetCurrentContext(void)
{
	__asm
	{
		jmp [orig_wglGetCurrentContext]
	}
}

__declspec(naked) void sys_wglGetCurrentDC(void)
{
	__asm
	{
		jmp [orig_wglGetCurrentDC]
	}
}

__declspec(naked) void sys_wglGetDefaultProcAddress(void)
{
	__asm
	{
		jmp [orig_wglGetDefaultProcAddress]
	}
}

__declspec(naked) void sys_wglGetLayerPaletteEntries(void)
{
	__asm
	{
		jmp [orig_wglGetLayerPaletteEntries]
	}
}

__declspec(naked) void sys_wglGetPixelFormat(void)
{
	__asm
	{
		jmp [orig_wglGetPixelFormat]
	}
}

__declspec(naked) void sys_wglMakeCurrent(void)
{
	__asm
	{
		jmp [orig_wglMakeCurrent]
	}
}

__declspec(naked) void sys_wglRealizeLayerPalette(void)
{
	__asm
	{
		jmp [orig_wglRealizeLayerPalette]
	}
}

__declspec(naked) void sys_wglSetLayerPaletteEntries(void)
{
	__asm
	{
		jmp [orig_wglSetLayerPaletteEntries]
	}
}

__declspec(naked) void sys_wglSetPixelFormat(void)
{
	__asm
	{
		jmp [orig_wglSetPixelFormat]
	}
}

__declspec(naked) void sys_wglShareLists(void)
{
	__asm
	{
		jmp [orig_wglShareLists]
	}
}

/*
// ASM Variant
__declspec(naked) void sys_wglSwapLayerBuffers(void)
{
	__asm
	{
		jmp [orig_wglSwapLayerBuffers]
	}
}
*/

__declspec(naked) void sys_wglUseFontBitmapsA(void)
{
	__asm
	{
		jmp [orig_wglUseFontBitmapsA]
	}
}

__declspec(naked) void sys_wglUseFontBitmapsW(void)
{
	__asm
	{
		jmp [orig_wglUseFontBitmapsW]
	}
}

__declspec(naked) void sys_wglUseFontOutlinesA(void)
{
	__asm
	{
		jmp [orig_wglUseFontOutlinesA]
	}
}

__declspec(naked) void sys_wglUseFontOutlinesW(void)
{
	__asm
	{
		jmp [orig_wglUseFontOutlinesW]
	}
}

BOOL Init (void)
{
	// Remove logfile.
	remove("3DVisionWrapper.log");

	if ( !hOriginalDll )
	{
		char sysdir[MAX_PATH];

        GetSystemDirectory (sysdir, sizeof(sysdir));
        strcat_s (sysdir, "\\");
        strcat_s (sysdir, ORIGINAL_DLL);

		hOriginalDll = LoadLibrary(sysdir);
				
		if ( !hOriginalDll )
		{
			add_log ("Couldn't load the library %s: %d", sysdir, GetLastError());
			return FALSE;
		}
	}
	
	if ( (orig_glAccum = (func_glAccum_t) GetProcAddress (hOriginalDll, "glAccum")) == NULL )
	{
		add_log ("Couldn't found a prototype for glAccum()");
		return FALSE;
	}

	if ( (orig_glAlphaFunc = (func_glAlphaFunc_t) GetProcAddress (hOriginalDll, "glAlphaFunc")) == NULL )
	{
		add_log ("Couldn't found a prototype for glAlphaFunc()");
		return FALSE;
	}

	if ( (orig_glAreTexturesResident = (func_glAreTexturesResident_t) GetProcAddress (hOriginalDll, "glAreTexturesResident")) == NULL )
	{
		add_log ("Couldn't found a prototype for glAreTexturesResident()");
		return FALSE;
	}

	if ( (orig_glArrayElement = (func_glArrayElement_t) GetProcAddress (hOriginalDll, "glArrayElement")) == NULL )
	{
		add_log ("Couldn't found a prototype for glArrayElement()");
		return FALSE;
	}

	if ( (orig_glBegin = (func_glBegin_t) GetProcAddress (hOriginalDll, "glBegin")) == NULL )
	{
		add_log ("Couldn't found a prototype for glBegin()");
		return FALSE;
	}

	if ( (orig_glBindTexture = (func_glBindTexture_t) GetProcAddress (hOriginalDll, "glBindTexture")) == NULL )
	{
		add_log ("Couldn't found a prototype for glBindTexture()");
		return FALSE;
	}

	if ( (orig_glBitmap = (func_glBitmap_t) GetProcAddress (hOriginalDll, "glBitmap")) == NULL )
	{
		add_log ("Couldn't found a prototype for glBitmap()");
		return FALSE;
	}

	if ( (orig_glBlendFunc = (func_glBlendFunc_t) GetProcAddress (hOriginalDll, "glBlendFunc")) == NULL )
	{
		add_log ("Couldn't found a prototype for glBlendFunc()");
		return FALSE;
	}

	if ( (orig_glCallList = (func_glCallList_t) GetProcAddress (hOriginalDll, "glCallList")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCallList()");
		return FALSE;
	}

	if ( (orig_glCallLists = (func_glCallLists_t) GetProcAddress (hOriginalDll, "glCallLists")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCallLists()");
		return FALSE;
	}

	if ( (orig_glClear = (func_glClear_t) GetProcAddress (hOriginalDll, "glClear")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClear()");
		return FALSE;
	}

	if ( (orig_glClearAccum = (func_glClearAccum_t) GetProcAddress (hOriginalDll, "glClearAccum")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClearAccum()");
		return FALSE;
	}

	if ( (orig_glClearColor = (func_glClearColor_t) GetProcAddress (hOriginalDll, "glClearColor")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClearColor()");
		return FALSE;
	}

	if ( (orig_glClearDepth = (func_glClearDepth_t) GetProcAddress (hOriginalDll, "glClearDepth")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClearDepth()");
		return FALSE;
	}

	if ( (orig_glClearIndex = (func_glClearIndex_t) GetProcAddress (hOriginalDll, "glClearIndex")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClearIndex()");
		return FALSE;
	}

	if ( (orig_glClearStencil = (func_glClearStencil_t) GetProcAddress (hOriginalDll, "glClearStencil")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClearStencil()");
		return FALSE;
	}

	if ( (orig_glClipPlane = (func_glClipPlane_t) GetProcAddress (hOriginalDll, "glClipPlane")) == NULL )
	{
		add_log ("Couldn't found a prototype for glClipPlane()");
		return FALSE;
	}

	if ( (orig_glColor3b = (func_glColor3b_t) GetProcAddress (hOriginalDll, "glColor3b")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3b()");
		return FALSE;
	}

	if ( (orig_glColor3bv = (func_glColor3bv_t) GetProcAddress (hOriginalDll, "glColor3bv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3bv()");
		return FALSE;
	}

	if ( (orig_glColor3d = (func_glColor3d_t) GetProcAddress (hOriginalDll, "glColor3d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3d()");
		return FALSE;
	}

	if ( (orig_glColor3dv = (func_glColor3dv_t) GetProcAddress (hOriginalDll, "glColor3dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3dv()");
		return FALSE;
	}

	if ( (orig_glColor3f = (func_glColor3f_t) GetProcAddress (hOriginalDll, "glColor3f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3f()");
		return FALSE;
	}

	if ( (orig_glColor3fv = (func_glColor3fv_t) GetProcAddress (hOriginalDll, "glColor3fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3fv()");
		return FALSE;
	}

	if ( (orig_glColor3i = (func_glColor3i_t) GetProcAddress (hOriginalDll, "glColor3i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3i()");
		return FALSE;
	}

	if ( (orig_glColor3iv = (func_glColor3iv_t) GetProcAddress (hOriginalDll, "glColor3iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3iv()");
		return FALSE;
	}

	if ( (orig_glColor3s = (func_glColor3s_t) GetProcAddress (hOriginalDll, "glColor3s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3s()");
		return FALSE;
	}

	if ( (orig_glColor3sv = (func_glColor3sv_t) GetProcAddress (hOriginalDll, "glColor3sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3sv()");
		return FALSE;
	}

	if ( (orig_glColor3ub = (func_glColor3ub_t) GetProcAddress (hOriginalDll, "glColor3ub")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3ub()");
		return FALSE;
	}

	if ( (orig_glColor3ubv = (func_glColor3ubv_t) GetProcAddress (hOriginalDll, "glColor3ubv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3ubv()");
		return FALSE;
	}

	if ( (orig_glColor3ui = (func_glColor3ui_t) GetProcAddress (hOriginalDll, "glColor3ui")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3ui()");
		return FALSE;
	}

	if ( (orig_glColor3uiv = (func_glColor3uiv_t) GetProcAddress (hOriginalDll, "glColor3uiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3uiv()");
		return FALSE;
	}

	if ( (orig_glColor3us = (func_glColor3us_t) GetProcAddress (hOriginalDll, "glColor3us")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3us()");
		return FALSE;
	}

	if ( (orig_glColor3usv = (func_glColor3usv_t) GetProcAddress (hOriginalDll, "glColor3usv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor3usv()");
		return FALSE;
	}

	if ( (orig_glColor4b = (func_glColor4b_t) GetProcAddress (hOriginalDll, "glColor4b")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4b()");
		return FALSE;
	}

	if ( (orig_glColor4bv = (func_glColor4bv_t) GetProcAddress (hOriginalDll, "glColor4bv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4bv()");
		return FALSE;
	}

	if ( (orig_glColor4d = (func_glColor4d_t) GetProcAddress (hOriginalDll, "glColor4d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4d()");
		return FALSE;
	}

	if ( (orig_glColor4dv = (func_glColor4dv_t) GetProcAddress (hOriginalDll, "glColor4dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4dv()");
		return FALSE;
	}

	if ( (orig_glColor4f = (func_glColor4f_t) GetProcAddress (hOriginalDll, "glColor4f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4f()");
		return FALSE;
	}

	if ( (orig_glColor4fv = (func_glColor4fv_t) GetProcAddress (hOriginalDll, "glColor4fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4fv()");
		return FALSE;
	}

	if ( (orig_glColor4i = (func_glColor4i_t) GetProcAddress (hOriginalDll, "glColor4i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4i()");
		return FALSE;
	}

	if ( (orig_glColor4iv = (func_glColor4iv_t) GetProcAddress (hOriginalDll, "glColor4iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4iv()");
		return FALSE;
	}

	if ( (orig_glColor4s = (func_glColor4s_t) GetProcAddress (hOriginalDll, "glColor4s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4s()");
		return FALSE;
	}

	if ( (orig_glColor4sv = (func_glColor4sv_t) GetProcAddress (hOriginalDll, "glColor4sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4sv()");
		return FALSE;
	}

	if ( (orig_glColor4ub = (func_glColor4ub_t) GetProcAddress (hOriginalDll, "glColor4ub")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4ub()");
		return FALSE;
	}

	if ( (orig_glColor4ubv = (func_glColor4ubv_t) GetProcAddress (hOriginalDll, "glColor4ubv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4ubv()");
		return FALSE;
	}

	if ( (orig_glColor4ui = (func_glColor4ui_t) GetProcAddress (hOriginalDll, "glColor4ui")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4ui()");
		return FALSE;
	}

	if ( (orig_glColor4uiv = (func_glColor4uiv_t) GetProcAddress (hOriginalDll, "glColor4uiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4uiv()");
		return FALSE;
	}

	if ( (orig_glColor4us = (func_glColor4us_t) GetProcAddress (hOriginalDll, "glColor4us")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4us()");
		return FALSE;
	}

	if ( (orig_glColor4usv = (func_glColor4usv_t) GetProcAddress (hOriginalDll, "glColor4usv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColor4usv()");
		return FALSE;
	}

	if ( (orig_glColorMask = (func_glColorMask_t) GetProcAddress (hOriginalDll, "glColorMask")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColorMask()");
		return FALSE;
	}

	if ( (orig_glColorMaterial = (func_glColorMaterial_t) GetProcAddress (hOriginalDll, "glColorMaterial")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColorMaterial()");
		return FALSE;
	}

	if ( (orig_glColorPointer = (func_glColorPointer_t) GetProcAddress (hOriginalDll, "glColorPointer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glColorPointer()");
		return FALSE;
	}

	if ( (orig_glCopyPixels = (func_glCopyPixels_t) GetProcAddress (hOriginalDll, "glCopyPixels")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCopyPixels()");
		return FALSE;
	}

	if ( (orig_glCopyTexImage1D = (func_glCopyTexImage1D_t) GetProcAddress (hOriginalDll, "glCopyTexImage1D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCopyTexImage1D()");
		return FALSE;
	}

	if ( (orig_glCopyTexImage2D = (func_glCopyTexImage2D_t) GetProcAddress (hOriginalDll, "glCopyTexImage2D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCopyTexImage2D()");
		return FALSE;
	}

	if ( (orig_glCopyTexSubImage1D = (func_glCopyTexSubImage1D_t) GetProcAddress (hOriginalDll, "glCopyTexSubImage1D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCopyTexSubImage1D()");
		return FALSE;
	}

	if ( (orig_glCopyTexSubImage2D = (func_glCopyTexSubImage2D_t) GetProcAddress (hOriginalDll, "glCopyTexSubImage2D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCopyTexSubImage2D()");
		return FALSE;
	}

	if ( (orig_glCullFace = (func_glCullFace_t) GetProcAddress (hOriginalDll, "glCullFace")) == NULL )
	{
		add_log ("Couldn't found a prototype for glCullFace()");
		return FALSE;
	}

	if ( (orig_glDebugEntry = (func_glDebugEntry_t) GetProcAddress (hOriginalDll, "glDebugEntry")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDebugEntry()");
		return FALSE;
	}

	if ( (orig_glDeleteLists = (func_glDeleteLists_t) GetProcAddress (hOriginalDll, "glDeleteLists")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDeleteLists()");
		return FALSE;
	}

	if ( (orig_glDeleteTextures = (func_glDeleteTextures_t) GetProcAddress (hOriginalDll, "glDeleteTextures")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDeleteTextures()");
		return FALSE;
	}

	if ( (orig_glDepthFunc = (func_glDepthFunc_t) GetProcAddress (hOriginalDll, "glDepthFunc")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDepthFunc()");
		return FALSE;
	}

	if ( (orig_glDepthMask = (func_glDepthMask_t) GetProcAddress (hOriginalDll, "glDepthMask")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDepthMask()");
		return FALSE;
	}

	if ( (orig_glDepthRange = (func_glDepthRange_t) GetProcAddress (hOriginalDll, "glDepthRange")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDepthRange()");
		return FALSE;
	}

	if ( (orig_glDisable = (func_glDisable_t) GetProcAddress (hOriginalDll, "glDisable")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDisable()");
		return FALSE;
	}

	if ( (orig_glDisableClientState = (func_glDisableClientState_t) GetProcAddress (hOriginalDll, "glDisableClientState")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDisableClientState()");
		return FALSE;
	}

	if ( (orig_glDrawArrays = (func_glDrawArrays_t) GetProcAddress (hOriginalDll, "glDrawArrays")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDrawArrays()");
		return FALSE;
	}

	if ( (orig_glDrawBuffer = (func_glDrawBuffer_t) GetProcAddress (hOriginalDll, "glDrawBuffer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDrawBuffer()");
		return FALSE;
	}

	if ( (orig_glDrawElements = (func_glDrawElements_t) GetProcAddress (hOriginalDll, "glDrawElements")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDrawElements()");
		return FALSE;
	}

	if ( (orig_glDrawPixels = (func_glDrawPixels_t) GetProcAddress (hOriginalDll, "glDrawPixels")) == NULL )
	{
		add_log ("Couldn't found a prototype for glDrawPixels()");
		return FALSE;
	}

	if ( (orig_glEdgeFlag = (func_glEdgeFlag_t) GetProcAddress (hOriginalDll, "glEdgeFlag")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEdgeFlag()");
		return FALSE;
	}

	if ( (orig_glEdgeFlagPointer = (func_glEdgeFlagPointer_t) GetProcAddress (hOriginalDll, "glEdgeFlagPointer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEdgeFlagPointer()");
		return FALSE;
	}

	if ( (orig_glEdgeFlagv = (func_glEdgeFlagv_t) GetProcAddress (hOriginalDll, "glEdgeFlagv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEdgeFlagv()");
		return FALSE;
	}

	if ( (orig_glEnable = (func_glEnable_t) GetProcAddress (hOriginalDll, "glEnable")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEnable()");
		return FALSE;
	}

	if ( (orig_glEnableClientState = (func_glEnableClientState_t) GetProcAddress (hOriginalDll, "glEnableClientState")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEnableClientState()");
		return FALSE;
	}

	if ( (orig_glEnd = (func_glEnd_t) GetProcAddress (hOriginalDll, "glEnd")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEnd()");
		return FALSE;
	}

	if ( (orig_glEndList = (func_glEndList_t) GetProcAddress (hOriginalDll, "glEndList")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEndList()");
		return FALSE;
	}

	if ( (orig_glEvalCoord1d = (func_glEvalCoord1d_t) GetProcAddress (hOriginalDll, "glEvalCoord1d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord1d()");
		return FALSE;
	}

	if ( (orig_glEvalCoord1dv = (func_glEvalCoord1dv_t) GetProcAddress (hOriginalDll, "glEvalCoord1dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord1dv()");
		return FALSE;
	}

	if ( (orig_glEvalCoord1f = (func_glEvalCoord1f_t) GetProcAddress (hOriginalDll, "glEvalCoord1f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord1f()");
		return FALSE;
	}

	if ( (orig_glEvalCoord1fv = (func_glEvalCoord1fv_t) GetProcAddress (hOriginalDll, "glEvalCoord1fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord1fv()");
		return FALSE;
	}

	if ( (orig_glEvalCoord2d = (func_glEvalCoord2d_t) GetProcAddress (hOriginalDll, "glEvalCoord2d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord2d()");
		return FALSE;
	}

	if ( (orig_glEvalCoord2dv = (func_glEvalCoord2dv_t) GetProcAddress (hOriginalDll, "glEvalCoord2dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord2dv()");
		return FALSE;
	}

	if ( (orig_glEvalCoord2f = (func_glEvalCoord2f_t) GetProcAddress (hOriginalDll, "glEvalCoord2f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord2f()");
		return FALSE;
	}

	if ( (orig_glEvalCoord2fv = (func_glEvalCoord2fv_t) GetProcAddress (hOriginalDll, "glEvalCoord2fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalCoord2fv()");
		return FALSE;
	}

	if ( (orig_glEvalMesh1 = (func_glEvalMesh1_t) GetProcAddress (hOriginalDll, "glEvalMesh1")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalMesh1()");
		return FALSE;
	}

	if ( (orig_glEvalMesh2 = (func_glEvalMesh2_t) GetProcAddress (hOriginalDll, "glEvalMesh2")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalMesh2()");
		return FALSE;
	}

	if ( (orig_glEvalPoint1 = (func_glEvalPoint1_t) GetProcAddress (hOriginalDll, "glEvalPoint1")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalPoint1()");
		return FALSE;
	}

	if ( (orig_glEvalPoint2 = (func_glEvalPoint2_t) GetProcAddress (hOriginalDll, "glEvalPoint2")) == NULL )
	{
		add_log ("Couldn't found a prototype for glEvalPoint2()");
		return FALSE;
	}

	if ( (orig_glFeedbackBuffer = (func_glFeedbackBuffer_t) GetProcAddress (hOriginalDll, "glFeedbackBuffer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFeedbackBuffer()");
		return FALSE;
	}

	if ( (orig_glFinish = (func_glFinish_t) GetProcAddress (hOriginalDll, "glFinish")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFinish()");
		return FALSE;
	}

	if ( (orig_glFlush = (func_glFlush_t) GetProcAddress (hOriginalDll, "glFlush")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFlush()");
		return FALSE;
	}

	if ( (orig_glFogf = (func_glFogf_t) GetProcAddress (hOriginalDll, "glFogf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFogf()");
		return FALSE;
	}

	if ( (orig_glFogfv = (func_glFogfv_t) GetProcAddress (hOriginalDll, "glFogfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFogfv()");
		return FALSE;
	}

	if ( (orig_glFogi = (func_glFogi_t) GetProcAddress (hOriginalDll, "glFogi")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFogi()");
		return FALSE;
	}

	if ( (orig_glFogiv = (func_glFogiv_t) GetProcAddress (hOriginalDll, "glFogiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFogiv()");
		return FALSE;
	}

	if ( (orig_glFrontFace = (func_glFrontFace_t) GetProcAddress (hOriginalDll, "glFrontFace")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFrontFace()");
		return FALSE;
	}

	if ( (orig_glFrustum = (func_glFrustum_t) GetProcAddress (hOriginalDll, "glFrustum")) == NULL )
	{
		add_log ("Couldn't found a prototype for glFrustum()");
		return FALSE;
	}

	if ( (orig_glGenLists = (func_glGenLists_t) GetProcAddress (hOriginalDll, "glGenLists")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGenLists()");
		return FALSE;
	}

	if ( (orig_glGenTextures = (func_glGenTextures_t) GetProcAddress (hOriginalDll, "glGenTextures")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGenTextures()");
		return FALSE;
	}

	if ( (orig_glGetBooleanv = (func_glGetBooleanv_t) GetProcAddress (hOriginalDll, "glGetBooleanv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetBooleanv()");
		return FALSE;
	}

	if ( (orig_glGetClipPlane = (func_glGetClipPlane_t) GetProcAddress (hOriginalDll, "glGetClipPlane")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetClipPlane()");
		return FALSE;
	}

	if ( (orig_glGetDoublev = (func_glGetDoublev_t) GetProcAddress (hOriginalDll, "glGetDoublev")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetDoublev()");
		return FALSE;
	}

	if ( (orig_glGetError = (func_glGetError_t) GetProcAddress (hOriginalDll, "glGetError")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetError()");
		return FALSE;
	}

	if ( (orig_glGetFloatv = (func_glGetFloatv_t) GetProcAddress (hOriginalDll, "glGetFloatv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetFloatv()");
		return FALSE;
	}

	if ( (orig_glGetIntegerv = (func_glGetIntegerv_t) GetProcAddress (hOriginalDll, "glGetIntegerv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetIntegerv()");
		return FALSE;
	}

	if ( (orig_glGetLightfv = (func_glGetLightfv_t) GetProcAddress (hOriginalDll, "glGetLightfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetLightfv()");
		return FALSE;
	}

	if ( (orig_glGetLightiv = (func_glGetLightiv_t) GetProcAddress (hOriginalDll, "glGetLightiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetLightiv()");
		return FALSE;
	}

	if ( (orig_glGetMapdv = (func_glGetMapdv_t) GetProcAddress (hOriginalDll, "glGetMapdv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetMapdv()");
		return FALSE;
	}

	if ( (orig_glGetMapfv = (func_glGetMapfv_t) GetProcAddress (hOriginalDll, "glGetMapfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetMapfv()");
		return FALSE;
	}

	if ( (orig_glGetMapiv = (func_glGetMapiv_t) GetProcAddress (hOriginalDll, "glGetMapiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetMapiv()");
		return FALSE;
	}

	if ( (orig_glGetMaterialfv = (func_glGetMaterialfv_t) GetProcAddress (hOriginalDll, "glGetMaterialfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetMaterialfv()");
		return FALSE;
	}

	if ( (orig_glGetMaterialiv = (func_glGetMaterialiv_t) GetProcAddress (hOriginalDll, "glGetMaterialiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetMaterialiv()");
		return FALSE;
	}

	if ( (orig_glGetPixelMapfv = (func_glGetPixelMapfv_t) GetProcAddress (hOriginalDll, "glGetPixelMapfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetPixelMapfv()");
		return FALSE;
	}

	if ( (orig_glGetPixelMapuiv = (func_glGetPixelMapuiv_t) GetProcAddress (hOriginalDll, "glGetPixelMapuiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetPixelMapuiv()");
		return FALSE;
	}

	if ( (orig_glGetPixelMapusv = (func_glGetPixelMapusv_t) GetProcAddress (hOriginalDll, "glGetPixelMapusv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetPixelMapusv()");
		return FALSE;
	}

	if ( (orig_glGetPointerv = (func_glGetPointerv_t) GetProcAddress (hOriginalDll, "glGetPointerv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetPointerv()");
		return FALSE;
	}

	if ( (orig_glGetPolygonStipple = (func_glGetPolygonStipple_t) GetProcAddress (hOriginalDll, "glGetPolygonStipple")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetPolygonStipple()");
		return FALSE;
	}

	if ( (orig_glGetString = (func_glGetString_t) GetProcAddress (hOriginalDll, "glGetString")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetString()");
		return FALSE;
	}

	if ( (orig_glGetTexEnvfv = (func_glGetTexEnvfv_t) GetProcAddress (hOriginalDll, "glGetTexEnvfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexEnvfv()");
		return FALSE;
	}

	if ( (orig_glGetTexEnviv = (func_glGetTexEnviv_t) GetProcAddress (hOriginalDll, "glGetTexEnviv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexEnviv()");
		return FALSE;
	}

	if ( (orig_glGetTexGendv = (func_glGetTexGendv_t) GetProcAddress (hOriginalDll, "glGetTexGendv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexGendv()");
		return FALSE;
	}

	if ( (orig_glGetTexGenfv = (func_glGetTexGenfv_t) GetProcAddress (hOriginalDll, "glGetTexGenfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexGenfv()");
		return FALSE;
	}

	if ( (orig_glGetTexGeniv = (func_glGetTexGeniv_t) GetProcAddress (hOriginalDll, "glGetTexGeniv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexGeniv()");
		return FALSE;
	}

	if ( (orig_glGetTexImage = (func_glGetTexImage_t) GetProcAddress (hOriginalDll, "glGetTexImage")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexImage()");
		return FALSE;
	}

	if ( (orig_glGetTexLevelParameterfv = (func_glGetTexLevelParameterfv_t) GetProcAddress (hOriginalDll, "glGetTexLevelParameterfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexLevelParameterfv()");
		return FALSE;
	}

	if ( (orig_glGetTexLevelParameteriv = (func_glGetTexLevelParameteriv_t) GetProcAddress (hOriginalDll, "glGetTexLevelParameteriv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexLevelParameteriv()");
		return FALSE;
	}

	if ( (orig_glGetTexParameterfv = (func_glGetTexParameterfv_t) GetProcAddress (hOriginalDll, "glGetTexParameterfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexParameterfv()");
		return FALSE;
	}

	if ( (orig_glGetTexParameteriv = (func_glGetTexParameteriv_t) GetProcAddress (hOriginalDll, "glGetTexParameteriv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glGetTexParameteriv()");
		return FALSE;
	}

	if ( (orig_glHint = (func_glHint_t) GetProcAddress (hOriginalDll, "glHint")) == NULL )
	{
		add_log ("Couldn't found a prototype for glHint()");
		return FALSE;
	}

	if ( (orig_glIndexMask = (func_glIndexMask_t) GetProcAddress (hOriginalDll, "glIndexMask")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexMask()");
		return FALSE;
	}

	if ( (orig_glIndexPointer = (func_glIndexPointer_t) GetProcAddress (hOriginalDll, "glIndexPointer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexPointer()");
		return FALSE;
	}

	if ( (orig_glIndexd = (func_glIndexd_t) GetProcAddress (hOriginalDll, "glIndexd")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexd()");
		return FALSE;
	}

	if ( (orig_glIndexdv = (func_glIndexdv_t) GetProcAddress (hOriginalDll, "glIndexdv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexdv()");
		return FALSE;
	}

	if ( (orig_glIndexf = (func_glIndexf_t) GetProcAddress (hOriginalDll, "glIndexf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexf()");
		return FALSE;
	}

	if ( (orig_glIndexfv = (func_glIndexfv_t) GetProcAddress (hOriginalDll, "glIndexfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexfv()");
		return FALSE;
	}

	if ( (orig_glIndexi = (func_glIndexi_t) GetProcAddress (hOriginalDll, "glIndexi")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexi()");
		return FALSE;
	}

	if ( (orig_glIndexiv = (func_glIndexiv_t) GetProcAddress (hOriginalDll, "glIndexiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexiv()");
		return FALSE;
	}

	if ( (orig_glIndexs = (func_glIndexs_t) GetProcAddress (hOriginalDll, "glIndexs")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexs()");
		return FALSE;
	}

	if ( (orig_glIndexsv = (func_glIndexsv_t) GetProcAddress (hOriginalDll, "glIndexsv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexsv()");
		return FALSE;
	}

	if ( (orig_glIndexub = (func_glIndexub_t) GetProcAddress (hOriginalDll, "glIndexub")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexub()");
		return FALSE;
	}

	if ( (orig_glIndexubv = (func_glIndexubv_t) GetProcAddress (hOriginalDll, "glIndexubv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIndexubv()");
		return FALSE;
	}

	if ( (orig_glInitNames = (func_glInitNames_t) GetProcAddress (hOriginalDll, "glInitNames")) == NULL )
	{
		add_log ("Couldn't found a prototype for glInitNames()");
		return FALSE;
	}

	if ( (orig_glInterleavedArrays = (func_glInterleavedArrays_t) GetProcAddress (hOriginalDll, "glInterleavedArrays")) == NULL )
	{
		add_log ("Couldn't found a prototype for glInterleavedArrays()");
		return FALSE;
	}

	if ( (orig_glIsEnabled = (func_glIsEnabled_t) GetProcAddress (hOriginalDll, "glIsEnabled")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIsEnabled()");
		return FALSE;
	}

	if ( (orig_glIsList = (func_glIsList_t) GetProcAddress (hOriginalDll, "glIsList")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIsList()");
		return FALSE;
	}

	if ( (orig_glIsTexture = (func_glIsTexture_t) GetProcAddress (hOriginalDll, "glIsTexture")) == NULL )
	{
		add_log ("Couldn't found a prototype for glIsTexture()");
		return FALSE;
	}

	if ( (orig_glLightModelf = (func_glLightModelf_t) GetProcAddress (hOriginalDll, "glLightModelf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightModelf()");
		return FALSE;
	}

	if ( (orig_glLightModelfv = (func_glLightModelfv_t) GetProcAddress (hOriginalDll, "glLightModelfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightModelfv()");
		return FALSE;
	}

	if ( (orig_glLightModeli = (func_glLightModeli_t) GetProcAddress (hOriginalDll, "glLightModeli")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightModeli()");
		return FALSE;
	}

	if ( (orig_glLightModeliv = (func_glLightModeliv_t) GetProcAddress (hOriginalDll, "glLightModeliv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightModeliv()");
		return FALSE;
	}

	if ( (orig_glLightf = (func_glLightf_t) GetProcAddress (hOriginalDll, "glLightf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightf()");
		return FALSE;
	}

	if ( (orig_glLightfv = (func_glLightfv_t) GetProcAddress (hOriginalDll, "glLightfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightfv()");
		return FALSE;
	}

	if ( (orig_glLighti = (func_glLighti_t) GetProcAddress (hOriginalDll, "glLighti")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLighti()");
		return FALSE;
	}

	if ( (orig_glLightiv = (func_glLightiv_t) GetProcAddress (hOriginalDll, "glLightiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLightiv()");
		return FALSE;
	}

	if ( (orig_glLineStipple = (func_glLineStipple_t) GetProcAddress (hOriginalDll, "glLineStipple")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLineStipple()");
		return FALSE;
	}

	if ( (orig_glLineWidth = (func_glLineWidth_t) GetProcAddress (hOriginalDll, "glLineWidth")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLineWidth()");
		return FALSE;
	}

	if ( (orig_glListBase = (func_glListBase_t) GetProcAddress (hOriginalDll, "glListBase")) == NULL )
	{
		add_log ("Couldn't found a prototype for glListBase()");
		return FALSE;
	}

	if ( (orig_glLoadIdentity = (func_glLoadIdentity_t) GetProcAddress (hOriginalDll, "glLoadIdentity")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLoadIdentity()");
		return FALSE;
	}

	if ( (orig_glLoadMatrixd = (func_glLoadMatrixd_t) GetProcAddress (hOriginalDll, "glLoadMatrixd")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLoadMatrixd()");
		return FALSE;
	}

	if ( (orig_glLoadMatrixf = (func_glLoadMatrixf_t) GetProcAddress (hOriginalDll, "glLoadMatrixf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLoadMatrixf()");
		return FALSE;
	}

	if ( (orig_glLoadName = (func_glLoadName_t) GetProcAddress (hOriginalDll, "glLoadName")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLoadName()");
		return FALSE;
	}

	if ( (orig_glLogicOp = (func_glLogicOp_t) GetProcAddress (hOriginalDll, "glLogicOp")) == NULL )
	{
		add_log ("Couldn't found a prototype for glLogicOp()");
		return FALSE;
	}

	if ( (orig_glMap1d = (func_glMap1d_t) GetProcAddress (hOriginalDll, "glMap1d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMap1d()");
		return FALSE;
	}

	if ( (orig_glMap1f = (func_glMap1f_t) GetProcAddress (hOriginalDll, "glMap1f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMap1f()");
		return FALSE;
	}

	if ( (orig_glMap2d = (func_glMap2d_t) GetProcAddress (hOriginalDll, "glMap2d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMap2d()");
		return FALSE;
	}

	if ( (orig_glMap2f = (func_glMap2f_t) GetProcAddress (hOriginalDll, "glMap2f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMap2f()");
		return FALSE;
	}

	if ( (orig_glMapGrid1d = (func_glMapGrid1d_t) GetProcAddress (hOriginalDll, "glMapGrid1d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMapGrid1d()");
		return FALSE;
	}

	if ( (orig_glMapGrid1f = (func_glMapGrid1f_t) GetProcAddress (hOriginalDll, "glMapGrid1f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMapGrid1f()");
		return FALSE;
	}

	if ( (orig_glMapGrid2d = (func_glMapGrid2d_t) GetProcAddress (hOriginalDll, "glMapGrid2d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMapGrid2d()");
		return FALSE;
	}

	if ( (orig_glMapGrid2f = (func_glMapGrid2f_t) GetProcAddress (hOriginalDll, "glMapGrid2f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMapGrid2f()");
		return FALSE;
	}

	if ( (orig_glMaterialf = (func_glMaterialf_t) GetProcAddress (hOriginalDll, "glMaterialf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMaterialf()");
		return FALSE;
	}

	if ( (orig_glMaterialfv = (func_glMaterialfv_t) GetProcAddress (hOriginalDll, "glMaterialfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMaterialfv()");
		return FALSE;
	}

	if ( (orig_glMateriali = (func_glMateriali_t) GetProcAddress (hOriginalDll, "glMateriali")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMateriali()");
		return FALSE;
	}

	if ( (orig_glMaterialiv = (func_glMaterialiv_t) GetProcAddress (hOriginalDll, "glMaterialiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMaterialiv()");
		return FALSE;
	}

	if ( (orig_glMatrixMode = (func_glMatrixMode_t) GetProcAddress (hOriginalDll, "glMatrixMode")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMatrixMode()");
		return FALSE;
	}

	if ( (orig_glMultMatrixd = (func_glMultMatrixd_t) GetProcAddress (hOriginalDll, "glMultMatrixd")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMultMatrixd()");
		return FALSE;
	}

	if ( (orig_glMultMatrixf = (func_glMultMatrixf_t) GetProcAddress (hOriginalDll, "glMultMatrixf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glMultMatrixf()");
		return FALSE;
	}

	if ( (orig_glNewList = (func_glNewList_t) GetProcAddress (hOriginalDll, "glNewList")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNewList()");
		return FALSE;
	}

	if ( (orig_glNormal3b = (func_glNormal3b_t) GetProcAddress (hOriginalDll, "glNormal3b")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3b()");
		return FALSE;
	}

	if ( (orig_glNormal3bv = (func_glNormal3bv_t) GetProcAddress (hOriginalDll, "glNormal3bv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3bv()");
		return FALSE;
	}

	if ( (orig_glNormal3d = (func_glNormal3d_t) GetProcAddress (hOriginalDll, "glNormal3d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3d()");
		return FALSE;
	}

	if ( (orig_glNormal3dv = (func_glNormal3dv_t) GetProcAddress (hOriginalDll, "glNormal3dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3dv()");
		return FALSE;
	}

	if ( (orig_glNormal3f = (func_glNormal3f_t) GetProcAddress (hOriginalDll, "glNormal3f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3f()");
		return FALSE;
	}

	if ( (orig_glNormal3fv = (func_glNormal3fv_t) GetProcAddress (hOriginalDll, "glNormal3fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3fv()");
		return FALSE;
	}

	if ( (orig_glNormal3i = (func_glNormal3i_t) GetProcAddress (hOriginalDll, "glNormal3i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3i()");
		return FALSE;
	}

	if ( (orig_glNormal3iv = (func_glNormal3iv_t) GetProcAddress (hOriginalDll, "glNormal3iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3iv()");
		return FALSE;
	}

	if ( (orig_glNormal3s = (func_glNormal3s_t) GetProcAddress (hOriginalDll, "glNormal3s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3s()");
		return FALSE;
	}

	if ( (orig_glNormal3sv = (func_glNormal3sv_t) GetProcAddress (hOriginalDll, "glNormal3sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormal3sv()");
		return FALSE;
	}

	if ( (orig_glNormalPointer = (func_glNormalPointer_t) GetProcAddress (hOriginalDll, "glNormalPointer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glNormalPointer()");
		return FALSE;
	}

	if ( (orig_glOrtho = (func_glOrtho_t) GetProcAddress (hOriginalDll, "glOrtho")) == NULL )
	{
		add_log ("Couldn't found a prototype for glOrtho()");
		return FALSE;
	}

	if ( (orig_glPassThrough = (func_glPassThrough_t) GetProcAddress (hOriginalDll, "glPassThrough")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPassThrough()");
		return FALSE;
	}

	if ( (orig_glPixelMapfv = (func_glPixelMapfv_t) GetProcAddress (hOriginalDll, "glPixelMapfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelMapfv()");
		return FALSE;
	}

	if ( (orig_glPixelMapuiv = (func_glPixelMapuiv_t) GetProcAddress (hOriginalDll, "glPixelMapuiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelMapuiv()");
		return FALSE;
	}

	if ( (orig_glPixelMapusv = (func_glPixelMapusv_t) GetProcAddress (hOriginalDll, "glPixelMapusv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelMapusv()");
		return FALSE;
	}

	if ( (orig_glPixelStoref = (func_glPixelStoref_t) GetProcAddress (hOriginalDll, "glPixelStoref")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelStoref()");
		return FALSE;
	}

	if ( (orig_glPixelStorei = (func_glPixelStorei_t) GetProcAddress (hOriginalDll, "glPixelStorei")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelStorei()");
		return FALSE;
	}

	if ( (orig_glPixelTransferf = (func_glPixelTransferf_t) GetProcAddress (hOriginalDll, "glPixelTransferf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelTransferf()");
		return FALSE;
	}

	if ( (orig_glPixelTransferi = (func_glPixelTransferi_t) GetProcAddress (hOriginalDll, "glPixelTransferi")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelTransferi()");
		return FALSE;
	}

	if ( (orig_glPixelZoom = (func_glPixelZoom_t) GetProcAddress (hOriginalDll, "glPixelZoom")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPixelZoom()");
		return FALSE;
	}

	if ( (orig_glPointSize = (func_glPointSize_t) GetProcAddress (hOriginalDll, "glPointSize")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPointSize()");
		return FALSE;
	}

	if ( (orig_glPolygonMode = (func_glPolygonMode_t) GetProcAddress (hOriginalDll, "glPolygonMode")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPolygonMode()");
		return FALSE;
	}

	if ( (orig_glPolygonOffset = (func_glPolygonOffset_t) GetProcAddress (hOriginalDll, "glPolygonOffset")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPolygonOffset()");
		return FALSE;
	}

	if ( (orig_glPolygonStipple = (func_glPolygonStipple_t) GetProcAddress (hOriginalDll, "glPolygonStipple")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPolygonStipple()");
		return FALSE;
	}

	if ( (orig_glPopAttrib = (func_glPopAttrib_t) GetProcAddress (hOriginalDll, "glPopAttrib")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPopAttrib()");
		return FALSE;
	}

	if ( (orig_glPopClientAttrib = (func_glPopClientAttrib_t) GetProcAddress (hOriginalDll, "glPopClientAttrib")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPopClientAttrib()");
		return FALSE;
	}

	if ( (orig_glPopMatrix = (func_glPopMatrix_t) GetProcAddress (hOriginalDll, "glPopMatrix")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPopMatrix()");
		return FALSE;
	}

	if ( (orig_glPopName = (func_glPopName_t) GetProcAddress (hOriginalDll, "glPopName")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPopName()");
		return FALSE;
	}

	if ( (orig_glPrioritizeTextures = (func_glPrioritizeTextures_t) GetProcAddress (hOriginalDll, "glPrioritizeTextures")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPrioritizeTextures()");
		return FALSE;
	}

	if ( (orig_glPushAttrib = (func_glPushAttrib_t) GetProcAddress (hOriginalDll, "glPushAttrib")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPushAttrib()");
		return FALSE;
	}

	if ( (orig_glPushClientAttrib = (func_glPushClientAttrib_t) GetProcAddress (hOriginalDll, "glPushClientAttrib")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPushClientAttrib()");
		return FALSE;
	}

	if ( (orig_glPushMatrix = (func_glPushMatrix_t) GetProcAddress (hOriginalDll, "glPushMatrix")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPushMatrix()");
		return FALSE;
	}

	if ( (orig_glPushName = (func_glPushName_t) GetProcAddress (hOriginalDll, "glPushName")) == NULL )
	{
		add_log ("Couldn't found a prototype for glPushName()");
		return FALSE;
	}

	if ( (orig_glRasterPos2d = (func_glRasterPos2d_t) GetProcAddress (hOriginalDll, "glRasterPos2d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2d()");
		return FALSE;
	}

	if ( (orig_glRasterPos2dv = (func_glRasterPos2dv_t) GetProcAddress (hOriginalDll, "glRasterPos2dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2dv()");
		return FALSE;
	}

	if ( (orig_glRasterPos2f = (func_glRasterPos2f_t) GetProcAddress (hOriginalDll, "glRasterPos2f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2f()");
		return FALSE;
	}

	if ( (orig_glRasterPos2fv = (func_glRasterPos2fv_t) GetProcAddress (hOriginalDll, "glRasterPos2fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2fv()");
		return FALSE;
	}

	if ( (orig_glRasterPos2i = (func_glRasterPos2i_t) GetProcAddress (hOriginalDll, "glRasterPos2i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2i()");
		return FALSE;
	}

	if ( (orig_glRasterPos2iv = (func_glRasterPos2iv_t) GetProcAddress (hOriginalDll, "glRasterPos2iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2iv()");
		return FALSE;
	}

	if ( (orig_glRasterPos2s = (func_glRasterPos2s_t) GetProcAddress (hOriginalDll, "glRasterPos2s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2s()");
		return FALSE;
	}

	if ( (orig_glRasterPos2sv = (func_glRasterPos2sv_t) GetProcAddress (hOriginalDll, "glRasterPos2sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos2sv()");
		return FALSE;
	}

	if ( (orig_glRasterPos3d = (func_glRasterPos3d_t) GetProcAddress (hOriginalDll, "glRasterPos3d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3d()");
		return FALSE;
	}

	if ( (orig_glRasterPos3dv = (func_glRasterPos3dv_t) GetProcAddress (hOriginalDll, "glRasterPos3dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3dv()");
		return FALSE;
	}

	if ( (orig_glRasterPos3f = (func_glRasterPos3f_t) GetProcAddress (hOriginalDll, "glRasterPos3f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3f()");
		return FALSE;
	}

	if ( (orig_glRasterPos3fv = (func_glRasterPos3fv_t) GetProcAddress (hOriginalDll, "glRasterPos3fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3fv()");
		return FALSE;
	}

	if ( (orig_glRasterPos3i = (func_glRasterPos3i_t) GetProcAddress (hOriginalDll, "glRasterPos3i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3i()");
		return FALSE;
	}

	if ( (orig_glRasterPos3iv = (func_glRasterPos3iv_t) GetProcAddress (hOriginalDll, "glRasterPos3iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3iv()");
		return FALSE;
	}

	if ( (orig_glRasterPos3s = (func_glRasterPos3s_t) GetProcAddress (hOriginalDll, "glRasterPos3s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3s()");
		return FALSE;
	}

	if ( (orig_glRasterPos3sv = (func_glRasterPos3sv_t) GetProcAddress (hOriginalDll, "glRasterPos3sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos3sv()");
		return FALSE;
	}

	if ( (orig_glRasterPos4d = (func_glRasterPos4d_t) GetProcAddress (hOriginalDll, "glRasterPos4d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4d()");
		return FALSE;
	}

	if ( (orig_glRasterPos4dv = (func_glRasterPos4dv_t) GetProcAddress (hOriginalDll, "glRasterPos4dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4dv()");
		return FALSE;
	}

	if ( (orig_glRasterPos4f = (func_glRasterPos4f_t) GetProcAddress (hOriginalDll, "glRasterPos4f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4f()");
		return FALSE;
	}

	if ( (orig_glRasterPos4fv = (func_glRasterPos4fv_t) GetProcAddress (hOriginalDll, "glRasterPos4fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4fv()");
		return FALSE;
	}

	if ( (orig_glRasterPos4i = (func_glRasterPos4i_t) GetProcAddress (hOriginalDll, "glRasterPos4i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4i()");
		return FALSE;
	}

	if ( (orig_glRasterPos4iv = (func_glRasterPos4iv_t) GetProcAddress (hOriginalDll, "glRasterPos4iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4iv()");
		return FALSE;
	}

	if ( (orig_glRasterPos4s = (func_glRasterPos4s_t) GetProcAddress (hOriginalDll, "glRasterPos4s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4s()");
		return FALSE;
	}

	if ( (orig_glRasterPos4sv = (func_glRasterPos4sv_t) GetProcAddress (hOriginalDll, "glRasterPos4sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRasterPos4sv()");
		return FALSE;
	}

	if ( (orig_glReadBuffer = (func_glReadBuffer_t) GetProcAddress (hOriginalDll, "glReadBuffer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glReadBuffer()");
		return FALSE;
	}

	if ( (orig_glReadPixels = (func_glReadPixels_t) GetProcAddress (hOriginalDll, "glReadPixels")) == NULL )
	{
		add_log ("Couldn't found a prototype for glReadPixels()");
		return FALSE;
	}

	if ( (orig_glRectd = (func_glRectd_t) GetProcAddress (hOriginalDll, "glRectd")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRectd()");
		return FALSE;
	}

	if ( (orig_glRectdv = (func_glRectdv_t) GetProcAddress (hOriginalDll, "glRectdv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRectdv()");
		return FALSE;
	}

	if ( (orig_glRectf = (func_glRectf_t) GetProcAddress (hOriginalDll, "glRectf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRectf()");
		return FALSE;
	}

	if ( (orig_glRectfv = (func_glRectfv_t) GetProcAddress (hOriginalDll, "glRectfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRectfv()");
		return FALSE;
	}

	if ( (orig_glRecti = (func_glRecti_t) GetProcAddress (hOriginalDll, "glRecti")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRecti()");
		return FALSE;
	}

	if ( (orig_glRectiv = (func_glRectiv_t) GetProcAddress (hOriginalDll, "glRectiv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRectiv()");
		return FALSE;
	}

	if ( (orig_glRects = (func_glRects_t) GetProcAddress (hOriginalDll, "glRects")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRects()");
		return FALSE;
	}

	if ( (orig_glRectsv = (func_glRectsv_t) GetProcAddress (hOriginalDll, "glRectsv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRectsv()");
		return FALSE;
	}

	if ( (orig_glRenderMode = (func_glRenderMode_t) GetProcAddress (hOriginalDll, "glRenderMode")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRenderMode()");
		return FALSE;
	}

	if ( (orig_glRotated = (func_glRotated_t) GetProcAddress (hOriginalDll, "glRotated")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRotated()");
		return FALSE;
	}

	if ( (orig_glRotatef = (func_glRotatef_t) GetProcAddress (hOriginalDll, "glRotatef")) == NULL )
	{
		add_log ("Couldn't found a prototype for glRotatef()");
		return FALSE;
	}

	if ( (orig_glScaled = (func_glScaled_t) GetProcAddress (hOriginalDll, "glScaled")) == NULL )
	{
		add_log ("Couldn't found a prototype for glScaled()");
		return FALSE;
	}

	if ( (orig_glScalef = (func_glScalef_t) GetProcAddress (hOriginalDll, "glScalef")) == NULL )
	{
		add_log ("Couldn't found a prototype for glScalef()");
		return FALSE;
	}

	if ( (orig_glScissor = (func_glScissor_t) GetProcAddress (hOriginalDll, "glScissor")) == NULL )
	{
		add_log ("Couldn't found a prototype for glScissor()");
		return FALSE;
	}

	if ( (orig_glSelectBuffer = (func_glSelectBuffer_t) GetProcAddress (hOriginalDll, "glSelectBuffer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glSelectBuffer()");
		return FALSE;
	}

	if ( (orig_glShadeModel = (func_glShadeModel_t) GetProcAddress (hOriginalDll, "glShadeModel")) == NULL )
	{
		add_log ("Couldn't found a prototype for glShadeModel()");
		return FALSE;
	}

	if ( (orig_glStencilFunc = (func_glStencilFunc_t) GetProcAddress (hOriginalDll, "glStencilFunc")) == NULL )
	{
		add_log ("Couldn't found a prototype for glStencilFunc()");
		return FALSE;
	}

	if ( (orig_glStencilMask = (func_glStencilMask_t) GetProcAddress (hOriginalDll, "glStencilMask")) == NULL )
	{
		add_log ("Couldn't found a prototype for glStencilMask()");
		return FALSE;
	}

	if ( (orig_glStencilOp = (func_glStencilOp_t) GetProcAddress (hOriginalDll, "glStencilOp")) == NULL )
	{
		add_log ("Couldn't found a prototype for glStencilOp()");
		return FALSE;
	}

	if ( (orig_glTexCoord1d = (func_glTexCoord1d_t) GetProcAddress (hOriginalDll, "glTexCoord1d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1d()");
		return FALSE;
	}

	if ( (orig_glTexCoord1dv = (func_glTexCoord1dv_t) GetProcAddress (hOriginalDll, "glTexCoord1dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1dv()");
		return FALSE;
	}

	if ( (orig_glTexCoord1f = (func_glTexCoord1f_t) GetProcAddress (hOriginalDll, "glTexCoord1f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1f()");
		return FALSE;
	}

	if ( (orig_glTexCoord1fv = (func_glTexCoord1fv_t) GetProcAddress (hOriginalDll, "glTexCoord1fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1fv()");
		return FALSE;
	}

	if ( (orig_glTexCoord1i = (func_glTexCoord1i_t) GetProcAddress (hOriginalDll, "glTexCoord1i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1i()");
		return FALSE;
	}

	if ( (orig_glTexCoord1iv = (func_glTexCoord1iv_t) GetProcAddress (hOriginalDll, "glTexCoord1iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1iv()");
		return FALSE;
	}

	if ( (orig_glTexCoord1s = (func_glTexCoord1s_t) GetProcAddress (hOriginalDll, "glTexCoord1s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1s()");
		return FALSE;
	}

	if ( (orig_glTexCoord1sv = (func_glTexCoord1sv_t) GetProcAddress (hOriginalDll, "glTexCoord1sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord1sv()");
		return FALSE;
	}

	if ( (orig_glTexCoord2d = (func_glTexCoord2d_t) GetProcAddress (hOriginalDll, "glTexCoord2d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2d()");
		return FALSE;
	}

	if ( (orig_glTexCoord2dv = (func_glTexCoord2dv_t) GetProcAddress (hOriginalDll, "glTexCoord2dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2dv()");
		return FALSE;
	}

	if ( (orig_glTexCoord2f = (func_glTexCoord2f_t) GetProcAddress (hOriginalDll, "glTexCoord2f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2f()");
		return FALSE;
	}

	if ( (orig_glTexCoord2fv = (func_glTexCoord2fv_t) GetProcAddress (hOriginalDll, "glTexCoord2fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2fv()");
		return FALSE;
	}

	if ( (orig_glTexCoord2i = (func_glTexCoord2i_t) GetProcAddress (hOriginalDll, "glTexCoord2i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2i()");
		return FALSE;
	}

	if ( (orig_glTexCoord2iv = (func_glTexCoord2iv_t) GetProcAddress (hOriginalDll, "glTexCoord2iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2iv()");
		return FALSE;
	}

	if ( (orig_glTexCoord2s = (func_glTexCoord2s_t) GetProcAddress (hOriginalDll, "glTexCoord2s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2s()");
		return FALSE;
	}

	if ( (orig_glTexCoord2sv = (func_glTexCoord2sv_t) GetProcAddress (hOriginalDll, "glTexCoord2sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord2sv()");
		return FALSE;
	}

	if ( (orig_glTexCoord3d = (func_glTexCoord3d_t) GetProcAddress (hOriginalDll, "glTexCoord3d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3d()");
		return FALSE;
	}

	if ( (orig_glTexCoord3dv = (func_glTexCoord3dv_t) GetProcAddress (hOriginalDll, "glTexCoord3dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3dv()");
		return FALSE;
	}

	if ( (orig_glTexCoord3f = (func_glTexCoord3f_t) GetProcAddress (hOriginalDll, "glTexCoord3f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3f()");
		return FALSE;
	}

	if ( (orig_glTexCoord3fv = (func_glTexCoord3fv_t) GetProcAddress (hOriginalDll, "glTexCoord3fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3fv()");
		return FALSE;
	}

	if ( (orig_glTexCoord3i = (func_glTexCoord3i_t) GetProcAddress (hOriginalDll, "glTexCoord3i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3i()");
		return FALSE;
	}

	if ( (orig_glTexCoord3iv = (func_glTexCoord3iv_t) GetProcAddress (hOriginalDll, "glTexCoord3iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3iv()");
		return FALSE;
	}

	if ( (orig_glTexCoord3s = (func_glTexCoord3s_t) GetProcAddress (hOriginalDll, "glTexCoord3s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3s()");
		return FALSE;
	}

	if ( (orig_glTexCoord3sv = (func_glTexCoord3sv_t) GetProcAddress (hOriginalDll, "glTexCoord3sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord3sv()");
		return FALSE;
	}

	if ( (orig_glTexCoord4d = (func_glTexCoord4d_t) GetProcAddress (hOriginalDll, "glTexCoord4d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4d()");
		return FALSE;
	}

	if ( (orig_glTexCoord4dv = (func_glTexCoord4dv_t) GetProcAddress (hOriginalDll, "glTexCoord4dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4dv()");
		return FALSE;
	}

	if ( (orig_glTexCoord4f = (func_glTexCoord4f_t) GetProcAddress (hOriginalDll, "glTexCoord4f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4f()");
		return FALSE;
	}

	if ( (orig_glTexCoord4fv = (func_glTexCoord4fv_t) GetProcAddress (hOriginalDll, "glTexCoord4fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4fv()");
		return FALSE;
	}

	if ( (orig_glTexCoord4i = (func_glTexCoord4i_t) GetProcAddress (hOriginalDll, "glTexCoord4i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4i()");
		return FALSE;
	}

	if ( (orig_glTexCoord4iv = (func_glTexCoord4iv_t) GetProcAddress (hOriginalDll, "glTexCoord4iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4iv()");
		return FALSE;
	}

	if ( (orig_glTexCoord4s = (func_glTexCoord4s_t) GetProcAddress (hOriginalDll, "glTexCoord4s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4s()");
		return FALSE;
	}

	if ( (orig_glTexCoord4sv = (func_glTexCoord4sv_t) GetProcAddress (hOriginalDll, "glTexCoord4sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoord4sv()");
		return FALSE;
	}

	if ( (orig_glTexCoordPointer = (func_glTexCoordPointer_t) GetProcAddress (hOriginalDll, "glTexCoordPointer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexCoordPointer()");
		return FALSE;
	}

	if ( (orig_glTexEnvf = (func_glTexEnvf_t) GetProcAddress (hOriginalDll, "glTexEnvf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexEnvf()");
		return FALSE;
	}

	if ( (orig_glTexEnvfv = (func_glTexEnvfv_t) GetProcAddress (hOriginalDll, "glTexEnvfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexEnvfv()");
		return FALSE;
	}

	if ( (orig_glTexEnvi = (func_glTexEnvi_t) GetProcAddress (hOriginalDll, "glTexEnvi")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexEnvi()");
		return FALSE;
	}

	if ( (orig_glTexEnviv = (func_glTexEnviv_t) GetProcAddress (hOriginalDll, "glTexEnviv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexEnviv()");
		return FALSE;
	}

	if ( (orig_glTexGend = (func_glTexGend_t) GetProcAddress (hOriginalDll, "glTexGend")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexGend()");
		return FALSE;
	}

	if ( (orig_glTexGendv = (func_glTexGendv_t) GetProcAddress (hOriginalDll, "glTexGendv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexGendv()");
		return FALSE;
	}

	if ( (orig_glTexGenf = (func_glTexGenf_t) GetProcAddress (hOriginalDll, "glTexGenf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexGenf()");
		return FALSE;
	}

	if ( (orig_glTexGenfv = (func_glTexGenfv_t) GetProcAddress (hOriginalDll, "glTexGenfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexGenfv()");
		return FALSE;
	}

	if ( (orig_glTexGeni = (func_glTexGeni_t) GetProcAddress (hOriginalDll, "glTexGeni")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexGeni()");
		return FALSE;
	}

	if ( (orig_glTexGeniv = (func_glTexGeniv_t) GetProcAddress (hOriginalDll, "glTexGeniv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexGeniv()");
		return FALSE;
	}

	if ( (orig_glTexImage1D = (func_glTexImage1D_t) GetProcAddress (hOriginalDll, "glTexImage1D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexImage1D()");
		return FALSE;
	}

	if ( (orig_glTexImage2D = (func_glTexImage2D_t) GetProcAddress (hOriginalDll, "glTexImage2D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexImage2D()");
		return FALSE;
	}

	if ( (orig_glTexParameterf = (func_glTexParameterf_t) GetProcAddress (hOriginalDll, "glTexParameterf")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexParameterf()");
		return FALSE;
	}

	if ( (orig_glTexParameterfv = (func_glTexParameterfv_t) GetProcAddress (hOriginalDll, "glTexParameterfv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexParameterfv()");
		return FALSE;
	}

	if ( (orig_glTexParameteri = (func_glTexParameteri_t) GetProcAddress (hOriginalDll, "glTexParameteri")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexParameteri()");
		return FALSE;
	}

	if ( (orig_glTexParameteriv = (func_glTexParameteriv_t) GetProcAddress (hOriginalDll, "glTexParameteriv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexParameteriv()");
		return FALSE;
	}

	if ( (orig_glTexSubImage1D = (func_glTexSubImage1D_t) GetProcAddress (hOriginalDll, "glTexSubImage1D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexSubImage1D()");
		return FALSE;
	}

	if ( (orig_glTexSubImage2D = (func_glTexSubImage2D_t) GetProcAddress (hOriginalDll, "glTexSubImage2D")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTexSubImage2D()");
		return FALSE;
	}

	if ( (orig_glTranslated = (func_glTranslated_t) GetProcAddress (hOriginalDll, "glTranslated")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTranslated()");
		return FALSE;
	}

	if ( (orig_glTranslatef = (func_glTranslatef_t) GetProcAddress (hOriginalDll, "glTranslatef")) == NULL )
	{
		add_log ("Couldn't found a prototype for glTranslatef()");
		return FALSE;
	}

	if ( (orig_glVertex2d = (func_glVertex2d_t) GetProcAddress (hOriginalDll, "glVertex2d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2d()");
		return FALSE;
	}

	if ( (orig_glVertex2dv = (func_glVertex2dv_t) GetProcAddress (hOriginalDll, "glVertex2dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2dv()");
		return FALSE;
	}

	if ( (orig_glVertex2f = (func_glVertex2f_t) GetProcAddress (hOriginalDll, "glVertex2f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2f()");
		return FALSE;
	}

	if ( (orig_glVertex2fv = (func_glVertex2fv_t) GetProcAddress (hOriginalDll, "glVertex2fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2fv()");
		return FALSE;
	}

	if ( (orig_glVertex2i = (func_glVertex2i_t) GetProcAddress (hOriginalDll, "glVertex2i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2i()");
		return FALSE;
	}

	if ( (orig_glVertex2iv = (func_glVertex2iv_t) GetProcAddress (hOriginalDll, "glVertex2iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2iv()");
		return FALSE;
	}

	if ( (orig_glVertex2s = (func_glVertex2s_t) GetProcAddress (hOriginalDll, "glVertex2s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2s()");
		return FALSE;
	}

	if ( (orig_glVertex2sv = (func_glVertex2sv_t) GetProcAddress (hOriginalDll, "glVertex2sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex2sv()");
		return FALSE;
	}

	if ( (orig_glVertex3d = (func_glVertex3d_t) GetProcAddress (hOriginalDll, "glVertex3d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3d()");
		return FALSE;
	}

	if ( (orig_glVertex3dv = (func_glVertex3dv_t) GetProcAddress (hOriginalDll, "glVertex3dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3dv()");
		return FALSE;
	}

	if ( (orig_glVertex3f = (func_glVertex3f_t) GetProcAddress (hOriginalDll, "glVertex3f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3f()");
		return FALSE;
	}

	if ( (orig_glVertex3fv = (func_glVertex3fv_t) GetProcAddress (hOriginalDll, "glVertex3fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3fv()");
		return FALSE;
	}

	if ( (orig_glVertex3i = (func_glVertex3i_t) GetProcAddress (hOriginalDll, "glVertex3i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3i()");
		return FALSE;
	}

	if ( (orig_glVertex3iv = (func_glVertex3iv_t) GetProcAddress (hOriginalDll, "glVertex3iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3iv()");
		return FALSE;
	}

	if ( (orig_glVertex3s = (func_glVertex3s_t) GetProcAddress (hOriginalDll, "glVertex3s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3s()");
		return FALSE;
	}

	if ( (orig_glVertex3sv = (func_glVertex3sv_t) GetProcAddress (hOriginalDll, "glVertex3sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex3sv()");
		return FALSE;
	}

	if ( (orig_glVertex4d = (func_glVertex4d_t) GetProcAddress (hOriginalDll, "glVertex4d")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4d()");
		return FALSE;
	}

	if ( (orig_glVertex4dv = (func_glVertex4dv_t) GetProcAddress (hOriginalDll, "glVertex4dv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4dv()");
		return FALSE;
	}

	if ( (orig_glVertex4f = (func_glVertex4f_t) GetProcAddress (hOriginalDll, "glVertex4f")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4f()");
		return FALSE;
	}

	if ( (orig_glVertex4fv = (func_glVertex4fv_t) GetProcAddress (hOriginalDll, "glVertex4fv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4fv()");
		return FALSE;
	}

	if ( (orig_glVertex4i = (func_glVertex4i_t) GetProcAddress (hOriginalDll, "glVertex4i")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4i()");
		return FALSE;
	}

	if ( (orig_glVertex4iv = (func_glVertex4iv_t) GetProcAddress (hOriginalDll, "glVertex4iv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4iv()");
		return FALSE;
	}

	if ( (orig_glVertex4s = (func_glVertex4s_t) GetProcAddress (hOriginalDll, "glVertex4s")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4s()");
		return FALSE;
	}

	if ( (orig_glVertex4sv = (func_glVertex4sv_t) GetProcAddress (hOriginalDll, "glVertex4sv")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertex4sv()");
		return FALSE;
	}

	if ( (orig_glVertexPointer = (func_glVertexPointer_t) GetProcAddress (hOriginalDll, "glVertexPointer")) == NULL )
	{
		add_log ("Couldn't found a prototype for glVertexPointer()");
		return FALSE;
	}

	if ( (orig_glViewport = (func_glViewport_t) GetProcAddress (hOriginalDll, "glViewport")) == NULL )
	{
		add_log ("Couldn't found a prototype for glViewport()");
		return FALSE;
	}

	if ( (orig_wglChoosePixelFormat = (func_wglChoosePixelFormat_t) GetProcAddress (hOriginalDll, "wglChoosePixelFormat")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglChoosePixelFormat()");
		return FALSE;
	}

	if ( (orig_wglCopyContext = (func_wglCopyContext_t) GetProcAddress (hOriginalDll, "wglCopyContext")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglCopyContext()");
		return FALSE;
	}

	if ( (orig_wglCreateContext = (func_wglCreateContext_t) GetProcAddress (hOriginalDll, "wglCreateContext")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglCreateContext()");
		return FALSE;
	}

	if ( (orig_wglCreateLayerContext = (func_wglCreateLayerContext_t) GetProcAddress (hOriginalDll, "wglCreateLayerContext")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglCreateLayerContext()");
		return FALSE;
	}

	if ( (orig_wglDeleteContext = (func_wglDeleteContext_t) GetProcAddress (hOriginalDll, "wglDeleteContext")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglDeleteContext()");
		return FALSE;
	}

	if ( (orig_wglDescribeLayerPlane = (func_wglDescribeLayerPlane_t) GetProcAddress (hOriginalDll, "wglDescribeLayerPlane")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglDescribeLayerPlane()");
		return FALSE;
	}

	if ( (orig_wglDescribePixelFormat = (func_wglDescribePixelFormat_t) GetProcAddress (hOriginalDll, "wglDescribePixelFormat")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglDescribePixelFormat()");
		return FALSE;
	}

	if ( (orig_wglGetCurrentContext = (func_wglGetCurrentContext_t) GetProcAddress (hOriginalDll, "wglGetCurrentContext")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglGetCurrentContext()");
		return FALSE;
	}

	if ( (orig_wglGetCurrentDC = (func_wglGetCurrentDC_t) GetProcAddress (hOriginalDll, "wglGetCurrentDC")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglGetCurrentDC()");
		return FALSE;
	}

	if ( (orig_wglGetDefaultProcAddress = (func_wglGetDefaultProcAddress_t) GetProcAddress (hOriginalDll, "wglGetDefaultProcAddress")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglGetDefaultProcAddress()");
		return FALSE;
	}

	if ( (orig_wglGetLayerPaletteEntries = (func_wglGetLayerPaletteEntries_t) GetProcAddress (hOriginalDll, "wglGetLayerPaletteEntries")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglGetLayerPaletteEntries()");
		return FALSE;
	}

	if ( (orig_wglGetPixelFormat = (func_wglGetPixelFormat_t) GetProcAddress (hOriginalDll, "wglGetPixelFormat")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglGetPixelFormat()");
		return FALSE;
	}

	if ( (orig_wglGetProcAddress = (func_wglGetProcAddress_t) GetProcAddress (hOriginalDll, "wglGetProcAddress")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglGetProcAddress()");
		return FALSE;
	}

	if ( (orig_wglMakeCurrent = (func_wglMakeCurrent_t) GetProcAddress (hOriginalDll, "wglMakeCurrent")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglMakeCurrent()");
		return FALSE;
	}

	if ( (orig_wglRealizeLayerPalette = (func_wglRealizeLayerPalette_t) GetProcAddress (hOriginalDll, "wglRealizeLayerPalette")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglRealizeLayerPalette()");
		return FALSE;
	}

	if ( (orig_wglSetLayerPaletteEntries = (func_wglSetLayerPaletteEntries_t) GetProcAddress (hOriginalDll, "wglSetLayerPaletteEntries")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglSetLayerPaletteEntries()");
		return FALSE;
	}

	if ( (orig_wglSetPixelFormat = (func_wglSetPixelFormat_t) GetProcAddress (hOriginalDll, "wglSetPixelFormat")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglSetPixelFormat()");
		return FALSE;
	}

	if ( (orig_wglShareLists = (func_wglShareLists_t) GetProcAddress (hOriginalDll, "wglShareLists")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglShareLists()");
		return FALSE;
	}

	if ( (orig_wglSwapBuffers = (func_wglSwapBuffers_t) GetProcAddress (hOriginalDll, "wglSwapBuffers")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglSwapBuffers()");
		return FALSE;
	}

	if ( (orig_wglSwapLayerBuffers = (func_wglSwapLayerBuffers_t) GetProcAddress (hOriginalDll, "wglSwapLayerBuffers")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglSwapLayerBuffers()");
		return FALSE;
	}

	if ( (orig_wglUseFontBitmapsA = (func_wglUseFontBitmapsA_t) GetProcAddress (hOriginalDll, "wglUseFontBitmapsA")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglUseFontBitmapsA()");
		return FALSE;
	}

	if ( (orig_wglUseFontBitmapsW = (func_wglUseFontBitmapsW_t) GetProcAddress (hOriginalDll, "wglUseFontBitmapsW")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglUseFontBitmapsW()");
		return FALSE;
	}

	if ( (orig_wglUseFontOutlinesA = (func_wglUseFontOutlinesA_t) GetProcAddress (hOriginalDll, "wglUseFontOutlinesA")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglUseFontOutlinesA()");
		return FALSE;
	}

	if ( (orig_wglUseFontOutlinesW = (func_wglUseFontOutlinesW_t) GetProcAddress (hOriginalDll, "wglUseFontOutlinesW")) == NULL )
	{
		add_log ("Couldn't found a prototype for wglUseFontOutlinesW()");
		return FALSE;
	}
    return TRUE;
}

// Other wrapping functions currently not used
#if 1
void sys_glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
	orig_glMultiTexCoord2fARB(target, s, t);
}

void sys_glActiveTextureARB(GLenum target)
{
	orig_glActiveTextureARB(target);
}

void sys_BindTextureEXT(GLenum target, GLuint texture)
{
	orig_BindTextureEXT(target, texture);
}

void sys_glAlphaFunc(GLenum func, GLclampf ref)
{
	(*orig_glAlphaFunc) (func, ref);
}

void sys_glBegin(GLenum mode)
{
	(*orig_glBegin) (mode);
}

void sys_glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
	(*orig_glBitmap) (width, height, xorig, yorig, xmove, ymove, bitmap);
}

void sys_glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	(*orig_glBlendFunc) (sfactor, dfactor);
}

void sys_glClear(GLbitfield mask)
{
	(*orig_glClear)(mask);
}

void sys_glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	(*orig_glClearAccum) (red, green, blue, alpha);
}

void sys_glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	(*orig_glClearColor) (red, green, blue, alpha);
}

void sys_glColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
	(*orig_glColor3f) (red, green, blue);
}

void sys_glColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
	(*orig_glColor3ub) (red, green, blue);
}

void sys_glColor3ubv(const GLubyte *v)
{
	(*orig_glColor3ubv) (v);
}

void sys_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	(*orig_glColor4f) (red, green, blue, alpha);
}

void sys_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	(*orig_glColor4ub) (red, green, blue, alpha);
}

void sys_glCullFace(GLenum mode)
{
	(*orig_glCullFace) (mode);
}

void sys_glDeleteTextures(GLsizei n, const GLuint *textures)
{
	(*orig_glDeleteTextures) (n, textures);
}

void sys_glDepthFunc(GLenum func)
{
	(*orig_glDepthFunc) (func);
}

void sys_glDepthMask(GLboolean flag)
{
	(*orig_glDepthMask) (flag);
}

void sys_glDepthRange(GLclampd zNear, GLclampd zFar)
{
	(*orig_glDepthRange) (zNear, zFar);
}

void sys_glDisable(GLenum cap)
{
	(*orig_glDisable) (cap);
}

void sys_glEnable(GLenum cap)
{
	(*orig_glEnable) (cap);
}

void sys_glEnd(void)
{
	(*orig_glEnd) ();
}

void sys_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	(*orig_glFrustum) (left, right, bottom, top, zNear, zFar);
}

void sys_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	(*orig_glOrtho) (left, right, bottom, top, zNear, zFar);
}

void sys_glPopMatrix(void)
{
	(*orig_glPopMatrix) ();
}

void sys_glPopName(void)
{
	(*orig_glPopName) ();
}

void sys_glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
	(*orig_glPrioritizeTextures) (n, textures, priorities);
}

void sys_glPushAttrib(GLbitfield mask)
{
	(*orig_glPushAttrib) (mask);
}

void sys_glPushClientAttrib(GLbitfield mask)
{
	(*orig_glPushClientAttrib) (mask);
}

void sys_glPushMatrix(void)
{
	(*orig_glPushMatrix) ();
}

void sys_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	(*orig_glRotatef) (angle, x, y, z);
}

void sys_glShadeModel(GLenum mode)
{
	(*orig_glShadeModel) (mode);
}

void sys_glTexCoord2f(GLfloat s, GLfloat t)
{
	(*orig_glTexCoord2f) (s, t);
}

void sys_glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	(*orig_glTexEnvf) (target, pname, param);
}

void sys_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	(*orig_glTexImage2D) (target, level, internalformat, width, height, border, format, type, pixels);
}

void sys_glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	(*orig_glTexParameterf) (target, pname, param);
}

void sys_glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
	(*orig_glTranslated) (x, y, z);
}

void sys_glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	(*orig_glTranslatef) (x, y, z);
}

void sys_glVertex2f(GLfloat x, GLfloat y)
{
	(*orig_glVertex2f) (x, y);
}

void sys_glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	(*orig_glVertex3f) (x, y, z);
}

void sys_glVertex3fv(const GLfloat *v)
{
	(*orig_glVertex3fv) (v);
}

void sys_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	(*orig_glViewport) (x, y, width, height);
}

void sys_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value)
{
	(orig_glUniformMatrix4fv)(location, count, transpose, value);
}

#endif