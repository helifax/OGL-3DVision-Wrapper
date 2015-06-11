/*
* Copyright Helifax 2014
*/

extern "C" {
#include "opengl_3dv.h"
}
#include "opengl32.h"
#include <string>
#include "include\ShaderManager.h"
#include "windows.h"
#include "d3d9.h"
#include "NVAPI_331\nvapi.h"
#include <thread>
#include "FPSInject.h"
#include "MessageBox\MessageBox.h"

static bool g_init = true;
static unsigned int frameCall = 0;
ShaderManager g_shaderManager;

unsigned int g_width = 0;
unsigned int g_height = 0;
bool g_windowResizeCheck = false;
static HWND handle;
bool g_swapConvergence = false;
GLD3DBuffers * gl_d3d_buffers;
std::vector<SHADER_EXCEPT_T> g_exceptShaders;

#ifdef NDEBUG
std::vector<SHADER_DEBUG_T> g_debug_ShaderPair;
int debugShaderIndex = 0;
int lastValidIndex = 0;
std::string debugOrigSource;
bool injected = false;
bool restoreDefault = false;
#endif

/*
static DWORD64 getHexRepresentation(float _number)
{
	DWORD64 result;
	result = *((unsigned int *)&_number);
	return result;
}
*/

//static void windowResize();

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

// Initialize 3D vision on first glClear color ever used
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

//
void sys_glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	
	// We are rendering the left frame, so let it run natively
	if (framebuffer == GLuint(0))
	{
		if ((frameCall == 1))
		{
			GLD3DBuffers_activate_left(gl_d3d_buffers);
		}
		else if ((frameCall == 2))
		{

			GLD3DBuffers_activate_right(gl_d3d_buffers);
		}
	}
	else //not our framebuffer 0
	{
		//Original Frame buffer
		(*orig_glBindFramebuffer)(target, framebuffer);
	}
	
	//(*orig_glBindFramebufferEXT)(target, framebuffer);
}

void sys_glLinkProgram(GLuint program)
{
	(*orig_glLinkProgram)(program);
}

