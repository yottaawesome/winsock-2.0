//
// ECHOCLNT.C	Datagram ECHO Client
//
//				Dynamically links to WS2_32.DLL or
//				if WinSock 2 isn't available, it
//				dynamically links to WSOCK32.DLL.
//
//				Protocol Independent under WinSock 2
//				Uses UDP on WinSock 1.1
//
//				Do NOT link to either 
//				WS2_32.LIB or WSOCK32.LIB
//

#include "echoclnt.h"
#include "..\dwinsock.h"
#include "..\aeguid.h"

TCHAR	gszAppName[]  = _TEXT("SocketAdapter ECHO Client");
HWND	ghwnd;
LPBYTE  gpProtocolBuf = NULL;
LPBYTE	gpSockets	  = NULL;
int		gNbrSockets   = 0;

//
// Pull in global function pointers as extern
//
#define DWINSOCK_EXTERN
#include "..\dwnsock1.inc"
#include "..\dwnsock2.inc"

////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	int nVersion;
		
	//
	// Dynamically link to newest available WinSock
	//
	nVersion = DWSInitWinSock();
	if (!nVersion)
	{
		MessageBox(NULL, 
				   _TEXT("No WinSock Available"),
				   gszAppName, 
				   MB_OK);
		return 0;
	}

	//
	// Use a dialog as a main window
	//
	DialogBox(hInstance,
			  MAKEINTRESOURCE(IDD_MAINWND),
			  NULL, 
			  MainWndProc);

	//
	// Free dynamically linked WinSock DLL
	//
	DWSFreeWinSock();
	return(0);
}

////////////////////////////////////////////////////////////

BOOL CALLBACK MainWndProc(HWND hwnd, 
						  UINT uMsg,
						  WPARAM wParam, 
						  LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_DLG_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_DLG_MSG(hwnd, WM_COMMAND,	OnCommand);

		case UM_ASYNC:  
			HandleAsyncMsg(hwnd, wParam, lParam);
			return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	ghwnd = hwnd;
	//
	// Associate an icon with the dialog box.
	//
	if (DWSVersion() == 1)
		SetClassLong(hwnd, 
					 GCL_HICON, 
					 (LONG)LoadIcon(GetWindowInstance(hwnd),
					 MAKEINTRESOURCE(IDI_ICON1)));
	else
		SetClassLong(hwnd, 
					 GCL_HICON, 
					 (LONG)LoadIcon(GetWindowInstance(hwnd),
					 MAKEINTRESOURCE(IDI_ICON2)));

	FillSocketChoices(hwnd);
	return(TRUE);
}

////////////////////////////////////////////////////////////

void OnCommand(HWND hwnd, int nId, HWND hwndCtl, UINT codeNotify) 
{
	SOCKET	*pSock;
	int		nCount;
	int		nRet;

	switch (nId) 
	{
		case IDC_SEND:
			SendEcho(hwnd);
			break;

		case IDCANCEL:
			//
			// Close all our sockets
			//
			pSock = (SOCKET *)gpSockets;
			for (nCount = 0; nCount < gNbrSockets; nCount++)
			{
				nRet = p_closesocket(*pSock);
				if (nRet == SOCKET_ERROR)
					ShowWinSockError(hwnd,
									 _TEXT("closesocket()"),
									 p_WSAGetLastError());
				pSock++;

			}
			if (gpProtocolBuf != NULL)
				free(gpProtocolBuf);
			if (gpSockets != NULL)
				free(gpSockets);
			EndDialog(hwnd, 0);
			break;
	}
}

////////////////////////////////////////////////////////////

void HandleAsyncMsg(HWND hwnd, 
					WPARAM wParam, 
					LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);

	switch(WSAGETSELECTEVENT(lParam))
	{
		case FD_READ:
			RecvEcho(hwnd, (SOCKET)wParam, nErrorCode);
			break;
	}
}

////////////////////////////////////////////////////////////

