//
// ECHOSERV.C	Dynamically linked ECHO Server
//
//				Dynamically links to WS2_32.DLL or
//				if WinSock 2 isn't available, it
//				dynamically links to WSOCK32.DLL.
//
//				Protocol Independent under WinSock 2
//				Uses UDP on WinSock 1.1
//
//

#include "echoserv.h"

SOCKET	listenSocket;

//
// Pull in global function pointers as extern
//
#define DWINSOCK_EXTERN
#include "..\dwnsock1.inc"
#include "..\dwnsock2.inc"

HWND	ghwnd;
UINT	guAsyMsg;
UINT	guAppMsg;

////////////////////////////////////////////////////////////

void HandleAsyncMsg(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);

	switch(WSAGETSELECTEVENT(lParam))
	{
		case FD_READ:
			RecvAndEcho((SOCKET)wParam, nErrorCode);
			break;
	}
}

BOOL StartServer(HWND hwnd, UINT uAsyMsg, UINT uAppMsg)
{
	LPSOCKETENTRY	lpEntry;

	//
	// Save HWND and message for later use
	//
	ghwnd	 = hwnd;
	guAsyMsg = uAsyMsg;
	guAppMsg = uAppMsg;

	LogEvent(hwnd, _TEXT("ECHO Server started using WinSock %d"),
					DWSVersion());

	//
	// If there are Name Space providers other than
	// DNS, then install service class if not already installed
	//
	if (!DWSDnsOnly())
		InstallClass();

	//
	// Open a listening socket for each appropriate protocol
	//
	if (!OpenListeners(hwnd, uAsyMsg))
		return FALSE;

	//
	// If there are Name Space providers other than
	// DNS, then advertise that we're starting
	//
	if (!DWSDnsOnly())
		SetEchoService(RNRSERVICE_REGISTER);

	//
	// Add listening socket descriptions to the listbox
	//
	lpEntry = GetFirstSocketEntry();
	while(lpEntry != NULL)
	{
		LogEvent(ghwnd, _TEXT("Listening on %s"),
				 lpEntry->Info.szProtocol);
		lpEntry = GetNextSocketEntry(lpEntry);
	}
	return TRUE;
}

void StopServer(void)
{

	//
	// If there are Name Space providers other than
	// DNS, then notify them that we're stopping
	//
	if (!DWSDnsOnly())
		SetEchoService(RNRSERVICE_DEREGISTER);
	CloseListeners();
	LogEvent(ghwnd, _TEXT("Socket Adapter ECHO Server stopped"));
}

////////////////////////////////////////////////////////////

void LogEvent(HWND hwnd, LPCTSTR lpFormat, ...)
{
	va_list Marker;
	TCHAR szBuf[1024];

	//	
	// Write text to string
	// and send to user interface
	//
	va_start(Marker, lpFormat);
	_vstprintf(szBuf, lpFormat, Marker);
	va_end(Marker);
	SendMessage(ghwnd,
				guAppMsg,
				ECHO_EVENT_MSG,
				(LPARAM)szBuf);
}

////////////////////////////////////////////////////////////

void LogWinSockError(HWND hwnd, LPCTSTR lpText, int nErrorCode)
{
	TCHAR szBuf[256];

	szBuf[0] = '\0';
	LoadString(GetWindowInstance(hwnd),
			   nErrorCode,
			   szBuf,
			   sizeof(szBuf)/sizeof(TCHAR));
	LogEvent(hwnd, _TEXT("%s : %s"), lpText, szBuf);
}

////////////////////////////////////////////////////////////

