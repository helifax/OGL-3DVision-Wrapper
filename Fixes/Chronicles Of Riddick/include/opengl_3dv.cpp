#include <stdio.h>
#include <thread>

#if defined(_WIN32)

extern "C" {
#include "opengl_3dv.h"
}

#include <thread>
#include "initguid.h" // this is necessary in order to use IID_IDirect3D9, see: http://support.microsoft.com/kb/130869
#include "d3d9.h"
#include "NVAPI_331\nvapi.h"
#include "wgl_custom.h"
#include "FPSInject.h"

float g_separation, g_convergence, orig_g_convergence, g_eyeSeparation;
NV_FRUSTUM_ADJUST_MODE frustum;
StereoHandle nvStereo;

NvAPI_Status status = NVAPI_OK;
BOOL g_NvSurround = FALSE;
BOOL g_NvSLI = FALSE;
BOOL g_fullscreen;
static void NVAPIquery(void);


extern unsigned int g_width;
extern unsigned int g_height;
extern bool g_windowResizeCheck;
extern int debugShaderIndex;
#ifdef NDEBUG
extern bool injected;
extern bool restoreDefault;
#endif

// use for hotkeys
bool m_swapConvergence = false;
bool m_preset2 = false;
bool m_preset3 = false;
static bool IsKeyDown(int keyCode);

// create our FBO
GLuint m_auxFBO;
GLuint m_auxTexture, m_auxDepth;


typedef HRESULT(WINAPI *DIRECT3DCREATE9EXFUNCTION)(UINT SDKVersion, IDirect3D9Ex**);
static char str[255];

