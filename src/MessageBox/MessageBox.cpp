////////////////////////////////////////////////////////////////////////////////
// CustomMessageBox.cpp : Custom MesageBox impementation.                     //
////////////////////////////////////////////////////////////////////////////////
#include "MessageBox.h"

// Declare CBT hook procedure
static LRESULT CALLBACK CbtHookProc(int, WPARAM, LPARAM);
// Declare custom message box procedure
LRESULT CALLBACK CustomMessageBoxProc(HWND, UINT, WPARAM, LPARAM);

////////////////////////////////////////////////////////////////////////////////
// CustomMessageBoxValues structure                                           //
////////////////////////////////////////////////////////////////////////////////
// Keeps data along custom message box functions and the hook procedure       //
////////////////////////////////////////////////////////////////////////////////
struct CustomMessageBoxValues
{
	HHOOK hHook;          // hook handle
	HWND  hWnd;           // message box handle
	WNDPROC lpMsgBoxProc; // window procedure
	COLORREF crText;      // text color
	HBRUSH hBrush;        // background brush
	HBITMAP hBitmap;      // hadle to backround bitmap
} static __declspec(thread) cmbv;

////////////////////////////////////////////////////////////////////////////////
// Custom message box function                                                //
////////////////////////////////////////////////////////////////////////////////
// Call this instead of MessageBox API function in order to display           //
// a customized message box.                                                  //
////////////////////////////////////////////////////////////////////////////////
int CustomMessageBox(HWND hWnd, LPCTSTR lpText,
	LPCTSTR lpCaption, UINT uType)
{
	// init CustomMessageBoxValues structure values
	cmbv.hHook = NULL;
	cmbv.hWnd = NULL;
	cmbv.lpMsgBoxProc = NULL;

	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	//  installs the hook procedure into a hook chain
	cmbv.hHook = SetWindowsHookEx(WH_CBT, CbtHookProc, hInstance, GetCurrentThreadId());

	// call "standard" MessageBox Windows API function 
	int nRet = MessageBox(hWnd, lpText, lpCaption, uType);

	// removes the hook procedure from the hook chain
	::UnhookWindowsHookEx(cmbv.hHook);
	return nRet;
}

////////////////////////////////////////////////////////////////////////////////
// Custom Message box window procedure                                        //
////////////////////////////////////////////////////////////////////////////////
// For making the demo application easier to understand, it only demonstrates //
// how to change the background and the text color.                           //
// You can further add any kind of customization.                             //
// As for example make owner drawn  buttons, create additional controls, set  //
// non-standard icons, change the window styles, and so on.                   //
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CustomMessageBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
						  // load the brush used for backround
						  cmbv.crText = RGB(0, 0, 255);
						  HINSTANCE hInstance = ::GetModuleHandle("openGl32.dll");
#ifdef DEBUG_WRAPPER
						  cmbv.hBitmap = (HBITMAP)::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
#else
						  // Load our logo
						  cmbv.hBitmap = (HBITMAP)LoadImage(hInstance, "Logo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
						  if (cmbv.hBitmap == 0)
						  {
							  // If we don't find it load our internal logo (from resources)
							  cmbv.hBitmap = (HBITMAP)::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
						  }
#endif
						  cmbv.hBrush = ::CreatePatternBrush(cmbv.hBitmap);
	}
		break;
	case WM_CTLCOLORDLG:
		break;
	case WM_CTLCOLORSTATIC:
	{
							  // set the custom background and text color
							  HDC hDC = (HDC)wParam;
							  ::SetBkMode(hDC, TRANSPARENT);
							  ::SetTextColor(hDC, RGB(0, 0, 128));
							  return (LRESULT)cmbv.hBrush;
	}
		break;
	case WM_COMMAND:
	{
					   // performing cleanup before exit
					   ::DeleteObject(cmbv.hBrush);
					   ::DeleteObject(cmbv.hBitmap);
	}
		break;
	}
	return CallWindowProc(cmbv.lpMsgBoxProc, hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// CBT (computer-based training) hook procedure                               //
////////////////////////////////////////////////////////////////////////////////
// Catch the dialog box creation in order to subclass it                      //
//////////////////////////////////////////////////////////////////////////////// 
LRESULT CALLBACK CbtHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return ::CallNextHookEx(cmbv.hHook, nCode, wParam, lParam);
	}
	switch (nCode)
	{
	case HCBT_CREATEWND: // a window is about to be created
	{
							 LPCBT_CREATEWND lpCbtCreate = (LPCBT_CREATEWND)lParam;
							 if (WC_DIALOG == lpCbtCreate->lpcs->lpszClass)
							 {
								 // WC_DIALOG is the class name of message box
								 // but it has not yet a window procedure set.
								 // So keep in mind the handle to sublass it later
								 // when its first child is about to be created
								 cmbv.hWnd = (HWND)wParam;
							 }
							 else
							 {
								 if ((NULL == cmbv.lpMsgBoxProc) && (NULL != cmbv.hWnd))
								 {
									 // subclass the dialog 
									 cmbv.lpMsgBoxProc =
									 (WNDPROC)::SetWindowLongPtr(cmbv.hWnd, GWLP_WNDPROC,
										 (INT64)CustomMessageBoxProc);
								 }
							 }
	}
		break;
	case HCBT_DESTROYWND: // a window is about to be destroyed
	{
							  if (cmbv.hWnd == (HWND)wParam) // it's our messge box
							  {
								  // so set back its default procedure
								  ::SetWindowLongPtr(cmbv.hWnd, GWLP_WNDPROC,
									  (INT64)cmbv.lpMsgBoxProc);
							  }
	}
	}
	return 0;
}
