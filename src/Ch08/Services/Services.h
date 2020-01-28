// Services.h : main header file for the SERVICES application
//

#if !defined(AFX_SERVICES_H__951771A5_0217_11D1_85E2_444553540000__INCLUDED_)
#define AFX_SERVICES_H__951771A5_0217_11D1_85E2_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CServicesApp:
// See Services.cpp for the implementation of this class
//

class CServicesApp : public CWinApp
{
public:
	CServicesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServicesApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CServicesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICES_H__951771A5_0217_11D1_85E2_444553540000__INCLUDED_)
