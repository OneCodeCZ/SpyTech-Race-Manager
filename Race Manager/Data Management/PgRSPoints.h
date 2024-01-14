///////////////////////////////////////////////////////////////////////////////
//
// Race Manager
// ------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CPgRSPoints : public CWsfPropertyPage
{
public:
	CTIRS						&m_RS;
	CWslArray<int>	m_aOwnPoints;
	int							m_nPointsType;
	BOOL						m_bEnablePoints;

public:
	CPgRSPoints(CTIRS *pRS);
	~CPgRSPoints();

protected:
	void BuildList(CWslArray<int> &aPoints);
	void BuildArrayFromList(CWslArray<int> &aPoints);
	void InsertAt(int nPos);

	// from WSF

	virtual BOOL OnOkButton();

// Dialog Data
	//{{AFX_DATA(CPgRSPoints)
	enum { IDD = IDD_RS_POINTS };
	CListCtrl	m_List;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgRSPoints)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CPgRSPoints)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddBefore();
	afx_msg void OnAddAfter();
	afx_msg void OnEdit();
	afx_msg void OnRemove();
	afx_msg void OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangePointsType();
	afx_msg void OnEnablePoints();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



