/******************
*                 *  DlgHdts.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#if !defined(AFX_DLGHDTS_H__B4601530_1AA6_11D4_AAFB_004F49094CF9__INCLUDED_)
#define AFX_DLGHDTS_H__B4601530_1AA6_11D4_AAFB_004F49094CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHdts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHdts dialog

#define MAXRACERS     16

#define HDTS_MODE_QUALIFICATION 0
#define HDTS_MODE_RACE          1

class CDlgHdts : public CDialog
{
// Construction
public:
	CDlgHdts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHdts)
	enum { IDD = IDD_HDTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHdts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL m_bQualification;

public:
	CWslArray<CWnd*> m_pArrControls;
	CWslArray<CWnd*> m_pArrColorFields;

	CWslArray<COLORREF> m_crArrColors;

	CString m_sCaption;
	DWORD m_dwNumRacers;

	DWORD m_dwArrNumLaps[MAXRACERS];
	DWORD m_dwArrNumHdts[MAXRACERS];
	int m_nArrPenalization[MAXRACERS];
	CString m_sArrNames[MAXRACERS];
	int PrepareData(CEntityArray &rArray, CTIMeasuredRace *pRace);
	// nNumRacers == -1 .. kvalifikace
	int DoModal(const char *pcszCaption, int nNumRacers);
	int GatherData(CEntityArray &rArray);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHdts)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnOK();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHDTS_H__B4601530_1AA6_11D4_AAFB_004F49094CF9__INCLUDED_)
