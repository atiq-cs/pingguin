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
	m_IsSingleHost = FALSE;
	m_IsNotifyOn = FALSE;
	//}}AFX_DATA_INIT
}

CString NetInfoDialog::GetWMINETINFO() {
	CString ErrorMsg;
	HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
		ErrorMsg.Format(_T("Failed to initialize COM library. Error code = %d"), hres);
        return ErrorMsg;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres))
    {
		ErrorMsg.Format(_T("Failed to initialize security. Error code = %d"), hres);

        CoUninitialize();
        return ErrorMsg;                    // Program has failed.
    }
    
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
		ErrorMsg.Format(_T("Failed to create IWbemLocator object. Error code = %d"), hres);
        CoUninitialize();
        return ErrorMsg;                    // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
	
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres)) {
		ErrorMsg.Format(_T("Could not connect. Error code = %d"), hres);
        pLoc->Release();     
        CoUninitialize();
        return ErrorMsg;                    // Program has failed.
    }

	//MessageBox(_T("Connected to ROOT\\CIMV2 WMI namespace"));


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
		ErrorMsg.Format(_T("Could not set proxy blanket. Error code = %d"), hres);
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return ErrorMsg;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_OperatingSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
		ErrorMsg.Format(_T("Query for operating system name failed. Error code = %d"), hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return ErrorMsg;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property
        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		/*_bstr_t tmp(vtProp.bstrVal, FALSE);   //wrap the BSTR
		TCHAR *str = static_cast<TCHAR *>(tmp);

		m_GWIP = str;*/
		m_GWIP = vtProp.bstrVal;
		
        //wcout << " OS Name : " << vtProp.bstrVal << endl;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Cleanup
    // ========
    
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

	return ErrorMsg;
}

BOOL NetInfoDialog::OnInitDialog() {
	//CEdit *CGateway, *CPriDNS, *CSecDNS;
	CDialog::OnInitDialog();

	CIPAddressCtrl *CGateway;
	CGateway = (CIPAddressCtrl *) GetDlgItem(IDC_IPADDRESS1);

	// Get Net Info for setting in ipaddress control
	CString ErrorMsg = GetWMINETINFO();
	if (ErrorMsg != _T(""))
		MessageBox(ErrorMsg);
	MessageBox(m_GWIP);
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
	DDX_Check(pDX, IDC_CHECK1, m_IsNotifyOn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NetInfoDialog, CDialog)
	//{{AFX_MSG_MAP(NetInfoDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK2, &NetInfoDialog::OnBnClickedCheck2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NetInfoDialog message handlers
/*
	Specifies what should happen user click checkbox "ping single host"
*/
void NetInfoDialog::OnBnClickedCheck2() {
	if (m_IsSingleHost == TRUE) {
		SetDlgItemText(IDC_STATIC2, _T("Default Gateway IP"));
		GetDlgItem(IDC_IPADDRESS2)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS3)->EnableWindow(TRUE);
		m_IsSingleHost = FALSE;
	}
	else {
		SetDlgItemText(IDC_STATIC2, _T("Target host IP"));
		GetDlgItem(IDC_IPADDRESS2)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS3)->EnableWindow(FALSE);

		m_IsSingleHost = TRUE;
	}
}
