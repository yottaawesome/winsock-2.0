//
// GetHTTP2.cpp --	Retrieve a file from a HTTP server
//
//					This version uses event objects and 
//					WSASelectEvent() for asynchronous
//					notification of network events.
//
// Compile and link with ws2_32.lib
//

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <winsock2.h>

void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName);

// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%s %d\n", s, WSAGetLastError())

void main(int argc, char **argv)
{
	WORD wVersionRequested = WINSOCK_VERSION;
	WSADATA wsaData;
	int nRet;

	//
	// Check arguments
	//
	if (argc != 3)
	{
		fprintf(stderr,
			"\nSyntax: GetHTTP ServerName FullPathName\n");
		return;
	}

	//
	// Initialize WinSock.dll
	//
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		fprintf(stderr,"\nWSAStartup(): %d\n", nRet);
		WSACleanup();
		return;
	}

	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{
		fprintf(stderr,"\nWinSock version not supported\n");
		WSACleanup();
		return;
	}

	//
	// Set "stdout" to binary mode
	// so that redirection will work
	// for .gif and .jpg files
	//
	_setmode(_fileno(stdout), _O_BINARY);

	//
	// Call GetHTTP() to do all the work
	//
	GetHTTP(argv[1], argv[2]);

	//
	// Release WinSock
	//
	WSACleanup();
}

////////////////////////////////////////////////////////////

void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName)
{
	// 
	// Lookup host
	//
	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(lpServerName);
	if (lpHostEntry == NULL)
	{
		PRINTERROR("gethostbyname()");
		return;
	}

	//
	// Fill in the server address structure
	//
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	sa.sin_port = htons(80);	// Well-known HTTP port

	//	
	// Create a TCP/IP stream socket
	//
	SOCKET	Socket;
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		PRINTERROR("socket()"); 
		return;
	}

	//
	// Create an event object to be used with this socket
	//
	WSAEVENT hEvent;
	hEvent = WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT)
	{
		PRINTERROR("WSACreateEvent()");
		closesocket(Socket);
		return;
	}

	//
	// Make the socket non-blocking and 
	// associate it with network events
	//
	int nRet;
	nRet = WSAEventSelect(Socket,
						  hEvent,
						  FD_READ|FD_CONNECT|FD_CLOSE);
	if (nRet == SOCKET_ERROR)
	{
		PRINTERROR("EventSelect()");
		closesocket(Socket);
		WSACloseEvent(hEvent);
		return;
	}

	//
	// Request a connection
	//
	nRet = connect(Socket, 
	               (LPSOCKADDR)&sa, 
				   sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		nRet = WSAGetLastError();
		if (nRet == WSAEWOULDBLOCK)
		{
			fprintf(stderr,"\nConnect would block");
		}
		else
		{
			PRINTERROR("connect()");
			closesocket(Socket);
			WSACloseEvent(hEvent);
			return;
		}
	}
	
	//
	// Handle async network events
	//
	char szBuffer[4096];
	WSANETWORKEVENTS events;
	while(1)
	{
		//
		// Wait for something to happen
		//
		fprintf(stderr,"\nWaitForMultipleEvents()");
		DWORD dwRet;
		dwRet = WSAWaitForMultipleEvents(1,
									 &hEvent,
									 FALSE,
									 10000,
									 FALSE);
		if (dwRet == WSA_WAIT_TIMEOUT)
		{
			fprintf(stderr,"\nWait timed out");
			break;
		}

		//
		// Figure out what happened
		//
		fprintf(stderr,"\nWSAEnumNetworkEvents()");
		nRet = WSAEnumNetworkEvents(Socket,
								 hEvent,
								 &events);
		if (nRet == SOCKET_ERROR)
		{
			PRINTERROR("WSAEnumNetworkEvents()");
			break;
		}

		//				 //
		// Handle events //
		//				 //

		// Connect event?
		if (events.lNetworkEvents & FD_CONNECT)
		{
			fprintf(stderr,"\nFD_CONNECT: %d",
					events.iErrorCode[FD_CONNECT_BIT]);
			// Send the http request
			sprintf(szBuffer, "GET %s\n", lpFileName);
			nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
			if (nRet == SOCKET_ERROR)
			{
				PRINTERROR("send()");
				break;
			}
		}

		// Read event?
		if (events.lNetworkEvents & FD_READ)
		{
			fprintf(stderr,"\nFD_READ: %d",
					events.iErrorCode[FD_READ_BIT]);
			// Read the data and write it to stdout
			nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
			if (nRet == SOCKET_ERROR)
			{
				PRINTERROR("recv()");
				break;
			}
			fprintf(stderr,"\nRead %d bytes", nRet);
			// Write to stdout
		    fwrite(szBuffer, nRet, 1, stdout);
		}

		// Close event?
		if (events.lNetworkEvents & FD_CLOSE)
		{
			fprintf(stderr,"\nFD_CLOSE: %d",
					events.iErrorCode[FD_CLOSE_BIT]);
			break;
		}

		// Write event?
		if (events.lNetworkEvents & FD_WRITE)
		{
			fprintf(stderr,"\nFD_WRITE: %d",
					events.iErrorCode[FD_WRITE_BIT]);
		}

	}
	closesocket(Socket);	
	WSACloseEvent(hEvent);
	return;
}

