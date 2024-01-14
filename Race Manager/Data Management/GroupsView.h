///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CGroupsView : public CFormView
{
protected:
	CWsfChildrenResizer m_Resize;
	CTIRace							&m_Race;
	int									m_nActiveGroup;

public:
	CGroupsView(CTIRace *pRace);
	virtual ~CGroupsView();

protected:
	void BuildGroupList();
	void BuildUndividedList();
	void RemoveFromGroupList(bool bRemoveAll);
	CString GetDispText(CTIRaceEntity *pRE);
	void UpdateControls();

// Dialog Data
	//{{AFX_DATA(CGroupsView)
	enum { IDD = IDD_GROUPS_VIEW };
	CListBox	m_GroupList;
	CListBox	m_UndividedList;
	CListBox	m_MemberList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupsView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAuto();
	afx_msg void OnRemoveAll();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPrint();
	afx_msg void OnSelchangeGroupsList();
	afx_msg void OnAddGroup();
	afx_msg void OnRemoveGroup();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnDblclkMembersList();
	afx_msg void OnDblclkUndividedList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
