///////////////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2008 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfCmdUI.h"
#include "WsfTreeItemSelectionContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTreeItemListView, CView)
	//{{AFX_MSG_MAP(CWsfTreeItemListView)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_WSF_DELETE, OnWsfDelete)
	ON_COMMAND(ID_WSF_RENAME, OnWsfRename)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_MESSAGE(WSF_WM_UPDATE_MESSAGE_TEXT, OnWSF_WM_UPDATE_MESSAGE_TEXT)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfTreeItemListView::CWsfTreeItemListView(CWsfTreeItem* pItem)
{
	ASSERT_VALID(pItem);
	m_pRootTreeItem = pItem;
}

//*****************************************************************************
CWsfTreeItemListView::~CWsfTreeItemListView()
{
}

//*****************************************************************************
BOOL CWsfTreeItemListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	BOOL b = CView::PreCreateWindow(cs);
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return b;
}

//*****************************************************************************
int CWsfTreeItemListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;

	// Init list control

	m_ListCtrl.Create(WS_CHILD | WS_VISIBLE | LVS_EDITLABELS | LVS_SHOWSELALWAYS |
		LVS_ALIGNTOP | LVS_AUTOARRANGE | WsfGetApp()->m_Setup.nListViewType, 
		WsfZeroRect, this, 1001);

	m_ListCtrl.SetImageList(WsfGetApp()->GetNormalIconList(), LVSIL_NORMAL);
	m_ListCtrl.SetImageList(WsfGetApp()->GetSmallIconList(), LVSIL_SMALL);

   // The list control paints the trasparent color white. 
   // This line makes it again transparent!
	WsfGetApp()->GetSmallIconList()->SetBkColor(CLR_NONE);
	WsfGetApp()->GetNormalIconList()->SetBkColor(CLR_NONE);

	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_SUBITEMIMAGES);

	//  Associate this window with the COleDropTarget class

	VERIFY(m_dt.Register(this));		// OLE must me initialized!

	return 0;
}

/*****************************************************************************/
void CWsfTreeItemListView::SetTreeItem(CWsfTreeItem* pItem, 
                                   CWsfTreeItem* pSelectItem /*= NULL*/)
{
	ASSERT_VALID(pItem);
	m_pRootTreeItem = pItem;

	WsfGetApp()->SetViewTitleIcon(WsfGetApp()->GetViewInfo(this), 
		pItem->GetPathTitle(),
		pItem->GetExpandedIcon());

	BuildList(pSelectItem);
}

//*****************************************************************************
void CWsfTreeItemListView::BuildList(CWsfTreeItem* pSelectItem)
{
	LVITEM li;
	CString str;
	CWsfTreeItem* p = m_pRootTreeItem->m_pChild;

	TRACE1("CWsfTreeItemListView::BuildList(%d)\n", pSelectItem);

	// Build list. InsertItem sends NOTIFY msg, so selection
	// will be updated.	
	
	m_ListCtrl.DeleteAllItems();		// Doesn't send NOTIFY msg
	m_aSelectedItems.RemoveAll();
	
	// If no item should be selected, select the first item by default

	if (!pSelectItem) pSelectItem = p;
	
	//---- ONLY TITLE ----------------------------------------------
	
	li.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	li.iItem = 0;
	li.iSubItem = 0;
	li.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		
	BOOL bSelected = false;

	while (p)
	{
		str = p->GetTitle();
		li.pszText = (char*) (const char*) str;
		li.iImage = p->GetIcon();
		li.lParam = (LPARAM) p;

		if (p==pSelectItem)
		{
			li.state = LVIS_SELECTED|LVIS_FOCUSED;
			bSelected = true;
		}
		else li.state = 0;
			
		// NOTE: Strange behavior! When the view is
		// opened InsertItem() doesn't send ItemChanged notification.
		// But later, e.g. new item is created, it does.

		m_ListCtrl.InsertItem(&li);
		p = p->m_pNextSibling;
		li.iItem++;
	}

	// Select the item selected in the control, if no item
	// is selected, select the root item

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (pos)
	{
		p = (CWsfTreeItem*) m_ListCtrl.GetItemData(m_ListCtrl.GetNextSelectedItem(pos));
		ASSERT_VALID(p);
		SelectOneTreeItem(p);
	}
	else SelectOneTreeItem(m_pRootTreeItem);

	// Update message text

	SetMessageString();
}

