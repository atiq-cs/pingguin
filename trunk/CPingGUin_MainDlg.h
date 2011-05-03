
// CPingGUin_MainDlg.h : header file
//

#pragma once
#include "ProvideNetInfoDialog.h"

// CPingGUin_MainDlg dialog
class CPingGUin_MainDlg : public CDialogEx
{
// Construction
public:
	CPingGUin_MainDlg(CWnd* pParent = NULL);	// standard constructor
	~CPingGUin_MainDlg();
	void InitVars(NetInfoDialog *obj);

// Dialog Data
	enum { IDD = IDD_PINGGUIN_DIALOGBASED_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
	HICON m_hIcon;

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
	UINT_PTR m_nWindowTimer;
};
