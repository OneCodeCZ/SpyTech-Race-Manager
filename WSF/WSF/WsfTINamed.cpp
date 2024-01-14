///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTINamed, CWsfTreeItem)
	//{{AFX_MSG_MAP(CWsfTINamed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CWsfTINamed, CWsfTreeItem, 1)

//*****************************************************************************
CWsfTINamed::CWsfTINamed()
{
}

//*****************************************************************************
CWsfTINamed::CWsfTINamed(const char* pcszName) : m_sName(pcszName)
{
}

//*****************************************************************************
CWsfTINamed::CWsfTINamed(const CString& sName) : m_sName(sName)
{
}

//*****************************************************************************
void CWsfTINamed::Serialize(CArchive &ar)
{
	CWsfTreeItem::Serialize(ar);

	if (ar.IsStoring())
		ar << m_sName;
	else
		ar >> m_sName;
}