//*****************************************************************************
void CWsfTreeItemListView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
//	POSITION p;
	//CString s; m_pRootTreeItem->GetTitle(s); TRACE2("LV \"%s\": %s\n", s, bActivate ? "AC" : "DEAC");

	SetMessageString(!bActivate);
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//*****************************************************************************
void CWsfTreeItemListView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	m_ListCtrl.SetFocus();
}

//*****************************************************************************
void CWsfTreeItemListView::OnSize(UINT nType, int cx, int cy) 
{
	m_ListCtrl.SetWindowPos(NULL, 0,0, cx,cy, SWP_NOMOVE|SWP_NOZORDER);
	CView::OnSize(nType, cx, cy);
}

//*****************************************************************************
void CWsfTreeItemListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
//	TRACE0("LV.OnUpdate\n");

	switch (lHint)
	{
	case 0:
		// default implementation calls by OnInitialUpdate()
		BuildList(NULL);
		break;

	case WSF_UV_TREE_HAS_CHANGED:
		{
			CWsfTreeChangeInfo *pInfo = (CWsfTreeChangeInfo*) pHint;
			ASSERT_VALID(pInfo->pItem);

			switch (pInfo->eChange)
			{
			case CWsfApp::SUBTREE_CHILDERN_DELETED:
			case CWsfApp::GENERAL:
			case CWsfApp::SUBTREE_CHILDERN_ADDED:
				if (pInfo->pItem->IsInSubTree(m_pRootTreeItem)) BuildList(NULL);
				break;
				
			case CWsfApp::CHILDERN_ADDED:
				if (pInfo->pItem== m_pRootTreeItem) BuildList(NULL);
				break;
				
			case CWsfApp::ITEM_ADDED:
			case CWsfApp::TITLE_ICON:
				if (pInfo->pItem->GetParent() == m_pRootTreeItem) BuildList(NULL);
				break;
				
			case CWsfApp::CHILDERN_DELETED:
				if (pInfo->pItem == m_pRootTreeItem) BuildList(NULL);
				break;
			}
		}
		break;

	case WSF_UV_LIST_VIEW_TYPE_CHANGED:
		m_ListCtrl.ModifyStyle(LVS_LIST|LVS_SMALLICON, WsfGetApp()->m_Setup.nListViewType);
		break;
	}
}

//*****************************************************************************
void CWsfTreeItemListView::OnRclick(int x, int y)
{
	ASSERT(m_aSelectedItems.GetSize() > 0);
	CWslArray<int> a(m_aSelectedItems.GetSize());

	for (int i=0; i<m_aSelectedItems.GetSize(); i++) 
		a[i] = m_aSelectedItems[i]->GetContextMenu();

	HMENU hMenu = WsfGetApp()->BuildPopupMenu(a, m_aSelectedItems[0]->GetDefaultCommand());

	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
		x, y, 0, WsfGetFrame()->m_hWnd, NULL);

	DestroyMenu(hMenu);
}

//*****************************************************************************
void CWsfTreeItemListView::OnItemChanged(NMLISTVIEW &lv)
{
	TRACE0("CWsfTreeItemListView::OnItemChanged()\n");
	if (!(lv.uOldState & LVIS_SELECTED) && (lv.uNewState & LVIS_SELECTED))
	{
		UnselectTreeItem(m_pRootTreeItem);
		SelectTreeItem((CWsfTreeItem*)m_ListCtrl.GetItemData(lv.iItem));
		SetMessageString();
	}
	else if ((lv.uOldState & LVIS_SELECTED) && !(lv.uNewState & LVIS_SELECTED))
	{
		UnselectTreeItem((CWsfTreeItem*)m_ListCtrl.GetItemData(lv.iItem));
		// If there is no selection, select the parent tree item.
		// e.g. when user clicks in the list but on no item, the context
		// menu of the parent is shown
		if (m_aSelectedItems.IsEmpty()) SelectTreeItem(m_pRootTreeItem);
		SetMessageString();
	}
}

//*****************************************************************************
BOOL CWsfTreeItemListView::OnBeginLabelEdit(NMLVDISPINFO *pInfo)
{
	CWsfTreeItem* pItem = (CWsfTreeItem*) m_ListCtrl.GetItemData(pInfo->item.iItem);
	CEdit *pEdit = m_ListCtrl.GetEditControl();

	return ! pItem->OnRename(pEdit, NULL);
}

