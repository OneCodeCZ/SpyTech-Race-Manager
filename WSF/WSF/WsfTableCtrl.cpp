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

#include "stdafx.h"
#include "main.h"
#include "WsfTableCtrl.h"
#include <generic/WslLongBinary.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WSF_LV_MIN_COL_WIDTH				10
#define WSF_LV_ICONIC_COLUMN_WIDTH		20

#define IsShiftDown()	( !(m_dwFlags & WSF_LVF_SINGLE_SELECTION) && (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) )
#define IsCtrlDown()	( !(m_dwFlags & WSF_LVF_SINGLE_SELECTION) && (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))))

BEGIN_MESSAGE_MAP(CWsfTableCtrl, CWnd)
   //{{AFX_MSG_MAP(CWsfTableCtrl)
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_COMMAND(ID_REMOVE_COLUMN, OnRemoveColumn)
   ON_COMMAND(ID_COLUMNS_RESET, OnColumnsReset)
   ON_COMMAND(ID_SORT_ASC, OnSortAsc)
   ON_COMMAND(ID_SORT_DESC, OnSortDesc)
   ON_WM_PAINT()
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
   ON_WM_LBUTTONDOWN()
   ON_WM_VSCROLL()
   ON_WM_MOUSEWHEEL()
   ON_WM_KEYDOWN()
   ON_WM_RBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_GETDLGCODE()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONUP()
   ON_WM_RBUTTONUP()
   ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_WSF_COLUMNS, OnWsfColumns)
   ON_UPDATE_COMMAND_UI(ID_WSF_COLUMNS, OnUpdateWsfColumns)
   ON_COMMAND(ID_WSF_SORT_ASC, OnWsfSortAsc)
   ON_UPDATE_COMMAND_UI(ID_WSF_SORT_ASC, OnUpdateWsfSortAsc)
   ON_COMMAND(ID_WSF_SORT_DESC, OnWsfSortDesc)
   ON_UPDATE_COMMAND_UI(ID_WSF_SORT_DESC, OnUpdateWsfSortDesc)
   ON_COMMAND(ID_WSF_SORT_COLUMN, OnWsfSortColumn)
   ON_UPDATE_COMMAND_UI(ID_WSF_SORT_COLUMN, OnUpdateWsfSortColumn)
   ON_WM_CHAR()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


IMPLEMENT_DYNCREATE(CWsfTableCtrl, CWnd)


/*****************************************************************************
*
* ITEM
*
*****************************************************************************/

/*****************************************************************************/
CWsfTableCtrl::CItem::CItem()
{
   dwStateFlags = 0;
   ClearContent();
}

/*****************************************************************************/
CWsfTableCtrl::CItem::~CItem()
{
}

/*****************************************************************************/
void CWsfTableCtrl::CItem::ClearContent()
{
   sText.Empty();
   nImageIdx = -1;
   rgbColor = ::GetSysColor(COLOR_WINDOWTEXT);
   rgbBackground = -1;
   dwParam = 0;
   bBold = false;
   bRectangle = false;
}

/*****************************************************************************/
void CWsfTableCtrl::CItem::operator =(const CItem& i)
{
   nImageIdx = i.nImageIdx;
   sText = i.sText;
   rgbColor = i.rgbColor;
   rgbBackground = i.rgbBackground;
   bBold = i.bBold;
   dwParam = i.dwParam;
   bRectangle = i.bRectangle;
}

/*****************************************************************************/
void CWsfTableCtrl::CItem::CopyWithState(const CItem& i)
{
   this->operator =(i);
   dwStateFlags = i.dwStateFlags;
}


/*****************************************************************************
*
* COLUMN
*
*****************************************************************************/

/*****************************************************************************/
CWsfTableCtrl::CColumn::CColumn()
{
   nImageIdx = -1;
   dwFlags = 0;
   eAlign = LEFT;
   eResizingType = DYNAMIC;
   nDefaultWidth = 100;
   nWidth = nDisplayWidth = 0;
   nHdrCtrlCol = -1;
}

/*****************************************************************************/
CWsfTableCtrl::CColumn::~CColumn()
{
}

//*****************************************************************************
DWORD CWsfTableCtrl::CColumn::GetHdrCtrlAlign() const
{
   switch (eAlign)
   {
   case CENTER: return HDF_CENTER;
   case RIGHT : return HDF_RIGHT;
   }
   return HDF_LEFT;
}

//*****************************************************************************
DWORD CWsfTableCtrl::CColumn::GetDrawTextAlign() const
{
   switch (eAlign)
   {
   case CENTER: return DT_CENTER;
   case RIGHT : return DT_RIGHT;
   }
   return DT_LEFT;
}


/*****************************************************************************
*
* TABLE
*
*****************************************************************************/

//*****************************************************************************
CWsfTableCtrl::CWsfTableCtrl()
{
   m_hArrowUp = NULL;
   m_hArrowDown = NULL;
   m_hFont = NULL;
   m_hBoldFont = NULL;
   m_hImageList = NULL;
   m_hSortColumnBackgndBrush = NULL;
   
   m_bNoHdrNotify = false;
   m_nNumRows = 0;
   m_nFirstRow = 0;
   m_dwFlags = WSF_LVF_HIGHLIGHT_SORTING_COLUMN;
   m_nFocusedRow = -1;
   m_nNumSelRows = 0;
   m_nFirstSelRow = 0;
   m_nDontDrawCounter = 0;
   
   m_nSortCol = -1;
   m_nDefSortCol = -1;
}

//*****************************************************************************
CWsfTableCtrl::~CWsfTableCtrl()
{
   // Clean items & columns
   
   RemoveAllColumns();
   
   // Clean stuff
   
   DeleteObject(m_hArrowUp);
   DeleteObject(m_hArrowDown);
   DeleteObject(m_hFont);
   DeleteObject(m_hBoldFont);
   DeleteObject(m_hBlackPen);
   DeleteObject(m_hSortColumnBackgndBrush);
}

//*****************************************************************************
BOOL CWsfTableCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
   if (!CWnd::PreCreateWindow(cs)) return FALSE;
   
   cs.style &= ~WS_BORDER;
   cs.style |= WS_VSCROLL;
   cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_DBLCLKS, 
      ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
   
   return TRUE;
}

/*****************************************************************************/
BOOL CWsfTableCtrl::CreateAsControl(CWnd* pParent, int nTemplateCtrlId)
{
   ASSERT(pParent);
   CWnd* p = pParent->GetDlgItem(nTemplateCtrlId);
   ASSERT(p);
   
   CRect r;
   p->GetWindowRect(&r);
   pParent->ScreenToClient(&r);
   
   BOOL b = CreateEx(WS_EX_CLIENTEDGE, 
      NULL, 
      NULL,
      WS_TABSTOP | WS_CHILD | WS_VISIBLE | HDS_DRAGDROP, 
      r, 
      pParent, 
      nTemplateCtrlId, 
      NULL);
   
   if (!b) return false;
   
   // Put the table at the same position in the children order
   // as the original window
   
   SetWindowPos(p, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
   p->DestroyWindow();
   
   return true;
}

//*****************************************************************************
int CWsfTableCtrl::OnCreate(LPCREATESTRUCT pCS) 
{
   LOGFONT lf;
   int n;
   
   if (CWnd::OnCreate(pCS) == -1) return -1;
   
   // Create GDI objects
   
   m_hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
   ASSERT(m_hFont);
   
   GetObject(m_hFont, sizeof(lf), &lf);
   lf.lfWeight = FW_BOLD;
   m_hBoldFont = CreateFontIndirect(&lf);
   ASSERT(m_hBoldFont);
   
   m_hBlackPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
   ASSERT(m_hBlackPen);

   UpdateVisualStyle();

   // Init DC
   
   m_hDC = ::GetDC(m_hWnd);
   SetBkMode(m_hDC, TRANSPARENT);
   
   TEXTMETRIC tm;
   GetTextMetrics(m_hDC, &tm);
   m_nFH = tm.tmHeight;
   m_nRH = m_nFH < 18 ? 18 : m_nFH + 2;
   
   // Create header control
   // 0x200=HDS_FLAT
   
   n = WS_CHILD | WS_VISIBLE | HDS_BUTTONS | (pCS->style & HDS_DRAGDROP) | 0x0200;
   VERIFY(m_HdrCtrl.Create(n, WsfZeroRect, this, 1000));
   m_HdrCtrl.SendMessage(WM_SETFONT, (WPARAM) m_hFont, MAKELPARAM(false, 0));
   
   // Init sort bitmaps
   
   m_hArrowUp = (HBITMAP) LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_ARROW_UP), 
      IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS | LR_SHARED);
   m_hArrowDown = (HBITMAP) LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_ARROW_DOWN), 
      IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS | LR_SHARED);

   // Default icons

   SetImageList(WsfGetApp()->GetSmallIconList(), true);
   SetImageList(WsfGetApp()->GetColumnIconList(), false);

   return 0;
}

//*****************************************************************************
void CWsfTableCtrl::ModifyFlags(int nAdd, int nRemove/*=0*/)
{
   m_dwFlags |= nAdd;
   m_dwFlags &= ~nRemove;
}

//*****************************************************************************
int CWsfTableCtrl::IndexToOrder(int nCol)
{
   int n = m_aColsOrder.GetSize();
   for (int i=0; i<n; i++) if (m_aColsOrder[i] == nCol) return i;
   return -1;
}

//*****************************************************************************
int CWsfTableCtrl::HdrIndexToOrder(int nHdrCol)
{
   int n = m_aColsOrder.GetSize();
   for (int i=0; i<n; i++) if (m_aCols[m_aColsOrder[i]].nHdrCtrlCol == nHdrCol) return i;
   ASSERT(false);
   return -1;
}

//*****************************************************************************
void CWsfTableCtrl::SetImageList(CImageList *pImgList, BOOL bListForItems)
{
   ASSERT_VALID(pImgList);
   ASSERT(pImgList->m_hImageList);
   
   if (bListForItems)
      m_hImageList = pImgList->m_hImageList;
   else 
      m_HdrCtrl.SetImageList(pImgList);
}

