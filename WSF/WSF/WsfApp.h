///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfApp;
class CWsfToolBar;
class CWsfCmd;
class CWsfIconList;
class CWsfWorkspaceDoc;
class CWsfPlugin;
class CWsfTreeItemTreeView;
class CWsfTreeItemListView;
class CWsfTreeItem;

// Update view hints

#define WSF_UV_TREE_HAS_CHANGED				30000
#define WSF_UV_LIST_VIEW_TYPE_CHANGED		30001
#define WSF_UV_DB_HAS_CHANGED					30002
#define WSF_UV_ENSURE_TI_VISIBLE				30003			// hint=tree item


/*****************************************************************************
*
* MESSAGES
*
*****************************************************************************/

// Sent to the current view when message text on the status 
// line is set to AFX_IDS_MESSAGEIDLE. If there is no active view or the message 
// returns FALSE, default string (AFX_IDS_MESSAGEIDLE) is taken.
#define WSF_WM_UPDATE_MESSAGE_TEXT		(WM_USER + 1000)	

// Send to the main frame when DbHasChanged
#define WSF_WM_DB_HAS_CHANGED				(WM_USER + 1001)

//*****************************************************************************
class CWsfViewInfo
{
public:
	int		nID;
	CView*	pView;
	CString	sTitle;
	BOOL		bEnableCloseWindow;
	BOOL		bLeftPane;		// false=right pane
};

//*****************************************************************************
class CWsfDocInfo
{
public:
   int         nID;
	CDocument*	pDoc;
	CWslArray<CWsfViewInfo*>	aViews;
};

/*****************************************************************************/
class CWsfDbEvent
{
public:
   enum EEvents 
   {
      GENERAL, ROW_UPDATED, ROW_DELETED, ROW_ADDED
   };

public:
	EEvents  eEvent;
	CString	sTblName;
	int		nPrimaryKey;	
};


/*****************************************************************************
*
* CWsfApp - Main Framework Object
*
*****************************************************************************/

class WSL_API CWsfApp : public CWinApp
{
friend CWsfFrame;

public:
	// Tree changes describe the way the tree of tree-items
	// has changed.
	// NOTE: Always use the appropriate change type
	// to minimize UI redraws and consequent flickering.
	enum ETreeChanges 
   { 
		GENERAL = 0,
		ITEM_ADDED,
		CHILDERN_ADDED,
		SUBTREE_CHILDERN_ADDED,
		TITLE_ICON,										// Title or icon of the item has changed
		SUBTREE_TITLE_ICON,
		CHILDERN_DELETED,
		SUBTREE_CHILDERN_DELETED
	};

	enum ELanguages	{ CZ, EN };

   enum EWinVersions { WINOTHER, WIN9x, WIN2000, WINXP };

private:
	struct CSetup
	{
		int		nX, nY, nW, nH;
		BOOL 	bLeftPaneVisible;
		int		nSplitter;
		int		nShowType;
		int		nListViewType;
	};

private:
	CWslArray<CWsfDocInfo*>		m_aDocs;			// Holds info about the opened documents
	CWsfTreeItem*			m_pTempTreeItem;			// For temporary usage 
	HCURSOR					m_hNoCursor;
	CWsfWorkspaceDoc*	   m_pWorkspaceDoc;
	int						m_nIdCounter;
   CWslArray<int>       m_CloseDocQueue;
   CWslArray<int>       m_CloseViewQueue;
	CString				   m_sAppPath;				// Folder the .EXE file is in, trailing slash IS!
   CString              m_sTempPath;         // With trailing slash

   // Identity
   CString              m_sAppNameWithoutVersion;
   DWORD                m_dwAppVersion;
   CString              m_sCompany;

	// View switching stuff (CTRL+TAB)
	BOOL						m_bTabPressed;
	int						m_nViewCounter;

	// Icons
	CWsfIconList*		m_pSmallIconList;
	CWsfIconList*		m_pSmallIconListGray;
	
   CWsfIconList*		m_pNormalIconList;
	CWsfIconList*		m_pColumnIconList;

