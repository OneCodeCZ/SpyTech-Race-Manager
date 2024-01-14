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

BEGIN_MESSAGE_MAP(CWsfTINamedFolder, CWsfTIFolder)
	//{{AFX_MSG_MAP(CWsfTINamedFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CWsfTINamedFolder, CWsfTIFolder, 1)

//*****************************************************************************
CWsfTINamedFolder::CWsfTINamedFolder()
{
}

//*****************************************************************************
CWsfTINamedFolder::CWsfTINamedFolder(const char* pcszName) : m_sName(pcszName)
{
}

//*****************************************************************************
CWsfTINamedFolder::CWsfTINamedFolder(const CString& sName) : m_sName(sName)
{
}

//*****************************************************************************
void CWsfTINamedFolder::Serialize(CArchive &ar)
{
	CWsfTIFolder::Serialize(ar);

	if (ar.IsStoring())
		ar << m_sName;
	else
		ar >> m_sName;
}