void SendEcho(HWND hwnd)
{
	LPWSAPROTOCOL_INFO	lpInfo;
	TCHAR		szHostName[256];
	ECHOREQ		echoReq;
	SOCKET		*pSock;
	SOCKADDR	saHost;
	HWND		hwndList;
	HWND		hwndEdit;
	int			nIndex;
	int			nRet;

	//
	// Get protocol selection and 
	// associated socket descriptor
	//
	hwndList = GetDlgItem(hwnd, IDC_SOCKETS);
	nIndex = ListBox_GetCurSel(hwndList);
	if (nIndex == LB_ERR)
	{
		MessageBox(hwnd, 
				   _TEXT("Please select a socket"),
				   gszAppName, MB_OK);
		return;
	}
	if (nIndex > gNbrSockets)
	{
		MessageBox(hwnd, 
				   _TEXT("Internal error"),
				   gszAppName, MB_OK);
		return;
	}

	lpInfo = (LPWSAPROTOCOL_INFO)gpProtocolBuf;
	lpInfo += nIndex;
	pSock = (SOCKET *)gpSockets;
	pSock  += nIndex;

	hwndEdit = GetDlgItem(hwnd, IDC_HOSTNAME);
	GetWindowText(hwndEdit, szHostName, 
				  sizeof(szHostName) / sizeof(TCHAR));
	//
	// AF_INET family MUST have a host name
	//
	if (lpInfo->iAddressFamily == AF_INET)
	{
		if (!_tcslen(szHostName))
		{
			MessageBox(hwnd, 
					   _TEXT("AF_INET Family requires a host name"),
					   _TEXT("Pleae enter a host name"), 
					   MB_OK);
			SetFocus(hwndEdit);
			return;
		}
	}

	//
	// Find the host for this address family
	//
	if (!FindEchoServer(*pSock,
						lpInfo, 
						&saHost, 
						sizeof(SOCKADDR), 
						szHostName))
	{
		MessageBox(hwnd, 
				   _TEXT("Host not found for this socket"),
				   gszAppName, MB_OK);
		return;
	}

	//
	// Send an ECHO request
	//
	FillEchoRequest(&echoReq);
	nRet = p_sendto(*pSock,
				   (char *)&echoReq,
				   sizeof(ECHOREQ),
				   0,
				   &saHost,
				   sizeof(SOCKADDR));
	if (nRet == SOCKET_ERROR)
		ShowWinSockError(hwnd, 
						 _TEXT("sendto()"),
						 p_WSAGetLastError());
}

////////////////////////////////////////////////////////////

