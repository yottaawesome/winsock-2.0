// view.cpp : implementation of the CTermView class
//

#include "stdafx.h"
#include "wsterm.h"

#include "doc.h"
#include "termsock.h"
#include "view.h"
#include "connectd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTermView

IMPLEMENT_DYNCREATE(CTermView, CEditView)

BEGIN_MESSAGE_MAP(CTermView, CEditView)
	//{{AFX_MSG_MAP(CTermView)
	ON_COMMAND(ID_SOCKET_CONNECT, OnSocketConnect)
	ON_UPDATE_COMMAND_UI(ID_SOCKET_CONNECT, OnUpdateSocketConnect)
	ON_COMMAND(ID_SOCKET_CLOSE, OnSocketClose)
	ON_UPDATE_COMMAND_UI(ID_SOCKET_CLOSE, OnUpdateSocketClose)
	ON_WM_CHAR()
	ON_COMMAND(ID_VIEW_SOCKETNOTIFICATIONS, OnViewSocketNotifications)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOCKETNOTIFICATIONS, OnUpdateViewSocketNotifications)
	ON_COMMAND(ID_EDIT_CLEARBUFFER, OnEditClearBuffer)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTermView construction/destruction

CTermView::CTermView()
{
	m_pSocket = NULL;
	m_fConnected = FALSE;
	m_fShowNotifications = FALSE;
}

CTermView::~CTermView()
{
	// If we allocated a socket
	if (m_pSocket != NULL)
	{            
		// If it's still connected
		if (m_fConnected)
			m_pSocket->Close();
		delete m_pSocket;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTermView drawing

void CTermView::OnDraw(CDC* pDC)
{
	CTermDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTermView printing

BOOL CTermView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CEditView preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CTermView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing.
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CTermView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CEditView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CTermView diagnostics

#ifdef _DEBUG
void CTermView::AssertValid() const
{
	CEditView::AssertValid();
}

void CTermView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CTermDoc* CTermView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermDoc)));
	return (CTermDoc*)m_pDocument;
}
#endif //_DEBUG

CTermView *CTermView::GetView()
{
	CFrameWnd *pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView *pView = pFrame->GetActiveView();
	if (!pView)
		return NULL;
	if (!pView->IsKindOf(RUNTIME_CLASS(CTermView)))
		return NULL;
	return (CTermView *)pView;
}

void CTermView::OnEditClearBuffer()
{
	SetWindowText(NULL);
}


// Helper function to display
// lines in the edit control
void CTermView::Display(LPCSTR lpFormat, ...)
{
	// Is the edit control almost full?
	CEdit& ed = GetEditCtrl();
	if (ed.GetLineCount() > 1000)
	{
		// Empty the whole thing
		SetWindowText(NULL);
	}
	
	va_list Marker;
	static char szBuf[256];
	
	// Write text to string
	// and append to edit control
	va_start(Marker, lpFormat);
	vsprintf(szBuf, lpFormat, Marker);
	va_end(Marker);
	ed.SetSel(-1,-1);
	ed.ReplaceSel(szBuf);
}

/////////////////////////////////////////////////////////////////////////////
// CTermView message handlers

// When user presses <ENTER>
// we send the current line
void CTermView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 	// If we're not connected...
	if (!m_fConnected)
	{
		// Ask the user to connect our socket
		AfxMessageBox("Choose Socket/Connect to start\r\n");
		return;
	}   
	// If the user hit <ENTER>
	if (nChar == 13)
	{               
		// Figure out what line we're on
		CEdit& ed = GetEditCtrl();
		int iStart, iEnd;
		ed.GetSel(iStart, iEnd);
		int iLine = ed.LineFromChar(iStart);
		if (iLine > -1)
		{
			static char szLine[256];
			memset(szLine, 0, sizeof(szLine));
			// Get the whole line
			int iNdx = ed.GetLine(iLine, szLine, sizeof(szLine)-1);
			if (iNdx > 0)
			{                    
				// and Send() it out our socket
				strcat(szLine, "\r\n");
				m_pSocket->Send(szLine, strlen(szLine));
			}
		}
	}
	CEditView::OnChar(nChar, nRepCnt, nFlags);
}