//*****************************************************************************
void CWsfTableCtrl::HightlightSortingColumn(BOOL bDoIt)
{
   if (bDoIt)
      m_dwFlags |= WSF_LVF_HIGHLIGHT_SORTING_COLUMN;
   else
      m_dwFlags &= ~WSF_LVF_HIGHLIGHT_SORTING_COLUMN;
}

/*****************************************************************************
*
* COLUMNS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfTableCtrl::AddColumn(const char* pcszTitle,
                              int nDefaultWidthInPx,
                              BOOL bDefault,
                              BOOL bSortable,
                              CColumn::EAligns eAlign,
                              CWslPtr<CObject> pColumnData)
{
   CColumn c;

   c.sTitle = pcszTitle;
   c.nDefaultWidth = nDefaultWidthInPx;
   if (bDefault) c.dwFlags |= CColumn::DEFAULT;
   if (bSortable) c.dwFlags |= CColumn::SORTABLE | CColumn::BIDI_SORTING;
   c.eAlign = eAlign;
   c.eResizingType = CColumn::DYNAMIC;
   c.pUserData = pColumnData;

   AddColumn(&c);
}

/*****************************************************************************/
void CWsfTableCtrl::AddStretchColumn(const char* pcszTitle,
                                     int nDefaultWidthInPercent,
                                     BOOL bDefault,
                                     BOOL bSortable,
                                     CColumn::EAligns eAlign,
                                     CWslPtr<CObject> pColumnData)
{
   CColumn c;

   c.sTitle = pcszTitle;
   c.nDefaultWidth = nDefaultWidthInPercent;
   if (bDefault) c.dwFlags |= CColumn::DEFAULT;
   if (bSortable) c.dwFlags |= CColumn::SORTABLE | CColumn::BIDI_SORTING;
   c.eAlign = eAlign;
   c.eResizingType = CColumn::STRETCHABLE;
   c.pUserData = pColumnData;

   AddColumn(&c);
}

/*****************************************************************************/
void CWsfTableCtrl::AddIconicColumn(const char* pcszTitle,
                                    int nIconBmpId,
                                    int nIconIdx,
                                    BOOL bDefault,
                                    BOOL bSortable,
                                    CWslPtr<CObject> pColumnData)
{
   CColumn c;

   c.sTitle = pcszTitle;
   if (bDefault) c.dwFlags |= CColumn::DEFAULT;
   if (bSortable) c.dwFlags |= CColumn::SORTABLE | CColumn::BIDI_SORTING;
   c.eResizingType = CColumn::FIXED;
   c.nImageIdx = WsfGetApp()->GetColumnIconIdx(nIconBmpId, nIconIdx);
   c.pUserData = pColumnData;

   ASSERT(c.nImageIdx != -1);

   AddColumn(&c);
}

//*****************************************************************************
void CWsfTableCtrl::AddColumn(CWsfTableCtrl::CColumn* pCol)
{
   m_aCols.GrowBy(1);
   CWsfTableCtrl::CColumn& lvc = m_aCols.GetLast();
   
   ASSERT(!pCol->sTitle.IsEmpty());
   
   lvc = *pCol;
   lvc.dwFlags &= ~(CColumn::VISIBLE);
   
   if (lvc.nImageIdx != -1) 
   {
      lvc.eResizingType = CColumn::FIXED;
      lvc.nDefaultWidth = WSF_LV_ICONIC_COLUMN_WIDTH;
   }
   
   if (lvc.nDefaultWidth <  WSF_LV_MIN_COL_WIDTH) lvc.nDefaultWidth = WSF_LV_MIN_COL_WIDTH;
   lvc.nWidth = lvc.nDefaultWidth;
}

//*****************************************************************************
void CWsfTableCtrl::ActivateColumn(int nCol, int nOrder /*= -1*/, 
                                   BOOL bRecalcLayout /*= true*/,
                                   BOOL bSetDefaultWidth /*= true*/)
{
   HDITEM hdi;
   
   if (nOrder == -1) nOrder = m_aColsOrder.GetSize();
   
   ASSERT(nCol < m_aCols.GetSize());
   ASSERT(nOrder <= m_aColsOrder.GetSize());
   
   m_aColsOrder.InsertAt(nOrder, nCol);
   CWsfTableCtrl::CColumn& lvc= m_aCols[nCol];
   
   if (bSetDefaultWidth) lvc.nWidth = lvc.nDefaultWidth;
   
   // Insert column to the header control
   
   hdi.mask = HDI_FORMAT | HDI_WIDTH;
   ASSERT(lvc.nWidth > 0);
   hdi.cxy = lvc.nWidth;
   hdi.fmt = lvc.GetHdrCtrlAlign();
   
   if (lvc.nImageIdx == -1)
   {
      hdi.pszText = (char*) (const char*) lvc.sTitle;
      hdi.mask |= HDI_TEXT;
   }
   else
   {
      hdi.iImage = lvc.nImageIdx;
      hdi.mask |= HDI_IMAGE;
      hdi.fmt |= HDF_IMAGE;
   }
   
   lvc.nHdrCtrlCol = m_aColsOrder.GetSize() - 1;
   lvc.dwFlags |= CColumn::VISIBLE;
   
   m_HdrCtrl.InsertItem(lvc.nHdrCtrlCol, &hdi);
   
   // Update
   
   if (bRecalcLayout) RecalcColumnLayout();
}

//*****************************************************************************
void CWsfTableCtrl::DeactivateColumn(int nCol)
{
   int i,n,x;
   
   // Remove from order and from header ctrl
   
   i = IndexToOrder(nCol);
   ASSERT(i != -1);
   
   m_aColsOrder.RemoveAt(i);
   x = m_aCols[nCol].nHdrCtrlCol;
   m_HdrCtrl.DeleteItem(x);
   
   // Shift column indexes
   
   for (i=0, n=m_aColsOrder.GetSize(); i<n; i++)
   {
      CWsfTableCtrl::CColumn &lvc= m_aCols[m_aColsOrder[i]];
      if (lvc.nHdrCtrlCol > x) lvc.nHdrCtrlCol--;
   }
   
   // Update
   
   CWsfTableCtrl::CColumn &lvc= m_aCols[nCol];
   lvc.dwFlags &= ~CColumn::VISIBLE;
   
   if (m_nSortCol == nCol) m_nSortCol = -1;
   RecalcColumnLayout();
}

//*****************************************************************************
void CWsfTableCtrl::DeactivateAllColumns()
{
   //DeleteAllRows();
   
   while (m_HdrCtrl.DeleteItem(0));
   m_aColsOrder.RemoveAll();
   m_nSortCol = -1;
   
   Redraw();
}

//*****************************************************************************
void CWsfTableCtrl::ActivateDefaultColumns()
{
   int i,n;
   
   DeactivateAllColumns();
   
   // Activate all default columns
   
   for (i=0,n=m_aCols.GetSize(); i<n; i++)
   {
      if (m_aCols[i].dwFlags & CColumn::DEFAULT)
         ActivateColumn(i, -1, false, true);
      else 
         m_aCols[i].dwFlags &= ~CColumn::VISIBLE;
   }
   
   // Default sorting
   
   SetSortColumn(m_nDefSortCol, m_bDefSortAsc, false);
   
   // Update
   
   RecalcColumnLayout();
}

//*****************************************************************************
CWslPtr<CObject> CWsfTableCtrl::GetColumnData(int nColumn)
{
   ASSERT(nColumn >= 0 && nColumn < m_aCols.GetSize());
   return m_aCols[nColumn].pUserData;
}

//*****************************************************************************
void CWsfTableCtrl::RemoveAllColumns()
{
   DeleteAllRows();
   m_aCols.RemoveAll();
}

/*****************************************************************************/
void CWsfTableCtrl::SetDefaultSortColumn(int nSortColIdx, BOOL bSortAsc)
{
   ASSERT(nSortColIdx>=-1 && nSortColIdx<m_aCols.GetSize());
   m_nDefSortCol = nSortColIdx;
   m_bDefSortAsc = bSortAsc;
}

/*****************************************************************************/
CWsfTableCtrl::CColumn& CWsfTableCtrl::GetColumnByOrder(int nColOrderIdx)
{
   ASSERT(nColOrderIdx >= 0 && nColOrderIdx < m_aColsOrder.GetSize());
   int n = m_aColsOrder[nColOrderIdx];
   ASSERT(n>=0 && n<=m_aCols.GetSize());
   return m_aCols[n];
}

/*****************************************************************************/
int CWsfTableCtrl::GetColIdx(const char* pcszTitle) const
{
   for (int i=0; i<m_aCols.GetSize(); i++)
   {
      if (!_stricoll(pcszTitle, m_aCols[i].sTitle)) return i;
   }

   return -1;
}

/*****************************************************************************/
void CWsfTableCtrl::SetColumnBidiSorting(int nColIdx, BOOL bAllow)
{
   ASSERT(nColIdx >= 0 && nColIdx < m_aCols.GetSize());

   if (bAllow)
      m_aCols[nColIdx].dwFlags |= CColumn::BIDI_SORTING;
   else
      m_aCols[nColIdx].dwFlags &= ~CColumn::BIDI_SORTING;
}

/*****************************************************************************
*
* ROWS
*
*****************************************************************************/

//*****************************************************************************
int CWsfTableCtrl::AddRow(int nPos/*=-1*/, CWslPtr<CObject> pRowData)
{
   if (nPos<0 || nPos>m_nNumRows) nPos = m_nNumRows;
   
   m_aRowData.InsertAt(nPos, pRowData);

   for (int i = m_aCols.GetSize(); i--;)
   {
      m_aCols[i].m_aItems.InsertAt(nPos);
   }
   
   m_nNumRows++;
   UpdateVScroller();
   
   return nPos;
}

