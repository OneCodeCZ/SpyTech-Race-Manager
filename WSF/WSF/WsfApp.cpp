///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfToolBar.h"
#include "WsfCmd.h"
#include "WsfIconList.h"
#include "WsfAppPlugin.h"
#include "WsfTreeItemDoc.h"
#include "locale.h"
#include "WsfHtmlView.h"
#include "WsfHtmlDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfApp, CWinApp)
	//{{AFX_MSG_MAP(CWsfApp)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex) 
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp) 
	ON_COMMAND(ID_HELP, OnHelp) 
	ON_COMMAND(ID_WSF_SAVE_ALL, OnWsfSaveAll) 
	ON_UPDATE_COMMAND_UI(ID_WSF_SAVE_ALL, OnUpdateWsfSaveAll) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfApp::CWsfApp()
{
	char temp[MAX_PATH+1];

	// Init stuff

	m_nIdCounter = 1;
	m_pWorkspaceDoc = NULL;
	m_nViewCounter = 0;
	VERIFY(m_hNoCursor = ::LoadCursor(NULL, IDC_NO));
	m_hViewAcceleratorTable = NULL;
	m_cfTreeItemList = RegisterClipboardFormat("SpyTech.WsfTreeItemList");
	srand(GetTickCount());
	
	m_pSmallIconList = new CWsfIconList();
	VERIFY(m_pSmallIconList->Create(16,16, ILC_MASK|ILC_COLOR32,0,512));

	m_pSmallIconListGray = new CWsfIconList();
	VERIFY(m_pSmallIconListGray->Create(16,16, ILC_MASK|ILC_COLOR32,0,512));

	m_pNormalIconList = new CWsfIconList();
	VERIFY(m_pNormalIconList->Create(32,32, ILC_MASK|ILC_COLOR32,0,512));

	m_pColumnIconList = new CWsfIconList();
	VERIFY(m_pColumnIconList->Create(8,10, ILC_MASK|ILC_COLOR32,0,512));

	m_pCmdIconList = new CWsfIconList();
	VERIFY(m_pCmdIconList->Create(16,16, ILC_MASK|ILC_COLOR32,0,512));

	// Get path of the EXE file

	HMODULE h = GetModuleHandle(NULL);
	VERIFY(GetModuleFileName(h, temp, 1024));
	char* p = PathFindFileName(temp);
	*p = 0;

	m_sAppPath = temp;
	ASSERT(!m_sAppPath.IsEmpty());
	TRACE1("EXE path: %s\n", m_sAppPath);

	// Get WSF version

	VERIFY(GetModuleFileName(g_hInstance, temp, 1024));
	m_dwWsfVersion = WsfGetDllVersion(temp);
	ASSERT(m_dwWsfVersion);
	TRACE1("WSF version: %s\n", WsfFormatDllVersion(m_dwWsfVersion));

	// Get language of the resources
	// TODO: Dirty -> make it better!

	VERIFY(::LoadString(g_hInstance, IDS_CANCEL, temp, MAX_PATH));
	m_eLanguage = !stricmp(temp, "Cancel") ? EN:CZ;

	// Set C library locale to the Windows User's Locale

	VERIFY(setlocale(LC_ALL,""));
	ASSERT(strcoll("È", "S")<0);     // Czech Windows assumed

   // Windows version

   OSVERSIONINFO osvi;
   osvi.dwOSVersionInfoSize = sizeof(osvi);

   VERIFY(GetVersionEx(&osvi));

   m_eWinVersion = WINOTHER;

   if (osvi.dwMajorVersion==4)
   {
      m_eWinVersion = WIN9x;
   }
   else if (osvi.dwMajorVersion==5)
   {
      switch (osvi.dwMinorVersion)
      {
      case 0: m_eWinVersion = WIN2000; break;
      default:
      case 1: m_eWinVersion = WINXP; break;
      }
   }
   else if (osvi.dwMajorVersion > 5)
   {
      m_eWinVersion = WINXP;
   }

   // CFileDialog BUG prevention
   
   ASSERT(sizeof(OPENFILENAME) == 76);
}

//*****************************************************************************
CWsfApp::~CWsfApp()
{
	if (m_hViewAcceleratorTable) VERIFY(DestroyAcceleratorTable(m_hViewAcceleratorTable));
	DestroyCursor(m_hNoCursor);
	delete m_pSmallIconList;
	delete m_pSmallIconListGray;
	delete m_pNormalIconList;
	delete m_pColumnIconList;
	delete m_pCmdIconList;
}

//*****************************************************************************
BOOL CWsfApp::InitInstance() 
{
   ASSERT(m_pszAppName);     // You did not call SetAppNameAndVersion()!

	// Call plug-ins

	for (int i=0; i < g_aAppPlugins.GetSize(); i++) 
   {
      g_aAppPlugins[i]->OnInitInstance();
   }

   // Init main window

   if (m_pMainWnd)
   {
	   // Position window

	   m_pMainWnd->SetWindowPos(NULL, m_Setup.nX, m_Setup.nY, m_Setup.nW, m_Setup.nH, SWP_NOZORDER);

	   // Load control bars info
	   // All toolbars should have been added until now.

	   GetFrame()->LoadBarState(WSF_CONTROL_BARS_REG_KEY);

	   // Set main window's title to the application's name

      m_pMainWnd->SetWindowText(m_pszAppName);
      CWsfFrame* pFrame = dynamic_cast<CWsfFrame*>(m_pMainWnd);
      if (pFrame) pFrame->SetTitle(m_pszAppName);

	   // Show window

      m_pMainWnd->ShowWindow(m_Setup.nShowType);
	   m_pMainWnd->UpdateWindow();
   }

	return CWinApp::InitInstance();
}

//*****************************************************************************
int CWsfApp::ExitInstance() 
{
	// Call plug-ins

	for (int i=0; i < g_aAppPlugins.GetSize(); i++) 
   {
      g_aAppPlugins[i]->OnExitInstance();
   }

	// Save setup

   try
   {
      CWslPtr<CWslRegKey> k = GetAppRegistryKey("WSF");
      k->Set("version", m_dwWsfVersion);
      k->Set("win_x", m_Setup.nX);
      k->Set("win_y", m_Setup.nY);
      k->Set("win_w", m_Setup.nW);
      k->Set("win_h", m_Setup.nH);
      k->Set("win_show_type", m_Setup.nShowType);
      k->Set("splitter_x", m_Setup.nSplitter);
      k->Set("left_pane_visible", m_Setup.bLeftPaneVisible);
      k->Set("list_view_type", m_Setup.nListViewType);
   }
   catch (CException* e)
   {
      ASSERT(false);
      e->Delete();
   }

	// Remove control bars
	// NOTE: Their position is stored in the CWsfFrame::OnClose() method.

	for (i=0; i < m_aControlBars.GetSize(); i++)
	{
		ASSERT_VALID(m_aControlBars[i]);
		delete m_aControlBars[i];
	}

	// Delete all commands

	for (i=0; i < m_aCmds.GetSize(); i++) delete m_aCmds[i];
	
   // Remove the temp folder
   
   if (!m_sTempPath.IsEmpty())
   {
      try
      {
         CWslFS::RemoveDirectory(m_sTempPath);
      }
      catch (CException* e)
      {
         ASSERT(false);
         e->Delete();
      }
   }

   // Finall exit

	return CWinApp::ExitInstance();
}

//*****************************************************************************
BOOL CWsfApp::PreTranslateMessage(MSG* pMsg) 
{
	// View's accelerator table
   // Commands are sent to the main frame and not the the target window
   // itself, because the window can be a non-MFC embedded in a MFC window.
   // For example HTML view has embedded the IE editor. In such a case
   // the commands are handled in the MFC wrapping window and not in the
   // target window and the target non-MFC window is unable to pass it up.

	if (m_hViewAcceleratorTable)
	{
      ASSERT_VALID(m_pMainWnd);
		if (::TranslateAccelerator(*m_pMainWnd, m_hViewAcceleratorTable, pMsg))
			return false;
	}
	
	// CTRL + TAB and ChooseToolBars

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_TAB)
		{
			if (GetKeyState(VK_CONTROL) >> 15)
			{
				m_bTabPressed = true;
				GetFrame()->m_pRightPane->ActivateNextWnd(++m_nViewCounter);
				return true;				// Fully processed
			}
		}
		break;

	case WM_KEYUP:
		if (pMsg->wParam == VK_CONTROL)
		{
			if (m_bTabPressed)
			{
				GetFrame()->m_pRightPane->MoveActiveWndToSwitchOrderTop();
				m_nViewCounter = 0;
				m_bTabPressed = false;
			}
		}
		break;

	case WM_RBUTTONDOWN:
		{
			int nID = GetWindowLong(pMsg->hwnd, GWL_ID);
			if (nID == AFX_IDW_DOCKBAR_TOP || nID == AFX_IDW_DOCKBAR_BOTTOM ||
				nID == AFX_IDW_DOCKBAR_RIGHT || nID == AFX_IDW_DOCKBAR_LEFT)
			{
				GetFrame()->ChooseToolBars();
				return true;
			}
		}
	}

	// CWinApp continues...

	return CWinApp::PreTranslateMessage(pMsg);
}

