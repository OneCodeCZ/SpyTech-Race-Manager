///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 07 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfApp;

//*****************************************************************************
class WSL_API CWsfWorkspaceDoc : public CDocument
{
friend CWsfApp;

protected:

public:
	CWsfWorkspaceDoc();
	virtual ~CWsfWorkspaceDoc();

	// Called when the workspace should be saved.
	// If the bSaveAs is true or the workspace is new,
	// you should display a "save as" dialog.
	// NOTE: You should also call CWsfApp::UpdateAppTitle after
	// the document is saved or its modification state is changed.
	// Return TRUE if the file was successfully saved.
	virtual BOOL DoSave(BOOL bSaveAs) { ASSERT(false); return false; }

	// Called when document is about to be closed.
	// NOTE: Called even if OnNewDocument or OnOpenDocument fails.
	virtual void OnCloseDocument() { }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfWorkspaceDoc)
	public:
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfWorkspaceDoc)
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

