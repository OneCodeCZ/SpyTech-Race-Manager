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

class CWslLongBinary;

// Notify codes

#define WSF_LVN_SORTINGCHANGED			35000
#define WSF_LVN_SELCHANGED					35001
#define WSF_LVN_DEL_PRESSED				35002
#define WSF_LVN_COL_ORDER_CHANGED		35003
#define WSF_LVN_COL_WIDTH_CHANGED		35004
#define WSF_LVN_COLUMN_ADDED				35005
#define WSF_LVN_COLUMN_REMOVED			35006
#define WSF_LVN_COLUMNS_RESETED			35007
#define WSF_LVN_DRAG							35008		
#define WSF_LVN_RDRAG						35009		// right button draging
#define WSF_LVN_INS_PRESSED				35010

// Flags

#define WSF_LVF_FOCUSED					   	1
#define WSF_LVF_SINGLE_SELECTION		      2   		// can be set by a user
#define WSF_LVF_CHECK_DRAG				      4
#define WSF_LVF_CHECK_RDRAG			   	8
#define WSF_LVF_INTERNAL_SORTING		      16			// can be set by a user
#define WSF_LVF_HIGHLIGHT_SORTING_COLUMN  32       // can be set by a user

//*****************************************************************************
class WSL_API CWsfTableCtrl : public CWnd
{
public:
   //*****************************************************************************
   class WSL_API CNotifyInfo
   {
   public:
      NMHDR   hdr;
      int	  nItem;			// item index
      DWORD	  dwState;		// item state
      BOOL	  bSortAsc;		// sorting direction
      int	  x,y;				// position (e.g. when r-clicked)
   };
   
   //**************************************************
   class WSL_API CItem
   {
      friend class CWsfTableCtrl;
      
   protected:
      enum EStateFlags { FOCUSED=1, SELECTED=2, REDRAW=4 };
      
   public:
      CString  sText;
      int		nImageIdx;     // Global image index, -1 = no image
      COLORREF rgbColor;
      COLORREF rgbBackground; // -1 = default window color
      BOOL     bBold;
      BOOL     bRectangle;
      DWORD		dwParam;		   // user data
      
   protected:
      DWORD		dwStateFlags;
      
   public:
      CItem();
      ~CItem();
      
      // Copies all but the state. The current state is kept.
      void operator =(const CItem& i);
      
      // Clears the item's content, but keeps item's state
      void ClearContent();
      
   protected:
      // States
      inline BOOL IsSelected() const { return dwStateFlags & SELECTED; }
      inline BOOL IsFocused() const { return dwStateFlags & FOCUSED; }
      inline BOOL IsMarkedRedraw() const { return dwStateFlags & REDRAW; }
      
      inline void Select() { dwStateFlags |= SELECTED; }
      inline void Unselect() { dwStateFlags &= ~SELECTED; }
      
      inline void Focus() { dwStateFlags |= FOCUSED; }
      inline void Blur() { dwStateFlags &= ~FOCUSED; }
      
      inline void SelectAndFocus() { dwStateFlags |= FOCUSED | SELECTED; }
      inline void InvertSelection() { dwStateFlags ^= SELECTED; }
      
      inline void MarkRedraw() { dwStateFlags |= REDRAW; }
      inline void ClearRedraw() { dwStateFlags &= ~REDRAW; }

      // ---------
      void CopyWithState(const CItem& i);
   };
   
   //*****************************************************************************
   class WSL_API CColumn
   {
   friend class CWsfTableCtrl;
   public:
      enum EFlags          { SORTABLE=1, VISIBLE=2, DEFAULT=4, BIDI_SORTING=8 };
      enum EAligns         { LEFT, RIGHT, CENTER };
      enum EResizingTypes  
      { 
         FIXED,      // cannot be resized
         DYNAMIC,    // resizable, doesn't stretch
         STRETCHABLE // resizable, stretches according the window size
      };

   public:
      // User parameters
      DWORD          dwFlags;
      EResizingTypes eResizingType;
      EAligns        eAlign;
      CString        sTitle;
      int		      nImageIdx;     // Global column image index, -1 = none
      int		      nDefaultWidth; // in pixels
      CWslPtr<CObject>	pUserData;
   
   protected:
      int		      nHdrCtrlCol;
      int            nWidth;        // The width set by the user. Pixels.
      int            nDisplayWidth; // The actual displayed width. Pixels.
      CWslObArray<CItem>  m_aItems;
      
   public:
      CColumn();
      ~CColumn();

