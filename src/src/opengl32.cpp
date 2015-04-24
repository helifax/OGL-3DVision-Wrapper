/*
* Game-Deception Blank Wrapper v2
* Copyright (c) Helifax 2015
*/

extern "C" {
#include "opengl_3dv.h"
}
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include "opengl32.h"
#include "include\ShaderManager.h"
#include "windows.h"
#include "d3d9.h"
#include "NVAPI_343\nvapi.h"
#include <thread>
#include "FPSInject/FPSInject.h"
#include "MessageBox\MessageBox.h"
#include "ConfigReader.h"

#include "opengl32_ARB_Variant.h"

// FPSInject
HINSTANCE FPSHandle;
typedef FPSInject* (__cdecl *create)();
typedef void(*destroy)(FPSInject*);
create FPScreate;
destroy FPSDestroy;
static void LoadFPSInject(void);
static bool isFPSInjectHandleValid(void);

// Our object
FPSInject* m_FPSInjector;

unsigned int g_width = 0;
unsigned int g_height = 0;
bool g_windowResizeCheck = false;
static HWND handle;
bool g_swapConvergence = false;
GLD3DBuffers * gl_d3d_buffers;

//Config reader
extern configReader *g_reader;

// Adding the current shaders
EXISTING_SHADERS_T g_currentShaders;

// normal buffer swapping on BindFrameBuffer
static void bindFrameBufferSwap(void);

// Automatic GL_SWAPBUFFERs - Auto hook point
static void AutomaticGlSwapBuffers(void);

// End frame method used in automatic hooking 
static void EndFrames(void);

// buffer swapping on SwapBuffer
static void swapBuffersSwap(void);

#ifdef DEBUG_WRAPPER
int debugShaderIndex = -1;
bool printCurrentShader = false;
bool isCurrentShaderVertex = true;
bool isCompileEnable = false;
#endif

// Window Resize
static void windowResize();

//********************************************************************************************
// DEBUG FIND ENTRY POINT
//********************************************************************************************

//log_glBindFramebuffer
void log_glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	char text[100];
	sprintf_s(text, "glBindFramebuffer %d Called !", framebuffer);
	if (framebuffer == 0)
	{
		//this is our ScreenFramebuffer
		strcat_s(text, 100, " This is the SCREEN FRAMEBUFFER !!!");
	}
	else
	{
		strcat_s(text, 100, " Ignore this one");
	}
	add_log(text);
	if (orig_glBindFramebuffer != 0)
	{
		(*orig_glBindFramebuffer)(target, framebuffer);
	}
}
///-------------------------------------------------------------------------------------------

//log_glBindFramebufferEXT
void log_glBindFramebufferEXT(GLenum target, GLuint framebuffer)
{
	char text[100];
	sprintf_s(text, "glBindFramebufferEXT %d Called !", framebuffer);
	if (framebuffer == 0)
	{
		//this is our ScreenFramebuffer
		strcat_s(text, 100, " This is the SCREEN FRAMEBUFFER !!!");
	}
	else
	{
		strcat_s(text, 100, " Ignore this one");
	}
	add_log(text);
	if (orig_glBindFramebuffer != 0)
	{
		(*orig_glBindFramebuffer)(target, framebuffer);
	}
}
///-------------------------------------------------------------------------------------------

//log_glBindFramebufferARB
void log_glBindFramebufferARB(GLenum target, GLuint framebuffer)
{
	char text[100];
	sprintf_s(text, 100, "glBindFramebufferARB %d Called !", framebuffer);
	if (framebuffer == 0)
	{
		//this is our ScreenFramebuffer
		strcat_s(text, 100, " This is the SCREEN FRAMEBUFFER !!!");
	}
	else
	{
		strcat_s(text, 100, " Ignore this one");
	}
	add_log(text);
	if (orig_glBindFramebuffer != 0)
	{
		(*orig_glBindFramebuffer)(target, framebuffer);
	}
}
///-------------------------------------------------------------------------------------------

//********************************************************************************************
// END DEBUG FIND ENTRY POINT
//********************************************************************************************

