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

class CWsfTIQuery;

// Column flags. 
// They must be kept in sync with the table control flags.

#define WSF_QCF_DEFAULT				4
#define WSF_QCF_SORTABLE			1
#define WSF_QCF_ALIGN_LEFT		16
#define WSF_QCF_ALIGN_CENTER	64
#define WSF_QCF_ALIGN_RIGHT		32

//*****************************************************************************
class WSL_API CWsfQueryDoc : public CDocument
{
friend class CWsfQueryView;
public:
	// Use CDocument::UpdateAllView() method to send these control
	// messages.
	enum EUVCodes 
	{ 
		REFRESH				= 30600,		// refreshes the view (requeries the DB).
		RESET					= 30601, 		// Resets the view: loads profile, redefines columns
		SAVE_PROFILE	= 30602			// Saves the current profile
	};

	// Action codes - what to do, if the DB has changed
	enum EActions
	{
		DO_NOTHING	= 0,
		REQUERY			= 1,
		UPDATE_ROW	= 2,
		DELETE_ROW	= 3
	};

protected:
	// A column in the SELECT command
	class CWsfQueryColumn
	{
	friend CWsfQueryDoc;
	friend CWsfQueryView;
	protected:
		CString				sTitle;
		BOOL					bPrimaryKey;
	public:
		// Accessible in the OnGetViewColumnValue() method.
		CWslVariant	v;						
	};

	// A column in the query view
	struct CWsfViewColumn
	{
		CString	sTitle;
		int			nQueryColIdx;		// for sorting & default OnGetViewColValue() impl.
		int			nImage;
		int			nWidth;
		DWORD		dwFlags;
	};

private:
	DWORD						m_dwMenu;						// Context menu of a row; hiword=sub menu idx, loword=menu id
	int							m_nDefCmd;					// Default command of a row
	BOOL						m_bFilterDlg;				// Is filter dialog supported?
	int							m_nPrimaryKeyColIdx;// index into m_lColumns list

	// Current row info
	// Valid in cmd handlers only!
	int							m_nPrimaryKey;
	CObject*				m_pRowData;

	// These are used while handling command in CWsfQueryView::OnCmdMsg
	// Valid in cmd handlers only!
	int							m_nSelCount;	
	BOOL						m_bFirstItem, m_bLastItem;
	BOOL						m_bCancelCommand;

protected:
	CWslArray<CWsfQueryColumn*>	m_aQueryCols;
	CWslArray<CWsfViewColumn*>	m_aViewCols;
	int							m_nSortColIdx;
	int							m_bSortAsc;
											
public:
	//---------------------------------------------------------------------------	
	// Lifespan of the class
	//---------------------------------------------------------------------------	

	DECLARE_DYNAMIC(CWsfQueryDoc)

	// Specify the default menu, command and filter status.
	CWsfQueryDoc(int nMenuId, 
		int nSubMenuIdx,
		BOOL bFilterDlg,
		int nDefCmd);
	virtual ~CWsfQueryDoc();

	//---------------------------------------------------------------------------	
	// Query & view columns
	//---------------------------------------------------------------------------	

	// Adds a view column. 
	// View columns are displayed in the query view's table control.
	// nQueryColIdx specifies which query column will be displayed in 
	// this view column by the default implementaion of the OnGetColumnValue() method. 
	// It also specifies the query column, that will be used for
	// sorting if the table should be sorted by this view column.
	void AddTextViewCol(const char* pcszTitle, 
		int nQueryColIdx,
		int nWidth,
		DWORD dwFlags = WSF_QCF_SORTABLE | WSF_QCF_DEFAULT | WSF_QCF_ALIGN_LEFT);

	// nIconBmpId must be registered in the framework.
	void AddIconViewCol(const char* pcszTitle, 
		int nQueryColIdx,
		int nIconBmpId,
 		int nIconIdx,
		DWORD dwFlags = WSF_QCF_SORTABLE | WSF_QCF_DEFAULT | WSF_QCF_ALIGN_LEFT);

	// Adds a query column. 
	// Query columns are used in the SELECT command, they are loaded
	// from the database.
	void AddQueryCol(const char* pcszName, BOOL bPrimaryKey = false);

	// Returns the value of the given query col.
	// Use only in the OnGetViewColumnValue() and OnGetRowData() methods.
	inline CWslVariant& GetQueryColValue(int n) { return m_aQueryCols[n]->v; }

	// Sets the default sorting column
	void SetSortCol(int nColIdx, BOOL bAsc);

  //---------------------------------------------------------------------------
	// Methods for querying the selection state. 
	// They can be called ONLY from message handlers!!
	//---------------------------------------------------------------------------

