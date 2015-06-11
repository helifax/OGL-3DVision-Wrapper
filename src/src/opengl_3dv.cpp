#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>


extern "C" {
#include "opengl_3dv.h"
}

#include <thread>
#include <VersionHelpers.h>
#include "initguid.h" // this is necessary in order to use IID_IDirect3D9, see: http://support.microsoft.com/kb/130869
#include "d3d9.h"
#include "NVAPI_343\nvapi.h"
#include "wgl_custom.h"
#include "FPSInject/FPSInject.h"
#include "ConfigReader.h"
#include "wrapper_version.h"
#include "WindowManager.h"
#include "ShaderManager.h"

// NVAPI 3D VISION
static float g_separation; 
static float g_convergence;
static float m_currentConvergence;
static float g_eyeSeparation;

// Mutex 
static std::recursive_mutex m_nvapiMutex;

// Used in init and rendering
static bool m_reInit = false;
static bool m_NV3DVisionInit = true;
static unsigned int m_frameCall = 1;

NV_FRUSTUM_ADJUST_MODE frustum;
StereoHandle nvStereo;

NvAPI_Status status = NVAPI_OK;
BOOL g_NvSurround = FALSE;
BOOL g_NvSLI = FALSE;
static bool m_isThreadInitialised = false;
static bool m_isThreadRunning = true;

// OpenGL-DirectX interoop state
static BOOL g_interop = FALSE;

// NVAPI Polling
static void NVAPIquery(void);

// Config Reader
extern configReader *g_reader;

//Logging
extern void __cdecl add_log(const char * fmt, ...);

// Extern
extern unsigned int g_width;
extern unsigned int g_height;
extern bool g_windowResizeCheck;
extern BOOL g_fullscreen;

#ifdef DEBUG_WRAPPER
extern int debugVertexIndex;
extern int debugPixelIndex;
extern bool printCurrentShader;
extern bool isCurrentShaderVertex;
extern bool isCompileEnable;
static size_t indexVertex = 0;
static size_t indexPixel = 0;

// Kill Shader Info Window
static bool KillShaderMessageBox(void);

// Print Current 3D Vision Settings
static void Print3DVisionInfo(void);
#endif

// use for hotkeys
static bool m_key1 = false;
static bool m_LastKeyState = false;
static bool IsToggleKeyDown(int keyCode);
static bool IsKeyDown(int keyCode);

// create our FBO
GLuint m_auxFBO;
GLuint m_auxTexture;

typedef HRESULT(WINAPI *DIRECT3DCREATE9EXFUNCTION)(UINT SDKVersion, IDirect3D9Ex**);

/*
* NVIDIA stereo
*
* (From nvstereo.h in NVIDIA Direct3D SDK)
*/

typedef struct _Nv_Stereo_Image_Header
{
	unsigned int dwSignature;
	unsigned int dwWidth;
	unsigned int dwHeight;
	unsigned int dwBPP;
	unsigned int dwFlags;
} NVSTEREOIMAGEHEADER, *LPNVSTEREOIMAGEHEADER;

// dwSignature value
#define NVSTEREO_IMAGE_SIGNATURE 0x4433564e

// dwFlags values
#define SIH_SWAP_EYES 0x00000001
#define SIH_SCALE_TO_FIT 0x00000002
#define SIH_SCALE_TO_FIT2 0x00000004 // Stretch to fullscreen? (undocumented?!)

/*
* Constants
*/

#define DEPTH_STENCIL_FORMAT D3DFMT_D24S8

/*
* Complex stuff!
*
* We need to:
*
* 1. Get the window handle.
* 2. Link the WGL API via the window's device context.
* 3. Make sure we support the WGL NV_DX_interop extension.
* 4. Enable the Direct3D API.
* 5. Create a Direct3D device for our window.
* 5. Enable OpenGL interop for the device.
* 6. Create the Direct3D textures and buffers.
* 7. Create OpenGL textures.
* 8. Bind them to the Direct3D buffers.
* 9. Create a regular OpenGL frame buffer object using the textures.
*
*
* Other useful information:
*  http://developer.download.nvidia.com/opengl/specs/WGL_NV_DX_interop.txt
*  http://sourceforge.net/projects/mingw-w64/forums/forum/723797/topic/5293852
*  http://www.panda3d.org/forums/viewtopic.php?t=11583
*/
///--------------------------------------------------------------------------------------

