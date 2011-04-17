// NetInfoDialog.cpp : implementation file
//

#include <afxwin.h>
#include <afxcmn.h>
#include "resource.h"
#include "ProvideNetInfoDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NetInfoDialog dialog

NetInfoDialog::NetInfoDialog() : CDialog(NetInfoDialog::IDD) {
	//{{AFX_DATA_INIT(NetInfoDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BOOL NetInfoDialog::OnInitDialog() {
	//CEdit *CGateway, *CPriDNS, *CSecDNS;
	CDialog::OnInitDialog();

	CIPAddressCtrl *CGateway;
	CGateway = (CIPAddressCtrl *) GetDlgItem(IDC_IPADDRESS1);

	/*SetDlgItemText(IDC_IPADDRESS1, _T("Enter default gateway here"));
	SetDlgItemText(IDC_IPADDRESS2, _T("Enter primary DNS here"));
	SetDlgItemText(IDC_IPADDRESS3, _T("Enter secondary DNS here"));
	CGateway = (CEdit *) GetDlgItem(IDC_IPADDRESS1);
	CPriDNS = (CEdit *) GetDlgItem(IDC_IPADDRESS2);
	CSecDNS = (CEdit *) GetDlgItem(IDC_IPADDRESS3);

	CGateway->SetWindowText(_T("Enter default gateway here"));
	CPriDNS->SetWindowText(_T("Enter primary DNS here"));
	CSecDNS->SetWindowText(_T("Enter secondary DNS here"));

	CGateway->SetSel(0, -1);
	//GotoDlgCtrl(CGateway);
	PostMessage(WM_NEXTDLGCTL, (WPARAM) CGateway->GetSafeHwnd(), TRUE);
	*/

	PostMessage(WM_NEXTDLGCTL, (WPARAM) CGateway->GetSafeHwnd(), TRUE);

	return TRUE;
}

/*	This function returns info depending on mode
 *
 */
void NetInfoDialog::GetNetInfo(CString* strArray) {
	strArray[0] = m_GWIP;
	strArray[1] = m_PRIDNS;
	strArray[2] = m_SecDNS;
}

void NetInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NetInfoDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_IPADDRESS1, m_GWIP);
	DDX_Text(pDX, IDC_IPADDRESS2, m_PRIDNS);
	DDX_Text(pDX, IDC_IPADDRESS3, m_SecDNS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NetInfoDialog, CDialog)
	//{{AFX_MSG_MAP(NetInfoDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NetInfoDialog message handlers
