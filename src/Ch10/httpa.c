//
// HTTPA.C	HTTP Server Functions
//
//			Asynchronous Model
//
//

#include "httpa.h"

//
// Internal linkage
//
static SOCKET listenSocket;
static char szWebRoot[_MAX_PATH];
static HWND	ghwnd;
static UINT guMsgAsy;
static UINT guMsgApp;

////////////////////////////////////////////////////////////

BOOL StartHTTP(LPHTTPSERVINFO lpInfo)
{
	SOCKADDR_IN		saServer;		
	LPSERVENT		lpServEnt;		
	DWORD			dwAddrStrLen;
	char			szBuf[256];		
	char			szAddress[128];
    int				nRet;			

	//
	// Save the Window handle and message
	// ID's for further use
	//
	ghwnd    = lpInfo->hwnd;
	guMsgAsy = lpInfo->uMsgAsy;
	guMsgApp = lpInfo->uMsgApp;
	if (lpInfo->lpRootDir != NULL)
		strcpy(szWebRoot, lpInfo->lpRootDir);
	else
		strcpy(szWebRoot, "/WebPages");

 	//
	// Create a TCP/IP stream socket
	//
	listenSocket = socket(AF_INET, 
						  SOCK_STREAM, 
						  IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		LogWinSockError(ghwnd, 
						"Could not create listen socket",
						WSAGetLastError());
		return FALSE;
	}

	//
	// Request async notification
	//
    nRet = WSAAsyncSelect(listenSocket, 
						  ghwnd, 
						  guMsgAsy,
						  FD_ACCEPT		| 
						  FD_READ		| 
						  FD_WRITE		| 
						  FD_CLOSE);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd,
						 "WSAAsyncSelect() error",
						 WSAGetLastError());
		closesocket(listenSocket);
		return FALSE;
	}

	//
	// If a specific port number was specified
	// then use it
	//
	if (lpInfo->nPort != 0)
		saServer.sin_port = htons(lpInfo->nPort);
	else
	{
		//
		// Find a port number
		//
		lpServEnt = getservbyname("http", "tcp");
		if (lpServEnt != NULL)
			saServer.sin_port = lpServEnt->s_port;
		else
			saServer.sin_port = htons(HTTPPORT);
	}

	//
	// Fill in the rest of the address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;

	//
	// bind our name to the socket
	//
	nRet = bind(listenSocket, 
				(LPSOCKADDR)&saServer, 
				sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd,
						 "bind() error",
						 WSAGetLastError());
		closesocket(listenSocket);
		return FALSE;
	}

	//
	// Set the socket to listen
	//
	nRet = listen(listenSocket, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd,
						 "listen() error",
						 WSAGetLastError());
		closesocket(listenSocket);
		return FALSE;
	}

	//
	// Display the host name and address
	//
	gethostname(szBuf, sizeof(szBuf));
	dwAddrStrLen = sizeof(szAddress);
	GetLocalAddress(szAddress, &dwAddrStrLen);
	LogEvent(ghwnd, 
			 "HTTP Server Started: %s [%s] on port %d",
			 szBuf,
			 szAddress,
			 htons(saServer.sin_port));
	return TRUE;
}

////////////////////////////////////////////////////////////

void StopHTTP()
{
	LPREQUEST lpReq;

	//
	// Close the listening socket
	//
	closesocket(listenSocket);

	//
	// Close all open connections
	//
	lpReq = GetFirstRequest();
	while(lpReq)
	{
		closesocket(lpReq->Socket);
		if (lpReq->hFile != HFILE_ERROR)
			_lclose(lpReq->hFile);
		lpReq = GetNextRequest(lpReq);
	}

	//
	// And clean up the linked list
	// of connections
	//
	DelAllRequests();
	LogEvent(ghwnd, "Server Stopped");
}

////////////////////////////////////////////////////////////

void HandleAsyncMsg(HWND hwnd, 
					WPARAM wParam, 
					LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);

	switch(WSAGETSELECTEVENT(lParam))
	{
		case FD_ACCEPT:
			OnAccept(hwnd, (SOCKET)wParam, nErrorCode);
			break;

		case FD_READ:
			OnRead((SOCKET)wParam, nErrorCode);
			break;

		case FD_WRITE:
			OnWrite((SOCKET)wParam, nErrorCode);
			break;

		case FD_CLOSE:
			OnClose((SOCKET)wParam, nErrorCode);
			break;
	}
}

