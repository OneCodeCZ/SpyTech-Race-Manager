///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// This class provides a higher level interface to the basic table control

/*
Initialization
~~~~~~~~~~~~~~
1. SetContextMenu()
2. SetDefaultCmd()
3. RemoveAllColumns() -- can be skipped if inited for the first time
4. AddCol(), AddCol(), ...
5. LoadLayout() or ActivateDefaultColumns();
*/

#pragma once
#include "WsfTableCtrl.h"

/*****************************************************************************/
class WSL_API CWsfTableCtrl2 : public CWsfTableCtrl
{
public:
   /*****************************************************************************/
   class WSL_API IEventSink
   {
   public:
      virtual void OnSelChanged(CWsfTableCtrl2& tbl) { };
      virtual void OnClick(CWsfTableCtrl2& tbl) { };
      virtual void OnDoubleClick(CWsfTableCtrl2& tbl) { };
   };

   /*****************************************************************************/
	class WSL_API CRowData : public CObject
	{
	public:
		int nRowId;
   public:
      CRowData() { nRowId = 0; }
      CRowData(int nRowId__) { nRowId = nRowId__; }
	};

   /*****************************************************************************/
	// This class collects and performs updates of the table.
	// The update requests are collected during the command handling 
	// and when it's finished, they are all performed.
	// It's importing to do it this way, because the table
	// cannot be changed during handling a multiselection command.
	class WSL_API CUpdateManager
	{	
   public:
	   enum EActions
	   {
		   REBUILD		= 1,
		   ADD_ROW		= 2,
		   UPDATE_ROW	= 3,		// if the row is not in the table, it's not updated
		   DELETE_ROW	= 4
	   };
      
   protected:
	   class CUpdateRequest
	   {
      public:
		   EActions	eAction;
		   int		nPrimaryKey;
	   };

	protected:
		CWslObArray<CUpdateRequest> m_aUpdateRequests;
		BOOL	   m_bRebuild;
		int		m_nSetSelPrimaryKey;
	
	public:
      CUpdateManager();
		
      // Update actions
      void AddAction(EActions eAction, int nKey);
      void AddAction(const CWsfDbEvent& e);
		void AddSelectSingleRow(int nKey);
      void Refresh();

      // Performs all the specified updates
		void Update(CWsfTableCtrl2& tbl);

   protected:
		void Clear();
	};

   /*****************************************************************************/
   class WSL_API CCmdContext
   {
   protected:
      int             m_nRowIdx;
      CRowData*       m_pRowData;
      int             m_nRowCounter;
      BOOL            m_bCanceled;
      CUpdateManager& m_UpdateManager;
      CWsfTableCtrl2& m_Tbl;
      POSITION        m_Pos;

   public:
      CCmdContext(CWsfTableCtrl2& Tbl, CUpdateManager& UM);

      // Row position
      BOOL IsLastRow() const { return m_Pos == NULL; }
      BOOL IsFirstRow() const { return m_nRowCounter == 1; }

      // Resets the context
      void Reset();

      // Moves to the next selected row
      BOOL MoveToNextRow();

	   // Stops processing the command. If there are any items
	   // that haven't been processed yet, they are skipped.
	   // NOTE: If you implement "global commands", e.i. commands
	   //   that doesn't act on items, but generally on the table,
	   //   call this method to stop processing commands after the first item.
	   //   If you didn't call it, the "global command" would be executed
	   //   for each selected row!
	   void CancelCommand() { m_bCanceled = true; }

      BOOL IsCanceled() const { return m_bCanceled; }

      // Row info
      int GetRowIdx() const { return m_nRowIdx; }
      int GetRowId() const { return m_pRowData->nRowId; }
      CRowData* GetRowData() const { return m_pRowData; }

      // Deletes the current row after the command has been fully processed
      void DeleteRow();

      // Updates the current row after the command has been fully processed
      void UpdateRow();
   };

friend class CUpdateManager;

private:
	BOOL						m_bInOnCmdMsg;			
   IEventSink*          m_pEventSink;
		
	DWORD						m_dwMenu;			// Context menu of a row; hiword=sub menu idx, loword=menu id
	int						m_nDefCmd;		   // Default command of a row

	// Queue of changes issued during handling of a command
	CUpdateManager	      m_UpdateManager;

   CCmdContext          m_CmdContext;
   int                  m_nPropertiesRowIdx;

public:
	CWsfTableCtrl2();
	virtual ~CWsfTableCtrl2();
   DECLARE_DYNCREATE(CWsfTableCtrl2)

   //---------------------------------------------------------------------------
	// GENERAL
	//---------------------------------------------------------------------------

	// Event sink
   void SetEventSink(CWsfTableCtrl2::IEventSink* p);
   inline CWsfTableCtrl2::IEventSink* GetEventSink() { return m_pEventSink; }

   // Command issued on double-click. Set 0 for none.
   void SetDefaultCmd(int nId);

