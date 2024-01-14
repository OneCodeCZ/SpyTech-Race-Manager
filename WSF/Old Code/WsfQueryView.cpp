///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfTableCtrl.h"
#include "WsfCmdUI.h"
#include <db\WslDb.h>
//#include "WsfTIQuery.h"
#include "WsfQueryDoc.h"
#include "WsfQueryView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfQueryView, CView)
	//{{AFX_MSG_MAP(CWsfQueryView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WSF_WM_UPDATE_MESSAGE_TEXT, OnWSF_WM_UPDATE_MESSAGE_TEXT)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_WSF_REFRESH, OnWsfRefresh)
	ON_UPDATE_COMMAND_UI(ID_WSF_REFRESH, OnUpdateWsfRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//WSL_IMPLEMENT_SERIAL_ABSTRACT_DLL(WSL_API, CWsfQueryDoc, CDocument, 1)

//*****************************************************************************
CWsfQueryView::CWsfQueryView() 
{
	m_pList = NULL;
	m_bInOnCmdMsg = false;
	m_bRequery = false;
	m_bUseFilter = false;
}

//*****************************************************************************
CWsfQueryView::~CWsfQueryView()
{
	//ASSERT(m_PrimaryKeyStack.IsEmpty());

	// Delete all row data and list items

	for (int i=m_pList->GetRowCount(); i--;) 
		delete (CObject*) m_pList->GetItemData(i,1);
	
	m_pList->DeleteAllRows();
	delete m_pList;
}


//*****************************************************************************
BOOL CWsfQueryView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CView::PreCreateWindow(cs)) return FALSE;

	cs.lpszClass = AfxRegisterWndClass(0, NULL, NULL, NULL);
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return TRUE;
}

//*****************************************************************************
int CWsfQueryView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;

	// Init list control

	m_pList = new CWsfTableCtrl();
	VERIFY(m_pList->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | HDS_DRAGDROP, WsfZeroRect, this, 1001, NULL));

	return 0;
}

//*****************************************************************************
void CWsfQueryView::OnDestroy() 
{
	SaveProfile();
	CView::OnDestroy();
}

//*****************************************************************************
void CWsfQueryView::OnInitialUpdate() 
{
	// Check columns

	CWsfQueryDoc& doc = GetDoc();
	ASSERT(doc.m_aQueryCols.GetSize());			// some columns must be defined
	ASSERT(doc.m_aViewCols.GetSize());			// some columns must be defined
	ASSERT(doc.m_nPrimaryKeyColIdx >= 0);		// there must a primary key
	ASSERT(doc.m_aViewCols.GetSize() >= 2);	// at least two columns must be defined

	// Define all view columns in the table control

	CWsfTableCtrl::CColumn lvc;
	m_pList->RemoveAllColumns();

	for (int i=0; i < doc.m_aViewCols.GetSize(); i++)
	{
		CWsfQueryDoc::CWsfViewColumn& c = *doc.m_aViewCols[i];

		lvc.dwFlags = c.dwFlags;
		lvc.sTitle = c.sTitle;
		lvc.nImageIdx = c.nImage;
		lvc.nDefaultWidth = (float) c.nWidth;
		lvc.dwParam = i;

		m_pList->AddColumn(&lvc);
	}

	// Load layout of the table control.
	// If it fails, activate default columns.

	CWsfTableCtrl::CLayout l;
	BOOL b = doc.OnLoadLayout(l);
	if (b) b = m_pList->LoadLayout(l);
	if (!b) 
	{
		m_pList->ActivateDefaultColumns();
		m_pList->SetSortColumn(doc.m_nSortColIdx, doc.m_bSortAsc, false);
	}
		
	// Assign image lists and build list

	m_pList->SetImageList(WsfGetApp()->GetSmallIconList(), true);
	m_pList->SetImageList(WsfGetApp()->GetColumnIconList(), false);

	BuildList(false);
}

/*****************************************************************************/
void CWsfQueryView::SaveProfile()
{
	CWsfTableCtrl::CLayout l;
	m_pList->SaveLayout(l);
	GetDoc().OnSaveLayout(l);
}

