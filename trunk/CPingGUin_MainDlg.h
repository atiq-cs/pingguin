
// CPingGUin_MainDlg.h : header file
//

#pragma once
#include "ProvideNetInfoDialog.h"
#include "afxcmn.h"

// CPingGUin_MainDlg dialog
class CPingGUin_MainDlg : public CDialogEx
{
// Construction
public:
	CPingGUin_MainDlg(CWnd* pParent = NULL);	// standard constructor
	~CPingGUin_MainDlg();
	void InitVars(NetInfoDialog *obj);

// Dialog Data
	enum { IDD = IDD_MAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
	HICON m_hIcon;
	// Handle for progress control required to set range and stepit
	CProgressCtrl* ProgressPingReq;

public:
// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

private:
	// Number of ping requests sent
	int noReq;
	// Number of ping requests replied
	int noReplies;
	CString mainIP;
	CString DNS[2];
	CString *PingIP;
	UINT_PTR m_nWindowTimer;
	// Whether to ping a single target or ISP target chain
	BOOL IsSingleHost;
	// Maximum number of requests to send set by user
	int MaxPingReqs;
	// which stage the app is running on
	BYTE RunStage;
};
