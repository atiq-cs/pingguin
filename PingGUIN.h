// Windows XP and later Compilation
#define WINVER	_WIN32_WINNT_WINXP
#include <afxwin.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define ID_TIMER 101
#define MSGSIZE	1000

class CCustomCommandLineInfo : public CCommandLineInfo {
public:
	CCustomCommandLineInfo() {}
	INT m_nError;
	//This function will retrieve the value of the option provided	
	BOOL GetOption (LPCTSTR option, CString& val);

	//This function will check for the commandline without ny options
	BOOL GetOption (LPCTSTR option);

protected:
	CMapStringToString m_options; // hash of options
	CString m_sLastOption; // last option encountered

	//Parse param overrided
	virtual void ParseParam (const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
};

class SAFrame : public CFrameWnd
{
public:
	SAFrame ();

protected:
	BOOL PreCreateWindow(CREATESTRUCT &cs);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
	static void CALLBACK EXPORT TimerProc (HWND hWnd, UINT nMsg, UINT nTimerID, DWORD dwTime);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	TCHAR TextStr[2 * MSGSIZE];
	CEdit m_Edit01;


	void PingQuit(TCHAR *str);
	void OnMaximizeApp();
	void OnExit();


private:
	int CountResponse;
	int CountRequest;
	int TimeOutInterval;
};

class CSAApp: public CWinApp {
public:
	BOOL InitInstance();
};

BEGIN_MESSAGE_MAP(SAFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	// System Tray Implementation
	ON_WM_SIZE()
	ON_COMMAND(WM_USER+1,  OnMaximizeApp)
	ON_COMMAND(WM_USER+2,  OnExit)
END_MESSAGE_MAP()