//*****************************************************************************
void CWsfTableCtrl::DeleteRow(int nRow, BOOL bUpdateFocus /*= true*/)
{
   ASSERT(nRow >=0 && nRow < m_nNumRows);
   CWsfTableCtrl::CItem* p;
   BOOL bSelChanged = false;
   
   DisableDrawing();
   
   // Remove item from selection
   
   if (m_aCols[0].m_aItems[nRow].IsSelected()) 
   {
      m_nNumSelRows--;
      bSelChanged = true;
   }
   
   // Remove from columns
   
   for (int t=0; t<m_aCols.GetSize(); t++)
   {
      m_aCols[t].m_aItems.RemoveAt(nRow);
   }

   // Delete the row data

   m_aRowData.RemoveAt(nRow);
   
   // Update first position to keep the window full of items
   
   if (m_nFirstRow + m_nMaxEntirelyVisibleRows > m_nNumRows)
   {
      m_nFirstRow = m_nNumRows - m_nMaxEntirelyVisibleRows;
      if (m_nFirstRow < 0) m_nFirstRow = 0;
   }
   
   UpdateVScroller();
   
   // Definitely remove item
   
   m_nNumRows--;
   ASSERT(m_nNumRows >= m_nNumSelRows);
   
   // Update focus
   
   if (m_nFocusedRow > nRow) 
   {
      m_nFocusedRow--;
   }
   else if (m_nFocusedRow == nRow)
   {
      if (bUpdateFocus)
      {
         // Set focus on the previous item
         if (m_nFocusedRow == m_nNumRows) m_nFocusedRow--;
         if (m_nFocusedRow > -1)
         {
            p = &m_aCols[0].m_aItems[m_nFocusedRow];
            if (!(p->IsSelected())) m_nNumSelRows++;
            p->SelectAndFocus();
         }
      }
      else m_nFocusedRow = -1;
   }
   
   ASSERT(m_nNumRows >= m_nNumSelRows);
   m_nFirstSelRow = (m_nFocusedRow==-1) ? 0 : m_nFocusedRow;
   
   // Redraw
   
   EnableDrawing();
   Redraw();
   
   // Notify parent of a selection change
   
   if (bSelChanged) Notify(WSF_LVN_SELCHANGED);
}

//*****************************************************************************
void CWsfTableCtrl::DeleteAllRows()
{
   if (!GetRowCount()) return;

   // Delete rows
   
   m_aRowData.RemoveAll();

   for (int t=0; t<m_aCols.GetSize(); t++)
   {
      m_aCols[t].m_aItems.RemoveAll();
   }
   
   // Init state
   
   m_nNumRows = 0;
   m_nFirstSelRow = 0;
   m_nFocusedRow = -1;
   m_nNumSelRows = 0;
   m_nFirstRow = 0;
   
   UpdateVScroller();
   
   // Notify parent of a selection change
   
   Notify(WSF_LVN_SELCHANGED);
}

//*****************************************************************************
void CWsfTableCtrl::SwapRows(int nRow1, int nRow2)
{
   CWsfTableCtrl::CItem lvi;
   
   ASSERT(nRow1 >=0 && nRow1 < m_nNumRows);
   ASSERT(nRow2 >=0 && nRow2 < m_nNumRows);
   
   // Swap
   
   for (int i=m_aCols.GetSize(); i--;)
   {
      lvi.CopyWithState(m_aCols[i].m_aItems[nRow1]);
      m_aCols[i].m_aItems[nRow1].CopyWithState(m_aCols[i].m_aItems[nRow2]);
      m_aCols[i].m_aItems[nRow2].CopyWithState(lvi);
   }

   CWslPtr<CObject> p;
   p = m_aRowData[nRow1];
   m_aRowData[nRow1] = m_aRowData[nRow2];
   m_aRowData[nRow2] = p;
   
   // Update focus, first selected item
   
   if (nRow1 == m_nFocusedRow) 
      m_nFocusedRow = nRow2;
   else if (nRow2 == m_nFocusedRow) 
      m_nFocusedRow = nRow1;
   
   m_nFirstSelRow = (m_nFocusedRow==-1) ? 0 : m_nFocusedRow;
   
   // Redraw items
   
   DrawRow(nRow1);
   DrawRow(nRow2);
}

/*****************************************************************************/   
CWslPtr<CObject> CWsfTableCtrl::GetRowData(int nRowIdx) const
{
   ASSERT(nRowIdx >= 0 && nRowIdx < m_nNumRows);
   return m_aRowData[nRowIdx];
}

/*****************************************************************************/
void CWsfTableCtrl::SetRowData(int nRowIdx, CWslPtr<CObject> pRowData)
{
   ASSERT(nRowIdx >= 0 && nRowIdx < m_nNumRows);
   m_aRowData[nRowIdx] = pRowData;
}

/*****************************************************************************
*
* ITEMS
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl::SetItem(int nRow, int nCol, 
                            const char* pcszText,
                            COLORREF rgbColor /*= -1*/,
                            BOOL bRedraw /*= false*/)
{
   ASSERT(nCol >= 0 && nCol < m_aCols.GetSize());
   ASSERT(nRow >= 0 && nRow < m_nNumRows);
   ASSERT(pcszText);
   
   CWsfTableCtrl::CItem& lvi = m_aCols[nCol].m_aItems[nRow];
   
   lvi.ClearContent();
   lvi.sText = pcszText;
   if (rgbColor != -1) lvi.rgbColor = rgbColor;
   
   if (bRedraw) DrawRow(nRow);
}

//*****************************************************************************
void CWsfTableCtrl::SetItem(int nRow, 
                            int nCol, 
                            CWsfTableCtrl::CItem* pItem, 
                            BOOL bRedraw /*= false*/)
{
   ASSERT(pItem);
   ASSERT(nCol >= 0 && nCol < m_aCols.GetSize());
   ASSERT(nRow >= 0 && nRow < m_nNumRows);
   
   m_aCols[nCol].m_aItems[nRow] = *pItem;
   
   if (bRedraw) DrawRow(nRow);
}


//*****************************************************************************
int CWsfTableCtrl::FindItem(int nCol, DWORD dwParam)
{
   ASSERT(nCol >=0 && nCol < m_aCols.GetSize());
   CWsfTableCtrl::CItem *p = m_aCols[nCol].m_aItems.GetData();
   
   for (int i=m_nNumRows; i--; p++) 
      if (p->dwParam == dwParam) return m_nNumRows - i - 1;
      
      return -1;
}

//*****************************************************************************
DWORD CWsfTableCtrl::GetItemData(int nRow, int nCol) const
{
   ASSERT(nRow >=0 && nRow < m_nNumRows);
   ASSERT(nCol >=0 && nCol < m_aCols.GetSize());
   return m_aCols[nCol].m_aItems[nRow].dwParam;
}

//*****************************************************************************
void CWsfTableCtrl::SetItemData(int nRow, int nCol, DWORD dwParam)
{
   ASSERT(nRow >=0 && nRow < m_nNumRows);
   ASSERT(nCol >=0 && nCol < m_aCols.GetSize());
   m_aCols[nCol].m_aItems[nRow].dwParam = dwParam;
}

//*****************************************************************************	
void CWsfTableCtrl::SetItemImage(int nRow, int nCol, int nImg)
{
   ASSERT(nRow>=0 && nRow<GetRowCount());
   ASSERT(nImg >= -1);
   ASSERT(nCol>=0 && nCol<m_aCols.GetSize());
   
   m_aCols[nCol].m_aItems[nRow].nImageIdx = nImg;
   DrawRow(nRow);
}

/*****************************************************************************/
const CWsfTableCtrl::CItem* CWsfTableCtrl::GetItem(int nRow, const char* pcszColTitle) const
{
   int nCol = GetColIdx(pcszColTitle);
   if (nCol == -1) { ASSERT(false); return NULL; }
   return &GetItem(nRow, nCol);
}

/*****************************************************************************
*
* SORTING
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl::SortRows()
{
   ASSERT(false);
/*   CWsfTableCtrl::CItem lvi;
   BOOL bSwap;	
   
   if (!(m_dwFlags & WSF_LVF_INTERNAL_SORTING)) return;
   
   CWslArray<CWsfTableCtrl::CItem>& aItems = m_aCols[m_nSortCol].m_aItems;
   int nItemType = m_aCols[m_nSortCol].nItemType;
   
   for (int i=0; i<m_nNumRows; i++)
   {
      for (int t=i; t<m_nNumRows; t++)
      {
         switch (nItemType)
         {
         case WSF_LVCIT_TEXT:
            bSwap = strcmp(aItems[i].sText, aItems[t].sText) > 0;
            break;
            
         case WSF_LVCIT_IMAGE:
            bSwap = aItems[i].nImageIdx > aItems[t].nImageIdx;
            break;
            
         case WSF_LVCIT_COLOR:
            bSwap = aItems[i].rgbColor > aItems[t].rgbColor;
            break;
            
         default:
            ASSERT(false);
            break;
         }
         
         if (bSwap)
         {
            lvi = aItems[i];
            aItems[i] = aItems[t];
            aItems[t] = lvi;
         }
      }
   }*/
   
   RedrawWindow();
}


