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

#include "stdafx.h"
#include "main.h"
#include "WsfTableCtrl2.h"
#include "WsfCmdUI.h"
#include "WsfPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CWsfTableCtrl2, CWsfTableCtrl)
   //{{AFX_MSG_MAP(CWsfTableCtrl2)
   ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
   ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
   ON_COMMAND(ID_WSF_REFRESH, OnWsfRefresh)
   ON_UPDATE_COMMAND_UI(ID_WSF_REFRESH, OnUpdateWsfRefresh)
   ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
   ON_MESSAGE(WSF_WM_UPDATE_MESSAGE_TEXT, OnWSF_WM_UPDATE_MESSAGE_TEXT)
   ON_NOTIFY_REFLECT(NM_RCLICK, OnNotifyReflect)
   ON_WM_KILLFOCUS()
   ON_WM_SETFOCUS()
   ON_WM_DESTROY()
	ON_COMMAND(ID_WSF_EXPORT_CSV, OnWsfExportCsv)
   ON_NOTIFY_REFLECT(NM_DBLCLK, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_COL_WIDTH_CHANGED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_COL_ORDER_CHANGED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_COLUMN_REMOVED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_SORTINGCHANGED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_COLUMN_ADDED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_COLUMNS_RESETED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_DEL_PRESSED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(WSF_LVN_SELCHANGED, OnNotifyReflect)
   ON_NOTIFY_REFLECT(NM_CLICK, OnNotifyReflect)
	ON_UPDATE_COMMAND_UI(ID_WSF_EXPORT_CSV, OnUpdateWsfExportCsv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWsfTableCtrl2, CWsfTableCtrl)


/*****************************************************************************/
CWsfTableCtrl2::CWsfTableCtrl2()
: m_CmdContext(*this, m_UpdateManager)
{
   m_bInOnCmdMsg = false;
   m_pEventSink = NULL;
   SetContextMenu(0,0);
   SetDefaultCmd(0);
}

/*****************************************************************************/
CWsfTableCtrl2::~CWsfTableCtrl2()
{  
}

/*****************************************************************************/
void CWsfTableCtrl2::SetEventSink(CWsfTableCtrl2::IEventSink* p)
{
   m_pEventSink = p;
}

/*****************************************************************************/
void CWsfTableCtrl2::OnDestroy() 
{
   SaveLayout();
   CWsfTableCtrl::OnDestroy();
}

//*****************************************************************************
void CWsfTableCtrl2::OnRclick(int nX, int nY)
{
   if (!m_dwMenu) return;
   
   // Get menu
   
   int nId = LOWORD(m_dwMenu);
   HMENU hMenu = ::LoadMenu(AfxFindResourceHandle((const char*)nId, RT_MENU), (const char*) nId);
   HMENU hPopUp = GetSubMenu(hMenu, HIWORD(m_dwMenu));
   
   WsfGetApp()->ProcessMenu(hPopUp);
   SetMenuDefaultItem(hPopUp, m_nDefCmd, false);
   
   // Display context menu
   
   TrackPopupMenu(hPopUp, 
      TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,
      nX, nY, 0, WsfGetFrame()->m_hWnd, NULL);
   
   DestroyMenu(hMenu);
}

//*****************************************************************************
void CWsfTableCtrl2::SetMessageString(BOOL bEmpty /*= false*/)
{
   //if (WsfGetFrame()->GetActiveView() != this) return;
   
   if (bEmpty) 
   {
      WsfGetFrame()->SetMessageText(AFX_IDS_IDLEMESSAGE);
   }
   else
   {
      CString s;
      OnWSF_WM_UPDATE_MESSAGE_TEXT((WPARAM) &s, 0);
      WsfGetFrame()->SetMessageText(s);
   }
}

//*****************************************************************************
BOOL CWsfTableCtrl2::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   ASSERT_VALID(this);
   
   if (pHandlerInfo) return false;
   if (m_bInOnCmdMsg) return false;
   
   // Send the command to all selected items. 
   
   if (GetSelRowCount())
   {
      if (nCode==CN_COMMAND)
      {
         m_bInOnCmdMsg = true;
         
         // Confirmation of the delete operation
         
         if (nID == ID_WSF_DELETE && !pHandlerInfo)
         {
            CString s;
            if (GetSelRowCount() > 1)
               s.Format(GetSelRowCount() < 5 ? IDS_REALLY_DELETE_2_TO_4 : IDS_REALLY_DELETE_5_MORE, GetSelRowCount());
            else
               VERIFY(s.LoadString(IDS_REALLY_DELETE_ITEM));
            
            if (AfxMessageBox(s, MB_ICONQUESTION|MB_YESNO) != IDYES) { m_bInOnCmdMsg = false; return true; }
         }
         
         // Call the command handler for each selected row
         
         m_CmdContext.Reset();
         BOOL b;
         
         while (!m_CmdContext.IsCanceled() && m_CmdContext.MoveToNextRow())
         {
            b = CWsfTableCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
            if (!b) break;
         }
         
         m_bInOnCmdMsg = false; 			
         
         // If there was a handler, perform any changes queued so far and quit
         
         if (b) 
         { 
            m_UpdateManager.Update(*this);
            return true; 
         }
      }
      
      else if (nCode==CN_UPDATE_COMMAND_UI)
      {
         m_bInOnCmdMsg = true;
         
         CWsfCmdUI cmdui((CCmdUI*)pExtra);
         BOOL b;

         m_CmdContext.Reset();
         
         while (m_CmdContext.MoveToNextRow())
         {
            b = CWsfTableCtrl::OnCmdMsg(nID, nCode, (void*) &cmdui, pHandlerInfo);
            if (!b) break;
            if (!cmdui.IsEnabled()) break;
         }
         
         m_bInOnCmdMsg = false;
         
         if (b) return true;
      }
   }
   else
   {
      // No item is selected or the table is empty
      
      m_bInOnCmdMsg = true;
      m_CmdContext.Reset();
      
      BOOL b = CWsfTableCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
      m_bInOnCmdMsg = false; 			
      
      if (b)
      {
         m_UpdateManager.Update(*this);
         return true;
      }
   }
   
   // Send it to the parent class
   
   return CWsfTableCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//*****************************************************************************
void CWsfTableCtrl2::OnNotifyReflect(NMHDR* pNotifyStruct, LRESULT* pResult) 
{
   ASSERT(pNotifyStruct);
   
   switch (pNotifyStruct->code)
   {
   case NM_CLICK:
      if (m_pEventSink) m_pEventSink->OnClick(*this);
      break;
      
   case NM_DBLCLK: 
      if (m_pEventSink) m_pEventSink->OnDoubleClick(*this);
      if (m_nDefCmd) WsfGetApp()->PostCommand(m_nDefCmd);
      break;
      
   case NM_RCLICK:	
      {
         CWsfTableCtrl::CNotifyInfo& s = *(CWsfTableCtrl::CNotifyInfo*) pNotifyStruct;
         OnRclick(s.x, s.y);
      }
      break;
      
   case WSF_LVN_SELCHANGED:
      SetMessageString();
      if (m_pEventSink) m_pEventSink->OnSelChanged(*this);
      break;
      
   case WSF_LVN_DEL_PRESSED:
      if (OnCmdMsg(ID_WSF_DELETE, CN_UPDATE_COMMAND_UI, NULL, NULL))
      {
         WsfGetApp()->PostCommand(ID_WSF_DELETE);
      }
      break;
      
   case WSF_LVN_COLUMNS_RESETED:
   case WSF_LVN_COLUMN_ADDED:
      {
         CWsfTableCtrl::CLayout l;
         CWsfTableCtrl::GetLayout(l);
         OnLayoutChanged(l);
         FillTable(true);
      }
      break;
      
   case WSF_LVN_SORTINGCHANGED:
      FillTable(true);
      break;
      
   case WSF_LVN_COLUMN_REMOVED:
   case WSF_LVN_COL_ORDER_CHANGED:
   case WSF_LVN_COL_WIDTH_CHANGED:
      {
         CWsfTableCtrl::CLayout l;
         CWsfTableCtrl::GetLayout(l);
         OnLayoutChanged(l);
      }
      break;
   }
}

/*****************************************************************************/
BOOL CWsfTableCtrl2::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch (((NMHDR*)lParam)->code)
	{
   case TTN_NEEDTEXT:
      *pResult = WsfGetFrame()->SendMessage(WM_NOTIFY, wParam, lParam);
      return true;
	
   default:	
		return CWsfTableCtrl::OnNotify(wParam, lParam, pResult);
	}

	return true;
}