BOOL NV3DVisionBuffers_create(GLD3DBuffers *gl_d3d_buffers, void *window_handle, bool stereo)
{
	if (m_reInit == false && m_NV3DVisionInit == true)
	{
		add_log("Creating OpenGL/Direct3D bridge...\n");
		ZeroMemory(gl_d3d_buffers, sizeof(GLD3DBuffers));
		gl_d3d_buffers->stereo = stereo;

		// WGL
		//
		// (Note: the window must have an OpenGL context for this to work;
		// SDL took care of that for us; also see wglGetCurrentDC)

		HWND hWnd = FindWindowFromProcessId();
		// Invalid handle
		if (!hWnd)
		{
			// Attempt to use the passed handle
			hWnd = (HWND)window_handle;
			if (!hWnd)
			{
				add_log("Cannot retrieve window information... Abort");
				return FALSE;
			}
		}

		WINDOWINFO windowInfo;
		HRESULT result;
		// Fullscreen?
		result = GetWindowInfo(hWnd, &windowInfo);
		if (FAILED(result))
		{
			add_log("Could not get window info");
		}
		
		g_fullscreen = (windowInfo.dwExStyle & WS_EX_TOPMOST) != 0;
		if (g_reader->IsFullScreenForced())
		{
			g_fullscreen = TRUE;
		}

		// If we force window mode support
		if (g_reader->IsWindowModeSupportEnabled() == true)
		{
			g_fullscreen = FALSE;
		}
		
		RECT windowRect;
		result = GetClientRect(hWnd, &windowRect);

		unsigned int width = windowRect.right - windowRect.left;
		unsigned int height = windowRect.bottom - windowRect.top;

		if ((width == 0) || (height == 0))
		{
			add_log("Invalid Window Information... Abort");
			return FALSE;
		}

		g_width = width;
		g_height = height;
		gl_d3d_buffers->width = width;
		gl_d3d_buffers->height = height;
		char str[255];
		sprintf_s(str, "Window size: %u, %u", width, height);
		add_log(str);
		
		HDC dc = GetDC(hWnd);
		if (wgl_LoadFunctions(dc) == wgl_LOAD_FAILED)
		{
			add_log("Failed to link to WGL API");
		}

		if (wgl_ext_NV_DX_interop == wgl_LOAD_FAILED)
		{
			add_log("WGL NV_DX_interop extension not supported on this platform");
		}
		add_log("WGL NV_DX_interop extension supported on this platform");

		// Are we a WDDM OS?
		// Higher or equal to vista ?
		BOOL wddmSupport = IsWindowsVistaOrGreater();
		if (wddmSupport)
		{
			add_log("This operating system uses WDDM (it's Windows Vista or later)");
		}
		else {
			add_log("This operating system does not use WDDM (it's prior to Windows Vista). Abort");
		}

		NvU8 stereoSupport = 0;
		// Check and See Stereo Support
		NvAPI_Stereo_IsEnabled(&stereoSupport);

		// Direct3D API
		//
		// (Note: Direct3D/OpenGL interop *requires* the use of 9Ex for WDDM-enabled operating systems)
		IDirect3D9 *d3d;
		// If we have wddmSupport and we have 3D Vision Enabled.
		if (wddmSupport && (stereoSupport == 1))
		{
			// Set Stereo Driver Mode
			NvAPI_Stereo_SetDriverMode(NVAPI_STEREO_DRIVER_MODE_AUTOMATIC);

			// We are dynamically linking to the Direct3DCreate9Ex function, because we
			// don't want to add a run-time dependency for it in our executable, which
			// would make it not run in Windows XP.
			//
			// See: http://msdn.microsoft.com/en-us/library/cc656710.aspx

			HMODULE d3dLibrary = LoadLibrary("d3d9.dll");
			if (!d3dLibrary)
			{
				add_log("Failed to link to d3d9.dll");
			}
			gl_d3d_buffers->d3dLibrary = d3dLibrary;

			DIRECT3DCREATE9EXFUNCTION pfnDirect3DCreate9Ex = (DIRECT3DCREATE9EXFUNCTION)GetProcAddress(d3dLibrary, "Direct3DCreate9Ex");
			if (!pfnDirect3DCreate9Ex)
			{
				add_log("Failed to link to Direct3D 9Ex (WDDM)");
			}

			IDirect3D9 *d3dEx;
			result = (*pfnDirect3DCreate9Ex)(D3D_SDK_VERSION, (IDirect3D9Ex**)&d3dEx);
			if (result != D3D_OK)
			{
				add_log("Failed to activate Direct3D 9Ex (WDDM)");
			}

			if (d3dEx->QueryInterface(IID_IDirect3D9, (LPVOID*)&d3d) != S_OK)
			{

				add_log("Failed to cast Direct3D 9Ex to Direct3D 9");
			}

			add_log("Activated Direct3D 9x");

			// Find display mode format
			//
			// (Our buffers will be the same to avoid conversion overhead)
			D3DDISPLAYMODE d3dDisplayMode;
			result = d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode);
			if (result != D3D_OK)
			{
				add_log("Failed to retrieve adapter display mode");
			}

			// Fixed Transparency issue
			// We need to support Fully ARGB32. Most of the times the adapter returns XRGB32
			// We need to to this only if from this format
			if (d3dDisplayMode.Format == D3DFMT_X8R8G8B8)
			{
				d3dDisplayMode.Format = D3DFMT_A8R8G8B8;
			}

			D3DFORMAT d3dBufferFormat = d3dDisplayMode.Format;

			sprintf_s(str, "Retrieved adapter display mode, format: %u", d3dBufferFormat);
			add_log(str);

			// Make sure devices can support the required formats
			result = d3d->CheckDeviceFormat(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
				D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DEPTH_STENCIL_FORMAT);
			if (result != D3D_OK)
			{
				add_log("Our required formats are not supported");
			}
			add_log("Our required formats are supported");

			// Get the device capabilities
			D3DCAPS9 d3dCaps;
			result = d3d->GetDeviceCaps(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);
			if (result != D3D_OK)
			{
				add_log("Failed to retrieve device capabilities");
			}

			add_log("Retrieved device capabilities");
			// Verify that we can do hardware vertex processing
			if (d3dCaps.VertexProcessingCaps == 0)
			{
				add_log("Hardware vertex processing is not supported");
			}
			add_log("Hardware vertex processing is supported");

			// Register stereo (must happen *before* device creation)
			if (gl_d3d_buffers->stereo)
			{
				if (NvAPI_Initialize() != NVAPI_OK)
				{
					add_log("Failed to initialize NVAPI");
				}
				else
				{
					add_log("NVAPI initialized");

					NvAPI_Status retVal;
					NvU32 displayID = 0;
					retVal = NvAPI_DISP_GetGDIPrimaryDisplayId(&displayID);

					if (retVal == NVAPI_OK)
					{
						NV_RECT viewports[NV_MOSAIC_MAX_DISPLAYS];
						NvU8 isBezelCorrected;

						retVal = NvAPI_Mosaic_GetDisplayViewportsByResolution(displayID, g_width, g_height, viewports, &isBezelCorrected);

						if (retVal == NVAPI_OK)
						{
							//NVIDIA Surround is enabled.
							add_log("NVIDIA Surround is enabled.");
							g_NvSurround = TRUE;
						}
						else if (retVal == NVAPI_MOSAIC_NOT_ACTIVE)
						{
							// No Surround
							g_NvSurround = FALSE;
							add_log("NVIDIA Single Screen is enabled.");
						}
					}


					// Check for SLI
					NvLogicalGpuHandle *nvGPUHandleLog = new NvLogicalGpuHandle;
					NvPhysicalGpuHandle *nvGPUHandlePhys = new NvPhysicalGpuHandle;

					NvU32 countLogical;
					NvU32 countPhysical;
					NvAPI_EnumLogicalGPUs(nvGPUHandleLog, &countLogical);
					NvAPI_EnumPhysicalGPUs(nvGPUHandlePhys, &countPhysical);

					// If we have more than 1 physical GPU and logical is 1 (SLI enabled)
					if ((countPhysical >= 2) && (countLogical < countPhysical))
					{
						// SLI enabled
						g_NvSLI = TRUE;
						// If we are in fullscreen
						if (g_fullscreen)
						{
							// And Surround is enabled
							if (g_NvSurround)
								add_log("Surround found and enabled.");
							else
								add_log("SLI enabled");
						}
					}
					else // we have a single GPU
					{
						g_NvSLI = FALSE;
						if (g_fullscreen)
						{
							// Even in fullscreen we don't set the fullscreen render flags!!!!
							g_fullscreen = FALSE;
						}
						add_log("NVIDIA SLI is Disabled.");
					}

					if (NvAPI_Unload() != NVAPI_OK)
						add_log("NVAPI Unloaded Successfully");
					else
						add_log("NVAPI Cannot be unloaded... Still in access ?!?!");
				}
			}
			else
			{
				add_log("No stereo Support.");
			}

			D3DPRESENT_PARAMETERS d3dPresent;
			ZeroMemory(&d3dPresent, sizeof(d3dPresent));
			d3dPresent.BackBufferCount = 1;
			d3dPresent.BackBufferFormat = d3dBufferFormat;
			d3dPresent.BackBufferWidth = width;
			d3dPresent.BackBufferHeight = height;
			d3dPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dPresent.MultiSampleQuality = 0;
			d3dPresent.MultiSampleType = D3DMULTISAMPLE_NONE;
			d3dPresent.EnableAutoDepthStencil = TRUE;
			d3dPresent.AutoDepthStencilFormat = DEPTH_STENCIL_FORMAT;
			d3dPresent.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

			if ((g_NvSLI == FALSE) && (g_NvSurround == FALSE))
			{
				//Single GPU D3DPRESENT_INTERVAL_IMMEDIATE + SWAP EYES
				d3dPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}
			else if ((g_NvSLI == TRUE) && (g_NvSurround == FALSE))
			{
				// SLI enabled but no surround
				d3dPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}
			else if ((g_NvSLI == TRUE) && (g_NvSurround == TRUE))
			{
				// Surround and SLI
				d3dPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}

			HWND localHwnd = NULL;

			if (g_fullscreen)
			{
				// This only applies in Surround
				d3dPresent.FullScreen_RefreshRateInHz = 120;
				d3dPresent.Windowed = FALSE;
				// Set the correct Handle based on mode !
				localHwnd = hWnd;
				d3dPresent.hDeviceWindow = hWnd; // can be NULL in windowed mode, in which case it will use the arg in CreateDevice

				add_log("Context is in Fullscreen Mode");
			}
			else
			{
				d3dPresent.Windowed = TRUE;
				//d3dPresent.FullScreen_RefreshRateInHz = 120;
				// Set the correct Handle based on mode !
				localHwnd = hWnd;
				d3dPresent.hDeviceWindow = hWnd; // can be NULL in windowed mode, in which case it will use the arg in CreateDevice
				add_log("Context is in Windowed Mode");
			}
			add_log("Windows information for D3D device set.");

			IDirect3DDevice9 *d3dDevice;
			result = d3d->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				localHwnd,
				//D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE | D3DCREATE_PUREDEVICE,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&d3dPresent,
				&d3dDevice);
			if (result != D3D_OK)
			{
				switch (result) {
				case D3DERR_DEVICELOST:
					add_log("Failed to create device: device lost");
					break;
				case D3DERR_INVALIDCALL:
					add_log("Failed to create device: invalid call");
					break;
				case D3DERR_NOTAVAILABLE:
					add_log("Failed to create device: not available");
					break;
				case D3DERR_OUTOFVIDEOMEMORY:
					add_log("Failed to create device: out of video memory");
					break;
				default:
					add_log("Failed to create device: unknown error %u", (int)result);
					break;
				}
			}
			gl_d3d_buffers->d3dDevice = d3dDevice;
			add_log("Created device\n");

			// Enable Direct3D/OpenGL interop on device
			HANDLE d3dDeviceInterop = wglDXOpenDeviceNV(d3dDevice);
			if (!d3dDeviceInterop)
			{
				DWORD e = GetLastError();
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				switch (e)
				{
				case ERROR_OPEN_FAILED:
					add_log("Failed to enable OpenGL interop on device");
				case ERROR_NOT_SUPPORTED:
					add_log("Failed to enable OpenGL interop on device: device not supported");
				default:
					add_log("Failed to enable OpenGL interop on device: unknown error %u", (int)e);
					break;
				}
			}
			gl_d3d_buffers->d3dDeviceInterop = d3dDeviceInterop;
			add_log("Enabled OpenGL interop on device");

			// Enable stereo on device (WDDM only)
			if (gl_d3d_buffers->stereo && wddmSupport)
			{
				if (NvAPI_Stereo_CreateHandleFromIUnknown((IUnknown *)d3dDevice, &nvStereo) != NVAPI_OK)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to create NV stereo handle");
				}
				gl_d3d_buffers->nvStereo = nvStereo;
				add_log("Created NV stereo handle on device:");

				if (NvAPI_Stereo_Activate(nvStereo) != NVAPI_OK)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to activate stereo");
				}
				add_log("Activated stereo");
				m_nvapiMutex.lock();
				NvAPI_Stereo_GetSeparation(nvStereo, &g_separation);
				NvAPI_Stereo_GetConvergence(nvStereo, &g_convergence);
				NvAPI_Stereo_GetFrustumAdjustMode(nvStereo, &frustum);
				NvAPI_Stereo_GetEyeSeparation(nvStereo, &g_eyeSeparation);
				m_nvapiMutex.unlock();

				// Save default convergence
				m_currentConvergence = g_convergence;
			}

			// Get device back buffer
			IDirect3DSurface9 *d3dBackBuffer;
			result = d3dDevice->GetBackBuffer(
				0, 0, D3DBACKBUFFER_TYPE_MONO, &d3dBackBuffer);
			if (result != D3D_OK)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				add_log("Failed to retrieve device back buffer");
			}
			gl_d3d_buffers->d3dBackBuffer = d3dBackBuffer;
			add_log("Retrieved device back buffer");

			// Direct3D textures
			//
			// (Note: we *must* use textures for stereo to show both eyes;
			// a CreateRenderTarget will only work for one eye)

			// Left texture
			IDirect3DTexture9 *d3dLeftColorTexture;
			result = d3dDevice->CreateTexture(
				width, height,
				0, // "levels" (mipmaps)
				0, // usage
				d3dBufferFormat,
				D3DPOOL_DEFAULT,
				&d3dLeftColorTexture,
				NULL);
			if (result != D3D_OK)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				add_log("Failed to create color texture (left)");
			}
			gl_d3d_buffers->d3dLeftColorTexture = d3dLeftColorTexture;
			add_log("Created color texture(left)");

			IDirect3DTexture9 *d3dRightColorTexture = 0;
			if (gl_d3d_buffers->stereo)
			{
				// Right texture
				result = d3dDevice->CreateTexture(
					width, height,
					0, // "levels" (mipmaps)
					0, // usage
					d3dBufferFormat,
					D3DPOOL_DEFAULT,
					&d3dRightColorTexture,
					NULL);
				if (result != D3D_OK)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to create color texture (right)");
				}
				gl_d3d_buffers->d3dRightColorTexture = d3dRightColorTexture;
				add_log("Created color texture (right)\n", "3D Vision Wrapper");
			}

			// Get Direct3D buffers from textures
			IDirect3DSurface9 *d3dLeftColorBuffer;
			result = d3dLeftColorTexture->GetSurfaceLevel(0, &d3dLeftColorBuffer);
			if (FAILED(result))
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				add_log("Failed to retrieve color buffer from color texture (left)");
			}
			gl_d3d_buffers->d3dLeftColorBuffer = d3dLeftColorBuffer;
			add_log("Retrieved color buffer from color texture (left)");

			if (gl_d3d_buffers->stereo)
			{
				IDirect3DSurface9 *d3dRightColorBuffer = 0;
				result = d3dRightColorTexture->GetSurfaceLevel(0, &d3dRightColorBuffer);
				if (FAILED(result))
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to retrieve color buffer from color texture (right)");
				}
				gl_d3d_buffers->d3dRightColorBuffer = d3dRightColorBuffer;
				add_log("Retrieved color buffer from color texture (right)");
			}

			if (gl_d3d_buffers->stereo)
			{
				// Stereo render target surface
				// (Note: must be at least 20 bytes wide to handle stereo header row!)
				IDirect3DSurface9 *d3dStereoColorBuffer;
				result = d3dDevice->CreateRenderTarget(
					width * 2, height + 1, // the extra row is for the stereo header
					d3dBufferFormat,
					D3DMULTISAMPLE_NONE, 0,
					TRUE, // must be lockable!
					&d3dStereoColorBuffer,
					NULL);

				if (result != D3D_OK)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to create render target surface (stereo)");
				}
				gl_d3d_buffers->d3dStereoColorBuffer = d3dStereoColorBuffer;
				add_log("Created render target surface (stereo)");

				D3DLOCKED_RECT d3dLockedRect;
				result = d3dStereoColorBuffer->LockRect(&d3dLockedRect, NULL, 0);
				if (result != D3D_OK)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to lock surface rect (stereo)");
				}

				// Insert stereo header into last row (the "+1") of stereo render target surface
				LPNVSTEREOIMAGEHEADER nvStereoHeader = (LPNVSTEREOIMAGEHEADER)(((unsigned char *)d3dLockedRect.pBits) + (d3dLockedRect.Pitch * gl_d3d_buffers->height));
				nvStereoHeader->dwSignature = NVSTEREO_IMAGE_SIGNATURE;

				// Check if Single GPU or SLI
				if (g_NvSLI == FALSE)
				{
					nvStereoHeader->dwFlags = SIH_SCALE_TO_FIT;
				}
				else if (g_NvSLI == TRUE)
				{
					// Normal
					nvStereoHeader->dwFlags = SIH_SCALE_TO_FIT2;
				}

				// Note: the following all seem to be ignored
				//nvStereoHeader->dwWidth = gl_d3d_buffers->width * 2;
				//nvStereoHeader->dwHeight = gl_d3d_buffers->height;
				//nvStereoHeader->dwBPP = 32; // hmm, get this from the format? my netbook declared D3DFMT_X8R8G8B8, which is indeed 32 bits

				result = d3dStereoColorBuffer->UnlockRect();
				if (result != D3D_OK)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to unlock surface rect (stereo)");
				}
				add_log("Inserted stereo header into render target surface (stereo)");
			}

			// Depth/stencil surface
			IDirect3DSurface9 *d3dDepthBuffer;
			result = d3dDevice->CreateDepthStencilSurface(
				width, height,
				DEPTH_STENCIL_FORMAT,
				D3DMULTISAMPLE_NONE, 0,
				FALSE, // Z-buffer discarding
				&d3dDepthBuffer,
				NULL);
			if (result != D3D_OK)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				add_log("Failed to create depth/stencil surface)");
			}
			gl_d3d_buffers->d3dDepthBuffer = d3dDepthBuffer;
			add_log("Created depth/stencil surface");

			// OpenGL textures
			GLenum error = glGetError();
			glGenTextures(1, &gl_d3d_buffers->texture_left);
			error = glGetError();
			if (error)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				add_log("Failed to create OpenGL color texture (left)");
			}
			add_log("Created OpenGL color texture (left)");

			if (gl_d3d_buffers->stereo)
			{
				glGenTextures(1, &gl_d3d_buffers->texture_right);
				GLenum error = glGetError();
				if (error)
				{
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					add_log("Failed to create OpenGL color texture (right)");
				}
				add_log("Created OpenGL color texture (right)");
			}

			// OpenGL render buffer (one should be enough for stereo?)
			glGenTextures(1, &gl_d3d_buffers->render_buffer);
			error = glGetError();
			if (error)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				add_log("Failed to create OpenGL depth/stencil texture");
			}
			add_log("Created OpenGL depth/stencil texture");

			HANDLE d3dLeftColorInterop = wglDXRegisterObjectNV(d3dDeviceInterop, d3dLeftColorTexture, gl_d3d_buffers->texture_left, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
			if (!d3dLeftColorInterop) {
				DWORD e = GetLastError();
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				switch (e)
				{
				case ERROR_OPEN_FAILED:
					add_log("Failed to bind render target surface to OpenGL texture (left)");
					break;
				case ERROR_INVALID_DATA:
					add_log("Failed to bind render target surface to OpenGL texture (left): invalid data");
					break;
				case ERROR_INVALID_HANDLE:
					add_log("Failed to bind render target surface to OpenGL texture (left): invalid handle");
					break;
				default:
					add_log("Failed to bind render target surface to OpenGL texture (left): unknown error %u", (int)e);
					break;
				}
			}
			gl_d3d_buffers->d3dLeftColorInterop = d3dLeftColorInterop;
			add_log("Bound render target surface to OpenGL texture (left)");

			HANDLE d3dRightColorInterop = wglDXRegisterObjectNV(d3dDeviceInterop, d3dRightColorTexture, gl_d3d_buffers->texture_right, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
			if (gl_d3d_buffers->stereo) {
				if (!d3dRightColorInterop) {
					DWORD e = GetLastError();
					NV3DVisionBuffers_destroy(gl_d3d_buffers);
					switch (e)
					{
					case ERROR_OPEN_FAILED:
						add_log("Failed to bind render target surface to OpenGL texture (right)");
						break;
					case ERROR_INVALID_DATA:
						add_log("Failed to bind render target surface to OpenGL texture (right): invalid data");
						break;
					case ERROR_INVALID_HANDLE:
						add_log("Failed to bind render target surface to OpenGL texture (right): invalid handle");
						break;
					default:
						add_log("Failed to bind render target surface to OpenGL texture (right): unknown error %u", (int)e);
						break;
					}
				}
				gl_d3d_buffers->d3dRightColorInterop = d3dRightColorInterop;
				add_log("Bound render target surface to OpenGL texture (right)");
			}

			// Bind OpenGL render buffer
			HANDLE d3dDepthInterop = wglDXRegisterObjectNV(d3dDeviceInterop, d3dDepthBuffer, gl_d3d_buffers->render_buffer, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
			if (!d3dDepthInterop) {
				DWORD e = GetLastError();
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				switch (e) {
				case ERROR_OPEN_FAILED:
					add_log("Failed to bind depth/stencil surface to OpenGL render buffer");
					break;
				case ERROR_INVALID_DATA:
					add_log("Failed to bind depth/stencil surface to OpenGL render buffer: invalid data");
					break;
				case ERROR_INVALID_HANDLE:
					add_log("Failed to bind depth/stencil surface to OpenGL render buffer: invalid handle");
					break;
				default:
					add_log("Failed to bind depth / stencil surface to OpenGL render buffer : unknown error %u", (int)e);
					break;
				}
			}
			gl_d3d_buffers->d3dDepthInterop = d3dDepthInterop;
			add_log("Bound depth/stencil surface to OpenGL render buffer");

			// Note: if we don't lock the interops first, glCheckFramebufferStatusEXT will fail with an unknown error

			// LOCK ALL 3 HANDLES AT ONCE TO AVOID CPU BOTTLENECK >:< as the GPU will take care of it. Net gain 10fps
			HANDLE gl_handles[3];
			gl_handles[0] = d3dLeftColorInterop;
			gl_handles[1] = d3dRightColorInterop;
			gl_handles[2] = d3dDepthInterop;
			// Do the resouce LOCK
			wglDXLockObjectsNV(d3dDeviceInterop, 3, gl_handles);

			// OpenGL frame buffer objects
			//
			// Note: why not glFramebufferRenderbufferEXT? Because it's apparently buggy in stereo :/
			// glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, gl_d3d_buffers->render_buffer);

			glGenFramebuffers(1, &gl_d3d_buffers->fbo_left);
			glBindFramebuffer(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_left);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_d3d_buffers->texture_left, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
			GLenum status_left = glCheckFramebufferStatus(GL_FRAMEBUFFER);


			GLenum status_right;
			if (gl_d3d_buffers->stereo)
			{
				glGenFramebuffers(1, &gl_d3d_buffers->fbo_right);
				glBindFramebuffer(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_right);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_d3d_buffers->texture_right, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
				status_right = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			}

			// Used in flipping
			glGenFramebuffers(1, &m_auxFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, m_auxFBO);
			glGenTextures(1, &m_auxTexture);
			glBindTexture(GL_TEXTURE_2D, m_auxTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_width, g_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_auxTexture, 0);
			status_right = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			// Used in flipping

			wglDXUnlockObjectsNV(d3dDeviceInterop, 1, &d3dLeftColorInterop);
			if (gl_d3d_buffers->stereo)
			{
				wglDXUnlockObjectsNV(d3dDeviceInterop, 1, &d3dRightColorInterop);
			}
			wglDXUnlockObjectsNV(d3dDeviceInterop, 1, &d3dDepthInterop);

			// Check for completeness

			if (status_left != GL_FRAMEBUFFER_COMPLETE_EXT)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				switch (status_left)
				{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					add_log("Failed to create OpenGL frame buffer object (left): attachment");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					add_log("Failed to create OpenGL frame buffer object (left): missing attachment");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
					add_log("Failed to create OpenGL frame buffer object (left): dimensions");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
					add_log("Failed to create OpenGL frame buffer object (left): formats");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
					add_log("Failed to create OpenGL frame buffer object (left): draw buffer");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
					add_log("Failed to create OpenGL frame buffer object (left): read buffer");
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
					add_log("Failed to create OpenGL frame buffer object (left): unsupported");
					break;
				default:
					add_log("Failed to create OpenGL frame buffer object (left): unknown error %u", (int)status_left);
					break;
				}
			}

			if (status_right != GL_FRAMEBUFFER_COMPLETE_EXT)
			{
				NV3DVisionBuffers_destroy(gl_d3d_buffers);
				switch (status_right)
				{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					add_log("Failed to create OpenGL frame buffer object (right): attachment");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					add_log("Failed to create OpenGL frame buffer object (right): missing attachment");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
					add_log("Failed to create OpenGL frame buffer object (right): dimensions");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
					add_log("Failed to create OpenGL frame buffer object (right): formats");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
					add_log("Failed to create OpenGL frame buffer object (right): draw buffer");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
					add_log("Failed to create OpenGL frame buffer object (right): read buffer");
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
					add_log("Failed to create OpenGL frame buffer object (right): unsupported");
					break;
				default:
					add_log("Failed to create OpenGL frame buffer object (right): unknown error %u", (int)status_left);
					break;
				}
			}

			glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
			add_log("Created OpenGL frame buffer objects");

			gl_d3d_buffers->initialized = true;
			m_NV3DVisionInit = false;
			add_log("---");
			add_log("3D Vision successfully initialized !");
			add_log("---");
			add_log("Starting Rendering:");
			add_log("---");
		}
		else
		{
			add_log("---");
			add_log("3D Vision is disabled in NVPanel. Please enable it.");
			add_log("---");
			g_reader->Set3DVisionEnabledStatus(false);
		}
		return TRUE;
	}
	return FALSE;
}
///--------------------------------------------------------------------------------------

