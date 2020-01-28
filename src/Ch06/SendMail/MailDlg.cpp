// MailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendMail.h"
#include "MailDlg.h"
#include "ServDlg.h"
#include "socketx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMailDlg dialog

CMailDlg::CMailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMailDlg)
	m_strMessage = _T("");
	m_strToAddress = _T("");
	m_strSubject = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMailDlg)
	DDX_Text(pDX, IDC_MESSAGE_BODY, m_strMessage);
	DDX_Text(pDX, IDC_TO_ADDRESS, m_strToAddress);
	DDX_Text(pDX, IDC_SUBJECT, m_strSubject);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMailDlg, CDialog)
	//{{AFX_MSG_MAP(CMailDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND_MAIL, OnSendMail)
	ON_BN_CLICKED(IDC_SMTP_INFO, OnSmtpInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMailDlg message handlers

BOOL CMailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMailDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMailDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMailDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMailDlg::OnSmtpInfo() 
{
	CServerDialog dlg;
	dlg.DoModal();
}

void CMailDlg::OnSendMail() 
{
	UpdateData(TRUE);

	// Has a to address and message been entered?
	if (m_strToAddress.IsEmpty())
	{
		CWnd *pWnd = GetDlgItem(IDC_TO_ADDRESS);
		if (pWnd)
			pWnd->SetFocus();
		AfxMessageBox("Please enter a TO: address");
		return;
	}
	if (m_strMessage.IsEmpty())
	{
		CWnd *pWnd = GetDlgItem(IDC_MESSAGE_BODY);
		if (pWnd)
			pWnd->SetFocus();
		AfxMessageBox("Please enter a message");
		return;
	}

	// Retrieve the SMTP Info
	CString strServer;
	CString strFrom;
	CSendMailApp* pApp = (CSendMailApp*)AfxGetApp();
	pApp->GetSmtpInfo(strServer, strFrom);

	// If the SMTP info hasn't been filled in
	// Show the SMTP dialog
	if (strServer.IsEmpty() || strFrom.IsEmpty())
	{
		CServerDialog dlg;
		// Loop until we have the info or the user cancels
		while(1)
		{
			if (dlg.DoModal() != IDOK)
				return;
			if (!dlg.m_strServer.IsEmpty() && 
			    !dlg.m_strFrom.IsEmpty())
			{
				strServer = dlg.m_strServer;
				strFrom   = dlg.m_strFrom;
				break;
			}
		}
	}

	// Display an hour glass while
	// we're working
	CWaitCursor wait;

	// Send the message to the server
	BOOL bRet = SendMailMessage(strServer,
							    strFrom,
								m_strToAddress,
								m_strSubject,
								m_strMessage);
	// If send was successful
	if (bRet)
	{
		// Tell the user
		AfxMessageBox("Message Sent");
		// Clear out the edit boxes
		m_strToAddress = "";
		m_strSubject = "";
		m_strMessage = "";
		UpdateData(FALSE);
		// And place the caret back in the To: line
		CWnd *pWnd = GetDlgItem(IDC_TO_ADDRESS);
		if (pWnd)
			pWnd->SetFocus();
	}
}

void CMailDlg::ReportSocketError(int nError)
{
	CString strError;
	strError.LoadString(nError);
	AfxMessageBox(strError);
}


// 
// SendMailMessage()
// Sends an email message through
// an SMTP server.
// Accepts all needed info as parameters
// to make it easier to steal.
//
//
BOOL CMailDlg::SendMailMessage(LPCTSTR szServer, 
							   LPCTSTR szFrom, 
							   LPCTSTR szTo, 
							   LPCTSTR szSubject, 
							   LPCTSTR szMessage)
{
	// Construct a socket from the derived class
	CSocketX theSocket;

	// And create the socket descriptor
	if (!theSocket.Create())
	{
		AfxMessageBox("Socket creation failed");
		return FALSE;
	} 

	// Connect to the server
	if (!theSocket.Connect(szServer, 25))
	{
		AfxMessageBox("Could not connect to server");
		return FALSE;
	}

	// General purpose strings
	CString strCommand;
	CString strResponse;

	// Set timeout to 10 seconds
	UINT uTimeOut = 10000;

	// Read the "HELO" response from the server
	if (theSocket.Receive(strResponse, uTimeOut) == SOCKET_ERROR)
	{
		ReportSocketError(theSocket.GetLastError());
		return FALSE;
	}

	// and check to see if we're talking to an
	// SMTP server
	if (strResponse.Left(3) != _T("220"))
	{
		CString strError = "ERROR: Not a valid SMTP server response\r\n";
		strError += strResponse;
		AfxMessageBox(strError);
		theSocket.Send("QUIT\r\n");
		return FALSE;
	}

	// Send the "FROM" line
	strCommand = "MAIL FROM:<";
	strCommand += szFrom;
	strCommand += ">\r\n";
	theSocket.Send(strCommand);
	
	// and check the response
	if (theSocket.Receive(strResponse, uTimeOut) == SOCKET_ERROR)
	{
		ReportSocketError(theSocket.GetLastError());
		return FALSE;
	}
	if (strResponse.Left(3) != _T("250"))
	{
		CString strError = "ERROR: Sender rejected\r\n";
		strError += strResponse;
		AfxMessageBox(strError);
		theSocket.Send("QUIT\r\n");
		return FALSE;
	}

	// Send the "RCPT" line
	strCommand = "RCPT TO:<";
	strCommand += szTo;
	strCommand += ">\r\n";
	theSocket.Send(strCommand);

	// and check the response
	if (theSocket.Receive(strResponse, uTimeOut) == SOCKET_ERROR)
	{
		ReportSocketError(theSocket.GetLastError());
		return FALSE;
	}
	if (strResponse.Left(3) != _T("250"))
	{
		CString strError = "ERROR: Recipient rejected\r\n";
		strError += strResponse;
		AfxMessageBox(strError);
		theSocket.Send("QUIT\r\n");
		return FALSE;
	}

	// Send the "DATA" line
	theSocket.Send("DATA\r\n");

	// and check the response
	if (theSocket.Receive(strResponse, uTimeOut) == SOCKET_ERROR)
	{
		ReportSocketError(theSocket.GetLastError());
		return FALSE;
	}
	if (strResponse.Left(3) != _T("354"))
	{
		CString strError = "ERROR: DATA command rejected\r\n";
		strError += strResponse;
		AfxMessageBox(strError);
		theSocket.Send("QUIT\r\n");
		return FALSE;
	}

	// Send the "Subject" line
	strCommand = "Subject: ";
	strCommand += szSubject;
	strCommand += "\r\n";
	theSocket.Send(strCommand);

	// No response from server expectd

	// Send the message data
	// This code assumes the message
	// data contains CRLF pairs 
	// where appropriate.
	if (theSocket.Send(szMessage) == SOCKET_ERROR)
	{
		ReportSocketError(theSocket.GetLastError());
		return FALSE;
	}

	// No response from server expectd

	// Send the termination line
	theSocket.Send("\r\n.\r\n");

	// and check the response
	if (theSocket.Receive(strResponse, uTimeOut) == SOCKET_ERROR)
	{
		ReportSocketError(theSocket.GetLastError());
		return FALSE;
	}
	if (strResponse.Left(3) != _T("250"))
	{
		CString strError = "ERROR: Message body rejected\r\n";
		strError += strResponse;
		AfxMessageBox(strError);
		theSocket.Send("QUIT\r\n");
		return FALSE;
	}

	// Send the "QUIT" line
	theSocket.Send("QUIT\r\n");

	return TRUE;
}