////////////////////////////////////////////////////////////

void OnAccept(HWND hwnd, SOCKET socket, int nErrorCode)
{
	SOCKADDR_IN	SockAddr;
	LPREQUEST	lpReq;
	SOCKET		peerSocket;
	int			nRet;
	int			nLen;

	//
	// accept the new socket descriptor
	//
	nLen = sizeof(SOCKADDR_IN);
	peerSocket = accept(listenSocket,
					    (LPSOCKADDR)&SockAddr,
						&nLen);
	if (peerSocket == SOCKET_ERROR)
	{
		nRet = WSAGetLastError();
		if (nRet != WSAEWOULDBLOCK)
		{
			//
			// Just log the error and return
			//
			LogWinSockError(ghwnd, 
							"accept() error",
							WSAGetLastError());
			return;
		}
	}

	//
	// Make sure we get async notices for this socket
	//
    nRet = WSAAsyncSelect(peerSocket,
						  hwnd,
						  guMsgAsy, 
						  FD_READ | FD_WRITE | FD_CLOSE);
	if (peerSocket == SOCKET_ERROR)
	{
		nRet = WSAGetLastError();
		//
		// Just log the error and return
		//
		LogWinSockError(ghwnd, 
						"accept() error",
						WSAGetLastError());
		return;
	}

	// 
	// Add the connection to the linked list
	//
	lpReq = AddRequest(peerSocket, 
					   (LPSOCKADDR)&SockAddr, 
					   nLen);
	if (lpReq == NULL)
	{
		// We're probably out of memory
		closesocket(peerSocket);
	}
	LogEvent(ghwnd, 
			 "Connection accepted on socket %d from: %s",
			 peerSocket,
			 inet_ntoa(SockAddr.sin_addr));
}

////////////////////////////////////////////////////////////

void OnRead(SOCKET socket, int nErrorCode)
{
	static BYTE		buf[2048];
	LPREQUEST		lpReq;
	int				nRet;

	//
	// Zero the buffer so the recv is null-terminated
	//
	memset(buf, 0, sizeof(buf));

	// 
	// Find this socket in the linked list
	//
	lpReq = GetRequest(socket);
	if (lpReq == NULL)
	{
		//
		// Not in the list. Log the error,
		// read the data to clear the buffers
		// and close the connection
		//
		nRet = 0;
		while(nRet != SOCKET_ERROR)
			nRet = recv(socket, buf, sizeof(buf)-1, 0);
		closesocket(socket);
		return;
	}

	//
	// Recv the data
	//
	nRet = recv(socket, buf, sizeof(buf)-1, 0);
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return;
		LogWinSockError(ghwnd,
						"recv()",
						WSAGetLastError());
		CloseConnection(lpReq);
		return;
	}

	//
	// Keep statistics
	//
	lpReq->dwRecv += nRet;

	//
	// Parse the HTTP request
	//
	ParseRequest(lpReq, buf);
}

////////////////////////////////////////////////////////////

void OnWrite(SOCKET socket, int nErrorCode)
{
	LPREQUEST lpReq;
	BYTE buf[1024];
	int nRet;

	lpReq = GetRequest(socket);
	if (lpReq == NULL)
	{
		//
		// Not in our list!?
		//
		nRet = 0;
		while(nRet != SOCKET_ERROR)
			nRet = recv(socket, buf, sizeof(buf)-1, 0);
		closesocket(socket);
		return;		
	}

	//
	// Is this the first FD_WRITE 
	// or did we fill the protocol
	// stack buffers?
	//
	if (lpReq->hFile == HFILE_ERROR)
		return;

	//
	// Continue sending a file
	//
	SendFileContents(lpReq);
}

////////////////////////////////////////////////////////////

void OnClose(SOCKET socket, int nErrorCode)
{
	LPREQUEST lpReq;

	//
	// Have we already deleted this entry?
	//
	lpReq = GetRequest(socket);
	if (lpReq == NULL)
		return;		

	// 
	// It's still in our list
	// The client must have reset the connection.
	// Clean up.
	//
	CloseConnection(lpReq);
}

////////////////////////////////////////////////////////////

