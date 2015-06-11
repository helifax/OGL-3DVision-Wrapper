/*
* Game-Deception Blank Wrapper v2
* Copyright (c) Helifax 2015
*/

#include <thread>
#include "WindowManager.h"


extern "C" {
#include "opengl_3dv.h"
}
#include "opengl32.h"
#include "ShaderManager.h"
#include "ConfigReader.h"

// Static functions
static void _getApplicationName(char *exeName);
static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam);
static void _windowResize(void);

// Static variables
static DWORD m_pid = 0;
static char m_exeName[255];
static bool m_isThreadRunning = true;

// Globals
// 3D Vision
extern GLD3DBuffers * gl_d3d_buffers;
BOOL g_fullscreen = TRUE;
extern bool g_windowResizeCheck;
extern HWND g_MainWindowHandle;
extern unsigned int g_width;
extern unsigned int g_height;
static bool m_isThreadInitialised = false;
extern configReader *g_reader;

struct EnumData
{
	DWORD dwProcessId;
	HWND hWnd;
};
///-------------------------------------------------------------------------------------------

void StartResizeThread(void)
{
	if (!m_isThreadInitialised)
	{
		add_log("Starting Resize Detection Thread");

		//Start our window resize checker thread
		std::thread ResizeCheck(_windowResize);
		ResizeCheck.detach();
		m_isThreadInitialised = true;
	}
}
///-------------------------------------------------------------------------------------------

void StopResizeThread(void)
{
	m_isThreadRunning = false;
}
///-------------------------------------------------------------------------------------------

const char* WindowGetExeName(void)
{
	return m_exeName;
}
///-------------------------------------------------------------------------------------------

DWORD WindowGetPid(void)
{
	return m_pid;
}
///-------------------------------------------------------------------------------------------

DWORD GetPidFromExeName(void)
{
	// Do some magic from here on
	_getApplicationName(m_exeName);

	DWORD pid = 0;
	HANDLE hsnap;
	PROCESSENTRY32 pt;
	hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		if (!strcmp(pt.szExeFile, m_exeName))
		{
			pid = pt.th32ProcessID;
			CloseHandle(hsnap);
			// Save the current PID
			m_pid = pid;
			return pid;
		}
	} while (Process32Next(hsnap, &pt));
	return 0;
}
///-------------------------------------------------------------------------------------------

HWND FindWindowFromProcessId(void)
{
	if (m_pid != 0)
	{
		EnumData ed = { m_pid };
		if (!EnumWindows(EnumProc, (LPARAM)&ed) && (GetLastError() == ERROR_SUCCESS))
		{
			return ed.hWnd;
		}
	}
	add_log("Cannot Find current Window handle");
	return NULL;
}
///-------------------------------------------------------------------------------------------

static void _getApplicationName(char *exeName)
{
	TCHAR exepath[MAX_PATH + 1];
	// Get the current exe name
	if (GetModuleFileName(0, exepath, MAX_PATH + 1) != 0)
	{
		const char* toFind = "\\";
		const char* prevAddress = 0;
		const char *posFound = strstr(exepath, toFind);
		//exeName
		while (posFound)
		{
			// Remove the slash
			posFound++;
			prevAddress = posFound;
			posFound = strstr(posFound, toFind);
		}

		// Copy only the exe name
		strcpy_s(exeName, 255, prevAddress);
	}
}
///-------------------------------------------------------------------------------------------

// Application-defined callback for EnumWindows
static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
{
	// Retrieve storage location for communication data
	EnumData& ed = *(EnumData*)lParam;
	DWORD dwProcessId = 0x0;

	// Query process ID for hWnd
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	// Apply filter - if you want to implement additional restrictions,
	// this is the place to do so.
	if ((ed.dwProcessId == dwProcessId) && hWnd == GetForegroundWindow())
	{
		// Found a window matching the process ID
		ed.hWnd = hWnd;
		// Report success
		SetLastError(ERROR_SUCCESS);
		// Stop enumeration
		return FALSE;
	}
	// Continue enumeration
	return TRUE;
}
///-------------------------------------------------------------------------------------------

static void _windowResize(void)
{
	RECT windowRect;
	unsigned int width;
	unsigned int height;
	WINDOWINFO windowInfo;

	// Always Keep this thread alive
	while (m_isThreadRunning)
	{
		// Do this ONLY if the init is done
		if (NV3DVisionIsNotInit() == false)
		{
			HWND secondaryWindowHandle = FindWindowFromProcessId();
			// If we have a valid Window Handle
			if (secondaryWindowHandle)
			{
				GetClientRect(secondaryWindowHandle, &windowRect);
				width = windowRect.right - windowRect.left;
				height = windowRect.bottom - windowRect.top;
				GetWindowInfo(secondaryWindowHandle, &windowInfo);
				BOOL fullscreen = (windowInfo.dwExStyle & WS_EX_TOPMOST) != 0;

				// If we don't want fullscreen detection we disable it.
				if (!g_reader->IsFullScreenDetectionEnabled())
				{
					fullscreen = g_fullscreen;
				}

				// Check fullscreen mode or resolution changes
				if ((secondaryWindowHandle != 0x00) && ((width != g_width) || (height != g_height) || (g_fullscreen != fullscreen) ))
				{
					// Update our Main handle (used to create the context)
					g_MainWindowHandle = secondaryWindowHandle;
					g_fullscreen = fullscreen;
#ifdef DEBUG_WRAPPER
					// Get the shaderManager
					ShaderManager * shaderManager = ShaderManager::getInstance();
					if (shaderManager->VertexShadersExceptionsEnabled() == false)
#endif
						// End the current thread
						g_windowResizeCheck = true;

					add_log("Resize Triggered; Regenerating the context !");
					add_log("---");
					add_log("---");
					add_log("---");
				}
			}
		}
		Sleep(500);
	}
	add_log("Resize Thread Stopped");
}
///-------------------------------------------------------------------------------------------