/*****************************************************************************
*
* OPERATIONS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfTableCtrl2::SetDefaultCmd(int nId)
{
   m_nDefCmd = nId;
}

/*****************************************************************************/
void CWsfTableCtrl2::SetContextMenu(int nMenuId, int nSubMenuIdx)
{
   m_dwMenu = nMenuId + (nSubMenuIdx << 16);
}

/*****************************************************************************/
void CWsfTableCtrl2::SelectSingleRowById(int nRowId)
{
   m_UpdateManager.AddSelectSingleRow(nRowId);
   if (!m_bInOnCmdMsg) m_UpdateManager.Update(*this);
}

/*****************************************************************************/
void CWsfTableCtrl2::Refresh()
{
   m_UpdateManager.AddAction(CUpdateManager::REBUILD, 0);
   if (!m_bInOnCmdMsg) m_UpdateManager.Update(*this);
}

/*****************************************************************************/
void CWsfTableCtrl2::ProcessDbEvent(CWsfDbEvent& e)
{
   OnDbEvent(e, m_UpdateManager);
   if (!m_bInOnCmdMsg) m_UpdateManager.Update(*this);
}

/*****************************************************************************/
int CWsfTableCtrl2::GetRowIdxById(int n)
{
   for (int i=0; i<GetRowCount(); i++)
   {
      CRowData* p = (CRowData*) (CObject*) GetRowData(i);
      ASSERT_VALID(p);
      if (p->nRowId == n) return i;
   }
   return -1;
}

