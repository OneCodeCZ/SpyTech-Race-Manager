///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|	 
//
// Part of CWsfApp implementation.
// Commands, accelerators management.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfToolBar.h"
#include "WsfCmd.h"
#include "WsfIconList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*****************************************************************************
BOOL CWsfApp::SetViewAcceleratorTable(int nID)
{
	if (m_hViewAcceleratorTable) 
	{
		VERIFY(DestroyAcceleratorTable(m_hViewAcceleratorTable));
		m_hViewAcceleratorTable = NULL;
	}

	if (!nID) return true;
	
	HINSTANCE h = AfxFindResourceHandle((const char*) nID, RT_ACCELERATOR);
	ASSERT(h);
	m_hViewAcceleratorTable = LoadAccelerators(h, (const char*) nID);

	return m_hViewAcceleratorTable != NULL;
}

//*****************************************************************************
const CWsfCmd* CWsfApp::GetCmd(int nId)
{
	for (int i = m_aCmds.GetSize(); i--;)
		if (m_aCmds[i]->m_nId == nId) return m_aCmds[i];
	return NULL;
}

/*****************************************************************************/
const CWsfCmd* CWsfApp::GetCmd(const char* pcszTextId)
{
	for (int i = m_aCmds.GetSize(); i--;)
		if (!strcmp(m_aCmds[i]->m_sTextId, pcszTextId)) return m_aCmds[i];
	return NULL;
}

//*****************************************************************************
void CWsfApp::AddCmdSimple(const char* pcszTextId, int nId, 
													 int nImgIdx/*=-1*/, 
													 int nBmpId/*=-1*/,
													 BOOL bText, 
													 BOOL bImage)
{
	ASSERT(GetCmd(nId) == NULL);  // check that command hasn't been already added

	if (nImgIdx != -1) nImgIdx = InitCmdImage(nImgIdx, nBmpId);

	CWsfCmd* p = new CWsfCmd(nId, pcszTextId, bText, bImage);
	m_aCmds += p;
	p->InitSimple(nImgIdx);
}

//*****************************************************************************
void CWsfApp::AddCmdComboBox(const char* pcszTextId, int nId, 
														 CRuntimeClass* pComboClass,
														 int nWidth,
														 BOOL bText)
{
	ASSERT(GetCmd(nId) == NULL);  // check that command hasn't been already added
	ASSERT(pComboClass);

	CWsfCmd* p = new CWsfCmd(nId, pcszTextId, bText, false);
	m_aCmds += p;

	p->InitComboBox(pComboClass, nWidth);
}

//*****************************************************************************
void CWsfApp::AddCmdDropDown(const char* pcszTextId, int nId, 
														 int nDefCmdId,
														 HMENU hMenu,
														 BOOL bText)
{
	ASSERT(GetCmd(nId) == NULL);  // check that command hasn't been already added
	ASSERT(hMenu);

	CWsfCmd* p = new CWsfCmd(nId, pcszTextId, bText, true);
	m_aCmds += p;

   ProcessMenu(hMenu);
   p->InitDropDown(nDefCmdId, hMenu);
}

//*****************************************************************************
void CWsfApp::AddCmdDropDownOnly(const char* pcszTextId, 
                                 int nId, 
                                 int nImgIdx, int nBmpId, 
                                 HMENU hMenu, 
                                 BOOL bText /*= false*/)
{
	ASSERT(GetCmd(nId) == NULL);  // check that command hasn't been already added
   ASSERT(hMenu);

	if (nImgIdx != -1) nImgIdx = InitCmdImage(nImgIdx, nBmpId);

	CWsfCmd* p = new CWsfCmd(nId, pcszTextId, bText, true);
	m_aCmds += p;

   ProcessMenu(hMenu);
	p->InitDropDownOnly(nImgIdx, hMenu);
}

