///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include <db\WslDb.h>
#include "WsfQueryDoc.h"
#include "WsfQueryView.h"
#include "WsfTIQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTIQuery, CWsfTreeItem)
	//{{AFX_MSG_MAP(CWsfTIQuery)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WSL_IMPLEMENT_SERIAL_ABSTRACT_DLL(WSL_API, CWsfTIQuery, CWsfTreeItem, 1)


//*****************************************************************************
CWsfTIQuery::CWsfTIQuery()
{
	m_pDocData = NULL;
	m_nDocDataSize = 0;
	m_bUseFilter = false;
}

//*****************************************************************************
CWsfTIQuery::~CWsfTIQuery()
{
	free(m_pDocData);
}

//*****************************************************************************
void CWsfTIQuery::Serialize(CArchive &ar)
{
	CWsfTreeItem::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_nDocDataSize;
		ar << m_bUseFilter;
		if (m_nDocDataSize) ar.Write(m_pDocData, m_nDocDataSize);
	}
	else
	{
		ar >> m_nDocDataSize;
		ar >> m_bUseFilter;
		if (m_nDocDataSize) 
		{
			free(m_pDocData);
			m_pDocData = malloc(m_nDocDataSize);
			ar.Read(m_pDocData, m_nDocDataSize);
		}
	}
}

/*
//*****************************************************************************
BOOL CWsfTIQuery::StandardOnOpenImpl(CRuntimeClass* pDocClass)
{
	ASSERT(pDocClass);

	if (WsfGetApp()->ActivateView(m_nQueryViewID)) return true;

	// Doc

	CDocument* pDoc = (CDocument*) pDocClass->CreateObject();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CDocument)));

	WsfGetApp()->OpenDoc(pDoc);

	// View

	CView* pView = new CWsfQueryView();
	m_nQueryViewID = WsfGetApp()->OpenView(pView, pDoc, GetTitle());
	
	if (!m_nQueryViewID)
	{
		delete pView;
		WsfGetApp()->CloseDoc(pDoc);
		return false;
	}

	return true;
}
*/

//*****************************************************************************
BOOL CWsfTIQuery::LoadProfile(void **ppData, int *pnSize)
{
	if (!m_pDocData) return false;

	*ppData = new BYTE[m_nDocDataSize];
	memcpy(*ppData, m_pDocData, m_nDocDataSize);
	*pnSize = m_nDocDataSize;

	return true;
}

//*****************************************************************************
BOOL CWsfTIQuery::SaveProfile(void *pData, int nSize)
{
	free(m_pDocData);
	m_pDocData = malloc(nSize);
	memcpy(m_pDocData, pData, nSize);
	m_nDocDataSize = nSize;
	return true;
}