/*****************************************************************************/
void CWsfTableCtrl2::SaveLayout()
{
   CWsfTableCtrl::CLayout l;
   CWsfTableCtrl::GetLayout(l);
   OnSaveLayout(l);
}

//*****************************************************************************
void CWsfTableCtrl2::OnSetFocus(CWnd* pOldWnd) 
{
   SetMessageString(false);
   CWsfTableCtrl::OnSetFocus(pOldWnd);
}

/*****************************************************************************/
void CWsfTableCtrl2::OnKillFocus(CWnd* pNewWnd) 
{
   SetMessageString(true);
   CWsfTableCtrl::OnKillFocus(pNewWnd);
}



/*****************************************************************************
*
* PROPERTIES
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTableCtrl2::PSCallback(void* pData, CWsfPropertySheet& ps, BOOL bOk)
{
   ASSERT(pData);
   CWsfTableCtrl2* p = (CWsfTableCtrl2*) pData;
   return p->OnCloseProperties(p->m_nPropertiesRowIdx, ps, bOk);
}

/*****************************************************************************/
int CWsfTableCtrl2::DisplayRowProperties(int nRowIdx)
{
   ASSERT(nRowIdx>=-1 && nRowIdx<GetRowCount());
   
   CWsfPropertySheet ps(nRowIdx==-1, PSCallback, this);
   m_nPropertiesRowIdx = nRowIdx;
   
   // Call user to init the ps
   
   if (!OnInitProperties(nRowIdx, ps))
   {
      ps.RemoveAllPages();	
      return IDCANCEL;
   }
   ASSERT(ps.GetPageCount() > 0);				// there must be at least one page
   
   // Do IT
   int n = ps.DoModal();
   ps.RemoveAllPages();	
   
   return n;
}

