///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 10
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfTableCtrl.h"
#include <db\WslDb.h>
#include "WsfQueryView.h"
#include "WsfQueryDoc.h"
#include "WsfPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWsfQueryDoc, CDocument)

BEGIN_MESSAGE_MAP(CWsfQueryDoc, CDocument)
	//{{AFX_MSG_MAP(CWsfQueryDoc)
	ON_COMMAND(ID_WSF_PROPERTIES, OnWsfProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfQueryDoc::CWsfQueryDoc(int nMenuId, 
													 int nSubMenuIdx,	 
													 BOOL bFilterDlg,
													 int nDefCmd)
{
	m_dwMenu = nMenuId + (nSubMenuIdx << 16);
	m_bFilterDlg = bFilterDlg;
	m_nDefCmd	= nDefCmd;
	m_nPrimaryKeyColIdx = -1;
	m_bFilterDlg = bFilterDlg;
	m_nSortColIdx = -1;
	m_nSelCount = 0;
}

//*****************************************************************************
CWsfQueryDoc::~CWsfQueryDoc()
{
	for (int i = m_aViewCols.GetSize(); i--;) delete m_aViewCols[i];
	for (i = m_aQueryCols.GetSize(); i--;) delete m_aQueryCols[i];
}


/*****************************************************************************
*
* COLUMNS
*
*****************************************************************************/

//*****************************************************************************
void CWsfQueryDoc::AddTextViewCol(const char* pcszTitle, 
															int nQueryColIdx,
															int nWidth,
															DWORD dwFlags /*= see the header file*/)
{
	ASSERT(pcszTitle); 
	ASSERT(nWidth > 0);

	CWsfViewColumn* p = new CWsfViewColumn();
	ASSERT(p);
	m_aViewCols += p;

	p->sTitle = pcszTitle;
	p->nWidth = nWidth;
	p->nImage = -1;
	p->dwFlags = dwFlags;
	p->nQueryColIdx = nQueryColIdx;
}

//*****************************************************************************
void CWsfQueryDoc::AddIconViewCol(const char* pcszTitle, 
															int nQueryColIdx,
															int nIconBmpId,
															int nIconIdx,
															DWORD dwFlags /*= see the header file*/)
{
	ASSERT(pcszTitle); 

	CWsfViewColumn* p = new CWsfViewColumn();
	ASSERT(p);
	m_aViewCols += p;

	p->sTitle = pcszTitle;
	p->nWidth = 0;		// not important
	p->nImage = WsfGetApp()->GetColumnIconIdx(nIconBmpId, nIconIdx);
	p->dwFlags = dwFlags;
	p->nQueryColIdx = nQueryColIdx;
}

//*****************************************************************************
void CWsfQueryDoc::AddQueryCol(const char* pcszName, 
															 BOOL bPrimaryKey /*= false*/)
{
	ASSERT(pcszName);

	CWsfQueryColumn* p = new CWsfQueryColumn();
	ASSERT(p);
	m_aQueryCols += p;

	p->sTitle = pcszName;
	p->bPrimaryKey = bPrimaryKey;

	if (bPrimaryKey) m_nPrimaryKeyColIdx = m_aQueryCols.GetSize() - 1;
}

/*****************************************************************************/
void CWsfQueryDoc::SetSortCol(int nColIdx, BOOL bAsc)
{
	ASSERT(nColIdx>=0 && nColIdx<m_aViewCols.GetSize());
	m_nSortColIdx = nColIdx;
	m_bSortAsc = bAsc;
}

/*****************************************************************************
*
* CUSTOMIZABLE METHODS
*
*****************************************************************************/

//*****************************************************************************
void CWsfQueryDoc::OnGetViewColumnValue(int nViewColIdx, CWsfTableCtrl::CItem& lvi)
{
	ASSERT(nViewColIdx >=0 && nViewColIdx < m_aViewCols.GetSize());

	// Get the query col associated with the nColIdx view col.

	int n = m_aViewCols[nViewColIdx]->nQueryColIdx;
	ASSERT(n >=0 && n < m_aQueryCols.GetSize());
	CWslVariant& v = m_aQueryCols[n]->v;

	// Set the LVI according the value type

	switch (v.GetType())
	{
	case CWslVariant::VT_LONG:
		lvi.sText.Format("%d", v.nVal);
		break;

	case CWslVariant::VT_STRING:
		lvi.sText = *v.pString;
		break;

	case CWslVariant::VT_DATE:
		lvi.sText.Format("%d.%d.%d %02d:%02d:%02d", 
			v.pDate->day, v.pDate->month, v.pDate->year,
			v.pDate->hour, v.pDate->minute, v.pDate->second);
		break;

	case CWslVariant::VT_NULL:
		lvi.sText = "–";
		break;

	default:
		lvi.sText = "BAD TYPE";
		break;
	}
}

//*****************************************************************************
int CWsfQueryDoc::OnDbChanged(CWsfDbEvent& e)
{
	int n;

	switch (e.eEvent)
	{
		case CWsfDbEvent::GENERAL		: n = REQUERY; break;
		case CWsfDbEvent::ROW_ADDED		: n = UPDATE_ROW; break; 
		case CWsfDbEvent::ROW_UPDATED	: n = UPDATE_ROW; break;
		case CWsfDbEvent::ROW_DELETED	: n = DELETE_ROW; break;
		default										: ASSERT(false); n = DO_NOTHING;
	}

	return n;
}

/*****************************************************************************
*
* QUERY ROW METHODS
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfQueryDoc::PSCallback(void* pData, CWsfPropertySheet& ps, BOOL bOk)
{
	ASSERT(pData);
	return ((CWsfQueryDoc*)pData)->OnCloseProperties(ps, bOk);
}

/*****************************************************************************/
int CWsfQueryDoc::DisplayProperties()
{
	CWsfPropertySheet ps(GetPrimaryKey() == -1, PSCallback, this);

	// Call user to init the ps

	if (!OnInitProperties(ps))
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

//*****************************************************************************
void CWsfQueryDoc::OnWsfProperties()
{
	DisplayProperties();
}

//*****************************************************************************
BOOL CWsfQueryDoc::DisplayNewItemProperties()
{
	m_nPrimaryKey = -1;
	m_pRowData = NULL;
	m_nSelCount = 1;
	m_bFirstItem = true;
	m_bLastItem = true;

	int n = DisplayProperties();

	m_nSelCount = 0;

	return n==IDOK;
}