BOOL OpenListeners(HWND hwnd, UINT uAsyMsg)
{
	LPSOCKETENTRY		lpEntry;
	LPWSAPROTOCOL_INFO	pInfo;
	LPBYTE				pProtocolBuf = NULL;
	SOCKET				socket;
	TCHAR				szBuf[256];
	DWORD				dwNeededLen;
	int					nCount;
	int					nSockets;
	int					nRet;

	//
	// Find all connectionless, message-oriented protocols
	// First, determine size for protocol buffer
	//
	dwNeededLen = 0;
	nRet = DWSSelectProtocols(XP1_CONNECTIONLESS | 
							  XP1_MESSAGE_ORIENTED,
							  0,
							  NULL,		
							  &dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		if (p_WSAGetLastError() != WSAENOBUFS)
		{
			LogWinSockError(hwnd, 
			 			    _TEXT("EnumProtocols 1"),
							p_WSAGetLastError());
			return FALSE;
		}
	}

	//
	// Allocate the buffer
	//
	pProtocolBuf = malloc(dwNeededLen);
	if (pProtocolBuf == NULL)
	{
		MessageBox(hwnd, 
				   _TEXT("Insufficient memory"),
				   _TEXT("OpenListeners()"), 
				   MB_OK);
		return FALSE;
	}

	//
	// Make the "real" call
	//
	nRet = DWSSelectProtocols(XP1_CONNECTIONLESS | 
							  XP1_MESSAGE_ORIENTED,
							  0,
							  (LPWSAPROTOCOL_INFO)pProtocolBuf,
							  &dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(hwnd, 
						 _TEXT("EnumProtocols 2"),
						 p_WSAGetLastError());
		return FALSE;
	}

	//
	// Loop through the returned protocols 
	// creating socket descriptors and adding 
	// descriptions to the linked list
	//
	pInfo      = (LPWSAPROTOCOL_INFO)pProtocolBuf;	
	nSockets   = nRet;
	for(nCount = 0; nCount < nSockets; nCount++)
	{
		socket = p_socket(pInfo->iAddressFamily,
			 		     pInfo->iSocketType,
						 pInfo->iProtocol);
		if (socket == INVALID_SOCKET)
		{
			wsprintf(szBuf, 
					 _TEXT("socket() failed on %s"),
					 pInfo->szProtocol);
			LogWinSockError(hwnd, 
						 szBuf,
						 p_WSAGetLastError());
			continue;
		}

		//
		// Add the socket to our list
		//
		lpEntry = AddSocketEntry(socket, pInfo);
		if (lpEntry == NULL)
		{
			MessageBox(hwnd, 
					   _TEXT("Error adding socket to list"),
					   _TEXT("OpenListeners()"), 
					   MB_OK);
			return FALSE;
		}

		//
		// Request async notification
		//
		nRet = p_WSAAsyncSelect(lpEntry->Socket,
								ghwnd, 
								uAsyMsg,
								FD_READ);
		if (nRet == SOCKET_ERROR)
		{
			wsprintf(szBuf,
					 _TEXT("WSAAsyncSelect() on %s"),
					 pInfo->szProtocol);
			LogWinSockError(hwnd, szBuf, p_WSAGetLastError());
			p_closesocket(lpEntry->Socket);
			DelSocketEntry(lpEntry);
			continue;
		}
		

		//
		// Fill our local address so that we can bind()
		//
		if (!FillLocalAddress(lpEntry->Socket, 
							  lpEntry->lpSockAddr, 
							  pInfo))
		{
			LogEvent(hwnd, 
					 _TEXT("Could not fill local address for %s"),
					 pInfo->szProtocol);
			p_closesocket(lpEntry->Socket);
			DelSocketEntry(lpEntry);
			continue;
			
		}

		//
		// bind local name to the socket
		//
		nRet = p_bind(lpEntry->Socket,
					  lpEntry->lpSockAddr,
					  lpEntry->Info.iMaxSockAddr);
		if (nRet == SOCKET_ERROR)
		{
			wsprintf(szBuf,
					 _TEXT("bind() error on %s"),
					 lpEntry->Info.szProtocol);
			LogWinSockError(ghwnd,
						    szBuf,
						    p_WSAGetLastError());
			p_closesocket(lpEntry->Socket);
			DelSocketEntry(lpEntry);
			return FALSE;
		}
		pInfo++;
	}
	//
	// Any sockets in the list?
	//
	lpEntry = GetFirstSocketEntry();
	if (lpEntry == NULL)
		return FALSE;
	else
		return TRUE;
}