void FillSocketChoices(HWND hwnd)
{
	LPWSAPROTOCOL_INFO	pInfo;
	SOCKET				*pSock;
	HWND				hwndList;
	DWORD				dwNeededLen;
	TCHAR				szBuf[256];
	int					nCount;
	int					nRet;
	BYTE				bufAddress[80];
	LPSOCKADDR			pAddr;

	//
	// Determine size for protocol buffer
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
			ShowWinSockError(hwnd, 
							 _TEXT("EnumProtocols 1"),
							 p_WSAGetLastError());
			return;
		}
	}

	//
	// Allocate the buffer
	//
	gpProtocolBuf = malloc(dwNeededLen);
	if (gpProtocolBuf == NULL)
	{
		MessageBox(hwnd, 
				   _TEXT("Insufficient memory"),
				   gszAppName, MB_OK);
		return;
	}

	//
	// Make the "real" call
	//
	nRet = DWSSelectProtocols(XP1_CONNECTIONLESS | 
							  XP1_MESSAGE_ORIENTED,
							  0,
							  (LPWSAPROTOCOL_INFO)gpProtocolBuf,
							  &dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		ShowWinSockError(hwnd, 
						 _TEXT("EnumProtocols 2"),
						 p_WSAGetLastError());
		return;
	}

	//
	// Allocate a buffer for socket descriptors
	//
	gpSockets = malloc(sizeof(SOCKET)*nRet);
	if (gpSockets == NULL)
	{
		MessageBox(hwnd, 
				   _TEXT("Insufficient memory"),
				   gszAppName, MB_OK);
		return;
	}

	//
	// Loop through the returned protocols 
	// creating socket descriptors and adding 
	// descriptions to the listbox.
	//
	hwndList   = GetDlgItem(hwnd, IDC_SOCKETS);
	pInfo      = (LPWSAPROTOCOL_INFO)gpProtocolBuf;	
	pSock      = (SOCKET *)gpSockets;
	gNbrSockets= nRet;
	for(nCount = 0; nCount < gNbrSockets; nCount++)
	{
		*pSock = p_socket(pInfo->iAddressFamily,
			 		      pInfo->iSocketType,
						  pInfo->iProtocol);
		if (*pSock == INVALID_SOCKET)
		{
			ShowWinSockError(hwnd, 
						 _TEXT("socket()"),
						 p_WSAGetLastError());
			gNbrSockets = nCount-1;
			return;
		}
		memset(bufAddress, 0, sizeof(bufAddress));
		pAddr = (LPSOCKADDR)bufAddress;
		pAddr->sa_family = pInfo->iAddressFamily;
		nRet = p_bind(*pSock, pAddr, pInfo->iMaxSockAddr);
		if (nRet == SOCKET_ERROR)
		{
			ShowWinSockError(hwnd, 
							 _TEXT("bind()"),
							 p_WSAGetLastError());
			return;
		}
		//
		// Request async notification
		//
		nRet = p_WSAAsyncSelect(*pSock,
							   hwnd,
							   UM_ASYNC,
							   FD_READ);
		if (nRet == SOCKET_ERROR)
		{
			ShowWinSockError(hwnd, 
							 _TEXT("WSAAsyncSelect()"),
							 p_WSAGetLastError());
			return;
		}
		wsprintf(szBuf, 
				 _TEXT(" %s - socket(%d, %d, %d)"),
				 pInfo->szProtocol,
				 pInfo->iAddressFamily,
				 pInfo->iSocketType,
				 pInfo->iProtocol);
		ListBox_AddString(hwndList, szBuf);
		pInfo++;
		pSock++;
	}
}

////////////////////////////////////////////////////////////

void FillEchoRequest(LPECHOREQ pechoReq)
{
	static dwSequence = 0;
	int nCount;

	for (nCount = 0; nCount < ECHODATASIZE; nCount++)
		pechoReq->nData[nCount] = nCount;
	pechoReq->dwSeq = dwSequence++;
	pechoReq->dwSentTime = GetTickCount();
}

////////////////////////////////////////////////////////////

void RecvEcho(HWND hwnd, SOCKET socket, int nErrorCode)
{
	char		szBuf[256];
	HWND		hwndList;
	ECHOREQ		echoReq;
	SOCKADDR	saFrom;
	DWORD		dwElapsed;
	int			nFromLen;
	int			nCount;
	int			nRet;

	//
	// Receive the data from the server
	//	
	nRet = p_recvfrom(socket,
					 (char *)&echoReq,
					 sizeof(echoReq),
					 0,
					 &saFrom,
					 &nFromLen);
	if (nRet == SOCKET_ERROR)
	{
		ShowWinSockError(hwnd, 
						 _TEXT("recvfrom()"),
						 p_WSAGetLastError());
		return;
	}

	//
	// Check the request
	//
	for (nCount = 0; nCount < ECHODATASIZE; nCount++)
	{
		if (echoReq.nData[nCount] != nCount)
		{
			MessageBox(NULL, 
					   _TEXT("Received ECHO is corrupted"),
					   gszAppName, 
					   MB_OK);
			return;
		}
	}

	//
	// Add it to the ECHO listbox
	//
	hwndList = GetDlgItem(hwnd, IDC_ECHOS);
	dwElapsed = (GetTickCount() - echoReq.dwSentTime);

	wsprintf(szBuf,
			 _TEXT("Seq: %ld Elapsed: %ld"),
			 echoReq.dwSeq, dwElapsed);
	ListBox_AddString(hwndList, szBuf);
	return;
}