   // Row's context menu. Set 0,0 for none.
   void SetContextMenu(int nMenuId, int nSubMenuIdx);

	// Rebuilds the table
	void Refresh();

	// Saves the layout of the table control
	void SaveLayout();

   // Updates the table according the given db-changed event 
   void ProcessDbEvent(CWsfDbEvent& e);

   // Creates an HTML representation of the table
   CString GenerateHtml();

   // Returns info on a row on which the command is being executed.
   // NOTE: Call only from a command handler.
   CCmdContext& GetCmdContext() { ASSERT(m_bInOnCmdMsg); return m_CmdContext; }

   // Creates a CSV representation of the table
   CString GenerateCsv();

   //---------------------------------------------------------------------------
	// SELECTION
	//---------------------------------------------------------------------------

	// Selects the given row
	void SelectSingleRowById(int nRowId);

   //---------------------------------------------------------------------------
	// ROW
	//---------------------------------------------------------------------------

   // Get ID of a row
   int GetRowId(int nRowIdx) const;

   // Displays row's properties.
   // If nRowId is 0, the wizard is displayed.
	int DisplayRowProperties(int nRowId);

   // Updates the row by calling the OnFillTable() method with
   // this row specified. 
   // NOTE: If the row is not updated, it's deleted.
   void RefreshRow(int nRowId);

   //---------------------------------------------------------------------------
   // CELLS
   //---------------------------------------------------------------------------

	// Sets the specified column of the current row. 
	// If the pv is set, the item's text is automatically taken from pv
	// according variant's type.
	// NOTE: The title comparsion is case insensitive.
	void SetItem(int nRowIdx, int nColIdx, CWsfTableCtrl::CItem& item, CWslVariant* pv = NULL);
	void SetItem(int nRowIdx, const char* pcszTitle, CWsfTableCtrl::CItem& item, CWslVariant* pv = NULL);

   //---------------------------------------------------------------------------
   // FOR OVERRIDDEN
   //---------------------------------------------------------------------------

	// Called when the content of the table should be set.
   // If nOnlyThisRowId is non-zero, update only the row with this ID
   // which has the index nOnlyThisRowIdx.
   virtual void OnFillTable(int nSortColIdx, 
      BOOL bSortAsc, 
         int nOnlyThisRowId,
         int nOnlyThisRowIdx) 
   { }

	// Called before properties of a row are displayed.
   // If nRowIdx is -1, properties for a new row assumed.
	// 1) Set the title of the property sheet.
	// 2) Create pages on the heap and add them to the sheet. 
	//		The pages will be auto deleted. (even if you return false and 
	//		some pages has already been added).
	// Return true, if properties can be displayed.
   virtual BOOL OnInitProperties(int nRowIdx, CWsfPropertySheet& ps) { return false; }

	// Called after user dismiss the properties dialog.
   // If nRowIdx is -1, properties for a new row assumed.
	// bOk is true, if OK or DONE button was pressed. It's false when CANCEL
	// button was pressed.
	// Return TRUE if the PS dialog can be closed.
   virtual BOOL OnCloseProperties(int nRowIdx, CWsfPropertySheet& ps, BOOL bOk) { return false; }

	// Called when WSF_UV_DB_HAS_CHANGED notification massage is recieved.
	// You should return an action that should be taken to update the view.
	// This method version translated the change type to action code
   virtual void OnDbEvent(const CWsfDbEvent& e, CWsfTableCtrl2::CUpdateManager& um) { }

   // Table layout persistance
   virtual void OnSaveLayout(::CWsfTableCtrl::CLayout& l) { }
   virtual void OnLayoutChanged(::CWsfTableCtrl::CLayout& l) { }

   // Selection change 
   virtual void OnSelectionChanged() { }

protected:  
	//---------------------------------------------------------------------------
	// Internals
	//---------------------------------------------------------------------------

	void FillTable(BOOL bRememberPos);

	// Sets the message string on the status bar
	void SetMessageString(BOOL bEmpty = false);

	// If a row with the given key doesn't exists, its added
	// otherwise is updated. 
	// If bUpdateOnly is TRUE, the row must exist.
	void AddUpdateRow(int nPrimaryKey, BOOL bUpdateOnly);

	// Displays the context menu on the specified position
	void OnRclick(int nX, int nY);

	// Properties callback
	static BOOL PSCallback(void *pData, CWsfPropertySheet& ps, BOOL bOk);

	int GetRowIdxById(int n);

	//---------------------------------------------------------------------------
	//  MFC
	//---------------------------------------------------------------------------

	//{{AFX_VIRTUAL(CWsfTableCtrl2)
   public:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWsfTableCtrl2)
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnWsfRefresh();
	afx_msg void OnUpdateWsfRefresh(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg LRESULT OnWSF_WM_UPDATE_MESSAGE_TEXT(WPARAM wParam, LPARAM lParam);
   afx_msg void OnNotifyReflect(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnWsfExportCsv();
	afx_msg void OnUpdateWsfExportCsv(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
