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
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
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
#include "wrapper_version.h"
#include "WindowManager.h"
#include "crc32.h"

// Mutex
static std::recursive_mutex m_opengl32Mutex;

// FPSInject
HINSTANCE FPSHandle;
typedef FPSInject* (__cdecl *create)();
typedef void(*destroy)(FPSInject*);
create FPScreate;
destroy FPSDestroy;
static void LoadFPSInject(void);
static bool isFPSInjectHandleValid(void);
static bool m_areExtensionsLoade = false;

// Our object
FPSInject* m_FPSInjector;

// Window Information
unsigned int g_width = 0;
unsigned int g_height = 0;
HWND g_MainWindowHandle;
bool g_windowResizeCheck = false;
// Enable hybrid drawing Mode both on SwapBuffers and BindFbo
static BOOL fboDrawingEnabled = FALSE;
// 3D Vision
GLD3DBuffers * gl_d3d_buffers;

//Config reader
extern configReader *g_reader;

// Automatic GL_SWAPBUFFERs - Auto hook point
static void AutomaticGlSwapBuffers(void);

// End frame method used in automatic hooking 
static void EndFrames(void);

#ifdef DEBUG_WRAPPER
int debugVertexIndex = -1;
int debugPixelIndex = -1;
bool printCurrentShader = false;
bool isCurrentShaderVertex = true;
bool isCompileEnable = false;
#endif

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
				NV3DVisionBuffers_activate_depth(gl_d3d_buffers);
				NV3DVisionBuffers_activate_left(gl_d3d_buffers);
				fboDrawingEnabled = TRUE;
			}
			else if ((NV3DVisionGetCurrentFrame() == 2))
			{
				NV3DVisionBuffers_activate_right(gl_d3d_buffers);
				fboDrawingEnabled = TRUE;
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

//********************************************************************************************
// SHADER MANIPULATION
//********************************************************************************************

GLuint sys_glCreateProgram(void)
{
	GLuint program = (*orig_glCreateProgram)();
	return program;
}
///-------------------------------------------------------------------------------------------

// We will inject the stereo here 
void sys_glAttachShader(GLuint program, GLuint shader)
{
	m_opengl32Mutex.lock();

	orig_glCompileShader = (func_glCompileShader)orig_wglGetProcAddress("glCompileShader");
	if (orig_glCompileShader == 0x0)
		add_log("glCompileShader not found !!!!");
	orig_glGetShaderiv = (func_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
	if (orig_glGetShaderiv == 0x0)
		add_log("glGetShaderiv not found !!!!");

	// Add the CRC of the program
	std::string programSring = std::to_string(program);
	DWORD progCRC32 = crc32buf(programSring.c_str(), programSring.length());
	
	// Get the correct shader type
	// Get the instance
	ShaderManager *shaderManager = ShaderManager::getInstance();
	
	// Depends on what type of shader it is we do stuff
	if (shaderManager->isShaderType(shader, GL_VERTEX_SHADER))
	{
		//get the original shader Source
		std::string shaderSource = shaderManager->getShaderSource(shader);

		// Calculate the CRC32 before we do any injection
		// Otherwise the CRC32 will change
		DWORD crc32 = crc32buf(shaderSource.c_str(), shaderSource.length());
		crc32 += progCRC32;

		if (shaderManager->GetVertexInjectionState())
		{
			//Apply the custom shaders
			// If we failed apply normal injection
			if (shaderManager->ApplyExceptionShaders(shaderSource, GL_VERTEX_SHADER, crc32) == false)
			{
				//Insert Stereo
				shaderSource = shaderManager->injectStereoScopy(shaderSource, program);
			}
		}

		// Swap the Source
		shaderManager->applyShaderSource(shader, shaderSource, NULL);

		// Store it as an existing shader
		EXISTING_SHADER_T currentShader;
		currentShader.m_CRC32 = crc32;
		currentShader.m_programId = program;
		currentShader.m_shaderId = shader;
		currentShader.m_shaderSourceCode = shaderSource;
		currentShader.m_shaderType = GL_VERTEX_SHADER;
		// Push it back
		shaderManager->addExistingShaderInfo(currentShader);
		
		// Export the Source
		ExportShader("Vertex", currentShader.m_programId, currentShader.m_CRC32, currentShader.m_shaderSourceCode);

		// We also need to compile the shader before attaching it
		//Compile shader
		(*orig_glCompileShader)(shader);

		//Test compile
		GLint statusOk = 0;
		(*orig_glGetShaderiv)(shader, GL_COMPILE_STATUS, &statusOk);

		if (statusOk == GL_FALSE)
		{
			CheckCompileState(shader);
		}
	}
	else if (shaderManager->isShaderType(shader, GL_FRAGMENT_SHADER))
	{
		//get the original shader Source
		std::string shaderSource = shaderManager->getShaderSource(shader);

		// Calculate the CRC32
		DWORD crc32 = crc32buf(shaderSource.c_str(), shaderSource.length());
		crc32 += progCRC32;

		if (shaderManager->GetVertexInjectionState())
		{
			//Apply the custom shaders
			// If we failed apply normal injection
			if (shaderManager->ApplyExceptionShaders(shaderSource, GL_FRAGMENT_SHADER, crc32) == false)
			{
				// This only happens in development mode.
#ifdef DEBUG_WRAPPER
				//Insert Stereo
				shaderSource = shaderManager->injectFragmentModification(shaderSource, program);
#endif
			}
		}

		// Swap the Source
		shaderManager->applyShaderSource(shader, shaderSource, NULL);

		// Store it as an existing shader
		EXISTING_SHADER_T currentShader;
		currentShader.m_CRC32 = crc32;
		currentShader.m_programId = program;
		currentShader.m_shaderId = shader;
		currentShader.m_shaderSourceCode = shaderSource;
		currentShader.m_shaderType = GL_FRAGMENT_SHADER;
		// Push it back
		shaderManager->addExistingShaderInfo(currentShader);

		// Export the Source
		ExportShader("Pixel", currentShader.m_programId, currentShader.m_CRC32, currentShader.m_shaderSourceCode);

		//Compile shader
		(*orig_glCompileShader)(shader);

		// We also need to compile the shader before attaching it
		//Test compile
		GLint statusOk = 0;
		(*orig_glGetShaderiv)(shader, GL_COMPILE_STATUS, &statusOk);

		if (statusOk == GL_FALSE)
		{
			CheckCompileState(shader);
		}
	}
	
	// We attach after we swapped the sources
	(*orig_glAttachShader)(program, shader);

	m_opengl32Mutex.unlock();
}
///-------------------------------------------------------------------------------------------

void sys_glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
	// Apply the original source
	(*orig_glShaderSource)(shader, count, string, length);
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
}
///-------------------------------------------------------------------------------------------

// add the stereo values
void sys_glUseProgram(GLuint program)
{
	m_opengl32Mutex.lock();

	// Get the shaderManager
	ShaderManager * shaderManager = ShaderManager::getInstance();

#ifdef DEBUG_WRAPPER
	if (shaderManager->GetVertexInjectionState())
	{
		shaderManager->ApplyDebugExceptionShaders();
	}
#endif

	// Use the original program
	(*orig_glUseProgram)(program);

	if (shaderManager->GetVertexInjectionState())
	{
		// Get the locations of the uniforms
		GLfloat value;
		const GLchar *uniform_eye = (GLchar*)"g_eye";
		const GLchar *uniform_eye_spearation = (GLchar*)"g_eye_separation";
		const GLchar *uniform_convergence = (GLchar*)"g_convergence";
		const GLchar *uniform_vertexEnabled = (GLchar*)"g_vertexEnabled";
		const GLchar *uniform_pixelEnabled = (GLchar*)"g_pixelEnabled";


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
			GLint location_vertexEnabled = (*orig_glGetUniformLocation)(program, uniform_vertexEnabled);
			GLint location_pixelEnabled = (*orig_glGetUniformLocation)(program, uniform_pixelEnabled);

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
			(*orig_glUniform1f)(location_vertexEnabled, 1.0f);
			(*orig_glUniform1f)(location_pixelEnabled, 1.0f);
		}

		/////////////////////////////////////
		// APPLY CUSTOM SHADER PARAMS
		/////////////////////////////////////
		shaderManager->ApplyCustomShaderParams(program);
		
		/////////////////////////////////////
		// USED TO DISABLE ALOT OF SHADERS
		/////////////////////////////////////
#ifdef	DEBUG_WRAPPER
		if (shaderManager->VertexShadersExceptionsEnabled())
		{
			if (((GLint)program >= (GLint)shaderManager->GetExceptionShaderStart() && (GLint)program <= (GLint)shaderManager->GetExceptionShaderEnd()))
			{
				GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
				GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);

				//Set the Separation and convergence
				(*orig_glUniform1f)(location_eye_separation, 0);
				(*orig_glUniform1f)(location_convergence, 0);

				// Do we disable the vertex shaders ???
				if (shaderManager->VertexShaderExceptionsDisableShader())
				{
					GLint location_enabled = (*orig_glGetUniformLocation)(program, uniform_vertexEnabled);
					(*orig_glUniform1f)(location_enabled, 0.0f);
				}
				else
				{
					GLint location_enabled = (*orig_glGetUniformLocation)(program, uniform_vertexEnabled);
					(*orig_glUniform1f)(location_enabled, 1.0f);
				}
			}
			if (shaderManager->DisableCurrentShader())
			{
				// If we are navigating vertexes
				if (isCurrentShaderVertex)
				{
					// Disable the current Shader
					if (debugVertexIndex == (int)program)
					{
						GLint location_enabled = (*orig_glGetUniformLocation)(program, uniform_vertexEnabled);
						(*orig_glUniform1f)(location_enabled, 0.0f);
					}
				}
				else // Pixel Shader
				{
					// Disable the current Shader
					if (debugPixelIndex == (int)program)
					{
						GLint location_enabled = (*orig_glGetUniformLocation)(program, uniform_pixelEnabled);
						(*orig_glUniform1f)(location_enabled, 0.0f);
					}
				}
			}
		}
#endif	
	}

	m_opengl32Mutex.unlock();
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
	else if (!strcmp(ProcName, "glAttachShader"))
	{
		orig_glAttachShader = (func_glAttachShader_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glAttachShader;
	}
	else if (!strcmp(ProcName, "glShaderSource"))
	{
		orig_glShaderSource = (funct_glShaderSource_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glShaderSource;
	}
	else if (!strcmp(ProcName, "glBindFramebuffer"))
	{
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glBindFramebuffer;
	}
	else if (!strcmp(ProcName, "glBindFramebufferEXT"))
	{
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glBindFramebuffer;
	}
	else if (!strcmp(ProcName, "glBindFramebufferARB"))
	{
		orig_glBindFramebuffer = (func_glBindFramebuffer_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glBindFramebuffer;
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
		// Automatic Hooking point
		AutomaticGlSwapBuffers();
	}
	else
	{
		add_log("glSwapBuffers Called !");
		(*orig_wglSwapBuffers)(hDC);
	}
	fboDrawingEnabled = FALSE;
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
		// Automatic Hooking point is enabled
		AutomaticGlSwapBuffers();
	}
	else
	{
		add_log("wglSwapLayersBuffers Called !");
		(*orig_wglSwapLayerBuffers)(hdc, fuPlanes);
	}
	fboDrawingEnabled = FALSE;
}
///-------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
/// AUTOMATIC SWAPPING
//////////////////////////////////////////////////////////////////////////

// The last Step before actually swapping the buffers
static void EndFrames(void)
{
	// Copy the right eye
	if (g_windowResizeCheck == false)
	{
		std::string injectionPoint = g_reader->GetInjectionPoint();
		if (injectionPoint != "glSwapBuffers" && injectionPoint != "SCREEN_BUFFER")
		{
			if (fboDrawingEnabled)
				NV3DVisionBuffers_copy_right(gl_d3d_buffers);
			else
				NV3DVisionBuffers_flip_right(gl_d3d_buffers);
		}
		else if (injectionPoint == "SCREEN_BUFFER")
		{
			NV3DVisionBuffers_copy_right(gl_d3d_buffers);
		}
	}

	// Deactivate 
	NV3DVisionBuffers_deactivate_right(gl_d3d_buffers);
	NV3DVisionBuffers_deactivate(gl_d3d_buffers);

	// We rendered everything so first we need to flush.
	NV3DVisionBuffers_flush(gl_d3d_buffers);

	//check for resize
	if (g_windowResizeCheck == true)
	{
		// Create the new buffers
		g_MainWindowHandle = FindWindowFromProcessId();
		// create DirectX 3D Vision buffers
		NV3DVisionReInitBuffers();
		NV3DVisionBuffers_destroy(gl_d3d_buffers);

		//Resize complete
		g_windowResizeCheck = false;

		NV3DVisionBuffers_create(gl_d3d_buffers, g_MainWindowHandle, true);
	}
}
///-------------------------------------------------------------------------------------------

//Initializes and renders
static void AutomaticGlSwapBuffers(void)
{
	//initializes
	if (NV3DVisionIsNotInit())
	{
		// Print just once:)
		add_log("Automatic Hooking Enabled !!!");

		// Load our extensions
		if (!m_areExtensionsLoade)
		{
			if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
			{
				add_log("Failed to get OpenGL extensions (ogl_LOAD_FAILED)");
			}
			else
			{
				add_log("OpenGL extensions Retrieved !!!");
			}
			m_areExtensionsLoade = true;
		}

		//Create our buffers
		gl_d3d_buffers = new GLD3DBuffers;

		g_MainWindowHandle = FindWindowFromProcessId();
		NV3DVisionBuffers_create(gl_d3d_buffers, g_MainWindowHandle, true);

		// Start our Threads
		StartResizeThread();
		StartNVAPIThread();

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
			NV3DVisionBuffers_activate_depth(gl_d3d_buffers);
			NV3DVisionBuffers_activate_left(gl_d3d_buffers);
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

			if (g_windowResizeCheck == false)
			{
				std::string injectionPoint = g_reader->GetInjectionPoint();
				if (injectionPoint != "glSwapBuffers" && injectionPoint != "SCREEN_BUFFER")
				{
					if (fboDrawingEnabled)
						NV3DVisionBuffers_copy_left(gl_d3d_buffers);
					else
						NV3DVisionBuffers_flip_left(gl_d3d_buffers);
					
				}
				else if (injectionPoint == "SCREEN_BUFFER")
				{
					NV3DVisionBuffers_copy_left(gl_d3d_buffers);
				}
			}
			NV3DVisionBuffers_deactivate_left(gl_d3d_buffers);
			NV3DVisionBuffers_activate_right(gl_d3d_buffers);

			NV3DVisionSetCurrentFrame(2);
		}
	}
}
///-------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
/// END OF AUTOMATIC SWAPPING
//////////////////////////////////////////////////////////////////////////