//*****************************************************************************
BOOL CWsfTreeItemListView::OnEndLabelEdit(NMLVDISPINFO *pInfo)
{
	if (!pInfo->item.pszText) return false;
	CWsfTreeItem* pItem = (CWsfTreeItem*) m_ListCtrl.GetItemData(pInfo->item.iItem);
	BOOL b = pItem->OnRename(NULL, pInfo->item.pszText);

	// If new label was not accepted, repeat the editing
	
	if (!b) m_ListCtrl.EditLabel(pInfo->item.iItem);
	
	// Notify of the change

	if (b) WsfGetApp()->TreeHasChanged(pItem, CWsfApp::TITLE_ICON);

	return b;
}


/*****************************************************************************
*
* TREE ITEM SELECTION
*
*****************************************************************************/

//*****************************************************************************
void CWsfTreeItemListView::SelectTreeItem(CWsfTreeItem* pItem)
{
	ASSERT_VALID(pItem);

	m_aSelectedItems.GrowBy(1);
	m_aSelectedItems.GetLast() = pItem;
}

//*****************************************************************************
void CWsfTreeItemListView::UnselectTreeItem(CWsfTreeItem* pItem)
{
	ASSERT_VALID(pItem);

	for (int i=0; i < m_aSelectedItems.GetSize(); i++)
	{
		if (m_aSelectedItems[i] == pItem)
		{
			m_aSelectedItems.RemoveAt(i);
			break;
		}
	}
}

//*****************************************************************************
void CWsfTreeItemListView::SelectOneTreeItem(CWsfTreeItem* pItem)
{
	if (pItem)
	{
		ASSERT_VALID(pItem);
		m_aSelectedItems.SetSize(1);
		m_aSelectedItems[0] = pItem;
	}
	else m_aSelectedItems.RemoveAll();
}

/*****************************************************************************
*
* DRAG & DROP
*
*****************************************************************************/

//*****************************************************************************
DROPEFFECT CWsfTreeItemListView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// NOTE:	This function is called at constant rate and is called ALWAYS, so 
	//				it must be fast.

	int nItem;
	CWsfTreeItem* pItem;
	DWORD de;
	bool bUpdate = false;

	// Detect some change

	if (m_dwLastKeyState != dwKeyState)
	{
		m_dwLastKeyState = dwKeyState;
		bUpdate = true;
	}

	if (m_LastPoint != point) 
	{
		m_LastPoint = point;
		nItem = m_ListCtrl.HitTest(point);

		if (nItem != m_nLastItem) 
		{
			bUpdate = true;
			if (m_nLastItem != -1) m_ListCtrl.SetItemState(m_nLastItem, 0, LVIS_DROPHILITED);
			if (nItem != -1) m_ListCtrl.SetItemState(nItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
			m_nLastItem = nItem;
		}
	}

	// If something has changed, update the drop effect 

	if (bUpdate)
	{
		pItem = m_nLastItem != -1 ? (CWsfTreeItem*)m_ListCtrl.GetItemData(m_nLastItem) : m_pRootTreeItem;
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
BOOL CWsfTreeItemListView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CWsfTreeItem* pItem;

	OnDragLeave();
	pItem = m_nLastItem!=-1 ? (CWsfTreeItem*) m_ListCtrl.GetItemData(m_nLastItem) : m_pRootTreeItem;
	ASSERT_VALID(pItem);
	pItem->OnDropDataObject(pDataObject, dropEffect);
	return true;
}

//*****************************************************************************
DROPEFFECT CWsfTreeItemListView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_LastPoint.x = -1;
	m_LastPoint.y = -1;
	m_nLastItem = -1;
	m_dwLastKeyState = 0;
	return OnDragOver(pDataObject, dwKeyState, point);
}

//*****************************************************************************
void CWsfTreeItemListView::OnDragLeave() 
{
	if (m_nLastItem != -1) m_ListCtrl.SetItemState(m_nLastItem, 0, TVIS_DROPHILITED);
}