/*****************************************************************************/
BOOL CWsfApp::OnIdle(LONG lCount) 
{
	if (CWinApp::OnIdle(lCount)) return true;

   // Perform delayed CloseDoc() calls
   // NOTE: The closing document MUST NOT display any UI!
   //       It must close immediatelly.

   for (int i=0; i<m_CloseDocQueue.GetSize(); i++)
   {
      CWsfDocInfo* pdi = GetDocInfo(m_CloseDocQueue[i]);
      if (pdi) VERIFY(CloseDoc(pdi, false));
   }

   m_CloseDocQueue.RemoveAll();

   // Perform delayed CloseView() calls

   for (i=0; i<m_CloseViewQueue.GetSize(); i++)
   {
      VERIFY(CloseView(m_CloseViewQueue[i], false));
   }

   m_CloseViewQueue.RemoveAll();

   return false;
}

/*****************************************************************************/
void CWsfApp::RegisterStandardCommands()
{
   AddIcons(IDB_SMALL_ICONS, IDB_LARGE_ICONS);

   AddCmdSimple("ID_HELP", ID_HELP, 0, IDB_SMALL_ICONS);
   AddCmdSimple("ID_WSF_TREE_VIEW", ID_WSF_TREE_VIEW, 0, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_OPEN", ID_WSF_OPEN, 15, IDB_SMALL_ICONS);
	AddCmdSimple("ID_FILE_OPEN", ID_FILE_OPEN, 1, IDB_SMALL_ICONS);
	AddCmdSimple("ID_FILE_SAVE", ID_FILE_SAVE, 2, IDB_SMALL_ICONS);
	AddCmdSimple("ID_FILE_SAVE_AS", ID_FILE_SAVE_AS, 19, IDB_SMALL_ICONS, true, true);
	AddCmdSimple("ID_WSF_SAVE_ALL", ID_WSF_SAVE_ALL, 3, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_SAVE_CLOSE", ID_WSF_SAVE_CLOSE, 2, IDB_SMALL_ICONS, true, true);
	AddCmdSimple("ID_EDIT_CUT", ID_EDIT_CUT, 4, IDB_SMALL_ICONS);
	AddCmdSimple("ID_EDIT_COPY", ID_EDIT_COPY, 5, IDB_SMALL_ICONS);
	AddCmdSimple("ID_EDIT_PASTE", ID_EDIT_PASTE, 6, IDB_SMALL_ICONS);
	AddCmdSimple("ID_FILE_PRINT", ID_FILE_PRINT, 7, IDB_SMALL_ICONS);
	AddCmdSimple("ID_FILE_PRINT_PREVIEW", ID_FILE_PRINT_PREVIEW, 8, IDB_SMALL_ICONS);
	AddCmdSimple("ID_EDIT_FIND", ID_EDIT_FIND, 24, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_DELETE", ID_WSF_DELETE, 10, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_PROPERTIES", ID_WSF_PROPERTIES, 11, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_REFRESH", ID_WSF_REFRESH, 12, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_RENAME", ID_WSF_RENAME, 13, IDB_SMALL_ICONS);
	AddCmdSimple("ID_WSF_PREVIEW_IN_BROWSER", ID_WSF_PREVIEW_IN_BROWSER, 14, IDB_SMALL_ICONS);
	AddCmdSimple("ID_EDIT_UNDO", ID_EDIT_UNDO, 18, IDB_SMALL_ICONS);
	AddCmdSimple("ID_EDIT_REDO", ID_EDIT_REDO, 23, IDB_SMALL_ICONS);

	AddCmdSimple("ID_VIEW_LARGEICON", ID_VIEW_LARGEICON);
	AddCmdSimple("ID_VIEW_SMALLICON", ID_VIEW_SMALLICON);
	AddCmdSimple("ID_VIEW_LIST", ID_VIEW_LIST);
	AddCmdSimple("ID_EDIT_SELECT_ALL", ID_EDIT_SELECT_ALL);
	AddCmdSimple("ID_APP_EXIT", ID_APP_EXIT);
	AddCmdSimple("ID_APP_ABOUT", ID_APP_ABOUT);
	AddCmdSimple("ID_FILE_MRU_FILE1", ID_FILE_MRU_FILE1);
	AddCmdSimple("ID_WSF_WINDOW_CLOSE", ID_WSF_WINDOW_CLOSE, 20, IDB_SMALL_ICONS, true, true);
	AddCmdSimple("ID_WSF_WINDOW_CLOSEALL", ID_WSF_WINDOW_CLOSEALL);
	AddCmdSimple("ID_WSF_PREVIOUS_PANE", ID_WSF_PREVIOUS_PANE);
	AddCmdSimple("ID_WSF_NEXT_PANE", ID_WSF_NEXT_PANE);
	AddCmdSimple("ID_WSF_COLUMNS", ID_WSF_COLUMNS);
	AddCmdSimple("ID_WSF_SORT_COLUMN", ID_WSF_SORT_COLUMN);
	AddCmdSimple("ID_FILE_CLOSE", ID_FILE_CLOSE);
	AddCmdSimple("ID_WSF_BROWSE_BACK", ID_WSF_BROWSE_BACK, 16, IDB_SMALL_ICONS, true, true);
	AddCmdSimple("ID_WSF_WINDOW_NEW", ID_WSF_WINDOW_NEW, 17, IDB_SMALL_ICONS, true, true);
	AddCmdSimple("ID_WSF_OPEN_IN_NEW_WINDOW", ID_WSF_OPEN_IN_NEW_WINDOW, 18, IDB_SMALL_ICONS, true, true);
	AddCmdSimple("ID_WSF_SORT_ASC", ID_WSF_SORT_ASC, 21, IDB_SMALL_ICONS, false, true);
	AddCmdSimple("ID_WSF_SORT_DESC", ID_WSF_SORT_DESC, 22, IDB_SMALL_ICONS, false, true);
	AddCmdSimple("ID_WSF_EXPORT_CSV", ID_WSF_EXPORT_CSV, 25, IDB_SMALL_ICONS, true, true);
}

//*****************************************************************************
void CWsfApp::LoadWsfSetup()
{
   BOOL bDefault = true;

   try
   {
      CWslPtr<CWslRegKey> k = GetAppRegistryKey("WSF");
   
      if (k->GetInt("version", 0) == m_dwWsfVersion)
      {
		   m_Setup.nX = k->GetInt("win_x", 0);
		   m_Setup.nY = k->GetInt("win_y", 0);
		   m_Setup.nW = k->GetInt("win_w", 600);
		   m_Setup.nH = k->GetInt("win_h", 400);
		   m_Setup.nShowType = k->GetInt("win_show_type", SW_SHOWMAXIMIZED);
		   m_Setup.nSplitter = k->GetInt("splitter_x", 200);
		   m_Setup.bLeftPaneVisible = k->GetInt("left_pane_visible", 1);
		   m_Setup.nListViewType = k->GetInt("list_view_type", LVS_LIST);
         bDefault = false;
      }
   }
   catch (CException* e)
   {
      ASSERT(false);
      e->Delete();
   }

   if (bDefault)
	{
		m_Setup.nX = 0;
		m_Setup.nY = 0;
		m_Setup.nW = 600;
		m_Setup.nH = 400;
		m_Setup.nSplitter = 200;
		m_Setup.bLeftPaneVisible = true;
		m_Setup.nShowType = SW_SHOWMAXIMIZED;
		m_Setup.nListViewType = LVS_LIST;
	}
}

/*****************************************************************************/
void CWsfApp::UpdateAppTitle()
{ 
	ASSERT(WsfGetFrame()); 
	WsfGetFrame()->OnUpdateFrameTitle(true); 
}

/*****************************************************************************/
CWslPtr<CWslRegKey> CWsfApp::GetAppRegistryKey(const char* pcszKey) throw (CException*)
{
   CWslPtr<CWslRegKey> k = new CWslRegKey();
   
   CString s;
   s.Format("Software\\%s\\%s", m_pszRegistryKey, m_pszProfileName);

   CWslRegKey AppKey;
   AppKey.Create(HKEY_CURRENT_USER, s);

   k->Create(AppKey, pcszKey);
   return k;      
}

/*****************************************************************************/
CWslPtr<CWslRegKey> CWsfApp::GetAppLocalMachineRegistryKey(const char* pcszKey,
                                                           bool bCreate) throw (CException*)
{
   CWslPtr<CWslRegKey> k = new CWslRegKey();
   
   CString s;
   s.Format("Software\\%s\\%s", m_pszRegistryKey, m_pszProfileName);

   if (pcszKey) 
   {
      s += '\\';
      s += pcszKey;
   }

   if (bCreate)
      k->Create(HKEY_LOCAL_MACHINE, s);
   else
      k->Open(HKEY_LOCAL_MACHINE, s, true);

   return k;      
}


/*****************************************************************************
*
* PLUG-INS
*
*****************************************************************************/

/*****************************************************************************/
const CWsfPluginsArray& CWsfApp::GetPlugins()
{
	return g_aAppPlugins;
}

/*****************************************************************************/
void CWsfApp::RegisterPlugin(CWsfPlugin* pPlugin)
{
	ASSERT_VALID(pPlugin);
	TRACE1("CWsfApp::RegisterAppPlugin(): %s\n", typeid(pPlugin).name());
	g_aAppPlugins += pPlugin;
}

/*****************************************************************************/
void CWsfApp::RemovePlugin(CWsfPlugin* pPlugin)
{
   for (int i=0; i<g_aAppPlugins.GetSize(); i++)
   {
      if (g_aAppPlugins[i] == pPlugin)
      {
         g_aAppPlugins.RemoveAt(i);
         break;
      }
   }
}

/*****************************************************************************/
CWsfPlugin* CWsfApp::GetPlugin(const char* pcszName)
{
   for (int i=0; i<g_aAppPlugins.GetSize(); i++)
   {
      if (!g_aAppPlugins[i]->GetName().CompareNoCase(pcszName)) 
         return g_aAppPlugins[i];
   }

   return NULL;
}

/*****************************************************************************
*
* TREE VIEWS
*
*****************************************************************************/

//*****************************************************************************
int CWsfApp::OpenTreeView(CWsfTreeItem* pRoot, 
                          CDocument* pDoc,
                          const char *pcszTitle,
                          int nPos,
                          BOOL bActivate,
                          int nIconBitmapId, 
                          int nIconIdx)
{
	ASSERT_VALID(pRoot);
	ASSERT(pcszTitle);
	ASSERT(pDoc);

	// Open view

	CWsfTreeItemTreeView *pView = new CWsfTreeItemTreeView();
	int nViewId = OpenView(pView, pDoc, pcszTitle, true, false, nPos, bActivate, nIconBitmapId, nIconIdx);
	ASSERT(nViewId != -1);

	// Init view

	pView->TreeHasChanged(pRoot, ITEM_ADDED);

	// Show workspace pane if hidden because there was no tree view before

	GetFrame()->ShowLeftPane(GetFrame()->IsLeftPaneVisible());

	return nViewId;
}

//*****************************************************************************
void CWsfApp::CloseTreeView(CWsfTreeItem* pRoot)
{
	ASSERT_VALID(pRoot);

	// Check all documents for a tree-view displaying 
   // the given root tree item

	for (int t=0; t < m_aDocs.GetSize(); t++)
	{
		CWsfDocInfo* pdi = m_aDocs[t];
		ASSERT(pdi);

		for (int i=0; i < pdi->aViews.GetSize(); i++)
		{
			ASSERT_VALID(pdi->aViews[i]->pView);
			CWsfTreeItemTreeView* p = dynamic_cast<CWsfTreeItemTreeView*>(pdi->aViews[i]->pView);

			if (p && p->GetRootItem() == pRoot) 
			{
				CloseView(pdi->aViews[i]->pView, false);
				return;
			}
		}

	}
}

/*****************************************************************************
*
* WORKSPACE
*
*****************************************************************************/

//*****************************************************************************
void CWsfApp::SetWorkspaceName(const char* pcszName)
{
	ASSERT(pcszName);
	ASSERT(m_pWorkspaceDoc);

	m_pWorkspaceDoc->SetTitle(pcszName);

	// Update frame title

	CWsfFrame *pFrame = GetFrame();
	if (pFrame) pFrame->OnUpdateFrameTitle(true);
}

//*****************************************************************************
void CWsfApp::SetWorkspaceName(CString& sName)
{
	ASSERT(m_pWorkspaceDoc);

	m_pWorkspaceDoc->SetTitle(sName);

	// Update frame title

	CWsfFrame *pFrame = GetFrame();
	if (pFrame) pFrame->OnUpdateFrameTitle(true);
}

//*****************************************************************************
BOOL CWsfApp::NewWorkspace(CWsfWorkspaceDoc* pDoc)
{
	ASSERT(pDoc);

	pDoc->SetPathName("");
	pDoc->m_bAutoDelete = false;
	m_pWorkspaceDoc = pDoc;
	OpenDoc(pDoc);

	if (!pDoc->OnNewDocument()) 
	{
		CloseDoc(pDoc);		// pDoc was deleted
		return false;
	}

	return true;
}

//*****************************************************************************
BOOL CWsfApp::OpenWorkspace(CWsfWorkspaceDoc* pDoc, const char* pcszPath)
{
	ASSERT(pDoc);
	ASSERT(pcszPath);

	pDoc->SetPathName(pcszPath);
	pDoc->m_bAutoDelete = false;
	m_pWorkspaceDoc = pDoc;
	OpenDoc(pDoc);

	if (!pDoc->OnOpenDocument(pcszPath)) 
	{
		CloseDoc(pDoc);		// pDoc was deleted
		return false;
	}

	return true;
}

//*****************************************************************************
BOOL CWsfApp::SaveWorkspace(BOOL bSaveAs)
{
	ASSERT(m_pWorkspaceDoc);
	return m_pWorkspaceDoc->DoSave(bSaveAs);
}

//*****************************************************************************
void CWsfApp::CloseWorkspace()
{
	if (m_pWorkspaceDoc)
   {
	   ASSERT(!m_pWorkspaceDoc->IsModified());
	   VERIFY(CloseDoc(m_pWorkspaceDoc));	// sets m_pWorkspaceDoc to NULL
   }
}

//*****************************************************************************
BOOL CWsfApp::SaveModifiedWorkspace()
{ 
	ASSERT(m_pWorkspaceDoc); 
	return m_pWorkspaceDoc->IsModified() ? 
		m_pWorkspaceDoc->SaveModified() : true;
}

//*****************************************************************************
void CWsfApp::SetWorkspaceModified(BOOL b /*= true*/)
{ 
	ASSERT(m_pWorkspaceDoc); 
	
	BOOL bOld = m_pWorkspaceDoc->IsModified();
	m_pWorkspaceDoc->SetModifiedFlag(b);

	if (b != bOld) UpdateAppTitle();
}

/*****************************************************************************
*
* DOCUMENTS
*
*****************************************************************************/

//*****************************************************************************
void CWsfApp::CloseAllDocuments(BOOL bEndSession)
{
	CWslArray<CWsfDocInfo*> aDocs;
	aDocs = m_aDocs;

	// Close all docs except the workspace

	for (int i=0; i<aDocs.GetSize(); i++)
	{
		ASSERT(!aDocs[i]->pDoc->IsModified());
		if (aDocs[i]->pDoc != m_pWorkspaceDoc) VERIFY(CloseDoc(aDocs[i]->pDoc));
	}

	// Close workspace as the last document

	if (m_pWorkspaceDoc) VERIFY(CloseDoc(m_pWorkspaceDoc));
}

//*****************************************************************************
void CWsfApp::CloseAllOrdinaryDocs(BOOL bRightPaneOnly)
{
	CWslArray<CWsfDocInfo*> aDocs;
	aDocs = m_aDocs;

	for (int i=0; i<aDocs.GetSize(); i++)
	{
		if (aDocs[i]->pDoc == m_pWorkspaceDoc) continue;
		
		if (bRightPaneOnly)
		{
			for (int t=aDocs[i]->aViews.GetSize(); t--;)
			{
				if (aDocs[i]->aViews[t]->bLeftPane) goto L1;
			}
		}

		ASSERT(!aDocs[i]->pDoc->IsModified());
		VERIFY(CloseDoc(aDocs[i]->pDoc));
L1:;
	}
}

//*****************************************************************************
BOOL CWsfApp::SaveAllModifiedOrdinaryDocs()
{
	// At first save all ordinary docs and then the workspace if any
	
	for (int i=m_aDocs.GetSize(); i--;)
	{
		CDocument* pDoc = m_aDocs[i]->pDoc;
		if (pDoc != m_pWorkspaceDoc && pDoc->IsModified())
		{
			if (!pDoc->SaveModified()) return false;
		}
	}

	if (m_pWorkspaceDoc && m_pWorkspaceDoc->IsModified()) 
	{
		if (!m_pWorkspaceDoc->SaveModified()) return false;
	}

	return true;
}

//*****************************************************************************
BOOL CWsfApp::SaveAllModified()
{
	return SaveAllModifiedOrdinaryDocs();
}

//*****************************************************************************
void CWsfApp::UpdateAllDocs(CView* pSender, int nHint, void* pData)
{
	for (int i = m_aDocs.GetSize(); i--;)
	{
		m_aDocs[i]->pDoc->UpdateAllViews(pSender, nHint, (CObject*) pData);
	}
}

//*****************************************************************************
CWsfDocInfo* CWsfApp::GetNextDocInfo(CDocument* pDoc)
{
	ASSERT_VALID(pDoc);

	for (int i = m_aDocs.GetSize(); i--;)
	{
		if (m_aDocs[i]->pDoc == pDoc)
		{
			if (++i == m_aDocs.GetSize()) i = 0;
			return m_aDocs[i];
		}
	}

	return NULL;
}

//*****************************************************************************
CWsfDocInfo* CWsfApp::GetDocInfo(const CString& sDocTitle)
{
	for (int i = m_aDocs.GetSize(); i--;)
	{
		ASSERT_VALID(m_aDocs[i]->pDoc);
		if (m_aDocs[i]->pDoc->GetTitle() == sDocTitle) return m_aDocs[i];
	}
	return NULL;
}

//*****************************************************************************
CWsfDocInfo* CWsfApp::GetDocInfo(CDocument* pDoc)
{
	for (int i = m_aDocs.GetSize(); i--;)
		if (m_aDocs[i]->pDoc == pDoc) return m_aDocs[i];

	return NULL;
}

/*****************************************************************************/
CWsfDocInfo* CWsfApp::GetDocInfo(int nID)
{
	for (int i = m_aDocs.GetSize(); i--;)
		if (m_aDocs[i]->nID == nID) return m_aDocs[i];

	return NULL;
}

/*****************************************************************************/
CWsfDocInfo* CWsfApp::GetDocInfoByPathName(const CString& sPathName)
{
	for (int i = m_aDocs.GetSize(); i--;)
	{
		ASSERT_VALID(m_aDocs[i]->pDoc);
		TRACE2("%s=%s\n",m_aDocs[i]->pDoc->GetPathName(), sPathName);
		if (!stricmp(m_aDocs[i]->pDoc->GetPathName(),sPathName)) return m_aDocs[i];
	}
	return NULL;
}

//*****************************************************************************
void CWsfApp::OpenDoc(CDocument *pDoc)
{
	ASSERT_VALID(pDoc);
	ASSERT(GetDocInfo(pDoc)==NULL); // must exist

	CWsfDocInfo* pdi = new CWsfDocInfo();
	m_aDocs += pdi;

   pdi->nID = GetID();
	pdi->pDoc = pDoc;

	pDoc->m_bAutoDelete = false;
}

//*****************************************************************************
int CWsfApp::OpenHtmlFile(const CString& sFileName, 
                          const CString& sViewName,
                          int nIconBitmapId, 
                          int nIconIdx,
                          BOOL bDeleteFileOnClose,
                          BOOL bAlwaysOpenNewView)
{
   // Check if the file isn't already opened
   // If it is, activate and refresh its view.

   if (!bAlwaysOpenNewView)
   {
      CWsfDocInfo* pdi = GetDocInfoByPathName(sFileName);
   
      if (pdi && ActivateDoc(pdi->pDoc)) 
      {
         CWsfHtmlView* p = dynamic_cast<CWsfHtmlView*>(pdi->aViews[0]->pView);
         if (p) p->Refresh();
         return pdi->aViews[0]->nID;
      }
   }
   
   // Open doc & view
   
   CWsfHtmlView* pView = new CWsfHtmlView();
   CDocument* pDoc = new CWsfHtmlDoc(bDeleteFileOnClose);
   pDoc->SetPathName(sFileName, false);
   pDoc->SetModifiedFlag(false);
   pDoc->SetTitle(sViewName);
   
   OpenDoc(pDoc);
   int nId = OpenView(pView, pDoc, sViewName, false, true, -1, true, nIconBitmapId, nIconIdx);
   VERIFY(nId);
   
   // Go to the page
   
   pView->Navigate(sFileName);
   
   return nId;
}

//*****************************************************************************
BOOL CWsfApp::CloseDoc(const CString& sDocTitle, BOOL bDelayed)
{
	CWsfDocInfo* pInfo = GetDocInfo(sDocTitle);
	return pInfo ? CloseDoc(pInfo, bDelayed) : true;
}

//*****************************************************************************
BOOL CWsfApp::CloseDoc(CDocument* pDoc, BOOL bDelayed)
{
	ASSERT_VALID(pDoc);
	CWsfDocInfo* pInfo = GetDocInfo(pDoc);
	return pInfo ? CloseDoc(pInfo, bDelayed) : true;
}

//*****************************************************************************
BOOL CWsfApp::CloseDoc(CWsfDocInfo* pdi, BOOL bDelayed)
{
	ASSERT(pdi);

   // Handle delayed closing

   if (bDelayed)
   {
      m_CloseDocQueue += pdi->nID;
      return true;
      //return GetFrame()->PostMessage(WSF_WM_CLOSE_DOC, (WPARAM) pdi->nID, 0);
   }

	// Ask the doc, if it can close

	if (pdi->pDoc->IsModified())
	{
		if (!pdi->pDoc->SaveModified()) return false;
	}

	// Auto delete is not allowed!

	ASSERT(!pdi->pDoc->m_bAutoDelete);

	// Close & destroy all views

	for (int i=pdi->aViews.GetSize(); i--;)
	{
		CWsfViewInfo* pvi = pdi->aViews[i];
		CWsfTabWnd* pTabWnd = pvi->bLeftPane ? GetFrame()->m_pLeftPane : GetFrame()->m_pRightPane;

      pTabWnd->CloseWnd(pvi->pView);
      VERIFY(pvi->pView->DestroyWindow());
      
      delete pvi;
	}

	pdi->aViews.RemoveAll();

	// Call OnCloseDocument()
	// TODO: call for all documents, now CDocument's implementation
	// destroys the frame

	if (m_pWorkspaceDoc == pdi->pDoc) 
		pdi->pDoc->OnCloseDocument();

	// Special workspace handling

	if (m_pWorkspaceDoc == pdi->pDoc) 
	{
		m_pWorkspaceDoc = NULL;
		CWsfFrame *pFrame = GetFrame();
		if (pFrame) pFrame->OnUpdateFrameTitle(true);
	}

	// Delete document object

	delete pdi->pDoc;

	// Delete doc & doc info

	for (i=m_aDocs.GetSize(); i--;)
	{
		if (m_aDocs[i] == pdi)
		{
			m_aDocs.RemoveAt(i);
			delete pdi;
			break;
		}
	}

	// Hide left pane if there are no more views

	if (!GetFrame()->m_pLeftPane->GetTabCount())
		GetFrame()->ShowLeftPane(false);

	return true;
}

//*****************************************************************************
BOOL CWsfApp::ActivateDoc(CDocument* pDoc)
{
	CWsfDocInfo* pdi;
	ASSERT_VALID(pDoc);

	pdi = GetDocInfo(pDoc);
	if (!pdi) return false;

	ASSERT(pdi->aViews.GetSize() > 0);	// there must at leat one view

	ActivateView(pdi->aViews[0]);
	return true;
}

//*****************************************************************************
BOOL CWsfApp::ActivateDoc(const CString& sDocTitle)
{
	CWsfDocInfo* pdi;

	pdi = GetDocInfo(sDocTitle);
	if (!pdi) return false;

	ASSERT(pdi->aViews.GetSize() > 0);	// there must at leat one view

	ActivateView(pdi->aViews[0]);
	return true;
}



/*****************************************************************************
*
* VIEWS
*	
*****************************************************************************/

//*****************************************************************************
int CWsfApp::OpenView(CView* pView, 
                      CDocument* pDoc, 
                      const char* pcszTitle,
                      BOOL bLeftPane /*= false*/,
                      BOOL bEnableCloseWindow /*= true*/,
                      int	nPos /*= -1*/,
                      BOOL bActivate /*= true*/,
                      int nIconBitmapId, 
                      int nIconIdx)
{
	CWsfViewInfo* pvi;
	CCreateContext cc;
	CWsfTabWnd* pTabWnd;

	ASSERT_VALID(pView);
	ASSERT_VALID(pDoc);
	ASSERT(pcszTitle);

	// Get doc info

	CWsfDocInfo *pdi = GetDocInfo(pDoc);
	ASSERT(pdi);			// the given doc was not opened by OpenDoc() method

	// Check if some view with given name exists
/*
	for (i=pdi->aViews.GetSize(); i--;)
	{
		if (pdi->aViews[i]->sTitle == pcszViewName) 
		{
			if (bActivate) ActivateView(pdi->aViews[i]);
			return 0;
		}
	}
*/
	// Add view

	pvi = new CWsfViewInfo();
	pdi->aViews += pvi;

	pvi->nID = GetID();
	pvi->bEnableCloseWindow = bEnableCloseWindow;
	pvi->bLeftPane = bLeftPane;
	pvi->pView = pView;
	pvi->sTitle = pcszTitle;

	// Create view

	cc.m_pCurrentDoc = pDoc;
	cc.m_pNewViewClass = NULL;
	cc.m_pCurrentFrame = GetFrame();
	cc.m_pLastView = NULL;
	cc.m_pNewDocTemplate = NULL;

	pTabWnd = bLeftPane ? GetFrame()->m_pLeftPane : GetFrame()->m_pRightPane;

	VERIFY(pView->Create(NULL, NULL, WS_CHILD, WsfZeroRect, pTabWnd, 1001, &cc));
   pView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	pView->SetWindowText(pcszTitle);

   // WM_INITIALUPDATE

	pView->SendMessage(0x0364, 0,0);

	// Add to the tab window
	// NOTE: Use pvi->sTitle for the title, because
	// the title could have been changed by WM_INITIALUPDATE

	pTabWnd->OpenTab(pView, pvi->sTitle, nPos, bActivate, bEnableCloseWindow, 
      GetIconIdx(nIconBitmapId, nIconIdx));

	// Show left pane if  there are some views and if it's enabled

	if (bLeftPane && GetFrame()->IsLeftPaneVisible())
		GetFrame()->ShowLeftPane(true);

	return pvi->nID;
}

/*****************************************************************************/
int CWsfApp::ReplaceView(CView* pCurrentView, 
                         CView* pNewView,
                         CDocument* pDoc,
                         const char* pcszTitle,
                         BOOL bLeftPane,
                         BOOL bEnableCloseWindow,
                         BOOL bActivate,
                         int nIconBitmapId, 
                         int nIconIdx)
{
   // Does the view to be replaced exist?
   // If not, open the view in a new tab

   if (!pCurrentView || !GetViewInfo(pCurrentView))
   {
      return OpenView(pNewView, pDoc, pcszTitle, bLeftPane, bEnableCloseWindow, -1, bActivate, nIconBitmapId, nIconIdx);
   }

   ASSERT(GetViewInfo(pCurrentView)->bLeftPane == bLeftPane);

   // Save old view's document

   if (pCurrentView->GetDocument()->IsModified())
   {
      if (!pCurrentView->GetDocument()->SaveModified()) return 0;
   }

   // Register new view

   CWsfDocInfo* pdi = GetDocInfo(pDoc);
   if (!pdi) return 0;

	CWsfViewInfo* pvi = new CWsfViewInfo();
	pdi->aViews += pvi;

	pvi->nID = GetID();
	pvi->bEnableCloseWindow = bEnableCloseWindow;
	pvi->bLeftPane = bLeftPane;
	pvi->pView = pNewView;
	pvi->sTitle = pcszTitle;

	// Create new view

	CCreateContext cc;
	cc.m_pCurrentDoc = pDoc;
	cc.m_pNewViewClass = NULL;
	cc.m_pCurrentFrame = GetFrame();
	cc.m_pLastView = NULL;
	cc.m_pNewDocTemplate = NULL;

   CWsfTabWnd* pTabWnd = GetFrame()->GetPane(pvi->bLeftPane);

	VERIFY(pNewView->Create(NULL, NULL, WS_CHILD, WsfZeroRect, pTabWnd, 1001, &cc));
   pNewView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	pNewView->SetWindowText(pcszTitle);

   // WM_INITIALUPDATE

	pNewView->SendMessage(0x0364, 0,0);

   // Update tab

   VERIFY(pTabWnd->SetTabWnd(pCurrentView, 
      pNewView, 
      pcszTitle, 
      GetIconIdx(nIconBitmapId, nIconIdx)));

   // Close old view

   CloseView(pCurrentView, true);

   // Activate

   if (bActivate) pTabWnd->SetActiveWnd(pNewView, true);

   return pvi->nID;
}

/*****************************************************************************/
int CWsfApp::ReplaceActiveView(CView* pNewView,
                               CDocument* pDoc,
                               const char* pcszTitle,
                               BOOL bLeftPane,
                               BOOL bEnableCloseWindow,
                               BOOL bActivate,
                               int nIconBitmapId, 
                               int nIconIdx)
{
   CView* p = GetActivePaneView(bLeftPane);
   return ReplaceView(p, pNewView, pDoc, pcszTitle, bLeftPane, bEnableCloseWindow, bActivate, nIconBitmapId, nIconIdx);
}

//*****************************************************************************
BOOL CWsfApp::CloseView(const char* pcszViewName, BOOL bDelayed)
{
	ASSERT(pcszViewName);

	for (int t=m_aDocs.GetSize(); t--;)
	{
		CWsfDocInfo* pdi = m_aDocs[t];
		for (int i=pdi->aViews.GetSize(); i--;)
		{
			if (pdi->aViews[i]->sTitle == pcszViewName) 
			{
				return CloseView(pdi, pdi->aViews[i], i, bDelayed);
			}
		}
	}

	return false;
}

//*****************************************************************************
BOOL CWsfApp::CloseView(CView* pView, BOOL bDelayed)
{
	ASSERT_VALID(pView);

	for (int t=m_aDocs.GetSize(); t--;)
	{
		CWsfDocInfo* pdi = m_aDocs[t];
		for (int i=pdi->aViews.GetSize(); i--;)
		{
			if (pdi->aViews[i]->pView == pView) 
			{
				return CloseView(pdi, pdi->aViews[i], i, bDelayed);
			}
		}
	}

	return false;
}

/*****************************************************************************/
BOOL CWsfApp::CloseView(int nID, BOOL bDelayed)
{
	for (int t=m_aDocs.GetSize(); t--;)
	{
		CWsfDocInfo* pdi = m_aDocs[t];
		for (int i=pdi->aViews.GetSize(); i--;)
		{
			if (pdi->aViews[i]->nID == nID) 
			{
				return CloseView(pdi, pdi->aViews[i], i, bDelayed);
			}
		}
	}
	return false;
}

//*****************************************************************************
BOOL CWsfApp::CloseView(CWsfDocInfo* pdi, 
                        CWsfViewInfo* pvi, 
                        int nViewIdx, 
                        BOOL bDelayed)
{
   if (bDelayed) 
   {
      m_CloseViewQueue += pvi->nID;
      return true;
   }

	// Check, if it's the last view of the document
   // If the doc is the workspace doc, don't close it!

	if (m_pWorkspaceDoc != pdi->pDoc && pdi->aViews.GetSize()==1)
	{
		// It also closes the view
		return CloseDoc(pdi);
	}

	// Close the view

	pdi->aViews.RemoveAt(nViewIdx);

   CWsfTabWnd* pTabWnd = pvi->bLeftPane ? GetFrame()->m_pLeftPane : GetFrame()->m_pRightPane;
	
   pTabWnd->CloseWnd(pvi->pView);
   VERIFY(pvi->pView->DestroyWindow()); // deletes the view

	delete pvi;
	return true;
}

//*****************************************************************************
void CWsfApp::CloseAllPaneViews(BOOL bLeftPane)
{
	for (int t=m_aDocs.GetSize(); t--;)
	{
		CWsfDocInfo* pdi = m_aDocs[t];

		for (int i=pdi->aViews.GetSize(); i--;)
		{
			if (pdi->aViews[i]->bLeftPane == bLeftPane) 
			{
				VERIFY(CloseView(pdi->aViews[i]->pView, true));
			}
		}
	}

}

//*****************************************************************************
BOOL CWsfApp::ActivateView(const char* pcszViewName)
{
	ASSERT(pcszViewName);
	CWsfViewInfo* pvi = GetViewInfo(pcszViewName);
	if (pvi) { ActivateView(pvi); return true; }
	return false;
}

/*****************************************************************************/
BOOL CWsfApp::ActivateView(int nViewID)
{
	CWsfViewInfo* pvi = GetViewInfo(nViewID);
	if (pvi) { ActivateView(pvi); return true; }
	return false;
}

//*****************************************************************************
void CWsfApp::ActivateView(CWsfViewInfo* pvi)
{
	ASSERT(pvi);
	CWsfTabWnd* pTabWnd;
	pTabWnd = pvi->bLeftPane ? GetFrame()->m_pLeftPane : GetFrame()->m_pRightPane;
	pTabWnd->SetActiveWnd(pvi->pView);
}

//*****************************************************************************
BOOL CWsfApp::ActivateView(CView* pView)
{
	ASSERT_VALID(pView);
	CWsfViewInfo* pvi = GetViewInfo(pView);
	if (pvi) { ActivateView(pvi); return true; }
	return false;
}

//*****************************************************************************
CWsfViewInfo* CWsfApp::GetViewInfo(CView* pView)
{
	ASSERT_VALID(pView);

	for (int i=0; i<m_aDocs.GetSize(); i++)
	{
		CWsfDocInfo* pdi = m_aDocs[i];
		for (int t=0; t<pdi->aViews.GetSize(); t++)
			if (pdi->aViews[t]->pView == pView) return pdi->aViews[t];
	}

	return NULL;
}

//*****************************************************************************
CWsfViewInfo* CWsfApp::GetViewInfo(int nViewID)
{
	for (int i=0; i<m_aDocs.GetSize(); i++)
	{
		CWsfDocInfo* pdi = m_aDocs[i];
		for (int t=0; t<pdi->aViews.GetSize(); t++)
			if (pdi->aViews[t]->nID == nViewID) return pdi->aViews[t];
	}
	return NULL;
}

/*****************************************************************************/
CWsfViewInfo* CWsfApp::GetViewInfo(const char* pcszViewName)
{
	ASSERT(pcszViewName);

	for (int i=0; i<m_aDocs.GetSize(); i++)
	{
		CWsfDocInfo* pdi = m_aDocs[i];
		for (int t=0; t<pdi->aViews.GetSize(); t++)
			if (pdi->aViews[t]->sTitle == pcszViewName) return pdi->aViews[t];
	}
	return NULL;
}

//*****************************************************************************
void CWsfApp::SetViewTitleIcon(CWsfViewInfo* pvi, 
                               const char* pcszTitle, 
                               int nIconIdx /*=-1*/)
{
	ASSERT(pvi);
	ASSERT(pcszTitle);
	ASSERT_VALID(pvi->pView);
	
	pvi->sTitle = pcszTitle;
	pvi->pView->SetWindowText(pcszTitle);

	CWsfTabWnd* p = GetFrame()->GetPane(pvi->bLeftPane);
	p->SetTabTitleIcon(pvi->pView, pcszTitle, nIconIdx);

	// If the view is in the right pane and is active,
	// update also the application title
		
	if (!pvi->bLeftPane && p->GetActiveWnd()==pvi->pView)
	{
		UpdateAppTitle();
	}
}

/*****************************************************************************
*
* PANES
*
*****************************************************************************/

//*****************************************************************************
CView* CWsfApp::GetActivePaneView(BOOL bLeftPane)
{
	return (CView*) GetFrame()->GetPane(bLeftPane)->GetActiveWnd();
}


/*****************************************************************************
*
* TREE ITEMS
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfApp::DeleteTreeItem(CWsfTreeItem* pItem, BOOL bUpdateViews, DWORD dwData)
{
	return __DeleteTreeItem(pItem, bUpdateViews, true, dwData);
}

//*****************************************************************************
BOOL CWsfApp::DeleteSubTree(CWsfTreeItem* pItem, BOOL bUpdateViews, DWORD dwData)
{
	return __DeleteTreeItem(pItem, bUpdateViews, false, dwData);
}

//*****************************************************************************
BOOL CWsfApp::__DeleteTreeItem(CWsfTreeItem* pItem, 
															 BOOL bUpdateViews,
															 BOOL bDeleteRoot,
															 DWORD dwData)
{
	CWsfTreeItem* pParent = pItem->GetParent();
	// Root item cannot be deleted by this function. Use delete operator instead. 
	ASSERT(!bDeleteRoot || pItem->GetParent());		

	// OnDeleteSubTree()

	if (!pItem->OnDeleteSubTree()) return false;

	// Delete item

	BOOL b = __DeleteTreeItem(pItem, bDeleteRoot, dwData);

	// Update UI

	if (bUpdateViews)
	{
		TreeHasChanged(bDeleteRoot ? pParent : pItem, CHILDERN_DELETED);
	}

	// Error

	if (!b)
	{
		ASSERT_VALID(m_pTempTreeItem);
		UpdateAllDocs(NULL, WSF_UV_ENSURE_TI_VISIBLE, m_pTempTreeItem);
		CString s;
		s.Format(IDS_ITEM_DELETE_ERROR, m_pTempTreeItem->GetPathTitle());
		AfxMessageBox(s);
	}

	return b;
}

//*****************************************************************************
BOOL CWsfApp::__DeleteTreeItem(CWsfTreeItem* pItem, BOOL bDeleteRoot, DWORD dwData)
{
  // Delete all childern
	
	for (CWsfTreeItem* p=pItem->GetChild(); p;)
	{
		CWsfTreeItem* p2 = p->GetNextSibling(); 
		if (!__DeleteTreeItem(p, true, dwData)) return false;
		p = p2;
	}
	
	// Delete the root item
	
	if (bDeleteRoot)
	{
		if (!pItem->OnDelete(dwData)) 
		{
			m_pTempTreeItem = pItem;
			return false;
		}
		delete pItem;
	}

	return true;
}

//*****************************************************************************
void CWsfApp::TreeHasChanged(CWsfTreeItem* pSubTree, ETreeChanges eChange)
{
	CWsfTreeChangeInfo UpdateInfo;

	// Update all docs

	UpdateInfo.eChange = eChange;
	UpdateInfo.pItem = pSubTree;

	UpdateAllDocs(NULL, WSF_UV_TREE_HAS_CHANGED, &UpdateInfo);

	// Update workplace tabwindow title & doc title if tree item title
	// has changed.

/*	if (nChangeType & (WSF_TCH_TITLE_ICON | WSF_TCH_GENERAL))
	{
		CString sTitle;
		CWsfDocInfo *pInfo;

		for (int i=m_aDocs.GetSize(); i--;)
		{
			pInfo = m_aDocs[i];

			if (pInfo->pTreeItem)
			{
				sTitle = pInfo->pTreeItem->GetTitle();
				
				if (sTitle != pInfo->pDoc->GetTitle())
				{
					pInfo->pDoc->SetTitle(sTitle);
					GetFrame()->m_pRightPane->UpdateWndTitle(pInfo->aViews[0]->pView, sTitle);
				}
			}
		}
	}*/

}

//*****************************************************************************
void CWsfApp::EnsureTreeItemVisible(CWsfTreeItem* pItem)
{
	ASSERT_VALID(pItem);
	UpdateAllDocs(NULL, WSF_UV_ENSURE_TI_VISIBLE, pItem);
}

//*****************************************************************************
void CWsfApp::DoTreeItemsDragDrop(CWslArray<CWsfTreeItem*>& aItems)
{
	COleDataSource ds;
	CWsfTreeItem* *ppItem;
	int nSize;
	HGLOBAL hData;

	// Prepare data for the data object

	nSize = aItems.GetSize();
	if (!nSize) return;
	
	hData = GlobalAlloc(GHND | GMEM_SHARE, (nSize+1) * sizeof(CWsfTreeItem*));
	ASSERT(hData);
	ppItem = (CWsfTreeItem**) GlobalLock(hData);
	ASSERT(ppItem);
	memcpy(ppItem, aItems.GetData(), nSize * sizeof(CWsfTreeItem*));
	ppItem[nSize] = NULL;				// Mark the end of the item list
	GlobalUnlock(hData);
	
	// Do drag & drop

	ds.CacheGlobalData(m_cfTreeItemList, hData);
	ds.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK);
} 



