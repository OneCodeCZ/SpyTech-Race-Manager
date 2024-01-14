///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CDlgSelectTreeItem : public CDialog
{
public:
	CWslArray<CRuntimeClass*>	*m_paClasses;
	CWsfTreeItem							*m_pTreeItem, *m_pSelTreeItem;
	int												m_nTitleID, m_nTextID;

public:
	CDlgSelectTreeItem(CWnd* pParent = NULL);

	HTREEITEM InsertItem(CWsfTreeItem *pItem, HTREEITEM hParent);
	void BuildTree(CWsfTreeItem *pItem, HTREEITEM hParent);

	// Dialog Data

	//{{AFX_DATA(CDlgSelectTreeItem)
	enum { IDD = IDD_SELECT_TREE_ITEM };
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectTreeItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectTreeItem)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};









