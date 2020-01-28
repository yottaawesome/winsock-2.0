// Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Services.h"
#include "Dlg.h"

// AFtoSTR.CPP -- Address Family to String
LPCTSTR AFtoSTR(int nAddressFamily);
// PROTSTR.CPP -- Protocol Family to String
LPCTSTR PROTOtoSTR(int iProtocol);
// SOCKSTR.CPP -- Socket Type to String
LPCSTR SocketTypeToString(int nSocketType);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////

typedef struct tagGUIDDESC
{
	DWORD	dwNameSpace;
	GUID	guid;
	LPCSTR	lpszDesc;
}GUIDDESC, LPGUIDESC;

////////////////////////////////////////////////////////////

GUIDDESC guidChoices[] = {
// General
NS_ALL,SVCID_HOSTNAME				 ,"SVCID_HOSTNAME",
// Internet - TCP/IP
NS_DNS,SVCID_INET_HOSTADDRBYNAME	 ,"SVCID_INET_HOSTADDRBYNAME",
NS_DNS,SVCID_INET_HOSTADDRBYINETSTRING, "SVCID_INET_HOSTADDRBYINETSTRING",
NS_DNS,SVCID_TCP(80)				 ,"SVCID_TCP(80)-HTTP",
NS_DNS,SVCID_DAYTIME_TCP             ,"SVCID_DAYTIME_TCP",
NS_DNS,SVCID_DAYTIME_UDP             ,"SVCID_DAYTIME_UDP",
NS_DNS,SVCID_DISCARD_TCP             ,"SVCID_DISCARD_TCP",
NS_DNS,SVCID_DISCARD_UDP             ,"SVCID_DISCARD_UDP",
NS_DNS,SVCID_DOMAIN_TCP              ,"SVCID_DOMAIN_TCP",
NS_DNS,SVCID_DOMAIN_UDP              ,"SVCID_DOMAIN_UDP",
NS_DNS,SVCID_ECHO_TCP                ,"SVCID_ECHO_TCP",
NS_DNS,SVCID_ECHO_UDP                ,"SVCID_ECHO_UDP",
NS_DNS,SVCID_FINGER_TCP              ,"SVCID_FINGER_TCP",
NS_DNS,SVCID_FTP_TCP                 ,"SVCID_FTP_TCP",
NS_DNS,SVCID_NAMESERVER_TCP          ,"SVCID_NAMESERVER_TCP",
NS_DNS,SVCID_NAMESERVER_UDP          ,"SVCID_NAMESERVER_UDP",
NS_DNS,SVCID_NAME_TCP                ,"SVCID_NAME_TCP",
NS_DNS,SVCID_NAME_UDP                ,"SVCID_NAME_UDP",
NS_DNS,SVCID_NEWS_TCP                ,"SVCID_NEWS_TCP",
NS_DNS,SVCID_POP2_TCP                ,"SVCID_POP2_TCP",
NS_DNS,SVCID_POP3_TCP                ,"SVCID_POP3_TCP",
NS_DNS,SVCID_POP_TCP                 ,"SVCID_POP_TCP",
NS_DNS,SVCID_QOTD_TCP                ,"SVCID_QOTD_TCP",
NS_DNS,SVCID_SMTP_TCP                ,"SVCID_SMTP_TCP",
NS_DNS,SVCID_SYSTAT_TCP              ,"SVCID_SYSTAT_TCP",
NS_DNS,SVCID_TELNET_TCP              ,"SVCID_TELNET_TCP",
NS_DNS,SVCID_TIME_TCP                ,"SVCID_TIME_TCP",
NS_DNS,SVCID_TIME_UDP                ,"SVCID_TIME_UDP",
NS_DNS,SVCID_WHOIS_TCP               ,"SVCID_WHOIS_TCP",
// Netware
NS_NDS,SVCID_PRINT_QUEUE             ,"(NW) SVCID_PRINT_QUEUE",
NS_NDS,SVCID_FILE_SERVER             ,"(NW) SVCID_FILE_SERVER",
NS_NDS,SVCID_JOB_SERVER              ,"(NW) SVCID_JOB_SERVER",
NS_NDS,SVCID_GATEWAY                 ,"(NW) SVCID_GATEWAY",
NS_NDS,SVCID_PRINT_SERVER            ,"(NW) SVCID_PRINT_SERVER",
NS_NDS,SVCID_ARCHIVE_QUEUE           ,"(NW) SVCID_ARCHIVE_QUEUE",
NS_NDS,SVCID_ARCHIVE_SERVER          ,"(NW) SVCID_ARCHIVE_SERVER",
NS_NDS,SVCID_JOB_QUEUE               ,"(NW) SVCID_JOB_QUEUE",
NS_NDS,SVCID_ADMINISTRATION          ,"(NW) SVCID_ADMINISTRATION",
NS_NDS,SVCID_NAS_SNA_GATEWAY         ,"(NW) SVCID_NAS_SNA_GATEWAY",
NS_NDS,SVCID_REMOTE_BRIDGE_SERVER    ,"(NW) SVCID_REMOTE_BRIDGE_SERVER",
NS_NDS,SVCID_TIME_SYNCHRONIZATION_SERVER,"(NW) SVCID_TIME_SYNCHRONIZATION_SERVER",
NS_NDS,SVCID_ARCHIVE_SERVER_DYNAMIC_SAP,"(NW) SVCID_ARCHIVE_SERVER_DYNAMIC_SAP",
NS_NDS,SVCID_ADVERTISING_PRINT_SERVER,"(NW) SVCID_ADVERTISING_PRINT_SERVER",
NS_NDS,SVCID_BTRIEVE_VAP             ,"(NW) SVCID_BTRIEVE_VAP",
NS_NDS,SVCID_DIRECTORY_SERVER        ,"(NW) SVCID_DIRECTORY_SERVER",
NS_NDS,SVCID_NETWARE_386             ,"(NW) SVCID_NETWARE_386",
NS_NDS,SVCID_HP_PRINT_SERVER         ,"(NW) SVCID_HP_PRINT_SERVER",
NS_NDS,SVCID_SNA_SERVER              ,"(NW) SVCID_SNA_SERVER",
NS_NDS,SVCID_SAA_SERVER              ,"(NW) SVCID_SAA_SERVER"
};

