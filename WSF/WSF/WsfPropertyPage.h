///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfDialog.h"

class CWsfPropertySheet;

//*****************************************************************************
class WSL_API CWsfPropertyPage : public CWsfDialog
{
private:
	int		            m_nDialogId;

private:
	CWsfPropertySheet*   m_pps;

public:
	CWsfPropertyPage(int nDialogId);
   DECLARE_DYNAMIC(CWsfPropertyPage)

   //---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------

   // Creates the page. The sheet calls this method.
   BOOL Create(CWsfPropertySheet* pps);

	// Sets page title.
	void SetPageTitle(const char* pcszTitle);

   // Resizes and moves the control to hide its left, right, and bottom border
   void HideControlBorders(int nId, BOOL bBottom = true);

   // Page's sheet
   CWsfPropertySheet* GetSheet() const;

   // Returns TRUE, if the page is active
   BOOL IsPageActive() const;

   //---------------------------------------------------------------------------
	// EVENTS
	//---------------------------------------------------------------------------

	// Called when the page is loosing its activity. 
   // - nNewPage contains the index of the page that will be active. You can change it.
	// - The default implementation calls UpdateData().
	// - If the nNewPage is -1, the "OK" or "FINISH" button was pressed.
   // - Wizard: it's called only when going forward - the NEXT button
	virtual BOOL OnKillActive(int& nNewPage);
	
	// Called when the page is set as active.
	// Default implementation does nothing.
	virtual BOOL OnSetActive();

	// Called when OK or DONE button is pressed.
	// The default implementaton returns TRUE.   
	virtual BOOL OnOkButton();

	// Called when CANCEL button is pressed. OnKillActive is not called.
	// Default implementation does nothing.
	virtual BOOL OnCancelButton();

	// Called when HELP button is pressed.
	// Default implementation does nothing.
	virtual void OnHelpButton();

   //---------------------------------------------------------------------------
   // Internals
   //---------------------------------------------------------------------------
private:
   BOOL IsEditCtrl(CWnd* pWnd);


public:
// Dialog Data
	//{{AFX_DATA(CWsfPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWsfPropertyPage)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWsfPropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