/*****************************************************************************
*
* SELECTION
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTableCtrl::IsRowSelected(int nItem)
{
   ASSERT(nItem >=0 && nItem < m_nNumRows);
   return m_aCols[0].m_aItems[nItem].IsSelected();
}

//*****************************************************************************
POSITION CWsfTableCtrl::GetFirstSelectedRowPosition()
{
   for (int i=0; i < m_nNumRows; i++)
   {
      if (m_aCols[0].m_aItems[i].IsSelected())
         return (POSITION) i+1;
   }
   return NULL;
}

//*****************************************************************************
int	CWsfTableCtrl::GetNextSelectedRow(POSITION& pos)
{
   int n,i;
   ASSERT(pos);
   n = (int)pos - 1;
   
   for (i=(int)pos-1+1; i < m_nNumRows; i++)
   {
      if (m_aCols[0].m_aItems[i].IsSelected())
      {
         pos = (POSITION) (i+1);
         return n;
      }
   }
   
   pos = NULL;
   return n;
}

//*****************************************************************************
void CWsfTableCtrl::SelectSingleRow(int nRowIdx, BOOL bEnsureVisible)
{
   ASSERT(nRowIdx >=-1 && nRowIdx < m_nNumRows);
   SelectRows(nRowIdx, nRowIdx, true, false, true);
   
   // Ensure visible
   
   if (nRowIdx != -1 && bEnsureVisible) 
   {
      if (nRowIdx < m_nFirstRow || nRowIdx >= m_nFirstRow+m_nMaxEntirelyVisibleRows)
      {
         // Try to display the item in the middle of the table
         int n = min(
            max(0, nRowIdx - m_nMaxEntirelyVisibleRows/2), 
            max(m_nNumRows - m_nMaxEntirelyVisibleRows, 0)
            );
         
         OnVScroll(SB_THUMBPOSITION, n, NULL);
      }
   }
}

//*****************************************************************************
void CWsfTableCtrl::SelectAllRows()
{
   if ((m_dwFlags & WSF_LVF_SINGLE_SELECTION) || !m_nNumRows) return;
   
   // Manually reset the selected items for speed
   m_nNumSelRows = 0;
   
   SelectRows(0, m_nNumRows-1, false, false, true);
}

//*****************************************************************************
void CWsfTableCtrl::Push()
{
   m_nFirstRowBak = m_nFirstRow;
   m_nFocusedRowBak = m_nFocusedRow;
}

//*****************************************************************************
void CWsfTableCtrl::Pop()
{
   // Restore first visible item and scroll to it
   
   if (m_nFirstRow != m_nFirstRowBak)
   {
      m_nFirstRow = m_nFirstRowBak;
      UpdateVScroller();
      //OnVScroll(SB_THUMBPOSITION, m_nFirstRowBak, 0);		
   }
   
   Redraw();
   
   // Reselect and focus item
   
   if (m_nFocusedRowBak == -1 && m_nNumRows) 
      m_nFocusedRowBak = 0;
   else if (m_nFocusedRowBak >= m_nNumRows) 
      m_nFocusedRowBak = m_nNumRows - 1;
   
   SelectRows(m_nFocusedRowBak, m_nFocusedRowBak, true, false, true);
}

//*****************************************************************************
void CWsfTableCtrl::SetSortColumn(int nCol, BOOL bSortAsc, BOOL bNotify)
{
   DWORD x;
   HDITEM hdi;
   
   // Update sort order or remove sort icon if sort column has changed
   
   if (m_nSortCol != nCol)
   {
      if (m_nSortCol != -1 && m_aCols[m_nSortCol].nImageIdx==-1)
      {
         hdi.mask = HDI_FORMAT;
         hdi.fmt = m_aCols[m_nSortCol].GetHdrCtrlAlign() | HDF_STRING;
         m_HdrCtrl.SetItem(m_aCols[m_nSortCol].nHdrCtrlCol, &hdi);
      }
      m_nSortCol = nCol;
   }
   else if (m_bSortAsc == bSortAsc) return;
   
   if (nCol == -1) return;
   ASSERT(m_aCols[nCol].dwFlags & CColumn::SORTABLE);
   m_bSortAsc = bSortAsc;
   
   // Set sort icon on the sort column. 
   // If it's a icon column -> has only icon, do not set the icon.
   // Set only if the bidirectional sorting is enabled.
   
   if (m_aCols[m_nSortCol].nImageIdx==-1 && m_aCols[m_nSortCol].IsBidiSorting())
   {
      hdi.mask = HDI_FORMAT | HDI_BITMAP;
      hdi.hbm = m_bSortAsc ? m_hArrowUp : m_hArrowDown;
      x = m_aCols[nCol].GetHdrCtrlAlign();
      hdi.fmt = x | (x==HDF_RIGHT ? 0:HDF_BITMAP_ON_RIGHT) | HDF_BITMAP | HDF_STRING;
      
      m_HdrCtrl.SetItem(m_aCols[nCol].nHdrCtrlCol, &hdi);
   }
   
   // Notify parent window
   
   if (bNotify) Notify(WSF_LVN_SORTINGCHANGED);
   if (m_dwFlags & WSF_LVF_INTERNAL_SORTING) SortRows();
   
   // End
   
   //TRACE2("CWsfTableCtrl: Sorting has changed: %d, asc: %d\n", nCol, m_bSortAsc);
}

//*****************************************************************************
void CWsfTableCtrl::RecalcColumnLayout()
{
   if (!m_aColsOrder.GetSize()) return;
//   m_bNoHdrNotify = true;
   
   // Get width of stretchable columns and non-stretchable columns
   // Get the last stretchable column

   int nFixedW = 0;
   int nStretchW = 0;
   int nLastStretchColIdx = -1;

   for (int i=0; i<m_aColsOrder.GetSize(); i++)
   {
      CColumn& c = m_aCols[m_aColsOrder[i]];
      
      if (c.IsStretchable()) 
      {
         nStretchW += c.nWidth;
         nLastStretchColIdx = i;
      }
      else nFixedW += c.nWidth;
   }

   // Get scale factor for stretching the witdh of stretchable columns

   double dScale = (double)(m_nWW - nFixedW) / (double)nStretchW;
   if (dScale < 0.0) dScale = 0.0;

   // Calculate the new width of columns
   // - Fixed columns have their fixed width
   // - Stretchable columns are stretched by the scale factor.
   //   Minimum width is forced.
   // - The width of the last strechable column is adjusted to fix the
   //   rounding error when converting a float width to an int width

   int nRealStretchW = 0;

   for (i=0; i<m_aColsOrder.GetSize(); i++)
   {
      CColumn& c = m_aCols[m_aColsOrder[i]];
      HDITEM hdi;
      hdi.mask = HDI_WIDTH;

      if (c.IsStretchable()) 
      {
         int n = (int) (c.nWidth * dScale);

         if (i==nLastStretchColIdx && (nStretchW - nRealStretchW > 0))
         {
            n = m_nWW - nRealStretchW - nFixedW;
         }

         hdi.cxy = n > WSF_LV_MIN_COL_WIDTH ? n : WSF_LV_MIN_COL_WIDTH;
         nRealStretchW += hdi.cxy;
      }
      else hdi.cxy = c.nWidth;

      c.nDisplayWidth = hdi.cxy;
      m_HdrCtrl.SetItem(c.nHdrCtrlCol, &hdi);
   }

   // Done
   
   Redraw();
//   m_bNoHdrNotify = false;
}

//*****************************************************************************
void CWsfTableCtrl::InvertTracker(int nX)
{
   PatBlt(m_hDC, nX , m_nHH, 1, m_nWH - m_nHH, PATINVERT);
}

//*****************************************************************************
BOOL CWsfTableCtrl::OnBeginTrack(int nColOrderIdx)
{
   if (!m_aCols[m_aColsOrder[nColOrderIdx]].IsResizable()) return false;

   // Calculate the minimum width of all columns
   
   int nMinW = 0;

   for (int i=0; i<m_aColsOrder.GetSize(); i++) 
   {
      if (i == nColOrderIdx) continue;

      CColumn& c = m_aCols[m_aColsOrder[i]];
      
      if (c.IsStretchable()) 
         nMinW += WSF_LV_MIN_COL_WIDTH;
      else 
         nMinW += c.nDisplayWidth;
   }
  
   // Get column left and right X coordinate
   
   m_nBLX = 0;
   
   for (i=0; i<nColOrderIdx; i++) 
   {
      m_nBLX += m_aCols[m_aColsOrder[i]].nDisplayWidth; 
   }

   m_nBRX = m_nBLX + m_aCols[m_aColsOrder[nColOrderIdx]].nDisplayWidth;

   // Get the maximum column width

   m_nMaxW = m_nWW - nMinW;
   
   // Draw bars

   InvertTracker(m_nBLX);
   InvertTracker(m_nBRX);
 
   return true;
}

/*****************************************************************************/
void CWsfTableCtrl::OnEndTrack(int nColOrderIdx, int nNewWidth)
{
   // Remove bars

   InvertTracker(m_nBRX);
   InvertTracker(m_nBLX);

   CColumn& c = GetColumnByOrder(nColOrderIdx);

   // Get the width of all stretchable columns behind the column being resized

   int nStretchColsBehindW = 0;

   for (int i=nColOrderIdx+1; i < GetVisibleColumnsCount(); i++)
   {
      CColumn& c = GetColumnByOrder(i);
      if (c.IsStretchable()) nStretchColsBehindW += c.nWidth;
   }

   // Resize the column. 
   // General rule: columns before the column being changed stay unchanged.

   if (c.IsStretchable())
   {
      // Change the width of the columns 
      // We change the width by the same factor as the display width was changed
      
      int nNewW = (int) (c.nWidth * ((double)nNewWidth / c.nDisplayWidth));
      int nDeltaW  = nNewW - c.nWidth;
      c.nWidth = nNewW;

      // Keep the total width of stretchable columns the same as before
      // the change -> distribute proportionally the width change to the columns
      // behind the column being resized -> columns before will be unchanged, 
      // columns behind will be resized

      for (int i=nColOrderIdx+1; i < GetVisibleColumnsCount(); i++)
      {
         CColumn& c = GetColumnByOrder(i);
      
         if (c.IsStretchable()) 
         {
            c.nWidth -= (int) (((double)c.nWidth / nStretchColsBehindW) * nDeltaW);
            if (c.nWidth <  WSF_LV_MIN_COL_WIDTH) c.nWidth = WSF_LV_MIN_COL_WIDTH;
         }
      }
   }

   else

   {
      // Get width of all non-stretchable columns

      int nFixedW = 0;

      for (int i=0; i < GetVisibleColumnsCount(); i++)
      {
         CColumn& c = GetColumnByOrder(i);
         if (!c.IsStretchable()) nFixedW += c.nWidth;
      }
     
      // Get the width of display area for all stretchable columns
      // Get the change of this width (casused by the sizing of a non-stretchable column)
      // Get the factor of the change

      int nDisplayStretchW = m_nWW - nFixedW;
      if (nDisplayStretchW < 0) nDisplayStretchW = 0;
      int nDeltaW  = nNewWidth - c.nWidth;
      double f = (double)nDisplayStretchW / (nDisplayStretchW - nDeltaW);
      ASSERT(f > 0);

      // Set new width of the column being changed

      c.nWidth = nNewWidth;

      // Update the width of all stretchable columns before the column being changed
      // We update the width for the display width to stay the same after the 
      // total display width has change -> columns stay unchanged.
      // We also calculate the total width change.
      //
      // Display widths before and after the change must stay the same:
      // W(t) * TotalDispW/TotalStretchW = W(t+1) * (TotalDispW-TotalDispWDelta)/TotalStretchW

      nDeltaW = 0;

      for (i=0; i < nColOrderIdx; i++)
      {
         CColumn& c = GetColumnByOrder(i);
         
         if (c.IsStretchable()) 
         {
            int n = (int) (c.nWidth * f);
            nDeltaW += n - c.nWidth;
            c.nWidth = n;
         }
      }

      // Now we must fix the total width of all stretchable columns.
      // It must be the same as before the change (see the formula above)
      // We distribute proportionally the change to all columns behind the column being changed.

      for (i=nColOrderIdx+1; i < GetVisibleColumnsCount(); i++)
      {
         CColumn& c = GetColumnByOrder(i);
         
         if (c.IsStretchable()) 
         {
            c.nWidth -= (int) (((double)c.nWidth / nStretchColsBehindW) * nDeltaW);
            if (c.nWidth <  WSF_LV_MIN_COL_WIDTH) c.nWidth = WSF_LV_MIN_COL_WIDTH;
         }
      }
   }

   // Done

   RecalcColumnLayout();
   Notify(WSF_LVN_COL_WIDTH_CHANGED);
}

