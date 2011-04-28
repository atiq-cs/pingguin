
// CPingGUin_MainDlg.h : header file
//

#pragma once


// CPingGUin_MainDlg dialog
class CPingGUin_MainDlg : public CDialogEx
{
// Construction
public:
	CPingGUin_MainDlg(CWnd* pParent = NULL);	// standard constructor
	void InitVars();

// Dialog Data
	enum { IDD = IDD_PINGGUIN_DIALOGBASED_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