//sys_glBindFramebuffer
void sys_glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	// If we have the injection point enabled
	if (g_reader->Get3DVisionEnabledStatus())
	{
		// We are rendering the left frame, so let it run natively
		if (framebuffer == GLuint(0) && GetInterop() == false)
		{
			if ((NV3DVisionGetCurrentFrame() == 1))
			{
				GLD3DBuffers_activate_depth(gl_d3d_buffers);
				GLD3DBuffers_activate_left(gl_d3d_buffers);
			}
			else if ((NV3DVisionGetCurrentFrame() == 2))
			{

				GLD3DBuffers_activate_right(gl_d3d_buffers);
			}
		}
		else //not our framebuffer 0
		{
			//Original Frame buffer
			(*orig_glBindFramebuffer)(target, framebuffer);
		}
	}
	else
	{
		//Original Frame buffer
		(*orig_glBindFramebuffer)(target, framebuffer);
	}
	
}
///-------------------------------------------------------------------------------------------

// normal buffer swapping on BindFrameBuffer
static void bindFrameBufferSwap(void)
{
	//initializes
	if (NV3DVisionIsNotInit())
	{
		//Start our window resize checker thread if the mode is enabled.
		if (g_reader->GetWindowResizeEnabled())
		{
			std::thread ResizeCheck(windowResize);
			ResizeCheck.detach();
		}

		// Load our extensions
		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
		{
			add_log("Failed to get OpenGL extensions (ogl_LOAD_FAILED)");
		}
		//Create our buffers
		gl_d3d_buffers = new GLD3DBuffers;

		handle = GetActiveWindow();
		GLD3DBuffers_create(gl_d3d_buffers, handle, true);

		//start the rendering
		NV3DVisionSetCurrentFrame(1);
		add_log("3D Vision successfully initialised !");

		// FPS Inject
		if (isFPSInjectHandleValid())
		{
			m_FPSInjector->synchronizeRendering();
		}
	}

	else if (NV3DVisionGetCurrentFrame() == 1)
	{
		// Sync Left eye
		if (isFPSInjectHandleValid())
		{
			m_FPSInjector->syncLeft();
		}
		NV3DVisionSetCurrentFrame(2);

	}
	else if (NV3DVisionGetCurrentFrame() == 2)
	{
		// Sync Right eye
		if (isFPSInjectHandleValid())
		{
			m_FPSInjector->syncRight();
		}
		NV3DVisionSetCurrentFrame(1);

		// both eyes are rendered. So swap buffers and reset counter
		//Flip the framebuffers vetically
		GLD3DBuffers_flip_left(gl_d3d_buffers);
		GLD3DBuffers_flip_right(gl_d3d_buffers);

		// Do the rendering
		GLD3DBuffers_deactivate(gl_d3d_buffers);
		GLD3DBuffers_flush(gl_d3d_buffers);
		

		//check for resize
		if (g_windowResizeCheck == true)
		{
			// create DirectX 3D Vision buffers
			GLD3DBuffers_destroy(gl_d3d_buffers);

			//Resize complete
			g_windowResizeCheck = false;

			// Create the new buffers
			handle = GetActiveWindow();
			GLD3DBuffers_create(gl_d3d_buffers, handle, true);
		}
	}
}
///-------------------------------------------------------------------------------------------