void CloseListeners(void)
{
	LPSOCKETENTRY lpEntry;

	lpEntry = GetFirstSocketEntry();
	while(lpEntry != NULL)
	{
		p_closesocket(lpEntry->Socket);
		lpEntry = GetNextSocketEntry(lpEntry);
	}
	DelAllSocketEntries();
}

void RecvAndEcho(SOCKET socket, int nErrorCode)
{
	LPSOCKETENTRY	lpEntry;
	BYTE		buf[2048];
	TCHAR		szAddrStr[256];
	DWORD		dwStrLen;
	int			nAddrLen;
	int			nBytes;
	int			nRet;
	STATS		stats;

	//
	// Retrieve the SOCKETENTRY
	//
	lpEntry = GetSocketEntry(socket);
	if (lpEntry == NULL)
	{
		LogEvent(ghwnd,
				 _TEXT("RecvAndEcho(): Entry not found for socket %d"),
				  socket);
		return;
	}	

	//
	// Receive the data from the client
	//
	nAddrLen = lpEntry->Info.iMaxSockAddr;
	nRet = p_recvfrom(socket,
					buf,
					sizeof(buf),
					0,
					lpEntry->lpSockAddr,
					&nAddrLen);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd,
						 _TEXT("recvfrom() error"),
						 p_WSAGetLastError());
		return;
	}

	nBytes = nRet;

	//
	// Echo the same data back
	//
	nRet = p_sendto(socket,
				  buf,
				  nBytes,
				  0,
				  lpEntry->lpSockAddr,
				  nAddrLen);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd,
						 _TEXT("sendto() error"),
						 p_WSAGetLastError());
		return;
	}

	//
	// Convert the address to a display string
	//
	dwStrLen = sizeof(szAddrStr) / sizeof(TCHAR);
	nRet = DWSAddressToString(lpEntry->lpSockAddr,
							  nAddrLen,
							  szAddrStr,
							  &dwStrLen);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd, 
						_TEXT("AddressToString()"),
						p_WSAGetLastError());
	}
	LogEvent(ghwnd, 
			 _TEXT("%d bytes echoed to %s using %s"), 
			 nBytes,
			 szAddrStr,
			 lpEntry->Info.szProtocol);
	//
	// Update the statistics
	//
	stats.dwDatagrams = 1;
	stats.dwRecv	  = nBytes;
	stats.dwSend	  = nBytes;
	stats.pInfo		  = &lpEntry->Info;
	SendMessage(ghwnd,
				guAppMsg,
				ECHO_STATS_MSG,
				(LPARAM)&stats);
}


////////////////////////////////////////////////////////////

