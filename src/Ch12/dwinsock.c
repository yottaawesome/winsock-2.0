//
// DWINSOCK.C	Dynamic WinSock
//
//				Functions for dynamically linking to
//				best available WinSock.
//
//				Dynamically links to WS2_32.DLL or
//				if WinSock 2 isn't available, it
//				dynamically links to WSOCK32.DLL.
//
//

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <wsipx.h>
#include <tchar.h>
#include "dwinsock.h"


//
// Declare global function pointers
//
#define DWINSOCK_GLOBAL
#include "dwnsock1.inc"
#include "dwnsock2.inc"

//
// Internal Functions and data
//
static BOOL MapFunctionPointers(BOOL fMapVersion2);
static int  FakeEnumProtocols(LPWSAPROTOCOL_INFO lpProtocolBuffer, 
							  LPDWORD pdwBufLen);
int IPXAddressToString(LPSOCKADDR_IPX pAddr, 
					   DWORD dwAddrLen,
					   LPTSTR lpAddrStr,
					   LPDWORD pdwStrLen);

static HANDLE	hndlWinSock = INVALID_HANDLE_VALUE;
static int		nVersion	= 0;
static int		nMaxUdp		= 0;

////////////////////////////////////////////////////////////

int DWSInitWinSock(void)
{
	WORD wVersionRequested;
	BOOL f2Loaded = TRUE;
	WSADATA wsaData;
	int nRet;

	//
	// Attempt to dynamically load WS2_32.DLL
	//
	hndlWinSock = LoadLibrary("WS2_32.DLL");
	if (hndlWinSock == NULL)
	{
		//
		// Couldn't load WinSock 2, try 1.1
		//
		f2Loaded = FALSE;
		hndlWinSock = LoadLibrary("WSOCK32.DLL");
		if (hndlWinSock == NULL)
			return 0;
	}

	//
	// Use GetProcAddress to initialize 
	// the function pointers
	//
	if (!MapFunctionPointers(f2Loaded))
		return 0;

	//
	// If WinSock 2 was loaded, ask for 2.2 otherwise 1.1
	//
	if (f2Loaded)
		wVersionRequested = MAKEWORD(2,2);
	else
		wVersionRequested = MAKEWORD(1,1);

	//
	// Call WSAStartup()
	//
	nRet = p_WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		FreeLibrary(hndlWinSock);
		return 0;
	}

	if (wVersionRequested != wsaData.wVersion)
	{
		FreeLibrary(hndlWinSock);
		return 0;
	}

	// Save Max UDP for use with 1.1
	nMaxUdp = wsaData.iMaxUdpDg;

	//
	// Return 1 or 2
	//
	nVersion = f2Loaded ? 2 : 1; 
	return(nVersion);
}

////////////////////////////////////////////////////////////

BOOL DWSFreeWinSock(void)
{
	if (p_WSACleanup != NULL)
		p_WSACleanup();
	nVersion = 0;
	return(FreeLibrary(hndlWinSock));
}

////////////////////////////////////////////////////////////

BOOL MapFunctionPointers(BOOL fMapVersion2)
{
	//
	// This variable must be declared
	// with this name in order to use
	// #define DWINSOCK_GETPROCADDRESS
	//
	BOOL fOK = TRUE;

	//
	// GetProcAddress for functions 
	// available in both 1.1 and 2
	//
	#define DWINSOCK_GETPROCADDRESS
	#include "dwnsock1.inc"

	//
	// If that went OK, and we're supposed 
	// to map version 2, then do GetProcAddress
	// for functions only available in WinSock 2
	//
	if (fOK && fMapVersion2)
	{
		#include "dwnsock2.inc"
	}
	return fOK;
}

////////////////////////////////////////////////////////////

int DWSVersion(void)
{
	return nVersion;
}

////////////////////////////////////////////////////////////

int DWSMaxMsgSize(SOCKET socket)
{
	int nOptVal;
	int nOptLen;
	int nRet;

	//
	// If WinSock isn't loaded
	// return 0
	//
	if (nVersion == 0)
		return 0;

	//
	// If we're using WinSock 1.1
	// return the previously saved
	// info from WSADATA
	//
	if (nVersion == 1)
		return nMaxUdp;

	//
	// Must be WinSock 2
	//

	// 
	// Use getsockopt() to find
	// the max message size for
	// this socket
	//
	nOptLen = sizeof(int);
	nRet = p_getsockopt(socket,
					  SOL_SOCKET,
					  SO_MAX_MSG_SIZE,
					  (char *)&nOptVal,
					  &nOptLen);
	if (nRet == SOCKET_ERROR)
		return SOCKET_ERROR;
	return nOptVal;
}

////////////////////////////////////////////////////////////