	// Commands
	CWslArray<CWsfCmd*>		m_aCmds;
	HACCEL						m_hViewAcceleratorTable;
	CWsfIconList*				m_pCmdIconList;

	// Control bars
	CWslArray<CControlBar*>	m_aControlBars;

public:
	CLIPFORMAT			m_cfTreeItemList;
	DWORD					m_dwWsfVersion;
	CSetup				m_Setup;						// Setup
	ELanguages			m_eLanguage;				// Lang of the resources
   EWinVersions      m_eWinVersion;

public:
	CWsfApp();
	virtual ~CWsfApp();

	//---------------------------------------------------------------------------
	// General stuff
	//---------------------------------------------------------------------------

	// Returns CWsfFrame pointer
	inline CWsfFrame* GetFrame() { return (CWsfFrame*)m_pMainWnd; }

	// Returns an unique ID within this instance of WSF
	// starting at 1.
	inline int GetID() { return m_nIdCounter++; }

	// Updates the main frame (application) title
	// Should be called when document modification state is changed.
	// NOTE: Doesn't guilt for WorkspaceDoc, because SetWorkspaceModified()
	// method does it automatically.
	inline void UpdateAppTitle();

   //------------------------------------------------------------------------
   // STORAGE
   // NOTE for paths: THERE IS ALWAYS THE BACKSLASH AT THE END!
   //------------------------------------------------------------------------

   // Returns a registry key in the application's registry profile (under HKEY_CURRENT_USER).
   // If it doesn't exist, it's created.
   CWslPtr<CWslRegKey> GetAppRegistryKey(const char* pcszKey) throw (CException*);

   // Returns a registry key in the application's registry profile (under HKEY_LOCAL_MACHINE).
   // - bCreate=TRUE: if it doesn't exist, it's created.
   //   bCreate=FALSE: if it doesn't exist, closed CWslRegKey is returned.
   // - NOTE: Only admin can create keys under HKEY_LOCAL_MACHINE.
   CWslPtr<CWslRegKey> GetAppLocalMachineRegistryKey(const char* pcszKey, bool bCreate) throw (CException*);

   // Returns an application specific temporary folder
   // Contents of the folder is delete upon application exit
   // The folder is created when the function is called for the first time
   // The path has the trainling slash.
   CString GetTempPath() throw (CException*);

   // Returns a unique temporary file with the given extension
   // The file is automatically deleted when the application quits
   CString GetTempPathName(const char* pcszExtension) throw (CException*);

   // .EXE path
   CString GetAppPath();
   
   // Non-roaming data
   CString GetLocalAppDataPath() throw (CException*);
   
   // Roaming data
   CString GetAppDataPath() throw (CException*);

	//---------------------------------------------------------------------------
	// IDENTITY
	//---------------------------------------------------------------------------

   // Sets the application name, version, company name
   // Call in InitInstance()
	// NOTE: MUST BE CALLED BEFORE THE MAIN FRAME IS CREATED!!!
   void SetAppIdentity(int nAppNameStringId, 
      HMODULE hModuleWithVersionInfo,
      const CString& sCompanyName);

   // Returns the applicatin's name and version.
   // Can be called after SetAppIdentity() has been called.
   const char* GetAppNameVersion() const { return m_pszAppName; }

   // Returns the application's name.
   // Can be called after SetAppIdentity() has been called.
   const CString& GetAppName() const { return m_sAppNameWithoutVersion; }

   DWORD GetAppVersion() const { return m_dwAppVersion; }
   const CString& GetCompany() const { return m_sCompany; }

	//---------------------------------------------------------------------------
	// Plug-ins
	//---------------------------------------------------------------------------

   static const CWsfPluginsArray& GetPlugins();

	// Registers a plug-in.
	static void RegisterPlugin(CWsfPlugin* pPlugin);

   // Removes a plug-in
   static void RemovePlugin(CWsfPlugin* pPlugin);

   // Returns a plugin with the given name.
   static CWsfPlugin* GetPlugin(const char* pcszName);

	//---------------------------------------------------------------------------
	// Toolbars
	//---------------------------------------------------------------------------