//*****************************************************************************
void CWsfTableCtrl::OnHeaderContextMenu(BOOL bOnParticularColumn)
{
   // Get clicked column and menu position
   
   POINT pt;
   m_nCurCol = -1;
   
   if (bOnParticularColumn)
   {
      GetCursorPos(&pt);
      ScreenToClient(&pt);
      
      int x = 0;
      for (int i=0; i < m_aColsOrder.GetSize(); i++) 
      {
         x += (int) m_aCols[m_aColsOrder[i]].nDisplayWidth;
         if (x > pt.x) { m_nCurCol = m_aColsOrder[i]; break; }
      }
   }
   else
   {
      pt.x = 0;
      pt.y = 0;
   }
   
   ClientToScreen(&pt);	
   
   // Get menu
   
   HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENUS));
   ASSERT(hMenu);
   HMENU hSubMenu = GetSubMenu(hMenu, 0);
   ASSERT(hSubMenu);
   
   // Append column names
   
   MENUITEMINFO mii;
   mii.cbSize = sizeof(mii);
   mii.fMask = MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_FTYPE;
   mii.fType = MFT_STRING;
   
   for (int i=0; i < m_aCols.GetSize(); i++)
   {
      mii.fState = 0;
      mii.wID = i+1;
      mii.dwTypeData = (char*) (const char*) m_aCols[i].sTitle;
      if (m_aCols[i].IsVisible()) mii.fState |= MFS_CHECKED;
      if (m_aCols.GetSize() == 1 || (m_aColsOrder.GetSize()==1 && m_aColsOrder[0]==i)) mii.fState |= MF_GRAYED;
      if (i==m_nCurCol) mii.fState |= MFS_DEFAULT;
      
      InsertMenuItem(hSubMenu, i+1, false, &mii);
   }
   
   // Disable commands if needed
   
   EnableMenuItem(hSubMenu, ID_SORT_DESC, MF_BYCOMMAND | 
      (m_nCurCol!=-1 && (m_aCols[m_nCurCol].IsSortable()) ? MF_ENABLED:MF_GRAYED) );
   EnableMenuItem(hSubMenu, ID_SORT_ASC, MF_BYCOMMAND | 
      (m_nCurCol!=-1 && (m_aCols[m_nCurCol].IsSortable()) ? MF_ENABLED:MF_GRAYED) );
   EnableMenuItem(hSubMenu, ID_REMOVE_COLUMN, MF_BYCOMMAND | 
      (m_nCurCol!=-1 && m_aColsOrder.GetSize() > 1 ? MF_ENABLED:MF_GRAYED) );
   
   // Display context menu
   
   int n = TrackPopupMenu(hSubMenu, 
      TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,
      pt.x, pt.y, 0, *this, NULL);
   DestroyMenu(hMenu);
   
   // Handle command
   
   if (n)
   {
      if (n < 256)
      {
         n--;
         if (IndexToOrder(n) == -1)
         {
            ActivateColumn(n, m_aColsOrder.GetSize());
            Notify(WSF_LVN_COLUMN_ADDED);
         }
         else
         {
            DeactivateColumn(n);
            Notify(WSF_LVN_COLUMN_REMOVED);
         }
      }
      else
      {
         OnCmdMsg(n, CN_COMMAND, NULL, NULL);
      }
   }
}

//*****************************************************************************
BOOL CWsfTableCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   int n, i;
   
   if (m_bNoHdrNotify) return true;
   m_bNoHdrNotify = true;
   
   NMHEADER &s = *(NMHEADER*)lParam;
   
   switch (((NMHDR*)lParam)->code)
   {
   case HDN_BEGINTRACK:
      *pResult = !OnBeginTrack(HdrIndexToOrder(s.iItem));
      break;
      
   case HDN_TRACK:
      n = s.pitem->cxy;
      
      if (n < WSF_LV_MIN_COL_WIDTH) 
         n = WSF_LV_MIN_COL_WIDTH; 
      else if (n > m_nMaxW) 
         n = m_nMaxW;
      
      InvertTracker(m_nBRX);
      m_nBRX = n + m_nBLX;
      InvertTracker(m_nBRX);
      s.pitem->cxy = n;
      break;
      
   case HDN_ENDTRACK:
      OnEndTrack(HdrIndexToOrder(s.iItem), s.pitem->cxy);
      *pResult = true;
      break;
      
   case NM_RCLICK:
      OnHeaderContextMenu(true);
      break;
      
   case HDN_ITEMCLICK:
      n = m_aColsOrder[HdrIndexToOrder(s.iItem)];
      if (m_aCols[n].IsSortable()) 
         SetSortColumn(n, n == m_nSortCol ? m_bSortAsc^1 : true, true);
      break;
      
   case HDN_ENDDRAG:
      if (s.pitem->iOrder != -1)
      {
         n = HdrIndexToOrder(s.iItem);
         i = m_aColsOrder[n];
         m_aColsOrder.RemoveAt(n);
         m_aColsOrder.InsertAt(s.pitem->iOrder, i);
         Redraw();
         Notify(WSF_LVN_COL_ORDER_CHANGED);
      }
      break;
      
   default:
      m_bNoHdrNotify = false;
      return CWnd::OnNotify(wParam, lParam, pResult);
   }
   
   m_bNoHdrNotify = false;
   return true;
}

//*****************************************************************************
void CWsfTableCtrl::Notify(int nCode, CWsfTableCtrl::CNotifyInfo* p /*= NULL*/)
{
   CWsfTableCtrl::CNotifyInfo nmlv;
   
   if (!p) p = &nmlv;
   
   p->hdr.code = nCode;
   p->hdr.hwndFrom = *this;
   p->hdr.idFrom = GetWindowLong(m_hWnd, GWL_ID);
   
   ::SendMessage(::GetParent(*this), WM_NOTIFY, (WPARAM) 0, (LPARAM) p);
}

/*****************************************************************************
*
* LAYOUT PERSISTENCE
*
*****************************************************************************/

#define LAYOUT_VERSION 1

//*****************************************************************************
BOOL CWsfTableCtrl::SetLayout(CWsfTableCtrl::CLayout& l)
{
   // Only valid layout
   // The number of columns must be the same
   
   if (!l.IsValid() || l.aColWidths.GetSize() != m_aCols.GetSize()) 
   {
      ActivateDefaultColumns();
      return false;
   }
   
   // If the column has an icon, its width must be 16. 
   // If it's not, data are corupted -> don't accept them.
   
   for (int i=0; i<m_aCols.GetSize(); i++) 
   {
      if (m_aCols[i].nImageIdx == -1)
      {
         ASSERT(l.aColWidths[i]);
         m_aCols[i].nWidth = l.aColWidths[i];
      }
   }
   
   DeactivateAllColumns();
   for (i=0; i<l.aColsOrder.GetSize(); i++) 
      ActivateColumn(l.aColsOrder[i], -1, false, false);
   
   SetSortColumn(l.nSortCol, l.bSortAsc, false);
   RecalcColumnLayout();
   
   return true;
}

//*****************************************************************************
void CWsfTableCtrl::GetLayout(CWsfTableCtrl::CLayout& l)
{
   l.aColsOrder = m_aColsOrder;
   l.nSortCol = m_nSortCol;
   l.bSortAsc = m_bSortAsc;
   
   for (int i=0; i<m_aCols.GetSize(); i++) 
   {
      ASSERT(m_aCols[i].nWidth);
      l.aColWidths += m_aCols[i].nWidth;
   }
}

/*****************************************************************************/
void CWsfTableCtrl::CLayout::operator=(const CLayout& l)
{
   aColsOrder = l.aColsOrder;
   aColWidths = l.aColWidths;
   nSortCol = l.nSortCol;
   bSortAsc = l.bSortAsc;
}

/*****************************************************************************/
void CWsfTableCtrl::CLayout::Serialize(CArchive& ar)
{
   if (ar.IsLoading())
   {
      int nVer;
      ar >> nVer;
      if (nVer != LAYOUT_VERSION) throw new CWslTextException(IDS_BAD_TABLE_LAYOUT_VERSION);

      ar >> nSortCol;
      ar >> bSortAsc;
   }
   else
   {
      ar << (int) LAYOUT_VERSION;
      ar << nSortCol;
      ar << bSortAsc;
   }

   CObject::Serialize(ar);
   aColsOrder.Serialize(ar);
   aColWidths.Serialize(ar);
}