////////////////////////////////////////////////////////////

BOOL FindEchoServer(SOCKET socket, 
					LPWSAPROTOCOL_INFO pInfo,
					LPSOCKADDR pAddr, 
					int nAddrLen,
					LPTSTR lpHostName)
{
	char			szHostName[256];
	WSAQUERYSET		qs;
	LPHOSTENT		lpHost;
	LPSERVENT		lpServ;
	LPSOCKADDR_IN	pInAddr; 
	IN_ADDR			iaHost;
	DWORD			dwFlags;
	int				nRet;
    HANDLE			hLookup;
    BYTE			bufResult[2048];
	DWORD			dwLen;
    LPWSAQUERYSET	pqs;
    LPCSADDR_INFO	pcsa;
    GUID guidEchoUdp  = SVCID_ECHO_UDP;

	//
	// If we're using WinSock 1.1 use the old functions
	//
	if (DWSVersion() == 1)
	{
		pInAddr = (LPSOCKADDR_IN)pAddr;

		//
		// If Unicode, convert lpHostName
		//
		#ifdef UNICODE
			WideCharToMultiByte(CP_ACP,
								0,
								lpHostName,
								-1,
								szHostName,
								sizeof(szHostName),
								NULL,
								NULL);
	
		#else
			strcpy(szHostName, lpHostName);
		#endif
		// 
		// Name might actually be an address
		// Use inet_addr to check
		//
		iaHost.s_addr = p_inet_addr(szHostName);
		if (iaHost.s_addr == INADDR_NONE)
		{
			// Wasn't an IP address string, assume it is a name
			lpHost= p_gethostbyname(szHostName);
		}
		else
		{
			// It was a valid IP address string
			lpHost = p_gethostbyaddr((const char *)&iaHost, 
									 sizeof(struct in_addr), 
									 AF_INET);
		}
		if (lpHost == NULL)
			return FALSE;
		//
		// Find the port
		//
		lpServ = p_getservbyname("echo", "udp");
		if (lpServ == NULL)
			pInAddr->sin_port = p_htons(7);
		else
			pInAddr->sin_port = lpServ->s_port;
		pInAddr->sin_family = AF_INET;
		pInAddr->sin_addr = *((LPIN_ADDR)*lpHost->h_addr_list);
		return TRUE;
	}

	//
	// Workaround for IPX until SAP/NDS provider is released
	//
	if (pInfo->iAddressFamily == AF_IPX && DWSDnsOnly())
	{
		// Asumme IPX address as "NetworkNumber.NodeNumber"
		return(FillIpx((LPSOCKADDR_IPX)pAddr, 
						nAddrLen,
						lpHostName));
	}


	//
	// Must be WinSock 2 - Use new service resolution functions
	//

	memset(&qs, 0, sizeof(WSAQUERYSET));
    qs.dwSize                   = sizeof(WSAQUERYSET);
    qs.lpszServiceInstanceName  = lpHostName;
    qs.dwNameSpace              = NS_ALL;

	//
	// If it's an Internet address, assume DNS (non-dynamic)
	// Use special GUID for ECHO on UDP
	//
	// Otherwise, use our GUID from ..\aeguid.h
	//
	if (pInfo->iAddressFamily == AF_INET)
		qs.lpServiceClassId = &guidEchoUdp;
	else
		qs.lpServiceClassId = &guidAdapterEcho;

    //
    // Begin lookup
    //
    dwFlags = LUP_RETURN_NAME|LUP_RETURN_ADDR;
    nRet = p_WSALookupServiceBegin(&qs,
                                   dwFlags, 
                                   &hLookup);
    if (nRet == SOCKET_ERROR)
	{
		ShowWinSockError(ghwnd, 
						 _TEXT("LookupServiceBegin()"),
						 p_WSAGetLastError());
        return FALSE;
	}

    //
    // Use the first one found
    //
	dwFlags = LUP_RETURN_NAME|LUP_RETURN_ADDR;
	dwLen = sizeof(bufResult);
	nRet = p_WSALookupServiceNext(hLookup,
                                 dwFlags, 
                                 &dwLen,
                                 (LPWSAQUERYSET)bufResult);
	if (nRet == SOCKET_ERROR)
	{
		nRet = p_WSAGetLastError();
		if (nRet != WSAENOMORE && nRet != WSA_E_NO_MORE)
			ShowWinSockError(ghwnd, 
						 _TEXT("LookupServiceNext()"),
						 p_WSAGetLastError());
	    p_WSALookupServiceEnd(hLookup);
		return FALSE;
    }

	//
	// Cast the result to a WSAQUERYSET pointer
	//
	pqs = (LPWSAQUERYSET)bufResult;

	//
	// Cast the lpcsaBuffer to a CSADDR_INFO pointer
	//
	pcsa = pqs->lpcsaBuffer;

	//
	// Check the length
	//
	if (pcsa->RemoteAddr.iSockaddrLength > nAddrLen)
	{
		MessageBox(NULL, 
				   _TEXT("SOCKADDR too large"),
				   gszAppName, MB_OK);
	    p_WSALookupServiceEnd(hLookup);
		return FALSE;
	}

	//
	// Copy the SOCKADDR
	//
	memcpy(pAddr, 
		   pcsa->RemoteAddr.lpSockaddr,
		   pcsa->RemoteAddr.iSockaddrLength);
    p_WSALookupServiceEnd(hLookup);
	return TRUE;
}

