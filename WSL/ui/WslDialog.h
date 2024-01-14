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

class CWslWnd;

//*****************************************************************************
class CWslDialog : public CWslWnd
{
protected:
	struct CWslDataExchange
	{
		BOOL				bSave;
		CWslDialog*	pDlg;

		// Call when the DDXV function fails
		void Fail(HWND hWnd, BOOL bEditCtrl) throw (int);
	};

public:
	CWslDialog();
	virtual ~CWslDialog();

  //---------------------------------------------------------------------------
	// Create & destroy
	//---------------------------------------------------------------------------

	// Creates modal dialog.
	int DoModal(int nResId, CWslWnd* pParent);
	BOOL DoModal(DLGTEMPLATE* pDlgTmp, CWslWnd* pParent);

	// Creates modeless dialog.
	BOOL Create(int nResId, CWslWnd* pParent);
	BOOL Create(DLGTEMPLATE* pDlgTmp, CWslWnd* pParent);

	// Destroys modal dialog only.
	BOOL EndDialog(int nResult);

	// Called when the dialog is created, but no displayed yet.
	_protected virtual BOOL OnInitDialog();

  //---------------------------------------------------------------------------
	// DDX 
	//---------------------------------------------------------------------------

	// Does the DDX. 
	_protected BOOL UpdateData(BOOL bSave);

	// Overload to implement DDX.
	_protected virtual void DoDataExchange(CWslDataExchange* pDX) throw (int);

  //---------------------------------------------------------------------------
	// Dialog management
	//---------------------------------------------------------------------------
public:
	inline HWND GetDlgItem(int nId) { ASSERT(m_hWnd); return ::GetDlgItem(m_hWnd, nId); }

  //---------------------------------------------------------------------------
	// Message handling
	//---------------------------------------------------------------------------

	// Dialog procedure
	_private static BOOL CALLBACK __DlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