int DWSAddressToString(LPSOCKADDR pAddr, DWORD dwAddrLen,
					   LPTSTR lpAddrStr, LPDWORD pdwStrLen)
{
	LPTSTR			lpStr;
	LPSOCKADDR_IN	pInAddr;
	int				nRet = 0;

	lpAddrStr[0] = '\0';

	// 
	// If WinSock isn't loaded, return 1
	//
	if (nVersion == 0)
		return 1;

	//
	// If using WinSock 1.1 then assume IP
	//
	if (nVersion == 1)
	{
		//
		// Convert address with inet_ntoa
		//
		pInAddr = (LPSOCKADDR_IN)pAddr;
		lpStr = p_inet_ntoa(pInAddr->sin_addr);
		if (lpStr == NULL)
		{
			p_WSASetLastError(WSAEINVAL);
			*pdwStrLen = 0;
			return SOCKET_ERROR;	
		}
		#ifdef UNICODE
			//
			// Convert inet_ntoa string to wide char
			//
			nRet = MultiByteToWideChar(CP_ACP,
										0,
										lpStr,
										-1,
										lpAddrStr,
										*pdwStrLen);
			if (nRet == 0)
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					p_WSASetLastError(WSAEFAULT);
				else
					p_WSASetLastError(WSAEINVALID);
				return SOCKET_ERROR;
			}
		#else
			//
			// ANSI -- Check the string length
			//
			if (strlen(lpStr) > *pdwStrLen)
			{
				p_WSASetLastError(WSAEFAULT);
				*pdwStrLen = strlen(lpStr);
				return SOCKET_ERROR;
			}
			strcpy(lpAddrStr, lpStr);
			*pdwStrLen = strlen(lpStr);
			return 0;
		#endif
	}

	//
	// Must be WinSock 2
	//
	nRet = p_WSAAddressToString(pAddr,
							    dwAddrLen,
							    NULL,
							    lpAddrStr,
							    pdwStrLen);
	//
	// Check for "unsupported" families
	//
	if (nRet == SOCKET_ERROR && p_WSAGetLastError() == WSAEOPNOTSUPP)
	{
		if (pAddr->sa_family == AF_IPX)
		{
			nRet = IPXAddressToString((LPSOCKADDR_IPX)pAddr,
									  dwAddrLen,
									  lpAddrStr,
									  pdwStrLen);
		}
	}
	return nRet;
}

////////////////////////////////////////////////////////////

int DWSEnumProtocols(LPWSAPROTOCOL_INFO lpProtocolBuffer, 
					 LPDWORD pdwBufLen)
{
	if (nVersion == 0)
		return 0;

	if (nVersion == 1)
		return(FakeEnumProtocols(lpProtocolBuffer,
								 pdwBufLen));
	//
	// Must be WinSock 2
	//
	return(p_WSAEnumProtocols(NULL,
							 lpProtocolBuffer,
							 pdwBufLen));
}

////////////////////////////////////////////////////////////