// buffer swapping on SwapBuffer
static void swapBuffersSwap(void)
{
	if (NV3DVisionGetCurrentFrame() == 0)
	{

		// Initialize the WRAPPER!!! BANG BANG! 3D Vision Active baby!!!!
		if (NV3DVisionIsNotInit())
		{
			//Start our window resize checker thread if the mode is enabled.
			if (g_reader->GetWindowResizeEnabled())
			{
				std::thread ResizeCheck(windowResize);
				ResizeCheck.detach();
			}

			// Load our extensions
			if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
			{
				add_log("Failed to get OpenGL extensions (ogl_LOAD_FAILED)");
			}

			//Create our buffers
			gl_d3d_buffers = new GLD3DBuffers;

			handle = GetActiveWindow();
			GLD3DBuffers_create(gl_d3d_buffers, handle, true);

			add_log("3D Vision successfully initialised !");

			// FPS Inject
			if (isFPSInjectHandleValid())
			{
				m_FPSInjector->synchronizeRendering();
			}
		}
		else
		{
			// Render what we have and continue rendering with the left eye
			//Flip the framebuffers vertically
			GLD3DBuffers_flip_left(gl_d3d_buffers);
			GLD3DBuffers_flip_right(gl_d3d_buffers);

			// Do the rendering
			GLD3DBuffers_deactivate(gl_d3d_buffers);
			GLD3DBuffers_flush(gl_d3d_buffers);

			//check for resize
			if (g_windowResizeCheck == true)
			{
				// create DirectX 3D Vision buffers
				GLD3DBuffers_destroy(gl_d3d_buffers);

				// Create the new buffers
				handle = GetActiveWindow();
				GLD3DBuffers_create(gl_d3d_buffers, handle, true);

				//Resize complete
				g_windowResizeCheck = false;
			}

			//start the rendering
			NV3DVisionSetCurrentFrame(1);
			//activate left buffer so everything is copied in it
			GLD3DBuffers_activate_depth(gl_d3d_buffers);
			GLD3DBuffers_activate_left(gl_d3d_buffers);

			// Sync Left eye
			if (isFPSInjectHandleValid())
			{
				m_FPSInjector->syncLeft();
			}
		}
	}
	else if (NV3DVisionGetCurrentFrame() == 1)
	{
		//activate right buffer so everything is copied in it
		NV3DVisionSetCurrentFrame(0);
		GLD3DBuffers_activate_right(gl_d3d_buffers);

		// Sync RIGHT eye
		if (isFPSInjectHandleValid())
		{
			m_FPSInjector->syncRight();
		}
	}
}
///-------------------------------------------------------------------------------------------

//********************************************************************************************
// SHADER MANIPULATION
//********************************************************************************************

GLuint sys_glCreateProgram(void)
{
	GLuint program = (*orig_glCreateProgram)();

	//Store the program
	g_currentShaders.m_programId = program;

	return program;
}
///-------------------------------------------------------------------------------------------

// do our stereo here
void sys_glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{

	// Apply the original source
	(*orig_glShaderSource)(shader, count, string, length);
	
	// Get the instance
	ShaderManager * g_shaderManager = ShaderManager::getInstance();

	// Make modifications only on the vertex shader
	if (g_shaderManager->isShaderType(shader, GL_VERTEX_SHADER))
	{
		//get the original shader Source
		std::string shdrSournce = g_shaderManager->getShaderSource(shader);

		//Save the pair
		g_currentShaders.m_vertexShader = shader;
		// Original Source
		g_currentShaders.m_vertexSource = shdrSournce;

		if (g_shaderManager->GetVertexInjectionState())
		{
			//Insert Stereo
			shdrSournce = g_shaderManager->injectStereoScopy(shdrSournce, g_currentShaders.m_programId);

			// Modified Source.
			// We want to store the modified source
			g_currentShaders.m_vertexSource = shdrSournce;
		}
		// Compile it
		g_shaderManager->applyShaderSource(shader, shdrSournce, NULL);
	}
	else if (g_shaderManager->isShaderType(shader, GL_FRAGMENT_SHADER))
	{
		//get the original shader Source
		std::string shdrSournce = g_shaderManager->getShaderSource(shader);

		//Save the pair
		g_currentShaders.m_pixelShader = shader;
		g_currentShaders.m_fragmentSource = shdrSournce;

		// Compile it
		g_shaderManager->applyShaderSource(shader, shdrSournce, NULL);
	}
	else if (g_shaderManager->isShaderType(shader, GL_GEOMETRY_SHADER_EXT))
	{
		//get the original shader Source
		std::string shdrSournce = g_shaderManager->getShaderSource(shader);
		g_shaderManager->applyShaderSource(shader, shdrSournce, NULL);
	}
}
///-------------------------------------------------------------------------------------------