//*****************************************************************************
void CWsfApp::AddCmdDropDown(const char* pcszTextId, int nId, 
														 CRuntimeClass* pWndClass, 
														 int nImgIdx/*=-1*/, 
														 int nBmpId/*=-1*/,
														 BOOL bText, 
														 BOOL bImage)
{
	ASSERT(GetCmd(nId) == NULL);  // check that command hasn't been already added
	ASSERT(pWndClass);

	if (nImgIdx != -1) nImgIdx = InitCmdImage(nImgIdx, nBmpId);

	CWsfCmd* p = new CWsfCmd(nId, pcszTextId, bText, bImage);
	m_aCmds += p;
	p->InitDropDown(nImgIdx, pWndClass);
}

/*****************************************************************************/
void CWsfApp::CloneCmd(int nId, const char* pcszNewTextId, int nNewId)
{
   if (GetCmd(nNewId)) return;

   const CWsfCmd* p = GetCmd(nId);
   if (!p) { ASSERT(false); return; }

   CWsfCmd* pNew = new CWsfCmd(p, nNewId, pcszNewTextId);
	m_aCmds += pNew;
}

/*****************************************************************************/
void CWsfApp::DeleteCmd(int nId)
{
	for (int i = m_aCmds.GetSize(); i--;)
   {
		if (m_aCmds[i]->m_nId == nId) 
      {
         delete m_aCmds[i];
         m_aCmds.RemoveAt(i);
         return;
      }
   }
}

//*****************************************************************************
int CWsfApp::InitCmdImage(int nImgIdx, int nBmpId)
{
   int n = m_pSmallIconList->GetIconIdx(nBmpId, nImgIdx);
   //ASSERT(n != -1);
   return n;

   /*ASSERT(nImgIdx >= 0 && nBmpId > 0);

	// Check if the bitmap already exists

	int i = m_pCmdIconList->GetIconIdx(nBmpId, nImgIdx);
	if (i != -1) return i;

	// If not, add it

	return m_pCmdIconList->AddIcons(nBmpId, false) + nImgIdx;*/
}

/*****************************************************************************/
void CWsfApp::PostCommand(int nCmdId)
{
	ASSERT_VALID(m_pMainWnd);
	ASSERT(nCmdId > 0 && nCmdId < 65536);
	VERIFY(m_pMainWnd->PostMessage(WM_COMMAND, nCmdId, 0));
}

/*****************************************************************************/
BOOL CWsfApp::SendCommand(CWsfViewInfo* pvi, int nCmdId)
{
	ASSERT(nCmdId > 0 && nCmdId < 65536);
	ASSERT(pvi);
	ASSERT_VALID(pvi->pView);
	return pvi->pView->OnCmdMsg(nCmdId, CN_COMMAND, NULL, NULL);
}

/*****************************************************************************
*
* MENU
*
*****************************************************************************/

/*****************************************************************************/
void CWsfApp::AppendPopupMenu(HMENU hMenu, const CWsfCmd* pCmd)
{
	ASSERT(hMenu);
	ASSERT(pCmd);
	CString s;
	MENUITEMINFO mmi;
	mmi.cbSize = sizeof(mmi);

	ASSERT(pCmd->m_nType==CWsfCmd::SIMPLE || 
		pCmd->m_nType==CWsfCmd::DROP_DOWN_MENU_ONLY ||
		pCmd->m_nType==CWsfCmd::DROP_DOWN_MENU);

	mmi.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_DATA;
	if (pCmd->m_nImgIdx != -1)
	{
		mmi.hbmpItem = HBMMENU_CALLBACK;
		mmi.fMask |= MIIM_BITMAP;
	}
	mmi.fType = MFT_STRING;
	mmi.wID = pCmd->m_nId;
	s = pCmd->GetTextAndAccel();
	mmi.dwTypeData = (char*) (const char*) s;
	mmi.dwItemData = (DWORD) pCmd;

	if (pCmd->m_nType==CWsfCmd::DROP_DOWN_MENU_ONLY || pCmd->m_nType==CWsfCmd::DROP_DOWN_MENU)
	{
		mmi.fMask |= MIIM_SUBMENU;
		mmi.hSubMenu = pCmd->m_hMenu;
	}

	VERIFY(InsertMenuItem(hMenu, GetMenuItemCount(hMenu), true, &mmi));
}

