///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfDocument : public CDocument
{
protected:

public:
	CWsfDocument();
	virtual ~CWsfDocument();
	
	// Sets the modified flag. When the flag is TRUE a star will appear next to 
	// the document's title and when doc should be closed a dialog box
	// will appear asking wheter you want to save the changed document.
	// NOTE: The name of the method is the same as the MFC version. So when working
	// with documents in general, use CWsfDocument NOT CDocument!!!
	
	void SetModifiedFlag(BOOL bModified = true);

	// Returns the document's title. If bUseModifiedFlag is TRUE, a star is appened
	// behind the title if the document is modified.

	CString GetTitle(BOOL bUseModifiedFlag);

  //---------------------------------------------------------------------------
	// From MFC
	//---------------------------------------------------------------------------

	// Sets the document's title. Everything in the framework displaying the title
	// will be updated.

	virtual void SetTitle(LPCTSTR lpszTitle);

	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfDocument)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