      // Resizing
      BOOL IsFixed() const { return eResizingType==FIXED; }
      BOOL IsDynamic() const { return eResizingType==DYNAMIC; }
      BOOL IsStretchable() const { return eResizingType==STRETCHABLE; }
      BOOL IsResizable() const { return IsStretchable() || IsDynamic(); }

      // Flags
      BOOL IsVisible() const { return dwFlags & VISIBLE; }
      BOOL IsSortable() const { return dwFlags & SORTABLE; }
      BOOL IsDefault() const { return dwFlags & DEFAULT; }
      BOOL IsBidiSorting() const { return dwFlags & BIDI_SORTING; }
   
      // Helpers
	   DWORD GetHdrCtrlAlign() const;
	   DWORD GetDrawTextAlign() const;
   };
   
   //**************************************************
   class WSL_API CLayout : public CObject
   {
   public:
      int		         nSortCol;
      BOOL	            bSortAsc;
      CWslArray<int>		aColsOrder;			// list of visible columns
      CWslArray<int>	   aColWidths;			// width of all columns
   public:
      BOOL LoadFromRegistry(const char* pcszKey); 
      void SaveToRegistry(const char* pcszKey); 

      void LoadFromFile(const CString& sPathName) throw (CException*);
      void SaveToFile(const CString& sPathName) throw (CException*);

      BOOL IsValid() const { return !aColsOrder.IsEmpty(); }

      virtual void Serialize(CArchive& ar);
      void operator =(const CLayout& l);
   };

protected:
	CHeaderCtrl				   m_HdrCtrl;
	CWslObArray<CColumn>	   m_aCols;
   CWslObArray<CWslPtr<CObject> > m_aRowData;
public:
	CWslArray<int>				m_aColsOrder;			// list of visible columns
protected:
	HFONT							m_hFont, m_hBoldFont;
	HDC							m_hDC;
	HBITMAP						m_hArrowUp, m_hArrowDown;
	HPEN							m_hBlackPen;
   HBRUSH                  m_hWindowBackgndStockBrush;      // stock object -> do not delete
   HBRUSH                  m_hSortColumnBackgndBrush;

	int							m_nNumRows;
	int							m_nFirstRow;		// First visible row
	int							m_nFirstSelRow;   // A row from which the SHIFT style selection begins
															// If there are no rows or no row is focused, it's 0.
	int							m_nFocusedRow;
	int							m_nNumSelRows;
	int							m_nMaxVisibleRows;
	int							m_nMaxEntirelyVisibleRows;

	int							m_nFocusedRowBak;	// Backup variables for position remembering
	int							m_nFirstRowBak;

	BOOL							m_bNoHdrNotify;
	int							m_nCurCol;

	int							m_nSortCol;				// -1 = none
	BOOL							m_bSortAsc;
	int							m_nDefSortCol;		// -1 = none
	BOOL							m_bDefSortAsc;

	DWORD							m_dwFlags;
	HIMAGELIST					m_hImageList;
	int							m_nFH;						// Fonts
	int							m_nWH, m_nWW;			// Window
	int							m_nHH;						// Header
	int							m_nBLX, m_nBRX;		// Border X pos
	int							m_nRH;						// Row height
	int							m_nMaxW;
	int							m_nDontDrawCounter;	// if > 0 nothing is drawn
	int							m_nDragX, m_nDragY;	// Start point for drag&drop

public:
	CWsfTableCtrl();
	virtual ~CWsfTableCtrl();
   DECLARE_DYNCREATE(CWsfTableCtrl)

   // Creates the table supposing it will be used as a control in a dialog
   // If the nId control already exists. The table will have 
   // its size and position, and the control will be destroyed.
   BOOL CreateAsControl(CWnd* pParent, int nTemplateCtrlId);

	//---------------------------------------------------------------------------
	// General
	//---------------------------------------------------------------------------

	// Creation
	// ~~~~~~~~
	// Call standard CWnd.Create method. The following styles are supported:
	// HDS_DRAGDROP	- enables columns drag & drop
	
	// Loads/saves the current settings (column order, active columns, ...)
	// If Load() returns FALSE, the data format is bad or the 
	// number of columns in the layout and currently in the table differs.
	BOOL SetLayout(CLayout& l);
	void GetLayout(CLayout& l);

	// Sets image lists.
	// bList = true -> for the list items
	//			 = false -> for the columns
	void SetImageList(CImageList *pImgList, BOOL bListForItems);

	// Redraws the list by posting the WM_PAINT message.
	void Redraw();

	// Redraws the given item
	void DrawRow(int nItem);

