//
// ECHOSERV.H	Datagram ECHO Server
//
//

#ifndef _ECHOSERV_H
#define _ECHOSERV_H

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <nspapi.h>
#include <wsipx.h>
#include <windowsx.h>
#include <tchar.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "..\aeguid.h"
#include "..\dwinsock.h"

#include "resource.h"

////////////////////////////////////////////////////////////

#define UM_ASYNC WM_USER+1
#define UM_ECHO WM_USER+2

#define	ECHO_STATS_MSG 1
#define ECHO_EVENT_MSG 2

//
// Structure for linked list
//
typedef struct tagSOCKETENTRY
{
  SOCKET			Socket;
  LPSOCKADDR		lpSockAddr;
  WSAPROTOCOL_INFOA	Info;
  struct			tagSOCKETENTRY *lpNext;
}SOCKETENTRY, *LPSOCKETENTRY;


typedef struct tagSTATS
{
	DWORD	dwDatagrams;
	DWORD	dwRecv;
	DWORD	dwSend;
	LPWSAPROTOCOL_INFO pInfo;
}STATS, *LPSTATS;

//
// ECHOUI.C
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

void UpdateStats(HWND hwnd, LPSTATS lpStats);
void ShowEvent(HWND hwnd, LPCTSTR lpStr);

//
// ECHOSERV.c
//
void HandleAsyncMsg(HWND hwnd, WPARAM wParam, LPARAM lParam);
BOOL StartServer(HWND hwnd, UINT uAsyMsg, UINT uAppMsg);
void StopServer(void);
void LogEvent(HWND hwnd, LPCTSTR lpFormat, ...);
void LogWinSockError(HWND hwnd, LPCTSTR lpText, int nErrorCode);
void InstallClass(void);
void SetEchoService(WSAESETSERVICEOP essOp);
BOOL OpenListeners(HWND hwnd, UINT uAsyMsg);
void CloseListeners(void);
void RecvAndEcho(SOCKET socket, int nErrorCode);
BOOL FillLocalAddress(SOCKET socket,
					  LPSOCKADDR lpAddr, 
					  LPWSAPROTOCOL_INFO lpInfo);

//
// SOCKLIST.C
//
LPSOCKETENTRY	AddSocketEntry(SOCKET Socket, 
							   LPWSAPROTOCOL_INFO lpInfo);
LPSOCKETENTRY GetSocketEntry(SOCKET Socket);
void DelSocketEntry(LPSOCKETENTRY lpThis);
LPSOCKETENTRY	GetFirstSocketEntry(void);
LPSOCKETENTRY	GetNextSocketEntry(LPSOCKETENTRY lpThis);
void DelAllSocketEntries(void);



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
#endif _ECHOSERV_H