//*****************************************************************************
void CWsfQueryView::GetQuery(CString &sQuery, int nPrimaryKey)
{
	CWsfQueryDoc &doc = GetDoc();
	CString sColumns, sFrom, sWhere, sOrderBy;
	int i,n;

	// Build columns part of the query

	for (n=doc.m_aQueryCols.GetSize(),i=0; i<n; i++)
	{
		if (!sColumns.IsEmpty()) sColumns += ',';
		sColumns += doc.m_aQueryCols[i]->sTitle;
	}

	// WHERE contition

	if (nPrimaryKey != -1)
	{
		sWhere.Format("%s=%d", doc.m_aQueryCols[doc.m_nPrimaryKeyColIdx]->sTitle,
			nPrimaryKey);
	}

	// ORDER BY part

	if (nPrimaryKey == -1 && (i = m_pList->GetSortCol()) != -1) 
	{
		i = m_pList->GetColumnData(i);
		i = doc.m_aViewCols[i]->nQueryColIdx;
		sOrderBy = doc.m_aQueryCols[i]->sTitle;
		sOrderBy += m_pList->GetSortDir() ? " ASC" : " DESC";
	}

	// Let the doc build the query

	sQuery = doc.OnGetQuery(sColumns, sWhere, sOrderBy, m_bUseFilter);
}

//*****************************************************************************
void CWsfQueryView::AddUpdateRow(CWslDbRecordset &rs, int nListItem, BOOL bDrawRow)
{
	CWsfQueryDoc &doc = GetDoc();
	CWsfTableCtrl::CItem lvi;
	int i, n, nCol;
	char szBuffer[1024];
	BOOL bAddMode = nListItem == -1;

	// Add item if add mode

	if (bAddMode)
	{
		nListItem = m_pList->GetRowCount();
		m_pList->AddRow();
	}
	else
	{
		// Delete row data
		delete (CObject*) m_pList->GetItemData(nListItem,1);
	}

	// Read query column valus

	for (i=doc.m_aQueryCols.GetSize(); i--;) 
	{
		rs.GetFieldValue(i, doc.m_aQueryCols[i]->v);
	}

	// Set value of visible view columns

	for (i=0, n = m_pList->m_aColsOrder.GetSize(); i<n; i++)
	{
		nCol = m_pList->GetColumnData(m_pList->m_aColsOrder[i]);
	
		szBuffer[0] = 0;
		lvi.sText = szBuffer;
		lvi.nImageIdx = -1;
		lvi.dwParam = 0;
		lvi.rgbColor = RGB(0,0,0);

		doc.OnGetViewColumnValue(nCol, lvi);

		//if (!szBuffer[0]) lvi.pszText = NULL;

		m_pList->SetItem(nListItem, nCol, &lvi);
	}

	// Get row data

	void* pData = doc.OnGetRowData();

	// Add data and primary key to the row

	m_pList->SetItemData(nListItem, 0, doc.m_aQueryCols[doc.m_nPrimaryKeyColIdx]->v.nVal);
	m_pList->SetItemData(nListItem, 1, (DWORD) pData);
	
	// Draw the item or select it if it's the only one item in the list

	if (bDrawRow)
	{
		if (bAddMode && m_pList->GetRowCount() == 1)
			m_pList->SelectSingleRow(0, true);
		else
			m_pList->DrawRow(nListItem);
	}
}

