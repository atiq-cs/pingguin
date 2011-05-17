// For COM
#define _WIN32_DCOM
#include <afxdisp.h>			// for AfxThrowOleDispatchException
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")


#if !defined(AFX_MYDIALOG_H__20B59A5E_FBE6_4A1C_A6B7_FDC199FE74EC__INCLUDED_)
#define AFX_MYDIALOG_H__20B59A5E_FBE6_4A1C_A6B7_FDC199FE74EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetInfoDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NetInfoDialog dialog

BOOL DNSLookUpPossible(const char* host_name);

class NetInfoDialog : public CDialog
{
// Construction
public:
	//NetInfoDialog(CWnd* pParent = NULL);   // standard constructor
	//NetInfoDialog::NetInfoDialog(UINT nIDTemplate);
	NetInfoDialog();
	void GetNetInfo(CString* strArray);
	BOOL IsNotifyOn() { return (m_IsNotifyOn != 0); }
	CString GetWMINETINFO();
	static UINT DNSThreadProc( LPVOID pParam );
	void AdjustNetControls(BOOL IsSingleHost, CString Msg);
	typedef struct THREADSTRUCT				//structure for passing to the controlling function
	{
		NetInfoDialog*	_this;
	} THREADSTRUCT;
	

// Dialog Data
	//{{AFX_DATA(NetInfoDialog)
	enum { IDD = NETINFO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(NetInfoDialog)
	afx_msg void OnBnClickedSingleHostCheck();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_GWIP;
	CString m_PRIDNS;
	CString m_SecDNS;

	BOOL m_IsNotifyOn;
	BOOL m_IsSingleHost;
	BOOL IsWindowClosed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDIALOG_H__20B59A5E_FBE6_4A1C_A6B7_FDC199FE74EC__INCLUDED_)