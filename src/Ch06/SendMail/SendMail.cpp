// SendMail.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SendMail.h"
#include "MailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendMailApp

BEGIN_MESSAGE_MAP(CSendMailApp, CWinApp)
	//{{AFX_MSG_MAP(CSendMailApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendMailApp construction

CSendMailApp::CSendMailApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSendMailApp object

CSendMailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSendMailApp initialization

BOOL CSendMailApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	//Disabled due to warning C4996
	//Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMailDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// .INI File Section and entries
static const char szSection[] = "SMTPInfo";
static const char szServer[] = "ServerName";
static const char szFrom[] = "From";


void CSendMailApp::GetSmtpInfo(CString & strServer, CString & strFrom)
{
	// Restore previous selections
	strServer = GetProfileString(szSection, 
							     szServer);
	strFrom   = GetProfileString(szSection, 
							     szFrom);
}

void CSendMailApp::WriteSmtpInfo(CString & strServer, CString & strFrom)
{
	// Save entries to .ini for next use
	WriteProfileString(szSection, 
								 szServer, 
								 strServer);
	WriteProfileString(szSection, 
								 szFrom, 
								 strFrom);
}
