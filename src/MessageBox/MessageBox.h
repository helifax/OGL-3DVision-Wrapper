/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef __MESSAGE_BOX_H__
#define __MESSAGE_BOX_H__

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <commdlg.h>
#include "resource.h"

// Custom message box function
int CustomMessageBox(HWND, LPCTSTR, LPCTSTR, UINT);

// Main dialog procedure
BOOL CALLBACK MainDialogProc(HWND, UINT, WPARAM, LPARAM);

// Main dialog message handler functions
BOOL MainDlg_OnInitDialog(HWND, HWND, LPARAM);
void MainDlg_OnClose(HWND);
void MainDlg_OnCommand(HWND, int, HWND, UINT);
HBRUSH MainDlg_OnCtlColor(HWND, HDC, HWND, int);

// Buttons clicked notification handlers
void MainDlg_OnButtonClickedMessageBox(HWND);

#endif