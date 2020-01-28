//
// HTTPA.H		HTTP Server Framework
//
//				Single Thread Model
//

#include <winsock.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef _HTTPA_H
#define _HTTPA_H

#define HTTPPORT 80
#define METHOD_GET 0

//
// Notification Messages
//
#define HTTP_STATS_MSG			WM_USER+1
#define HTTP_FILEOK_MSG			WM_USER+2
#define HTTP_FILENOTFOUND_MSG	WM_USER+3
#define HTTP_EVENT_MSG			WM_USER+4


//
// HTTPSERVINFO structure
//
typedef struct tagHTTPSERVINFO
{
	HWND		hwnd;
	UINT		uMsgAsy;
	UINT		uMsgApp;
	short		nPort;
	LPCSTR		lpRootDir;
}HTTPSERVINFO, *LPHTTPSERVINFO;

//
// CONNECTION structure
//
typedef struct tagREQUEST
{
  SOCKET		Socket;
  LPSOCKADDR	lpSockAddr;
  int			nAddrLen;
  int			nMethod;
  BOOL			fFullResponse;
  DWORD			dwConnectTime;
  DWORD			dwRecv;
  DWORD			dwSend;
  HFILE			hFile;
  DWORD			dwFilePtr;
  struct		tagREQUEST *lpNext;
}REQUEST, *LPREQUEST;

//
// Statistics structure
//
typedef struct tagHTTPSTATS
{
	DWORD	dwElapsedTime;
	DWORD	dwRecv;
	DWORD	dwSend;
}HTTPSTATS, *LPHTTPSTATS;

//
// These numbers relate to their
// sequence in an array in
// SendError()
//
#define HTTP_STATUS_OK				0
#define HTTP_STATUS_CREATED			1
#define HTTP_STATUS_ACCEPTED		2
#define HTTP_STATUS_NOCONTENT		3
#define HTTP_STATUS_MOVEDPERM		4
#define HTTP_STATUS_MOVEDTEMP		5
#define HTTP_STATUS_NOTMODIFIED		6
#define HTTP_STATUS_BADREQUEST		7
#define HTTP_STATUS_UNAUTHORIZED	8
#define HTTP_STATUS_FORBIDDEN		9
#define HTTP_STATUS_NOTFOUND		10
#define HTTP_STATUS_SERVERERROR		11
#define HTTP_STATUS_NOTIMPLEMENTED	12
#define HTTP_STATUS_BADGATEWAY		13
#define HTTP_STATUS_UNAVAILABLE		14


//
// HTTPSERV.C
//
BOOL StartHTTP(LPHTTPSERVINFO lpInfo);
void StopHTTP(void);
void HandleAsyncMsg(HWND hwnd, WPARAM wParam, 
				    LPARAM lParam);
void OnAccept(HWND hwnd, SOCKET socket, int nErrorCode);
void OnRead(SOCKET socket, int nErrorCode);
void OnWrite(SOCKET socket, int nErrorCode);
void OnClose(SOCKET socket, int nErrorCode);
void ParseRequest(LPREQUEST lpReq, LPBYTE lpBuf);
void SendFile(LPREQUEST lpReq, LPCSTR lpFileName);
void CloseConnection(LPREQUEST lpReq);
void SendError(LPREQUEST lpReq, UINT uError);
void SendFileContents(LPREQUEST lpReq);
void LogWinSockError(HWND hwnd, LPCSTR lpText, int nErrorCode);
void LogEvent(HWND hwn, LPCSTR lpFormat, ...);
int GetLocalAddress(LPSTR lpStr, LPDWORD lpdwStrLen);

//
// REQLIST.C
//
LPREQUEST	AddRequest(SOCKET Socket, 
					   LPSOCKADDR lpSockAddr,
					   int nAddrLen);
LPREQUEST	GetRequest(SOCKET Socket);
LPREQUEST	GetFirstRequest(void);
LPREQUEST	GetNextRequest(LPREQUEST lpThis);
void		DelRequest(LPREQUEST lpThis);
void		DelAllRequests(void);


#endif _HTTPA_H

