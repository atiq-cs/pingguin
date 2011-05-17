// NetInfoDialog.cpp : implementation file
//
#include "stdafx.h"
#include "PingGUIN.h"
#include "ProvideNetInfoDialog.h"
#include "CPingGUin_MainDlg.h"
#include <afxwin.h>
#include <afxcmn.h>
// For COM
#define _WIN32_DCOM
#include <afxdisp.h>			// for AfxThrowOleDispatchException
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NetInfoDialog dialog

BEGIN_MESSAGE_MAP(NetInfoDialog, CDialog)
	//{{AFX_MSG_MAP(NetInfoDialog)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_CHECKISP, &NetInfoDialog::OnBnClickedSingleHostCheck)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

NetInfoDialog::NetInfoDialog() : CDialog(NetInfoDialog::IDD) , m_MaxPingReqs(10), m_IsSingleHost(TRUE), m_IsNotifyOn(FALSE)
{
	//{{AFX_DATA_INIT(NetInfoDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CString NetInfoDialog::GetWMINETINFO() {
	CString ErrorMsg;
	HRESULT hres;
	LONG cElements, lLBound, lUBound;

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
        CLSID_WbemAdministrativeLocator, // changed
        0, 
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, // changed from CLSCTX_INPROC_SERVER, 
        IID_IUnknown, // changed from IID_IWbemLocator,
		(LPVOID *) &pLoc);
 
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
        bstr_t("SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = 'TRUE'"),	//bstr_t("SELECT * FROM Win32_NetworkAdapterConfiguration"),
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

        if(0 == uReturn) break;

        VARIANT vtProp;
		BSTR* pbstr = NULL;
		hr = pclsObj->Get(L"IPEnabled", 0, &vtProp, 0, 0);
		if(vtProp.boolVal) {
			SAFEARRAY *psa;
			// Get the value of the DefaultIPGateway property
			hr = pclsObj->Get(L"DefaultIPGateway", 0, &vtProp, 0, 0);

			if (vtProp.vt == (VT_ARRAY|VT_BSTR)) {
				psa = vtProp.parray;
				// Get the lower and upper bound
				hr = SafeArrayGetLBound( psa, 1, &lLBound );
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed to get LBound"), hres);
					return ErrorMsg;
				}

				hr = SafeArrayGetUBound( psa, 1, &lUBound );
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed to get UBound"), hres);
					return ErrorMsg;
				}

				hr = SafeArrayAccessData(psa, (void HUGEP**)&pbstr);
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed to access buffer for Default GW"), hres);
					return ErrorMsg;
				}

				cElements = lUBound-lLBound+1;
				if (cElements > 0)
					m_GWIP = pbstr[0];
				/*for (int i = 0; i < cElements; i++)
				{
					CString tmp(pbstr[i]);
					AfxMessageBox(tmp);
				}*/
				hr = SafeArrayUnaccessData(psa); 
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed SafeArrayUnaccessData"), hres);
					return ErrorMsg;
				}
			}
			
			// Get the value of the DNSServerSearchOrder property
			hr = pclsObj->Get(L"DNSServerSearchOrder", 0, &vtProp, 0, 0);

			if (vtProp.vt == (VT_ARRAY|VT_BSTR)) {
				psa = vtProp.parray;
				// 1st Index
				// Get the lower and upper bound
				hr = SafeArrayGetLBound( psa, 1, &lLBound );
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed to get LBound"), hres);
					return ErrorMsg;
				}

				hr = SafeArrayGetUBound( psa, 1, &lUBound );
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed to get UBound"), hres);
					return ErrorMsg;
				}

				hr = SafeArrayAccessData(psa, (void HUGEP**)&pbstr);
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed to access buffer for DNS Servers"), hres);
					return ErrorMsg;
				}
				//char *ch; 
				// ch = _com_util::ConvertBSTRToString(pbstr[0]);
				cElements = lUBound-lLBound+1;
				if (cElements > 0)
					m_PRIDNS = pbstr[0];
				if (cElements > 1)
					m_SecDNS = pbstr[1];

				/*for (int i = 0; i < cElements; i++)
				{
					CString tmp(pbstr[i]);
					AfxMessageBox(tmp);
				}*/
			 	/*CString str = ch;
				m_PRIDNS = str;
				AfxMessageBox(_T("Primary DNS Server: ") + m_PRIDNS);
				ch = _com_util::ConvertBSTRToString(pbstr[1]);
			 	str = ch;
				m_SecDNS = str;
				AfxMessageBox(_T("Secondary DNS Server: ") + m_SecDNS);
				ErrorMsg = _T("");
				//for(idx=lLBound; idx <= lUBound; idx++)
				//	cout << "GATEWAY:" << pbstr[idx] << endl;*/
				hr = SafeArrayUnaccessData(psa); 
				if(FAILED(hr)) {
					ErrorMsg.Format(_T("Failed SafeArrayUnaccessData"), hres);
					return ErrorMsg;
				}
			}

			// break here if GWIP is already found
			if (m_GWIP != "")
				break;
		}
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

