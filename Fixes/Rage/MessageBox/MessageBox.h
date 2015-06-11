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