/*****************************************************************************
*
* ROWS
*
*****************************************************************************/

/*****************************************************************************/
int CWsfTableCtrl2::GetRowId(int nRowIdx) const
{
   ASSERT(nRowIdx>=0 && nRowIdx<GetRowCount());
   return ((CRowData*)(CObject*) GetRowData(nRowIdx))->nRowId;
}

/*****************************************************************************/
void CWsfTableCtrl2::RefreshRow(int nRowId)
{
   AddUpdateRow(nRowId, false);
}

/*****************************************************************************
*
* ITEMS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfTableCtrl2::SetItem(int nRowIdx, 
                             const char* pcszTitle, 
                             CWsfTableCtrl::CItem& item, 
                             CWslVariant* pv)
{
   ASSERT(pcszTitle);
   int i = GetColIdx(pcszTitle);
   ASSERT(i!=-1);      // No such a column!
   if (i != -1) SetItem(nRowIdx, i, item, pv);
}

/*****************************************************************************/
void CWsfTableCtrl2::SetItem(int nRowIdx, 
                             int nColIdx, 
                             CWsfTableCtrl::CItem& item, 
                             CWslVariant* pv)
{
   ASSERT(nColIdx >= 0 && nColIdx < m_aCols.GetSize());
   char temp[1024];
   
   // Keep item's data
   
   item.dwParam = GetItemData(nRowIdx, nColIdx);
   
   // Auto format data from a variant or use the item's text
   
   if (pv)
   {
      item.sText = temp;
      switch (pv->GetType())
      {
      case CWslVariant::VT_LONG:
         item.sText.Format("%d", pv->nVal);
         break;
         
      case CWslVariant::VT_STRING:
         if (pv->pString->GetLength() < 1024)
            item.sText = *pv->pString;
         else
            item.sText = "<string too long>";
         break;
         
      case CWslVariant::VT_DATE:
         item.sText.Format("%d.%d.%d %02d:%02d:%02d", 
            pv->pDate->day, pv->pDate->month, pv->pDate->year,
            pv->pDate->hour, pv->pDate->minute, pv->pDate->second);
         break;
         
      case CWslVariant::VT_NULL:
         item.sText = "–";
         break;
         
      default:
         item.sText.Format("<unsupported type #%d>", pv->GetType());
         break;
      }
   }
   else
   {
      item.sText = item.sText;
   }
   
   // Set item
   
   CWsfTableCtrl::SetItem(nRowIdx, nColIdx, &item);
}

/*****************************************************************************
*
* BUILDING TABLE CONTENT
*
*****************************************************************************/

//*****************************************************************************
void CWsfTableCtrl2::FillTable(BOOL bRememberPos)
{
   TRACE0("CWsfTableCtrl2::FillTable()\n");
   int nSelKey;
   
   // Remember position
   
   if (bRememberPos) 
   {
      //Push();
      nSelKey = GetFocusedRow();
      if (nSelKey != -1) nSelKey = GetRowId(nSelKey);
   }
   
   // Delete table content
   
   DeleteAllRows();
   
   // Update window, because the filling operation
   // can be quit lenghty
   
   //   RedrawWindow();
   
   // Fill the table
   
   OnFillTable(GetSortCol(), GetSortDir(), 0, -1);
   
   // Restore the old position
   
   if (bRememberPos)
   {
      //Pop();
      if (nSelKey != -1) nSelKey = GetRowIdxById(nSelKey); 
      
      if (nSelKey != -1) 
         CWsfTableCtrl::SelectSingleRow(nSelKey, true);
      else if (GetRowCount())
         CWsfTableCtrl::SelectSingleRow(0, true);
      
      Redraw();
   }
   else if (GetRowCount())
   {
      CWsfTableCtrl::SelectSingleRow(0, true);
      Redraw();
   }
   
   SetMessageString();
}