void sys_glCompileShader(GLuint shader)
{
	// Compile the shader
	(*orig_glCompileShader)(shader);

	// Check to see if the compile was ok
	GLint OK = GL_FALSE;
	orig_glGetShaderiv = (func_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
	(*orig_glGetShaderiv)(shader, GL_COMPILE_STATUS, &OK);

	if (OK == GL_FALSE)
	{
		CheckCompileState(shader);
		exit(0);
	}
}
///-------------------------------------------------------------------------------------------

void sys_glLinkProgram(GLuint program)
{
	//Check if we have the sources 0 if so attempt to extract them.
	if ((g_currentShaders.m_vertexShader == 0) && (g_currentShaders.m_pixelShader == 0))
	{
		GLsizei shaderCount;
		GLuint shaders[2] = { 0, 0 };
		glGetAttachedShaders(program, 4, &shaderCount, shaders);
		g_currentShaders.m_pixelShader = shaders[0];
		g_currentShaders.m_vertexShader = shaders[1];

		if (g_reader->GetEnableShaderDump())
		{
			//Get the sources
			GLchar source[50000];
			GLsizei size;
			glGetShaderSource(g_currentShaders.m_vertexShader, 50000, &size, source);
			g_currentShaders.m_vertexSource = source;

			//Fragment
			glGetShaderSource(g_currentShaders.m_pixelShader, 50000, &size, source);
			g_currentShaders.m_fragmentSource = source;
		}
	}

	// Get the instance
	ShaderManager * g_shaderManager = ShaderManager::getInstance();
	g_shaderManager->addExistingShaderInfo(g_currentShaders);

	//Export the shader
	ExportShader("Vertex", program, g_currentShaders.m_vertexShader, g_currentShaders.m_vertexSource);
	ExportShader("Fragment", program, g_currentShaders.m_pixelShader, g_currentShaders.m_fragmentSource);
	

	// Link the original program now that the correct shaders are swapped
	(*orig_glLinkProgram)(program);

	//Check for error
	GLint LinkOK = 0;
	orig_glGetProgramiv = (func_glGetProgramiv_t)orig_wglGetProcAddress("glGetProgramiv");
	(*orig_glGetProgramiv)(program, GL_LINK_STATUS, &LinkOK);
	if (LinkOK == GL_FALSE)
	{
		CheckLinkState(program);
	}

	// Make the shaders null for next round
	//Make our shaders NULL first
	g_currentShaders.m_pixelShader = 0;
	g_currentShaders.m_vertexShader = 0;
	g_currentShaders.m_fragmentSource = "";
	g_currentShaders.m_vertexSource = "";
}
///-------------------------------------------------------------------------------------------

// add the stereo values
void sys_glUseProgram(GLuint program)
{
	// Get the shaderManager
	ShaderManager * shaderManager = ShaderManager::getInstance();

	if (shaderManager->GetVertexInjectionState())
	{
		//Apply the shaders
		shaderManager->ApplyExceptionShaders();

#ifdef DEBUG_WRAPPER
		shaderManager->ApplyDebugExceptionShaders();
#endif
	}

	// Use the original program
	(*orig_glUseProgram)(program);

	if (shaderManager->GetVertexInjectionState())
	{
		// Get the locations of the uniforms
		GLfloat value;
		const GLchar *uniform_eye = (GLchar*)"g_eye";
		const GLchar *uniform_eye_spearation = (GLchar*)"g_eye_separation";
		const GLchar *uniform_convergence = (GLchar*)"g_convergence";


		// Get our functions from the driver. Be sure to only take them once
		if (orig_glGetUniformLocation == NULL)
		{
			orig_glGetUniformLocation = (func_glGetUniformLocation_t)(orig_wglGetProcAddress)("glGetUniformLocation");
		}
		//glUniform1f
		if (orig_glUniform1f == NULL)
		{

			orig_glUniform1f = (func_glUniform1f_t)(orig_wglGetProcAddress)("glUniform1f");
		}

		if ((GLint)program >= 0)
		{
			GLint location_eye = (*orig_glGetUniformLocation)(program, uniform_eye);
			GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
			GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);

			//set the uniform inside the shaders
			if (NV3DVisionGetCurrentFrame() == 1)
			{
				//left eye
				value = -1.0f;
				(*orig_glUniform1f)(location_eye, value);
			}
			else
			{
				//right eye
				value = +1.0f;
				(*orig_glUniform1f)(location_eye, value);
			}
			//Set the Separation and convergence
			(*orig_glUniform1f)(location_eye_separation, Get3DEyeSeparation());
			(*orig_glUniform1f)(location_convergence, Get3DConvergence());
		}

		/////////////////////////////////////
		// USED TO DISABLE ALOT OF SHADERS
		/////////////////////////////////////
#ifdef	DEBUG_WRAPPER
		if (shaderManager->VertexShadersExceptionsEnabled())
		{
			if (((GLint)program >= (GLint)shaderManager->GetExceptionShaderVertexStart() && (GLint)program <= (GLint)shaderManager->GetExceptionShaderVertexEnd()))
			{
				GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
				GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);

				//Set the Separation and convergence
				(*orig_glUniform1f)(location_eye_separation, 0);
				(*orig_glUniform1f)(location_convergence, 0);
			}
		}
#endif	
	}
}

