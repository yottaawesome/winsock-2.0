// SOCKETX.CPP -- Extension of the CSocket class
//

#include "stdafx.h"
#include "socketx.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSocketX, CSocket)

int CSocketX::Send(LPCTSTR lpszStr, UINT uTimeOut, int nFlags)
{
	// If a timeout value was specified, set it
	if (uTimeOut > 0)
		SetTimeOut(uTimeOut);

	// Call base class function
	int nRet = CSocket::Send(lpszStr, strlen(lpszStr), nFlags);

	// If we previously set a timeout
	if (uTimeOut > 0)
	{
		KillTimeOut();
		// If the operation timedout, set a more
		// natural error message
		if (GetLastError() == WSAEINTR)
			SetLastError(WSAETIMEDOUT);
	}
	return nRet;
}


int CSocketX::Receive(CString& str, UINT uTimeOut, int nFlags)
{
	static char szBuf[256];
	memset(szBuf, 0, sizeof(szBuf));

	// If a timeout value was specified, set it
	if (uTimeOut > 0)
		SetTimeOut(uTimeOut);

	// Call base class function
	int nRet = CSocket::Receive(szBuf, sizeof(szBuf), nFlags);

	// If we previously set a timeout
	if (uTimeOut > 0)
	{
		KillTimeOut();
		// If the operation timedout, set a more
		// natural error message
		if (nRet == SOCKET_ERROR)
		{
			if (GetLastError() == WSAEINTR)
				SetLastError(WSAETIMEDOUT);
		}
	}

	// Fill in the CString reference
	str = szBuf;
	return nRet;
}

BOOL CSocketX::OnMessagePending() 
{
	MSG msg;

	// Watch for our timer message
	if(::PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_NOREMOVE))
	{
		// If our timer expired...
		if (msg.wParam == (UINT) m_nTimerID)
		{
			// Remove the message
			::PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE);
			// And cancel the call
			CancelBlockingCall();
			return FALSE;
		}
	}
	// Call base class function
	return CSocket::OnMessagePending();
} 


BOOL CSocketX::SetTimeOut(UINT uTimeOut) 
{ 
	m_nTimerID = SetTimer(NULL,0,uTimeOut,NULL);
	return m_nTimerID;
} 


BOOL CSocketX::KillTimeOut() 
{ 
	return KillTimer(NULL,m_nTimerID);
} 
