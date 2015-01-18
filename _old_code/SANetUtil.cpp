/*	Author	:	Saint Atique
 *	Date	:	May 03, 2011
 *	Desc	:	Network Utility Functions
				It 
 */

#pragma once
#include "stdafx.h"
#include "SANetUtil.h"

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

/*	Pings target host
 *	Returns 0 on error
 *			1 when reply is received
 *			2 when reply no ..
 *			3 when given ip address is wrong
 *			4 when icmp handle error
 *			5 when buffer allocation error
 *			6 when request timed out
 *			7 when some other error occurred
 */
int PingHost(CString ipaddrstr) {
	HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;
	CStringA ipaddrstrA(ipaddrstr);

	// Convert Ip address to ip addr
	ipaddr = inet_addr(ipaddrstrA);

    if (ipaddr == INADDR_NONE)
		return 3;
 
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
		return 4;

    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID*) malloc(ReplySize);

    if (ReplyBuffer == NULL)
		return 5;

    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
        NULL, ReplyBuffer, ReplySize, 1500);

    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;

		if (pEchoReply->Status == 11003)
			return 6;
		else
			return 1;

        // _stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Roundtrip time = %ld milliseconds\r\n"), pEchoReply->RoundTripTime);
    }
    else {
		/*
		int errorno = GetLastError();
		_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("lost in the network\r\n"));
		switch (errorno) {
			case 11010:
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Timed out due to possible unreachability\r\n"),  errno);
				pMainWnd->CountResponse++;
				break;
			default:
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("IcmpSendEcho returned error: %ld\r\n"),  errorno);
		}
		if (IsNotifyOn == FALSE) {
			if (pMainWnd->CountResponse >= 48) {
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Target client is possibly down.\r\n\r\nClick close button to quit the program."));
				pMainWnd->PingQuit(pstr);
				return ;
			}
			else {
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\n\r\nAnalyzing target host's unavailability.\r\n"));
				_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("Absence of reply count %d.\r\n"), pMainWnd->CountResponse);
				pMainWnd->PostMessage(WM_PAINT, (LPARAM)0, (LPARAM)0);
				return ;
			}
		}
		else
			_stprintf_s(&pstr[_tcslen(pstr)], MSGSIZE, _T("\r\nServer is still dumb."));*/
		return 7;
    }

	//pMainWnd->PostMessage(WM_PAINT, (LPARAM)0, (LPARAM)0);
}