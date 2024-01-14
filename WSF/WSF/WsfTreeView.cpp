///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfTreeItemSelectionContext.h"
#include "WsfCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTreeItemTreeView, CView)
	//{{AFX_MSG_MAP(CWsfTreeItemTreeView)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_WSF_RENAME, OnWsfRename)
	ON_COMMAND(ID_WSF_DELETE, OnWsfDelete)
	ON_WM_CONTEXTMENU()
//	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfTreeItemTreeView::CWsfTreeItemTreeView()
{
}

//*****************************************************************************
CWsfTreeItemTreeView::~CWsfTreeItemTreeView()
{
}

//*****************************************************************************
BOOL CWsfTreeItemTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CView::PreCreateWindow(cs)) return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	return TRUE;
}

//*****************************************************************************
int CWsfTreeItemTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;
	
	// Init tree control

	m_TreeCtrl.Create(
		TVS_HASBUTTONS|WS_CHILD|WS_VISIBLE|TVS_EDITLABELS|TVS_SHOWSELALWAYS, 
		WsfZeroRect, this, 1001);

	m_TreeCtrl.SetImageList(WsfGetApp()->GetSmallIconList(), TVSIL_NORMAL);

   // The tree control paints the trasparent color white. 
   // This line makes it again transparent!
	WsfGetApp()->GetSmallIconList()->SetBkColor(CLR_NONE);

	//  Associate this window with the COleDropTarget class

	VERIFY(m_dt.Register(this));		// OLE must me initialized!

	return 0;
}

/*****************************************************************************/
void CWsfTreeItemTreeView::__ResetHTREEITEM(CWsfTreeItem* p, void*)
{
	p->m_hTreeItem = NULL;
}

/*****************************************************************************/
void CWsfTreeItemTreeView::OnDestroy() 
{
	// Reset m_hTreeItem member for all items

	CWsfTreeItem* p = GetRootItem();
	ASSERT_VALID(p);
	p->DoForWholeTree(__ResetHTREEITEM, NULL);

	CView::OnDestroy();
}

//*****************************************************************************
void CWsfTreeItemTreeView::RedrawItem(CWsfTreeItem *pItem)
{
	// Redrawing the item by changing the icon

	ASSERT_VALID(pItem);
	int n = pItem->m_dwFlags & CWsfTreeItem::EXPANDED ? pItem->GetExpandedIcon(): pItem->GetIcon();
	m_TreeCtrl.SetItemImage(pItem->m_hTreeItem, n, n);
}

//*****************************************************************************
void CWsfTreeItemTreeView::UpdatePlusSign(CWsfTreeItem *pItem)
{
	TVITEM tvi;

	tvi.mask = TVIF_CHILDREN | TVIF_HANDLE;
	tvi.hItem = pItem->m_hTreeItem;
	tvi.cChildren = pItem->m_pChild ? 1 : 0;

	m_TreeCtrl.SetItem(&tvi);
}

//*****************************************************************************
void CWsfTreeItemTreeView::InsertItem(CWsfTreeItem *pItem)
{
	TVINSERTSTRUCT s;
	CString sTitle = pItem->GetTitle();

	s.hParent = pItem->m_pParent ? pItem->m_pParent->m_hTreeItem : TVI_ROOT;
	s.hInsertAfter = pItem->m_pPrevSibling ? pItem->m_pPrevSibling->m_hTreeItem : TVI_FIRST;
	s.item.mask = TVIF_STATE|TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_PARAM|TVIF_IMAGE|TVIF_CHILDREN;
	
	s.item.state = 0;
	if (pItem->m_dwFlags & CWsfTreeItem::EXPANDED) s.item.state |= TVIS_EXPANDED;
	if (pItem->m_dwFlags & CWsfTreeItem::BOLD) s.item.state |= TVIS_BOLD;

	s.item.stateMask = TVIS_EXPANDED | TVIS_BOLD;
	s.item.pszText = (char*) (const char*) sTitle;
	s.item.iImage = pItem->m_dwFlags & CWsfTreeItem::EXPANDED ? pItem->GetExpandedIcon(): pItem->GetIcon();
	s.item.iSelectedImage = s.item.iImage;
	s.item.cChildren = pItem->m_pChild ? 1:0;
	s.item.lParam = (LPARAM) pItem;

	pItem->m_hTreeItem = m_TreeCtrl.InsertItem(&s);
}