void ParseRequest(LPREQUEST lpReq, LPBYTE lpBuf)
{
	char szFileName[_MAX_PATH];
	char szSeps[] = " \n";
	char *cpToken;

	//
	// Don't let requests include ".." characters
	// in requests
	//
	if (strstr(lpBuf, "..") != NULL)
	{
		// Send "bad request" error
		SendError(lpReq, HTTP_STATUS_BADREQUEST);
		CloseConnection(lpReq);
		return;
	}

	//
	// Determine request method
	//
	cpToken = strtok(lpBuf, szSeps);
	if (!_stricmp(cpToken, "GET"))
		lpReq->nMethod = METHOD_GET;
	else
	{
		// Send "not implemented" error
		SendError(lpReq, HTTP_STATUS_NOTIMPLEMENTED);
		CloseConnection(lpReq);
		return;
	}

	//
	// Get the file name
	//
	cpToken = strtok(NULL, szSeps);
	if (cpToken == NULL)
	{
		// Send "bad request" error
		SendError(lpReq, HTTP_STATUS_BADREQUEST);
		CloseConnection(lpReq);
		return;
	}
	strcpy(szFileName, szWebRoot);
	if (strlen(cpToken) > 1)
		strcat(szFileName, cpToken);
	else
		strcat(szFileName, "/index.html");
	SendFile(lpReq, szFileName);

}

////////////////////////////////////////////////////////////

void CloseConnection(LPREQUEST lpReq)
{
	HTTPSTATS stats;

	LogEvent(ghwnd, 
			 "Closing socket: %d",
			 lpReq->Socket);
	//
	// Log the event and close the socket
	//
	closesocket(lpReq->Socket);

	//
	// If the file is still open,
	// then close it
	//
	if (lpReq->hFile != HFILE_ERROR)
		_lclose(lpReq->hFile);

	//
	// Give the user interface the stats
	//
	stats.dwElapsedTime = 
			(GetTickCount() - lpReq->dwConnectTime);
	stats.dwRecv = lpReq->dwRecv;
	stats.dwSend = lpReq->dwSend;
	SendMessage(ghwnd,
				guMsgApp,
				HTTP_STATS_MSG,
				(LPARAM)&stats);
	DelRequest(lpReq);
}

////////////////////////////////////////////////////////////

void SendFile(LPREQUEST lpReq, LPCSTR lpFileName)
{
	//
	// Open the file for reading
	//
	lpReq->hFile = _lopen(lpFileName, 
						  OF_READ|OF_SHARE_COMPAT);
	if (lpReq->hFile == HFILE_ERROR)
	{
		SendMessage(ghwnd,
					guMsgApp,
					HTTP_FILENOTFOUND_MSG,
					(LPARAM)(LPCSTR)lpFileName);
		// Send "404 Not Found" error
		SendError(lpReq, HTTP_STATUS_NOTFOUND);
		CloseConnection(lpReq);
		return;
	}

	//
	// Tell the user interface about the file hit
	// (Sending just the request portion -- without
	// the root web directory portion)
	//
	SendMessage(ghwnd,
				guMsgApp,
				HTTP_FILEOK_MSG,
				(LPARAM)(LPCSTR)lpFileName + strlen(szWebRoot));

	//
	// Send as much of the file as we can
	//
	lpReq->dwFilePtr = 0L;
	SendFileContents(lpReq);
}

////////////////////////////////////////////////////////////

void SendError(LPREQUEST lpReq, UINT uError)
{
	int nRet;
	static char szMsg[512];
	static char *szStatMsgs [] = {
								"200 OK",
								"201 Created",
								"202 Accepted",
								"204 No Content",
								"301 Moved Permanently",
								"302 Moved Temporarily",
								"304 Not Modified",
								"400 Bad Request",
								"401 Unauthorized",
								"403 Forbidden",
								"404 Not Found",
								"500 Internal Server Error",
								"501 Not Implemented",
								"502 Bad Gateway",
								"503 Service Unavailable"
								};
	#define NUMSTATMSGS sizeof(szStatMsgs) / sizeof(szStatMsgs[0])

	if (uError < 0 || uError > NUMSTATMSGS)
		return;

	wsprintf(szMsg, "<body><h1>%s</h1></body>",
			 szStatMsgs[uError]);
	nRet = send(lpReq->Socket,
				szMsg,
				strlen(szMsg),
				0);
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			LogWinSockError(ghwnd, 
							"send()@SendError()",
							WSAGetLastError());
			nRet = 0;
		}
	}
	lpReq->dwSend += nRet;
}