//*****************************************************************************
void CWsfQueryView::BuildList(BOOL bRememberPos)
{
	CString sQuery;
	CWsfQueryDoc &doc = GetDoc();
	int i;
	CWaitCursor wc;

	TRACE0("CWsfQueryView::BuildList()\n");

	// Delete list items & update selections

	if (bRememberPos) m_pList->Push();

	for (i=m_pList->GetRowCount(); i--;) delete (CObject*) m_pList->GetItemData(i,1);
	m_pList->DeleteAllRows();

	// Get SQL query

	GetQuery(sQuery, -1);

	// Build the list

	if (!sQuery.IsEmpty())
	{
		try
		{
			CWslDbRecordsetPtr rs = doc.OnGetDb().CreateRecordsetObject();
			for (rs->Open(sQuery); !rs->IsEOF(); rs->MoveNext()) 
			{
				AddUpdateRow(*rs, -1, false);
			}
			rs->Close();
			doc.OnGetDb().Execute("COMMIT");
			TRACE0("-------------------------------------------------------------\n");
		}
		catch (CException* e)
		{
			try
			{
				doc.OnGetDb().Execute("ROLLBACK");
			}
			catch (CException* e) { e->Delete(); }
			WsfReportError(e, IDS_DB_ERROR, true);
		}
	}

	// Restore position
	
	if (bRememberPos)
	{
		m_pList->Pop();
	}
	else if (m_pList->GetRowCount())
	{
		m_pList->SelectSingleRow(0, true);
		m_pList->Redraw();
	}

	SetMessageString();
}

//*****************************************************************************
void CWsfQueryView::UpdateRow(int nPrimaryKey)
{
	CString sQuery;
	CWsfQueryDoc &doc = GetDoc();
	int nListItem = m_pList->FindItem(0, nPrimaryKey);

	try
	{
		// Init the query string & perform query
		// Insert row to the list (if any)

		GetQuery(sQuery, nPrimaryKey);
		CWslDbRecordsetPtr rs = doc.OnGetDb().CreateRecordsetObject();
		rs->Open(sQuery);
		if (!rs->IsEOF()) AddUpdateRow(*rs, nListItem, true);
		rs->Close();
		doc.OnGetDb().Execute("COMMIT");
		TRACE0("-------------------------------------------------------------\n");
	}

	catch (CException* e)
	{
		try
		{
			doc.OnGetDb().Execute("ROLLBACK");
		}
		catch (CException* e) { e->Delete(); }
		WsfReportError(e, IDS_DB_ERROR, true);
	}
}

//*****************************************************************************
void CWsfQueryView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	//CString s; GetDoc().GetTitle(s); TRACE2("WsdbTableView: \"%s\": %s\n", s, bActivate ? "AC" : "DEAC");

	//WsfGetFrame()->SelectOneTreeItem(NULL);
	SetMessageString(!bActivate);
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//*****************************************************************************
void CWsfQueryView::OnRclick(int nX, int nY)
{
	// Get menu

	CWslArray<int> a;
	a += GetDoc().m_dwMenu;
	HMENU hMenu = WsfGetApp()->BuildPopupMenu(a, GetDoc().m_nDefCmd);

	// Display context menu

	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
		nX, nY, 0, WsfGetFrame()->m_hWnd, NULL);

	DestroyMenu(hMenu);
}

//*****************************************************************************
BOOL CWsfQueryView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
  CWsfTableCtrl::CNotifyInfo &s = *(CWsfTableCtrl::CNotifyInfo*) lParam;

	switch ( ((NMHDR*)lParam) -> code )
	{
		case NM_DBLCLK: 
			WsfGetFrame()->SendMessage(WM_COMMAND, GetDoc().m_nDefCmd, 0);
			break;

		case NM_RCLICK:	
			OnRclick(s.x, s.y);
			break;

		case WSF_LVN_SELCHANGED:
			SetMessageString();
			break;

		case WSF_LVN_DEL_PRESSED:
			if (OnCmdMsg(ID_WSF_DELETE, CN_UPDATE_COMMAND_UI, NULL, NULL))
				WsfGetFrame()->SendMessage(WM_COMMAND, ID_WSF_DELETE, 0);
			break;

		case WSF_LVN_COLUMNS_RESETED:
		case WSF_LVN_COLUMN_ADDED:
			BuildList(true);
			break;

		case WSF_LVN_SORTINGCHANGED:
			BuildList(true);
			break;

		case WSF_LVN_COLUMN_REMOVED:
		case WSF_LVN_COL_ORDER_CHANGED:
		case WSF_LVN_COL_WIDTH_CHANGED:
			break;

		default:	
			return CView::OnNotify(wParam, lParam, pResult);
	}

	return true;
}