/*****************************************************************************
*
* DATABASE
*
*****************************************************************************/

//*****************************************************************************
void CWsfApp::SendDbEvent(CWsfDbEvent::EEvents eEvent, 
                          const char* pcszTblName, 
                          int nPrimaryKey)
{
   // NOTE: This method can be called from a thread that the main UI thread.
   // So, we use the message queue to move the processing to the 
   // main UI thread.

   // NOTE: The message handler deletes the object
	CWsfDbEvent& s = *new CWsfDbEvent();
	
	s.eEvent = eEvent;
	s.sTblName = pcszTblName;
	s.nPrimaryKey = nPrimaryKey;

	GetFrame()->SendMessage(WSF_WM_DB_HAS_CHANGED, (WPARAM) &s, 0);
}



/*****************************************************************************
*
* HELP
*
*****************************************************************************/

//*****************************************************************************
void CWsfApp::OnContextHelp()
{
	//m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_CONTEXTHELP, 0);
	CWinApp::OnContextHelp();
}

//*****************************************************************************
void CWsfApp::OnHelp()
{
	// No support for MFC help style. This must be empty.
	//CWinApp::OnHelp();
}

//*****************************************************************************
void CWsfApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	ASSERT(false);
/*	CString strChm = WsfGetApp()->m_sAppPath + WSF_HELP_FILE_NAME;

	switch (nCmd)
	{
	case HELP_INDEX:
		HtmlHelp(*m_pMainWnd, strChm, HH_DISPLAY_TOPIC, NULL);
		break;
	
	case HELP_CONTEXT:
		strChm.Format("Help not supported. (ID=%d)", dwData);
		AfxMessageBox(strChm);
		break;

	default:
		ASSERT(false);
		break;
	}*/
}

