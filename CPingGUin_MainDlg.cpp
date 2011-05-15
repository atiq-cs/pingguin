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
	, IsSingleHost(false)
	, MaxPingReqs(0)
	, RunStage(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPingGUin_MainDlg::~CPingGUin_MainDlg() {
}

void CPingGUin_MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_PINGPROGRESS, ProgressPingReq);
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
		SetDlgItemText(IDC_TARGETTYPE, _T("Single Host"));
		RunStage = 3;
	}
	else
		SetDlgItemText(IDC_TARGETTYPE, _T("Default Gateway"));

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
		ProgressPingReq->SetPos(1);
		switch (RunStage) {
		case 1:
			PingIP = &DNS[0];
			SetDlgItemText(IDC_TARGETTYPE, _T("Primary DNS Server"));
			SetDlgItemText(IDC_TARGETIP, *PingIP);
			break;
		case 2:
			SetDlgItemText(IDC_TARGETTYPE, _T("Secondary DNS Server"));
			PingIP = &DNS[1];
			SetDlgItemText(IDC_TARGETIP, *PingIP);
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
	}

	// if requests reached max
	if (noReq == MaxPingReqs) {
		noReq = 0;
		noReplies = 0;
		RunStage++;
		if (RunStage >= 3)
			KillTimer(m_nWindowTimer);
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