//*****************************************************************************
void CWsfTableCtrl2::AddUpdateRow(int nPrimaryKey, BOOL bUpdateOnly)
{
   // Find the item with the given primary key
   // If it's found, active the update mode, otherwise add the item.
   
   int nRowIdx = GetRowIdxById(nPrimaryKey);
   int bUpdateMode = nRowIdx != -1;
   
   if (bUpdateOnly && !bUpdateMode) return;
   
   if (bUpdateMode) SetRowData(nRowIdx, NULL);

   // Get the item
   
   OnFillTable(GetSortCol(), GetSortDir(), nPrimaryKey, nRowIdx);
   
   // Delete the item, if it has not been updated by the previous call
   // Applied e.g. when an item doesn't pass the current filter after it was updated
   
   if (bUpdateMode && !GetRowData(nRowIdx))
   {
      DeleteRow(nRowIdx);
      nRowIdx = -1;
   }
   
   // Redraw the item
   // Select it, if it's the only one item in the list
   
   if (GetRowCount()==1)
   {
      CWsfTableCtrl::SelectSingleRow(0, true);
   }
   else
   {
      nRowIdx = nRowIdx == -1 ? GetRowIdxById(nPrimaryKey) : nRowIdx;
      if (nRowIdx != -1)
      {
         ASSERT(nRowIdx < GetRowCount());
         DrawRow(nRowIdx);
      }
   }
}

/*****************************************************************************
*
* PRINTING END EXPORTING
*
*****************************************************************************/

/*****************************************************************************/
CString CWsfTableCtrl2::GenerateHtml()
{
   // Create a temp HTML file
  
   CString sPathName = WsfGetApp()->GetTempPathName("html");
   CWslFile f;
   f.Create(sPathName);
   
   // Styles
   
   f.WriteLn("<html>");
   f.WriteLn("<head>");
   
   f.WriteLn("<style>");
   f.WriteLn("body     { font-family: arial; font-size: 8pt }");
   f.WriteLn("td       { font-family: arial; font-size: 8pt; border-right: solid #000000 1px; border-top: solid #000000 1px }");
   f.WriteLn("th       { font-family: arial; font-size: 8pt; border-right: solid #000000 1px; border-bottom: solid #000000 1px }");
   f.WriteLn("</style>");
   
   f.WriteLn("<meta http-equiv='author' content='Copyright (C) SpyTech - http://www.spytech.cz'>");
   f.WriteLn("<meta http-equiv='Content-Type' content='text/html; charset=windows-1250'>");

   f.WriteLn("</head>");
   f.WriteLn("<body>");
   
   CString s;
   GetWindowText(s);
   f.FormatLn("<p><b>%s</b></p>", CWslXMLParser::EncodeSpecialChars(s, CWslXMLParser::CDATA, 1250));
   
   // Table header
   
   f.WriteLn("<table cellspacing='0' cellpadding='2' style='font-family: verdana; font-size: 10pt; border: solid #000000 2px'>");
   
   for (int i=0; i < m_aColsOrder.GetSize(); i++)
   {
      const CWsfTableCtrl::CColumn& c = GetColumn(m_aColsOrder[i]);
      
      if (c.eAlign==CWsfTableCtrl::CColumn::RIGHT)
         f.WriteLn("<colgroup style='text-align: right'/>");
      else if (c.eAlign==CWsfTableCtrl::CColumn::RIGHT) 
         f.WriteLn("<colgroup style='text-align: center'/>");
      else
         f.WriteLn("<colgroup/>");
   }
   
   f.WriteLn("<tr>");
   
   for (i=0; i < m_aColsOrder.GetSize(); i++)
   {
      const CWsfTableCtrl::CColumn& c = GetColumn(m_aColsOrder[i]);
      f.FormatLn("<th>%s</th>", CWslXMLParser::EncodeSpecialChars(c.sTitle, CWslXMLParser::CDATA, 1250));
   }
   
   f.WriteLn("</tr>");
   
   // Content
   
   for (i=0; i < GetRowCount(); i++)
   {
      f.WriteLn("<tr>");
      
      for (int t=0; t < m_aColsOrder.GetSize(); t++)
      {
         const CWsfTableCtrl::CItem& x = GetItem(i, m_aColsOrder[t]);
         CString sStyle;
         
         if (x.bBold) sStyle += "; font-weight: bold";
         
         f.FormatLn("<td style='%s'>%s</td>", sStyle, 
            x.sText.IsEmpty() ? "&nbsp;" : CWslXMLParser::EncodeSpecialChars(x.sText, CWslXMLParser::CDATA, 1250));
      }
      
      f.WriteLn("</tr>");
   }
   
   f.WriteLn("</table>");
   
   // Sumary

   VERIFY(s.LoadString(IDS_TOTAL_ROWS));
   f.FormatLn("<p><b>%s</b> %d</p>", s, GetRowCount());
   
   // Done
   
   f.WriteLn("</body>");
   f.WriteLn("</html>");
   
   f.Close();
   
   return sPathName;
}