int FakeEnumProtocols(LPWSAPROTOCOL_INFO lpProtocolBuffer, 
					  LPDWORD pdwBufLen)
{
	WSAPROTOCOL_INFO FakeTcp;
	WSAPROTOCOL_INFO FakeUdp;
	LPWSAPROTOCOL_INFO lpInfo;

	//
	// Assume WinSock 1.1, we're returning
	// 2 WSAPROTOCOL_INFO structures.
	// 1 for TCP, 1 for UDP
	//

	//
	// Check the buffer size
	//
	if (*pdwBufLen < (sizeof(WSAPROTOCOL_INFO)*2))
	{
		*pdwBufLen = (sizeof(WSAPROTOCOL_INFO)*2);
		p_WSASetLastError(WSAENOBUFS);
		return SOCKET_ERROR;
	}

	//
	// Build fake TCP entry
	//
	memset(&FakeTcp, 0, sizeof(WSAPROTOCOL_INFO));
	FakeTcp.dwServiceFlags1 = XP1_GUARANTEED_DELIVERY	|
							  XP1_GUARANTEED_ORDER		|
							  XP1_GRACEFUL_CLOSE		|
							  XP1_EXPEDITED_DATA;
	FakeTcp.dwProviderFlags = PFL_MATCHES_PROTOCOL_ZERO;
	FakeUdp.ProtocolChain.ChainLen = 1;	// Base protocol
	FakeTcp.dwCatalogEntryId= 1;;
	FakeTcp.iVersion		= 1;
	FakeTcp.iAddressFamily  = AF_INET;
	FakeTcp.iMaxSockAddr	= sizeof(SOCKADDR_IN);
	FakeTcp.iMinSockAddr	= sizeof(SOCKADDR_IN);
	FakeTcp.iSocketType		= SOCK_STREAM;
	FakeTcp.iProtocol		= IPPROTO_TCP;
	FakeTcp.iNetworkByteOrder = BIGENDIAN;
	FakeTcp.dwMessageSize	= 0;
	_tcscpy(FakeTcp.szProtocol, _TEXT("tcp"));

	//
	// Build fake UDP entry
	//
	memset(&FakeUdp, 0, sizeof(WSAPROTOCOL_INFO));
	FakeUdp.dwServiceFlags1 = XP1_CONNECTIONLESS		|
							  XP1_MESSAGE_ORIENTED		|
							  XP1_SUPPORT_BROADCAST;		
							  // Might want to add multipoint
	FakeUdp.dwProviderFlags = PFL_MATCHES_PROTOCOL_ZERO;
	FakeUdp.ProtocolChain.ChainLen = 1;	// Base protocol
	FakeUdp.dwCatalogEntryId= 2;;
	FakeUdp.iVersion		= 1;
	FakeUdp.iAddressFamily  = AF_INET;
	FakeUdp.iMaxSockAddr	= sizeof(SOCKADDR_IN);
	FakeUdp.iMinSockAddr	= sizeof(SOCKADDR_IN);
	FakeUdp.iSocketType		= SOCK_DGRAM;
	FakeUdp.iProtocol		= IPPROTO_UDP;
	FakeUdp.iNetworkByteOrder = BIGENDIAN;
	FakeUdp.dwMessageSize	= nMaxUdp;	// The reason this can't be static
	_tcscpy(FakeUdp.szProtocol, _TEXT("udp"));

	//
	// Copy the fake entries to the supplied buffer
	lpInfo = lpProtocolBuffer;
	memcpy(lpInfo, &FakeTcp, sizeof(WSAPROTOCOL_INFO));
	lpInfo++;
	memcpy(lpInfo, &FakeUdp, sizeof(WSAPROTOCOL_INFO));

	//
	// Returning 2 protocols
	//	
	return 2;
}

////////////////////////////////////////////////////////////

int DWSSelectProtocols(
					DWORD dwSetFlags,
					DWORD dwNotSetFlags,
					LPWSAPROTOCOL_INFO lpProtocolBuffer,
					LPDWORD lpdwBufferLength
					)
{
	LPBYTE				pBuf;
	LPWSAPROTOCOL_INFO	pInfo;
	DWORD				dwNeededLen;
	LPWSAPROTOCOL_INFO	pRetInfo;
	DWORD				dwRetLen;
	int					nCount;
	int					nMatchCount;
	int					nRet;

	if (nVersion == 0)
		return 0;

	//
	// Determine needed buffer size
	//
	dwNeededLen = 0;
	nRet = DWSEnumProtocols(NULL, &dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		if (p_WSAGetLastError() != WSAENOBUFS)
			return SOCKET_ERROR;
	}

	//
	// Allocate the buffer
	//
	pBuf = malloc(dwNeededLen);
	if (pBuf == NULL)
	{
		p_WSASetLastError(WSAENOBUFS);
		return SOCKET_ERROR;
	}

	//
	// Make the "real" call
	//
	nRet = DWSEnumProtocols((LPWSAPROTOCOL_INFO)pBuf, 
							 &dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		free(pBuf);
		return SOCKET_ERROR;
	}

	//
	// Helper macros for selecting protocols
	//
	#define REJECTSET(f) \
	    ((dwSetFlags & f) && !(pInfo->dwServiceFlags1 & f))
	#define REJECTNOTSET(f) \
	    ((dwNotSetFlags &f) && (pInfo->dwServiceFlags1 & f))
	#define REJECTEDBY(f) (REJECTSET(f) || REJECTNOTSET(f))

	//
	// Loop through the protocols making selections
	//
	pInfo = (LPWSAPROTOCOL_INFO)pBuf;	
	pRetInfo = lpProtocolBuffer;
	dwRetLen = 0;
	nMatchCount = 0;
	for(nCount = 0; nCount < nRet; nCount++)
	{
		//
		// Check all of the requested flags
		//
		while(1)
		{
			if (REJECTEDBY(XP1_CONNECTIONLESS))
				break;
			if (REJECTEDBY(XP1_GUARANTEED_DELIVERY))
				break;
			if (REJECTEDBY(XP1_GUARANTEED_ORDER))
				break;
			if (REJECTEDBY(XP1_MESSAGE_ORIENTED))
				break;
			if (REJECTEDBY(XP1_PSEUDO_STREAM))
				break;
			if (REJECTEDBY(XP1_GRACEFUL_CLOSE))
				break;
			if (REJECTEDBY(XP1_EXPEDITED_DATA))
				break;
			if (REJECTEDBY(XP1_CONNECT_DATA))
				break;
			if (REJECTEDBY(XP1_DISCONNECT_DATA))
				break;
			if (REJECTEDBY(XP1_SUPPORT_BROADCAST)) 
				break;
			if (REJECTEDBY(XP1_SUPPORT_MULTIPOINT))
				break;
			if (REJECTEDBY(XP1_MULTIPOINT_DATA_PLANE))
				break;
			if (REJECTEDBY(XP1_QOS_SUPPORTED))
				break;
			if (REJECTEDBY(XP1_UNI_SEND))
				break;
			if (REJECTEDBY(XP1_UNI_RECV))
				break;
			if (REJECTEDBY(XP1_IFS_HANDLES))
				break;
			if (REJECTEDBY(XP1_PARTIAL_MESSAGE))
				break;
			//
			// If we made it here, 
			//the protocol meets all requirements
			//
			dwRetLen += sizeof(WSAPROTOCOL_INFO);
			if (dwRetLen > *lpdwBufferLength)
			{
				// The supplied buffer is too small
				p_WSASetLastError(WSAENOBUFS);
				*lpdwBufferLength = dwNeededLen;
				free(pBuf);
				return SOCKET_ERROR;
			}
			nMatchCount++;
			// Copy this protocol to the caller's buffer
			memcpy(pRetInfo, pInfo, sizeof(WSAPROTOCOL_INFO));
			pRetInfo++;
			break;
		}
		pInfo++;
	}
	free(pBuf);
	*lpdwBufferLength = dwRetLen;
	return(nMatchCount);
}