	// Adds new control bar to the framework. 
	CControlBar* AddControlBar(CControlBar* pControlBar, const char* pcszName, BOOL bVisible);

	// Adds new toolbar to the framework. The toolbar must be built manualy.
	CWsfToolBar* AddToolBar(const char* pcszName, int nID, BOOL bVisible = true);

	// Adds new toolbar to the framework. The toolbar is loaded from the resources.
	CWsfToolBar* AddToolBar(const char* pcszName, int nID, int nBmpId, BOOL bVisible);

	// Shows/hides a control bar
	void ShowControlBar(int nId, BOOL bShow);

   //---------------------------------------------------------------------------
	// Commands
	//---------------------------------------------------------------------------

	// Posts a command. The command is routed as usualy.
	void PostCommand(int nCmdId);

	// Sends the given command to the given view.
	// Returns TRUE if the command was handled.
	BOOL SendCommand(CWsfViewInfo* pvi, int nCmdId);

	// Adds commands to the framework
	void AddCmdSimple(const char* pcszTextId, int nId, int nImgIdx=-1, int nBmpId=-1, BOOL bText = false, BOOL bImage = true);
	void AddCmdComboBox(const char* pcszTextId, int nId, CRuntimeClass* pComboClass, int nWidth, BOOL bText = false);
	void AddCmdDropDown(const char* pcszTextId, int nId, int nDefCmdId, HMENU hMenu, BOOL bText = false);
	void AddCmdDropDown(const char* pcszTextId, int nId, CRuntimeClass* pWndClass, int nImgIdx=-1, int nBmpId=-1, BOOL bText = false, BOOL bImage = true);
	void AddCmdDropDownOnly(const char* pcszTextId, int nId, int nImgIdx, int nBmpId, HMENU hMenu, BOOL bText = false);

   // Creates a copy of the given command and assigns it a new ID
   void CloneCmd(int nId, const char* pcszNewTextId, int nNewId);

   // Deletes a command
   void DeleteCmd(int nId);

	// Returns pointer to the object representing the command with given id.
	const CWsfCmd* GetCmd(int nId);
	const CWsfCmd* GetCmd(const char* pcszTextId);

private:
	// Checks if the bitmap is loaded and then
	// adjusts the image index. Returns new image index.
	int InitCmdImage(int nImgIdx, int nBmpId);
public:

	// OBSOLETE - TO BE REMOVED - USE CView::PreTranslateMessage()
	// Set view's accelerator table.
	// This table is checked before the main-frame's one.
	// If nID is 0, the table is disabled.
	// It should be called in the CView::OnActivateView()
	// Returns TRUE if the table was loaded.
	BOOL SetViewAcceleratorTable(int nID);

	// Registers standard MFC & WSF commands.
	void RegisterStandardCommands();

   //---------------------------------------------------------------------------
	// Menu
	//---------------------------------------------------------------------------

	// Builds a menu from the XML definition file.
	HMENU LoadMenu(const char* pcszXMLFile) throw (CException*);

   // - Adds icons to all menu items.
   // - If the menu text equals to the command identificator,
   //   it is replaces by the command text.
   // - Automatically called for the main frame's menu.
   void ProcessMenu(HMENU hMenu);

	// Builds a pop-up menu from all the given menus.
	// Only common commands are present in the resulting menu.
	// Returns NULL if the array is empty.
	HMENU BuildPopupMenu(CWslArray<int>& aMenuIDs, int nDefCmd);

	// Appends the given menu with the given command
	// Separator: nCmd = -1 
	void AppendPopupMenu(HMENU hMenu, const CWsfCmd* pCmd);
	void AppendPopupMenu(HMENU hMenu, int nCmdId);

   //---------------------------------------------------------------------------
	// Small, normal & column icons for lists and tables
	//---------------------------------------------------------------------------

	// Adds small (16x16) and normal (32x32) icons for tree items and table items.
	// Background must be the color RGB(0,255,255).
	// Both bitmaps must contain the same number of icons.
	// The icon is later identified by the nSmallBmpId bitmap ID and it's idnex 
	// in this bitmap.
	void AddIcons(int nSmallBmpId, int nNormalBmpId);