/*****************************************************************************
*
* CONTROL BARS
*
*****************************************************************************/

//*****************************************************************************
CControlBar* CWsfApp::AddControlBar(CControlBar* pControlBar, 
																		const char* pcszName, 
																		BOOL bVisible)
{
	ASSERT_VALID(pControlBar);
	ASSERT_VALID(GetFrame());
	CString s;
	
	// Insert it to the list of control bars
	// in the right order
   int i;
	for (i=0; i<m_aControlBars.GetSize(); i++)
	{
		m_aControlBars[i]->GetWindowText(s);
		if (strcoll(s, pcszName) > 0) break;
		}
	m_aControlBars.InsertAt(i, pControlBar);

	// Init control bar

	pControlBar->SetBarStyle(pControlBar->GetBarStyle() | 
		CBRS_TOP | CBRS_SIZE_DYNAMIC |
		CBRS_TOOLTIPS | CBRS_GRIPPER |
		CBRS_BORDER_TOP | CBRS_BORDER_RIGHT | CBRS_BORDER_LEFT | CBRS_BORDER_3D);

	pControlBar->SetWindowText(pcszName);
	pControlBar->EnableDocking(CBRS_ALIGN_ANY);
	GetFrame()->ShowControlBar(pControlBar, bVisible, true);
	GetFrame()->DockControlBar(pControlBar, AFX_IDW_DOCKBAR_TOP);

	return pControlBar;
}

