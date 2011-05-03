
// CPingGUin_MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CPingGUin_MainDlg.h"
#include "afxdialogex.h"
#include "SANetUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPingGUin_MainDlg dialog


CPingGUin_MainDlg::CPingGUin_MainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPingGUin_MainDlg::IDD, pParent)
	, noReq(0)
	, noReplies(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPingGUin_MainDlg::~CPingGUin_MainDlg() {
	KillTimer(m_nWindowTimer);
}

void CPingGUin_MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_REQ, noReq);
	//DDX_Text(pDX, IDC_Reply, noReplies);
}

BEGIN_MESSAGE_MAP(CPingGUin_MainDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CPingGUin_MainDlg message handlers

BOOL CPingGUin_MainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_nWindowTimer = SetTimer(1, 2000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

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
}

void CPingGUin_MainDlg::OnTimer(UINT_PTR nIDEvent) {
	// increment Request Number
	noReq++;
	CString temp;
	temp.Format(_T("%d"), noReq);
	SetDlgItemText(IDC_REQ, temp);
	// received reply
	if (PingHost(mainIP) == 1) {
	// increment Reply number
		temp.Format(_T("%d"), noReplies);
		SetDlgItemText(IDC_REPLY, temp);
		noReplies++;
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPingGUin_MainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