	// Adds icons used in the headers of tables.
	// Background must be the color RGB(0,255,255).
	void AddColumnIcons(int nBmpId);

	// Returns an icon index in the global image list.
	// The index for small and normal icon is the same.
	int GetIconIdx(int nSmallBmpId, int nIconIdx);

	// Returns index to the global column icons image list.
	int GetColumnIconIdx(int nBmpId, int nIconIdx);

	// Returns image list for the given type of icons.
	inline CImageList* GetSmallIconList() { return (CImageList*) m_pSmallIconList; }
	inline CImageList* GetGraySmallIconList() { return (CImageList*) m_pSmallIconListGray; }
	inline CImageList* GetNormalIconList() { return (CImageList*) m_pNormalIconList; }
	inline CImageList* GetColumnIconList() { return (CImageList*) m_pColumnIconList; }

  //---------------------------------------------------------------------------
	// Documents
	//---------------------------------------------------------------------------

	// Returns number of opened docs
	inline int GetDocCount() const { return m_aDocs.GetSize(); }

	// Returns info about all opened docs
	inline CWslArray<CWsfDocInfo*>& GetDocInfo() { return m_aDocs; }

	// Returns info about the given document.
	CWsfDocInfo* GetDocInfo(CDocument* pDoc);
	CWsfDocInfo* GetDocInfo(const CString& sDocTitle);
	CWsfDocInfo* GetDocInfo(int nID);
	CWsfDocInfo* GetDocInfoByPathName(const CString& sPathName);

	// Returns document stored in the list after the
	// given document.
	CWsfDocInfo* GetNextDocInfo(CDocument* pDoc);

	// Opens given doc.
	void OpenDoc(CDocument* pDoc);
   
   // Opens a HTML file - an easy way how to do it!
   // - It creates a document/view with the path_name set to the sFileName value. 
   // - If such a document already exists, it's view is activated and 
   //   no new document is created (bAlwaysOpenNewView = false).
   // - Returns the view's id or zero if something went wrong.
   // - NOTE: The path must be fully qualified!!!
   int OpenHtmlFile(const CString& sFileName, 
      const CString& sViewName, 
      int nIconBitmapId, 
      int nIconIdx,
      BOOL bDeleteFileOnClose,
      BOOL bAlwaysOpenNewView = false);

	// Closes an already opened document and all its views.
   // If the bDelayed is TRUE, the doc is closed asynchronously
   // as soon as CWinApp::OnIdle() is called.
	BOOL CloseDoc(CDocument* pDoc, BOOL bDelayed = false);
	BOOL CloseDoc(const CString& sDocTitle, BOOL bDelayed = false);
	BOOL CloseDoc(CWsfDocInfo* pdi, BOOL bDelayed = false);

	// Called when all documents and workspace should be closed.
	// When called all documents must have the modified flag clear.
	// All docs & views will be destroyed & deleted.
	// When the main frame is about to be destroyed (= app is quiting),
	// bEndSession is true.
	// NOTE: When CWsfFrame is about to be closed, it calls this
	// method with bEndSession=true.
	// NOTE: The workspace doc is closed as the last doc.
	virtual void CloseAllDocuments(BOOL bEndSession);

	// Called when all documents should be closed.
	// When called all documents must have the modified flag clear.
	// All docs & views will be destroyed & deleted.
	// When bRightPaneOnly is TRUE, only docs that doesn't have
	// oprned view in the left pane will be closed.
	// NOTE: It doesn't close the workspace!
	void CloseAllOrdinaryDocs(BOOL bRightPaneOnly);

	// Saves all modified (ordinary) documents, NOT THE WORKSPACE.
	BOOL SaveAllModifiedOrdinaryDocs();

	// Called when all docs and the workspace should be saved.
	// NOTE: When CWsfFrame is about to be closed, it calls this
	// method.
	// NOTE: The workspace is save as the last doc.
	virtual BOOL SaveAllModified();	

	// All views of all docs.
	void UpdateAllDocs(CView* pSender, int nHint, void* pData);