//*****************************************************************************
CWsfToolBar *CWsfApp::AddToolBar(const char *pcszName, int nID,
																 int nBmpId, 
																 BOOL bVisible /*= true*/)
{
	BOOL b;
	ASSERT_VALID(GetFrame());
	ASSERT(nID);
	
	CToolBar *p = new CToolBar();

	b = p->CreateEx(GetFrame(), TBSTYLE_FLAT, WS_CHILD, WsfZeroRect, nID);
	if (!b) { ASSERT(false); delete p; return NULL; }

	p->SetHeight(27);
	if (!p->LoadToolBar(nBmpId)) { ASSERT(false); delete p; return NULL; }

	return (CWsfToolBar*) AddControlBar(p, pcszName, bVisible);
}

//*****************************************************************************
CWsfToolBar *CWsfApp::AddToolBar(const char *pcszName, int nID, 
																 BOOL bVisible /*= true*/)
{
	BOOL b;
	ASSERT_VALID(GetFrame());
	ASSERT(nID);
	
	CWsfToolBar *p = new CWsfToolBar();

	b = p->Create(GetFrame(), WS_CHILD, nID);
	if (!b) { ASSERT(false); delete p; return NULL; }
   
	p->SetCmdBitmaps();

	return (CWsfToolBar*) AddControlBar(p, pcszName, bVisible);
}