/*****************************************************************************/
CString CWsfTableCtrl2::GenerateCsv()
{
   CString sCSV;
   CString s, s2;

   // Build the header

   for (int i=0; i < m_aColsOrder.GetSize(); i++)
   {
      const CWsfTableCtrl::CColumn& c = GetColumn(m_aColsOrder[i]);

      if (i > 0) sCSV += ";";
      s = c.sTitle;
      s.Replace("\"", "\"\"");
      s2.Format("\"%s\"", s);
      sCSV += s2;
   }

   sCSV += "\r\n";

   // Rows

   for (i=0; i < GetRowCount(); i++)
   {
      for (int t=0; t < m_aColsOrder.GetSize(); t++)
      {
         const CWsfTableCtrl::CItem& x = GetItem(i, m_aColsOrder[t]);
         
         if (t > 0) sCSV += ";";
         s = x.sText;
         s.Replace("\"", "\"\"");
         s2.Format("\"%s\"", s);
         sCSV += s2;
      }
      
      sCSV += "\r\n";
   }

   return sCSV;
}

/*****************************************************************************
*
* COMMAND CONTEXT
*
*****************************************************************************/

/*****************************************************************************/
CWsfTableCtrl2::CCmdContext::CCmdContext(CWsfTableCtrl2& Tbl, CUpdateManager& UM)
: m_Tbl(Tbl), m_UpdateManager(UM)
{
}

/*****************************************************************************/
void CWsfTableCtrl2::CCmdContext::Reset()
{
   m_bCanceled = false;
   m_nRowCounter = 0;
   m_pRowData = NULL;
   m_nRowIdx = -1;
   m_Pos = m_Tbl.GetFirstSelectedRowPosition();
}

/*****************************************************************************/
BOOL CWsfTableCtrl2::CCmdContext::MoveToNextRow()
{
   if (!m_Pos) 
   {
      m_pRowData = NULL;
      m_nRowIdx = -1;
      m_nRowCounter = 0;
      return false;
   }

   m_nRowIdx = m_Tbl.GetNextSelectedRow(m_Pos);
   m_pRowData = (CRowData*) (CObject*) m_Tbl.GetRowData(m_nRowIdx);
   ASSERT_VALID(m_pRowData);

   m_nRowCounter++;
   return true;
}

/*****************************************************************************/
void CWsfTableCtrl2::CCmdContext::DeleteRow()
{
   m_UpdateManager.AddAction(CUpdateManager::DELETE_ROW, GetRowId());
}

