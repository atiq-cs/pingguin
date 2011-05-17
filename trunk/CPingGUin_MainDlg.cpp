/*
	Author:		Saint Atique
	Desc:		Use of timer
				Implementation of timer using callback procedure
				Use of edit box, OnCreate and OnPaint
				Impelemnetation of ICMP echo,
				DNS Look up
	Site:		http://saosx.com
	Last Mod:	17 April, 2011
*/

// CPingGUin_MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PingGUIN.h"
#include "ProvideNetInfoDialog.h"
#include "CPingGUin_MainDlg.h"
#include "SANetUtil.h"
#include "afxdialogex.h"
#include "SATaskBarNotification.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UM_TRAYNOTIFY	(WM_USER + 1)
#define ID_MINTRAYICON 0x201

/////////////////////////////////////////////////////////////////////////////
// CPingGUin_MainDlg dialog

BEGIN_MESSAGE_MAP(CPingGUin_MainDlg, CDialogEx)
	//{{AFX_MSG_MAP(NetInfoDialog)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(UM_TRAYNOTIFY, OnTrayNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPingGUin_MainDlg::CPingGUin_MainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPingGUin_MainDlg::IDD, pParent)
	, noReq(0)
	, noReplies(0)
	, IsSingleHost(false)
	, MaxPingReqs(0)
	, RunStage(0)
	, CurReportStatic(NULL)
	, _bVisible(TRUE)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

CPingGUin_MainDlg::~CPingGUin_MainDlg() {
	// if we do a killtimer here the application crashes
	//Shell_NotifyIcon( NIM_DELETE, &m_nid ); // delete from the status area
}

void CPingGUin_MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_PINGPROGRESS, ProgressPingReq);
}

// CPingGUin_MainDlg message handlers

BOOL CPingGUin_MainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	/* tray
		_ToolTipCtrl.Create( this, 
						 // the ToolTip control's style 
		                 TTS_NOPREFIX | // prevents the system from stripping the ampersand (&) 
						                // character from a string

						 TTS_BALLOON  | // the ToolTip control has the appearance of
						 // 0x40        // a cartoon "balloon," with rounded corners 
										// and a stem pointing to the item. 

						 TTS_ALWAYSTIP  // the ToolTip will appear when the
	                                    // cursor is on a tool, regardless of 
	                                    // whether the ToolTip control's owner
	                                    // window is active or inactive
					   );

	 SetIconAndTitleForBalloonTip( &_ToolTipCtrl, TTI_INFO, _T("Easy App - (title)"));*/


	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Set the range for progress control
	ProgressPingReq = (CProgressCtrl *) GetDlgItem(IDC_PINGPROGRESS);
	ProgressPingReq->SetRange(0, MaxPingReqs); 
	ProgressPingReq->SetStep(1);
	ProgressPingReq->SetPos(1);

	PingIP = &mainIP;
	SetDlgItemText(IDC_TARGETIP, *PingIP);

	if (IsSingleHost) {
		ReportStaticDefaultText = _T("Single Host");
		SetDlgItemText(IDC_TARGETTYPE, ReportStaticDefaultText);
		RunStage = 3;
		// For single host select the middle static control for writing
		CurReportStatic = (CStatic *) GetDlgItem(IDC_REPORT_PDNS);
	}
	else {
		ReportStaticDefaultText = _T("Default Gateway");
		SetDlgItemText(IDC_TARGETTYPE, ReportStaticDefaultText);
		CurReportStatic = (CStatic *) GetDlgItem(IDC_REPORT_DG);
	}

	m_nWindowTimer = SetTimer(1, 2000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*void CPingGUin_MainDlg::LoadToTray( CWnd    *pWnd, 
	  	                      UINT	  uCallbackMessage,
				              CString sInfoTitle, // title for a balloon ToolTip.
												  // This title appears in boldface above the text.
												  // It can have a maximum of 63 characters
				              CString sInfo, // the text for a balloon ToolTip, it can have
							                 // a maximum of 255 characters
				              CString sTip, // the text for a standard ToolTip. 
											// It can have a maximum of 128 characters, 
											// including the terminating NULL.
			                  int     uTimeout, // in sec.
				              HICON	  icon )
{
	//NOTIFYICONDATA contains information that the system needs to process taskbar status area messages

	ZeroMemory( &m_nid, sizeof( NOTIFYICONDATA ) );

	m_nid.cbSize		      = sizeof( NOTIFYICONDATA );
	m_nid.hWnd			  = pWnd->GetSafeHwnd();
	m_nid.uID			  = 0;
	
	m_nid.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
			// Flag Description:
			// - NIF_ICON	 The hIcon member is valid.  
	        // - NIF_MESSAGE The uCallbackMessage member is valid. 
	        // - NIF_TIP	 The szTip member is valid. 
	        // - NIF_STATE	 The dwState and dwStateMask members are valid. 
	        // - NIF_INFO	 Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid. 
            // - NIF_GUID	 Reserved. 

	m_nid.dwInfoFlags      = NIIF_INFO; // add an icon to a balloon ToolTip
			// Flag Description 
			// - NIIF_ERROR     An error icon. 
			// - NIIF_INFO      An information icon. 
			// - NIIF_NONE      No icon. 
			// - NIIF_WARNING   A warning icon. 
			// - NIIF_ICON_MASK Version 6.0. Reserved. 
			// - NIIF_NOSOUND   Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips 

	m_nid.uCallbackMessage = uCallbackMessage;  
	//m_nid.uTimeout         = uTimeout * 1000;
	/*if (IsWin7OrLater())
		m_nid.hBalloonIcon  = icon;
	else
		m_nid.hIcon  = icon;
	m_nid.hIcon		   	  = icon;

	_tcscpy_s( m_nid.szInfoTitle, sInfoTitle );
	_tcscpy_s( m_nid.szInfo,      sInfo      );
	_tcscpy_s( m_nid.szTip,       sTip       );

	Shell_NotifyIcon( NIM_ADD, &m_nid ); // add to the taskbar's status area
}*/

void CPingGUin_MainDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == SC_MINIMIZE)
		// Hide app and activate tray
		HideApp();
	else
		CDialogEx::OnSysCommand(nID, lParam);
}