	// Sends to all documents WSF_UV_DB_HAS_CHANGED OnUpdate() notification.
   void SendDbEvent(CWsfDbEvent::EEvents eEvent, 
      const char* pcszTblName, 
      int nPrimaryKey);

	// Brings the first view of the given document to the front. 
	// Returns false if the document doesn't exist.
	BOOL ActivateDoc(CDocument* pDoc);
	BOOL ActivateDoc(const CString& sDocTitle);

   //---------------------------------------------------------------------------
	// Views
	//---------------------------------------------------------------------------

	// Opens a view.
	// The pDoc document must have been opened using OpenDoc() method.
   // If nPos is -1, the view is put on the end.
	// Returns the view's ID or ZERO when something went wrong.
	int OpenView(CView* pView, 
		CDocument* pDoc, 
		const char* pcszTitle,
		BOOL bLeftPane,
		BOOL bEnableCloseWindow,
		int nPos,
		BOOL bActivate,
		int nIconBitmapId, 
      int nIconIdx);

   // Replaces the given view with a new one. The tab is not closed.
   // If the old view's doc is modified, doc's SaveModified() method is called.
   // If there is no such a view or pCurrentView is NULL, a new tab is opened.
	// Returns a view ID or ZERO when something went wrong.
   int ReplaceView(CView* pCurrentView, 
      CView* pNewView,
      CDocument* pDoc,
      const char* pcszTitle,
		BOOL bLeftPane,
		BOOL bEnableCloseWindow,
		BOOL bActivate,
      int nIconBitmapId, 
      int nIconIdx);

   // Replaces the active view in the given pane.
   // If there is no view, it opens a new tab.
	// Returns a view ID or ZERO when something went wrong.
   int ReplaceActiveView(CView* pNewView,
      CDocument* pDoc,
      const char* pcszTitle,
      BOOL bLeftPane,
		BOOL bEnableCloseWindow,
		BOOL bActivate,
      int nIconBitmapId, 
      int nIconIdx);

   // Closes the given view. 
	// If it's the last view of the view's document CloseDoc() is called.
	// If the view was closed, TRUE is returned.
   // If bDelay TRUE the view is closed at the next OnIdle() processing.
	BOOL CloseView(CView* pView, BOOL bDelayed);
	BOOL CloseView(const char* pcszViewName, BOOL bDelayed);
	BOOL CloseView(int nID, BOOL bDelayed);
protected:
	BOOL CloseView(CWsfDocInfo* pdi, CWsfViewInfo* pvi, int nViewIdx, BOOL bDelayed);
public:

	// Closes all views in the given pane
	void CloseAllPaneViews(BOOL bLeftPane);

	// Activates the given view.
	BOOL ActivateView(CView* pView);
	void ActivateView(CWsfViewInfo* pViewInfo);
	BOOL ActivateView(const char* pcszViewName);
	BOOL ActivateView(int nViewID);

	// Returns info about the given view. 
	// Returns NULL if the view is not found.
	CWsfViewInfo* GetViewInfo(CView* pView);
	CWsfViewInfo* GetViewInfo(int nViewID);
	CWsfViewInfo* GetViewInfo(const char* pcszViewName);

	// Sets the view's title and icon and updates UI to reflect the change.
	void SetViewTitleIcon(CWsfViewInfo* pvi, const char* pcszTitle, int nIconIdx = -1);

  //---------------------------------------------------------------------------
	// Panes
	//---------------------------------------------------------------------------

	// Returns the active pane's view
	CView* GetActivePaneView(BOOL bLeftPane);

	// Called when in a pane a new window is seleceted 
	// NOTE: pOld or pNew can be NULL.
	virtual void OnTabWindowSelChange(BOOL bLeftPane, CView* pOld, CView* pNew) { /*dummy*/ };

   //---------------------------------------------------------------------------
	// The workspace 
	//---------------------------------------------------------------------------

	// Sets the name of the workspace
	void SetWorkspaceName(CString& sName);
	void SetWorkspaceName(const char* pcszName);

	// Returns the name of the workspace
	inline const CString& GetWorkspaceName() const { ASSERT(m_pWorkspaceDoc); return m_pWorkspaceDoc->GetTitle(); }