// Activate the DEPTH Buffer
void NV3DVisionBuffers_activate_depth(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		wglDXLockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dDepthInterop);
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionBuffers_deactivate_depth(GLD3DBuffers *gl_d3d_buffers)
{
	if ((!m_NV3DVisionInit) || (m_reInit))
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		BOOL ok = wglDXUnlockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dDepthInterop);

		char res[255];
		strcpy_s(res, "Deactivate DEPTH Buffer Interop: ");
		if (ok == TRUE)
		{
			strcat_s(res, "TRUE");
		}
		else
		{
			strcat_s(res, "FALSE");
		}
		add_log(res);
	}
}

/*
* Before using the OpenGL frame buffer object, we need to make sure to lock
* the Direct3D buffers.
*/
void NV3DVisionBuffers_activate_left(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		BOOL result = FALSE;
		char res[255];

		// MANUAL LOCK ON EACH...For some reason locking both like above doesn't always work
		result = wglDXLockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dLeftColorInterop);

		strcpy_s(res, "LEFT EYE: Lock Color Interop: ");
		if (result == TRUE)
		{
			strcat_s(res, "TRUE");
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, gl_d3d_buffers->fbo_left);
		}
		else
		{
			strcat_s(res, "FALSE");
		}
		add_log(res);
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionBuffers_activate_right(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		BOOL result = FALSE;
		char res[255];

		result = wglDXLockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dRightColorInterop);
		strcpy_s(res, "RIGHT EYE: Lock Color Interop: ");
		if (result == TRUE)
		{
			strcat_s(res, "TRUE");
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, gl_d3d_buffers->fbo_right);
		}
		else
		{
			strcat_s(res, "FALSE");
		}
		add_log(res);
	}
}
///--------------------------------------------------------------------------------------

