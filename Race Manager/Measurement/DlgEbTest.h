#if !defined(AFX_DLGEBTEST_H__2C66D1D1_A580_4E84_9FB5_C301005733E1__INCLUDED_)
#define AFX_DLGEBTEST_H__2C66D1D1_A580_4E84_9FB5_C301005733E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEbTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEbTest dialog

class CDlgEbTest : public CDialog
{
// Construction
public:
	CDlgEbTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEbTest)
	enum { IDD = IDD_EB_TEST };
	CString	m_sLane;
	CString	m_sLaps;
	CString	m_sLights;
	CString	m_sState;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEbTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEbTest)
	afx_msg void OnPing();
	afx_msg void OnReset();
	afx_msg void OnUpdateLights();
	afx_msg void OnUpdateLaps();
	afx_msg void OnUpdateState();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEBTEST_H__2C66D1D1_A580_4E84_9FB5_C301005733E1__INCLUDED_)
