// CPingGUin_MainDlg.h : header file
//

#pragma once

//UINT WorkerThreadDNSLookUpProc( LPVOID pParam );

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
	//friend UINT WorkerThreadDNSLookUpProc( LPVOID pParam );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	HICON m_hIcon;
	// Handle for progress control required to set range and stepit
	CProgressCtrl* ProgressPingReq;

	// added for system tray
	//CToolTipCtrl   _ToolTipCtrl;
	//NOTIFYICONDATA m_nid;
	BOOL m_DlgVisible;

	/*BOOL SetIconAndTitleForBalloonTip( CToolTipCtrl *pToolTipCtrl,
		                               int          tti_ICON, 
									   CString      title );

	void LoadToTray( CWnd*	 pWnd, 
		             UINT	 uCallbackMessage,
					 CString sInfoTitle,
					 CString sInfo,
					 CString sTip,
			         int     uTimeout,
					 HICON	 icon );*/
	// sys tray end
	void HideApp();
	void OnTrayContextMenu ();

    // Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnClose( );
	afx_msg void OnPaint();
	afx_msg void OnAppRestore();
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// sys tray start
	//afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg LRESULT OnTrayNotify ( WPARAM wParam, LPARAM lParam ) ;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	// Number of ping requests sent
	int noReq;
	// Number of ping requests replied
	int noReplies;
	CString mainIP;
	CString DNS[2];
	CString *PingIP;
	UINT_PTR m_PingTimer;
	UINT_PTR m_DNSLookUpTimer;
	// Whether to ping a single target or ISP target chain
	BOOL IsSingleHost;
	BOOL IsNotifyEnabled;
	// Maximum number of requests to send set by user
	int MaxPingReqs;
	// which stage the app is running on
	BYTE RunStage;
	// Currently selected static box of the report group box
	CStatic *CurReportStatic;
	// 
	CString ReportStaticDefaultText;
};
