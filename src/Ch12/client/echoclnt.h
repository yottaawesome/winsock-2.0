//
// ECHOCLNT.H	Datagram ECHO Client
//
//

#ifndef _ECHOCLNT_H
#define _ECHOCLNT_H

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <wsipx.h>
#include <svcguid.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "resource.h"

////////////////////////////////////////////////////////////

#define UM_ASYNC WM_USER+1

#define ECHODATASIZE	50

typedef struct tagECHOREQ
{
	DWORD	dwSentTime;
	DWORD	dwSeq;
	int		nData[ECHODATASIZE];
}ECHOREQ, *LPECHOREQ;

//
// ECHOCLNT.C
//
BOOL OnInitDialog(HWND hwnd, 
				  HWND hwndFocus, 
				  LPARAM lParam); 

void OnCommand(HWND hwnd, 
			   int nId, 
			   HWND hwndCtl, 
			   UINT codeNotify); 

BOOL CALLBACK MainWndProc(HWND hwnd, 
						  UINT uMsg, 
						  WPARAM wParam, 
				          LPARAM lParam);
void ShowWinSockError(HWND hwnd, LPCSTR lpText, int nErrorCode);
void HandleAsyncMsg(HWND hwnd, WPARAM wParam, LPARAM lParam);
void FillSocketChoices(HWND hwnd);
void SendEcho(HWND hwnd);
void RecvEcho(HWND hwnd, SOCKET socket, int nErrorCode);
void FillEchoRequest(LPECHOREQ pechoReq);
BOOL FindEchoServer(SOCKET socket, 
					LPWSAPROTOCOL_INFO pInfo,
					LPSOCKADDR pAddr, 
					int nAddrLen,
					LPSTR lpHostName);
BOOL FillIpx(LPSOCKADDR_IPX pAddr, 
			 int nAddrLen,
			 LPTSTR lpszAddress);



////////////////////////////////////////////////////////////

//
// Message Handler macro for dialog boxes
//
#define HANDLE_DLG_MSG(hwnd, message, fn)				\
		case(message):									\
			return (BOOL) HANDLE_##message((hwnd),		\
										   (wParam),	\
										   (lParam),	\
										   (fn))	
#endif _ECHOCLNT_H