//*****************************************************************************
void CWsfTreeItemTreeView::BuildTree(CWsfTreeItem *pItem, bool bSubTree)
{
	InsertItem(pItem);

	// Insert childern

	if (bSubTree)
	{
		pItem = pItem->m_pChild;
		while (pItem)
		{
			BuildTree(pItem, true);
			pItem = pItem->m_pNextSibling;
		} 
	}
}

//*****************************************************************************
void CWsfTreeItemTreeView::UpdateTitleIcon(CWsfTreeItem *pItem, bool bSubTree)
{
	// Update

	m_TreeCtrl.SetItemText(pItem->m_hTreeItem, pItem->GetTitle());
	int i = pItem->m_dwFlags & CWsfTreeItem::EXPANDED ? pItem->GetExpandedIcon(): pItem->GetIcon();
	m_TreeCtrl.SetItemImage(pItem->m_hTreeItem, i, i);

	// UpdateChildern

	if (bSubTree)
	{
		pItem = pItem->m_pChild;
		while (pItem)
		{
			UpdateTitleIcon(pItem, true);
			pItem = pItem->m_pNextSibling;
		} 
	}

}

//*****************************************************************************
void CWsfTreeItemTreeView::RemoveDeletedChildern(CWsfTreeItem *pParent, bool bSubTree)
{
	HTREEITEM hItem, hItem2;
	CWsfTreeItem *pItem;

	hItem = m_TreeCtrl.GetChildItem(pParent->m_hTreeItem);
	pItem = pParent->m_pChild;

	if (pItem)
	{
		while (pItem)
		{
			while (pItem->m_hTreeItem != hItem)
			{
				hItem2 = hItem;
				hItem = m_TreeCtrl.GetNextSiblingItem(hItem);
				ASSERT(hItem);
				m_TreeCtrl.DeleteItem(hItem2);
			}
			
			if (bSubTree) RemoveDeletedChildern(pItem, true);
			pItem = pItem->m_pNextSibling;
			hItem = m_TreeCtrl.GetNextSiblingItem(hItem);
		}

		while (hItem)			// Remove tree items behind last CWsfTreeItem
		{
			hItem2 = hItem;
			hItem = m_TreeCtrl.GetNextSiblingItem(hItem);
			m_TreeCtrl.DeleteItem(hItem2);
		}
	}
	else
	{ 
		// Set normal not expanded icon

		pParent->m_dwFlags &= ~CWsfTreeItem::EXPANDED;
		int i = pParent->GetIcon();
		m_TreeCtrl.SetItemImage(pParent->m_hTreeItem, i, i);

		// Ensure there is no plus sign

		UpdatePlusSign(pParent);

		// No Wsf tree child item -> remove whole subtree in the tree ctrl

		while (hItem)
		{
			hItem2 = hItem;
			hItem = m_TreeCtrl.GetNextSiblingItem(hItem);
			m_TreeCtrl.DeleteItem(hItem2);
		}
	}
}

//*****************************************************************************
void CWsfTreeItemTreeView::AddNewChildern(CWsfTreeItem *pParent, bool bSubTree)
{
	BOOL bAdded = false;

	ASSERT_VALID(pParent);
	CWsfTreeItem *pItem = pParent->m_pChild;

	while (pItem)
	{
		if (!pItem->m_hTreeItem) 
		{
			BuildTree(pItem, true);
			bAdded = true;
		}
		else if (bSubTree) AddNewChildern(pItem, true);

		pItem = pItem->m_pNextSibling;
	}

	// Ensure the plus sign is there

	if (bAdded) UpdatePlusSign(pParent);
}

