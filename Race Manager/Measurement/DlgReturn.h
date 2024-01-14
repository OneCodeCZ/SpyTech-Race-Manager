#if !defined(AFX_DLGRETURN_H__463C8CC1_80BB_11D4_B035_0000B49E3BD7__INCLUDED_)
#define AFX_DLGRETURN_H__463C8CC1_80BB_11D4_B035_0000B49E3BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgReturn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgReturn dialog

class CDlgReturn : public CDialog
{
// Construction
public:
	CDlgReturn(CWnd* pParent = NULL);   // standard constructor

	CTIMeasuredRace *m_pRace;

// Dialog Data
	//{{AFX_DATA(CDlgReturn)
	enum { IDD = IDD_RETURN_TO_RACE_PART };
	CListBox	m_listGroups;
	CListBox	m_listRides;
	CString	m_listParts;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgReturn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:

	int m_nGroupIdx;
	int m_nRideIdx;

	BOOL m_bUserWantsEndRacePart;

	void FillListOfRides(int nGroupIdx);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgReturn)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeGroups();
	afx_msg void OnWhole();
	afx_msg void OnReturn();
	afx_msg void OnEnd();
	afx_msg void OnContinue();
	afx_msg void OnSelchangeRides();
	afx_msg void OnReset();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRETURN_H__463C8CC1_80BB_11D4_B035_0000B49E3BD7__INCLUDED_)
