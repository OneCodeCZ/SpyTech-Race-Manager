///////////////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 12
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfPropertyPage.h"
#include "WsfPropertySheet.h"
#include "WsfTreeItemSelectionContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTreeItem, CCmdTarget)
	//{{AFX_MSG_MAP(CWsfTreeItem)
	ON_UPDATE_COMMAND_UI(ID_WSF_RENAME, OnUpdateWsfRename)
	ON_UPDATE_COMMAND_UI(ID_WSF_OPEN, OnUpdateWsfOpen)
	ON_UPDATE_COMMAND_UI(ID_WSF_HELP, OnUpdateWsfHelp)
	ON_UPDATE_COMMAND_UI(ID_WSF_DELETE, OnUpdateWsfDelete)
	ON_UPDATE_COMMAND_UI(ID_WSF_PROPERTIES, OnUpdateWsfProperties)
	ON_COMMAND(ID_WSF_PROPERTIES, OnWsfProperties)
	ON_COMMAND(ID_WSF_HELP, OnWsfHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define CURRENT_SERIALIZATION_VERSION		1
WSL_IMPLEMENT_SERIAL_ABSTRACT(CWsfTreeItem, CCmdTarget, 1)


//*****************************************************************************
CWsfTreeItem::CWsfTreeItem()
{
	m_pParent = NULL;
	m_pChild = NULL;
	m_pNextSibling = NULL;
	m_pPrevSibling = NULL;
	m_hTreeItem = NULL;
	m_dwFlags = 0;
	m_pSelContext = NULL;
}

//*****************************************************************************
CWsfTreeItem::CWsfTreeItem(int nContextMenuId, int nContextMenuIdx,
													 int nIconId, int nIconIdx,
													 int nExpandedIconId, int nExpandedIconIdx,
													 DWORD	dwFlags)
{
	m_pParent = NULL;
	m_pChild = NULL;
	m_pNextSibling = NULL;
	m_pPrevSibling = NULL;
	m_hTreeItem = NULL;
	m_pSelContext = NULL;

	Init(nContextMenuId, nContextMenuIdx, nIconId, nIconIdx,
		nExpandedIconId, nExpandedIconIdx, dwFlags);
}

//*****************************************************************************
CWsfTreeItem::CWsfTreeItem(const CWsfTreeItem& item)
{
	if (this == &item) return;

	m_dwFlags = item.m_dwFlags;
	m_dwMenu = item.m_dwMenu;
	m_dwIcon = item.m_dwIcon;
	m_dwExpandedIcon = item.m_dwExpandedIcon;

	m_pParent = NULL;
	m_pChild = NULL;
	m_pNextSibling = NULL;
	m_pPrevSibling = NULL;

	m_hTreeItem = NULL;
	m_pSelContext = NULL;
}

//*****************************************************************************
void CWsfTreeItem::Init(int nContextMenuId, int nContextMenuIdx,
												int nIconId, int nIconIdx,	
												int nExpandedIconId, int nExpandedIconIdx,
												DWORD	dwFlags /*=0*/)
{
	m_dwMenu = nContextMenuId + (nContextMenuIdx << 16);
	SetIcon(nIconId, nIconIdx);
	SetExpandedIcon(nExpandedIconId, nExpandedIconIdx);
	m_dwFlags = dwFlags;
}

//*****************************************************************************
CWsfTreeItem::~CWsfTreeItem()
{
	CWsfTreeItem* p, *p2;

	RemoveFromTree();

	// Delete the subtree

  p = m_pChild;
	while (p)
	{
		ASSERT_VALID(p);
		p2 = p->m_pNextSibling;
		delete p;
		p = p2;
	}
}

//*****************************************************************************
int CWsfTreeItem::GetIcon() const
{
	return WsfGetApp()->GetIconIdx(m_dwIcon & 0xFFFF, m_dwIcon >> 16);
}

//*****************************************************************************
int CWsfTreeItem::GetExpandedIcon() const
{
	return WsfGetApp()->GetIconIdx(m_dwExpandedIcon & 0xFFFF, m_dwExpandedIcon >> 16);
}

/*****************************************************************************/
void CWsfTreeItem::SetIcon(int nBmpId, int nIconIdx)
{
	m_dwIcon = nBmpId + (nIconIdx << 16);
}

/*****************************************************************************/
void CWsfTreeItem::SetExpandedIcon(int nBmpId, int nIconIdx)
{
	m_dwExpandedIcon = nBmpId + (nIconIdx << 16);
}

/*****************************************************************************/
void CWsfTreeItem::SetContextMenu(int nMenuId, int nMenuIdx)
{
	m_dwMenu = nMenuId + (nMenuIdx << 16);
}

/*****************************************************************************/
CString CWsfTreeItem::GetPathTitle(int nStopLevel /*=0*/) const
{
	CString s = GetTitle();

	if (nStopLevel==0)
	{
		for (CWsfTreeItem* p=GetParent(); p; p=p->GetParent())
			s = p->GetTitle() + " > " + s;
	}
	else
	{
		int n = 0;
      CWsfTreeItem* p;
      for (p=GetParent(); p; p=p->GetParent()) { n++; }
      for (p=GetParent(); n>nStopLevel; p=p->GetParent(), n--) { s = p->GetTitle() + " > " + s; }
	}
	return s;
}


//*****************************************************************************
void CWsfTreeItem::RemoveFromTree()
{
	if (m_pPrevSibling)
	{
		ASSERT_VALID(m_pPrevSibling);
		m_pPrevSibling->m_pNextSibling = m_pNextSibling;
	}
	else if (m_pParent)
	{
		ASSERT_VALID(m_pParent);
		m_pParent->m_pChild = m_pNextSibling;
		if (!m_pParent->m_pChild) m_pParent->m_dwFlags &= ~EXPANDED;
	}

	if (m_pNextSibling) 
	{
		ASSERT_VALID(m_pNextSibling);
		m_pNextSibling->m_pPrevSibling = m_pPrevSibling;
	}

	m_pPrevSibling = NULL;
	m_pNextSibling = NULL;
	m_pParent = NULL;
	m_hTreeItem = NULL;
}

//*****************************************************************************
void CWsfTreeItem::AddChild(CWsfTreeItem* pChild)
{
	ASSERT_VALID(pChild);
	ASSERT(!pChild->m_pPrevSibling && !pChild->m_pNextSibling && !pChild->m_pParent);
	pChild->m_pParent = this;

	// Trivial insertion

	if (!m_pChild)
	{
		m_pChild = pChild;
		return;
	}

	// Find position

	CWsfTreeItem* pPrev, *p = m_pChild;
	while (p)
	{
		if (OnCompare(pChild, p) <= 0) break;
		pPrev = p;
		p = p->m_pNextSibling;
	}

	// Insert

	if (p == m_pChild)		// Insert at the head
	{
		pChild->m_pNextSibling = m_pChild;
		m_pChild->m_pPrevSibling = pChild;
		m_pChild = pChild;
	}
	else if (p == NULL)		// Insert at the tail
	{
		pPrev->m_pNextSibling = pChild;
		pChild->m_pPrevSibling = pPrev;
	}
	else		// Insert in the middle
	{
		pChild->m_pNextSibling = p;
		pChild->m_pPrevSibling = p->m_pPrevSibling;
		p->m_pPrevSibling = pChild;
		pPrev->m_pNextSibling = pChild;
	}
}

//*****************************************************************************
void CWsfTreeItem::Reorder()
{
	CWsfTreeItem* p = GetParent();
	if (!p) return;
	
	// If the item is the only child of the parent
	// the expanded flag we will be reset so rememeber it
		
	BOOL bExpanded = p->GetFlags() & CWsfTreeItem::EXPANDED;
	RemoveFromTree();
	p->AddChild(this);
	if (bExpanded) p->AddFlags(bExpanded);
}

/*****************************************************************************/
CWsfTreeItem* CWsfTreeItem::FindItem(const char* pcszTitle)
{
	if (GetTitle() == pcszTitle) return this;
	
	for (CWsfTreeItem* p=m_pChild; p; p=p->m_pNextSibling)
	{
		ASSERT_VALID(p);
		CWsfTreeItem* x = p->FindItem(pcszTitle);
		if (x) return x;
	}
	
	return NULL;
}

/*****************************************************************************/
CWsfTreeItem* CWsfTreeItem::FindItem(int nId)
{
	if (GetId() == nId) return this;
	
	for (CWsfTreeItem* p=m_pChild; p; p=p->m_pNextSibling)
	{
		ASSERT_VALID(p);
		CWsfTreeItem* x = p->FindItem(nId);
		if (x) return x;
	}
	
	return NULL;
}

//*****************************************************************************
CWsfTreeItem* CWsfTreeItem::FindChildItem(const char *pcszTitle, BOOL bWholeSubtree)
{
	CWsfTreeItem* x, *p = m_pChild;
	
	while (p)
	{
		ASSERT_VALID(p);
		if (p->GetTitle() == pcszTitle) return p;
		if (bWholeSubtree && p->GetChild()) { if (x = p->FindChildItem(pcszTitle, true)) return x; }
		p = p->m_pNextSibling;
	}
	
	return NULL;
}

//*****************************************************************************
CWsfTreeItem* CWsfTreeItem::FindChildItem(int nId, BOOL bWholeSubtree)
{
	CWsfTreeItem* x, *p = m_pChild;
	
	while (p)
	{
		ASSERT_VALID(p);
		if (nId == p->GetId()) return p;
		if (bWholeSubtree && p->GetChild()) { if (x = p->FindChildItem(nId, true)) return x; }
		p = p->m_pNextSibling;
	}
	
	return NULL;
}

//*****************************************************************************
CWsfTreeItem* CWsfTreeItem::FindChildItem(const CRuntimeClass *pClass, BOOL bWholeSubtree)
{
	CWsfTreeItem* x, *p = m_pChild;
	
	while (p)
	{
		ASSERT_VALID(p);
		if (p->IsKindOf(pClass)) return p;
		if (bWholeSubtree && p->GetChild()) { if (x = p->FindChildItem(pClass, true)) return x; }
		p = p->m_pNextSibling;
	}
	
	return NULL;
}

/*****************************************************************************/
void CWsfTreeItem::EnumChildItems(CWslArray<CWsfTreeItem*>& aItems, BOOL bWholeSubtree)
{
	for (CWsfTreeItem* p=m_pChild; p; p=p->m_pNextSibling)
	{
		ASSERT_VALID(p);
		aItems += p;
		if (bWholeSubtree && p->GetChild()) p->EnumChildItems(aItems, true);
	}
}

//*****************************************************************************
BOOL CWsfTreeItem::IsInSubTree(CWsfTreeItem* pItem)
{
	if (pItem == this) return true;

	CWsfTreeItem* p = GetChild();
	while (p)
	{
		ASSERT_VALID(p);
		if (p->IsInSubTree(pItem)) return true;
		p = p->m_pNextSibling;
	}
	
	return false;
}

//*****************************************************************************
CWsfTreeItem* CWsfTreeItem::GetRoot()
{
	ASSERT_VALID(this);
	CWsfTreeItem* p = this;
	while (p->m_pParent) p = p->m_pParent;
	return p;
}

//*****************************************************************************
void CWsfTreeItem::Load(CArchive &ar)
{
	int n;
	CWsfTreeItem* p, *pPrev;

	// Load item

	ar >> m_dwFlags;
	int nVersion = (m_dwFlags>>23) & 15;

	if (nVersion == 0)
	{
		// Ignoring UI stuff
		// Used e.g. in old Race Manager files
		DWORD dw;
		ar >> dw;
	}
	else 
	{
		ASSERT(nVersion == CURRENT_SERIALIZATION_VERSION);
		if (m_dwFlags & SERIALIZE_UI)
		{
			ar >> m_dwMenu;
			ar >> m_dwIcon;
			ar >> m_dwExpandedIcon;
		}
	}

	// Load subtree

	ar >> n;

	if (n == 1)
	{
		n = 0;
		while (n == 0)
		{
			ar >> p;
			ar >> n;
			ASSERT(n<5);		// Bad format
			
			if (!m_pChild)
				m_pChild = p;
			else
			{
				pPrev->m_pNextSibling = p;
				p->m_pPrevSibling = pPrev;
			}

			p->m_pParent = this;

			pPrev = p;
		}
	}
}

//*****************************************************************************
void CWsfTreeItem::Save(CArchive &ar)
{
	// Save item

	DWORD dw = m_dwFlags | (CURRENT_SERIALIZATION_VERSION<<23);
	ar << dw;

	if (m_dwFlags & SERIALIZE_UI)
	{
		ar << m_dwMenu;
		ar << m_dwIcon;
		ar << m_dwExpandedIcon;
	}

	if (m_dwFlags & DONT_SAVE_CHILDERN) 
	{
		ar << (int) 0;
		return;
	}

	// Save sub tree

	CWsfTreeItem* p = m_pChild;
	for (; p && (p->m_dwFlags & DONT_SAVE); p=p->m_pNextSibling) ; 

	if (p)
	{
		ar << (int) 1;
		while (p)
		{
			ar << p;
			for (p=p->m_pNextSibling; p && (p->m_dwFlags & DONT_SAVE); p=p->m_pNextSibling) ; 
			ar << (int) (p ? 0 : 1);
		}
	}
	else ar << (int) 0;

}

//*****************************************************************************
void CWsfTreeItem::Serialize(CArchive &ar)
{
	CCmdTarget::Serialize(ar);

	if (ar.IsStoring())
	  Save(ar);
	else
		Load(ar);
}

//*****************************************************************************
CWsfTreeItem* CWsfTreeItem::Load(const char *pcszFileName)
{
	CWsfTreeItem* pItem;
	CFile f(pcszFileName, CFile::modeRead);
	CArchive ar(&f, CArchive::load);
	ar >> pItem;
	ar.Close();

	ASSERT_VALID(pItem);
	return pItem;
}

//*****************************************************************************
void CWsfTreeItem::Save(const char *pcszFileName)
{
	ASSERT_VALID(this);
	CFile f(pcszFileName, CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&f, CArchive::store);
	ar << this;
	ar.Close();
}

//*****************************************************************************
void CWsfTreeItem::DoForWholeTree(WSF_TI_CALLBACK *pfnCallback, void *pData)
{
	CWsfTreeItem* p, *p2;
	p = m_pChild;
	
	while (p)
	{
		ASSERT_VALID(p);
		p2 = p->m_pNextSibling;		// Get next item now, because callback can delete "p"
		p->DoForWholeTree(pfnCallback, pData);
		p = p2;
	}

	pfnCallback(this, pData);
}

//*****************************************************************************
void CWsfTreeItem::DoForWholeSubTree(WSF_TI_CALLBACK *pfnCallback, void *pData)
{
	CWsfTreeItem* p, *p2;
	p = m_pChild;
	
	while (p)
	{
		ASSERT_VALID(p);
		p2 = p->m_pNextSibling;		// Get next item now, because callback can delete "p"
		p->DoForWholeTree(pfnCallback, pData);
		p = p2;
	}
}

//*****************************************************************************
BOOL CWsfTreeItem::PSCallback(void *pData, CWsfPropertySheet& ps, BOOL bOk)
{
	ASSERT(pData);
	return ((CWsfTreeItem*)pData)->OnCloseProperties(ps, bOk);
}

//*****************************************************************************
int CWsfTreeItem::DisplayProperties(BOOL bWizard /*= false*/)
{
	CWsfPropertySheet ps(bWizard, PSCallback, this);

	// Call user to init the ps

	if (!OnInitProperties(ps))
	{
		ps.RemoveAllPages();	
		return IDCANCEL; 
	}
	ASSERT(ps.GetPageCount() > 0);				// there must be at least one page

	// Display ps

	int n = ps.DoModal();

	// Clean up

	ps.RemoveAllPages();	

	return n;
}

//*****************************************************************************
void CWsfTreeItem::OnWsfProperties()
{
	if (DisplayProperties() == IDOK)
	{
		WsfGetApp()->TreeHasChanged(this, CWsfApp::TITLE_ICON);
	}
}

//*****************************************************************************
void CWsfTreeItem::OnWsfHelp()
{
/*	CString strFile, strChm;

	strChm = WsfGetApp()->m_strAppPath + WSF_HELP_FILE_NAME;
	strFile = GetRuntimeClass()->m_lpszClassName;
	strFile += ".htm";
	
	TRACE1("Help for tree item: %s\n", strFile);
	HtmlHelp(*WsfGetFrame(), strChm, HH_DISPLAY_TOPIC, (DWORD)(const char*) strFile);*/
}

//*****************************************************************************
// Standard commands are disabled.

void CWsfTreeItem::OnUpdateWsfOpen(CCmdUI* pCmdUI) { pCmdUI->Enable(m_dwFlags & CWsfTreeItem::ENABLE_OPEN);	}
void CWsfTreeItem::OnUpdateWsfRename(CCmdUI* pCmdUI) { pCmdUI->Enable(false);	}
void CWsfTreeItem::OnUpdateWsfDelete(CCmdUI* pCmdUI) { pCmdUI->Enable(m_dwFlags & CWsfTreeItem::ENABLE_DELETE); }
void CWsfTreeItem::OnUpdateWsfHelp(CCmdUI* pCmdUI) { pCmdUI->Enable(false);	}
void CWsfTreeItem::OnUpdateWsfProperties(CCmdUI* pCmdUI) { pCmdUI->Enable(false);	}


/*****************************************************************************
*
* SELECTION CONTEXT
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWsfTreeItem::IsMultiselection() const
{
	ASSERT(m_pSelContext);
	return m_pSelContext->dwCHFlags & CWsfTISelContext::MULTI;
}

/*****************************************************************************/
int CWsfTreeItem::GetSelCount() const
{
	ASSERT(m_pSelContext);
	return m_pSelContext->nSelCount;
}

/*****************************************************************************/
int CWsfTreeItem::GetSelPos() const 
{
	ASSERT(m_pSelContext);
	return m_pSelContext->nSelPos;
}

/*****************************************************************************/
BOOL CWsfTreeItem::IsFirstSelected() const
{
	ASSERT(m_pSelContext);
	return m_pSelContext->dwCHFlags & CWsfTISelContext::FIRST;
}

/*****************************************************************************/
BOOL CWsfTreeItem::IsLastSelected() const
{
	ASSERT(m_pSelContext);
	return m_pSelContext->dwCHFlags & CWsfTISelContext::LAST;
}

/*****************************************************************************/
void CWsfTreeItem::CancelCommand()
{
	ASSERT(m_pSelContext);
	m_pSelContext->dwCHFlags |= CWsfTISelContext::CANCEL;
}