	// Creates new workspace. Calls pDoc->OnNewDocument.
	BOOL NewWorkspace(CWsfWorkspaceDoc* pDoc);

	// Opens new workspace. Calls pDoc->OnOpenDocument(pcszPath)
	// Sets the doument's path to pcszPath. It's upto the document
	// to set the workspace title.
	BOOL OpenWorkspace(CWsfWorkspaceDoc* pDoc, const char* pcszPath);

	// Saves workspace. Calls OnSaveDocument().
	BOOL SaveWorkspace(BOOL bSaveAs);

	// Closes the workspace. Calls OnCloseDocument().
	// NOTE: The workspace doc must have the modified flag set to false!
	// Call SaveModifiedWorkspace() or SaveAllModified() before this function.
	void CloseWorkspace();

	// Prompts for saving the workspace if it's modified.
	BOOL SaveModifiedWorkspace();

	// Returns workspace document
	inline CWsfWorkspaceDoc* GetWorkspaceDoc() const { return m_pWorkspaceDoc; }
	
	// Returns TRUE if the project is modified
	inline BOOL IsWorkspaceModified() const { ASSERT(m_pWorkspaceDoc); return m_pWorkspaceDoc->IsModified(); }

	// Sets modified flag
	void SetWorkspaceModified(BOOL b = true);
	
	// Return true if the workspace is opened.
	// That means the workdpace document is set.
	inline BOOL IsWorkspaceOpened() const { return m_pWorkspaceDoc != NULL; }

  //---------------------------------------------------------------------------
	// The tree views
	//---------------------------------------------------------------------------
	
	// Opens new tree view at the specified position.
	// If nPos is -1, the view is added at the end.
	// Tree view is a view in the workspace pane.
	// Returns the view ID or -1 if something went wrong.
	int OpenTreeView(CWsfTreeItem* pRoot, 
		CDocument* pDoc,
		const char* pcszTitle,
		int nPos,
		BOOL bActivate,
		int nIconBitmapId, 
      int nIconIdx);

	// Removes a tree view with the given tree item as its root.
	void CloseTreeView(CWsfTreeItem* pRoot);

	//---------------------------------------------------------------------------
	// Tree items & subtrees
	//---------------------------------------------------------------------------

	// Deletes the tree item and all its subitems.
	// Returns FALSE when some item could not be deleted.
	// bUpdateViews = true, all views will be updated via 
	// OnUpdate method.
	// dwData will be passed to the CWsfTreeItem::OnDelete function
	BOOL DeleteTreeItem(CWsfTreeItem* pItem, BOOL bUpdateViews, DWORD dwData);

	// Deletes all subitems of the given item.
	// Returns FALSE when some item could not be deleted.
	// bUpdateViews = true, all views will be updated via 
	// OnUpdate method.
	// dwData will be passed to the CWsfTreeItem::OnDelete function
	BOOL DeleteSubTree(CWsfTreeItem* pItem, BOOL bUpdateViews, DWORD dwData);

	// Tells the framework, that the pSubTree has changes.
	// Function updates the tree view & all documents.
	void TreeHasChanged(CWsfTreeItem* pSubTree, ETreeChanges eChange);

	// Ensures that the given tree item is visible within
	// it's tree view, but it doesn't bring the tree view
	// to the front. (TODO: maybe it should???)
	void EnsureTreeItemVisible(CWsfTreeItem* pItem);

	// Starts the drag & drop operation with given items.
	void DoTreeItemsDragDrop(CWslArray<CWsfTreeItem*>& aItems);

	//---------------------------------------------------------------------------------
	// Helper functions
	//---------------------------------------------------------------------------------

protected:
	BOOL __DeleteTreeItem(CWsfTreeItem* pItem, BOOL bDeleteRoot, DWORD dwData);
	BOOL __DeleteTreeItem(CWsfTreeItem* pItem, BOOL bUpdateViews, BOOL bDeleteRoot, DWORD dwData);
	void OnContextHelp();	// ID_CONTEXT_HELP
	void OnHelp();				// ID_HELP
	void ParseMenu(class CWslXMLElement* pMenuElement, HMENU hMenu);
	void LoadWsfSetup();

