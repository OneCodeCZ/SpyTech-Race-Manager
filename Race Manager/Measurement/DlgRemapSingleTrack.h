#if !defined(AFX_DLGREMAPSINGLETRACK_H__88C1EFC0_7F41_11D4_B035_0000B49E3BD7__INCLUDED_)
#define AFX_DLGREMAPSINGLETRACK_H__88C1EFC0_7F41_11D4_B035_0000B49E3BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRemapSingleTrack.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRemapSingleTrack dialog

class MEASURE_API CDlgRemapSingleTrack : public CDialog
{
// Construction
public:

	CSlotTrackMonitor monitor;

	int m_nVirtualTrack;
	int m_nPhysicalTrack;

	CDlgRemapSingleTrack(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRemapSingleTrack)
	enum { IDD = IDD_REMAP_SINGLE_TRACK };
	CString	m_sChallenge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRemapSingleTrack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRemapSingleTrack)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREMAPSINGLETRACK_H__88C1EFC0_7F41_11D4_B035_0000B49E3BD7__INCLUDED_)
