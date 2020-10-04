//
// DServer.cpp
//
// Extremely simple, totally useless datagram server example.
// Works in conjunction with DClient.cpp.
//
// The program sets itself up as a server using the UDP
// protoocl. It waits for data from a client, displays
// the incoming data, sends a message back to the client
// and then exits.
//
// Compile and link with wsock32.lib
//
// Pass the port number that the server should bind() to
// on the command line. Any port number not already in use
// can be specified.
//
// Example: DServer 2000
//

#include <stdio.h>
#include <winsock.h>

#pragma comment(lib, "wsock32.lib")

// Function prototype
void DatagramServer(short nPort);

// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError())

////////////////////////////////////////////////////////////

void main(int argc, char **argv)
{
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	int nRet;
	short nPort;

	//
	// Check for port argument
	//
	if (argc != 2)
	{
		fprintf(stderr,"\nSyntax: dserver PortNumber\n");
		return;
	}

	nPort = atoi(argv[1]);
	
	//
	// Initialize WinSock and check version
	//
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{	
		fprintf(stderr,"\n Wrong version\n");
		return;
	}

	//
	// Do all the stuff a datagram server does
	//
	DatagramServer(nPort);
	
	//
	// Release WinSock
	//
	WSACleanup();
}

////////////////////////////////////////////////////////////

void DatagramServer(short nPort)
{
	//
	// Create a UDP/IP datagram socket
	//
	SOCKET theSocket;

	theSocket = socket(AF_INET,		// Address family
					   SOCK_DGRAM,  // Socket type
					   IPPROTO_UDP);// Protocol
	if (theSocket == INVALID_SOCKET)
	{
		PRINTERROR("socket()");
		return;
	}

	
	//
	// Fill in the address structure
	//
	SOCKADDR_IN saServer;

	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY; // Let WinSock assign address
	saServer.sin_port = htons(nPort);	   // Use port passed from user


	//
	// bind the name to the socket
	//
	int nRet;

	nRet = bind(theSocket,				// Socket descriptor
				(LPSOCKADDR)&saServer,  // Address to bind to
				sizeof(struct sockaddr)	// Size of address
				);
	if (nRet == SOCKET_ERROR)
	{
		PRINTERROR("bind()");
		closesocket(theSocket);
		return;
	}


	//
	// This isn't normally done or required, but in this 
	// example we're printing out where the server is waiting
	// so that you can connect the example client.
	//
	int nLen;
	nLen = sizeof(SOCKADDR);
	char szBuf[256];

	nRet = gethostname(szBuf, sizeof(szBuf));
	if (nRet == SOCKET_ERROR)
	{
		PRINTERROR("gethostname()");
		closesocket(theSocket);
		return;
	}

	//
	// Show the server name and port number
	//
	printf("\nServer named %s waiting on port %d\n",
			szBuf, nPort);
			

	//
	// Wait for data from the client
	//
	SOCKADDR_IN saClient;

	memset(szBuf, 0, sizeof(szBuf));
	nRet = recvfrom(theSocket,				// Bound socket
					szBuf,					// Receive buffer
					sizeof(szBuf),			// Size of buffer in bytes
					0,						// Flags
					(LPSOCKADDR)&saClient,	// Buffer to receive client address 
					&nLen);					// Length of client address buffer

	//
	// Show that we've received some data
	//
	printf("\nData received: %s", szBuf);


	//
	// Send data back to the client
	//
	strcpy(szBuf, "From the Server");
	sendto(theSocket,						// Bound socket
		   szBuf,							// Send buffer
		   strlen(szBuf),					// Length of data to be sent
		   0,								// Flags
		   (LPSOCKADDR)&saClient,			// Address to send data to
		   nLen);							// Length of address

	//
	// Normally a server continues to run so that
	// it is available to other clients. In this
	// example, we exit as soon as one transaction
	// has taken place.
	//
	closesocket(theSocket);
	return;
}