//********************************************************************************************
// END SHADER MANIPULATION
//********************************************************************************************

PROC sys_wglGetProcAddress(LPCSTR ProcName)
{
	std::string injectionPoint = g_reader->GetInjectionPoint();

	if (!strcmp(ProcName, "glUseProgram"))
	{
		orig_glUseProgram = (funct_glUseProgram_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glUseProgram;
	}
	// ARB Variant
	else if (!strcmp(ProcName, "glUseProgramObjectARB"))
	{
		orig_glUseProgramObjectARB = (funct_glUseProgram_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glUseProgramObjectARB;
	}
	else if (!strcmp(ProcName, "glCreateProgram"))
	{
		orig_glCreateProgram = (funct_glCreateProgram_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glCreateProgram;
	}
	else if (!strcmp(ProcName, "glShaderSource"))
	{
		orig_glShaderSource = (funct_glShaderSource_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glShaderSource;
	}
	// we do the normal rendering Not printing the debug info
	else if (!strcmp(ProcName, injectionPoint.c_str()) && (!g_reader->GetPrintDebugInfoState()))
	{
		char result[100];
		sprintf_s(result, "3D Vision Hooking point is: %s", injectionPoint.c_str());
		add_log(result);
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glBindFramebuffer;
	}
	else if (!strcmp(ProcName, "glBindFramebuffer") && (g_reader->GetPrintDebugInfoState()))
	{
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)log_glBindFramebuffer;
	}
	else if (!strcmp(ProcName, "glBindFramebufferEXT") && (g_reader->GetPrintDebugInfoState()))
	{
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)log_glBindFramebufferEXT;
	}
	else if (!strcmp(ProcName, "glBindFramebufferARB") && (g_reader->GetPrintDebugInfoState()))
	{
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)log_glBindFramebufferARB;
	}
	else if (!strcmp(ProcName, "glLinkProgram"))
	{
		orig_glLinkProgram = (func_glLinkProgram_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glLinkProgram;
	}
	else if (!strcmp(ProcName, "glCompileShader"))
	{
		orig_glCompileShader = (func_glCompileShader)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glCompileShader;
	}
	return orig_wglGetProcAddress(ProcName);
}
///-------------------------------------------------------------------------------------------

const GLubyte * sys_glGetString(GLenum name)
{
	return (*orig_glGetString) (name);
}
///-------------------------------------------------------------------------------------------

//Initializes and renders
void sys_wglSwapBuffers(HDC hDC)
{	
	//Avoid warning
	(void)hDC;
	
	// If we have the injection point enabled
	if (g_reader->Get3DVisionEnabledStatus())
	{
		if (g_reader->IsAutomaticHookMode())
		{
			// If Automatic Hooking point is enabled
			AutomaticGlSwapBuffers();

			add_log("Automatic Hooking Enabled !!!");
		}
		else
		{
			add_log("Manual Hooking Enabled SwapBuffers !!!");

			// If Manual Hooking point is enabled
			std::string injectionPoint = g_reader->GetInjectionPoint();

			// if we swap buffer on framebuffer bind
			if (injectionPoint != "glSwapBuffers" && injectionPoint != "SCREEN_BUFFER")
			{
				bindFrameBufferSwap();
			}
			// if we swap buffer on wgl_swapbuffers
			else if (injectionPoint == "glSwapBuffers")
			{
				swapBuffersSwap();
			}
		}
	}
	else
	{
		if (g_reader->GetPrintDebugInfoState())
		{
			add_log("glSwapBuffers Called !");
		}
		(*orig_wglSwapBuffers)(hDC);
	}
}
///-------------------------------------------------------------------------------------------

/// Windows Variant of SwapBuffers

void sys_wglSwapLayerBuffers(HDC hdc, UINT fuPlanes)
{
	//Avoid warning
	(void)hdc;
	(void)fuPlanes;

	// If we have the injection point enabled
	if (g_reader->Get3DVisionEnabledStatus())
	{
		if (g_reader->IsAutomaticHookMode())
		{
			// If Automatic Hooking point is enabled
			AutomaticGlSwapBuffers();

			add_log("Automatic Hooking Enabled !!!");
		}
		else
		{
			add_log("Manual Hooking Enabled SwapLayerBuffers!!!");

			// If Manual Hooking point is enabled
			std::string injectionPoint = g_reader->GetInjectionPoint();

			// if we swap buffer on framebuffer bind
			if (injectionPoint != "glSwapBuffers" && injectionPoint != "SCREEN_BUFFER")
			{
				bindFrameBufferSwap();
			}
		}
	}
	else
	{
		if (g_reader->GetPrintDebugInfoState())
		{
			add_log("wglSwapLayersBuffers Called !");
		}
		(*orig_wglSwapLayerBuffers)(hdc, fuPlanes);
	}
}
///-------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
/// AUTOMATIC SWAPPING
//////////////////////////////////////////////////////////////////////////

// The last Step before actually swapping the buffers
static void EndFrames(void)
{
	// Copy the right eye
	if (!g_windowResizeCheck)
	{
		std::string injectionPoint = g_reader->GetInjectionPoint();
		if (injectionPoint != "glSwapBuffers" && injectionPoint != "SCREEN_BUFFER")
		{
			GLD3DBuffers_flip_right(gl_d3d_buffers);
		}
		else if (injectionPoint == "SCREEN_BUFFER")
		{
			GLD3DBuffers_copy_right(gl_d3d_buffers);
		}
	}

	// Deactivate 
	GLD3DBuffers_deactivate_right(gl_d3d_buffers);
	GLD3DBuffers_deactivate(gl_d3d_buffers);

	// We rendered everything so first we need to flush.
	GLD3DBuffers_flush(gl_d3d_buffers);

	//check for resize
	if (g_windowResizeCheck == true)
	{
		// create DirectX 3D Vision buffers
		GLD3DBuffers_destroy(gl_d3d_buffers);

		//Resize complete
		g_windowResizeCheck = false;

		// Create the new buffers
		GLD3DBuffers_create(gl_d3d_buffers, handle, true);
	}
}
///-------------------------------------------------------------------------------------------

//Initializes and renders
static void AutomaticGlSwapBuffers(void)
{
	//initializes
	if (NV3DVisionIsNotInit())
	{
		//Start our window resize checker thread if the mode is enabled.
		if (g_reader->GetWindowResizeEnabled())
		{
			std::thread ResizeCheck(windowResize);
			ResizeCheck.detach();
		}

		// Load our extensions
		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
		{
			add_log("Failed to get OpenGL extensions (ogl_LOAD_FAILED)");
		}
		//Create our buffers
		gl_d3d_buffers = new GLD3DBuffers;

		handle = GetActiveWindow();
		GLD3DBuffers_create(gl_d3d_buffers, handle, true);

		//start the rendering
		NV3DVisionSetCurrentFrame(1);

		// FPS Inject
		if (isFPSInjectHandleValid())
		{
			m_FPSInjector->synchronizeRendering();
		}
	}
	// If we are initialised
	if (!NV3DVisionIsNotInit())
	{
		if (NV3DVisionGetCurrentFrame() == 2)
		{
			EndFrames();
			GLD3DBuffers_activate_depth(gl_d3d_buffers);
			GLD3DBuffers_activate_left(gl_d3d_buffers);
			NV3DVisionSetCurrentFrame(1);

			// Sync Left eye
			if (isFPSInjectHandleValid())
			{
				m_FPSInjector->syncLeft();
			}
		}
		else if (NV3DVisionGetCurrentFrame() == 1)
		{
			// Sync Right eye
			if (isFPSInjectHandleValid())
			{
				m_FPSInjector->syncRight();
			}

			if (!g_windowResizeCheck)
			{
				std::string injectionPoint = g_reader->GetInjectionPoint();
				if (injectionPoint != "glSwapBuffers" && injectionPoint != "SCREEN_BUFFER")
				{
					GLD3DBuffers_flip_left(gl_d3d_buffers);
				}
				else if (injectionPoint == "SCREEN_BUFFER")
				{
					GLD3DBuffers_copy_left(gl_d3d_buffers);
				}
			}
			GLD3DBuffers_deactivate_left(gl_d3d_buffers);
			GLD3DBuffers_activate_right(gl_d3d_buffers);

			NV3DVisionSetCurrentFrame(2);
		}
	}
}
///-------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
/// END OF AUTOMATIC SWAPPING
//////////////////////////////////////////////////////////////////////////

static void windowResize()
{
	RECT windowRect;
	unsigned int width;
	unsigned int height;
	for (;;)
	{
		GetClientRect(handle, &windowRect);
		width = windowRect.right - windowRect.left;
		height = windowRect.bottom - windowRect.top;

		if ((handle != 0x00) && ((width != g_width) || (height != g_height)))
		{
			g_windowResizeCheck = true;
			add_log("Resize Triggered");
		}
		Sleep(500);
	}
}
///-------------------------------------------------------------------------------------------

// MAIN ENTRY
#pragma warning(disable:4100)
BOOL __stdcall DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	//Read the config file
	g_reader = new configReader;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hOriginalDll);

		// Splash Screen enabled?
		if (g_reader->GetEnableSplashScreen())
		{
			// SDK Message box
			CustomMessageBox(NULL, _T("  \t\t\t\t\t\t               \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"),
				_T("OpenGL3DVision Wrapper v.2.00 Beta"), MB_OK);
		}
		// Init the wrapper
		LoadFPSInject();
		Init();
		break;

	case DLL_PROCESS_DETACH:
		if (hOriginalDll != NULL)
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			add_log("Disabling 3D Vision... Cleaning up and freeing the resources...\n");
			// Removing the FPS inject
			(FPSDestroy)(m_FPSInjector);
			FreeLibrary(FPSHandle);
			// The the openGL32.dll lib
			FreeLibrary(hOriginalDll);
			add_log("Application closed successfully !\nEND\n");
			hOriginalDll = NULL;
		}
		break;
	}
	return TRUE;
}
#pragma warning(default:4100)
///-------------------------------------------------------------------------------------------