#define NUMGUIDCHOICES sizeof(guidChoices) / sizeof(guidChoices[0])

////////////////////////////////////////////////////////////
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
	virtual void DoDataExchange(CDataExchange* pDX);
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

////////////////////////////////////////////////////////////
// CServicesDlg dialog

CServicesDlg::CServicesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServicesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServicesDlg)
	m_nNameSpace = -1;
	m_fProtocolRestrict = FALSE;
	m_strQuery = _T("");
	m_strServiceName = _T("");
	m_strGuidChoice = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a 
	// subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServicesDlg)
	DDX_Control(pDX, IDC_RESULTS, m_ctlResults);
	DDX_Control(pDX, IDC_PROTOCOL_LIST, m_ctlProtocols);
	DDX_CBIndex(pDX, IDC_NAME_SPACE, m_nNameSpace);
	DDX_Check(pDX, IDC_PROTOCOL_RESTRICT, m_fProtocolRestrict);
	DDX_Text(pDX, IDC_QUERY_STRING, m_strQuery);
	DDX_Text(pDX, IDC_SERVICE_NAME, m_strServiceName);
	DDX_CBString(pDX, IDC_CLASS_GUID, m_strGuidChoice);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CServicesDlg, CDialog)
	//{{AFX_MSG_MAP(CServicesDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_NAME_SPACE, OnSelchangeNameSpace)
	ON_BN_CLICKED(IDC_PROTOCOL_RESTRICT, OnProtocolRestrict)
	ON_BN_CLICKED(ID_LOOKUP, OnLookup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////
// CServicesDlg message handlers

BOOL CServicesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pProtocolsBuf  = NULL;
	m_pNameSpacesBuf = NULL;

	//
	// Add "About..." menu item to system menu.
	//
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
			pSysMenu->AppendMenu(MF_STRING, 
								 IDM_ABOUTBOX, 
								 strAboutMenu);
		}
	}

	//
	// Set the icon for this dialog.
	// 
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// 
	// Get the list of protocols
	//
	EnumProtocols();

	//
	// Get the list of name space providers
	//
	EnumNameSpaces();

	m_nNameSpace = 0;
	UpdateData(FALSE);
	FillGuidSelections(NS_ALL);
	return TRUE;
}

