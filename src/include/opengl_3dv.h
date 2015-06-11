/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef ___3DVISION_OGL___
#define ___3DVISION_OGL___

#include "gl_custom.h"
#include "wgl_custom.h"
#include "windows.h"
#include <GL/GL.h>

typedef struct struct_GLD3DBuffers
{
	GLuint fbo_left, fbo_right, texture_left, texture_right, render_buffer;
	int width, height;
	bool initialized, stereo;

	// Internal
	void *d3dLibrary;
	void *d3dDevice;
	void *d3dLeftColorTexture;
	void *d3dRightColorTexture;
	void *d3dLeftColorBuffer;
	void *d3dRightColorBuffer;
	void *d3dStereoColorBuffer;
	void *d3dDepthBuffer;
	void *d3dBackBuffer;
	void *d3dDeviceInterop;
	void *d3dLeftColorInterop;
	void *d3dRightColorInterop;
	void *d3dDepthInterop;
	void *nvStereo;
} GLD3DBuffers;


BOOL NV3DVisionBuffers_create(GLD3DBuffers *gl_d3d_buffers, void *window_handle, bool stereo);
void NV3DVisionBuffers_destroy(GLD3DBuffers *gl_d3d_buffers);

void NV3DVisionBuffers_activate_left(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_flip_left(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_flip_right(GLD3DBuffers *gl_d3d_buffers);

void NV3DVisionBuffers_activate_right(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_deactivate(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_flush(GLD3DBuffers *gl_d3d_buffers);

// Version 1.5 Wrapper with Automatic Hooking
void NV3DVisionBuffers_copy_left(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_copy_right(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_activate_depth(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_deactivate_depth(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_activate_right(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_deactivate_left(GLD3DBuffers *gl_d3d_buffers);
void NV3DVisionBuffers_deactivate_right(GLD3DBuffers *gl_d3d_buffers);

/// 3D Vision Initialisation
void NV3DVisionReInitBuffers(void);
bool NV3DVisionIsNotInit();
void NV3DVisionSetCurrentFrame(unsigned int frameNumber);
unsigned int NV3DVisionGetCurrentFrame(void);

//// NVAPI 3D Vision Values
void StartNVAPIThread(void);
void StoptNVAPIThread(void);
void Set3DSeparation(float value);
void Set3DConvergence(float value);
void Set3DEyeSeparation(float value);

float Get3DSeparation();
float Get3DConvergence();
float Get3DEyeSeparation();

//// OpenGL-DirectX interoop state
void SetInterop(bool state);
BOOL GetInterop(void);

// Versioning
void OGL3DVisionWrapperGetVersion(char *pVersionStr);

#endif