// add the stereo values
void sys_glUseProgram(GLuint program)
{
	// Use the original program
	(*orig_glUseProgram)(program);

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

	if (program != 0 && program != 1)
	{
		GLint location_eye = (*orig_glGetUniformLocation)(program, uniform_eye);
		GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
		GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);

		//set the uniform inside the shaders
		if (frameCall == 1)
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
		//Set the Separation and convergenceB
		(*orig_glUniform1f)(location_eye_separation, g_eyeSeparation);
		(*orig_glUniform1f)(location_convergence, g_convergence);

		GLsizei shaderCount;
		GLuint shaders[2] = { 0, 0 };
		glGetAttachedShaders(program, 4, &shaderCount, shaders);
		
		
#ifdef NDEBUG
		bool shadersFound = false;

		if (g_debug_ShaderPair.size() != 0)
		{
			for (int i = 0; i < g_debug_ShaderPair.size(); i++)
			{
				// we push only if it doesnt exist
				if (program == g_debug_ShaderPair[i].m_programId && g_debug_ShaderPair[i].m_pixelShader == shaders[0] && g_debug_ShaderPair[i].m_vertexShader == shaders[1])
				{
					shadersFound = true;
				}
			}
			if (!shadersFound)
			{
				addDebugtShaderPair(program, shaders[1], shaders[0]);
			}
		}
		else
		{
			addDebugtShaderPair(program, shaders[1], shaders[0]);
		}
		
		//Pixel Only
		if (debugShaderIndex < g_debug_ShaderPair.size())
		{
			GLint params;
			glGetShaderiv(g_debug_ShaderPair[debugShaderIndex].m_pixelShader, GL_SHADER_SOURCE_LENGTH, &params);
			GLchar currentSource[50000];
			glGetShaderSource(g_debug_ShaderPair[debugShaderIndex].m_pixelShader, 50000, &params, currentSource);

			if (g_shaderManager.isShaderType(g_debug_ShaderPair[debugShaderIndex].m_pixelShader, GL_FRAGMENT_SHADER))
			{
				std::string debugSource = currentSource;
				
				if (!injected && restoreDefault == false)
				{
					debugOrigSource = currentSource;
					lastValidIndex = debugShaderIndex;
					injected = true;

					//Insert Debug Information
					debugSource = g_shaderManager.debug_injectFragmentModification(debugSource);
					//Apply Source
					params = debugSource.length();
					g_shaderManager.applyShaderSource(g_debug_ShaderPair[debugShaderIndex].m_pixelShader, debugSource, &params);
					//Compile
					glCompileShader(g_debug_ShaderPair[debugShaderIndex].m_pixelShader);

					//Test compile
					GLint isCompiled = 0;
					glGetShaderiv(shaders[0], GL_COMPILE_STATUS, &isCompiled);

					if (isCompiled != GL_FALSE)
					{
						//Attach
						glAttachShader(g_debug_ShaderPair[debugShaderIndex].m_programId, g_debug_ShaderPair[debugShaderIndex].m_pixelShader);
						glLinkProgram(g_debug_ShaderPair[debugShaderIndex].m_programId);
						GLenum test = glGetError();
					}
				}

				// 
				if (restoreDefault == true && lastValidIndex >= 0)
				{
					if (debugOrigSource.size() != 0)
					{
						g_shaderManager.applyShaderSource(g_debug_ShaderPair[lastValidIndex].m_pixelShader, debugOrigSource, &params);

						//Compile
						glCompileShader(g_debug_ShaderPair[lastValidIndex].m_pixelShader);

						//Test compile
						GLint isCompiled = 0;
						glGetShaderiv(shaders[0], GL_COMPILE_STATUS, &isCompiled);

						if (isCompiled != GL_FALSE)
						{
							//Attach
							glAttachShader(g_debug_ShaderPair[lastValidIndex].m_programId, g_debug_ShaderPair[lastValidIndex].m_pixelShader);
							glLinkProgram(g_debug_ShaderPair[lastValidIndex].m_programId);
							GLenum test = glGetError();
						}
					}
					restoreDefault = false;
				}
			}
		}
#endif
		
		// The normal injection
#if 1
		for (unsigned int i = 0; i < g_exceptShaders.size(); i++)
		{
			// Vertex & Pixel
			if (g_exceptShaders[i].m_pixelShader == shaders[0] && g_exceptShaders[i].m_vertexShader == shaders[1])
			{
				//SSAO
				if (788 == shaders[0] && 58 == shaders[1])
				{
					//(*orig_glUniform1f)(location_eye_separation, (GLfloat)(g_eyeSeparation*3.129)); // High
					(*orig_glUniform1f)(location_eye_separation, (GLfloat)(g_eyeSeparation*3.589)); // Medium
					if (frameCall == 1)
					{
						//left eye
						value = 1.0f;
						(*orig_glUniform1f)(location_eye, value);
					}
					else
					{
						//right eye
						value = -1.0f;
						(*orig_glUniform1f)(location_eye, value);
					}
				}
				// Face blood
				else if (363 == shaders[0] && 256 == shaders[1])
				{
					GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
					(*orig_glUniform1f)(location_eye_separation, (GLfloat)(g_eyeSeparation / 1.029));
				}
				// DOF
				else if (879 == shaders[0] && 58 == shaders[1])
				{
					//Fragment no addition
					GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
					(*orig_glUniform1f)(location_eye_separation, 0);

					//Pixel Shader
					const GLchar *uniform_eye_spearation_pixel = (GLchar*)"g_eye_separation_pixel";
					GLint location_eye_separation_pixel = (*orig_glGetUniformLocation)(program, uniform_eye_spearation_pixel);
					if (frameCall == 1)
					{
						(*orig_glUniform1f)(location_eye_separation_pixel, -g_eyeSeparation * 4);
					}
					else if (frameCall == 2)
					{
						(*orig_glUniform1f)(location_eye_separation_pixel, g_eyeSeparation * 4);
					}
				}
				else if (495 == shaders[0] && 89 == shaders[1])
				{
					//fog and shiet
					GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
					(*orig_glUniform1f)(location_eye_separation, g_eyeSeparation);
				}
				//Main Menu
				else if (61 == shaders[0] && 0 == shaders[1])
				{
					(*orig_glUniform1f)(location_eye_separation, (GLfloat)0.02);
					(*orig_glUniform1f)(location_convergence, (GLfloat)1.971265);
				}
				//Main Menu
				else if (701 == shaders[0] && 188 == shaders[1])
				{
					(*orig_glUniform1f)(location_eye_separation, (GLfloat)0.02);
					(*orig_glUniform1f)(location_convergence, (GLfloat)1.971265);
				}
				//Main Menu
				else if (458 == shaders[0] && 124 == shaders[1])
					
				{
					(*orig_glUniform1f)(location_eye_separation, (GLfloat)0.02);
					(*orig_glUniform1f)(location_convergence, (GLfloat)1.971265);
				}
				//Main Menu
				else if (458 == shaders[0] && 51 == shaders[1])
				{
					// Breaks in-game stuff
					//(*orig_glUniform1f)(location_eye_separation, 0.02);
					//(*orig_glUniform1f)(location_convergence, 1.971265);
				}
				else
				{
					value = 0.0f;
					GLint location_eye = (*orig_glGetUniformLocation)(program, uniform_eye);
					GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
					GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);
					(*orig_glUniform1f)(location_eye, value);
					(*orig_glUniform1f)(location_eye_separation, value);
					(*orig_glUniform1f)(location_convergence, value);
				}
				// if found no need to search more
				break;
			}
		}
