///////////////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfTreeItemTreeView;
class CWsfTreeItemListView;
class CWsfPropertySheet;
class CWsfTISelContext;

//******************************************************************************/
class WSL_API CWsfTreeItem : public CCmdTarget
{
friend CWsfTreeItemTreeView;
friend CWsfTreeItemListView;

public:
	// Flags.
	// Item user can freely use the USER flags.
	// lo-word = flags, hi-word=user flags
	enum EFlags		
   { 
      EXPANDED=1, 
		ENABLE_DELETE=2, 
		ENABLE_OPEN=4, 
		BOLD=8,
		SERIALIZE_UI=16, // serializes icons and context menu IDs
		DONT_SAVE_CHILDERN=32,
		DONT_SAVE=64,
		
		USER1=(1<<16), USER2=(1<<17), USER3=(1<<18),
		USER4=(1<<19), USER5=(1<<20), USER6=(1<<21),
		USER7=(1<<22), USER8=(1<<23)

		// bits 24-27 reserved for serialization version
   };

private:
	// DoForWholeTree callback. See DoForWholeTree().
	typedef void WSF_TI_CALLBACK(CWsfTreeItem* pItem, void* pData);

	// Flags. It can be a combination of EFlags.
	DWORD					m_dwFlags;				

	// Menu & icons
	DWORD					m_dwMenu;			// hiword=sub menu idx, loword=menu id
	DWORD					m_dwIcon;			// hiword=icon idx, loword=bmp id
	DWORD					m_dwExpandedIcon;	// hiword=icon idx, loword=bmp id

	// Tree stuff
	CWsfTreeItem*	m_pChild;
	CWsfTreeItem*	m_pNextSibling;
	CWsfTreeItem*	m_pPrevSibling;
	CWsfTreeItem*	m_pParent;					

	// A handle of an item in the tree control associated
	// with this tree item. 
	HTREEITEM			m_hTreeItem;			

	// When this item is a part of a selection and it should
	// handle a command, the following variable points to 
	// a selection context (see above)
	// It's a responsibility of the tree item view to init and deinit this
	// value.
	CWsfTISelContext* m_pSelContext;

public:
	//---------------------------------------------------------------------------
	// Construction
	//---------------------------------------------------------------------------

	// Contructs a new tree item.
	CWsfTreeItem(int nContextMenuId, int nContextMenuIdx,
		int nIconId, int nIconIdx,
		int nExpandedIconId, int nExpandedIconIdx,
		DWORD	dwFlags);

	// Constructs a new tree item. 
	// Call Init() for setting the icons and menu.
	CWsfTreeItem();										
	
	// Creates a copy of the given item. Position in the tree
	// is not copied!!! It is set to NULL.
	CWsfTreeItem(const CWsfTreeItem& item);

	// Destructor removes the item from the tree and deletes
	// all childern.
	virtual ~CWsfTreeItem();

	// Sets item's icons and context menu
	void Init(int nContextMenuId, int nContextMenuIdx,
		int nIconId, int nIconIdx,
		int nExpandedIconId, int nExpandedIconIdx,
		DWORD dwFlags = 0);


	//---------------------------------------------------------------------------
	// Persistance
	//---------------------------------------------------------------------------

	// Overload this function to serialize your class data.<BR>
	// Don't forget to call the parent's serialize method 
	// at the beginning of your code!
	virtual void Serialize(CArchive &ar) throw(CException*);

	// Loads a tree from a file. The returned item is the root the loaded tree.
	static CWsfTreeItem* Load(const char *pcszFileName) throw(CException*);

	// Save tree item and its subtree to the given file.
	void Save(const char* pcszFileName) throw(CException*);

private:
	void Load(CArchive& ar) throw (CException*);
	void Save(CArchive& ar) throw (CException*);
public:

	//---------------------------------------------------------------------------
	// Flags
	//---------------------------------------------------------------------------

	// Returns tree item's flags.
	inline DWORD GetFlags() const { return m_dwFlags; }
	
	// Sets tree item's flags.<BR>
	// Some flags may affect the UI. By altering the flags UI is not updated.
	// You must call some UI update function to reflect the change.
	inline void SetFlags(DWORD dwFlags) { m_dwFlags = dwFlags; }
	
	// Adds given flags to the tree item's flags.<BR>
	// Some flags may affect the UI. By altering the flags UI is not updated.
	// You must call some UI update function to reflect the change.
	inline void AddFlags(DWORD dwFlags) { m_dwFlags |= dwFlags; }