/*****************************************************************************/
void CWsfTableCtrl2::CCmdContext::UpdateRow()
{
   m_UpdateManager.AddAction(CUpdateManager::UPDATE_ROW, GetRowId());
}


/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

//*****************************************************************************
LRESULT CWsfTableCtrl2::OnWSF_WM_UPDATE_MESSAGE_TEXT(WPARAM wParam, LPARAM lParam)
{
   CString& s = *(CString*) wParam;
   
   if (GetSelRowCount() > 1)
   {
      s.Format(IDS_TOTAL_AND_SELECTED_ITEMS, 
         GetRowCount(), GetSelRowCount());
   }
   else
   {
      s.Format(IDS_TOTAL_ITEMS, GetRowCount());
   }
   
   return true;
}

//*****************************************************************************
void CWsfTableCtrl2::OnEditSelectAll()
{
   CWsfTableCtrl::SelectAllRows();
}

//*****************************************************************************
void CWsfTableCtrl2::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!(m_dwFlags & WSF_LVF_SINGLE_SELECTION) && GetRowCount());
}

/*****************************************************************************/
void CWsfTableCtrl2::OnWsfRefresh()
{
   Refresh();
}

/*****************************************************************************/
void CWsfTableCtrl2::OnUpdateWsfRefresh(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(true);
}

/*****************************************************************************/
void CWsfTableCtrl2::OnFilePrint() 
{
   try
   {
      CString s, s2, s3;
      s = GenerateHtml();
      
      GetWindowText(s2);
      VERIFY(s3.LoadString(IDS_PREVIEW_PREFIX));
      
      VERIFY(WsfGetApp()->OpenHtmlFile(s, 
         s2.IsEmpty() ? s3 : s3 + " – " + s2, 
         IDB_SMALL_ICONS, 8,
         true));
      
      WsfGetApp()->PostCommand(ID_FILE_PRINT);
   }
   
   catch (CException* e)
   {
      WsfReportError(e, IDS_TABLE_PRINT_ERROR, true);
   }
}

/*****************************************************************************/
void CWsfTableCtrl2::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}

/*****************************************************************************/
void CWsfTableCtrl2::OnFilePrintPreview() 
{
   try
   {
      CString s, s2, s3;
      s = GenerateHtml();
      
      GetWindowText(s2);
      VERIFY(s3.LoadString(IDS_PREVIEW_PREFIX));
      
      VERIFY(WsfGetApp()->OpenHtmlFile(s, 
         s2.IsEmpty() ? s3 : s3 + " – " + s2, 
         IDB_SMALL_ICONS, 8,
         true));
   }
   
   catch (CException* e)
   {
      WsfReportError(e, IDS_TABLE_PRINT_ERROR, true);
   }
}

/*****************************************************************************/
void CWsfTableCtrl2::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}

/*****************************************************************************/
void CWsfTableCtrl2::OnWsfExportCsv() 
{
   CString s2;

   // Get file name from the first non-empty window name

   CWnd* pWnd = this;

   do
   {
      pWnd->GetWindowText(s2);
      pWnd = pWnd->GetParent();
   }
   while (s2.IsEmpty() && pWnd && pWnd != WsfGetFrame());

   if (s2.IsEmpty()) s2.LoadString(IDS_NONAME);
   
   // Ask for file

   CString s((const char*) IDS_CSV_SAVE_FILE_FILTER);
   CFileDialog fd(false, ".csv", s2, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER , s, this);
   
   if (fd.DoModal() == IDCANCEL) return;
   
   // Save

   try
   {
      CWslFile f;
      f.Create(fd.GetPathName());
      f.Write(GenerateCsv());
   }
   catch (CException* e)
   {
      WsfReportError(e, IDS_ERROR_EXPORTING_CSV, true);
   }

   GetCmdContext().CancelCommand();
}

void CWsfTableCtrl2::OnUpdateWsfExportCsv(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}