/*****************************************************************************/
BOOL CWsfTableCtrl::CLayout::LoadFromRegistry(const char* pcszKey)
{
   int n = WsfGetApp()->GetProfileInt(pcszKey, "Version", 1);
   if (n != LAYOUT_VERSION) return false;

   nSortCol = WsfGetApp()->GetProfileInt(pcszKey, "Sorting Column", 0);
   bSortAsc = WsfGetApp()->GetProfileInt(pcszKey, "Sorting Dir", true);
   
   CString s = WsfGetApp()->GetProfileString(pcszKey, "Columns Order");
   CWslStringArray a;
   a.ExplodeString(s, ",");
   aColsOrder.RemoveAll();
   for (int i=0; i<a.GetSize(); i++) aColsOrder += atoi(a[i]);
   
   s = WsfGetApp()->GetProfileString(pcszKey, "Columns Widths");
   a.ExplodeString(s, ",");
   aColWidths.RemoveAll();
   for (i=0; i<a.GetSize(); i++) aColWidths += atoi(a[i]);

   return true;
}

/*****************************************************************************/
void CWsfTableCtrl::CLayout::SaveToRegistry(const char* pcszKey)
{
   WsfGetApp()->WriteProfileInt(pcszKey, "Version", LAYOUT_VERSION);

   WsfGetApp()->WriteProfileInt(pcszKey, "Sorting Column", nSortCol);
   WsfGetApp()->WriteProfileInt(pcszKey, "Sorting Dir", bSortAsc);
   
   // Order

   CString s;
   
   for (int i=0; i<aColsOrder.GetSize(); i++)
   {
      if (i>0) s += ',';
      s += itoa(aColsOrder[i], temp, 10);
   }
   
   WsfGetApp()->WriteProfileString(pcszKey, "Columns Order", s);
   
   // Widths

   s.Empty();
   
   for (i=0; i<aColWidths.GetSize(); i++)
   {
      if (i>0) s += ',';
      sprintf(temp,"%d", aColWidths[i]);
      s += temp;
   }
   
   WsfGetApp()->WriteProfileString(pcszKey, "Columns Widths", s);
}

/*****************************************************************************/
void CWsfTableCtrl::CLayout::LoadFromFile(const CString& sPathName) throw (CException*)
{
   CFile f(sPathName, CFile::modeRead);
   CArchive ar(&f, CArchive::load);

   Serialize(ar);

   ar.Close();
	f.Close();
}

/*****************************************************************************/
void CWsfTableCtrl::CLayout::SaveToFile(const CString& sPathName) throw (CException*)
{
   CFile f(sPathName, CFile::modeCreate|CFile::modeWrite);
   CArchive ar(&f, CArchive::store);

   Serialize(ar);

   ar.Close();
	f.Close();
}

/*****************************************************************************
*
* DRAWING
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl::Redraw()
{
   RECT r;
   if (!m_hWnd) return;
   
   //TRACE0("CWsfTableCtrl::Redraw()\n");
   
   r.top = m_nHH;
   r.bottom = m_nWH;
   r.left = 0;
   r.right = m_nWW;
   
   InvalidateRect(&r);
}

//*****************************************************************************
void CWsfTableCtrl::OnPaint() 
{
   CPaintDC dc(this);
   int nY, nFirst, nLast;
   RECT r;
   
   if (m_nDontDrawCounter) return;

   // Check special case when there are no items in the list
   
   if (!m_nNumRows)
   {
      CString s;
      s.LoadString(IDS_NO_ITEMS_IN_THE_LIST);
      
      SetTextColor(m_hDC, ::GetSysColor(COLOR_BTNSHADOW));
      SelectObject(m_hDC, m_hFont);
      
      r.top = m_nHH;
      r.bottom = m_nWH;
      r.left = 0;
      r.right = m_nWW;
      
      FillRect(m_hDC, &r, m_hWindowBackgndStockBrush);
      DrawText(m_hDC, s, s.GetLength(), &r, DT_END_ELLIPSIS | DT_SINGLELINE | DT_CENTER);
      return;
   }
   
   // Only rows in the paint rectangle are drawn.
   
   if (dc.m_ps.rcPaint.bottom < m_nHH) return;
   
   nY = dc.m_ps.rcPaint.top;
   if (nY < m_nHH) nY = m_nHH;
   nFirst = (nY - m_nHH) / m_nRH;
   nY = nFirst * m_nRH + m_nHH;
   nFirst += m_nFirstRow;
   nLast = (dc.m_ps.rcPaint.bottom - m_nHH + m_nRH - 1) / m_nRH + m_nFirstRow;
   if (nLast >= m_nNumRows) nLast = m_nNumRows - 1;
   
   //TRACE0("---------------------------------------\n");
   //TRACE2("WsfTableCtrl::OnPaint(): %d, %d\n", nFirst, nLast);
   ASSERT(nFirst >=0);
   
   // Draw items
   
   for (int i = nFirst; i<=nLast; i++)
   {
      DrawRow(i, nY);
      nY += m_nRH;
   }
   
   // Clear the rest of the paint rectangle
   
   if (nY < dc.m_ps.rcPaint.bottom)
   {
      r.top = nY;
      r.bottom = dc.m_ps.rcPaint.bottom;
      r.left = 0;
      r.right = m_nWW;
      FillRect(m_hDC, &r, m_hWindowBackgndStockBrush);
   }
}

//*****************************************************************************
void CWsfTableCtrl::DrawRow(int nItem, int nY)
{
   if (m_nDontDrawCounter) return;
   
   // Init
   
   RECT r;
   r.top = nY;
   r.bottom = nY + m_nRH;
   r.left = 0;
   r.right = m_nWW;
   
   CItem& item = m_aCols[0].m_aItems[nItem];
   item.ClearRedraw();
   
   int nLX = 0;
   BOOL bNotSelectedOrUnactive = !(item.IsSelected()) || !(m_dwFlags & WSF_LVF_FOCUSED);
   
   // Draw background in the selection color, if the item selected
   
   if (item.IsSelected())
   {
      int n = m_dwFlags & WSF_LVF_FOCUSED ? COLOR_ACTIVECAPTION : COLOR_3DFACE;
      ::FillRect(m_hDC, &r, (HBRUSH) (n+1));
      ::SetBkColor(m_hDC, ::GetSysColor(n));
   }
   
   // Draw columns
   
   for (int i=0, n=m_aColsOrder.GetSize(); i<n; i++)
   {
      const CWsfTableCtrl::CColumn& lvc = m_aCols[m_aColsOrder[i]];
      const CWsfTableCtrl::CItem& lvi = lvc.m_aItems[nItem];
      int nRX = nLX + lvc.nDisplayWidth;
      
      if (!(item.IsSelected()))
      {
         r.right = nRX;
         r.left = nLX;
         
         HBRUSH h;
         
         if (lvi.rgbBackground == -1)
         {
            if (m_aColsOrder[i] == m_nSortCol && (m_dwFlags & WSF_LVF_HIGHLIGHT_SORTING_COLUMN)) 
               h = m_hSortColumnBackgndBrush; 
            else
               h = m_hWindowBackgndStockBrush;
         }
         else h = ::CreateSolidBrush(lvi.rgbBackground);
         
         ::FillRect(m_hDC, &r, h);
         ::SetBkColor(m_hDC, lvi.rgbBackground);

         if (lvi.rgbBackground != -1) ::DeleteObject(h);
      }
      
      if (lvi.nImageIdx != -1)
      {
         VERIFY(ImageList_Draw(m_hImageList, lvi.nImageIdx, m_hDC,
            nLX+2, nY + (m_nRH-16)/2, ILD_TRANSPARENT));
         nLX += 2+16+2;
      }
      
      if (!lvi.sText.IsEmpty())
      {
         r.right = nRX - 4;
         r.left = nLX + 4;
         
         ::SetTextColor(m_hDC, bNotSelectedOrUnactive ? lvi.rgbColor : RGB(255,255,255));
         ::SelectObject(m_hDC, lvi.bBold ? m_hBoldFont : m_hFont);
         
         ::DrawText(m_hDC, lvi.sText, lvi.sText.GetLength(), &r, 
            DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER | DT_SINGLELINE | lvc.GetDrawTextAlign());
      }
      
      if (lvi.bRectangle)
      {
         HGDIOBJ ohbr = ::SelectObject(m_hDC, CreateSolidBrush(lvi.rgbColor));
         ::SelectObject(m_hDC, m_hBlackPen);
         ::Rectangle(m_hDC, nLX+4, r.top+1, nRX-4, r.bottom-1);
         ::DeleteObject(SelectObject(m_hDC, ohbr));
      }
      
      nLX = nRX;
   }
   
   // Erase the rest of the line occupied by no column
   
   if (!(item.IsSelected()) && nLX < m_nWW)
   {
      r.right = m_nWW;
      r.left = nLX;
      ::FillRect(m_hDC, &r, m_hWindowBackgndStockBrush);
   }
   
   // Draw focus rectangle
   
   if ((m_dwFlags & WSF_LVF_FOCUSED) && item.IsFocused()) 
   {
      r.left = 0;
      r.right = m_nWW;
      ::SetBkColor(m_hDC, RGB(255,255,255));
      ::SetTextColor(m_hDC, RGB(0,0,0));
      ::DrawFocusRect(m_hDC, &r);
   }
}

//*****************************************************************************
void CWsfTableCtrl::DrawRow(int nItem)
{
   if (m_nDontDrawCounter) return;
   
   int n = nItem - m_nFirstRow;
   if (n < 0) return;
   n = n * m_nRH + m_nHH;
   if (n >= m_nWH) return;
   
   DrawRow(nItem, n);
}

//*****************************************************************************
void CWsfTableCtrl::UpdateVisualStyle()
{
   m_hWindowBackgndStockBrush = ::GetSysColorBrush(IsWindowEnabled() ? COLOR_WINDOW : COLOR_3DFACE);

   // Sorting column background

   COLORREF c = GetSysColor(COLOR_3DFACE);
   
   c = RGB(
      (int)(GetRValue(c) + (255-GetRValue(c))*0.75),
      (int)(GetGValue(c) + (255-GetGValue(c))*0.75),
      (int)(GetBValue(c) + (255-GetBValue(c))*0.75)
      );

   m_hSortColumnBackgndBrush = ::CreateSolidBrush(c);
}

/*****************************************************************************
*
* SELECTION
*
*****************************************************************************/