//*****************************************************************************
void CWsfApp::ShowControlBar(int nId, BOOL bShow)
{
   int i;
	for (i=m_aControlBars.GetSize(); i--;)
		if (GetWindowLong(*m_aControlBars[i], GWL_ID) == nId) break;
	ASSERT(i!=-1);
	ASSERT_VALID(m_aControlBars[i]);

	GetFrame()->ShowControlBar(m_aControlBars[i], bShow, false);
}


/*****************************************************************************
*
* ICONS
*
*****************************************************************************/

//*****************************************************************************
void CWsfApp::AddIcons(int nSmallBmpId, int nNormalBmpId)
{
	m_pSmallIconList->AddIcons(nSmallBmpId, false);
	m_pSmallIconListGray->AddIcons(nSmallBmpId, true);
   m_pNormalIconList->AddIcons(nNormalBmpId, false);

	// The number of icons must be the same.
	// If it fails, the given bitmaps doesn't have the same icon count!!!

	ASSERT(m_pNormalIconList->GetImageCount() == m_pSmallIconList->GetImageCount());
}

//*****************************************************************************
void CWsfApp::AddColumnIcons(int nBmpId)
{
	m_pColumnIconList->AddIcons(nBmpId, false);
}

//*****************************************************************************
int CWsfApp::GetIconIdx(int nSmallBmpId, int nIconIdx)
{
	int n = m_pSmallIconList->GetIconIdx(nSmallBmpId, nIconIdx);
	ASSERT(n != -1);	// icon must exist
	return n;
}

