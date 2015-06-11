
/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef __WINDOW__MANAGER_H__
#define __WINDOW__MANAGER_H__

#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include "windows.h"
// To get current PID
#include <TlHelp32.h>
#include "Psapi.h"

// function to get the exe pid
DWORD GetPidFromExeName(void);

// Get the Window Handle of the current PID
HWND FindWindowFromProcessId(void);

// Return the name of the executable
const char* WindowGetExeName(void);

// Return the ID of the process.
DWORD WindowGetPid(void);

// Thread that will check the Window Resize
void StartResizeThread(void);

// Stop the thread
void StopResizeThread(void);

#endif