//*****************************************************************************
void CWsfQueryView::PerformChanges()
{
	int i,n,t;

	if (m_bRequery)
	{
		m_aChanges.RemoveAll();
		BuildList(true);
		m_bRequery = false;
		return;
	}

	for (n=m_aChanges.GetSize(),i=0; i < n; i++)
	{
		switch (m_aChanges[i].nChangeType)
		{
		case CWsfQueryDoc::UPDATE_ROW:
			UpdateRow(m_aChanges[i].nPrimaryKey);
			break;
			
		case CWsfQueryDoc::DELETE_ROW:
			t = m_pList->FindItem(0, m_aChanges[i].nPrimaryKey);
			if (t != -1) 
			{
				delete (CObject*) m_pList->GetItemData(t, 1);
				m_pList->DeleteRow(t);
			}
			break;
		}
	}

	m_aChanges.RemoveAll();
	SetMessageString();
}

//*****************************************************************************
void CWsfQueryView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CWsfQueryDoc &doc = GetDoc();
	int n;

	TRACE1("CWsfQueryView::OnUpdate(): lHint = %d\n", lHint);

	switch (lHint)
	{
	case WSF_UV_DB_HAS_CHANGED:
		{
			ASSERT(pHint);
			CWsfDbEvent& e = *((CWsfDbEvent*)pHint);

			n = doc.OnDbChanged(e);
			if (n == CWsfQueryDoc::DO_NOTHING) break;

			if (!m_bRequery)
			{
				if (n != CWsfQueryDoc::REQUERY) 
				{	
					m_aChanges.GrowBy(1);
					CChangeInfo& ui = m_aChanges.GetLast();
					ui.nChangeType = n;
					ui.nPrimaryKey = e.nPrimaryKey;
				} 
				else m_bRequery = true;
			}

			if (!m_bInOnCmdMsg) PerformChanges();
		}
		break;

	case CWsfQueryDoc::SAVE_PROFILE:
		SaveProfile();
		break;

	case CWsfQueryDoc::RESET:
		OnInitialUpdate();
		break;

	case CWsfQueryDoc::REFRESH:
		BuildList(true);
		break;
	}
}	

//*****************************************************************************
void CWsfQueryView::OnDraw(CDC* pDC) 
{
}

//*****************************************************************************
void CWsfQueryView::OnSize(UINT nType, int cx, int cy) 
{
	m_pList->SetWindowPos(NULL, 0,0, cx,cy, SWP_NOMOVE|SWP_NOZORDER);
	CView::OnSize(nType, cx, cy);
}