// Probably will not be used
void NV3DVisionBuffers_flip_left(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		// Read to our FBO
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, gl_d3d_buffers->fbo_left);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, m_auxFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		// This is the flipping
		glBlitFramebuffer(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Put the flipped image in the Back buffer
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, m_auxFBO);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, gl_d3d_buffers->fbo_left);

		glClear(GL_COLOR_BUFFER_BIT);
		glBlitFramebuffer(0, 0, g_width, g_height, 0, g_height, g_width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebufferEXT(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_left);

		add_log("Left Eye: Flip Successfully");
	}
}
///--------------------------------------------------------------------------------------

// Probably will not be used
void NV3DVisionBuffers_flip_right(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		// Read to our FBO
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, gl_d3d_buffers->fbo_right);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, m_auxFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		// This is the flipping
		glBlitFramebuffer(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Put the flipped image in the Back buffer
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, m_auxFBO);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, gl_d3d_buffers->fbo_right);

		glClear(GL_COLOR_BUFFER_BIT);
		glBlitFramebuffer(0, 0, g_width, g_height, 0, g_height, g_width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebufferEXT(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_right);

		add_log("Right Eye: Flip Successfully");
	}
}
///--------------------------------------------------------------------------------------

// Copy left image from OGL screen buffer to DX Screen Buffer.
// Perform the flip in one GO
void NV3DVisionBuffers_copy_left(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		// Read to our FBO
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, 0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, gl_d3d_buffers->fbo_left);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlitFramebuffer(0, 0, g_width, g_height, 0, g_height, g_width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		add_log("Left Eye: Render Successfully");
	}
}
///--------------------------------------------------------------------------------------

