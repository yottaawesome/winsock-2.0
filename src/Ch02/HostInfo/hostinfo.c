//
// HostInfo.c  
//
// Use WinSock gethostbyX functions to lookup a host
// name or IP address and print the returned hostent
// structure;
//

//
// Pass a server name or IP address on the command line.
//
// Examples:
//        HostInfo www.idgbooks.com
//        HostInfo www.sockets.com
//        HostInfo 207.68.156.52
//

#include <stdio.h>
#include <winsock.h>

// Function to print a hostent structure
int Printhostent(LPCSTR lpServerNameOrAddress);

void main(int argc, char **argv)
{
    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA wsaData;
    int nRC;

    // Check arguments
    if (argc != 2)
    {
        fprintf(stderr,
            "\nSyntax: HostInfo ServerNameOrAddress\n");
        return;
    }

    // Initialize WinSock.dll
    nRC = WSAStartup(wVersionRequested, &wsaData);
    if (nRC)
    {
        fprintf(stderr,"\nWSAStartup() error: %d\n", nRC); 
        WSACleanup();
        return;
    }

    // Check WinSock version
    if (wVersionRequested != wsaData.wVersion)
    {
        fprintf(stderr,"\nWinSock version 1.1 not supported\n");
        WSACleanup();
        return;
    }

    // Call Printhostent() to do all the work
    nRC = Printhostent(argv[1]);
    if (nRC)
        fprintf(stderr,"\nPrinthostent return code: %d\n", nRC);
    WSACleanup();
}

int Printhostent(LPCSTR lpServerNameOrAddress)
{
    LPHOSTENT lpHostEntry;     // Pointer to host entry structure
    struct in_addr iaHost;     // Internet address structure
    struct in_addr *pinAddr;   // Pointer to an internet address
    LPSTR lpAlias;             // Character pointer for alias names
    int iNdx;

    // Use inet_addr() to determine if we're dealing with a name
    // or an address
    iaHost.s_addr = inet_addr(lpServerNameOrAddress);
    if (iaHost.s_addr == INADDR_NONE)
    {
        // Wasn't an IP address string, assume it is a name
        lpHostEntry = gethostbyname(lpServerNameOrAddress);
    }
    else
    {
        // It was a valid IP address string
        lpHostEntry = gethostbyaddr((const char *)&iaHost, 
                        sizeof(struct in_addr), AF_INET);
    }

    // Check return value
    if (lpHostEntry == NULL)
    {
        fprintf(stderr,"\nError getting host: %d",
                 WSAGetLastError());
        return WSAGetLastError();
    }

    // Print structure
    printf("\n\nHOSTENT");
    printf("\n-----------------");

    // Host name
    printf("\nHost Name........: %s", lpHostEntry->h_name);

    // List of host aliases
    printf("\nHost Aliases.....");
    for (iNdx = 0; ; iNdx++)
    {
        lpAlias = lpHostEntry->h_aliases[iNdx];
        if (lpAlias == NULL)
            break;
        printf(": %s", lpAlias);
        printf("\n                 ");
    }

    // Address type
    printf("\nAddress type.....: %d", lpHostEntry->h_addrtype);
    if (lpHostEntry->h_addrtype == AF_INET)
        printf(" (AF_INET)");
    else
        printf(" (UnknownType)");

    // Address length
    printf("\nAddress length...: %d", lpHostEntry->h_length);

    // List of IP addresses
    printf("\nIP Addresses.....");
    for (iNdx = 0; ; iNdx++)
    {
        pinAddr = ((LPIN_ADDR)lpHostEntry->h_addr_list[iNdx]);
        if (pinAddr == NULL)
            break;
        printf(": %s", inet_ntoa(*pinAddr));
        printf("\n                 ");
    }
    printf("\n");
    return 0;
}