//*****************************************************************************
BOOL CWsfQueryView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL b;
	int i;
	CString s;
	CWslArray<int> m_aPrimaryKeys;
	POSITION pos;

	if (pHandlerInfo) return false;
	if (m_bInOnCmdMsg) return false;

	CWsfQueryDoc& doc = GetDoc();

	// --- SEND COMMAND TO THE QUERY ITEM FOR ALL SELECTED ROWS --------------------------------------------

	if (m_pList->GetSelRowCount())
	{
		switch (nCode)
		{
		// CN_COMMAND

		case CN_COMMAND:
			{
				m_bInOnCmdMsg = true;

				// Confirmation of the delete operation
				
				if (nID == ID_WSF_DELETE && !pHandlerInfo)
				{
					if (m_pList->GetSelRowCount() > 1)
					{
						s.Format(m_pList->GetSelRowCount() < 5 ? IDS_REALLY_DELETE_2_TO_4 : IDS_REALLY_DELETE_5_MORE, 
							m_pList->GetSelRowCount());
					}
					else
					{
						VERIFY(s.LoadString(IDS_REALLY_DELETE_ITEM));
					}
					
					if (AfxMessageBox(s, MB_ICONQUESTION|MB_YESNO) != IDYES) { m_bInOnCmdMsg = false; return false; }
				}
				
				// Call command handler for every selected row

				doc.m_nSelCount = m_pList->GetSelRowCount();
				doc.m_bCancelCommand = false;
				doc.m_bFirstItem = true;
				pos = m_pList->GetFirstSelectedRowPosition();
				ASSERT(pos);

				while (!doc.m_bCancelCommand && pos)
				{
					i = m_pList->GetNextSelectedRow(pos);
					doc.m_bLastItem = (pos==NULL);

					doc.m_nPrimaryKey = m_pList->GetItemData(i, 0);
					doc.m_pRowData = (CObject*) m_pList->GetItemData(i, 1);
					
					b = doc.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
					if (!b) break;

					doc.m_bFirstItem = false;
				}

				doc.m_nPrimaryKey = -1;
				doc.m_nSelCount = 0;
				
				// If there was a handler, perform any changes queued so far and quit

				m_bInOnCmdMsg = false; 

				if (b) 
				{ 
					PerformChanges(); 
					return true; 
				}
			}
			break;

		// CN_UPDATE_COMMAND_UI

		case CN_UPDATE_COMMAND_UI:
			{
				CWsfCmdUI	cmdui((CCmdUI*)pExtra);
				pos = m_pList->GetFirstSelectedRowPosition();
				ASSERT(pos);
				doc.m_nSelCount = m_pList->GetSelRowCount();
				doc.m_bFirstItem = true;

				while (pos)
				{
					i = m_pList->GetNextSelectedRow(pos);
					doc.m_bLastItem = (pos==NULL);

					doc.m_nPrimaryKey = m_pList->GetItemData(i, 0);
					doc.m_pRowData = (CObject*) m_pList->GetItemData(i, 1);
					
					b = doc.OnCmdMsg(nID, nCode, (void*) &cmdui, pHandlerInfo);
					if (!b) break;
					if (!cmdui.IsEnabled()) break;
					doc.m_bFirstItem = false;
				}

				doc.m_nPrimaryKey = -1;
				doc.m_nSelCount = 0;

				if (b) return true;
			}
			break;
		}
	}

	// --- SEND IT TO THE TABLE CONTROL AND TO THE DOC/VIEW ---------------------

	if (m_pList->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return true;
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//*****************************************************************************
void CWsfQueryView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	m_pList->SetFocus();	
}

//*****************************************************************************
void CWsfQueryView::OnWsfToogleFilter()
{
	m_bUseFilter ^= 1;
	BuildList(false);
}

//*****************************************************************************
void CWsfQueryView::OnWsfFilterDlg()
{
	if (GetDoc().OnDisplayFilterDlg())
	{
		m_bUseFilter = true;
		BuildList(false);
	}
}

//*****************************************************************************
void CWsfQueryView::OnUpdateWsfToogleFilter(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDoc().m_bFilterDlg);
	pCmdUI->SetCheck(m_bUseFilter);
}

//*****************************************************************************
void CWsfQueryView::OnUpdateWsfFilterDlg(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDoc().m_bFilterDlg);
}

//*****************************************************************************
LRESULT CWsfQueryView::OnWSF_WM_UPDATE_MESSAGE_TEXT(WPARAM wParam, LPARAM lParam)
{
	CString& s = *(CString*) wParam;

	if (m_pList->GetSelRowCount() > 1)
	{
		s.Format(IDS_TOTAL_AND_SELECTED_ITEMS, 
			m_pList->GetRowCount(), m_pList->GetSelRowCount());
	}
	else
	{
		s.Format(IDS_TOTAL_ITEMS, m_pList->GetRowCount());
	}

/*	if (m_bUseFilter)
	{
		CString x;
		x.LoadString(IDS_USING_FILTER);
		s += x;
	}*/

	return true;
}

//*****************************************************************************
void CWsfQueryView::SetMessageString(BOOL bEmpty /*= false*/)
{
	if (WsfGetFrame()->GetActiveView() != this) return;

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
void CWsfQueryView::OnEditSelectAll()
{
	m_pList->SelectAllRows();
}

//*****************************************************************************
void CWsfQueryView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pList->GetRowCount());
}

/*****************************************************************************/
void CWsfQueryView::OnWsfRefresh()
{
	BuildList(true);
}

/*****************************************************************************/
void CWsfQueryView::OnUpdateWsfRefresh(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}





