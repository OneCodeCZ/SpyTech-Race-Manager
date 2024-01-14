///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 07 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// Workspace represents a race file

#pragma once

/*****************************************************************************/
class RMDM_API CWorkspace : public CWsfWorkspaceDoc
{
public:
	CTIMeasuredRace*	m_pTIRace;

public:
  //---------------------------------------------------------------------------
	// Public API
	//---------------------------------------------------------------------------

	CWorkspace();
	virtual ~CWorkspace();

  //---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------
protected:

	// Inits the UI when a new race is created or opened
	void CWorkspace::InitUI();

  //---------------------------------------------------------------------------
	// From WSF
	//---------------------------------------------------------------------------

	_public virtual BOOL OnNewDocument();
	_public virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	_public virtual BOOL OnSaveDocument( LPCTSTR lpszPathName);
	_public virtual void OnCloseDocument();
	_public	virtual BOOL DoSave(BOOL bSaveAs);

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CWorkspace)
	//}}AFX_MSG
};