////////////////////////////////////////////////////////////

BOOL DWSDnsOnly(void)
{
	//
	// Return TRUE if DNS is the only name space provider
	//

	LPWSANAMESPACE_INFO pInfo;
	DWORD				dwBufLen;
	PBYTE				pBuf;
	int					nCount;
	BOOL				fRet = TRUE;
	int					nRet;

	//
	// If we're running on WinSock 1.1 return TRUE
	//
	if (nVersion == 1)
		return fRet;

	// WinSock 2

	//
	// Intentionally generate an error
	// to get the required buffer size
	//
	dwBufLen = 0;
	nRet = p_WSAEnumNameSpaceProviders(&dwBufLen, NULL);
	if (nRet == SOCKET_ERROR)
	{
		if (p_WSAGetLastError() != WSAEFAULT)
			return fRet;
	}

	//
	// dwBufLen now equals needed size
	//
	pBuf = malloc(dwBufLen+1);
	if (pBuf == NULL)
		return fRet;

	//
	// Now, make the "real" call
	//
	nRet = p_WSAEnumNameSpaceProviders(&dwBufLen,
									   (LPWSANAMESPACE_INFO)pBuf);
	if (nRet == SOCKET_ERROR)
	{
		free(pBuf);
		return fRet;
	}

	pInfo = (LPWSANAMESPACE_INFO)pBuf;
	for (nCount = 0; nCount < nRet; nCount++)
	{
		if (pInfo->dwNameSpace != NS_DNS)
		{
			fRet = FALSE;
			break;
		}
		pInfo++;
	}
	free(pBuf);
	return fRet;
}

////////////////////////////////////////////////////////////

char NibbleToHex(BYTE b)
{
    if (b < 10)
		return (b + '0');

    return (b - 10 + 'A');
}

void BinToHex(PBYTE pBytes, int nNbrBytes, LPSTR lpStr)
{
	BYTE b;
    while(nNbrBytes--)
    {
		// High order nibble first
		b = (*pBytes >> 4);
		*lpStr = NibbleToHex(b);
		lpStr++;
		// Then low order nibble
		b = (*pBytes & 0x0F);
		*lpStr = NibbleToHex(b);
		lpStr++;
		pBytes++;
    }
    *lpStr = '\0';
}

////////////////////////////////////////////////////////////

//
// Workaround for WSAAddressToString()/IPX bug
//
int IPXAddressToString(LPSOCKADDR_IPX pAddr, 
					   DWORD dwAddrLen,
					   LPTSTR lpAddrStr,
					   LPDWORD pdwStrLen)
{
	char szAddr[32];
	char szTmp[20];
	char *cp = szAddr;

	//
	// Check destination length
	//
	if (*pdwStrLen < 27)
	{
		p_WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	//
	// Convert network number
	//
    BinToHex((PBYTE)&pAddr->sa_netnum, 4, szTmp);
	strcpy(szAddr, szTmp);
    strcat(szAddr, ".");
    
	// Node Number
    BinToHex((PBYTE)&pAddr->sa_nodenum, 6, szTmp);
    strcat(szAddr, szTmp);
    strcat(szAddr, ".");

	// IPX Address Socket number
    BinToHex((PBYTE)&pAddr->sa_socket, 2, szTmp);
    strcat(szAddr, szTmp);

	strcpy(lpAddrStr, szAddr);
}