void __cdecl add_log(const char * fmt, ...)
{
	if (g_reader->GetEnableLog())
	{
		va_list va_alist;
		char logbuf[5000] = "";
		FILE * fp;
		errno_t err;

		va_start(va_alist, fmt);
		_vsnprintf_s(logbuf + strlen(logbuf), 5000, sizeof(logbuf)-strlen(logbuf), fmt, va_alist);
		va_end(va_alist);

		//if ((fp = fopen("3DVisionWrapper.log", "ab")) != NULL)
		err = fopen_s(&fp, "3DVisionWrapper.log", "ab");
		if (err == 0)
		{
			fprintf(fp, "%s", logbuf);
			fprintf(fp, "\n");
			fclose(fp);
		}
	}
}
///-------------------------------------------------------------------------------------------

static void LoadFPSInject(void)
{
	// Load the library
	FPSHandle = LoadLibrary(FPSINJECT_DLL);

	if (FPSHandle != 0)
	{
		// Get the pointers
		FPScreate = (create)GetProcAddress(FPSHandle, "create_object");
		FPSDestroy = (destroy)GetProcAddress(FPSHandle, "destroy_object");

		// Assign the pointer
		m_FPSInjector = (FPScreate)();
	}
}
///-------------------------------------------------------------------------------------------

static bool isFPSInjectHandleValid(void)
{
	bool ret = false;

	if (FPSHandle != 0)
	{
		ret = true;
	}

	return ret;
}
///-------------------------------------------------------------------------------------------