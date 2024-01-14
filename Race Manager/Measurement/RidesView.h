/******************
*                 *  RidesView.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  SEP/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#if !defined(AFX_RIDESVIEW_H__FDC9D5C0_9473_11D4_9925_004F49094CF9__INCLUDED_)
#define AFX_RIDESVIEW_H__FDC9D5C0_9473_11D4_9925_004F49094CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*******************************************************************************
*
*  CRidesView
*
*******************************************************************************/

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CRidesView : public CFormView
{
public:
	CRidesView();
	DECLARE_DYNCREATE(CRidesView)

// Form Data
public:
	//{{AFX_DATA(CRidesView)
	enum { IDD = IDD_RIDES_VIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CTIMeasuredRace *m_pRace;
	CWsfChildrenResizer m_Resize;
	CWsfTableCtrl m_wndTblCtrl;

// Operations
public:

	void UpdateList(int nRacePart);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRidesView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRidesView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	// Generated message map functions
	//{{AFX_MSG(CRidesView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/******************************************************************************/

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RIDESVIEW_H__FDC9D5C0_9473_11D4_9925_004F49094CF9__INCLUDED_)

/*********************************************************************** eof **/
