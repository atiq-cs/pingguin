#include "stdafx.h"
#include "SATaskBarNotification.h"
#include <Strsafe.h>

// MyTaskBarAddIcon - adds an icon to the notification area. 
// Returns TRUE if successful, or FALSE otherwise. 
// hwnd - handle to the window to receive callback messages. 
// uID - identifier of the icon. 
// hicon - handle to the icon to add. 
// lpszTip - ToolTip text. 

BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, UINT CallbackMessageID, HICON hicon, LPTSTR lpszTip) { 
    BOOL res; 
	NOTIFYICONDATA tnid = {0};
	HRESULT hr;
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd;
    tnid.uID = uID;
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnid.uCallbackMessage = CallbackMessageID;
    tnid.hIcon = hicon;
    if (lpszTip) {
        hr = StringCbCopyN(tnid.szTip, sizeof(tnid.szTip), lpszTip, 
                           sizeof(tnid.szTip));
       	if(FAILED(hr))
			MessageBox(NULL,
			_T("Error occurred!"),
			_T("Tray Notification"),
			NULL);
	}
    else
        tnid.szTip[0] = (TCHAR)'\0';
 
    res = Shell_NotifyIcon(NIM_ADD, &tnid);
 
    return res;
}

BOOL SAShowBalloonTip(HINSTANCE hInst, HWND hWnd, UINT uID, LPCTSTR pszTitle, LPCTSTR pszText, /*HICON hicon*/DWORD dwIcon)
{
	HRESULT hr;
	NOTIFYICONDATA nid={0};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_INFO;
	//nid.uFlags = NIF_INFO|NIF_ICON|NIF_TIP;
	nid.uID = uID;
	nid.hWnd = hWnd;
	nid.dwInfoFlags = NIIF_USER;

	hr = StringCchCopy(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle), pszTitle);

	// When we use m_hIcon which is created using LoadIcon in the constructor, just doesn't work
	HICON hicon = (HICON)LoadImage( //load up the icon:
		hInst, //get the HINSTANCE to this program
		MAKEINTRESOURCE(dwIcon), //grab the icon out of our resource file
		IMAGE_ICON, //tells the versatile LoadImage function that we are loading an icon
		16, 16, //x and y values. we want a 16x16-pixel icon for the tray.
		0); //no flags necessary. these flags specify special behavior, such as loading the icon from a file instead of a resource. see source list below for MSDN docs on LoadImage.*/

	if (IsWin7OrLater()) {
		nid.hBalloonIcon  = hicon;
	}
	else {
		nid.hIcon  = hicon;
	}
	//LoadIconMetric(g_hInst, MAKEINTRESOURCE(dwIcon), LIM_LARGE, &nid.hBalloonIcon);

	if(FAILED(hr))
	{
	// TODO: Write an error handler in case the call to StringCchCopy fails.
		MessageBox(NULL,
		_T("Error occurred!"),
		_T("Tray Notification"),
		NULL);
		return FALSE;
	}

	hr = StringCchCopy(nid.szInfo, ARRAYSIZE(nid.szInfo), pszText);

	if(FAILED(hr))
	{
	// TODO: Write an error handler in case the call to StringCchCopy fails.
		MessageBox(NULL,
		_T("Error occurred!"),
		_T("Tray Notification"),
		NULL);
		return FALSE;
	}

	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL SATaskBarDelIcon(HWND hwnd, UINT uID) { 
    BOOL res; 
	NOTIFYICONDATA tnid = {0};
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd;
    tnid.uID = uID;
    /*tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnid.uCallbackMessage = CallbackMessageID;
    tnid.hIcon = hicon;
    if (lpszTip) {
        hr = StringCbCopyN(tnid.szTip, sizeof(tnid.szTip), lpszTip, 
                           sizeof(tnid.szTip));
       	if(FAILED(hr))
			MessageBox(NULL,
			_T("Error occurred!"),
			_T("Tray Notification"),
			NULL);
	}
    else
        tnid.szTip[0] = (TCHAR)'\0';*/
 
    res = Shell_NotifyIcon(NIM_DELETE, &tnid);
 
    return res;
}

BOOL IsWin7OrLater()
{
    // Initialize the OSVERSIONINFOEX structure.
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osvi.dwMajorVersion = 6;
    osvi.dwMinorVersion = 1;

    // Initialize the condition mask.
    DWORDLONG dwlConditionMask = 0;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    // Perform the test.
    return VerifyVersionInfo(&osvi, 
                             VER_MAJORVERSION | VER_MINORVERSION,
                             dwlConditionMask);
}