void InstallClass(void)
{
	WSASERVICECLASSINFO sci;
	TCHAR				szBuf[256];
	DWORD				dwLen;
	WSANSCLASSINFO      nsciArray[4];
	DWORD				dwZero = 0;
	DWORD               dwPort;
	DWORD               dwSapId = AE_SAPID;
	LPSERVENT			lpServEnt;
	int                 nRet;

	//
	// If we're using WinSock 1.1,
	// we can't do this
	//
	if (DWSVersion() == 1)
		return;

	//
	// Has the class already been installed?
	//
	dwLen = sizeof(szBuf) / sizeof(TCHAR);
	nRet = p_WSAGetServiceClassNameByClassId(&guidAdapterEcho,
											 szBuf,
											 &dwLen);
	if (nRet != SOCKET_ERROR)
		return;

	//
	// Check for specific error
	//
	if (p_WSAGetLastError() != WSASERVICE_NOT_FOUND)
	{
		LogWinSockError(ghwnd, 
						_TEXT("Error retrieving class name"),
						p_WSAGetLastError());
		//return;
	}


	//
	// Service class not found, install it
	//

	//
	// Fill in service class info
	//
	sci.lpServiceClassId = &guidAdapterEcho;
	sci.lpszServiceClassName = AE_SERVICECLASSNAME;
	sci.dwCount = 4;
	sci.lpClassInfos = nsciArray;

	//
	// DNS setup
	//

	// Connectionless
	nsciArray[0].lpszName = SERVICE_TYPE_VALUE_CONN;
	nsciArray[0].dwNameSpace = NS_DNS;
	nsciArray[0].dwValueType = REG_DWORD;
	nsciArray[0].dwValueSize = sizeof(DWORD);
	nsciArray[0].lpValue     = &dwZero;

	// Port number 
	lpServEnt = p_getservbyname("echo", "udp");
	if (lpServEnt != NULL)
		dwPort = p_ntohs(lpServEnt->s_port);
	else
		dwPort = AE_UPDPORT;
	nsciArray[1].lpszName = SERVICE_TYPE_VALUE_UDPPORT;
	nsciArray[1].dwNameSpace = NS_DNS;
	nsciArray[1].dwValueType = REG_DWORD;
	nsciArray[1].dwValueSize = sizeof(DWORD);
	nsciArray[1].lpValue     = &dwPort;


	//
	// Novell SAP setup
	//

	// Connectionless
	nsciArray[2].lpszName = SERVICE_TYPE_VALUE_CONN;
	nsciArray[2].dwNameSpace = NS_SAP;
	nsciArray[2].dwValueType = REG_DWORD;
	nsciArray[2].dwValueSize = sizeof(DWORD);
	nsciArray[2].lpValue     = &dwZero;
	// SAP ID 
	nsciArray[3].lpszName = SERVICE_TYPE_VALUE_SAPID;
	nsciArray[3].dwNameSpace = NS_SAP;
	nsciArray[3].dwValueType = REG_DWORD;
	nsciArray[3].dwValueSize = sizeof(DWORD);
	nsciArray[3].lpValue     = &dwSapId;

	//
	// Add parameters for other name spaces here
	//

	//
	// Install the new class
	//
	nRet = p_WSAInstallServiceClass(&sci);
	if (nRet == SOCKET_ERROR)
	{
		LogWinSockError(ghwnd, 
						_TEXT("Could not install service class"),
						p_WSAGetLastError());
	}
}