//*****************************************************************************
void CWsfTableCtrl::RemoveSelection(int nFirst, int nLast)
{
   // Unselects all item except the items in the range <nFirst, nLast>
   
   if (!m_nNumSelRows) return;
   
   int n = m_nNumSelRows;
   m_nNumSelRows = 0;
   
   if (n != 1)
   {
      CWsfTableCtrl::CItem* plvi = m_aCols[0].m_aItems.GetData();
      
      for (int i=0; i<nFirst && n; i++,plvi++)
      {
         if (plvi->IsSelected())
         {
            plvi->Unselect();
            DrawRow(i);
            if (!n--) return;
         }
      }
      
      plvi = m_aCols[0].m_aItems.GetData() + (nLast+1);
      
      for (i=nLast+1; i<m_nNumRows && n; i++,plvi++) 
      {
         if (plvi->IsSelected())
         {
            plvi->Unselect();
            DrawRow(i);
            if (!n--) return;
         }
      }
   }
   else
   {
      // Special but most propable case. It's optimized.
      
      CWsfTableCtrl::CItem* plvi = &m_aCols[0].m_aItems[m_nFirstSelRow];
      
      if (plvi->IsSelected())
      {
         plvi->Unselect();
         DrawRow(m_nFirstSelRow);
      }
   }
}

//*****************************************************************************
void CWsfTableCtrl::SelectRows(int nFirst, int nLast, BOOL bRemovePrevSel, BOOL bInvert, BOOL bSetFirstSelItem)
{
   CWsfTableCtrl::CItem *plvi, *pfold, *pfnew;
   int i, n, nNewFocusItem;
   
   // Update focus
   
   nNewFocusItem = nFirst;
   pfold = pfnew = NULL;
   
   if (m_nFocusedRow != nNewFocusItem)
   {
      if (m_nFocusedRow != -1)
      {
         pfold = &m_aCols[0].m_aItems[m_nFocusedRow];
         pfold->Blur();
         pfold->MarkRedraw();
      }
      if (nNewFocusItem != -1)
      {
         pfnew = &m_aCols[0].m_aItems[nNewFocusItem];
         pfnew->Focus();
         pfnew->MarkRedraw();
      }
   }
   
   // Remove selection. Doesn't affect the item range <nFirst,nLast>.
   
   if (nFirst > nLast) { n = nFirst; nFirst = nLast; nLast = n; }
   if (bRemovePrevSel) RemoveSelection(nFirst, nLast);
   
   // Change selection of the given item range in the given way.
   
   if (nFirst != -1)
   {
      plvi = &m_aCols[0].m_aItems[nFirst];
      
      if (bInvert)
      {
         for (n=0,i=nFirst; i<=nLast; i++,plvi++) 
         { 
            plvi->InvertSelection();
            if (plvi->IsSelected()) n++; else n--;
            DrawRow(i); 
         }
         m_nNumSelRows += n;
      }
      else
      {
         for (i=nFirst; i<=nLast; i++,plvi++) 
         {
            if (!plvi->IsSelected())
            {
               plvi->Select();
               DrawRow(i);
            }
         }
         m_nNumSelRows += nLast - nFirst + 1;
      }
   }
   
   // Check if old & new focus items have been redrawn
   
   if (pfnew && pfnew->IsMarkedRedraw()) DrawRow(nNewFocusItem);
   if (pfold && pfold->IsMarkedRedraw()) DrawRow(m_nFocusedRow);
   
   m_nFocusedRow = nNewFocusItem;
   if (bSetFirstSelItem) m_nFirstSelRow = (nFirst==-1 ? 0:nFirst);
   
   // Notify parent of a selection change
   
   Notify(WSF_LVN_SELCHANGED);
}

/*****************************************************************************
*
* WINDOW STATE
*
*****************************************************************************/


//*****************************************************************************
void CWsfTableCtrl::OnSetFocus(CWnd* pOldWnd) 
{
   CWnd::OnSetFocus(pOldWnd);
   m_dwFlags |= WSF_LVF_FOCUSED;
   Redraw();
}

//*****************************************************************************
void CWsfTableCtrl::OnKillFocus(CWnd* pNewWnd) 
{
   CWnd::OnKillFocus(pNewWnd);
   m_dwFlags &= ~(WSF_LVF_FOCUSED | WSF_LVF_CHECK_DRAG | WSF_LVF_CHECK_RDRAG);
   Redraw();
}

/*****************************************************************************/
void CWsfTableCtrl::OnEnable(BOOL bEnable) 
{
   UpdateVisualStyle();
   Redraw();
   CWnd::OnEnable(bEnable);
}

//*****************************************************************************
void CWsfTableCtrl::OnSize(UINT nType, int cx, int cy) 
{
   HDLAYOUT l;
   RECT r;
   WINDOWPOS wp;
   
   // Repos header ctrl
   
   r.bottom = cy;
   r.right = cx;
   r.left = r.top = 0;
   l.prc = &r;
   l.pwpos = &wp;
   
   m_HdrCtrl.Layout(&l);
   m_nHH = wp.cy;
   
   ::SetWindowPos(m_HdrCtrl, NULL, wp.x, wp.y, wp.cx, wp.cy, SWP_NOZORDER);
   
   // Init
   
   m_nMaxVisibleRows = (cy - m_nHH + m_nRH - 1) / m_nRH;
   m_nMaxEntirelyVisibleRows = (cy - m_nHH) / m_nRH;
   
   // Update scroller
   
   UpdateVScroller();
   
   // Recalc layout of the columns
   
   m_nWW = cx;
   m_nWH = cy;
   
   RecalcColumnLayout();
   
   // End
   
   CWnd::OnSize(nType, cx, cy);
}