	// Push/pop list position and focus
	// REDRAW: Pop(): entire list
	void Push();
	void Pop();

	// Modifies the flags. This should be done after the control is created.
	void ModifyFlags(int nAdd, int nRemove=0);

   // Hightlights the sorting column by making its background color different
   // REDRAW: none
   void HightlightSortingColumn(BOOL bDoIt);

	//---------------------------------------------------------------------------
	// ROWS
	//---------------------------------------------------------------------------

	// Adds a new row to the table. The columns are empty by default.
	// Returns the index of the new row.
	// REDRAW: only scroll bar
	int AddRow(int nPos = -1, CWslPtr<CObject> pRowData = NULL);

	// Deletes a row. If the item was focused and bUpdateFocus is true, next item is 
	// selected and gains focus.
	void DeleteRow(int nRow, BOOL bUpdateFocus = true);

	// Deletes all rows.
	// REDRAW: only scroller 
	void DeleteAllRows();

	// Swaps two rows
	void SwapRows(int nRow1, int nRow2);

	// Item count
	inline int GetRowCount() const { return m_nNumRows; }

   // Returns the row's data
   CWslPtr<CObject> GetRowData(int nRowIdx) const;

   // Sets the row's data
   void SetRowData(int nRowIdx, CWslPtr<CObject> pRowData);

	//---------------------------------------------------------------------------
	// ITEMS 
	//---------------------------------------------------------------------------

	// Sets item data.
	// REDRAW: item if specified
	void SetItem(int nRow, int nCol, CItem* pItem, BOOL bRedraw = false);

	// Sets item's properties.
	// REDRAW: item if specified
	void SetItem(int nRow, int nCol, const char *pcszText, 
		COLORREF rgbColor = -1, BOOL bRedraw = false);

	// Sets/gets item's data.
	DWORD GetItemData(int nRow, int nCol) const;
	void SetItemData(int nRow, int nCol, DWORD dwParam); 

	// Sets item's image
	void SetItemImage(int nRow, int nCol, int nImg);

	// Returns index of an item whose dwParam is equal to the given dwParam.
	// Returns -1 when no item has been found.
	int FindItem(int nCol, DWORD dwParam);

   // Returns an item at the given position

   const CItem& GetItem(int nRow, int nCol) const { return m_aCols[nCol].m_aItems[nRow]; }
   const CItem* GetItem(int nRow, const char* pcszColTitle) const;

	//---------------------------------------------------------------------------
	// SELECTION
	//---------------------------------------------------------------------------
	
	// Number of selected rows
	inline int GetSelRowCount() const { return m_nNumSelRows; }
	
   inline BOOL IsSelection() const { return GetSelRowCount() > 0; }
   inline BOOL IsMultiselection() const { return GetSelRowCount() > 1; }
	inline BOOL IsSingleSelection() const { return GetSelRowCount() == 1; }

	// For iterating the selected items.
	// Should not be used if items are deleted in the loop.
	// Use "for (i=0;i<GetNumItems();i++) if (IsItemSelected(i))..." instead.
	POSITION GetFirstSelectedRowPosition();
	int GetNextSelectedRow(POSITION& pos);

	// Return true if item is selected.
	BOOL IsRowSelected(int nItem);

	// Gets currently selected item. -1 if no selection.
	// Works well ONLY in SINGLE SELECTION mode.
	inline int GetCurSel() const { ASSERT(m_dwFlags & WSF_LVF_SINGLE_SELECTION); return m_nFocusedRow; }

	// Gets currently focused item. -1 if no item is focused.
	inline int GetFocusedRow() const { return m_nFocusedRow; } 

	// Sets current selection and focus to only one item.
	// If nItem is -1 selection is removed.
	// REDRAW: only affected items
	void SelectSingleRow(int nRowIdx, BOOL bEnsureVisible);

	// Select all items
	void SelectAllRows();

	//---------------------------------------------------------------------------
	// COLUMNS 
	//---------------------------------------------------------------------------

	// Adds a new column
	void AddColumn(CColumn* pCol);

   void AddColumn(const char* pcszTitle,
      int nDefaultWidthInPx,
		BOOL bDefault = true,
		BOOL bSortable = true,
		CColumn::EAligns eAlign = CColumn::LEFT,
      CWslPtr<CObject> pColumnData = NULL);

   void AddStretchColumn(const char* pcszTitle,
      int nDefaultWidthInPercent,
		BOOL bDefault = true,
		BOOL bSortable = true,
		CColumn::EAligns eAlign = CColumn::LEFT,
      CWslPtr<CObject> pColumnData = NULL);