//*****************************************************************************
void CWsfTreeItemTreeView::TreeHasChanged(CWsfTreeItem *pItem, 
                                          CWsfApp::ETreeChanges eChange)
{
	HTREEITEM h;
	ASSERT_VALID(pItem);

	switch (eChange)
	{
	case CWsfApp::GENERAL:
		ASSERT(pItem->m_hTreeItem);
		m_TreeCtrl.DeleteItem(pItem->m_hTreeItem);
		BuildTree(pItem, true);
		break;

	case CWsfApp::ITEM_ADDED:
		ASSERT(pItem->m_hTreeItem == NULL);
		BuildTree(pItem, true);
		if (pItem->GetParent()) UpdatePlusSign(pItem->GetParent()); // Ensure the plus sign is there
		break;

	case CWsfApp::CHILDERN_ADDED:
		AddNewChildern(pItem, false);
		break;

	case CWsfApp::SUBTREE_CHILDERN_ADDED:
		AddNewChildern(pItem, true);
		break;

	case CWsfApp::SUBTREE_TITLE_ICON:
		UpdateTitleIcon(pItem, true);
		break;

	case CWsfApp::TITLE_ICON:
		UpdateTitleIcon(pItem, false);
		break;

	case CWsfApp::CHILDERN_DELETED:
		RemoveDeletedChildern(pItem, false);
		break;

	case CWsfApp::SUBTREE_CHILDERN_DELETED:
		RemoveDeletedChildern(pItem, true);
		break;
	}

	// Select root item if no item is selected

	if (!m_TreeCtrl.GetSelectedItem() && (h=m_TreeCtrl.GetRootItem())) 
	{
		m_TreeCtrl.Select(h, TVGN_CARET);
	}
}


//*****************************************************************************
void CWsfTreeItemTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch (lHint)
	{
	case WSF_UV_TREE_HAS_CHANGED:
		{
			CWsfTreeChangeInfo *pInfo = (CWsfTreeChangeInfo*) pHint;
			ASSERT_VALID(pInfo->pItem);
			if (GetRootItem() == pInfo->pItem->GetRoot())
				TreeHasChanged(pInfo->pItem, pInfo->eChange);
		}
		break;

	case WSF_UV_ENSURE_TI_VISIBLE:
		ASSERT(pHint);
		if (GetRootItem() == ((CWsfTreeItem*)pHint)->GetRoot())
			EnsureVisible((CWsfTreeItem*) pHint);
		break;

	default:
		CView::OnUpdate(pSender, lHint, pHint);
	}
}

//*****************************************************************************
void CWsfTreeItemTreeView::EnsureVisible(CWsfTreeItem *pItem)
{
	ASSERT(pItem && pItem->m_hTreeItem);
	m_TreeCtrl.EnsureVisible(pItem->m_hTreeItem);
	m_TreeCtrl.SelectItem(pItem->m_hTreeItem);
}

//*****************************************************************************
BOOL CWsfTreeItemTreeView::ExpandItem(CWsfTreeItem *pItem)
{
	ASSERT(pItem);
	return m_TreeCtrl.Expand(pItem->m_hTreeItem, TVE_EXPAND);
}