void CPingGUin_MainDlg::HideApp()
{
	/*// Set tray notification window:
	CString strToolTip = _T("PingGUIn Notifier");
	m_nid.hWnd = GetSafeHwnd ();
	m_nid.uCallbackMessage = UM_TRAYNOTIFY;
	// Initialize NOTIFYICONDATA
	memset(&m_nid, 0 , sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;


	m_nid.dwInfoFlags      = NIIF_INFO; // add an icon to a balloon ToolTip
	// Set tray icon and tooltip:
	m_nid.hIcon = m_hIcon;
	_tcsncpy_s (m_nid.szTip, strToolTip, strToolTip.GetLength ());
	Shell_NotifyIcon (NIM_ADD, &m_nid);

	/*LoadToTray( this, 
		    UM_TRAYNOTIFY, // user defined 
			_T("Easy App - (title)"),    
			_T("string 1 - body text"),  
			_T("Easy App - (tool tip)"),
			15, //sec
			AfxGetApp()->LoadIcon(IDR_MAINFRAME));	*/
	theApp.HideApplication();
	if (!MyTaskBarAddIcon(this->GetSafeHwnd(), ID_MINTRAYICON, UM_TRAYNOTIFY, m_hIcon, _T("PingGUIn Net Notifier")))
		MessageBox(_T("Could not create taskbar icon!"));
	//this->ShowWindow(SW_HIDE);
	_bVisible = FALSE;
}

LRESULT CPingGUin_MainDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uiMsg = (UINT) lParam;

	switch(uiMsg)
	{
	case WM_RBUTTONUP: //on double-click the left mouse button restore the dialog
		OnTrayContextMenu();
		return 1;
	case WM_LBUTTONDBLCLK: //on double-click the left mouse button restore the dialog
		//m_nid.hIcon = NULL;
		//Shell_NotifyIcon (NIM_DELETE, &m_nid);
		_bVisible = TRUE;
		this->ShowWindow( SW_RESTORE );
		if (!SATaskBarDelIcon(this->GetSafeHwnd(), ID_MINTRAYICON))
			MessageBox(_T("Could not create taskbar icon!"));

		//this->ShowWindow( SW_SHOW );
		return 1;
	}
	return 0;
}

void CPingGUin_MainDlg::OnTrayContextMenu ()
{
	CPoint point;
	::GetCursorPos (&point);

	CMenu menu;
	menu.LoadMenu (IDR_SYSMENU);
	ASSERT(menu);
	
	CMFCPopupMenu::SetForceShadow (TRUE);

	HMENU hMenu = menu.GetSubMenu (0)->Detach ();
	CMFCPopupMenu* pMenu = theApp.GetContextMenuManager()->ShowPopupMenu(hMenu, point.x, point.y, this, TRUE);
	pMenu->SetForegroundWindow ();
}