/*****************************************************************************
*
* NOTIFY
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTreeItemListView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMLISTVIEW &lv = *(NMLISTVIEW*) lParam;

	switch ( ((NMHDR*)lParam) -> code )
	{
   case CWsfListCtrl::KEY_ENTER:
			if (m_aSelectedItems.GetSize() > 1) break;
			// continue

		case NM_DBLCLK: 
			if (!m_aSelectedItems.IsEmpty())
			{
				WsfGetFrame()->SendMessage(WM_COMMAND, 
					m_aSelectedItems[0]->GetDefaultCommand(),
					0);
			}
			break;

		case NM_RCLICK:	
			{
				POINT p;
				GetCursorPos(&p);
				OnRclick(p.x, p.y);
			}
			break;

		case LVN_BEGINLABELEDIT:
			// We must test if the rename command is not disabled, when renaming
			// is invoked by mouse click on an item
			*pResult = WsfGetFrame()->IsCmdEnabled(ID_WSF_RENAME) ? 
				OnBeginLabelEdit((NMLVDISPINFO*) lParam) : true;
			break;

		case LVN_ENDLABELEDIT:
			*pResult = OnEndLabelEdit((NMLVDISPINFO*) lParam); 
			break;

		case LVN_ITEMCHANGED:
			if (WsfGetFrame()->GetActiveView() == this) OnItemChanged(lv);
			break;

		case CWsfListCtrl::KEY_DELETE:
			if (!m_aSelectedItems.IsEmpty())
			{
				WsfGetFrame()->SendMessage(WM_COMMAND, ID_WSF_DELETE, 0);
			}
			break;

		case CWsfListCtrl::KEY_BACKSPACE:
			if (m_pRootTreeItem->GetParent()) 
				SetTreeItem(m_pRootTreeItem->GetParent(), m_pRootTreeItem);
			break;

		case LVN_BEGINRDRAG:
		case LVN_BEGINDRAG:
			WsfGetApp()->DoTreeItemsDragDrop(m_aSelectedItems);
			break;

		default:	
			return CView::OnNotify(wParam, lParam, pResult);
	}

	return true;
}

/*****************************************************************************
*
* COMMAND HANDLING
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWsfTreeItemListView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	int i;
	BOOL bHandled = false;

  //---------------------------------------------------------------------------
	// Send command to all selected items
  //---------------------------------------------------------------------------

	if (!m_aSelectedItems.IsEmpty())
	{
		// Init tree item selection context

		CWsfTISelContext sc;
		sc.nSelCount = m_aSelectedItems.GetSize();
		sc.dwCHFlags = m_aSelectedItems.GetSize() > 1 ? 
			CWsfTISelContext::MULTI | CWsfTISelContext::FIRST : 
			CWsfTISelContext::FIRST;

		if (nCode == CN_UPDATE_COMMAND_UI)
		{
			// Wrap the CmdUI in pExtra to be able to check
			// if the is disabled or enabled
			CWsfCmdUI cmdui((CCmdUI*)pExtra);

			for (i=0; i<m_aSelectedItems.GetSize(); i++)
			{	
				// Force disabling of the following commands on the
				// root item, because they cannot be handled -> failure!
				
				if (m_aSelectedItems[i]==m_pRootTreeItem)
				{
					if (nID==ID_WSF_OPEN || nID==ID_WSF_DELETE || nID==ID_WSF_RENAME)
					{
						bHandled = true;
						if (pExtra) cmdui.Enable(false);
						break;
					}
				}

				// Call handler

				if (i==sc.nSelCount-1) sc.dwCHFlags |= CWsfTISelContext::LAST;
				sc.nSelPos = i;

				// NOTE: The current selection context must be preserved, because
				// the this could have been called from a command handler!
				CWsfTISelContext* psc = m_aSelectedItems[i]->m_pSelContext;
				m_aSelectedItems[i]->m_pSelContext = &sc;
				
				bHandled |= m_aSelectedItems[i]->OnCmdMsg(nID, nCode, (void*) &cmdui, pHandlerInfo);
				
				m_aSelectedItems[i]->m_pSelContext = psc;

				// If the command was not handled, exit the loop.
				// If the item disabled the command, stop sending the message,
				// because other items could enable it again, but we want it to stay disabled.
				
				if (!bHandled) break;
				if (!cmdui.IsEnabled()) break;	

				if (i==0) sc.dwCHFlags &= ~CWsfTISelContext::FIRST;
			}
		}
		else if (nCode == CN_COMMAND)
		{
			// If not NULL, MFC is checking if there is a handler.
			// We don't want that checking, so skip it and answer there is no handler.
			// MFC will disable the command.
			
			if (pHandlerInfo) return false;

			// Copy of the selection is necessary, because command handler can change it
			// (eg. when TreeItemView is activated is rebuilds the selection and
			// can change the order)
			
			CWslArray<CWsfTreeItem*> aSel;
			aSel = m_aSelectedItems;

			for (i=0; i<aSel.GetSize() && !(sc.dwCHFlags & CWsfTISelContext::CANCEL); i++)
			{
				if (i==sc.nSelCount-1) sc.dwCHFlags |= CWsfTISelContext::LAST;
				sc.nSelPos = i;

				// NOTE: The current selection context must be preserved, because
				// the this could have been called from a command handler!
				CWsfTISelContext* psc = aSel[i]->m_pSelContext;	
				aSel[i]->m_pSelContext = &sc;
				
				bHandled |= aSel[i]->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

				aSel[i]->m_pSelContext = psc;
				if (i==0) sc.dwCHFlags &= ~CWsfTISelContext::FIRST;
			}
		}
	}

  //---------------------------------------------------------------------------
	// Send it to the doc/view
  //---------------------------------------------------------------------------
	
	if (!bHandled) bHandled =  CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	return bHandled;
}


/*****************************************************************************
*
* COMMAND HANDLERS
*
*****************************************************************************/