void SetEchoService(WSAESETSERVICEOP essOp)
{
	LPBYTE			lpcsaBuffer;
	LPBYTE			lpAddrBuffer;
	LPSOCKETENTRY	lpEntry;
	LPCSADDR_INFO	lpCsa;
	LPSOCKADDR		lpAddr;
	WSAQUERYSET		qs;
	WSAVERSION		Version;
	int				nNbrAddresses;
	int				nTotalAddrSize;
	int				nRet;

	//
	// If we're using WinSock 1.1,
	// we can't do this
	//
	if (DWSVersion() == 1)
		return;

	//
	// If we don't have any sockets,
	// we don't have anything to advertise
	//
	lpEntry = GetFirstSocketEntry();
	if (lpEntry == NULL)
		return;

	//
	// Count the number of addresses we
	// have available and the total space
	// needed to hold all SOCKADDRs
	//
	nNbrAddresses  = 0;
	nTotalAddrSize = 0;
	while(lpEntry != NULL)
	{
		nNbrAddresses++;
		nTotalAddrSize += lpEntry->Info.iMaxSockAddr;
		lpEntry = GetNextSocketEntry(lpEntry);
	}

	//
	// Allocate a buffer for the CSADDR_INFO structures
	//
	lpcsaBuffer = malloc(sizeof(CSADDR_INFO)*nNbrAddresses);
	if (lpcsaBuffer == NULL)
	{
		LogEvent(ghwnd, 
				 _TEXT("Advertise(): No memory for CSADDR_INFO"));
		return;
	}
	lpCsa = (LPCSADDR_INFO)lpcsaBuffer;

	//
	// Allocate a buffer for a copy of all the SOCKADDRs
	//
	lpAddrBuffer = malloc(nTotalAddrSize);
	if (lpAddrBuffer == NULL)
	{
		LogEvent(ghwnd, 
				 _TEXT("Advertise(): No memory for SOCKADDRs"));
		free(lpcsaBuffer);
		return;
	}
	lpAddr = (LPSOCKADDR)lpAddrBuffer;

	// 
	// Loop through our addresses, filling
	// in the CSADDR_INFO structures
	//
	lpEntry = GetFirstSocketEntry();
	while(lpEntry != NULL)
	{
		lpCsa->iProtocol   = lpEntry->Info.iProtocol;
		lpCsa->iSocketType = lpEntry->Info.iSocketType;

		// Copy the SOCKADDR
		memcpy(lpAddr, 
			   lpEntry->lpSockAddr,
			   lpEntry->Info.iMaxSockAddr);

		lpCsa->LocalAddr.iSockaddrLength  = lpEntry->Info.iMaxSockAddr;
		lpCsa->RemoteAddr.iSockaddrLength = lpEntry->Info.iMaxSockAddr;

		lpCsa->LocalAddr.lpSockaddr  = lpAddr;
		lpCsa->RemoteAddr.lpSockaddr = lpAddr;

		//
		// Move to next entries in buffers
		//
		lpAddr += lpEntry->Info.iMaxSockAddr;
		lpCsa++;
		lpEntry = GetNextSocketEntry(lpEntry);
	}

	// 
	// Register service for availability
	//
	memset(&qs, 0, sizeof(WSAQUERYSET));
	qs.dwSize					= sizeof(WSAQUERYSET);
	qs.lpszServiceInstanceName	= AE_INSTANCENAME;
	qs.lpServiceClassId			= &guidAdapterEcho;
	qs.lpVersion				= &Version;
			Version.dwVersion	= 1;
			//Version.ecHow		= COMP_NOTLESS;
	qs.dwNameSpace				= NS_ALL;
	qs.dwNumberOfCsAddrs		= nNbrAddresses;
	qs.lpcsaBuffer				= (LPCSADDR_INFO)lpcsaBuffer;

	nRet = p_WSASetService(&qs,
						    essOp,
						    SERVICE_MULTIPLE);
	if (nRet == SOCKET_ERROR)
	{
		nRet = p_WSAGetLastError();
		LogWinSockError(ghwnd,
						_TEXT("SetService()"),
						p_WSAGetLastError());
	}
	free(lpcsaBuffer);
	free(lpAddrBuffer);
}

////////////////////////////////////////////////////////////

BOOL FillLocalAddress(SOCKET socket,
					  LPSOCKADDR lpAddr, 
					  LPWSAPROTOCOL_INFO lpInfo)
{
	LPSERVENT		lpServEnt;
	LPSOCKADDR_IN	pInAddr;
	LPSOCKADDR_IPX	pIpxAddr;

	//
	// Fill in local address based on address family
	//
	switch (lpInfo->iAddressFamily)
	{
		case AF_INET:
			pInAddr = (LPSOCKADDR_IN)lpAddr;
			pInAddr->sin_family = AF_INET;
			pInAddr->sin_addr.s_addr = INADDR_ANY;
			//
			// Find a port number
			//
			lpServEnt = p_getservbyname("echo", "udp");
			if (lpServEnt != NULL)
				pInAddr->sin_port = lpServEnt->s_port;
			else
				// Default to well-known UDP ECHO port
				pInAddr->sin_port = p_htons(AE_UPDPORT);
			return TRUE;

		case AF_IPX:
			pIpxAddr = (LPSOCKADDR_IPX)lpAddr;
			memset(pIpxAddr->sa_netnum, 0, sizeof(pIpxAddr->sa_netnum));
			memset(pIpxAddr->sa_nodenum, 0, sizeof(pIpxAddr->sa_nodenum));
			pIpxAddr->sa_family = AF_IPX;
			pIpxAddr->sa_socket = p_htons(AE_IPXPORT);
			return TRUE;

		//
		// Fill in known values for other 
		// address families here.
		//

		default:
			memset(lpAddr, 0, lpInfo->iMaxSockAddr);
			lpAddr->sa_family = lpInfo->iAddressFamily;
			return TRUE;
	}
	return FALSE;
}

