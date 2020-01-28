// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendMail.h"
#include "ServDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerDialog dialog


CServerDialog::CServerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerDialog)
	m_strServer = _T("");
	m_strFrom = _T("");
	//}}AFX_DATA_INIT
}


void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerDialog)
	DDX_Text(pDX, IDC_SERVER, m_strServer);
	DDX_Text(pDX, IDC_FROM_ADDRESS, m_strFrom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerDialog, CDialog)
	//{{AFX_MSG_MAP(CServerDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDialog message handlers


BOOL CServerDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CSendMailApp* pApp = (CSendMailApp*)AfxGetApp();
	pApp->GetSmtpInfo(m_strServer, m_strFrom);
	
	UpdateData(FALSE);
	
	return TRUE;
}

void CServerDialog::OnOK() 
{
	UpdateData(TRUE);

	CSendMailApp* pApp = (CSendMailApp*)AfxGetApp();
	pApp->WriteSmtpInfo(m_strServer, m_strFrom);
	
	CDialog::OnOK();
}
