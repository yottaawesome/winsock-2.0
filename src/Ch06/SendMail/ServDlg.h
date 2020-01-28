#if !defined(AFX_SERVDLG_H__84389A51_F582_11D0_85E1_444553540000__INCLUDED_)
#define AFX_SERVDLG_H__84389A51_F582_11D0_85E1_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ServDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerDialog dialog

class CServerDialog : public CDialog
{
// Construction
public:
	CServerDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServerDialog)
	enum { IDD = IDD_SERVER_DIALOG };
	CString	m_strServer;
	CString	m_strFrom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVDLG_H__84389A51_F582_11D0_85E1_444553540000__INCLUDED_)
