/*
	Author:		Saint Atique
	Desc:		Use of timer
				Implementation of timer using callback procedure
				Use of edit box, OnCreate and OnPaint
				Impelemnetation of ICMP echo,
				DNS
	Site:		http://saosx.com
	Last Mod:	17 April, 2011
*/

#include "stdafx.h"
#include "PingGUIN.h"
#include "CPingGUin_MainDlg.h"
#include "ProvideNetInfoDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Global variables declarations
static CString IpAddress[3];
static CString ipaddrstr;
static CStringA ipaddrstrA;

BOOL IsNotifyOn = FALSE;
/*	RunStage 1 means current target host is Gateway IP Address
 *	2 means Primary DNS Server
 *	3 means Secondary DNS Server
 */
BYTE RunStage = 1;

void CCustomCommandLineInfo::ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast) {
	if (bFlag) {
		// this is a "flag" (begins with / or -)
		m_options [pszParam] = "TRUE"; // default value is "TRUE"
		m_sLastOption = pszParam; // save in case other value specified 
	}
	else if (!m_sLastOption.IsEmpty ()) {
		// last token was option: set value
		m_options [m_sLastOption] = pszParam;
		m_sLastOption.Empty (); // clear
	} 

	// Call base class so MFC can see this param/token.
	CCommandLineInfo::ParseParam (pszParam, bFlag, bLast);
}

//Retrieve the value for option provided
BOOL CCustomCommandLineInfo::GetOption (LPCTSTR option, CString& val) {
	return m_options.Lookup (option, val);
}

BOOL CCustomCommandLineInfo::GetOption (LPCTSTR option) {
	return GetOption (option, CString ());
}

BOOL CSAApp::InitInstance() {
		// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	BOOL ClProvided = FALSE;
	CCustomCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.GetOption (_T("t"))) {
		//Write code to display help
		IsNotifyOn = TRUE;
	}

	if (cmdInfo.GetOption (_T("gw"), ipaddrstr)) {
		//Write code to display help
		IpAddress[0] = ipaddrstr;
		ClProvided = TRUE;
	}

	if (cmdInfo.GetOption (_T("pd"), ipaddrstr)) {
		//Write code to display help
		IpAddress[1] = ipaddrstr;
		ClProvided = TRUE;
	}

	if (cmdInfo.GetOption (_T("sd"), ipaddrstr)) {
		//Write code to display help
		IpAddress[2] = ipaddrstr;
		ClProvided = TRUE;
	}

	// Create the main window dialog required for data transfer
	CPingGUin_MainDlg *SAMainWindlg;

	if (ClProvided == FALSE) {
		// This is the way to create modal dialog
		NetInfoDialog* SADlg = new NetInfoDialog;
		INT_PTR nRet = -1;
		nRet = SADlg->DoModal();

		// Handle the return value from DoModal
		switch (nRet) {
			case -1: 
				AfxMessageBox(_T("Dialog box could not be created!"));
				break;
			case IDABORT:
				// Do something
				break;
			case IDOK:
				// Transfer info from previous dialog
				SAMainWindlg= new CPingGUin_MainDlg;
				SAMainWindlg->InitVars(SADlg);
				break;
			case IDCANCEL:
				return FALSE;
				break;
			default:
				// Do something
				break;
		};
		delete SADlg;
	}

	m_pMainWnd = SAMainWindlg;

	INT_PTR nResponse = SAMainWindlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		AfxMessageBox(_T("You clicked close!"));
	}
	AfxMessageBox(_T("Reached here!"));
	delete SAMainWindlg;
	SAMainWindlg = NULL;
	m_pMainWnd = NULL;

	/*LoadIcon(IDI_ICON1);
	m_pMainWnd = new SAFrame;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();*/

	return FALSE;
}

CSAApp theApp;

/*void SAFrame::PingQuit(TCHAR *str) {
	if (!IsWindowVisible())
		ShowWindow(SW_RESTORE);
	PostMessage(WM_PAINT, (LPARAM)0, (LPARAM)0);
	KillTimer(ID_TIMER);
	SetForegroundWindow();
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

BOOL SAFrame::PreCreateWindow(CREATESTRUCT &cs) {
	// call default implementation
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// fix the dimension
	cs.cx = 380;
	cs.cy = 185;

	// disable sizing
	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~WS_MAXIMIZEBOX;

	return TRUE;
}

// Create Message handling
int SAFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Call the base class to create the window
	if( CFrameWnd::OnCreate(lpCreateStruct) != 0)
		return -1;

	CRect PRect;
	this->GetClientRect(PRect);
	m_Edit01.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOVSCROLL | ES_READONLY,
		          PRect, this, 0x188);
	// Since the window was successfully created, return 0

	return 0;
}

void SAFrame::OnPaint() {
	CFrameWnd::OnPaint();
	SAFrame* pMainWnd = (SAFrame*) AfxGetMainWnd ();
	if (pMainWnd->TextStr[0])
		m_Edit01.SetWindowText(CString(pMainWnd->TextStr));
}

void SAFrame::OnSize(UINT nType, int cx, int cy) {
	CFrameWnd::OnSize(nType, cx, cy);
  
	//track the window minimizing message
   if(nType == SIZE_MINIMIZED) {
		NOTIFYICONDATA niData;
		ZeroMemory(&niData,sizeof(NOTIFYICONDATA));

		niData.cbSize = sizeof(niData);
		niData.hWnd   = this->m_hWnd;
		niData.uID    = 100;
		niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		//u could load ur own icon here
		niData.hIcon  = AfxGetApp()->LoadIcon(IDI_APPLICATION);

		//set the CALLBACK message ID
		niData.uCallbackMessage =  WM_USER + 200; 

		//Adds our App. icon to the taskbar status area
		Shell_NotifyIcon( NIM_ADD, &niData);     

		//hide our application/window
		this->ShowWindow(SW_HIDE);        
	}
}

void SAFrame::OnMaximizeApp()
{
   ShowWindow(SW_RESTORE);
}

void SAFrame::OnExit() {
	PostMessage(WM_QUIT, (LPARAM)0, (LPARAM)0);
}

void SAFrame::OnClose() {
	this->KillTimer(ID_TIMER);
	DestroyWindow();
}

LRESULT SAFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
   //if  the message is from our application
   //which is hiding in the tray area
   if( message == (WM_USER + 200) ) {
      //trace the right mouse click msg
      if(lParam == WM_RBUTTONDOWN)
      {
         CMenu myMenu;
         myMenu.CreatePopupMenu();        
        
         POINT pt;
         //Get the current cursor point
         GetCursorPos(&pt);
        
         //add menu items
         myMenu.AppendMenu( MF_POPUP, WM_USER + 1, _T("Maximize App.") );
         myMenu.AppendMenu( MF_POPUP, WM_USER + 2, _T("Exit") );

         myMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,pt.x,pt.y,this );
      }  
   }
   return CFrameWnd::WindowProc(message, wParam, lParam);
}

*/