/*****************************************************************************/
CWsfTreeItem* CWsfTreeItemTreeView::GetSelectedTreeItem()
{
	HTREEITEM h = m_TreeCtrl.GetSelectedItem();
	if (h)
	{
		CWsfTreeItem* p = (CWsfTreeItem*)m_TreeCtrl.GetItemData(h);
		ASSERT_VALID(p);
		return p;
	}
	return NULL;
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	//TRACE1("TV: %s\n", bActivate ? "AC" : "DEAC");

	// Set active tree item

/*	if (bActivate)
	{
		HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
		WsfGetFrame()->SelectOneTreeItem(hItem ? (CWsfTreeItem*)m_TreeCtrl.GetItemData(hItem) : NULL);
	}
	else WsfGetFrame()->SelectOneTreeItem(NULL);*/

	if (bActivate) WsfGetFrame()->m_StatusBarWnd.SetPaneText(0,"");	
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	m_TreeCtrl.SetFocus();
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// The message must have come because of a key stroke
	if (point.x != -1 || point.y != -1) return;

	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if (!hItem) return;

	// Get position & limit it to be inside the window
	
	RECT r, r2;
	POINT p;

	VERIFY(m_TreeCtrl.GetItemRect(hItem, &r, true));
	GetClientRect(&r2);
	p.x = min(max(r.left,0),r2.right);
	p.y = min(max(r.bottom,0),r2.bottom);

	// Display menu!

	m_TreeCtrl.ClientToScreen(&p);
	OnRclick(p.x, p.y);
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnRclick(int nX, int nY) 
{
	CWslArray<int> a;
	CWsfTreeItem* pItem = GetSelectedTreeItem();
	ASSERT(pItem);

	a += pItem->GetContextMenu();

	HMENU hMenu = WsfGetApp()->BuildPopupMenu(a, pItem->GetDefaultCommand());
	ASSERT(hMenu);

	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
		nX, nY, 0, WsfGetFrame()->m_hWnd, NULL);

	DestroyMenu(hMenu);
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnDblclk(NMHDR *,LRESULT *) 
{
	TVHITTESTINFO s;

	// Test if it was clicked on some item.

	GetCursorPos(&s.pt);
	m_TreeCtrl.ScreenToClient(&s.pt);
	m_TreeCtrl.HitTest(&s);
	if (!(s.flags & TVHT_ONITEM))  return;

	// Open selected item (item is is selected, because NM_SELCHANGED was sent)
	// PostMessage is used, because dblclk sets focus back to the tree ctrl

	CWsfTreeItem* p = GetSelectedTreeItem();
	ASSERT_VALID(p);
	WsfGetFrame()->PostMessage(WM_COMMAND, p->GetDefaultCommand(), 0);
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnSize(UINT nType, int cx, int cy) 
{
	m_TreeCtrl.SetWindowPos(NULL, 0,0, cx,cy, SWP_NOMOVE|SWP_NOZORDER);
	CView::OnSize(nType, cx, cy);
}

//*****************************************************************************
BOOL CWsfTreeItemTreeView::OnBeginLabelEdit(NMTVDISPINFO *pInfo)
{
	CWsfTreeItem *pItem = (CWsfTreeItem*) m_TreeCtrl.GetItemData(pInfo->item.hItem);
	CEdit *pEdit = m_TreeCtrl.GetEditControl();

	return ! pItem->OnRename(pEdit, NULL);
}

//*****************************************************************************
BOOL CWsfTreeItemTreeView::OnEndLabelEdit(NMTVDISPINFO *pInfo)
{
	if (!pInfo->item.pszText) return false;
	CWsfTreeItem *pItem = (CWsfTreeItem*) m_TreeCtrl.GetItemData(pInfo->item.hItem);
	BOOL b = pItem->OnRename(NULL, pInfo->item.pszText);

	// If new label was not accepted, repeat the editing
	
	if (!b) m_TreeCtrl.EditLabel(pItem->m_hTreeItem);
	
	// Notify of the change

	if (b) WsfGetApp()->TreeHasChanged(pItem, CWsfApp::TITLE_ICON);

	return b;
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnItemExpanded(NMTREEVIEW *pInfo)
{
	int i;
	CWsfTreeItem *pItem = (CWsfTreeItem*) m_TreeCtrl.GetItemData(pInfo->itemNew.hItem);

	if (pInfo->action == TVE_EXPAND)
	{
		i = pItem->GetExpandedIcon();
		pItem->m_dwFlags |= CWsfTreeItem::EXPANDED;
	}
	else
	{
		i = pItem->GetIcon();
		pItem->m_dwFlags &= ~CWsfTreeItem::EXPANDED;
	}

	m_TreeCtrl.SetItemImage(pInfo->itemNew.hItem, i, i);
}

/*****************************************************************************
*
* DRAG & DROP
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTreeItemTreeView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	OnDragLeave();
	CWsfTreeItem *pItem = (CWsfTreeItem*) m_TreeCtrl.GetItemData(m_hLastItem);
	ASSERT_VALID(pItem);
	pItem->OnDropDataObject(pDataObject, dropEffect);
	return true;
}

//*****************************************************************************
DROPEFFECT CWsfTreeItemTreeView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// NOTE:	This function is called at constant rate and is called ALWAYS, so 
	//				it must be fast.

	HTREEITEM hItem;
	CWsfTreeItem *pItem;
	DWORD de;
	bool bUpdate = false;

	// Expand the node if the cursor is idle for the given time.

	if (m_hLastItem && (GetTickCount() - m_dwTime > WSF_TIME_TO_NODE_EXPAND))
	{
		m_dwTime = GetTickCount();
		m_TreeCtrl.Expand(m_hLastItem, TVE_EXPAND);
	}

	// Detect some change

	if (m_dwLastKeyState != dwKeyState)
	{
		m_dwLastKeyState = dwKeyState;
		bUpdate = true;
	}

	if (m_LastPoint != point) 
	{
		m_LastPoint = point;
		hItem = m_TreeCtrl.HitTest(point);

		if (hItem != m_hLastItem)
		{
			bUpdate = true;
			if (m_hLastItem) m_TreeCtrl.SetItemState(m_hLastItem, 0, TVIS_DROPHILITED);
			m_hLastItem = hItem;
			if (!hItem) return m_LastDropEffect = DROPEFFECT_NONE;
			m_TreeCtrl.SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
			m_dwTime = GetTickCount();
		}
	}

	// If something has changed, update the drop effect 

	if (bUpdate && m_hLastItem)
	{
		pItem = (CWsfTreeItem*) m_TreeCtrl.GetItemData(m_hLastItem);
		ASSERT_VALID(pItem);
		de = pItem->OnAcceptDataObject(pDataObject);

		switch (dwKeyState & (~(MK_LBUTTON|MK_RBUTTON|MK_MBUTTON)) )
		{
		case 0:
			m_LastDropEffect = (de & DROPEFFECT_MOVE) ? DROPEFFECT_MOVE :
				(de & DROPEFFECT_COPY) ? DROPEFFECT_COPY : DROPEFFECT_NONE;
			break;
		case MK_ALT:
			m_LastDropEffect = (de & DROPEFFECT_MOVE) ? DROPEFFECT_MOVE : DROPEFFECT_NONE;
			break; 
		case MK_CONTROL:
			m_LastDropEffect = (de & DROPEFFECT_COPY) ? DROPEFFECT_COPY : DROPEFFECT_NONE;
			break;
		case MK_CONTROL | MK_SHIFT:
			m_LastDropEffect = (de & DROPEFFECT_LINK) ? DROPEFFECT_LINK : DROPEFFECT_NONE;
			break;
		default:
			TRACE1("D&D: Unsupported dwKeyState: %d\n", dwKeyState);
			m_LastDropEffect = DROPEFFECT_NONE;
			break;
		}
	}

	return m_LastDropEffect;
}

//*****************************************************************************
DROPEFFECT CWsfTreeItemTreeView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_LastPoint.x = -1;
	m_LastPoint.y = -1;
	m_hLastItem = NULL;
	m_dwLastKeyState = 0;
	m_LastDropEffect = DROPEFFECT_NONE;
	return OnDragOver(pDataObject, dwKeyState, point);
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnDragLeave() 
{
	if (m_hLastItem) m_TreeCtrl.SetItemState(m_hLastItem, 0, TVIS_DROPHILITED);
}

/*****************************************************************************
*
* NOTIFY HANDLING
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTreeItemTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	switch ( ((NMHDR*)lParam) -> code )
	{
	case WSF_TREECTRL_NM_ENTER:
      {
	      CWsfTreeItem* p = GetSelectedTreeItem();
	      ASSERT_VALID(p);
	      WsfGetFrame()->PostMessage(WM_COMMAND, p->GetDefaultCommand(), 0);
		   *pResult = true;
		   break;
      }

	case NM_DBLCLK: 
		OnDblclk(NULL, NULL); 
		*pResult = 1;
		break;

	case NM_RCLICK:	
		{
			// Check if there is an item on the cursor pos.
			// If yes, select it and display the context menu.
			TVHITTESTINFO s;
			POINT p;
			
			VERIFY(GetCursorPos(&s.pt));
			p = s.pt;
			m_TreeCtrl.ScreenToClient(&s.pt);
			m_TreeCtrl.HitTest(&s);
			
			if (!(s.flags & TVHT_ONITEM)) break;

			m_TreeCtrl.SelectItem(s.hItem);
			OnRclick(p.x, p.y);
			
			*pResult = 0;
		}
		break;

	case TVN_SELCHANGED:
      {
         // Make the path from the selected item to the root bold

         HTREEITEM h;
         CWslArray<HTREEITEM> a;

         h = ((NM_TREEVIEW*)lParam)->itemNew.hItem;
         for (; h; h=m_TreeCtrl.GetParentItem(h))
         {
            a += h;
            m_TreeCtrl.SetItemState(h, TVIS_BOLD, TVIS_BOLD);
         }

         // Make the old path normal
         // Optimalization: stop when the new and the old paths meet 

         h = ((NM_TREEVIEW*)lParam)->itemOld.hItem;
         for (; h; h=m_TreeCtrl.GetParentItem(h))
         {
            for (int i=0; i<a.GetSize(); i++)
            {
               if (a[i] == h) { h = NULL; break; }
            }
            if (h) m_TreeCtrl.SetItemState(h, 0, TVIS_BOLD);
         }
      }
		break;

	case TVN_ITEMEXPANDED:
		OnItemExpanded((NMTREEVIEW*) lParam);
		break;

	case TVN_BEGINLABELEDIT:
		// We must test if the rename command is not disabled, when renaming
		// is invoked by mouse click on an item
		*pResult = WsfGetFrame()->IsCmdEnabled(ID_WSF_RENAME) ? 
			OnBeginLabelEdit((LPNMTVDISPINFO) lParam) : true;
		break;

	case TVN_ENDLABELEDIT:
		*pResult = OnEndLabelEdit((LPNMTVDISPINFO) lParam); 
		break;

	case WSF_TREECTRL_NM_DELETE:
		if (GetSelectedTreeItem())
		{
			WsfGetFrame()->SendMessage(WM_COMMAND, ID_WSF_DELETE, 0);
		}
		break;

	case TVN_BEGINRDRAG:
	case TVN_BEGINDRAG:
		{
			m_TreeCtrl.SelectItem(((NM_TREEVIEW*)lParam)->itemNew.hItem);
			CWslArray<CWsfTreeItem*> a;
			a += GetSelectedTreeItem();
			WsfGetApp()->DoTreeItemsDragDrop(a);
		}
		break;

	case TVN_DELETEITEM:
		// NOTE: This MUST be handled, it is sent when the UI tree
		// is being updated to match the WSF tree.
		// If it is not handled, it is finally routed to the
		// OnCmdMsg() and there GetSelectedItem() fails, because
		// the UI item being deleted can be selected and points
		// to the already deleted WSF item!!!!
		break;

   case TTN_NEEDTEXT:
      *pResult = WsfGetFrame()->SendMessage(WM_NOTIFY, wParam, lParam);
      return true;
	
   default:	
		return CView::OnNotify(wParam, lParam, pResult);
	}

	return true;
}

/*****************************************************************************
*
* COMMAND ROUTING
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWsfTreeItemTreeView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{ 
	BOOL bHandled = false;

	// Send command to the selected tree item, if any

	CWsfTreeItem* pItem = GetSelectedTreeItem();

	if (pItem)
	{
		ASSERT_VALID(pItem);

		CWsfTISelContext sc;
		sc.dwCHFlags = CWsfTISelContext::FIRST | CWsfTISelContext::LAST;
		sc.nSelCount = 1;
		sc.nSelPos = 0;

		// NOTE: The current selection context must be preserved, because
		// the this could have been called from a command handler!
		CWsfTISelContext* psc = pItem->m_pSelContext;
		pItem->m_pSelContext = &sc;

		bHandled = pItem->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

		pItem->m_pSelContext = psc;
	}

	// Send it to the doc/view
	
	if (!bHandled) bHandled = CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	return bHandled;
}

/*****************************************************************************
*
* COMMAND HANDLING
*
*****************************************************************************/


//*****************************************************************************
void CWsfTreeItemTreeView::OnWsfRename() 
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	ASSERT(hItem);	// Some item must be active, otherwise we wouldn't get this message
	CWsfTreeItem *pItem = (CWsfTreeItem*)m_TreeCtrl.GetItemData(hItem);

	// Edit 

	m_TreeCtrl.EnsureVisible(hItem);
	m_TreeCtrl.EditLabel(hItem);
}

//*****************************************************************************
void CWsfTreeItemTreeView::OnWsfDelete() 
{
	CWsfTreeItem* p = GetSelectedTreeItem();
	ASSERT_VALID(p);

	CString s;
	s.LoadString(p->GetChild() ? IDS_DELETE_SUBTREE : IDS_DELETE_ITEM);

	if (AfxMessageBox(s, MB_ICONEXCLAMATION|MB_YESNO) == IDYES)
		WsfGetApp()->DeleteTreeItem(p, true, 0);
}