/*BOOL CPingGUin_MainDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_MOUSEMOVE )
	{
		_ToolTipCtrl.RelayEvent( pMsg ); // pass the mouse message to the ToolTip control for processing
	}

	// NO ESCAPE or RETURN key
    if (pMsg->message == WM_KEYDOWN)
    {
		if( pMsg->wParam == VK_RETURN ||
			pMsg->wParam == VK_ESCAPE )
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
		
			return TRUE;				// DO NOT process further
		}
	}
	
	// NO ALT+ key
	if (pMsg->message == WM_SYSCOMMAND)
	{
		return TRUE;
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}*/

/*// (MSDN) The framework calls this member function when the size, position, 
// or Z-order is about to change as a result of a call to theSetWindowPos member
// function or another window-management function
void CPingGUin_MainDlg::OnWindowPosChanging( WINDOWPOS FAR* lpwndpos ) 
{
	if( !_bVisible ) //do this only once to hide the dialog on start
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}

	CDialogEx::OnWindowPosChanging( lpwndpos );
}*/

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPingGUin_MainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CPingGUin_MainDlg::InitVars(NetInfoDialog *obj) {
	mainIP = obj->m_GWIP;
	DNS[0] = obj->m_PRIDNS;
	DNS[1] = obj->m_SecDNS;
	IsSingleHost = obj->m_IsSingleHost;
	MaxPingReqs = obj->m_MaxPingReqs;
}

void CPingGUin_MainDlg::OnTimer(UINT_PTR nIDEvent) {
	CString temp;

	// Update static target IP
	//if (noReq==0)
		//SetDlgItemText(IDC_TARGETIP, mainIP);

	// if requests reached max
	if (noReq == 0) {
		// Set progress bar to initial position
		ProgressPingReq->SetPos(1);
		switch (RunStage) {
			// For each case change the text and IP address
		case 1:
			PingIP = &DNS[0];
			ReportStaticDefaultText = _T("Primary DNS Server");
			SetDlgItemText(IDC_TARGETTYPE, ReportStaticDefaultText);
			SetDlgItemText(IDC_TARGETIP, *PingIP);
			CurReportStatic = (CStatic *) GetDlgItem(IDC_REPORT_PDNS);
			break;
		case 2:
			ReportStaticDefaultText = _T("Secondary DNS Server");
			SetDlgItemText(IDC_TARGETTYPE, ReportStaticDefaultText);
			PingIP = &DNS[1];
			SetDlgItemText(IDC_TARGETIP, *PingIP);
			CurReportStatic = (CStatic *) GetDlgItem(IDC_REPORT_SDNS);
			break;
		default:
			break;
		}
	}

	// increment Request Number
	noReq++;
	// Progress the progress control

	// Update number of request static box
	temp.Format(_T("%d"), noReq);
	SetDlgItemText(IDC_REQ, temp);

	// received reply
	if (PingHost(*PingIP) == 1) {
	// increment Reply number
		noReplies++;
		// Update number of reply static box
		temp.Format(_T("%d"), noReplies);
		SetDlgItemText(IDC_NOREPLY, temp);

		/*switch (RunStage) {
			// For each case chane the text and IP address
		case 0:
			SetDlgItemText(IDC_REPORT_DG, temp);
			break;
		case 1:
			temp.Format(_T("Primary DNS replied %d%% of requests"), PercentageOfReplies);
			SetDlgItemText(IDC_REPORT_PDNS, temp);
			break;
		case 2:
			temp.Format(_T("Secondary DNS replied %d%% of requests"), PercentageOfReplies);
			SetDlgItemText(IDC_REPORT_SDNS, temp);
			break;
		default:
			break;
		}*/
	}
	int PercentageOfReplies = noReplies * 100 / noReq;
	temp.Format(_T(" replied %d%% requests"), PercentageOfReplies);
	CurReportStatic->SetWindowText(ReportStaticDefaultText+temp);

	// if requests reached max
	if (noReq == MaxPingReqs) {
		noReq = 0;
		noReplies = 0;
		RunStage++;
		if (RunStage >= 3) {
			KillTimer(m_nWindowTimer);
			SetDlgItemText(IDC_STATUSBAR, _T("                                      You may close the window now."));
			if (SAShowBalloonTip(theApp.m_hInstance, this->GetSafeHwnd(), ID_MINTRAYICON, _T("SAOSX PingGUIn"), _T("You may close the window now"), IDR_MAINFRAME) == FALSE)
				//MessageBox(NULL, _T("Error on Notification!"), _T("SA CDEject App"), NULL);
				MessageBox(_T("Error on taskbar notification!"));
		}
	}
	else
		ProgressPingReq->StepIt();

	CDialogEx::OnTimer(nIDEvent);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPingGUin_MainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