//*****************************************************************************
int CWsfApp::GetColumnIconIdx(int nBmpId, int nIconIdx)
{
	int n = m_pColumnIconList->GetIconIdx(nBmpId, nIconIdx);
	ASSERT(n != -1);	// icon must exist
	return n;
}

/*****************************************************************************
*
* PATHS
*
*****************************************************************************/

/*****************************************************************************/
CString CWsfApp::GetAppPath()
{
   return m_sAppPath;
}

/*****************************************************************************/
CString CWsfApp::GetLocalAppDataPath() throw (CException*)
{
   ASSERT(sizeof(temp) >= MAX_PATH);

   if (SHGetFolderPath(*WsfGetFrame(), CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, temp) != S_OK)
   {
      throw new CWslTextException((int)IDS_ERROR_READING_FOLDER_PATH);
   }

	PathRemoveBackslash(temp);

   CString s;
	s.Format("%s\\%s\\%s\\", temp, m_pszRegistryKey, m_pszProfileName);

   return s;
}

/*****************************************************************************/
void CWsfApp::SetAppIdentity(int nAppNameStringId, 
                             HMODULE hModuleWithVersionInfo,
                             const CString& sCompanyName)
{
   ASSERT(!sCompanyName.IsEmpty());
   m_sCompany = sCompanyName;

   CString s, s2;
   VERIFY(s.LoadString(nAppNameStringId));
   VERIFY(GetModuleFileName(hModuleWithVersionInfo, temp, 1024));
   m_dwAppVersion = WsfGetDllVersion(temp);
   ASSERT(m_dwAppVersion);

   m_sAppNameWithoutVersion = s;

   s2 = s + ' ' + WsfFormatDllVersion(m_dwAppVersion);
   free((void*)m_pszAppName);
   m_pszAppName = _strdup(s2);
   // New in MFC 7. See CWinApp::SetCurrentHandles() in appinit.cpp.
   AfxGetModuleState()->m_lpszCurrentAppName = m_pszAppName;   

   SetRegistryKey(sCompanyName);
   s2 = s + '\\' + WsfFormatDllVersion(m_dwAppVersion);
   free((void*)m_pszProfileName);
   m_pszProfileName = _strdup(s2);

   // Load setup from registry
   
   LoadWsfSetup();
}

/*****************************************************************************/
CString CWsfApp::GetAppDataPath() throw (CException*)
{
   ASSERT(sizeof(temp) >= MAX_PATH);

   if (SHGetFolderPath(*WsfGetFrame(), CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, temp) != S_OK)
   {
      throw new CWslTextException(IDS_ERROR_READING_FOLDER_PATH);
   }

	PathRemoveBackslash(temp);

   CString s;
	s.Format("%s\\%s\\%s\\", temp, m_pszRegistryKey, m_pszProfileName);

   return s;
}

/*****************************************************************************/
CString CWsfApp::GetTempPath() throw (CException*)
{
   if (m_sTempPath.IsEmpty())
   {
      ASSERT(sizeof(temp) >= MAX_PATH);
   
      if (!::GetTempPath(1024, temp)) throw new CWslWin32Exception();
      PathRemoveBackslash(temp);

      CWslStringArray a;
      a.ExplodeString(GetAppName(), " ");

      CString s;
      for (int i=0; i<a.GetSize(); i++) s += a[i][0];

      m_sTempPath.Format("%s\\%s %x%x%x%x\\", temp, s, rand(), rand(), rand(), rand());
      CWslFS::CreatePath(m_sTempPath);
   }

   return m_sTempPath;
}

/*****************************************************************************/
CString CWsfApp::GetTempPathName(const char* pcszExtension) throw (CException*)
{
   ASSERT(pcszExtension);
   CString s;

   do
   {
      s.Format("%s%x%x%x%x.%s", GetTempPath(), rand(), rand(), rand(), rand(), pcszExtension);
   }
   while (CWslFile::Exists(s));

   return s;
}


/*****************************************************************************
*
* COMMAND HANDLERS
*
*****************************************************************************/

//*****************************************************************************
void CWsfApp::OnWsfSaveAll()
{
   // Save all normal docs

	for (int i=m_aDocs.GetSize(); i--;)
	{
		CDocument* pDoc = m_aDocs[i]->pDoc;

		if (pDoc != m_pWorkspaceDoc && pDoc->IsModified()) 
      {
         pDoc->OnCmdMsg(ID_FILE_SAVE, CN_COMMAND, NULL, NULL);
      }
	}

   // Save workspace

   if (m_pWorkspaceDoc && m_pWorkspaceDoc->IsModified())
   {
      SaveWorkspace(false);
   }
}