// MAIN ENTRY
BOOL __stdcall DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	(void)lpvReserved;
	(void)hinstDll;

	//Read the config file
	g_reader = new configReader;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hOriginalDll);

		// Splash Screen enabled?
		if (g_reader->GetEnableSplashScreen())
		{
			char info[255];
			sprintf_s(info, "OpenGL3DVision Wrapper v.%s", OGL3DVISION_VERSION_STR);
			// SDK Message box
			CustomMessageBox(NULL, _T("  \t\t\t\t\t\t               \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"),
				_T(info), MB_OK);
		}
		// Init the wrapper
		LoadFPSInject();
		Init();
		break;

	case DLL_PROCESS_DETACH:
		if (hOriginalDll != NULL)
		{
			// Stop the threads
			StopResizeThread();
			StoptNVAPIThread();
			Sleep(500);

			NV3DVisionBuffers_destroy(gl_d3d_buffers);
			add_log("\nDisabling 3D Vision... Cleaning up and freeing the resources...");
			// Removing the FPS inject
			if (FPSDestroy)
			{
				(FPSDestroy)(m_FPSInjector);
			}
			FreeLibrary(FPSHandle);
			add_log("Application closed successfully !\nEND\n");
			// The the openGL32.dll lib
			FreeLibrary(hOriginalDll);
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

DWORD GetMainThreadId(void) 
{
	const std::tr1::shared_ptr<void> hThreadSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
	if (hThreadSnapshot.get() == INVALID_HANDLE_VALUE) 
	{
		add_log("GetMainThreadId failed");
		return 0;
	}
	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(THREADENTRY32);
	DWORD result = 0;
	DWORD currentPID = GetCurrentProcessId();
	for (BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
		!result && success && GetLastError() != ERROR_NO_MORE_FILES;
		success = Thread32Next(hThreadSnapshot.get(), &tEntry))
	{
		if (tEntry.th32OwnerProcessID == currentPID) 
		{
			result = tEntry.th32ThreadID;
		}
	}
	return result;
}
///-------------------------------------------------------------------------------------------