UINT NetInfoDialog::DNSThreadProc( LPVOID pParam )
{
    /*CMyObject* pObject = (CMyObject*)pParam;

    if (pObject == NULL ||
        !pObject->IsKindOf(RUNTIME_CLASS(CMyObject)))
    return 1;   // if pObject is not valid*/

    // do something with 'pObject'
	THREADSTRUCT*	ts = (THREADSTRUCT*) pParam;
	/*for (int i = 0; i<10; i++) {
		Sleep(500);
		ts->_this->MessageBox(_T("Got gateway ip: ") + ts->_this->m_GWIP);
	}*/

	//-----------------------------------------
    // Declare and initialize variables
	if (DNSLookUpPossible("www.google.com")) {
		if (::IsWindow(ts->_this->m_hWnd) == TRUE) {
		//if (ts->_this->IsWindowClosed == FALSE) {
			//AfxMessageBox(_T("Window is active, dns update!!"));
			ts->_this->AdjustNetControls(true, _T("Notice: Internet is available"));
		}
		//AfxMessageBox(_T("DNS Lookup successful!"));
	}
	else {
		if (::IsWindow(ts->_this->m_hWnd) == TRUE)
			ts->_this->AdjustNetControls(false, _T("Notice: Internet is not available"));
	}
	//AfxMessageBox(_T("Thread terminating.."));

    return 0;   // thread completed successfully
}


BOOL NetInfoDialog::OnInitDialog() {
	//CEdit *CGateway, *CPriDNS, *CSecDNS;
	CDialog::OnInitDialog();

	CIPAddressCtrl *CGateway;
	THREADSTRUCT *_param = new THREADSTRUCT;
	_param->_this = this;

	AfxBeginThread (NetInfoDialog::DNSThreadProc, _param);
	CGateway = (CIPAddressCtrl *) GetDlgItem(IDC_IPADDRESS1);

	// Get Net Info for setting in ipaddress control
	CString ErrorMsg = GetWMINETINFO();

	if (!ErrorMsg.IsEmpty())
	MessageBox(_T("In Dialog Error: ") + ErrorMsg);

	/*MessageBox(_T("Before calling function"));
	MessageBox(_T("After function"));*/

	if (m_GWIP == "")
		m_GWIP = _T("0.0.0.0");
	if (m_PRIDNS == "")
		m_PRIDNS = _T("8.8.8.8");
	if (m_SecDNS == "")
		m_SecDNS = _T("4.2.2.2");

	SetDlgItemText(IDC_NOTIFYBOX, _T("Checking internet availibility.."));
	SetDlgItemText(IDC_IPADDRESS1, m_GWIP);
	SetDlgItemText(IDC_IPADDRESS2, m_PRIDNS);
	SetDlgItemText(IDC_IPADDRESS3, m_SecDNS);
	/*if (ErrorMsg != _T(""))
		MessageBox(ErrorMsg);
	MessageBox(m_GWIP);
	SetDlgItemText(IDC_IPADDRESS1, _T("Enter default gateway here"));
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

	//PostMessage(WM_NEXTDLGCTL, (WPARAM) CGateway->GetSafeHwnd(), TRUE);

	GetDlgItem(IDC_IPADDRESS2)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS3)->EnableWindow(FALSE);


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
	DDX_Check(pDX, IDC_CHECKNOTIFY, m_IsNotifyOn);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NOREQ, m_MaxPingReqs);
	DDV_MinMaxInt(pDX, m_MaxPingReqs, 1, 100);
}


/////////////////////////////////////////////////////////////////////////////
// NetInfoDialog message handlers
// Specifies what should happen user click checkbox "ping single host"
void NetInfoDialog::OnBnClickedSingleHostCheck() {
	// true means toggle true
	if (m_IsSingleHost == TRUE)
		m_IsSingleHost = FALSE;
	else
		m_IsSingleHost = TRUE;

	AdjustNetControls(m_IsSingleHost, _T(""));
}

/*void NetInfoDialog::OnClose() {
	MessageBox(_T("Dialog closed."));

	CDialog::OnClose();
}*/

void NetInfoDialog::AdjustNetControls(BOOL IsSingleHost, CString Msg) {
	if (IsSingleHost == TRUE) {
		SetDlgItemText(IDC_STATIC2, _T("Target host &IP"));
		SetDlgItemText(IDC_CHECKNOTIFY, _T("Notify when &target comes online"));
		GetDlgItem(IDC_IPADDRESS2)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS3)->EnableWindow(FALSE);
	}
	else {
		SetDlgItemText(IDC_STATIC2, _T("Default Gateway &IP"));
		SetDlgItemText(IDC_CHECKNOTIFY, _T("Notify when in&ternet is available"));
		GetDlgItem(IDC_IPADDRESS2)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS3)->EnableWindow(TRUE);
	}
	if (Msg != _T(""))
		SetDlgItemText(IDC_NOTIFYBOX, Msg);
}

