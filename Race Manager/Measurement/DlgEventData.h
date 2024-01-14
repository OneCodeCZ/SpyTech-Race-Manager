#if !defined(AFX_DLGEVENTDATA_H__69F7D5C0_9B91_11D4_9925_004F49094CF9__INCLUDED_)
#define AFX_DLGEVENTDATA_H__69F7D5C0_9B91_11D4_9925_004F49094CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEventData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEventData dialog

class CDlgEventData : public CDialog
{
// Construction
public:
	CDlgEventData(CWnd* pParent = NULL);   // standard constructor

	CTIMeasuredEntity *m_pEntity;
	int nRacePart;

// Dialog Data
	//{{AFX_DATA(CDlgEventData)
	enum { IDD = IDD_EVENT_DATA };
	CStatic	m_wndCaption2;
	CStatic	m_wndCaption1;
	CString	m_sRide1;
	CString	m_sRide2;
	CString	m_sRide3;
	CString	m_sRide4;
	CString	m_sRide5;
	CString	m_sRide6;
	CString	m_sRide7;
	CString	m_sRide8;
	CString	m_sRide9;
	CString	m_sRide10;
	CString	m_sHdts;
	CString	m_sLapsElapsed;
	CString	m_sPenalization;
	CString	m_sTotalLapsElapsed;
	//}}AFX_DATA

	void UpdateDependentVariables();

	CWslArray<CString*> m_pArrStrings;
	CWslArray<CWnd*> m_pArrEdits;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEventData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEventData)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnUpdateRide1();
	afx_msg void OnUpdateRide10();
	afx_msg void OnUpdateRide2();
	afx_msg void OnUpdateRide3();
	afx_msg void OnUpdateRide4();
	afx_msg void OnUpdateRide5();
	afx_msg void OnUpdateRide6();
	afx_msg void OnUpdateRide7();
	afx_msg void OnUpdateRide8();
	afx_msg void OnUpdateRide9();
	afx_msg void OnUpdatePenalization();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEVENTDATA_H__69F7D5C0_9B91_11D4_9925_004F49094CF9__INCLUDED_)