void CServicesDlg::OnCancel() 
{
	if (m_pProtocolsBuf  != NULL)
		delete m_pProtocolsBuf;	
	if (m_pNameSpacesBuf != NULL)
		delete m_pNameSpacesBuf;
	CDialog::OnCancel();
}

void CServicesDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will 
// need the code below to draw the icon.  For MFC 
// applications using the document/view model,
// this is automatically done for you by the framework.
void CServicesDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display 
// while the user drags the minimized window.
HCURSOR CServicesDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CServicesDlg::OnSelchangeNameSpace() 
{
	//
	// When the NameSpace selection changes
	// Change the options in the Class GUID box
	//
	CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_NAME_SPACE);
	ASSERT(pBox);

	int nIndex = pBox->GetCurSel();

	//
	// If the choice wasn't "NS_ALL"
	//
	if (nIndex != 0)
	{
		//
		// Lookup the Name SpaceID
		//
		LPWSANAMESPACE_INFO pInfo = 
				(LPWSANAMESPACE_INFO)m_pNameSpacesBuf;
		nIndex--;
		pInfo+=nIndex;
		FillGuidSelections(pInfo->dwNameSpace);
	}
	else
		FillGuidSelections(NS_ALL);
}

void CServicesDlg::OnProtocolRestrict() 
{
	CButton *pButton = (CButton *)GetDlgItem(IDC_PROTOCOL_RESTRICT);
	ASSERT(pButton);
	CWnd *pWnd = GetDlgItem(IDC_PROTOCOL_LIST);
	ASSERT(pWnd);
	pWnd->EnableWindow(pButton->GetCheck());
}


void CServicesDlg::EnumProtocols()
{
	//
	// Determine needed buffer size by
	// intentionally generating an error.
	//
	DWORD dwLen = 0;
	int nRet = WSAEnumProtocols(NULL,
								NULL,
								&dwLen);
	if (nRet == SOCKET_ERROR)
	{
		// Look for the expected error
		if (WSAGetLastError() != WSAENOBUFS)
		{
			AfxMessageBox(WSAGetLastError());
			return;
		}
	}

	//
	// dwLen contains needed buffer size
	//
	m_pProtocolsBuf= new BYTE[dwLen];

	//
	// Make the "real" call
	//
	nRet = WSAEnumProtocols(NULL, 
				(LPWSAPROTOCOL_INFO)m_pProtocolsBuf, 
				&dwLen);
	if (nRet == SOCKET_ERROR)
	{
		delete m_pProtocolsBuf;
		m_pProtocolsBuf = NULL;
		AfxMessageBox(WSAGetLastError());
		return;
	}

	//
	// Add protocol descriptions to the list box
	//
	LPWSAPROTOCOL_INFO pInfo =						
			(LPWSAPROTOCOL_INFO)m_pProtocolsBuf;
	for (int nCount = 0; nCount < nRet; nCount++)
	{
		m_ctlProtocols.AddString(pInfo->szProtocol);
		pInfo++;
	}
}