	BOOL IsSelection() const { return m_nSelCount > 0; }
	BOOL IsMultiselection() const { return m_nSelCount > 1; }
	BOOL IsSingleSelection() const { return m_nSelCount == 1; }
	BOOL IsFirstItem() const { return m_bFirstItem; }
	BOOL IsLastItem() const { return m_bLastItem; }
	int GetSelCount() const { return m_nSelCount; }
	void CancelCommand() { m_bCancelCommand = true; }

  //---------------------------------------------------------------------------
	// Query row
	//---------------------------------------------------------------------------

	// Returns the primary key of the current row.
	inline int GetPrimaryKey() const { return m_nPrimaryKey; }
	inline void SetPrimaryKey(int n) { m_nPrimaryKey = n; }

	// Data associated with current item
	inline CObject* GetRowData() const { return m_pRowData; }

	// Called before properties of a row are displayed.
	// 1) Set title of the property sheet.
	// 2) Create pages on the heap and add them to the sheet. 
	//		The pages will be auto deleted. (even if you return false and 
	//		some pages has already been added).
	// Return true, if properties can be displayed.
	virtual BOOL OnInitProperties(CWsfPropertySheet& ps) { ASSERT(false); return false; }

	// Called after user dismiss the properties dialog.
	// bOk is true, if OK or DONE button was pressed. It's false when CANCEL
	// button was pressed.
	// Return TRUE if the PS dialog can be closed.
	virtual BOOL OnCloseProperties(CWsfPropertySheet& ps, BOOL bOk) { return true; }

	// Displayes properties for a new item. 
	// It sets the primary key to -1 and displays the properties.
	// Returns TRUE if the properties were closed with the OK button.
	// NOTE: If SetPrimaryKey() was called in the OnCloseProperties(), you
	// can get the primary key of the new item by calling GetPrimaryKey() 
	// upon return from NewItem(). If it's still -1, something went wrong.
	// NOTE: The new item must not be in the visible in the view bacause of
	// the filter!
	BOOL DisplayNewItemProperties();

	//---------------------------------------------------------------------------	
	// Overload these to customize behavior
	//---------------------------------------------------------------------------	

	// Called when the view needs to connect to the DB
	virtual CWslDatabase& OnGetDb() = 0;

	// Load/save the view layout
	virtual BOOL OnLoadLayout(CWsfTableCtrl::CLayout& l) = 0;
	virtual void OnSaveLayout(CWsfTableCtrl::CLayout& l) = 0;

	// Called when WSF_UV_DB_HAS_CHANGED notification massage is recieved.
	// You should return an action that should be taken to update the view.
	// Action codes are WSF_QV_*
	// This method version translated the change type to action code;
	virtual int OnDbChanged(CWsfDbEvent& e);

	// Displayes user defined filter dialog. Returns TRUE if filter
	// should be turned on.
	virtual BOOL OnDisplayFilterDlg() { return false; }
	
	// Returns the information for performing a query.
	// You must build a query using the provided information:
	// - aColumns specifying the columns to be selected
	// - sWhere specifying a constraint condition
	// - sOrderBy specifying an ordering condition
	virtual CString OnGetQuery(const CString& sColumns, 
		const CString& sWhere, 
		const CString& sOrderBy,
		BOOL bFilter) = 0;

	// Fill "lvi" structure with data that will be displayed 
	// in the "nColIdx" view column.
	// The m_aQueryCols array contains the values of the query columns.
	// NOTE: The default implementation displays in the given
	// view column the value of the query column, that was
	// specified while the view column was created.
	// NOTE: lvi.pszText points to buffer of 1024 chars. You can
	// free use this buffer to store the value.
	// NOTE: when using icons, the specified icon index is 
	// an index into the global icon list maintaing by the CWsfApp object
	virtual void OnGetViewColumnValue(int nViewColIdx, CWsfTableCtrl::CItem& lvi);

	// Return pointer to data that will be associated with current row.
	// The values of query columns are still valid.
	virtual CObject* OnGetRowData() { return NULL; }

  //---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------
protected:

	// Properties callback
	static BOOL PSCallback(void *pData, CWsfPropertySheet& ps, BOOL bOk);

	int DisplayProperties();

protected:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfQueryDoc)
	public:
	//}}AFX_VIRTUAL

	// Generated message map functions

protected:
	//{{AFX_MSG(CWsfQueryDoc)
	afx_msg void OnWsfProperties();
	//}}AFX_MSG

	// Serialization
	//WSL_DECLARE_SERIAL_DLL(WSL_API, CWsfQueryDoc)

	DECLARE_MESSAGE_MAP()
};
