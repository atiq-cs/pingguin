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

#include "PingGUIN.h"
#include "ProvideNetInfoDialog.h"

static CString IpAddress[3];
static CString ipaddrstr;
static CStringA ipaddrstrA;

BOOL IsToggleMode = FALSE;
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

SAFrame::SAFrame() {
	// Create the window's frame
	Create(NULL, _T("SAOSX PingGUIn"), WS_OVERLAPPEDWINDOW,
	       CRect(120, 100, 500, 285), NULL);
	TimeOutInterval = 5100;
	SetTimer (ID_TIMER, TimeOutInterval-100, &SAFrame::TimerProc);
	_stprintf_s(TextStr, MSGSIZE, _T("\r\n\tPing MessageBox"));
	CountResponse = 0;
	CountRequest = 0;
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
	m_Edit01.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
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

void PingQuit(SAFrame* pMainWnd, TCHAR *str);

void CALLBACK EXPORT SAFrame::TimerProc (HWND hWnd, UINT nMsg, UINT nTimerID, DWORD dwTime) {
	// Code
	SAFrame* pMainWnd = (SAFrame*) AfxGetMainWnd ();
	TCHAR *pstr = pMainWnd->TextStr;
	pstr[0] = '\0';
    
	HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;
	pMainWnd->CountRequest++;

	if (RunStage > 0 && RunStage < 4)
		ipaddrstr = IpAddress[RunStage-1];
	else {
        _stprintf_s(pstr, MSGSIZE, _T("\r\nRun stage invalid. Please check.\r\n"));
		pMainWnd->PingQuit(pstr);
	}

	ipaddrstrA = ipaddrstr;
	ipaddr = inet_addr(ipaddrstrA);

    if (ipaddr == INADDR_NONE) {
        _stprintf_s(pstr, MSGSIZE, _T("\r\nusage: pingguin /t [optional] /ip IPAddress\r\n"));
		pMainWnd->PingQuit(pstr);
		return ;
    }
 
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        _stprintf_s(pstr, MSGSIZE, _T("\tUnable to open handle.\r\n"));
        _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("IcmpCreatefile returned error: %ld\r\n"), GetLastError() );
		pMainWnd->PingQuit(pstr);
		return ;
    }

    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID*) malloc(ReplySize);

    if (ReplyBuffer == NULL) {
        _stprintf_s(pstr, MSGSIZE, _T("\tUnable to allocate memory\r\n"));
		pMainWnd->MessageBox(pstr);
        pMainWnd->PingQuit(pstr);
		return ;
    }
	switch (RunStage) {
	case 1:
		_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Target: Gateway IP [%s] "), ipaddrstr);
		break;
	case 2:
		_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\nTarget: Primary DNS Server [%s] "), ipaddrstr);
		break;
	case 3:
		_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\nTarget: Secondary DNS Server [%s] "), ipaddrstr);
		break;
	default:
		break;
	}
	
	_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\nRequest [%d] "), pMainWnd->CountRequest);

    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
        NULL, ReplyBuffer, ReplySize, pMainWnd->TimeOutInterval);

    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T(" sent to %s\r\n"), ipaddrstr);

		if (dwRetVal > 1) {
            _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\tReceived %ld icmp message responses\r\n\r\n"), dwRetVal);
            _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\tInformation from the first response:\r\n")); 
        }    
        else {    
            _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Received %ld icmp message response\r\n"), dwRetVal);
        }    
        _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Received from %s\r\n"), CString(inet_ntoa( ReplyAddr )) );
		if (pEchoReply->Status == 11003)
			_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Status = Request timed out.\r\n"));
		else
			_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Status = %ld\r\n"), 
				pEchoReply->Status);
        _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Roundtrip time = %ld milliseconds\r\n"), 
            pEchoReply->RoundTripTime);

		if (IsToggleMode == TRUE && pEchoReply->Status != 11003) {
			_stprintf_s(pstr, MSGSIZE, _T("\r\nServer is up.\r\n\r\nClick close button to quit the program."));
			pMainWnd->PingQuit(pstr);
		}
		pMainWnd->CountResponse = 0;
    }
    else {
		int errorno = GetLastError();
		_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("lost in the network\r\n"));
		switch (errorno) {
			case 11010:
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Timed out due to possible unreachability\r\n"),  errno);
				pMainWnd->CountResponse++;
				break;
			default:
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("IcmpSendEcho returned error: %ld\r\n"),  errorno);
		}
		if (IsToggleMode == FALSE) {
			if (pMainWnd->CountResponse >= 48) {
				_stprintf_s(pstr, MSGSIZE, _T("Target client is possibly down.\r\n\r\nClick close button to quit the program."));
				pMainWnd->PingQuit(pstr);
				return ;
			}
			else {
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\n\r\nAnalyzing target host's unavailability.\r\n"));
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Absence of reply count %d.\r\n"), pMainWnd->CountResponse);
				pMainWnd->PostMessage(WM_PAINT, (LPARAM)0, (LPARAM)0);
				return ;
			}
		}
		else
			_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\nServer is still dumb."));
    }
	pMainWnd->PostMessage(WM_PAINT, (LPARAM)0, (LPARAM)0);
}


BOOL CSAApp::InitInstance() {
	CCustomCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.GetOption (_T("t"))) {
		//Write code to display help
		IsToggleMode = TRUE;
	}

	if (cmdInfo.GetOption (_T("gw"),ipaddrstr)) {
		//Write code to display help
		IpAddress[0] = ipaddrstr;
	}
	else {
		// This is the way to create modal dialog
		NetInfoDialog SADlg;
		INT_PTR nRet = -1;
		nRet = SADlg.DoModal();

		// Handle the return value from DoModal
		switch (nRet) {
			case -1: 
				AfxMessageBox(_T("Dialog box could not be created!"));
				break;
			case IDABORT:
				// Do something
				break;
			case IDOK:
				// Retrieve available information
				SADlg.GetNetInfo(IpAddress);
				break;
			case IDCANCEL:
				return FALSE;
				break;
			default:
				// Do something
				break;
		};

	}

	m_pMainWnd = new SAFrame;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

CSAApp theApp;

void SAFrame::PingQuit(TCHAR *str) {
	if (!IsWindowVisible())
		ShowWindow(SW_RESTORE);
	PostMessage(WM_PAINT, (LPARAM)0, (LPARAM)0);
	KillTimer(ID_TIMER);
	SetForegroundWindow();
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