/*****************************************************************************
*
* MOUSE HANDLING
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
   int nItem;
   
   // Set focus if not set
   
   if (!(m_dwFlags & WSF_LVF_FOCUSED))
   {
      m_dwFlags |= WSF_LVF_FOCUSED;
      CWnd::SetFocus();
   }
   else ASSERT(GetFocus()->m_hWnd == m_hWnd);
   
   // Handle lclick
   
   nItem = m_nFirstRow + (point.y - m_nHH) / m_nRH;
   if (nItem >= m_nNumRows) 
   {
      //nItem = m_nNumRows - 1;
      //SelectRows(-1, -1, true, false, true);
      return;
   }
   
   if (m_dwFlags & WSF_LVF_SINGLE_SELECTION) nFlags = 0;
   
   switch (nFlags & (MK_CONTROL|MK_SHIFT))
   {
   case MK_SHIFT:
      SelectRows(nItem, m_nFirstSelRow, true, false, false);
      break;
      
   case MK_CONTROL:
      SelectRows(nItem, nItem, false, true, false);
      break;
      
   default:
      SelectRows(nItem, nItem, true, false, true);
      break;
   }
   
   // Drag & drop
   
   m_dwFlags |= WSF_LVF_CHECK_DRAG;
   m_nDragX = point.x;
   m_nDragY = point.y;
   
   // Notify
   
   Notify(NM_CLICK);
}

//*****************************************************************************
void CWsfTableCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   int nItem = m_nFirstRow + (point.y - m_nHH) / m_nRH;
   if (nItem < m_nNumRows) Notify(NM_DBLCLK);
}

//*****************************************************************************
void CWsfTableCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
   int nItem;
   
   // Set focus if not set
   
   if (!(m_dwFlags & WSF_LVF_FOCUSED))
   {
      m_dwFlags |= WSF_LVF_FOCUSED;
      CWnd::SetFocus();
   }
   else ASSERT(GetFocus()->m_hWnd == m_hWnd);
   
   // Handle rclick
   
   nItem = m_nFirstRow + (point.y - m_nHH) / m_nRH;
   
   if (nItem < m_nNumRows)
   {
      // If clicked item is not selected select it first
      if (!(m_aCols[0].m_aItems[nItem].IsSelected()))
      {
         SelectRows(nItem, nItem, true, false, true);
      }
   }
   else
   {
      // Remove selection
      //SelectRows(-1, -1, true, false, true);
   }
   
   // Drag & drop
   
   m_dwFlags |= WSF_LVF_CHECK_RDRAG;
   m_nDragX = point.x;
   m_nDragY = point.y;
}

//*****************************************************************************
void CWsfTableCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   // Handles SHIFT+10 and VK_APPS
   // NOTE: Generation of this msg by rclick is disabled,
   // because my OnRButtonUp() doesn't call the default implementation!
   // NOTE: HeaderCtrl generates this when rclicked! (x!=-1)
   
   if (point.x != -1 || point.y != -1) return;
   
   RECT r;
   CNotifyInfo s;
   
   GetClientRect(&r);
   ClientToScreen(&r);
   s.y = r.top;
   s.x = r.left;
   
   if (m_nFocusedRow != -1)
   {
      int n = m_nFocusedRow - m_nFirstRow;
      if (n >= 0) 
      {	
         n = n * m_nRH + m_nHH;
         if (n < m_nWH) s.y += n + m_nRH;
      }
   }
   else s.y += m_nHH;
   
   s.nItem = -1;
   Notify(NM_RCLICK, &s);
}

//*****************************************************************************
void CWsfTableCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (!(m_dwFlags & (WSF_LVF_CHECK_DRAG|WSF_LVF_CHECK_RDRAG))) return;
   
   if (abs(point.x-m_nDragX) > 5 || abs(point.y-m_nDragY) > 5)
   {
      Notify(m_dwFlags & WSF_LVF_CHECK_DRAG ? WSF_LVN_DRAG : WSF_LVN_RDRAG);
      m_dwFlags &= ~(WSF_LVF_CHECK_DRAG | WSF_LVF_CHECK_RDRAG);
   }
}

//*****************************************************************************
void CWsfTableCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
   // Cancel drop check mode
   
   m_dwFlags &= ~(WSF_LVF_CHECK_DRAG | WSF_LVF_CHECK_RDRAG);
}

//*****************************************************************************
void CWsfTableCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
   // Cancel drop check mode
   
   m_dwFlags &= ~(WSF_LVF_CHECK_DRAG | WSF_LVF_CHECK_RDRAG);
   
   // Notify parent
   
   CNotifyInfo s;
   
   ClientToScreen(&point);
   s.x = point.x;
   s.y = point.y;
   s.nItem = -1;
   
   Notify(NM_RCLICK, &s);
}

//*****************************************************************************
BOOL CWsfTableCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
   int n = (int)zDelta / WHEEL_DELTA;		// Get number of scrolled rows
   
   if (n > 0) 
      while (n--) PostMessage(WM_VSCROLL, SB_LINEUP, 0);
      else
         while (n++) PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
         
         return true;
}


/*****************************************************************************
*
*
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl::UpdateVScroller()
{
   SCROLLINFO si;
   
   // Shift the first item if needed to keep the window full of items
   
   if (m_nFirstRow + m_nMaxEntirelyVisibleRows > m_nNumRows)
   {
      m_nFirstRow = m_nNumRows - m_nMaxEntirelyVisibleRows;
   }
   if (m_nFirstRow < 0) m_nFirstRow = 0;
   
   // Set scroller
   
   si.cbSize = sizeof(SCROLLINFO);
   si.fMask  = SIF_PAGE|SIF_RANGE|SIF_POS|SIF_DISABLENOSCROLL;
   si.nPage = m_nMaxVisibleRows;
   si.nMin = 0;
   si.nMax = m_nMaxEntirelyVisibleRows < m_nNumRows ? m_nNumRows : 0;
   si.nPos = m_nFirstRow;
   
   ::SetScrollInfo(*this, SB_VERT, &si, true);
}

//*****************************************************************************
void CWsfTableCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int n, nDelta, nOldFirstItem;
   RECT r;
   
   n= m_nFirstRow;
   
   switch(nSBCode)
   {
   case SB_LINEUP				: n--; break;
   case SB_LINEDOWN			: n++;break;
   case SB_PAGEUP				: n -= m_nMaxVisibleRows; break;
   case SB_PAGEDOWN			: n += m_nMaxEntirelyVisibleRows; break;
   case SB_THUMBPOSITION	:
   case SB_THUMBTRACK		: n = nPos; break;
   case SB_TOP						: n = 0; break;
   case SB_BOTTOM				: n = m_nNumRows - 1; break;
   default								: return;
   }
   
   // Update scroller
   
   nOldFirstItem = m_nFirstRow;
   m_nFirstRow = n;
   UpdateVScroller();
   
   // Update window
   
   nDelta = nOldFirstItem - m_nFirstRow;
   if (!nDelta) return;
   
   r.top = m_nHH;
   r.bottom = m_nWH;
   r.left = 0;
   r.right = m_nWW;
   
   if (abs(nDelta) < m_nMaxEntirelyVisibleRows)
      ::ScrollWindow(*this, 0, nDelta * m_nRH, &r, &r);
   else 
      Redraw();
}

//*****************************************************************************
void CWsfTableCtrl::SelectRowsAccordingKeyState(int nNewFocusedItem)
{
   if (IsShiftDown())
   {
      SelectRows(nNewFocusedItem, m_nFirstSelRow, true, false, false);
   }
   else if (IsCtrlDown())
   {
      CItem* pItem = &m_aCols[0].m_aItems[m_nFocusedRow];
      if (pItem->IsFocused())
      {
         pItem->Blur();
         DrawRow(m_nFocusedRow);
      }
      
      pItem = &m_aCols[0].m_aItems[nNewFocusedItem];
      if (!pItem->IsFocused())
      {
         pItem->Focus();
         DrawRow(nNewFocusedItem);
      }
      
      m_nFocusedRow = nNewFocusedItem;
   }
   else
   {
      SelectRows(nNewFocusedItem, nNewFocusedItem, true, false, true);
   }
}

//*****************************************************************************
void CWsfTableCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   int n;
   
   if (!m_nNumRows) return;
   
   // Handle key
   
   switch (nChar)
   {
   case VK_RETURN:
      if (m_nNumSelRows == 1) Notify(NM_DBLCLK);
      break;
      
   case VK_DELETE:
      if (m_nNumSelRows) Notify(WSF_LVN_DEL_PRESSED);
      break;
      
   case VK_INSERT:
      if (m_nNumSelRows) Notify(WSF_LVN_INS_PRESSED);
      break;
   }
   
   // If no item is focused, select & focus the first one
   
   if (m_nFocusedRow == -1) 
   {
      //	SelectRows(0,0, true, false, true);
      return;
   }
   
   // Handle key
   
   switch (nChar)
   {
   case VK_DOWN:
      if (m_nFocusedRow < m_nNumRows - 1)
      {
         SelectRowsAccordingKeyState(m_nFocusedRow + 1);
         if (m_nFocusedRow >= m_nFirstRow + m_nMaxEntirelyVisibleRows) 
            OnVScroll(SB_THUMBPOSITION,m_nFocusedRow - m_nMaxEntirelyVisibleRows + 1, 0);
      }
      break;
      
   case VK_UP:
      if (m_nFocusedRow > 0)
      {
         SelectRowsAccordingKeyState(m_nFocusedRow - 1);
         if (m_nFocusedRow < m_nFirstRow) OnVScroll(SB_THUMBPOSITION, m_nFocusedRow, NULL);
      }
      break;
      
   case VK_SPACE:
      if (IsCtrlDown()) SelectRows(m_nFocusedRow, m_nFocusedRow, false, true, false);
      break;
      
   case VK_NEXT:
      if (m_nFocusedRow != m_nNumRows - 1)
      {
         n = m_nFocusedRow + m_nMaxEntirelyVisibleRows - 1;
         if (n >= m_nNumRows) n = m_nNumRows - 1;
         SelectRowsAccordingKeyState(n);
         n = m_nFocusedRow - m_nMaxEntirelyVisibleRows + 1;
         if (n < 0) n = 0;
         OnVScroll(SB_THUMBPOSITION, n, 0);
      }
      break;
      
   case VK_PRIOR:
      if (m_nFocusedRow > 0)
      {
         n = m_nFocusedRow - m_nMaxEntirelyVisibleRows + 1;
         if (n < 0) n = 0;
         SelectRowsAccordingKeyState(n);
         OnVScroll(SB_THUMBPOSITION, n, NULL);
      }
      break;
      
   case VK_HOME:
      if (m_nFocusedRow > 0) 
      {
         SelectRowsAccordingKeyState(0);
         OnVScroll(SB_THUMBPOSITION, 0, NULL);
      }
      break;
      
   case VK_END:
      if (m_nFocusedRow != m_nNumRows - 1)
      {
         SelectRowsAccordingKeyState(m_nNumRows - 1);
         n = m_nNumRows - m_nMaxEntirelyVisibleRows;
         if (n < 0) n = 0;
         OnVScroll(SB_THUMBPOSITION, n, NULL);
      }
      break;
      
   default:
      CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
      return;
   }
}

/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl::OnRemoveColumn() 
{
   DeactivateColumn(m_nCurCol);
   Notify(WSF_LVN_COLUMN_REMOVED);
}

//*****************************************************************************
void CWsfTableCtrl::OnColumnsReset() 
{
   ActivateDefaultColumns();
   Notify(WSF_LVN_COLUMNS_RESETED);
}

//*****************************************************************************
void CWsfTableCtrl::OnSortAsc() 
{
   SetSortColumn(m_nCurCol, true, true);
}

//*****************************************************************************
void CWsfTableCtrl::OnSortDesc() 
{
   SetSortColumn(m_nCurCol, false, true);
}

//*****************************************************************************
UINT CWsfTableCtrl::OnGetDlgCode() 
{
   return DLGC_WANTARROWS | DLGC_WANTCHARS;	
   //return DLGC_WANTARROWS;
}

/*****************************************************************************/
void CWsfTableCtrl::OnWsfColumns() 
{
   OnHeaderContextMenu(false);
}

/*****************************************************************************/
void CWsfTableCtrl::OnUpdateWsfColumns(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}

/*****************************************************************************/
void CWsfTableCtrl::OnWsfSortAsc() 
{
   SetSortColumn(m_nSortCol, true, true);
}

/*****************************************************************************/
void CWsfTableCtrl::OnUpdateWsfSortAsc(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_nSortCol != -1);
   pCmdUI->SetRadio(m_bSortAsc);
}

/*****************************************************************************/
void CWsfTableCtrl::OnWsfSortDesc() 
{
   SetSortColumn(m_nSortCol, false, true);
}

/*****************************************************************************/
void CWsfTableCtrl::OnUpdateWsfSortDesc(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_nSortCol != -1);
   pCmdUI->SetRadio(!m_bSortAsc);
}

/*****************************************************************************/
void CWsfTableCtrl::OnWsfSortColumn() 
{
   // Build column menu
   
   HMENU hMenu = CreatePopupMenu();
   ASSERT(hMenu);
   
   MENUITEMINFO mii;
   mii.cbSize = sizeof(mii);
   mii.fMask = MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_FTYPE;
   mii.fType = MFT_STRING | MFT_RADIOCHECK;
   
   for (int i=0; i < m_aCols.GetSize(); i++)
   {
      if (!(m_aCols[i].IsSortable())) continue;
      
      mii.fState = 0;
      mii.wID = i+1;
      mii.dwTypeData = (char*) (const char*) m_aCols[i].sTitle;
      if (i == m_nSortCol) mii.fState |= MFS_CHECKED;
      
      InsertMenuItem(hMenu, i, true, &mii);
   }
   
   // Display context menu
   
   POINT pt; pt.x = 0; pt.y = 0;
   ClientToScreen(&pt);	
   int n = TrackPopupMenu(hMenu, 
      TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,
      pt.x, pt.y, 0, *this, NULL);
   DestroyMenu(hMenu);
   
   // Handle command
   
   if (n && --n != m_nSortCol) SetSortColumn(n, m_bSortAsc, true);
}

/*****************************************************************************/
void CWsfTableCtrl::OnUpdateWsfSortColumn(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}