void CServicesDlg::EnumNameSpaces()
{
	//
	// Intentionally generate an error
	// to get the required buffer size
	//
	DWORD dwLen = 0;
	int nRet = WSAEnumNameSpaceProviders(&dwLen, NULL);
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEFAULT)
		{
			AfxMessageBox(WSAGetLastError());
			return;
		}
	}

	//
	// dwLen now equals needed size
	//
	m_pNameSpacesBuf = new BYTE[dwLen];

	//
	// Now, make the "real" call
	//
	nRet = WSAEnumNameSpaceProviders(&dwLen,
				(LPWSANAMESPACE_INFO)m_pNameSpacesBuf);
	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox(WSAGetLastError());
		delete m_pNameSpacesBuf;
		m_pNameSpacesBuf = NULL;
		return;
	}

	CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_NAME_SPACE);
	ASSERT(pBox);

	//
	// Add the NS_ALL entry
	//
	pBox->AddString("NS_ALL");

	//
	// Add all other available name spaces
	//
	LPWSANAMESPACE_INFO pInfo = 
				(LPWSANAMESPACE_INFO)m_pNameSpacesBuf;

	for (int nCount = 0; nCount < nRet; nCount++)
	{
		pBox->AddString(pInfo->lpszIdentifier);
		pInfo++;
	}
}

void CServicesDlg::FillGuidSelections(DWORD dwNameSpaceId)
{
	CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_CLASS_GUID);
	ASSERT(pBox);

	// 
	// Empty the selection box
	//
	pBox->ResetContent();

	for (int nCount = 0; nCount < NUMGUIDCHOICES; nCount++)
	{
		if (dwNameSpaceId == NS_ALL ||
		    guidChoices[nCount].dwNameSpace == dwNameSpaceId)
			pBox->AddString(guidChoices[nCount].lpszDesc);
	}
	pBox->SetCurSel(0);
}

////////////////////////////////////////////////////////////

