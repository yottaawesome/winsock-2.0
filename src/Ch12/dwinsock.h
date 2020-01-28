//
// DWINSOCK.H	Dynamic WinSock
//
//				Functions for dynamically linking to
//				best available WinSock.
//
//				Dynamically links to WS2_32.DLL or
//				if WinSock 2 isn't available, it
//				dynamically links to WSOCK32.DLL.
//
//

#ifndef DWINSOCK_H
#define DWINSOCK_H

int  DWSInitWinSock(void);
BOOL DWSFreeWinSock(void);
int  DWSVersion(void);
int  DWSMaxMsgSize(SOCKET socket);
int  DWSAddressToString(LPSOCKADDR pAddr, DWORD dwAddrLen,
						LPSTR lpAddrStr, LPDWORD pdwStrLen);
int  DWSEnumProtocols(LPWSAPROTOCOL_INFO lpProtocolBuffer, 
		   			  LPDWORD pdwBufLen);
int  DWSSelectProtocols(DWORD dwSetFlags,
					    DWORD dwNotSetFlags,
					    LPWSAPROTOCOL_INFO lpProtocolBuffer,
					    LPDWORD lpdwBufferLength);
BOOL DWSDnsOnly(void);

//
// Define generic pointer names for both
// ANSI and Wide versions
// 
#ifdef UNICODE
	#define p_WSAAddressToString				p_WSAAddressToStringW 
	#define p_WSAAddressToString				p_WSAAddressToStringW
	#define p_WSADuplicateSocket				p_WSADuplicateSocketW
	#define p_WSAEnumNameSpaceProviders			p_WSAEnumNameSpaceProvidersW
	#define p_WSAEnumProtocols					p_WSAEnumProtocolsW
	#define p_WSAGetServiceClassInfo			p_WSAGetServiceClassInfoW
	#define p_WSAGetServiceClassNameByClassId	p_WSAGetServiceClassNameByClassIdW
	#define p_WSAInstallServiceClass			p_WSAInstallServiceClassW
	#define p_WSALookupServiceBegin				p_WSALookupServiceBeginW
	#define p_WSALookupServiceNext				p_WSALookupServiceNextW
	#define p_WSASetService						p_WSASetServiceW
	#define p_WSASocket							p_WSASocketW
	#define p_WSAStringToAddress				p_WSAStringToAddressW
#else
	#define p_WSAAddressToString				p_WSAAddressToStringA
	#define p_WSAAddressToString				p_WSAAddressToStringA
	#define p_WSADuplicateSocket				p_WSADuplicateSocketA
	#define p_WSAEnumNameSpaceProviders			p_WSAEnumNameSpaceProvidersA
	#define p_WSAEnumProtocols					p_WSAEnumProtocolsA
	#define p_WSAGetServiceClassInfo			p_WSAGetServiceClassInfoA
	#define p_WSAGetServiceClassNameByClassId	p_WSAGetServiceClassNameByClassIdA
	#define p_WSAInstallServiceClass			p_WSAInstallServiceClassA
	#define p_WSALookupServiceBegin				p_WSALookupServiceBeginA
	#define p_WSALookupServiceNext				p_WSALookupServiceNextA
	#define p_WSASetService						p_WSASetServiceA
	#define p_WSASocket							p_WSASocketA
	#define p_WSAStringToAddress				p_WSAStringToAddressA
#endif // UNICODE


#endif // DWINSOCK_H