	// Removes given flags from the tree item's flags.<BR>
	// Some flags may affect the UI. By altering the flags UI is not updated.
	// You must call some UI update function to reflect the change.
	inline void RemoveFlags(DWORD dwFlags) { m_dwFlags &= ~dwFlags; }

	//---------------------------------------------------------------------------
	// UI stuff
	//---------------------------------------------------------------------------

	// Returns index to the CWsfApp::m_pSmallIconList and 
	// to the CWsfApp::m_pNormalIconList.
	// Icon is used it tree views when the item is not expanded and
	// in list views.
	int GetIcon() const;

	// Returns index to the CWsfApp::m_pSmallIconList and 
	// to the CWsfApp::m_pNormalIconList.
	// Icon is used it tree views when the item is expanded.
	int GetExpandedIcon() const;
   int GetExpandedIconBitmapId() const { return m_dwExpandedIcon & 0xFFFF; }
   int GetExpandedIconIdx() const { return m_dwExpandedIcon >> 16; }

	// Sets the normal and expanded icons.
	// The specified bitmap must be registered in the framework.
	// The UI is not updated, you should send e.g. WSF_TCH_TITLE_ICON.
	void SetIcon(int nBmpId, int nIconIdx);
	void SetExpandedIcon(int nBmpId, int nIconIdx);

	// Sets the item's context menu.
	void SetContextMenu(int nMenuId, int nMenuIdx);

	// Returns the context menu assocciated with this tree item.
	// loword=res_id, hiword=submenu_idx
	inline DWORD GetContextMenu() const { return m_dwMenu; }

	// Returns default UI commnand.
	// Overload this function to change the default command.
	virtual int GetDefaultCommand() { return ID_WSF_PROPERTIES; }

	// Returns the item's title. Title is used in the UI.<BR>
	// Overload this PURE vitual function to provide item's title.
	virtual CString GetTitle() const = 0;

	// Returns tree path in following format: folder\folder\...\item
	// It starts to build the path from inside the tree moving to the
	// top. nStopLevel specifies how many levels from the root
	// it stops.
	CString GetPathTitle(int nStopLevel = 0) const;

	// Reorders the item among the siblings.
	void Reorder();

	// Returns ID that uniquely identifies tree item (= instance of this class)
	// Default implementation returns "this" pointer.
	virtual int GetId() const { return (int) this; }

	// Called before and after title editing
	// pEditCtrl != NULL && pcszNewTitle == NULL -> its before editing
	// pEditCtrl == NULL && pEditCtrl != NULL -> is after editing
	// RETURN VALUE: before editing: TRUE=accept editing, after editing: TRUE = new title accepted
	virtual BOOL OnRename(CEdit *pEditCtrl, const char *pcszNewTitle) { return false; }

	// It's called when the item is about to be deleted.
	// Return TRUE if the item can be deleted.
	// When the item is deleted form the CWsfTreeView or CWsfTreeItemView, dwData is 0.
	// This method is called from the CWsfApp::DeleteTreeItem() or DeleteSubTree() APIs.
	virtual BOOL OnDelete(DWORD dwData) { return true; }

	// Called once before the sub-tree of this item is about to be deleted.
	// This method is called from the CWsfApp::DeleteTreeItem() or DeleteSubTree() APIs.
	virtual BOOL OnDeleteSubTree() { return true; }

	//---------------------------------------------------------------------------
	// Properties
	//---------------------------------------------------------------------------

	// Displayes properties of this item. If bWizard is TRUE, property
	// sheet will be displayed in wizard 97 mode.
	int DisplayProperties(BOOL bWizard = false);

	// Called before properties of a row are displayed.
	// 1) Set title of the property sheet.
	// 2) Create pages on the heap and add them to the sheet. 
	//		The pages will be auto deleted. (even if you return false and 
	//		some pages has already been added).
	// Return true, if properties can be displayed.
	virtual BOOL OnInitProperties(CWsfPropertySheet& ps) { return false; }

	// Called after user dismiss the properties dialog.
	// bOk is true, if OK or DONE button was pressed. It's false when CANCEL
	// button was pressed.
	// Return TRUE if the PS dialog can be closed.
	virtual BOOL OnCloseProperties(CWsfPropertySheet& ps, BOOL bOk) { return true; }

	//---------------------------------------------------------------------------
	// Drag and drop support
	//---------------------------------------------------------------------------