	void AddIconicColumn(const char* pcszTitle,
		int nIconBmpId,
 		int nIconIdx,
		BOOL bDefault = true,
		BOOL bSortable = true,
      CWslPtr<CObject> pColumnData = NULL);

	// Shows/hides a column
	void ActivateColumn(int nCol, 
		int nOrder = -1, 
		BOOL bRecalcLayout = true,
		BOOL bSetDefaultWidth = true);
	void DeactivateColumn(int nCol);

	void SetSortColumn(int nCol, BOOL bSortAsc, BOOL bNotify);
	void ActivateDefaultColumns();

	// Returns sortinig column. -1 if none.
	inline int GetSortCol() const { return m_nSortCol; }

	// Returns true if sorting is ascending.
	inline int GetSortDir() const { return m_bSortAsc; }

	// Returns data associated with a column 
	CWslPtr<CObject> GetColumnData(int nColumn);

	// Removes all columns
	// NOTE: You must define immediatelly new columns, because
	// the drawing code will fail if there are no columns!
	void RemoveAllColumns();

	// Sets the default sorting column that is used when
	// the layout is reseted.
	void SetDefaultSortColumn(int nSortColIdx, BOOL bSortAsc);

   // Returns a column at the given position
   const CColumn& GetColumn(int nIdx) { return m_aCols[nIdx]; }
    
	// Returns the index of the given column. 
	// If it's not found, -1 is returned.
	// NOTE: Case insensitive.
	int GetColIdx(const char* pcszTitle) const;

   void SetColumnBidiSorting(int nColIdx, BOOL bAllow);

	//---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------
protected:
	void DrawRow(int nItem, int nY);

	void SelectRowsAccordingKeyState(int nNewFocusedItem);

	// Enables/disables any drawing
	inline void EnableDrawing() { m_nDontDrawCounter--; }
	inline void DisableDrawing() { m_nDontDrawCounter++; }

	// Reinits the scroller according current state
	void UpdateVScroller();

	// Draw XORed vertical line
	void InvertTracker(int nX);

	// Returns order of the column with given index
	int IndexToOrder(int nCol);

	// Returns order of the column with given header ctrl index
	int HdrIndexToOrder(int nHdrCol);

	// Sends WM_NOTIFY message. "p" can be NULL.
   void Notify(int nCode, CWsfTableCtrl::CNotifyInfo* p = NULL);

	// Selectes given item range. The first item gets the focus.
	void SelectRows(int nFirst, int nLast, BOOL bRemovePrevSel, BOOL bInvert, BOOL bSetFirstSelItem);

	// Marks all items as unselected but the items in the given range.
	// Sets number of selected items to 0.
	void RemoveSelection(int nFirst, int nLast);

	// Sorts items according current sorting column
	void SortRows();

   // Updates visual stuff like the background color, font etc.
   void UpdateVisualStyle();

   //---------------------------------------------------------------------------
   // COLUMNS (INTERNAL)
   //---------------------------------------------------------------------------

   // CColumn from column order
   CColumn& GetColumnByOrder(int nColOrderIdx);

   // Total visible columns
   int GetVisibleColumnsCount() const { return m_aColsOrder.GetSize(); }

	// Changing the width of a column
	BOOL OnBeginTrack(int nColOrderIdx);
   void OnEndTrack(int nColOrderIdx, int nNewWidth);

   // Column's context menu
	void OnHeaderContextMenu(BOOL bOnParticularColumn);

   // Hides all columns
	void DeactivateAllColumns();

	// Recalcs the width of columns.
	void RecalcColumnLayout();

   //---------------------------------------------------------------------------
	// MFC stuff
	//---------------------------------------------------------------------------

	//{{AFX_VIRTUAL(CWsfTableCtrl)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWsfTableCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT pCS);
	afx_msg void OnRemoveColumn();
	afx_msg void OnColumnsReset();
	afx_msg void OnSortAsc();
	afx_msg void OnSortDesc();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnWsfColumns();
	afx_msg void OnUpdateWsfColumns(CCmdUI* pCmdUI);
	afx_msg void OnWsfSortAsc();
	afx_msg void OnUpdateWsfSortAsc(CCmdUI* pCmdUI);
	afx_msg void OnWsfSortDesc();
	afx_msg void OnUpdateWsfSortDesc(CCmdUI* pCmdUI);
	afx_msg void OnWsfSortColumn();
	afx_msg void OnUpdateWsfSortColumn(CCmdUI* pCmdUI);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
