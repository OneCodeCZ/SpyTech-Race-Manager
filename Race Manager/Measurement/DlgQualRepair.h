#if !defined(AFX_DLGQUALREPAIR_H__A6F95620_9195_11D4_9DF4_00E0981D0DC7__INCLUDED_)
#define AFX_DLGQUALREPAIR_H__A6F95620_9195_11D4_9DF4_00E0981D0DC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQualRepair.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgQualRepair dialog

class CDlgQualRepair : public CDialog
{
// Construction
public:
	CDlgQualRepair(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQualRepair)
	enum { IDD = IDD_QUAL_REPAIR };
	DWORD	m_dw1;
	DWORD	m_dw2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQualRepair)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQualRepair)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUALREPAIR_H__A6F95620_9195_11D4_9DF4_00E0981D0DC7__INCLUDED_)