void CServicesDlg::OnLookup() 
{
	// 
	// Empty the results listbox
	//
	m_ctlResults.ResetContent();

	//
	// Get the user's selections
	//
	UpdateData();
	
	//
	// Initialize default WSAQUERYSET values
	//
	WSAQUERYSET qs;
	memset(&qs, 0, sizeof(WSAQUERYSET));
	qs.dwSize = sizeof(WSAQUERYSET);

	//
	// If the search was restricted by protcol,
	// then setup the AFPROTOCOLS array.
	// At least one protocol must be selected
	//
	LPAFPROTOCOLS pAfProtocols = NULL;
	DWORD dwNumberOfProtocols;
	if (m_fProtocolRestrict)
	{
		CListBox *pBox = 
				(CListBox *)GetDlgItem(IDC_PROTOCOL_LIST);
		ASSERT(pBox);
		dwNumberOfProtocols = pBox->GetSelCount();
		if (dwNumberOfProtocols < 1)
		{
			AfxMessageBox(
				"Please select at least one protocol");
			return;
		}
		pAfProtocols = new AFPROTOCOLS[dwNumberOfProtocols];
		LPINT pIndexes = new INT[dwNumberOfProtocols];
		int nRet = pBox->GetSelItems(dwNumberOfProtocols, 
								     pIndexes);
		if (nRet == LB_ERR)
		{
			AfxMessageBox(
				"Couldn't get selected protocols");
			return;
		}
		LPWSAPROTOCOL_INFO pProto = 
				(LPWSAPROTOCOL_INFO)m_pProtocolsBuf;
		for (DWORD dwX = 0; dwX < dwNumberOfProtocols; dwX++)
		{
			pAfProtocols[dwX].iAddressFamily = 
							pProto[dwX].iAddressFamily;	
			pAfProtocols[dwX].iProtocol = 
							pProto[dwX].iProtocol;
		}
		delete pIndexes;
		qs.dwNumberOfProtocols = dwNumberOfProtocols;
		qs.lpafpProtocols	   = pAfProtocols;
	}

	//
	// Assign the selected Name Space
	//
	if (m_nNameSpace == 0)
		qs.dwNameSpace = NS_ALL;
	else
	{
		// Lookup the Name SpaceID
		LPWSANAMESPACE_INFO pInfo = 
					(LPWSANAMESPACE_INFO)m_pNameSpacesBuf;
		pInfo += (--m_nNameSpace);
		qs.dwNameSpace = pInfo->dwNameSpace;
	}

	//
	// Translate the selected GUID string to a real GUID
	//
	GUID guidServiceClass;
	if (!GetGuidChoice(m_strGuidChoice, &guidServiceClass))
	{
		AfxMessageBox("Couldn't get GUID Choice");
		if (pAfProtocols != NULL)
			delete pAfProtocols;
		return;
	}
	qs.lpServiceClassId = &guidServiceClass;

	//
	// Assign Service Name
	//
	qs.lpszServiceInstanceName = 
			m_strServiceName.GetBuffer(1);
	
	//
	// Begin the lookup
	//
	DWORD dwFlags;
	HANDLE hLookup;
	dwFlags = LUP_RETURN_NAME|LUP_RETURN_ADDR;
	CWaitCursor wait; 
	int nRet = WSALookupServiceBegin(&qs,
								 dwFlags, 
								 &hLookup);
	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox(WSAGetLastError());
		if (pAfProtocols != NULL)
			delete pAfProtocols;
		return;
	}

	//
	// Loop through the services
	//
	BOOL fFound = FALSE;
	DWORD dwResultLen = 1024;
	LPBYTE pResultBuf = new BYTE[dwResultLen];
	int iBreak = 0;
	while(1)
	{
		dwFlags = LUP_RETURN_NAME|LUP_RETURN_ADDR;
		nRet = WSALookupServiceNext(hLookup,
									dwFlags, 
									&dwResultLen,
									(LPWSAQUERYSET)pResultBuf);
		if (nRet == SOCKET_ERROR)
		{
			// Buffer too small?
			if (WSAGetLastError() == WSAEFAULT)
			{
				delete pResultBuf;
				pResultBuf = new BYTE[dwResultLen];
				continue;
			}
			if (WSAGetLastError() != WSA_E_NO_MORE)
				AfxMessageBox(WSAGetLastError());
			break;
		}

		fFound = TRUE;
		//
		// Cast a pointer to the resulting WSAQUERYSET
		//
		LPWSAQUERYSET pqs;
		pqs = (LPWSAQUERYSET)pResultBuf;

		//
		// and to the first CSADDR_INFO
		//
		LPCSADDR_INFO pcsa;
		pcsa = pqs->lpcsaBuffer;

		//
		// Loop through the CSADDR_INFO array
		//
		DWORD dwNdx;
		CString strResult;
		for (dwNdx = 0; dwNdx < pqs->dwNumberOfCsAddrs; dwNdx++)
		{
			// Get string equivalent for address
			char szAddrBuf[256];
			DWORD dwLen = sizeof(szAddrBuf);
			nRet = WSAAddressToString(pcsa->RemoteAddr.lpSockaddr,
								  pcsa->RemoteAddr.iSockaddrLength,
								  NULL,
								  szAddrBuf,
								  &dwLen);
			if (nRet == SOCKET_ERROR)
			{
				AfxMessageBox(WSAGetLastError());
				break;
			}
			//
			// Format the results
			//
			strResult.Format("%s [%s]  \tsocket ( %s, %s, %s )",
					pqs->lpszServiceInstanceName,
					szAddrBuf,
					AFtoSTR(pcsa->RemoteAddr.lpSockaddr->sa_family),
					SocketTypeToString(pcsa->iSocketType),
					PROTOtoSTR(pcsa->iProtocol)
					);
			// and add the string to the listbox
			m_ctlResults.AddString(strResult);
			pcsa++;
		}
		// Safety mechanism to not return too much stuff
		iBreak++;
		if (iBreak > 100)
		{
			AfxMessageBox("Over 100 entries returned");
			break;
		}
	}

	nRet = WSALookupServiceEnd(hLookup);
	if (nRet == SOCKET_ERROR)
		AfxMessageBox(WSAGetLastError());
	delete pResultBuf;
	if (pAfProtocols != NULL)
		delete pAfProtocols;
	if (!fFound)
		AfxMessageBox("Not found");
}


BOOL CServicesDlg::GetGuidChoice(LPCSTR lpszName, LPGUID lpGuid)
{
	for (int nCount = 0; nCount < NUMGUIDCHOICES; nCount++)
	{
		if (!strcmp(guidChoices[nCount].lpszDesc, lpszName))
		{
			*lpGuid = guidChoices[nCount].guid;
			return TRUE;
		}
	}
	return FALSE;
}