#endif
	}
}

// do our stereo here
void sys_glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
	// Make modifications only on the vertex shader
	if (g_shaderManager.isShaderType(shader, GL_VERTEX_SHADER))
	{
		//get the original shader Source
		std::string shdrSournce = g_shaderManager.getShaderSource(count, string);
		//Insert Stereo
		shdrSournce = g_shaderManager.injectStereoScopy(shdrSournce);
		// Compile it
		g_shaderManager.applyShaderSource(shader, shdrSournce, length);
	}
	else if (g_shaderManager.isShaderType(shader, GL_FRAGMENT_SHADER))
	{
		//get the original shader Source
		std::string shdrSournce = g_shaderManager.getShaderSource(count, string);
		//Insert Stereo
		shdrSournce = g_shaderManager.injectFragmentModification(shdrSournce);
		// Compile it
		g_shaderManager.applyShaderSource(shader, shdrSournce, length);
	}
	else
	{
		(*orig_glShaderSource)(shader, count, string, length);
	}
}

PROC sys_wglGetProcAddress(LPCSTR ProcName)
{
	if (!strcmp(ProcName, "glMultiTexCoord2fARB"))
	{
		orig_glMultiTexCoord2fARB = (func_glMultiTexCoord2fARB_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glMultiTexCoord2fARB;
	}
	else if (!strcmp(ProcName, "glBindTextureEXT"))
	{
		orig_BindTextureEXT = (func_BindTextureEXT_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_BindTextureEXT;
	}
	else if (!strcmp(ProcName, "glActiveTextureARB"))
	{
		orig_glActiveTextureARB = (func_glActiveTextureARB_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glActiveTextureARB;
	}

	else if (!strcmp(ProcName, "glUniformMatrix4fv"))
	{
		orig_glUniformMatrix4fv = (func_glUniformMatrix4fv_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glUniformMatrix4fv;
	}
	else if (!strcmp(ProcName, "glUseProgram"))
	{
		orig_glUseProgram = (funct_glUseProgram_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glUseProgram;
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
	else if (!strcmp(ProcName, "glLinkProgram"))
	{
		orig_glLinkProgram = (func_glLinkProgram_t)orig_wglGetProcAddress(ProcName);
		return (FARPROC)sys_glLinkProgram;
	}
	return orig_wglGetProcAddress(ProcName);
}

const GLubyte * sys_glGetString(GLenum name)
{
	add_log("sys_glGetString %d", name);

	return (*orig_glGetString) (name);
}

//Initializes and renders
void sys_wglSwapBuffers(HDC hDC)
{	
	//initializes
	if (g_init)
	{
		// Load our extensions
		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
		{
			////MessageBox(NULL, " Failed to get ogl_Load", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		//Create our buffers
		gl_d3d_buffers = new GLD3DBuffers;

		handle = GetActiveWindow();
		//HDC hDC = GetDC(handle);
		GLD3DBuffers_create(gl_d3d_buffers, handle, true);
		g_init = false;
		
		//start the rendering
		frameCall = 1;

		// Add our shader pair!!!!!
		// Color Correction
		addExceptShader(58, 384);
		addExceptShader(51, 387);
		addExceptShader(61, 810);
		addExceptShader(58, 830);
		addExceptShader(90, 897);
		addExceptShader(51, 895);
		addExceptShader(92, 896);
		addExceptShader(90, 895);
		addExceptShader(96, 896);
		//
		//Other menu stuff
		addExceptShader(51, 457); //953
		addExceptShader(41, 883); //954
		addExceptShader(61, 433); //964 ingame color
		addExceptShader(61, 882); //966 menu shaking

		//Graphical fixes
		addExceptShader(61, 814); //968
		addExceptShader(61, 891); //997
		addExceptShader(42, 0); //998
		addExceptShader(42, 806); //1000
		addExceptShader(285, 665); //1001
		addExceptShader(42, 805); //1002
		addExceptShader(42, 807); //1003
		addExceptShader(51, 457); //1004
		addExceptShader(61, 388); //1005
		addExceptShader(61, 891); //1006

		//Stealth Mode
		addExceptShader(61, 393);

		//SSAO
		addExceptShader(58, 788); //1008

		//Hit Screen
		addExceptShader(58, 386);

		//Death screen
		addExceptShader(58, 385); //1014

		//Decal Face
		addExceptShader(256, 363); //1014

		//FOg
		addExceptShader(89, 495); //1014

		//DOF
		addExceptShader(61, 812);
		addExceptShader(242, 520);
		addExceptShader(43, 520);
		// Good DOF
		addExceptShader(58, 879);

		// Test
		addExceptShader(58, 863);

		//Main Menu
		//addExceptShader(0, 61);
		//addExceptShader(188, 701);
		//addExceptShader(124, 458);
		//addExceptShader(51, 458);

		//inject our Rendering Protocols
		//Not actually required
		//synchronizeRendering();
	}
	
	else if (frameCall == 1)
	{
		//syncLeft();
		frameCall = 2;
	}
	else if (frameCall == 2)
	{
		//syncRight();
		frameCall = 1;

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
			GLD3DBuffers_create(gl_d3d_buffers, handle, true);
		}
	}
	// Used for fing the return address
	//void *exten = _ReturnAddress();
	//(*orig_wglSwapBuffers) (hDC);
}
/*
static void windowResize()
{
	RECT windowRect;
	unsigned int width;
	unsigned int height;
	
	while (true)
	{
		GetClientRect(handle, &windowRect);
		width = windowRect.right - windowRect.left;
		height = windowRect.bottom - windowRect.top;

		if ((handle != 0x00) && ((width != g_width) || (height != g_height)))
		{
			g_windowResizeCheck = true;
		}
		Sleep(150);
	}
}
*/
#pragma warning(disable:4100)
BOOL __stdcall DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hOriginalDll);
// 		CustomMessageBox(NULL, _T("  \t\t\t\t\t\t      \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"),
// 			_T("OpenGL-3D Vision Wrapper: The Chronicles of Riddick: Dark Athena"), MB_OK);
		Init();
		break;

	case DLL_PROCESS_DETACH:
		if (hOriginalDll != NULL)
		{
			FreeLibrary(hOriginalDll);
			hOriginalDll = NULL;
		}
		break;
	}
	return TRUE;
}
#pragma warning(default:4100)


void __cdecl add_log(const char * fmt, ...)
{
	va_list va_alist;
	char logbuf[256] = "";
	FILE * fp;

	va_start(va_alist, fmt);
	_vsnprintf(logbuf + strlen(logbuf), sizeof(logbuf)-strlen(logbuf), fmt, va_alist);
	va_end(va_alist);

	if ((fp = fopen("opengl32.log", "ab")) != NULL)
	{
		fprintf(fp, "%s\n", logbuf);
		fclose(fp);
	}
}