#define D3D_EXCEPTION(M) printf("Error: %s\n", M); fflush(stdout); return;
//#define D3D_EXCEPTIONF(F, ARGS...) printf("Error: " F " %s\n", ARGS); fflush(stdout); return;

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
* 1. Get the window handle from SDL.
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
* See my own post here: http://www.mtbs3d.com/phpbb/viewtopic.php?f=105&t=16310&p=97553
*
* Other useful information:
*  http://developer.download.nvidia.com/opengl/specs/WGL_NV_DX_interop.txt
*  http://sourceforge.net/projects/mingw-w64/forums/forum/723797/topic/5293852
*  http://www.panda3d.org/forums/viewtopic.php?t=11583
*/
void GLD3DBuffers_create(GLD3DBuffers *gl_d3d_buffers, void *window_handle, bool stereo)
{

	printf("Creating OpenGL/Direct3D bridge...\n");

	ZeroMemory(gl_d3d_buffers, sizeof(GLD3DBuffers));
	gl_d3d_buffers->stereo = stereo;

	// WGL
	//
	// (Note: the window must have an OpenGL context for this to work;
	// SDL took care of that for us; also see wglGetCurrentDC)


	HWND hWnd = (HWND)window_handle;
	WINDOWINFO windowInfo;
	HRESULT result;
	// Fullscreen?
	result = GetWindowInfo(hWnd, &windowInfo);
	if (FAILED(result)) {
		D3D_EXCEPTION("Could not get window info");
	}
	g_fullscreen = (windowInfo.dwExStyle & WS_EX_TOPMOST) != 0;

	if (FAILED(result))
	{
		//MessageBox(NULL, "Could not get window info", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	RECT windowRect;
	result = GetClientRect(hWnd, &windowRect);

	unsigned int width = windowRect.right - windowRect.left;
	unsigned int height = windowRect.bottom - windowRect.top;
	g_width = width;
	g_height = height;
	gl_d3d_buffers->width = width;
	gl_d3d_buffers->height = height;
	sprintf_s(str, " Window size: %u, %u\n", width, height);
	//MessageBox(NULL, str, "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	HDC dc = GetDC(hWnd);
	if (wgl_LoadFunctions(dc) == wgl_LOAD_FAILED)
	{
		//MessageBox(NULL, "Failed to link to WGL API", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	if (wgl_ext_NV_DX_interop == wgl_LOAD_FAILED)
	{
		//MessageBox(NULL, "WGL NV_DX_interop extension not supported on this platform", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//MessageBox(NULL, "WGL NV_DX_interop extension supported on this platform", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	// Are we a WDDM OS?
	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
	if (!GetVersionEx(&osVersionInfo))
	{
		//MessageBox(NULL, "Could not get Windows version", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	BOOL wddm = osVersionInfo.dwMajorVersion == 6;
	if (wddm) {
		printf(" This operating system uses WDDM (it's Windows Vista or later)\n");
	}
	else {
		printf(" This operating system does not use WDDM (it's prior to Windows Vista)\n");
	}

	// Direct3D API
	//
	// (Note: Direct3D/OpenGL interop *requires* the use of 9Ex for WDDM-enabled operating systems)
	IDirect3D9 *d3d;
	if (wddm) {
		// We are dynamically linking to the Direct3DCreate9Ex function, because we
		// don't want to add a run-time dependency for it in our executable, which
		// would make it not run in Windows XP.
		//
		// See: http://msdn.microsoft.com/en-us/library/cc656710.aspx

		HMODULE d3dLibrary = LoadLibrary("d3d9.dll");
		if (!d3dLibrary)
		{
			//MessageBox(NULL, "Failed to link to d3d9.dll", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		gl_d3d_buffers->d3dLibrary = d3dLibrary;

		DIRECT3DCREATE9EXFUNCTION pfnDirect3DCreate9Ex = (DIRECT3DCREATE9EXFUNCTION)GetProcAddress(d3dLibrary, "Direct3DCreate9Ex");
		if (!pfnDirect3DCreate9Ex)
		{
			//MessageBox(NULL, "Failed to link to Direct3D 9Ex (WDDM)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}

		IDirect3D9 *d3dEx;
		result = (*pfnDirect3DCreate9Ex)(D3D_SDK_VERSION, (IDirect3D9Ex**)&d3dEx);
		if (result != D3D_OK)
		{
			//MessageBox(NULL, "Failed to activate Direct3D 9Ex (WDDM)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}

		if (d3dEx->QueryInterface(IID_IDirect3D9, (LPVOID*)&d3d) != S_OK)
		{

			//MessageBox(NULL, "Failed to cast Direct3D 9Ex to Direct3D 9", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}

		//MessageBox(NULL, "Activated Direct3D 9x", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	else {
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (!d3d)
		{
			//MessageBox(NULL, "Failed to activate Direct3D 9 (no WDDM)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		//MessageBox(NULL, " Activated Direct3D 9 (no WDDM)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	// Find display mode format
	//
	// (Our buffers will be the same to avoid conversion overhead)
	D3DDISPLAYMODE d3dDisplayMode;
	result = d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode);
	if (result != D3D_OK)
	{
		//MessageBox(NULL, "Failed to retrieve adapter display mode", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	D3DFORMAT d3dBufferFormat = d3dDisplayMode.Format;
	sprintf_s(str, "Retrieved adapter display mode, format: %u", d3dBufferFormat);
	//MessageBox(NULL, str, "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	// Make sure devices can support the required formats
	result = d3d->CheckDeviceFormat(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dBufferFormat,
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DEPTH_STENCIL_FORMAT);
	if (result != D3D_OK)
	{
		//MessageBox(NULL, "Our required formats are not supported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//MessageBox(NULL, "Our required formats are supported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	// Get the device capabilities
	D3DCAPS9 d3dCaps;
	result = d3d->GetDeviceCaps(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);
	if (result != D3D_OK)
	{
		//MessageBox(NULL, "Failed to retrieve device capabilities", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	//printf(" Retrieved device capabilities\n");
	// Verify that we can do hardware vertex processing
	if (d3dCaps.VertexProcessingCaps == 0)
	{
		//MessageBox(NULL, "Hardware vertex processing is not supported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//MessageBox(NULL, "Hardware vertex processing is supported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	// Register stereo (must happen *before* device creation)
	if (gl_d3d_buffers->stereo)
	{
		if (NvAPI_Initialize() != NVAPI_OK)
		{
			//MessageBox(NULL, "Failed to initialize NV API", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		else
		{

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
					g_NvSurround = TRUE;
				}
				else if (retVal == NVAPI_MOSAIC_NOT_ACTIVE)
				{
					// No Surround
					g_NvSurround = FALSE;
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
					{
						g_fullscreen = TRUE;
					}
					else
					{
						//fullscreen = FALSE;
					}
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
			}
		}

		//if (NvAPI_Stereo_CreateConfigurationProfileRegistryKey(NVAPI_STEREO_DX9_REGISTRY_PROFILE) != NVAPI_OK);
		//if (NvAPI_Stereo_Enable() != NVAPI_OK);
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

	if (g_fullscreen)
	{
		// This only applies in Surround
		d3dPresent.FullScreen_RefreshRateInHz = 120;
		d3dPresent.Windowed = FALSE;
	}
	else
	{
		d3dPresent.Windowed = TRUE;
		d3dPresent.hDeviceWindow = hWnd; // can be NULL in windowed mode, in which case it will use the arg in CreateDevice
	}

	IDirect3DDevice9 *d3dDevice;
	result = d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		//NULL, // used for ALT+TAB; must be top-level window in fullscreen mode; can be NULL in windowed mode
		//D3DCREATE_SOFTWARE_VERTEXPROCESSING,// | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE | D3DCREATE_PUREDEVICE,
		&d3dPresent,
		&d3dDevice);
	if (result != D3D_OK)
	{
		switch (result) {
		case D3DERR_DEVICELOST:
			//MessageBox(NULL, "Failed to create device: device lost", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case D3DERR_INVALIDCALL:
			//MessageBox(NULL, "Failed to create device: invalid call", "3D Vision Wrapper",  MB_OK | MB_ICONINFORMATION);
		case D3DERR_NOTAVAILABLE:
			//MessageBox(NULL, "Failed to create device: not available", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case D3DERR_OUTOFVIDEOMEMORY:
			//MessageBox(NULL, "Failed to create device: out of video memory", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		default:
			//			D3D_EXCEPTIONF("Failed to create device: unknown error %u", (int) result);
			break;
		}
	}
	gl_d3d_buffers->d3dDevice = d3dDevice;
	//printf(" Created device\n");
	//MessageBox(NULL, "Created device", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	// Enable Direct3D/OpenGL interop on device
	HANDLE d3dDeviceInterop = wglDXOpenDeviceNV(d3dDevice);
	if (!d3dDeviceInterop)
	{
		DWORD e = GetLastError();
		GLD3DBuffers_destroy(gl_d3d_buffers);
		switch (e)
		{
		case ERROR_OPEN_FAILED:
			//MessageBox(NULL, "Failed to enable OpenGL interop on device", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case ERROR_NOT_SUPPORTED:
			//MessageBox(NULL, "Failed to enable OpenGL interop on device: device not supported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		default:
			//			D3D_EXCEPTIONF("Failed to enable OpenGL interop on device: unknown error %u", (int) e);
			break;
		}
	}
	gl_d3d_buffers->d3dDeviceInterop = d3dDeviceInterop;
	//MessageBox(NULL, "Enabled OpenGL interop on device", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);



	// Enable stereo on device (WDDM only)
	if (gl_d3d_buffers->stereo && wddm)
	{
		if (NvAPI_Stereo_CreateHandleFromIUnknown((IUnknown *)d3dDevice, &nvStereo) != NVAPI_OK)
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to create NV stereo handle", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		gl_d3d_buffers->nvStereo = nvStereo;
		//MessageBox(NULL, " Created NV stereo handle on device:", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

		if (NvAPI_Stereo_Activate(nvStereo) != NVAPI_OK)
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to activate stereo", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		//MessageBox(NULL, "  Activated stereo", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);


		NvAPI_Stereo_GetSeparation(nvStereo, &g_separation);
		NvAPI_Stereo_GetConvergence(nvStereo, &g_convergence);
		NvAPI_Stereo_GetFrustumAdjustMode(nvStereo, &frustum);
		NvAPI_Stereo_GetEyeSeparation(nvStereo, &g_eyeSeparation);

		// Start our nvapi thread poll
		std::thread NVAPIPoll(NVAPIquery);
		NVAPIPoll.detach();
	}

	// Get device back buffer
	IDirect3DSurface9 *d3dBackBuffer;
	result = d3dDevice->GetBackBuffer(
		0, 0, D3DBACKBUFFER_TYPE_MONO, &d3dBackBuffer);
	if (result != D3D_OK)
	{
		GLD3DBuffers_destroy(gl_d3d_buffers);
		//MessageBox(NULL, "Failed to retrieve device back buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	gl_d3d_buffers->d3dBackBuffer = d3dBackBuffer;
	//MessageBox(NULL, "Retrieved device back buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

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
		GLD3DBuffers_destroy(gl_d3d_buffers);
		//MessageBox(NULL, "Failed to create color texture (left)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	gl_d3d_buffers->d3dLeftColorTexture = d3dLeftColorTexture;
	//MessageBox(NULL, " Created color texture(left)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

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
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to create color texture (right)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		gl_d3d_buffers->d3dRightColorTexture = d3dRightColorTexture;
		//MessageBox(NULL, " Created color texture (right)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	// Get Direct3D buffers from textures

	IDirect3DSurface9 *d3dLeftColorBuffer;
	result = d3dLeftColorTexture->GetSurfaceLevel(0, &d3dLeftColorBuffer);
	if (FAILED(result))
	{
		GLD3DBuffers_destroy(gl_d3d_buffers);
		//MessageBox(NULL, "Failed to retrieve color buffer from color texture (left)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	gl_d3d_buffers->d3dLeftColorBuffer = d3dLeftColorBuffer;
	//MessageBox(NULL, " Retrieved color buffer from color texture (left)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	if (gl_d3d_buffers->stereo)
	{
		IDirect3DSurface9 *d3dRightColorBuffer = 0;
		result = d3dRightColorTexture->GetSurfaceLevel(0, &d3dRightColorBuffer);
		if (FAILED(result))
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to retrieve color buffer from color texture (right)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		gl_d3d_buffers->d3dRightColorBuffer = d3dRightColorBuffer;
		//MessageBox(NULL, " Retrieved color buffer from color texture (right)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
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
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to create render target surface (stereo)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		gl_d3d_buffers->d3dStereoColorBuffer = d3dStereoColorBuffer;
		//MessageBox(NULL, " Created render target surface (stereo)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

		D3DLOCKED_RECT d3dLockedRect;
		result = d3dStereoColorBuffer->LockRect(&d3dLockedRect, NULL, 0);
		if (result != D3D_OK)
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to lock surface rect (stereo)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}

		// Insert stereo header into last row (the "+1") of stereo render target surface
		LPNVSTEREOIMAGEHEADER nvStereoHeader = (LPNVSTEREOIMAGEHEADER)(((unsigned char *)d3dLockedRect.pBits) + (d3dLockedRect.Pitch * gl_d3d_buffers->height));
		nvStereoHeader->dwSignature = NVSTEREO_IMAGE_SIGNATURE;

		// Check if Single GPU or SLI
		if (g_NvSLI == FALSE)
		{
			// We need to swap the eyes
			// For Broken Age this is not needed!
			//nvStereoHeader->dwFlags = SIH_SWAP_EYES;
			nvStereoHeader->dwFlags = SIH_SCALE_TO_FIT;
		}
		else if (g_NvSLI == TRUE)
		{
			// Normal
			nvStereoHeader->dwFlags = SIH_SCALE_TO_FIT2;
			//nvStereoHeader->dwFlags = 0;
		}

		// Note: the following all seem to be ignored
		//nvStereoHeader->dwWidth = gl_d3d_buffers->width * 2;
		//nvStereoHeader->dwHeight = gl_d3d_buffers->height;
		//nvStereoHeader->dwBPP = 32; // hmm, get this from the format? my netbook declared D3DFMT_X8R8G8B8, which is indeed 32 bits

		result = d3dStereoColorBuffer->UnlockRect();
		if (result != D3D_OK)
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to unlock surface rect (stereo)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		//MessageBox(NULL, " Inserted stereo header into render target surface (stereo)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
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
		GLD3DBuffers_destroy(gl_d3d_buffers);
		//MessageBox(NULL, "Failed to create depth/stencil surface)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	gl_d3d_buffers->d3dDepthBuffer = d3dDepthBuffer;
	//MessageBox(NULL, " Created depth/stencil surface\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	// OpenGL textures
	glGenTextures(1, &gl_d3d_buffers->texture_left);
	GLenum error = glGetError();
	if (error)
	{
		GLD3DBuffers_destroy(gl_d3d_buffers);
		//MessageBox(NULL, "Failed to create OpenGL color texture (left)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//MessageBox(NULL, " Created OpenGL color texture (left)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	if (gl_d3d_buffers->stereo)
	{
		glGenTextures(1, &gl_d3d_buffers->texture_right);
		GLenum error = glGetError();
		if (error)
		{
			GLD3DBuffers_destroy(gl_d3d_buffers);
			//MessageBox(NULL, "Failed to create OpenGL color texture (right)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		}
		//MessageBox(NULL, " Created OpenGL color texture (right)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	// OpenGL render buffer (one should be enough for stereo?)
	glGenTextures(1, &gl_d3d_buffers->render_buffer);
	error = glGetError();
	if (error)
	{
		GLD3DBuffers_destroy(gl_d3d_buffers);
		//MessageBox(NULL, "Failed to create OpenGL depth/stencil texture", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//MessageBox(NULL, " Created OpenGL depth/stencil texture\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	HANDLE d3dLeftColorInterop = wglDXRegisterObjectNV(d3dDeviceInterop, d3dLeftColorTexture, gl_d3d_buffers->texture_left, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
	if (!d3dLeftColorInterop) {
		DWORD e = GetLastError();
		GLD3DBuffers_destroy(gl_d3d_buffers);
		switch (e)
		{
		case ERROR_OPEN_FAILED:
			//MessageBox(NULL, "Failed to bind render target surface to OpenGL texture (left)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case ERROR_INVALID_DATA:
			//MessageBox(NULL, "Failed to bind render target surface to OpenGL texture (left): invalid data", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case ERROR_INVALID_HANDLE:
			//MessageBox(NULL, "Failed to bind render target surface to OpenGL texture (left): invalid handle", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		default:
			//			D3D_EXCEPTIONF("Failed to bind render target surface to OpenGL texture (left): unknown error %u", (int) e);
			break;
		}
	}
	gl_d3d_buffers->d3dLeftColorInterop = d3dLeftColorInterop;
	//MessageBox(NULL, " Bound render target surface to OpenGL texture (left)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	HANDLE d3dRightColorInterop;
	if (gl_d3d_buffers->stereo) {
		d3dRightColorInterop = wglDXRegisterObjectNV(d3dDeviceInterop, d3dRightColorTexture, gl_d3d_buffers->texture_right, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
		if (!d3dRightColorInterop) {
			DWORD e = GetLastError();
			GLD3DBuffers_destroy(gl_d3d_buffers);
			switch (e)
			{
			case ERROR_OPEN_FAILED:
				//MessageBox(NULL, "Failed to bind render target surface to OpenGL texture (right)", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
			case ERROR_INVALID_DATA:
				//MessageBox(NULL, "Failed to bind render target surface to OpenGL texture (right): invalid data", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
			case ERROR_INVALID_HANDLE:
				//MessageBox(NULL, "Failed to bind render target surface to OpenGL texture (right): invalid handle", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
			default:
				//				D3D_EXCEPTIONF("Failed to bind render target surface to OpenGL texture (right): unknown error %u", (int) e);
				break;
			}
		}
		gl_d3d_buffers->d3dRightColorInterop = d3dRightColorInterop;
		//MessageBox(NULL, " Bound render target surface to OpenGL texture (right)\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	// Bind OpenGL render buffer
	HANDLE d3dDepthInterop = wglDXRegisterObjectNV(d3dDeviceInterop, d3dDepthBuffer, gl_d3d_buffers->render_buffer, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
	if (!d3dDepthInterop) {
		DWORD e = GetLastError();
		GLD3DBuffers_destroy(gl_d3d_buffers);
		switch (e) {
		case ERROR_OPEN_FAILED:
			//MessageBox(NULL, "Failed to bind depth/stencil surface to OpenGL render buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case ERROR_INVALID_DATA:
			//MessageBox(NULL, "Failed to bind depth/stencil surface to OpenGL render buffer: invalid data", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case ERROR_INVALID_HANDLE:
			//MessageBox(NULL, "Failed to bind depth/stencil surface to OpenGL render buffer: invalid handle", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		default:
			//			D3D_EXCEPTIONF("Failed to bind depth/stencil surface to OpenGL render buffer: unknown error %u", (int) e);
			break;
		}
	}
	gl_d3d_buffers->d3dDepthInterop = d3dDepthInterop;
	//MessageBox(NULL, " Bound depth/stencil surface to OpenGL render buffer\n", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

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

	/*
	//Original
	glGenFramebuffersEXT(1, &gl_d3d_buffers->fbo_left);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, gl_d3d_buffers->fbo_left);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, gl_d3d_buffers->texture_left, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
	GLenum status_left = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	*/

	glGenFramebuffers(1, &gl_d3d_buffers->fbo_left);
	glBindFramebuffer(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_left);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_d3d_buffers->texture_left, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
	GLenum status_left = glCheckFramebufferStatus(GL_FRAMEBUFFER);


	GLenum status_right;
	if (gl_d3d_buffers->stereo)
	{
		/*
		//Original
		glGenFramebuffersEXT(1, &gl_d3d_buffers->fbo_right);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, gl_d3d_buffers->fbo_right);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, gl_d3d_buffers->texture_right, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, gl_d3d_buffers->render_buffer, 0);
		status_right = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		*/

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
		GLD3DBuffers_destroy(gl_d3d_buffers);
		switch (status_left)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): attachment", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): missing attachment", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): dimensions", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): formats", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): draw buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): read buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (left): unsupported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		default:
			//			D3D_EXCEPTIONF("Failed to create OpenGL frame buffer object (left): unknown error %u", (int) status_left);
			break;
		}
	}

	if (status_right != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		GLD3DBuffers_destroy(gl_d3d_buffers);
		switch (status_right)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): attachment", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): missing attachment", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): dimensions", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): formats", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): draw buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): read buffer", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			//MessageBox(NULL, "Failed to create OpenGL frame buffer object (right): unsupported", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
		default:
			//			D3D_EXCEPTIONF("Failed to create OpenGL frame buffer object (right): unknown error %u", (int) status_right);
			break;
		}
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	//MessageBox(NULL, " Created OpenGL frame buffer objects", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);

	gl_d3d_buffers->initialized = true;
}

/*
* Before using the OpenGL frame buffer object, we need to make sure to lock
* the Direct3D buffers.
*/
void GLD3DBuffers_activate_left(GLD3DBuffers *gl_d3d_buffers)
{
	//g_interop = true;
	wglDXLockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dLeftColorInterop);
	wglDXLockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dDepthInterop);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, gl_d3d_buffers->fbo_left);
	//g_interop = false;
}

void GLD3DBuffers_activate_right(GLD3DBuffers *gl_d3d_buffers)
{
	//g_interop = true;
	wglDXLockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 1, &gl_d3d_buffers->d3dRightColorInterop);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, gl_d3d_buffers->fbo_right);
	//g_interop = false;
}

void GLD3DBuffers_flip_left(GLD3DBuffers *gl_d3d_buffers)
{
	// Read to our FBO
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER, gl_d3d_buffers->fbo_left);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_auxFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	// This is the flipping
	glBlitFramebuffer(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Put the flipped image in the Back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_auxFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_d3d_buffers->fbo_left);

	glClear(GL_COLOR_BUFFER_BIT);
	glBlitFramebuffer(0, 0, g_width, g_height, 0, g_height, g_width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebufferEXT(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_left);
}

void GLD3DBuffers_flip_right(GLD3DBuffers *gl_d3d_buffers)
{
	// Read to our FBO
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER, gl_d3d_buffers->fbo_right);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_auxFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	// This is the flipping
	glBlitFramebuffer(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Put the flipped image in the Back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_auxFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_d3d_buffers->fbo_right);

	glClear(GL_COLOR_BUFFER_BIT);
	glBlitFramebuffer(0, 0, g_width, g_height, 0, g_height, g_width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebufferEXT(GL_FRAMEBUFFER, gl_d3d_buffers->fbo_right);
}

void GLD3DBuffers_deactivate(GLD3DBuffers *gl_d3d_buffers)
{
	HANDLE gl_handles[3];
	gl_handles[0] = gl_d3d_buffers->d3dLeftColorInterop;
	gl_handles[1] = gl_d3d_buffers->d3dRightColorInterop;
	gl_handles[2] = gl_d3d_buffers->d3dDepthInterop;

	// UNLOCK just the LEFT ONE to gain maximum FPS
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	wglDXUnlockObjectsNV(gl_d3d_buffers->d3dDeviceInterop, 3, gl_handles);
}

/*
* Flushing:
*
* 1. Copy our color buffer to the device's back buffer.
* 2. Present the device back buffer.
*/
void GLD3DBuffers_flush(GLD3DBuffers *gl_d3d_buffers)
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

#if 0
	//if (result == D3DERR_DEVICEHUNG)
	//if (result == D3DOK_NOAUTOGEN)
	{

		//MessageBox(NULL, "D3DOK_NOAUTOGEN", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_CONFLICTINGRENDERSTATE)
	{

		//MessageBox(NULL, "D3DERR_CONFLICTINGRENDERSTATE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_CONFLICTINGTEXTUREFILTER)
	{

		//MessageBox(NULL, "D3DERR_CONFLICTINGTEXTUREFILTER", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_CONFLICTINGTEXTUREPALETTE)
	{

		//MessageBox(NULL, "D3DERR_CONFLICTINGTEXTUREPALETTE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_DEVICEHUNG)
	{

		//MessageBox(NULL, "D3DERR_DEVICEHUNG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_DEVICELOST)
	{

		//MessageBox(NULL, "D3DERR_DEVICELOST", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_DEVICENOTRESET)
	{

		//MessageBox(NULL, "D3DERR_DEVICENOTRESET", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_DEVICEREMOVED)
	{

		//MessageBox(NULL, "D3DERR_DEVICEREMOVED", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_DRIVERINTERNALERROR)
	{

		//MessageBox(NULL, "D3DERR_DRIVERINTERNALERROR", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_INVALIDCALL)
	{

		//MessageBox(NULL, "D3DERR_INVALIDCALL", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_INVALIDDEVICE)
	{

		//MessageBox(NULL, "D3DERR_INVALIDDEVICE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_MOREDATA)
	{

		//MessageBox(NULL, "D3DERR_MOREDATA", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_NOTAVAILABLE)
	{

		//MessageBox(NULL, "D3DERR_NOTAVAILABLE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_NOTFOUND)
	{

		//MessageBox(NULL, "D3DERR_NOTFOUND", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_OUTOFVIDEOMEMORY)
	{

		//MessageBox(NULL, "D3DERR_OUTOFVIDEOMEMORY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_TOOMANYOPERATIONS)
	{

		//MessageBox(NULL, "D3DERR_TOOMANYOPERATIONS", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDALPHAARG)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDALPHAARG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDALPHAOPERATION)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDALPHAOPERATION", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDCOLORARG)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDCOLORARG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}

	//else if (result == D3DERR_UNSUPPORTEDCOLOROPERATION)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDCOLOROPERATION", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDFACTORVALUE)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDFACTORVALUE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDTEXTUREFILTER)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDTEXTUREFILTER", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_WASSTILLDRAWING)
	{

		//MessageBox(NULL, "D3DERR_WASSTILLDRAWING", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_WRONGTEXTUREFORMAT)
	{

		//MessageBox(NULL, "D3DERR_WRONGTEXTUREFORMAT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == E_FAIL)
	{

		//MessageBox(NULL, "E_FAIL", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == E_INVALIDARG)
	{

		//MessageBox(NULL, "E_INVALIDARG", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == E_NOINTERFACE)
	{

		//MessageBox(NULL, "E_NOINTERFACE", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == E_OUTOFMEMORY)
	{

		//MessageBox(NULL, "E_OUTOFMEMORY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == S_NOT_RESIDENT)
	{

		//MessageBox(NULL, "S_NOT_RESIDENT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == S_RESIDENT_IN_SHARED_MEMORY)
	{

		//MessageBox(NULL, "S_RESIDENT_IN_SHARED_MEMORY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == S_PRESENT_MODE_CHANGED)
	{

		//MessageBox(NULL, "S_PRESENT_MODE_CHANGED", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == S_PRESENT_OCCLUDED)
	{

		//MessageBox(NULL, "S_PRESENT_OCCLUDED", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDOVERLAY)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDOVERLAY", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDOVERLAYFORMAT)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDOVERLAYFORMAT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_CANNOTPROTECTCONTENT)
	{

		//MessageBox(NULL, "D3DERR_CANNOTPROTECTCONTENT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_UNSUPPORTEDCRYPTO)
	{

		//MessageBox(NULL, "D3DERR_UNSUPPORTEDCRYPTO", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
	//else if (result == D3DERR_PRESENT_STATISTICS_DISJOINT)
	{

		//MessageBox(NULL, "D3DERR_PRESENT_STATISTICS_DISJOINT", "3D Vision Wrapper", MB_OK | MB_ICONINFORMATION);
	}
#endif

}

void GLD3DBuffers_destroy(GLD3DBuffers *gl_d3d_buffers) {
	// Destroy NV stereo handle
	if (gl_d3d_buffers->nvStereo) {
		NvAPI_Stereo_DestroyHandle(gl_d3d_buffers->nvStereo);
		gl_d3d_buffers->nvStereo = NULL;
	}

	// Unbind OpenGL textures from Direct3D buffers
	if (gl_d3d_buffers->d3dDeviceInterop && gl_d3d_buffers->d3dLeftColorInterop) {
		wglDXUnregisterObjectNV(gl_d3d_buffers->d3dDeviceInterop, gl_d3d_buffers->d3dLeftColorInterop);
		gl_d3d_buffers->d3dLeftColorInterop = NULL;
	}
	if (gl_d3d_buffers->d3dDeviceInterop && gl_d3d_buffers->d3dRightColorInterop) {
		wglDXUnregisterObjectNV(gl_d3d_buffers->d3dDeviceInterop, gl_d3d_buffers->d3dRightColorInterop);
		gl_d3d_buffers->d3dRightColorInterop = NULL;
	}
	if (gl_d3d_buffers->d3dDeviceInterop && gl_d3d_buffers->d3dDepthInterop) {
		wglDXUnregisterObjectNV(gl_d3d_buffers->d3dDeviceInterop, gl_d3d_buffers->d3dDepthInterop);
		gl_d3d_buffers->d3dDepthInterop = NULL;
	}

	// Disable Direct3D/OpenGL interop
	if (gl_d3d_buffers->d3dDeviceInterop) {
		wglDXCloseDeviceNV(gl_d3d_buffers->d3dDeviceInterop);
		gl_d3d_buffers->d3dDeviceInterop = NULL;
	}

	// Release Direct3D objects
	if (gl_d3d_buffers->d3dLeftColorTexture) {
		IDirect3DSurface9 *d3dLeftColorTexture = (IDirect3DSurface9 *)gl_d3d_buffers->d3dLeftColorTexture;
		d3dLeftColorTexture->Release();
		gl_d3d_buffers->d3dLeftColorTexture = NULL;
	}
	if (gl_d3d_buffers->d3dRightColorTexture) {
		IDirect3DSurface9 *d3dRightColorTexture = (IDirect3DSurface9 *)gl_d3d_buffers->d3dRightColorTexture;
		d3dRightColorTexture->Release();
		gl_d3d_buffers->d3dRightColorTexture = NULL;
	}
	if (gl_d3d_buffers->d3dLeftColorBuffer) {
		IDirect3DSurface9 *d3dLeftColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dLeftColorBuffer;
		d3dLeftColorBuffer->Release();
		gl_d3d_buffers->d3dLeftColorBuffer = NULL;
	}
	if (gl_d3d_buffers->d3dRightColorBuffer) {
		IDirect3DSurface9 *d3dRightColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dRightColorBuffer;
		d3dRightColorBuffer->Release();
		gl_d3d_buffers->d3dRightColorBuffer = NULL;
	}
	if (gl_d3d_buffers->d3dStereoColorBuffer) {
		IDirect3DSurface9 *d3dStereoColorBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dStereoColorBuffer;
		d3dStereoColorBuffer->Release();
		gl_d3d_buffers->d3dStereoColorBuffer = NULL;
	}
	if (gl_d3d_buffers->d3dDepthBuffer) {
		IDirect3DSurface9 *d3dDepthBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dDepthBuffer;
		d3dDepthBuffer->Release();
		gl_d3d_buffers->d3dDepthBuffer = NULL;
	}
	if (gl_d3d_buffers->d3dBackBuffer) {
		IDirect3DSurface9 *d3dBackBuffer = (IDirect3DSurface9 *)gl_d3d_buffers->d3dBackBuffer;
		d3dBackBuffer->Release();
		gl_d3d_buffers->d3dBackBuffer = NULL;
	}
	if (gl_d3d_buffers->d3dDevice) {
		IDirect3DDevice9 *d3dDevice = (IDirect3DDevice9 *)gl_d3d_buffers->d3dDevice;
		d3dDevice->Release();
		gl_d3d_buffers->d3dDevice = NULL;
	}
	if (gl_d3d_buffers->d3dLibrary) {
		FreeLibrary((HINSTANCE)gl_d3d_buffers->d3dLibrary);
		gl_d3d_buffers->d3dLibrary = NULL;
	}

	// Delete OpenGL frame buffer object
	if (gl_d3d_buffers->fbo_left) {
		glDeleteFramebuffers(1, &gl_d3d_buffers->fbo_left);
		gl_d3d_buffers->fbo_left = 0;
	}

	// Delete OpenGL render buffer
	if (gl_d3d_buffers->render_buffer) {
		//glDeleteTextures(1, &gl_d3d_buffers->render_buffer);
		glDeleteRenderbuffersEXT(1, &gl_d3d_buffers->render_buffer);
		gl_d3d_buffers->render_buffer = 0;
	}

	// Delete OpenGL textures
	if (gl_d3d_buffers->texture_left) {
		glDeleteTextures(1, &gl_d3d_buffers->texture_left);
		gl_d3d_buffers->texture_left = 0;
	}
	if (gl_d3d_buffers->texture_right) {
		glDeleteTextures(1, &gl_d3d_buffers->texture_right);
		gl_d3d_buffers->texture_right = 0;
	}
}

// our NVAPI poll
static void NVAPIquery(void)
{
	while (!g_windowResizeCheck)
	{
		NvAPI_Stereo_GetSeparation(nvStereo, &g_separation);
		NvAPI_Stereo_GetConvergence(nvStereo, &g_convergence);
		NvAPI_Stereo_GetEyeSeparation(nvStereo, &g_eyeSeparation);

		// get the correct separation
		g_eyeSeparation = g_separation * g_eyeSeparation / 100;

		// This applies for window mode
		if ((g_NvSLI == FALSE) || (g_fullscreen == FALSE))
		{
			g_eyeSeparation = g_eyeSeparation * 2;
		}
		if (g_fullscreen == TRUE)
		{
			//g_eyeSeparation = g_eyeSeparation / 2;
		}

		if ((IsKeyDown(0x71) != 0) && (m_swapConvergence == FALSE)) //F2
		{
			m_swapConvergence = TRUE;
			m_preset2 = FALSE;
			m_preset3 = FALSE;
		}
		else if (IsKeyDown(0x71) != 0)
		{
			m_swapConvergence = FALSE;
		}

		if ((IsKeyDown(0x72) !=0) && (m_preset2 == FALSE)) //F3
		{
			m_preset2 = TRUE;
			m_swapConvergence = FALSE;
			m_preset3 = FALSE;
		}
		else if (IsKeyDown(0x72) != 0)
		{
			m_preset2 = FALSE;
		}
		if ((IsKeyDown(0x73) !=0 ) && (m_preset3 == FALSE)) //F4
		{
			m_preset3 = TRUE;
			m_swapConvergence = FALSE;
			m_preset2 = FALSE;
		}
		else if (IsKeyDown(0x73) != 0)
		{
			m_preset3 = FALSE;
		}
#ifdef NDEBUG
		else if (IsKeyDown(0xBB) != 0) // +key next shader
		{
			debugShaderIndex++;
			restoreDefault = true;
			injected = false;
		}
		else if (IsKeyDown(0xBD) != 0) // -key previous shader
		{
			debugShaderIndex--;
			restoreDefault = true;
			injected = false;
		}
		else if (IsKeyDown(0xDC) != 0) // \key reset shader
		{
			restoreDefault = true;
		}
#endif

		//See what to do
		if (m_swapConvergence)
		{
			g_eyeSeparation = g_eyeSeparation/10;
			g_convergence = g_convergence;
		}
		if (m_preset2)
		{
			g_eyeSeparation = g_eyeSeparation;
			g_convergence = g_convergence * 4;
		}
		if (m_preset3)
		{
			m_swapConvergence = FALSE;
			m_preset2 = FALSE;
			m_preset3 = FALSE;
		}

		Sleep(100);
	}
}

static bool IsKeyDown(int keyCode)
{
	//Return if the high byte is true (ie key is down)
	return (GetAsyncKeyState(keyCode) & 1) != 0;
}


#else

void GLD3DBuffers_create(GLD3DBuffers *gl_d3d_buffers, void *window_handle, bool vsync, bool stereo)
{
	printf("Direct3D not available on this platform\n");
}

void GLD3DBuffers_activate_left(GLD3DBuffers *gl_d3d_buffers) {
}

void GLD3DBuffers_activate_right(GLD3DBuffers *gl_d3d_buffers) {
}

void GLD3DBuffers_deactivate(GLD3DBuffers *gl_d3d_buffers) {
}

void GLD3DBuffers_flush(GLD3DBuffers *gl_d3d_buffers) {
}

void GLD3DBuffers_destroy(GLD3DBuffers *gl_d3d_buffers) {
}

#endif