	// Called when this item should decide if the given 
	// data object can be dropped. Returns all possible drop effects.
	// Drop effects: DROPEFFECT_NONE, DROPEFFECT_MOVE, DROPEFFECT_COPY, DROPEFFECT_LINK
	virtual DROPEFFECT OnAcceptDataObject(COleDataObject* pDataObject) 
		{ return DROPEFFECT_NONE; }

	// Called when data object is dropped
	virtual void OnDropDataObject(COleDataObject* pDataObject, DROPEFFECT dropEffect) 
		{ ASSERT(false); }

	//---------------------------------------------------------------------------
	// Tree structure
	//---------------------------------------------------------------------------

	// Returns a parent item of this item. If this item is a root NULL is returned.
	inline CWsfTreeItem* GetParent() const  { return m_pParent; }

	// Returns the first child item of this item. If no child exists NULL is returned.
	inline CWsfTreeItem* GetChild() const { return m_pChild; }

	// Returns the next sibling of this item. If no next sibling exists NULL is returned.
	inline CWsfTreeItem* GetNextSibling() const { return m_pNextSibling; }

	// Returns the previous sibling of this item. If this item is first in the sibling list
	// NULL is returned.
	inline CWsfTreeItem* GetPrevSibling() const { return m_pPrevSibling; }

	// Calls the callback for this item and for all subitems.
	void DoForWholeTree(WSF_TI_CALLBACK *pfnCallback, void *pData);

	// Calls the callback for all subitems.
	void DoForWholeSubTree(WSF_TI_CALLBACK *pfnCallback, void *pData);

	// Adds new child item
	void AddChild(CWsfTreeItem* pChild);

	// Removes the item from the tree. The item is not deleted.
	void RemoveFromTree();

	// Searches the item's subtree for a child item that
	// matches the given criteria.
	// If no item is found NULL is returned.
	// [in] pcszTitle The title of an item the tree is searched for.
	// [in] bWholeSubtree If it's true whole subtree is searched, if it's false
	//		only direct childern are tested.
	CWsfTreeItem* FindChildItem(const char* pcszTitle, BOOL bWholeSubtree);
	CWsfTreeItem* FindChildItem(int nId, BOOL bWholeSubtree);
	CWsfTreeItem* FindChildItem(const CRuntimeClass* pClass, BOOL bWholeSubtree);

	// Search the tree for the given item.
	CWsfTreeItem* FindItem(const char* pcszTitle);
	CWsfTreeItem* FindItem(int nId);

	// Enumerates all child items of the given item.
	void EnumChildItems(CWslArray<CWsfTreeItem*>& aItems, BOOL bWholeSubtree);

	// Returns TRUE if a given item is in the subtree of this item.
	BOOL IsInSubTree(CWsfTreeItem* pItem);

	// Returns root item of the tree this item is in.
	CWsfTreeItem* GetRoot();

	// Called when child item should be compared with another child item.
	// E.g. called by AddChild() to get the position of the
	// child being added.
	// RETURNS: <0 = item1, should be before the given item2
	//          =0 = doesn't matter
	//					>0 = item2, should be before the given item1 
	virtual int OnCompare(CWsfTreeItem* pItem1, CWsfTreeItem* pItem2) { return 1; }

  //---------------------------------------------------------------------------
	// Selection info querying and selection processing
	// NOTE: Can be used only in UI and command handlers!
	//---------------------------------------------------------------------------

	// Returns TRUE if one or no item is selected.
	BOOL IsMultiselection() const;

	// Get number of selected items
	int GetSelCount() const;

	// Info on relation of this item to the selection
	int GetSelPos() const;
	BOOL IsFirstSelected() const;
	BOOL IsLastSelected() const;

	// If there is a multiselection and this method is called in a cmd handler, 
	// calling the cmd handler for the rest of selected items will be canceled.
	void CancelCommand();

private:
	//---------------------------------------------------------------------------
	// Various internal stuff
	//---------------------------------------------------------------------------

	static BOOL PSCallback(void* pData, CWsfPropertySheet& ps, BOOL bOk);

	// Serialization
	DECLARE_SERIAL(CWsfTreeItem)

	// Message map
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CWsfTreeItem)
	afx_msg void OnWsfHelp();
	afx_msg void OnWsfProperties();
	afx_msg void OnUpdateWsfProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfHelp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfDelete(CCmdUI* pCmdUI);
	//}}AFX_MSG
};