	//---------------------------------------------------------------------------------
	// Various Message handlers
	//---------------------------------------------------------------------------------

	afx_msg void OnWsfSaveAll();
	afx_msg void OnUpdateWsfSaveAll(CCmdUI* pCmdUI);

public:
	//---------------------------------------------------------------------------------
	// ClassWizard generated virtual function overrides
	//---------------------------------------------------------------------------------

	//{{AFX_VIRTUAL(CWsfApp)
	public:
	virtual int ExitInstance();
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

protected:
	//---------------------------------------------------------------------------------
	// Generated message map functions
	//---------------------------------------------------------------------------------

	//{{AFX_MSG(CWsfApp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//*****************************************************************************
class CWsfTreeChangeInfo
{
public:
   CWsfApp::ETreeChanges eChange;
	CWsfTreeItem*	       pItem;
};


/*****************************************************************************
*
* Various help functions
*
*****************************************************************************/

#ifndef WSF_DLL_BUILD

class CApp;


inline CApp* WsfGetCApp() { return (CApp*)AfxGetApp(); }

class CWorkspace;
inline CWorkspace* WsfGetCWorkspace() { 
	return (CWorkspace*) ((CWsfApp*)AfxGetApp())->GetWorkspaceDoc(); 
}

#endif

inline CWsfApp* WsfGetApp() { return (CWsfApp*)AfxGetApp(); }
inline CWsfWorkspaceDoc* WsfGetWorkspace() { return ((CWsfApp*)AfxGetApp())->GetWorkspaceDoc(); }
inline CWsfFrame* WsfGetFrame() { return WsfGetApp()->GetFrame(); }
inline CString WsfGetAppPath() { return WsfGetApp()->GetAppPath(); }

WSL_API void WsfSetReadOnly(HWND hWnd, BOOL bReadOnly = true);
WSL_API void WsfSetDialogReadOnly(CDialog *pDlg);
WSL_API	void WsfPrepareForEdit(CWnd *pWnd);
WSL_API void WsfDisplayPopupHelp(const char* pcszTxtFile, HWND hWnd);
WSL_API void WsfSetBoldFont(CWnd* pParentWnd, int nChildId);
WSL_API void WsfSetBoldFont(CWnd* pWnd);
WSL_API HBITMAP WsfLoadBitmap(int nBitmapID, BOOL bMap3DColors);
WSL_API CString WsfBrowseForFolder(const char* pcszInfo);

// Returns the PRODUCT VERSION of the given DLL: x.x.x.x
// If something fails 0 is returned.
// NOTE: the parts of the version are truncated to BYTE size
// in order to fit in the DWORD.
WSL_API DWORD WsfGetDllVersion(const char* pcszPath);

// Formats the DLL version to the string as follows: x.xx.x
WSL_API CString WsfFormatDllVersion(DWORD dwVersion);

// Fills the given combo-box with values from the given string.
// Format: value1\nvalue2\nvalue3
WSL_API void WsfFillComboBox(CComboBox* c, int nStringId);

// Throws an OLE disptch exception with content from the given C++ exception.
// The C++ exception is deleted.
void __declspec(noreturn) WSL_API WsfThrowOleDispatchException(CException* e) throw (COleDispatchException*);

// Displays an email client with prefilled "TO" field
WSL_API void WsfSendEmail(const CString& sTo);

//*****************************************************************************
// Error outputing 

//#ifdef _DEBUG
#define WsfReportError(p1,p2,p3) WsfReportError2(p1,p2,p3,__LINE__,__FILE__)
//#else
//#define	WsfReportError(p1,p2,p3) WsfReportError2(p1,p2,p3,-1,NULL)
//#endif

WSL_API void WsfReportError2(CException* pErr, 
                             const char* pcszText, 
                             BOOL bDeleteErr = false,
                             int nLine = -1,
                             const char* pcszFile = NULL);

WSL_API void WsfReportError2(CException* pErr, 
                             int nID, 
                             BOOL bDeleteErr = false,
                             int nLine = -1,
                             const char* pcszFile = NULL);











