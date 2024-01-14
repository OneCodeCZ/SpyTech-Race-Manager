///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "main.h"
#include "DlgSelectTreeItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*****************************************************************************
WSL_API CWsfTreeItem* WsfSelectTreeItem(int nTitleID, int nTextID,
                                        CWsfTreeItem *pTreeItem,
                                        CWslArray<CRuntimeClass*> *paClasses,
                                        CWnd *pParent /*= NULL*/)
{
	ASSERT(paClasses);
	ASSERT_VALID(pTreeItem);
	if (!pParent) pParent = WsfGetFrame();
	ASSERT_VALID(pParent);

	CDlgSelectTreeItem d(pParent);

	d.m_nTitleID = nTitleID;
	d.m_nTextID = nTextID;
	d.m_paClasses = paClasses;
	d.m_pTreeItem = pTreeItem;

	return d.DoModal() == IDOK ? d.m_pSelTreeItem : NULL;
}


//*****************************************************************************
//*** DIALOG ******************************************************************
//*****************************************************************************

BEGIN_MESSAGE_MAP(CDlgSelectTreeItem, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectTreeItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CDlgSelectTreeItem::CDlgSelectTreeItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectTreeItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectTreeItem)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CDlgSelectTreeItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectTreeItem)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	//}}AFX_DATA_MAP
}

//*****************************************************************************
BOOL CDlgSelectTreeItem::OnInitDialog() 
{
	LRESULT r;
	CString s;

	CDialog::OnInitDialog();

	// Init tree

	m_Tree.SetImageList(WsfGetApp()->GetSmallIconList(), TVSIL_NORMAL);

   // The tree control paints the trasparent color white. 
   // This line makes it again transparent!
	WsfGetApp()->GetSmallIconList()->SetBkColor(CLR_NONE);

	BuildTree(m_pTreeItem, TVI_ROOT);
	OnSelchangedTree(NULL, &r);

	// Init

	s.LoadString(m_nTitleID);
	SetWindowText(s);

	s.LoadString(m_nTextID);
	GetDlgItem(IDC_TEXT)->SetWindowText(s);

	return TRUE;
}

//*****************************************************************************
HTREEITEM CDlgSelectTreeItem::InsertItem(CWsfTreeItem *pItem, HTREEITEM hParent)
{
	TVINSERTSTRUCT s;
	CString sTitle = pItem->GetTitle();

	s.hParent = hParent;
	s.hInsertAfter = TVI_LAST;
	s.item.mask = TVIF_STATE|TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_PARAM|TVIF_IMAGE|TVIF_CHILDREN;
	s.item.state = pItem->GetFlags() & CWsfTreeItem::EXPANDED ? TVIS_EXPANDED : 0;
	//s.item.state = pItem->GetChild() ? TVIS_EXPANDED : 0;
	s.item.stateMask = TVIS_EXPANDED;
	s.item.pszText = (char*) (const char*) sTitle;
	s.item.iImage = s.item.state & TVIS_EXPANDED ? pItem->GetExpandedIcon(): pItem->GetIcon();
	s.item.iSelectedImage = s.item.iImage;
	s.item.cChildren = pItem->GetChild() ? 1 : 0;
	s.item.lParam = (LPARAM) pItem;

	return m_Tree.InsertItem(&s);
}

//*****************************************************************************
void CDlgSelectTreeItem::BuildTree(CWsfTreeItem *pItem, HTREEITEM hParent)
{
	HTREEITEM hNewParent = InsertItem(pItem, hParent);

	// Insert childern

	pItem = pItem->GetChild();
	while (pItem)
	{
		BuildTree(pItem, hNewParent);
		pItem = pItem->GetNextSibling();
	} 
}

//*****************************************************************************
void CDlgSelectTreeItem::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem;
	CWsfTreeItem *pItem;
	bool b = false;
	
	*pResult = 0;
	hItem = m_Tree.GetSelectedItem();

	if (hItem)
	{
		pItem = (CWsfTreeItem*) m_Tree.GetItemData(hItem);

		for (int i = m_paClasses->GetSize(); i--;)
		{
			if (pItem->IsKindOf((*m_paClasses)[i])) 
			{
				m_pSelTreeItem = pItem;
				b = true;
				break;
			}
		}
	}

	GetDlgItem(IDOK)->EnableWindow(b);
}

//*****************************************************************************
void CDlgSelectTreeItem::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if (GetDlgItem(IDOK)->IsWindowEnabled()) OnOK();
}