//*****************************************************************************
void CWsfApp::OnUpdateWsfSaveAll(CCmdUI* pCmdUI)
{
	BOOL b = false;

	for (int i=m_aDocs.GetSize(); i--;) 
	{
		if (m_pWorkspaceDoc != m_aDocs[i]->pDoc)
		{
			b |= m_aDocs[i]->pDoc->IsModified();
			if (b) break;
		}
	}

	pCmdUI->Enable(b);
}


/*****************************************************************************
*
* GLOBAL FUNCTIONS
*
*****************************************************************************/

//*****************************************************************************
void WsfSetReadOnly(HWND hWnd, BOOL bReadOnly /*= true*/)
{
	if (!SendMessage(hWnd, EM_SETREADONLY, true, 0))
		EnableWindow(hWnd, false);
}

//*****************************************************************************
static BOOL CALLBACK __SetDialogReadOnlyCallback(HWND hChild, LPARAM bDirectChild)
{
	int n = GetDlgCtrlID(hChild);
	if (n == IDCANCEL || n == IDOK) return true;
	
	if (GetWindowLong(hChild, GWL_STYLE) & WS_TABSTOP)
	{
		if (!SendMessage(hChild, EM_SETREADONLY, true, 0))
			EnableWindow(hChild, false);
	}

	return true;
}

//*****************************************************************************
void WsfSetDialogReadOnly(CDialog *pDlg)
{
	EnumChildWindows(*pDlg, __SetDialogReadOnlyCallback, true);
}

//*****************************************************************************
void WsfPrepareForEdit(CWnd *pWnd)
{
	ASSERT_VALID(pWnd);

	((CEdit*)pWnd)->SetSel(0,-1);
	pWnd->SetFocus();
}

//*****************************************************************************
void WsfDisplayPopupHelp(const char *pcszTxtFile, HWND hWnd)
{
	ASSERT(false);

/*	CString strChm;
	DWORD dwId;
	RECT r;
	
	ASSERT(hWnd);

	dwId = ::GetDlgCtrlID(hWnd);
	if (!dwId) return;

	GetWindowRect(hWnd, &r);

	strChm = WsfGetApp()->m_sAppPath + WSF_HELP_FILE_NAME;
	strChm += "::/";
	strChm += pcszTxtFile;

	HH_POPUP s;
	s.cbStruct = sizeof(s);
	s.hinst = NULL;
	s.idString = dwId;
	s.pt.x = r.left;
	s.pt.y = r.bottom;
	s.clrForeground = -1;
	s.clrBackground = -1;
	s.rcMargins.top = -1;
	s.rcMargins.bottom = -1;
	s.rcMargins.left = -1;
	s.rcMargins.right = -1;
	s.pszFont = NULL;
	
	HtmlHelp(hWnd, strChm, HH_DISPLAY_TEXT_POPUP, (DWORD)&s);*/
}

//*****************************************************************************
void WsfSetBoldFont(CWnd* pWnd)
{
  static CFont g_DlgBoldFont;
	ASSERT_VALID(pWnd);

	// Create bold font if it doesn't exist

  if (!g_DlgBoldFont.m_hObject)
  {
    LOGFONT lf;
    CFont* pFont = pWnd->GetFont();
    ASSERT_VALID(pFont);
    if (!pFont) return;
    pFont->GetLogFont(&lf);
    lf.lfWeight = 1000;		
    g_DlgBoldFont.CreateFontIndirect(&lf);
  }
  
	// Set font

	pWnd->SetFont(&g_DlgBoldFont);
}

//*****************************************************************************
void WsfSetBoldFont(CWnd* pParentWnd, int nChildId)
{
	ASSERT_VALID(pParentWnd);
	CWnd* pWnd = pParentWnd->GetDlgItem(nChildId);
	ASSERT_VALID(pWnd);
   WsfSetBoldFont(pWnd);
}

//*****************************************************************************
HBITMAP WsfLoadBitmap(int nBitmapID, BOOL bMap3DColors)
{
	HINSTANCE h = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapID), RT_BITMAP);
	if (!h) return NULL;

	return (HBITMAP) LoadImage(h, MAKEINTRESOURCE(nBitmapID), IMAGE_BITMAP, 
		0,0, LR_LOADMAP3DCOLORS | LR_SHARED);
}

//*****************************************************************************
void WsfReportError2(CException* pErr, 
										 const char* pcszText, 
										 BOOL bDeleteErr,
										 int nLine,
										 const char* pcszFile)
{
	CString s;
	char temp[1024];

	if (!pErr->GetErrorMessage(temp, 1024)) strcpy(temp, "N/A");

//	if (nLine == -1 || !pcszFile)
//	{
		s.Format(IDS_ERROR_FORMAT, pcszText, temp);
//	}
//	else
//	{
		//s.Format(IDS_ERROR_FORMAT_DEBUG, pcszText, temp, pcszFile, nLine);
	//}

	AfxMessageBox(s);
	if (bDeleteErr) pErr->Delete();
}
										 
//*****************************************************************************
void WsfReportError2(CException* pErr, 
										 int nID, 
										 BOOL bDeleteErr,
										 int nLine,
										 const char* pcszFile)
{
	CString s;
	VERIFY(s.LoadString(nID));
	WsfReportError2(pErr, s, bDeleteErr, nLine, pcszFile);
}

/*****************************************************************************/
DWORD WsfGetDllVersion(const char* pcszPath)
{
   DWORD dw;
   UINT ui;
   int n;
   void* p;
   VS_FIXEDFILEINFO* pffi;
   
   ASSERT(pcszPath);
   
   n = GetFileVersionInfoSize((char*)pcszPath, &dw);
   ASSERT(n);
   if (!n) return 0;
   p = malloc(n);
   VERIFY(GetFileVersionInfo((char*)pcszPath, 0, n, p));
   VERIFY(VerQueryValue(p, "\\", (void**)&pffi, &ui));
   
   dw = MAKEDWORD(
      (BYTE) (pffi->dwProductVersionMS >> 16),
      (BYTE) (pffi->dwProductVersionMS & 0xFFFF),
      (BYTE) (pffi->dwProductVersionLS >> 16),
      (BYTE) (pffi->dwProductVersionLS & 0xFFFF)
      );
   
   free(p);
   return dw;
}

/*****************************************************************************/
CString WsfFormatDllVersion(DWORD dwVersion)
{
	CString s, s2;
	DWORD dw;
	
	s.Format("%d.%d", dwVersion>>24, (dwVersion>>16)&0xFF);

	dw = (dwVersion>>8)&0xFF;
	if (dw)  { s2.Format(".%d",dw); s += s2; }

	dw = (dwVersion)&0xFF;
	if (dw)
	{
		if (dw > 200)  
			s2.Format(" ALPHA %d",dw-200); 
      else
		if (dw > 100)  
			s2.Format(" BETA %d",dw-100); 
		else
			s2.Format(".%d",dw); 
		s += s2;
	}

	return s;
}

/*****************************************************************************/
void WsfFillComboBox(CComboBox* c, int nStringId)
{
	ASSERT_VALID(c);
	CString s;
	VERIFY(s.LoadString(nStringId));

	for (int n; (n=s.Find('\n')) != -1; s=s.Mid(n+1)) c->AddString(s.Left(n));
	if (!s.IsEmpty()) c->AddString(s);
}

/*****************************************************************************/
CString WsfBrowseForFolder(const char* pcszInfo)
{
	BROWSEINFO bi;
	LPMALLOC pMalloc;
	ITEMIDLIST *pidl;
	char temp[MAX_PATH+1];

	// Get folder

	bi.hwndOwner = *WsfGetFrame();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = temp;
	bi.lpszTitle = pcszInfo ? pcszInfo : "Vyberte složku:";
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_USENEWUI;
	bi.lpfn = NULL;

	pidl = SHBrowseForFolder(&bi);
	if (!pidl) return "";

	// Get path from the return list

	if (!SHGetPathFromIDList(pidl, temp)) return "";

	// Release the list using IMalloc interface
	
	if (SHGetMalloc(&pMalloc))
	{
		pMalloc->Free(pidl);
		pMalloc->Release();
	}

	return temp;
}

/*****************************************************************************/
void __declspec(noreturn) WsfThrowOleDispatchException(CException* e) throw (COleDispatchException*)
{
   e->GetErrorMessage(temp, 1024);
   e->Delete();
   AfxThrowOleDispatchException(0, temp);
}

/*****************************************************************************/
void WsfSendEmail(const CString& sTo)
{
   ASSERT_VALID(WsfGetFrame());

   ShellExecute(*WsfGetFrame(), "open", "mailto:"+sTo, "", "", SW_SHOW);
}