////////////////////////////////////////////////////////////

void SendFileContents(LPREQUEST lpReq)
{
	static BYTE buf[1024];
	UINT uBytes;
	BOOL fEof;
	int nBytesSent;

	fEof = FALSE;

	//
	// We may be continuing, so seek to where
	// we left off last time
	//
	if (lpReq->dwFilePtr > 0)
		_llseek(lpReq->hFile, lpReq->dwFilePtr, FILE_BEGIN);

	//
	// Send as much of the file as we can
	//
	while(1)
	{
		//
		// Read a buffer full from the file
		//
		uBytes = _lread(lpReq->hFile,
						buf,
						sizeof(buf));
		if (uBytes == HFILE_ERROR)
		{
			LogEvent(ghwnd, 
					 "Read file error: %d", 
					 GetLastError());
			// Send "500 Internal server" error
			SendError(lpReq, HTTP_STATUS_SERVERERROR);
			CloseConnection(lpReq);
			_lclose(lpReq->hFile);
			lpReq->hFile = HFILE_ERROR;
			return;
		}

		//
		// Are we at End of File?
		//
		if (uBytes < sizeof(buf))
			fEof = TRUE;

		//
		// Send this buffer to the client
		//
		nBytesSent = send(lpReq->Socket,
						  buf,
						  uBytes,
						  0);
		if (nBytesSent == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				LogWinSockError(ghwnd, 
								"send()",
								WSAGetLastError());
				CloseConnection(lpReq);
				_lclose(lpReq->hFile);
				lpReq->hFile = HFILE_ERROR;
				return;
			}
			nBytesSent = 0;
		}

		//
		// Keep track of what has actually been sent
		// from the file
		//
		lpReq->dwFilePtr += nBytesSent;

		//
		// And for statistics
		//
		lpReq->dwSend += nBytesSent;

		//
		// Are the protocol stack buffers full?
		//
		if (nBytesSent < (int)uBytes)
		{
			// We'll have to finish later
			return;
		}

		//
		// Are we finished?
		//
		if (fEof)
		{
			CloseConnection(lpReq);
			_lclose(lpReq->hFile);
			lpReq->hFile = HFILE_ERROR;
			return;
		}
	}
}

////////////////////////////////////////////////////////////

void LogEvent(HWND hwnd, LPCSTR lpFormat, ...)
{
	va_list Marker;
	char szBuf[256];
	
	// Write text to string
	// and append to edit control
	va_start(Marker, lpFormat);
	vsprintf(szBuf, lpFormat, Marker);
	va_end(Marker);
	SendMessage(ghwnd,
				guMsgApp,
				HTTP_EVENT_MSG,
				(LPARAM)szBuf);
}

////////////////////////////////////////////////////////////

void LogWinSockError(HWND hwnd, LPCSTR lpText, int nErrorCode)
{
	char szBuf[256];

	szBuf[0] = '\0';
	LoadString(GetWindowInstance(hwnd),
			   nErrorCode,
			   szBuf,
			   sizeof(szBuf));
	LogEvent(hwnd, "%s : %s", lpText, szBuf);
}

////////////////////////////////////////////////////////////

int GetLocalAddress(LPSTR lpStr, LPDWORD lpdwStrLen)
{
    struct in_addr *pinAddr;
    LPHOSTENT	lpHostEnt;
	int			nRet;
	int			nLen;

	//
	// Get our local name
	//
    nRet = gethostname(lpStr, *lpdwStrLen);
	if (nRet == SOCKET_ERROR)
	{
		lpStr[0] = '\0';
		return SOCKET_ERROR;
	}

	//
	// "Lookup" the local name
	//
	lpHostEnt = gethostbyname(lpStr);
    if (lpHostEnt == NULL)
	{
		lpStr[0] = '\0';
		return SOCKET_ERROR;
	}

	//
    // Format first address in the list
	//
	pinAddr = ((LPIN_ADDR)lpHostEnt->h_addr);
	nLen = strlen(inet_ntoa(*pinAddr));
	if ((DWORD)nLen > *lpdwStrLen)
	{
		*lpdwStrLen = nLen;
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	*lpdwStrLen = nLen;
	strcpy(lpStr, inet_ntoa(*pinAddr));
    return 0;
}
