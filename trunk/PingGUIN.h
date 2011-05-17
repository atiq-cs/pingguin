
// PingGUin.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTrayMenuApp:
// See TrayMenu.cpp for the implementation of this class
//

class CCustomCommandLineInfo : public CCommandLineInfo {
public:
	CCustomCommandLineInfo() {}
	INT m_nError;
	//This function will retrieve the value of the option provided	
	BOOL GetOption (LPCTSTR option, CString& val);

	//This function will check for the commandline without ny options
	BOOL GetOption (LPCTSTR option);

protected:
	CMapStringToString m_options; // hash of options
	CString m_sLastOption; // last option encountered

	//Parse param overrided
	virtual void ParseParam (const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
};

class CSAApp: public CWinAppEx {
public:
	virtual BOOL InitInstance();
};

extern CSAApp theApp;