void CTermView::OnViewSocketNotifications()
{
	if (m_fShowNotifications)
		m_fShowNotifications = FALSE;
	else
		m_fShowNotifications = TRUE;
}

void CTermView::OnUpdateViewSocketNotifications(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_fShowNotifications);
}


// Socket/Connect Menu handler
void CTermView::OnSocketConnect()
{
	// Display the Connect Dialog
	CConnectDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	Display("Connect to port %d on %s...\r\n", 
			dlg.m_nPort,
			dlg.m_strHostName);

	// Make sure we don't have a socket left
	// from our last connection
	if (m_pSocket != NULL)
	{
		delete m_pSocket;
		m_pSocket = NULL;
	}
			
	// Create a new CTermSocket object on the heap.
	m_pSocket = new CTermSocket();
	
	// And then call the Create() member
	// accepting all defaults
	if (!m_pSocket->Create())
	{
		AfxMessageBox("Socket creation failed");
		return;
	} 
	
	// Connect to the host
	if (!m_pSocket->Connect(dlg.m_strHostName, dlg.m_nPort))
	{   
		if (m_pSocket->GetLastError() != WSAEWOULDBLOCK)
		{                                
			CString strError;
			strError.LoadString(m_pSocket->GetLastError());
			Display("Connect() failed: %s\r\n",
				strError);
			m_fConnected = FALSE;
			delete m_pSocket;
			m_pSocket = NULL;
		}
	}
	// Assume we're connected until OnConnect
	// tells us otherwise. This is done to
	// make menu pCmdUI's work
	m_fConnected = TRUE;
}

void CTermView::OnUpdateSocketConnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_fConnected);
}

void CTermView::OnSocketClose()
{                      
	// Initiate a graceful close
	// Tell the peer that we're
	// not going to send anymore
	m_pSocket->ShutDown(CAsyncSocket::sends);
	
	// Receive any pending data
	int nRet;
	char szBuf[256];
	while(1)
	{
    	nRet = m_pSocket->Receive(szBuf, sizeof(szBuf));
    	if (nRet == 0 || nRet == SOCKET_ERROR)
    		break;
	}
	// Tell the peer that we're not going
	// to send or receive anymore
	m_pSocket->ShutDown(CAsyncSocket::both);
	
	// Close the socket	
	m_pSocket->Close();
	// And delete it
	delete m_pSocket;
	m_pSocket = NULL;
	m_fConnected = FALSE;
}

void CTermView::OnUpdateSocketClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_fConnected);
}

void CTermView::OnConnect(int nErrorCode)
{
	if (m_fShowNotifications)
		Display("\tOnConnect(%d)\r\n", nErrorCode);
		
	if (nErrorCode)
	{
		m_fConnected = FALSE;
		Display("\tError OnConnect(): %d\r\n",
					nErrorCode);
	}
	else
	{
		m_fConnected = TRUE;
		Display("\tSocket connected\r\n");
	}
}

void CTermView::OnSend(int nErrorCode)
{
	if (m_fShowNotifications)
		Display("\tOnSend(%d)\r\n", nErrorCode);
}

void CTermView::OnReceive(int nErrorCode)
{
	if (m_fShowNotifications)
		Display("\tOnReceive(%d)\r\n", nErrorCode);
		
	static char szBuf[256];
	// Set buffer to all zeros
	// So that it is already
	// NULL terminated	
	memset(szBuf, 0, sizeof(szBuf));
	// Receive maximum of one less byte
	// than buffer size, so NULL
	// is always last character.
	int nBytes = m_pSocket->Receive(szBuf, 255, 0);
	if (nBytes == 0)
	{
		Display("Receive() indicates that socket is closed\r\n");
		return;
	}
	if (nBytes == SOCKET_ERROR)
	{                            
		CString strError;
		strError.LoadString(m_pSocket->GetLastError());
		Display("Receive error %s\r\n", strError);
		return;
	}
	Display(szBuf);
}

void CTermView::OnClose(int nErrorCode)
{                       
	if (m_fShowNotifications)
		Display("\tOnClose(%d)\r\n", nErrorCode);

	AfxMessageBox("Socket closed");
	m_fConnected = FALSE;
}