/*****************************************************************************/
void CWsfApp::AppendPopupMenu(HMENU hMenu, int nCmdId)
{
	if (nCmdId == -1)
	{
		int n = GetMenuItemCount(hMenu);
		if (n==0) return;

		MENUITEMINFO mmi;
		mmi.cbSize = sizeof(mmi);
		mmi.fMask = MIIM_FTYPE;
		mmi.fType = MFT_SEPARATOR;
		VERIFY(InsertMenuItem(hMenu, n, true, &mmi));
	}
	else
	{
		AppendPopupMenu(hMenu, GetCmd(nCmdId));
	}
}


/*****************************************************************************
*
* MAIN MENU FROM XML
*
*****************************************************************************/

/*****************************************************************************/
void CWsfApp::ParseMenu(CWslXMLElement* pMenuElement, HMENU hMenu)
{
	CString s, sLang;
	MENUITEMINFO mmi;
	mmi.cbSize = sizeof(mmi);

	for (int i = pMenuElement->GetFirstElementPos(); i!=-1; i=pMenuElement->GetNextElementPos(i))
	{
		CWslXMLElement* e = pMenuElement->GetElement(i);

		if (e->GetName() == "item")
		{
			const CWsfCmd* pCmd = GetCmd(e->GetAttributeString("id"));
			if (pCmd) 
				AppendPopupMenu(hMenu, pCmd); 
			else
				TRACE1("CWsfApp::ParseMenu(): Command \"%s\" nor found!\n", e->GetAttributeString("id"));
		}
		else if (e->GetName() == "popup")
		{
			mmi.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_FTYPE;
			mmi.fType = MFT_STRING;
			s = e->GetAttributeString(m_eLanguage==CZ ? "cz":"en");
			mmi.dwTypeData = (char*) (const char*) s;
			mmi.hSubMenu = CreatePopupMenu();

			ParseMenu(e, mmi.hSubMenu);
			VERIFY(InsertMenuItem(hMenu, GetMenuItemCount(hMenu), true, &mmi));
		}
		else if (e->GetName() == "separator")
		{
			AppendPopupMenu(hMenu, -1); 
		}
	}
}

/*****************************************************************************/
HMENU CWsfApp::LoadMenu(const char* pcszXMLFile) throw (CException*)
{
	CWslXMLParser p;
	CWslPtr<CWslXMLElement> pRoot;
	CWslXMLElement* e;

	pRoot = p.ParseFile(pcszXMLFile, 1250);
	e = pRoot->GetElement("menu");
	if (!e) return NULL;

	HMENU hMenu = CreateMenu();
	ParseMenu(e, hMenu);
	return hMenu;
}

/*****************************************************************************
*
* PROCESS MENU 
*
*****************************************************************************/

/*****************************************************************************/
void CWsfApp::ProcessMenu(HMENU hMenu)
{
   ASSERT(hMenu);
   
   MENUITEMINFO mii;
   mii.cbSize = sizeof(mii);
   int n = GetMenuItemCount(hMenu);
   CString s;
   ASSERT(n != -1);

   for (int i=0; i<n; i++)
   {
      // Get item

      mii.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE | MIIM_STRING;
      mii.cch = MAX_PATH;
      mii.dwTypeData = temp;

      GetMenuItemInfo(hMenu, i, true, &mii);
      
      if (mii.fType == MFT_SEPARATOR) continue;
      if (mii.hSubMenu) { ProcessMenu(mii.hSubMenu); continue; }

      // Get item's command

      const CWsfCmd* pCmd = GetCmd(mii.wID);
      if (!pCmd) continue;

	   mii.fMask = MIIM_DATA;
	   mii.dwItemData = (DWORD) pCmd;
	
      // Add icon

      if (pCmd->m_nImgIdx != -1)
	   {
		   mii.hbmpItem = HBMMENU_CALLBACK;
		   mii.fMask |= MIIM_BITMAP;
	   }

      // Add text

      if (!temp[0] || pCmd->m_sTextId == temp)
      {
         mii.fMask |= MIIM_STRING;
      	s = pCmd->GetTextAndAccel();
	      mii.dwTypeData = (char*) (const char*) s;
      }

      // Update item

      SetMenuItemInfo(hMenu, i, true, &mii);
   }
}

