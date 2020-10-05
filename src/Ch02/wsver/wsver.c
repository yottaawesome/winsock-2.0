//
// WSVer.c
//
// Prints info returned from WSAStartup()
//

//
// Optionally, you can pass the version you 
// would like to request on the command line.
// Pass the major version number followed by 
// the minor version number.
//
// Examples:
// To request version 1.1	WSVer 1 1
// To request version 2.0	WSVer 2 0
//
// If you do not specify a version to try,
// the program will request version 1.1
//

#include <stdio.h>
#include <winsock.h>

#pragma comment(lib, "wsock32.lib")

// Utility function in wsedesc.c
LPCSTR WSErrorDescription(int iErrorCode);

void PrintWSAData(LPWSADATA pWSAData);

void main(int argc, char **argv)
{
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	int rc;

	if (argc == 3)
		wVersionRequested = MAKEWORD(atol(argv[1]), 
								atol(argv[2]));

	printf("\nRequesting version %d.%d\n",
						LOBYTE(wVersionRequested),
						HIBYTE(wVersionRequested));

	rc = WSAStartup(wVersionRequested, &wsaData);
	if (!rc)
		PrintWSAData(&wsaData);
	else
		fprintf(stderr,"\nWSAStartup() error (%d) %s\n", 
										rc,
										WSErrorDescription(rc));
	WSACleanup();
}

void PrintWSAData(LPWSADATA pWSAData)
{
	printf("\nWSADATA");
	printf("\n----------------------");
	printf("\nVersion..............: %d.%d", 
			LOBYTE(pWSAData->wVersion),
			HIBYTE(pWSAData->wVersion));
	printf("\nHighVersion..........: %d.%d",
			LOBYTE(pWSAData->wHighVersion),
			HIBYTE(pWSAData->wHighVersion));
	printf("\nDescription..........: %s", 
			pWSAData->szDescription);
	printf("\nSystem status........: %s",
			pWSAData->szSystemStatus);
	printf("\nMax number of sockets: %d",
			pWSAData->iMaxSockets);
	printf("\nMAX UDP datagram size: %d\n",
			pWSAData->iMaxUdpDg);
}

