// MailDlg.h : header file
//

#if !defined(AFX_MAILDLG_H__84389A49_F582_11D0_85E1_444553540000__INCLUDED_)
#define AFX_MAILDLG_H__84389A49_F582_11D0_85E1_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMailDlg dialog

class CMailDlg : public CDialog
{
// Construction
public:
	void ReportSocketError(int nError);
	BOOL SendMailMessage(LPCTSTR szServer, LPCTSTR szFrom, LPCTSTR szTo, LPCTSTR szSubject, LPCTSTR szMessage);
	CMailDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMailDlg)
	enum { IDD = IDD_SENDMAIL_DIALOG };
	CString	m_strMessage;
	CString	m_strToAddress;
	CString	m_strSubject;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMailDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSendMail();
	afx_msg void OnSmtpInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAILDLG_H__84389A49_F582_11D0_85E1_444553540000__INCLUDED_)