BOOL DNSLookUpPossible(const char* host_name) {
    WSADATA wsaData;
    int iResult;

    DWORD dwError;
    int i = 0;

    struct hostent *remoteHost;
    struct in_addr addr;

    //char **pAlias;
	//CString MsgTemp;

	// Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        //AfxMessageBox(_T("WSAStartup failed: ") + iResult);
        return false;
    }

	/*CString temp = host_name;
	MsgTemp.Format(_T("Calling gethostbyname with %s"), temp);
    AfxMessageBox(MsgTemp);*/
    remoteHost = gethostbyname(host_name);
    
    if (remoteHost == NULL) {
        dwError = WSAGetLastError();
        if (dwError != 0) {
            if (dwError == WSAHOST_NOT_FOUND) {
                //AfxMessageBox(_T("Host not found"));
                return false;
            } else if (dwError == WSANO_DATA) {
                //AfxMessageBox(_T("No data record found"));
                return false;
            } else {
				// dwError is the error
				//AfxMessageBox(_T("Function failed with error: "));
                return false;
            }
        }
    } else {
        /*AfxMessageBox(_T("Function returned:\n"));
        AfxMessageBox(_T("\tOfficial name: ")+ CString(remoteHost->h_name));
        for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
			//temp = *pAlias;
			MsgTemp.Format(_T("\tAlternate name #%d: %s"), ++i, CString(*pAlias));
            AfxMessageBox(MsgTemp);
        }
        AfxMessageBox(_T("\tAddress type: "));
        switch (remoteHost->h_addrtype) {
        case AF_INET:
            AfxMessageBox(_T("AF_INET"));
            break;
        case AF_NETBIOS:
            AfxMessageBox(_T("AF_NETBIOS"));
            break;
        default:
			MsgTemp.Format(_T(" %d"), remoteHost->h_addrtype);
            AfxMessageBox(MsgTemp);
            break;
        }
		MsgTemp.Format(_T("\tAddress length: %d"), remoteHost->h_length);
        AfxMessageBox(MsgTemp);*/

        i = 0;
        if (remoteHost->h_addrtype == AF_INET)
        {
            while (remoteHost->h_addr_list[i] != 0) {
                addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
				//temp = ;
				//MsgTemp.Format(_T("\tIP Address #%d: %s"), i, CString(inet_ntoa(addr)));
				if (inet_ntoa(addr) != 0)
					return true;
				//AfxMessageBox(MsgTemp);
            }
        }
        /*else if (remoteHost->h_addrtype == AF_NETBIOS)
        {   
            AfxMessageBox(_T("NETBIOS address was returned"));
			return false;
        }*/
    }
	return false;
}
