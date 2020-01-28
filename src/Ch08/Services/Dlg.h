// Dlg.h : header file
//

#if !defined(AFX_DLG_H__951771A7_0217_11D1_85E2_444553540000__INCLUDED_)
#define AFX_DLG_H__951771A7_0217_11D1_85E2_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CServicesDlg dialog

class CServicesDlg : public CDialog
{
// Construction
public:
	LPVOID m_pNameSpacesBuf;
	LPVOID m_pProtocolsBuf;
	void EnumNameSpaces(void);
	void EnumProtocols(void);
	void FillGuidSelections(DWORD dwNameSpaceId);
	BOOL GetGuidChoice(LPCSTR lpszName, LPGUID lpGuid);
	CServicesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CServicesDlg)
	enum { IDD = IDD_SERVICES_DIALOG };
	CListBox	m_ctlResults;
	CListBox	m_ctlProtocols;
	int		m_nNameSpace;
	BOOL	m_fProtocolRestrict;
	CString	m_strQuery;
	CString	m_strServiceName;
	CString	m_strGuidChoice;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServicesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CServicesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeNameSpace();
	afx_msg void OnProtocolRestrict();
	afx_msg void OnLookup();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__951771A7_0217_11D1_85E2_444553540000__INCLUDED_)