// Copy right image from OGL screen buffer to DX Screen Buffer.
// Perform the flip in one GO
void NV3DVisionBuffers_copy_right(GLD3DBuffers *gl_d3d_buffers)
{
	if (!m_NV3DVisionInit)
	{
		// Read to our FBO
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, 0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, gl_d3d_buffers->fbo_right);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlitFramebuffer(0, 0, g_width, g_height, 0, g_height, g_width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		add_log("Right Eye: Render Successfully");
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionBuffers_deactivate_left(GLD3DBuffers *gl_d3d_buffers)
{
	if ((!m_NV3DVisionInit) || (m_reInit))
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		BOOL ok = wglDXUnlockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dLeftColorInterop);

		char res[255];
		strcpy_s(res, "Deactivate LEFT Buffer Interop: ");
		if (ok == TRUE)
		{
			strcat_s(res, "TRUE");
		}
		else
		{
			strcat_s(res, "FALSE");
		}
		add_log(res);
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionBuffers_deactivate_right(GLD3DBuffers *gl_d3d_buffers)
{
	if ((!m_NV3DVisionInit) || (m_reInit))
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		BOOL ok = wglDXUnlockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dRightColorInterop);

		char res[255];
		strcpy_s(res, "Deactivate RIGHT Buffer Interop: ");
		if (ok == TRUE)
		{
			strcat_s(res, "TRUE");
		}
		else
		{
			strcat_s(res, "FALSE");
		}
		add_log(res);
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionBuffers_deactivate(GLD3DBuffers *gl_d3d_buffers)
{
	if ((!m_NV3DVisionInit) || (m_reInit))
	{
		SetInterop(true);
		HANDLE gl_handles[3];
		gl_handles[0] = gl_d3d_buffers->d3dLeftColorInterop;
		gl_handles[1] = gl_d3d_buffers->d3dRightColorInterop;
		gl_handles[2] = gl_d3d_buffers->d3dDepthInterop;

		// UNLOCK just the LEFT ONE to gain maximum FPS
		BOOL result = FALSE;
		char res[255];
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		result = wglDXUnlockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 3, gl_handles);
		strcpy_s(res, "FLUSH: Unlock Interop Buffers: ");
		if (result == TRUE)
		{
			strcat_s(res, "TRUE");
		}
		else
		{
			strcat_s(res, "FALSE");
		}
		add_log(res);
		SetInterop(false);
	}
}
///--------------------------------------------------------------------------------------

/*
* Flushing:
*
* 1. Copy our color buffer to the device's back buffer.
* 2. Present the device back buffer.
*/
void NV3DVisionBuffers_flush(GLD3DBuffers *gl_d3d_buffers)
{
	// Only if we are initialized
	if ((!m_NV3DVisionInit) || (m_reInit))
	{
	IDirect3DDevice9 *d3dDevice = (IDirect3DDevice9 *)gl_d3d_buffers->d3dDevice;
	IDirect3DSurface9 *d3dBackBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dBackBuffer;
	IDirect3DSurface9 *d3dLeftColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dLeftColorBuffer;
	IDirect3DSurface9 *d3dRightColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dRightColorBuffer;
	IDirect3DSurface9 *d3dStereoColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dStereoColorBuffer;

	HRESULT result;

	if (gl_d3d_buffers->stereo /*&& d3dRightColorBuffer*/ && d3dStereoColorBuffer)
	{
		// Stretch left color buffer to left side
		RECT dest;
		dest.top = 0;
		dest.bottom = gl_d3d_buffers->height;
		dest.left = 0;
		dest.right = gl_d3d_buffers->width;
		result = d3dDevice->StretchRect(d3dLeftColorBuffer, NULL, d3dStereoColorBuffer, &dest, D3DTEXF_NONE);
		//if (result != D3D_OK)
		{
			//MessageBox(NULL, "Failed to stretch left color buffer to stereo color buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}

		// Stretch right color buffer to right side
		dest.left = gl_d3d_buffers->width;
		dest.right = gl_d3d_buffers->width * 2;
		result = d3dDevice->StretchRect(d3dRightColorBuffer, NULL, d3dStereoColorBuffer, &dest, D3DTEXF_NONE);
		//if (result != D3D_OK)
		{
			//MessageBox(NULL, "Failed to stretch right color buffer to stereo color buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}

		// Stretch stereo color buffer to back buffer
		//
		// This is where the stereo magic happens! The driver discovers the header and
		// does the right thing with our left/right images. In fact, it ignores the
		// source and dest rects in this case. The StretchRect call seems to trigger
		// an entirely different function in the driver.
		//
		// Note: we are currently getting an "Out of Memory" error for this in fullscreen
		// mode.

		result = d3dDevice->StretchRect(d3dStereoColorBuffer, NULL, d3dBackBuffer, NULL, D3DTEXF_NONE);
		//if (result != D3D_OK)
		{
			//MessageBox(NULL, "Failed to stretch stereo color buffer to back buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		// Stretch left color buffer to back buffer
		result = d3dDevice->StretchRect(d3dLeftColorBuffer, NULL, d3dBackBuffer, NULL, D3DTEXF_NONE);
	}

	// Present back buffer
	result = d3dDevice->Present(NULL, NULL, NULL, NULL);

	// This can be used for debugging purposes.
#if 0
	if (result == D3DOK_NOAUTOGEN)
	{
		MessageBox(NULL, "D3DOK_NOAUTOGEN", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_CONFLICTINGRENDERSTATE)
	{

		MessageBox(NULL, "D3DERR_CONFLICTINGRENDERSTATE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_CONFLICTINGTEXTUREFILTER)
	{

		MessageBox(NULL, "D3DERR_CONFLICTINGTEXTUREFILTER", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_CONFLICTINGTEXTUREPALETTE)
	{

		MessageBox(NULL, "D3DERR_CONFLICTINGTEXTUREPALETTE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_DEVICEHUNG)
	{

		MessageBox(NULL, "D3DERR_DEVICEHUNG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_DEVICELOST)
	{

		MessageBox(NULL, "D3DERR_DEVICELOST", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_DEVICENOTRESET)
	{

		MessageBox(NULL, "D3DERR_DEVICENOTRESET", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_DEVICEREMOVED)
	{

		MessageBox(NULL, "D3DERR_DEVICEREMOVED", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_DRIVERINTERNALERROR)
	{

		MessageBox(NULL, "D3DERR_DRIVERINTERNALERROR", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_INVALIDCALL)
	{
		MessageBox(NULL, "D3DERR_INVALIDCALL", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_INVALIDDEVICE)
	{

		MessageBox(NULL, "D3DERR_INVALIDDEVICE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_MOREDATA)
	{

		MessageBox(NULL, "D3DERR_MOREDATA", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_NOTAVAILABLE)
	{

		MessageBox(NULL, "D3DERR_NOTAVAILABLE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_NOTFOUND)
	{

		MessageBox(NULL, "D3DERR_NOTFOUND", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_OUTOFVIDEOMEMORY)
	{

		MessageBox(NULL, "D3DERR_OUTOFVIDEOMEMORY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_TOOMANYOPERATIONS)
	{

		MessageBox(NULL, "D3DERR_TOOMANYOPERATIONS", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDALPHAARG)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDALPHAARG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDALPHAOPERATION)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDALPHAOPERATION", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDCOLORARG)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDCOLORARG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	else if (result == D3DERR_UNSUPPORTEDCOLOROPERATION)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDCOLOROPERATION", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDFACTORVALUE)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDFACTORVALUE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDTEXTUREFILTER)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDTEXTUREFILTER", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_WASSTILLDRAWING)
	{

		MessageBox(NULL, "D3DERR_WASSTILLDRAWING", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_WRONGTEXTUREFORMAT)
	{

		MessageBox(NULL, "D3DERR_WRONGTEXTUREFORMAT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == E_FAIL)
	{

		MessageBox(NULL, "E_FAIL", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == E_INVALIDARG)
	{

		MessageBox(NULL, "E_INVALIDARG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == E_NOINTERFACE)
	{

		MessageBox(NULL, "E_NOINTERFACE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == E_OUTOFMEMORY)
	{

		MessageBox(NULL, "E_OUTOFMEMORY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == S_NOT_RESIDENT)
	{

		MessageBox(NULL, "S_NOT_RESIDENT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == S_RESIDENT_IN_SHARED_MEMORY)
	{

		MessageBox(NULL, "S_RESIDENT_IN_SHARED_MEMORY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == S_PRESENT_MODE_CHANGED)
	{

		MessageBox(NULL, "S_PRESENT_MODE_CHANGED", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == S_PRESENT_OCCLUDED)
	{

		MessageBox(NULL, "S_PRESENT_OCCLUDED", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDOVERLAY)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDOVERLAY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDOVERLAYFORMAT)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDOVERLAYFORMAT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_CANNOTPROTECTCONTENT)
	{

		MessageBox(NULL, "D3DERR_CANNOTPROTECTCONTENT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_UNSUPPORTEDCRYPTO)
	{

		MessageBox(NULL, "D3DERR_UNSUPPORTEDCRYPTO", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else if (result == D3DERR_PRESENT_STATISTICS_DISJOINT)
	{

		MessageBox(NULL, "D3DERR_PRESENT_STATISTICS_DISJOINT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
#endif
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionBuffers_destroy(GLD3DBuffers *gl_d3d_buffers) 
{
	// Only if we are allowed to destroy
	if (m_reInit == true && m_NV3DVisionInit == false)
	{
		add_log("Disabling 3D Vision and cleaning up !");

		// First let's deactivate all the buffers
		NV3DVisionBuffers_deactivate_left(gl_d3d_buffers);
		NV3DVisionBuffers_deactivate_right(gl_d3d_buffers);
		NV3DVisionBuffers_deactivate_depth(gl_d3d_buffers);
		// Be sure that everything is deactivated
		NV3DVisionBuffers_deactivate(gl_d3d_buffers);
		// We must flush the current content
		//GLD3DBuffers_flush(gl_d3d_buffers);

		// Reset the frames flags
		m_frameCall = 0;
		//////////////////////////////////////////////////////////////////////////
		/// Destroy the objects
		//////////////////////////////////////////////////////////////////////////

		// Destroy NV stereo handle
		if (gl_d3d_buffers->nvStereo)
		{
			NvAPI_Stereo_DestroyHandle(gl_d3d_buffers->nvStereo);
			gl_d3d_buffers->nvStereo = NULL;
			add_log("Deleting the Stereo Handle");
		}

		// Unbind OpenGL textures from Direct3D buffers
		if (gl_d3d_buffers->d3dDeviceInterop && gl_d3d_buffers->d3dLeftColorInterop)
		{
			if (wglDXUnregisterObjectNV(gl_d3d_buffers->d3dDeviceInterop, gl_d3d_buffers->d3dLeftColorInterop))
			{
				gl_d3d_buffers->d3dLeftColorInterop = NULL;
				add_log("De-registered Left Color buffer");
			}
		}

		if (gl_d3d_buffers->d3dDeviceInterop && gl_d3d_buffers->d3dRightColorInterop)
		{
			wglDXUnregisterObjectNV(gl_d3d_buffers->d3dDeviceInterop, gl_d3d_buffers->d3dRightColorInterop);
			gl_d3d_buffers->d3dRightColorInterop = NULL;
			add_log("De-registered Right Color buffer");
		}

		if (gl_d3d_buffers->d3dDeviceInterop && gl_d3d_buffers->d3dDepthInterop)
		{
			wglDXUnregisterObjectNV(gl_d3d_buffers->d3dDeviceInterop, gl_d3d_buffers->d3dDepthInterop);
			gl_d3d_buffers->d3dDepthInterop = NULL;
			add_log("De-registered Depth buffer");
		}

		// Disable Direct3D/OpenGL interop
		if (gl_d3d_buffers->d3dDeviceInterop)
		{
			wglDXCloseDeviceNV(gl_d3d_buffers->d3dDeviceInterop);
			gl_d3d_buffers->d3dDeviceInterop = NULL;
			add_log("Disabled DX-OGL Interop layer");
		}

		// Release Direct3D objects
		if (gl_d3d_buffers->d3dLeftColorTexture)
		{
			IDirect3DSurface9 *d3dLeftColorTexture = (IDirect3DSurface9 *)gl_d3d_buffers->d3dLeftColorTexture;
			d3dLeftColorTexture->Release();
			gl_d3d_buffers->d3dLeftColorTexture = NULL;
			add_log("Releasing Direct3D Left Color Texture");
		}

		if (gl_d3d_buffers->d3dRightColorTexture)
		{
			IDirect3DSurface9 *d3dRightColorTexture = (IDirect3DSurface9 *)gl_d3d_buffers->d3dRightColorTexture;
			d3dRightColorTexture->Release();
			gl_d3d_buffers->d3dRightColorTexture = NULL;
			add_log("Releasing Direct3D Right Color Texture");
		}

		if (gl_d3d_buffers->d3dLeftColorBuffer)
		{
			IDirect3DSurface9 *d3dLeftColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dLeftColorBuffer;
			d3dLeftColorBuffer->Release();
			gl_d3d_buffers->d3dLeftColorBuffer = NULL;
			add_log("Releasing Direct3D Left Color Buffer");
		}

		if (gl_d3d_buffers->d3dRightColorBuffer)
		{
			IDirect3DSurface9 *d3dRightColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dRightColorBuffer;
			d3dRightColorBuffer->Release();
			gl_d3d_buffers->d3dRightColorBuffer = NULL;
			add_log("Releasing Direct3D Right Color Buffer");
		}

		if (gl_d3d_buffers->d3dStereoColorBuffer)
		{
			IDirect3DSurface9 *d3dStereoColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dStereoColorBuffer;
			d3dStereoColorBuffer->Release();
			gl_d3d_buffers->d3dStereoColorBuffer = NULL;
			add_log("Releasing Direct3D Stereo Color Buffer");
		}

		if (gl_d3d_buffers->d3dDepthBuffer)
		{
			IDirect3DSurface9 *d3dDepthBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dDepthBuffer;
			d3dDepthBuffer->Release();
			gl_d3d_buffers->d3dDepthBuffer = NULL;
			add_log("Releasing Direct3D Depth Buffer");
		}

		if (gl_d3d_buffers->d3dBackBuffer)
		{
			IDirect3DSurface9 *d3dBackBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dBackBuffer;
			d3dBackBuffer->Release();
			gl_d3d_buffers->d3dBackBuffer = NULL;
			add_log("Releasing Direct3D Back Buffer");
		}

		if (gl_d3d_buffers->d3dDevice)
		{
			IDirect3DDevice9 *d3dDevice = (IDirect3DDevice9 *)gl_d3d_buffers->d3dDevice;
			d3dDevice->Release();
			gl_d3d_buffers->d3dDevice = NULL;
			add_log("Releasing Direct3D Device");
		}

		if (gl_d3d_buffers->d3dLibrary)
		{
			FreeLibrary((HINSTANCE)gl_d3d_buffers->d3dLibrary);
			gl_d3d_buffers->d3dLibrary = NULL;
			add_log("DirectX Library unloaded");
		}

		// Delete OpenGL frame buffer object
		if (gl_d3d_buffers->fbo_left)
		{
			glDeleteFramebuffers(1, &gl_d3d_buffers->fbo_left);
			gl_d3d_buffers->fbo_left = 0;
			add_log("Deleting OpenGL Left FBO");
		}

		// Delete OpenGL render buffer
		if (gl_d3d_buffers->render_buffer)
		{
			glDeleteRenderbuffersEXT(1, &gl_d3d_buffers->render_buffer);
			gl_d3d_buffers->render_buffer = 0;
			add_log("Deleting OpenGL Render Buffer");
		}

		// Delete OpenGL textures
		if (gl_d3d_buffers->texture_left)
		{
			glDeleteTextures(1, &gl_d3d_buffers->texture_left);
			gl_d3d_buffers->texture_left = 0;
			add_log("Deleting OpenGL Left Texture");
		}

		if (gl_d3d_buffers->texture_right)
		{
			glDeleteTextures(1, &gl_d3d_buffers->texture_right);
			gl_d3d_buffers->texture_right = 0;
			add_log("Deleting OpenGL Right Texture");
		}
		// Unload NvAPI
		NvAPI_Unload();
		add_log("Cleanup complete!");
		m_reInit = false;
		m_NV3DVisionInit = true;
	}
}
///--------------------------------------------------------------------------------------

// our NVAPI poll
static void NVAPIquery(void)
{
	// Run as long as is alive
	while (m_isThreadRunning)
	{
		// Accept keys only if we are initialized
		if (NV3DVisionIsNotInit() == false)
		{
			m_nvapiMutex.lock();
			NvAPI_Stereo_GetSeparation(nvStereo, &g_separation);
			NvAPI_Stereo_GetConvergence(nvStereo, &g_convergence);
			NvAPI_Stereo_GetEyeSeparation(nvStereo, &g_eyeSeparation);

			// get the correct separation
			g_eyeSeparation = g_separation * g_eyeSeparation / 100;

			float factor = g_reader->GetDepthFactor();
			if (factor > 0.0)
			{
				g_eyeSeparation = g_eyeSeparation * factor;
			}

			// Update the current convergence only if we are in normal mode
			if (!m_key1)
			{
				m_currentConvergence = g_convergence;
			}

			// Key Handler
			/////////////////////////////////////

			if (g_reader->GetKeyToggleMode())
			{
				// Toggled Enabled
				if (IsToggleKeyDown(g_reader->GetKeyNumber()))
				{
					m_key1 = !m_key1;
				}
			}
			else
			{
				// No toggle mode
				if (IsKeyDown(g_reader->GetKeyNumber()))
				{
					m_key1 = TRUE;
				}
				else
				{
					m_key1 = FALSE;
				}
			}

			// Toggle settings
			if (m_key1)
			{
				// Aim convergence
				g_convergence = g_reader->GetAltConvergence();

				// Apply the new convergence
				NvAPI_Stereo_SetConvergence(nvStereo, g_convergence);
			}
			else
			{
				// Apply the normal convergence
				NvAPI_Stereo_SetConvergence(nvStereo, m_currentConvergence);
			}
			/////////////////////////////////////
#ifdef DEBUG_WRAPPER
			if ((IsKeyDown(0x11) != 0) && (IsKeyDown(0xBB) != 0))  // [CTRL +] key next Vertex shader
			{
				if (!KillShaderMessageBox())
				{
					// find first program index in shader list.
					ShaderManager *shaderManager = ShaderManager::getInstance();
					std::vector<EXISTING_SHADER_T> existingShaders = shaderManager->GetExistingShaders();
					size_t index = indexVertex;

					for (; index < (existingShaders.size() - 1); index++)
					{
						if ((debugVertexIndex == (int)existingShaders[index].m_programId))
						{
							// We found the current one
							break;
						}
					}

					// Now we need to find the next valid one
					while ((index++) < (existingShaders.size() - 1))
					{
						int newIndex = existingShaders[index].m_programId;
						if (debugVertexIndex != newIndex)
						{
							debugVertexIndex = (int)existingShaders[index].m_programId;
							indexVertex = index;
							break;
						}
					}

					isCurrentShaderVertex = true;
					printCurrentShader = true;
				}
			}
			else if ((IsKeyDown(0x11) != 0) && IsKeyDown(0xBD) != 0) //[CTRL -] key previous Vertex shader
			{
				if (!KillShaderMessageBox())
				{
					// Now we need to find the next valid one
					ShaderManager *shaderManager = ShaderManager::getInstance();
					std::vector<EXISTING_SHADER_T> existingShaders = shaderManager->GetExistingShaders();
					size_t index = indexVertex;

					for (; index > 1; index--)
					{
						if ((debugVertexIndex == (int)existingShaders[index].m_programId))
						{
							// We found the current one
							break;
						}
					}

					// Now we need to find the next valid one
					while ((index--) > 1)
					{
						int newIndex = existingShaders[index].m_programId;
						if (debugVertexIndex != newIndex)
						{
							debugVertexIndex = (int)existingShaders[index].m_programId;
							indexVertex = index;
							break;
						}
					}
					isCurrentShaderVertex = true;
					printCurrentShader = true;
				}
			}
			else if ((IsKeyDown(0x11) != 0) && (IsKeyDown(0xDD) != 0))  // [CTRL ]] key next Pixel shader
			{
				if (!KillShaderMessageBox())
				{
					// find first program index in shader list.
					ShaderManager *shaderManager = ShaderManager::getInstance();
					std::vector<EXISTING_SHADER_T> existingShaders = shaderManager->GetExistingShaders();
					size_t index = indexPixel;

					for (; index < (existingShaders.size() - 1); index++)
					{
						if ((debugPixelIndex == (int)existingShaders[index].m_programId))
						{
							// We found the current one
							break;
						}
					}

					// Now we need to find the next valid one
					while ((index++) < (existingShaders.size() - 1))
					{
						int newIndex = existingShaders[index].m_programId;
						if (debugPixelIndex != newIndex)
						{
							debugPixelIndex = (int)existingShaders[index].m_programId;
							indexPixel = index;
							break;
						}
					}

					isCurrentShaderVertex = false;
					printCurrentShader = true;
				}
			}
			else if ((IsKeyDown(0x11) != 0) && IsKeyDown(0xDB) != 0) //[CTRL [] key previous Pixel shader
			{
				if (!KillShaderMessageBox())
				{
					// find first program index in shader list.
					ShaderManager *shaderManager = ShaderManager::getInstance();
					std::vector<EXISTING_SHADER_T> existingShaders = shaderManager->GetExistingShaders();
					size_t index = indexPixel;

					for (; index > 1; index--)
					{
						if ((debugPixelIndex == (int)existingShaders[index].m_programId))
						{
							// We found the current one
							break;
						}
					}

					// Now we need to find the next one
					while ((index--) > 1)
					{
						int newIndex = existingShaders[index].m_programId;
						if (debugPixelIndex != newIndex)
						{
							debugPixelIndex = (int)existingShaders[index].m_programId;
							indexPixel = index;
							break;
						}
					}

					isCurrentShaderVertex = false;
					printCurrentShader = true;
				}
			}
			
			else if (IsToggleKeyDown(0x78) != 0) // [F9]key compile current selected shader
			{
				isCompileEnable = true;
			}

			else if (IsToggleKeyDown(0x79) != 0) // [F10]key Show the current Convergence + Separation Values
			{
				std::thread Print3DVisionInfo(Print3DVisionInfo);
				Print3DVisionInfo.detach();
			}
			else if (IsToggleKeyDown(0x7A) != 0) // [F11]key Reload Config file (Shader Wise)
			{
				ShaderManager * g_shaderManager = ShaderManager::getInstance();
				g_shaderManager->ReadConfigFile();
				g_reader->ReadConfigFile();
				MessageBox(NULL, "Config File Read !", "Info", MB_OK);
			}
#endif
			m_nvapiMutex.unlock();
		}
#ifdef DEBUG_WRAPPER
		Sleep(300);
#else
		Sleep(100);
#endif
	}
	add_log("NVAPI Thread Stopped");
}
///--------------------------------------------------------------------------------------

#ifdef DEBUG_WRAPPER
static bool KillShaderMessageBox(void)
{
	// Get the info box and kill it
	HWND hwnd = FindWindow(NULL, "Shader Info :");
	if (hwnd)
	{
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		return true;
	}
	return false;
}
///--------------------------------------------------------------------------------------

static void Print3DVisionInfo(void)
{
	char str[500];
	sprintf_s(str, "Current 3D Vision Settings: \n\nSEPARATION (Depth): %f.\n\nCONVERGENCE: %f. \n", Get3DEyeSeparation(), Get3DConvergence());
	
	// Print info
	MessageBox(NULL, str, "3D Vision Current Settings :", MB_OK);
}
///--------------------------------------------------------------------------------------
#endif

static bool IsToggleKeyDown(int keyCode)
{
	//Return if the high byte is true (ie key is down)
	return (GetAsyncKeyState(keyCode)& 1) != 0;
}
///--------------------------------------------------------------------------------------

static bool IsKeyDown(int keyCode)
{
	//Return if the high byte is true (ie key is down)
	return (GetKeyState(keyCode) & 0x80) != 0;
}
///--------------------------------------------------------------------------------------

void SetInterop(bool state)
{
	m_nvapiMutex.lock();
	g_interop = state;
	m_nvapiMutex.unlock();
}
///--------------------------------------------------------------------------------------

BOOL GetInterop(void)
{
	BOOL state = FALSE;
	
	m_nvapiMutex.lock();
	state = g_interop;
	m_nvapiMutex.unlock();

	return state;
}
///--------------------------------------------------------------------------------------

void Set3DSeparation(float value)
{
	m_nvapiMutex.lock();
	g_separation = value;
	m_nvapiMutex.unlock();
}
///--------------------------------------------------------------------------------------

void Set3DConvergence(float value)
{
	m_nvapiMutex.lock();
	g_convergence = value;
	m_nvapiMutex.unlock();
}
///--------------------------------------------------------------------------------------

void Set3DEyeSeparation(float value)
{
	m_nvapiMutex.lock();
	g_eyeSeparation = value;
	m_nvapiMutex.unlock();
}
///--------------------------------------------------------------------------------------

float Get3DSeparation()
{
	float ret = 0;

	m_nvapiMutex.lock();
	ret = g_separation;
	m_nvapiMutex.unlock();

	return ret;
}
///--------------------------------------------------------------------------------------

float Get3DConvergence()
{
	float ret = 0;

	m_nvapiMutex.lock();
	ret = g_convergence;
	m_nvapiMutex.unlock();

	return ret;
}
///--------------------------------------------------------------------------------------

float Get3DEyeSeparation()
{
	float ret = 0;

	m_nvapiMutex.lock();
	ret = g_eyeSeparation;
	m_nvapiMutex.unlock();

	return ret;
}
///--------------------------------------------------------------------------------------

bool NV3DVisionIsNotInit()
{
	return m_NV3DVisionInit;
}
///--------------------------------------------------------------------------------------

void NV3DVisionSetCurrentFrame(unsigned int frameNumber)
{
	m_frameCall = frameNumber;
}
///--------------------------------------------------------------------------------------

unsigned int NV3DVisionGetCurrentFrame(void)
{
	return m_frameCall;
}
///--------------------------------------------------------------------------------------

void OGL3DVisionWrapperGetVersion(char *pVersionStr)
{
	if (pVersionStr)
	{
		strcpy_s(pVersionStr, 255, OGL3DVISION_VERSION_STR);
	}
}
///--------------------------------------------------------------------------------------

void StartNVAPIThread(void)
{
	if (!m_isThreadInitialised)
	{
		add_log("Starting NVAPI Thread");
		// Start our nvapi thread poll
		std::thread NVAPIPoll(NVAPIquery);
		NVAPIPoll.detach();
		m_isThreadInitialised = true;
	}
}
///--------------------------------------------------------------------------------------

void StoptNVAPIThread(void)
{
	if (m_isThreadInitialised)
	{
		m_isThreadRunning = false;
	}
}
///--------------------------------------------------------------------------------------

void NV3DVisionReInitBuffers(void)
{
	add_log("Re-initialising 3D Vision Buffers");
	m_reInit = true;
}

///-------------------------------------------------------------------------------------