//*****************************************************************************
void CWsfTreeItemListView::OnWsfDelete() 
{
	CString s;
	CWslArray<CWsfTreeItem*> aItems;
	aItems = m_aSelectedItems;
	int i = aItems.GetSize();
	ASSERT(i > 0);
	
	// Confirmation the delete operation

	if (i > 1)
	{
		s.Format(i < 5 ? IDS_REALLY_DELETE_2_TO_4 : IDS_REALLY_DELETE_5_MORE, i);
		if (AfxMessageBox(s, MB_ICONEXCLAMATION|MB_YESNO) != IDYES) return;
	}
	else
	{
		s.LoadString(aItems[0]->GetChild() ? IDS_DELETE_SUBTREE : IDS_DELETE_ITEM);
		if (AfxMessageBox(s, MB_ICONEXCLAMATION|MB_YESNO) != IDYES) return;
	}

	// Delete without updating views

	for (; i--;) 
	{
		ASSERT_VALID(aItems[i]);
		WsfGetApp()->DeleteTreeItem(aItems[i], false, 0);
	}

	// Update all views

	WsfGetApp()->TreeHasChanged(m_pRootTreeItem, CWsfApp::CHILDERN_DELETED);
}

//*****************************************************************************
void CWsfTreeItemListView::OnWsfRename() 
{
	POSITION p = m_ListCtrl.GetFirstSelectedItemPosition();
	ASSERT(p);
	m_ListCtrl.EditLabel(m_ListCtrl.GetNextSelectedItem(p));
}

//*****************************************************************************
void CWsfTreeItemListView::OnEditSelectAll()
{
	int n = m_ListCtrl.GetItemCount();
	
	for (int i = 0; i<n; i++)
	{
		m_ListCtrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

//*****************************************************************************
void CWsfTreeItemListView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{ 
	pCmdUI->Enable(true);	
}

//*****************************************************************************
LRESULT CWsfTreeItemListView::OnWSF_WM_UPDATE_MESSAGE_TEXT(WPARAM wParam, LPARAM lParam)
{
	CString *p = (CString*) wParam;
	int n = m_aSelectedItems.GetSize();

	if (n > 0 && m_aSelectedItems[0] != m_pRootTreeItem)
	{
		p->Format(IDS_TOTAL_AND_SELECTED_ITEMS, 
			m_ListCtrl.GetItemCount(), n);
	}
	else
	{
		p->Format(IDS_TOTAL_ITEMS, m_ListCtrl.GetItemCount());
	}

	return true;
}

//*****************************************************************************
void CWsfTreeItemListView::SetMessageString(bool bEmpty /*= false*/)
{
	CString s;
	
	if (bEmpty) 
	{
		WsfGetFrame()->SetMessageText(AFX_IDS_IDLEMESSAGE);
	}
	else
	{
		OnWSF_WM_UPDATE_MESSAGE_TEXT((WPARAM) &s, 0);
		WsfGetFrame()->SetMessageText(s);
	}
}

/*****************************************************************************/
void CWsfTreeItemListView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// The message must have come because of a key stroke
	if (point.x != -1 || point.y != -1) return;

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	// Get position & limit it to be inside the window
	
	RECT r, r2;
	POINT p;

	VERIFY(m_ListCtrl.GetItemRect(m_ListCtrl.GetNextSelectedItem(pos), &r, true));
	GetClientRect(&r2);
	p.x = min(max(r.left,0),r2.right);
	p.y = min(max(r.bottom,0),r2.bottom);

	// Display menu!

	m_ListCtrl.ClientToScreen(&p);
	OnRclick(p.x, p.y);
}