////////////////////////////////////////////////////////////

void ShowWinSockError(HWND hwnd, 
					  LPCTSTR lpText, 
					  int nErrorCode)
{
	TCHAR szBuf[256];

	szBuf[0] = '\0';
	LoadString(GetWindowInstance(hwnd),
			   nErrorCode,
			   szBuf,
			   sizeof(szBuf)/sizeof(TCHAR));
	MessageBox(hwnd,
			   szBuf,
			   lpText,
			   MB_OK);
}

////////////////////////////////////////////////////////////

BYTE HexToNibble(char ch)
{
	char chHex = toupper(ch);
	if (chHex <= '9')
		return (chHex - '0');
	return(chHex - 'A' + 0xA);
}

void HexToBin(LPSTR lpStr, PBYTE pDest, int nNbrBytes)
{
    while(nNbrBytes--)
    {
		*pDest  = HexToNibble(*lpStr) << 4;
		lpStr++;
		*pDest += HexToNibble(*lpStr);
		lpStr++;
		pDest++;
    }
}

BOOL FillIpx(LPSOCKADDR_IPX pAddr, 
			 int nAddrLen,
			 LPTSTR lpszAddress)
{
	char szAddress[80];
	char cpSeps[] = ".";
	char *cp;

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP,
						0,
						lpszAddress,
						-1,
						szAddress,
						sizeof(szAddress),
						NULL,
						NULL);
	
#else
	strcpy(szAddress, lpszAddress);
#endif

	// 
	// Break out network number from node number
	//
	cp = strtok(szAddress, cpSeps);
	if (cp == NULL)
		return FALSE;
    HexToBin(cp, (char *)&pAddr->sa_netnum, 4);

	cp = strtok(NULL, cpSeps);
	if (cp == NULL)
		return FALSE;
    HexToBin(cp, (char *)&pAddr->sa_nodenum, 6);

	pAddr->sa_socket = p_htons(AE_IPXPORT);
    pAddr->sa_family = AF_IPX;
	return TRUE;
}
