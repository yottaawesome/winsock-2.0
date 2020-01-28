//
// GetHTTP.cpp
//
// Retrieves a file using the Hyper Text Transfer Protocol
// and prints its contents to stdout.
//

//
// Pass the server name and full path of the file on the 
// command line and redirect the output to a file. The program
// prints messages to stderr as it progresses.
//
// Example:
//		GetHTTP www.idgbooks.com /index.html > index.html
//

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <winsock.h>


void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName);

// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError())

////////////////////////////////////////////////////////////

void main(int argc, char **argv)
{
	WORD wVersionRequested = MAKEWORD(1,1);
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
	// Use inet_addr() to determine if we're dealing with a name
	// or an address
	//
	IN_ADDR		iaHost;
	LPHOSTENT	lpHostEntry;

	iaHost.s_addr = inet_addr(lpServerName);
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(lpServerName);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost, 
						sizeof(struct in_addr), AF_INET);
	}
	if (lpHostEntry == NULL)
	{
		PRINTERROR("gethostbyname()");
		return;
	}


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
	// Find the port number for the HTTP service on TCP
	//
	LPSERVENT lpServEnt;
	SOCKADDR_IN saServer;

	lpServEnt = getservbyname("http", "tcp");
	if (lpServEnt == NULL)
		saServer.sin_port = htons(80);
	else
		saServer.sin_port = lpServEnt->s_port;


	//
	// Fill in the rest of the server address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);


	//
	// Connect the socket
	//
	int nRet;

	nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		PRINTERROR("connect()");
		closesocket(Socket);
		return;
	}

	
	//
	// Format the HTTP request
	//
	char szBuffer[1024];

	sprintf(szBuffer, "GET %s\n", lpFileName);
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
		PRINTERROR("send()");
		closesocket(Socket);	
		return;
	}

	//
	// Receive the file contents and print to stdout
	//
	while(1)
	{
		// Wait to receive, nRet = NumberOfBytesReceived
		nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
		if (nRet == SOCKET_ERROR)
		{
			PRINTERROR("recv()");
			break;
		}

		fprintf(stderr,"\nrecv() returned %d bytes", nRet);

		// Did the server close the connection?
		if (nRet == 0)
			break;
		// Write to stdout
        fwrite(szBuffer, nRet, 1, stdout);
	}
	closesocket(Socket);	
}