/*****************************************************************************
*
* CONTEXT MENU FROM RESOURCES
*
*****************************************************************************/

//*****************************************************************************
HMENU CWsfApp::BuildPopupMenu(CWslArray<int>& aMenuIDs, int nDefCmd)
{
	HMENU hMenu, hPopupMenu, hNewMenu;
	MENUITEMINFO mi, mi2;
	CWslArray<HMENU> aMenus, aLevelUpMenus;
	int nPos, nCount, t;
	char temp[128];

	// Init

	if (!aMenuIDs.GetSize()) return NULL;

	// Get all different pop-up menus

	for (int i=0; i<aMenuIDs.GetSize(); i++)
	{
		if (aMenuIDs.Find(aMenuIDs[i]) < i) continue;

		t = LOWORD(aMenuIDs[i]);
		hMenu = ::LoadMenu(AfxFindResourceHandle(MAKEINTRESOURCE(t), RT_MENU),
			MAKEINTRESOURCE(t));
		ASSERT(hMenu);

		hPopupMenu = GetSubMenu(hMenu, HIWORD(aMenuIDs[i]));
		ASSERT(hPopupMenu);

		aMenus += hPopupMenu;
		aLevelUpMenus += hMenu;
	}

	// ---- Build new pop-up menu -----------------------------------------------
	// The new menu is initially the same as the first menu in the array.
	// Then we must test if items of this menu are also in all other menus.
	
	// Init

	hNewMenu = CreatePopupMenu();
	
	BOOL bSeparatorPending = false;
	nPos = 0;
	nCount = GetMenuItemCount(aMenus[0]);
	mi.cbSize = mi2.cbSize = sizeof(mi);
	mi2.fMask = 0;
	
	// Build menu

	for (t = 0; t < nCount; t++)
	{
		mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE | MIIM_STRING;
		mi.dwTypeData = temp;
		mi.cch = 128;
		VERIFY(GetMenuItemInfo(aMenus[0], t, true, &mi));

		// NOTE: Doesn't support nested popup menus!
		ASSERT(!mi.hSubMenu);

		// Search all menus if the item is there. If so, add it to the menu.
		// Separators are not inserted immediatelly but they are delayed
		// until a menu item is about to be inserted.
		// This prevents separators to appear at the end of the menu.

		if (!(mi.fType & MFT_SEPARATOR))
		{
         int i;
			for (i=aMenus.GetSize(); i-- > 1;)
				if (!GetMenuItemInfo(aMenus[i], mi.wID, false, &mi2)) break;
			if (i > 0) continue;

			if (bSeparatorPending)
			{
				mi2.fMask = MIIM_TYPE;
				mi2.fType = MFT_SEPARATOR;
				InsertMenuItem(hNewMenu, nPos++, true, &mi2);
				bSeparatorPending = false;
			}

			VERIFY(InsertMenuItem(hNewMenu, nPos++, true, &mi));
		}
		else bSeparatorPending = true;
	}

	// Set default item

	SetMenuDefaultItem(hNewMenu, nDefCmd, false);

	// End

	for (i=aLevelUpMenus.GetSize(); i--;) DestroyMenu(aLevelUpMenus[i]);

   ProcessMenu(hNewMenu);
	return hNewMenu;
}





