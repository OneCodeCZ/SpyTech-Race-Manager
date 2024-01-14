///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 09 ??
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWslPropertySheet;

//*****************************************************************************
class CWslPropertyPage : public CWslDialog
{
friend CWslPropertySheet;

private:
	int		m_nDialogId;
protected:
	CWslPropertySheet	*m_pps;

public:
	CWslPropertyPage(int nDialogId);
	virtual ~CWslPropertyPage();

  //---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------

	// Sets page title.
	void SetPageTitle(const char* pcszTitle);

  //---------------------------------------------------------------------------
	// Events for overloading
	//---------------------------------------------------------------------------

	// Called when the page is no longer active. nNewPage contains
	// the index of the page that will be active. You can change
	// it here.
	// The default implementation calls UpdateData() here.
	virtual BOOL OnKillActive(int& nNewPage);
	
	// Called when the page is set as active.
	// Default implementation does nothing.
	virtual BOOL OnSetActive();

	// Called when OK or DONE button is pressed. OnKillActive is not called.
	// The default implementaton calls OnKillActive(-1).
	virtual BOOL OnOkButton();

	// Called when CANCEL button is pressed. OnKillActive is not called.
	// Default implementation does nothing.
	virtual BOOL OnCancelButton();

	// Called when HELP button is pressed.
	// Default implementation does nothing.
	_protected virtual void OnHelpButton();
};

