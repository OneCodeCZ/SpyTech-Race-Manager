///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "DocWorkspace.h"

BEGIN_MESSAGE_MAP(CWorkspace, CWsfWorkspaceDoc)
	//{{AFX_MSG_MAP(CWorkspace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWorkspace::CWorkspace()
{
	m_pTIRace = NULL;
}

/*****************************************************************************/
CWorkspace::~CWorkspace()
{
	ASSERT(m_pTIRace == NULL);
}

/*****************************************************************************/
BOOL CWorkspace::OnNewDocument()
{
	// Select RS

	CWslArray<CRuntimeClass*> aClasses;
	aClasses += RUNTIME_CLASS(CTIRS);

	CTIRS* pRS = (CTIRS*) WsfSelectTreeItem(IDS_SELECT_RS, IDS_SELECT_RS_DETAIL,
		g_pApp->m_pTIDbRSFolder,
		&aClasses,
		WsfGetFrame());
	if (!pRS) return false;

	// New race

	m_pTIRace = new CTIMeasuredRace(g_pApp);	

	if (m_pTIRace->DisplayProperties(true) != IDOK)
	{
		delete m_pTIRace;
		m_pTIRace = NULL;
		return false;
	}

	m_pTIRace->AddChild(new CTIResultsFolder());

	// Add RS

	m_pTIRace->SetRS(pRS);

	// Update UI

	InitUI();
	SetModifiedFlag(true);
	return true;
}

/*****************************************************************************/
BOOL CWorkspace::OnOpenDocument(LPCTSTR lpszPathName)
{
	ASSERT(lpszPathName);

	try
	{
		CFile f(lpszPathName, CFile::modeRead);
		CArchive ar(&f, CArchive::load);
		ar >> m_pTIRace;
		ar.Close();
	}
	
	catch (CException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	
	InitUI();
	SetPathName(lpszPathName, true);
	SetModifiedFlag(false);
	return true;
}

/*****************************************************************************/
BOOL CWorkspace::OnSaveDocument(LPCTSTR lpszPathName)
{
	ASSERT(lpszPathName);

	try
	{
		CFile f(lpszPathName, CFile::modeCreate | CFile::modeWrite);
		CArchive ar(&f, CArchive::store);
		ar << m_pTIRace;
		ar.Close();
	}
	
	catch (CException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	SetModifiedFlag(false);
	SetPathName(lpszPathName, true);
	return true;
}

/*****************************************************************************/
void CWorkspace::InitUI()
{
	CString s((char*)IDS_RACE);
	WsfGetApp()->OpenTreeView(m_pTIRace, this, s, -1, true, IDB_SMALL_ICONS, 5);
	WsfGetApp()->TreeHasChanged(m_pTIRace, CWsfApp::ETreeChanges::GENERAL);
	SetTitle(m_pTIRace->m_strName);
}

/*****************************************************************************/
void CWorkspace::OnCloseDocument()
{
	if (m_pTIRace)
	{
		WsfGetApp()->CloseTreeView(m_pTIRace);
		delete m_pTIRace;
		m_pTIRace = NULL;
	}
}

/*****************************************************************************/
BOOL CWorkspace::DoSave(BOOL bSaveAs)
{
	if (bSaveAs || GetPathName().IsEmpty())
	{
		CFileDialog d(false, RACE_EXT, GetPathName(), 
			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER, FILTER_TEMPLATE, WsfGetFrame());
		if (d.DoModal() != IDOK) return false;

		SetPathName(d.GetPathName());
	}

	BOOL b = OnSaveDocument(GetPathName());
	WsfGetApp()->UpdateAppTitle();
	return b;
}







