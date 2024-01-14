///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 12
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWslWnd;
class CWslDialog;
class CWslWaitCursor;

//*****************************************************************************
class CWslApp  
{
friend CWslWaitCursor;

protected:
	struct CWndInfo
	{
		HWND				hWnd;
		CWslWnd*		pWnd;
	};

protected:
	CWslArray<CWndInfo>		m_aWnds;
	HFONT							m_hBoldDlgFont, m_hUnderlinedDlgFont;

	// Wait cursor
	static int						m_nWaitCursorRef;	// reference counter
	static HCURSOR				m_hWaitCursor;		// wait cursor
	static HCURSOR				m_hOldCursor;			// cursor backup for restoring it later

public:
	HINSTANCE			m_hInst;
	CWslString		m_sAppName;		// you should init this variable in OnInitInstance()!
	HWND					m_hMainWnd;		// you should init this variable in OnInitInstance()!
	CWslString		m_sAppPath;		// includes the trailing slash!

public:
	CWslApp();
	virtual ~CWslApp();

  //---------------------------------------------------------------------------
	// App start-up
	//---------------------------------------------------------------------------

	// Call to start the message loop.
	void Run();

	// Call on app init.
	// Use should set the m_hMainWnd member here.
	_public virtual BOOL OnInitInstance(HINSTANCE hInst, const char* pcszCmdLine);

	// Call on app exit.
	_public virtual void OnExitInstance();

  //---------------------------------------------------------------------------
	// Messages
	//---------------------------------------------------------------------------

	// Called after the message is retrieved from the queue and before
	// it's translated. You can modify the message. 
	// Return TRUE if the message was fully processed and should
	// not be processed further.
	_protected virtual BOOL PreTranslateMessage(MSG& m);

  //---------------------------------------------------------------------------
	// Window management
	//---------------------------------------------------------------------------
public:
	void AddWnd(CWslWnd* pWnd);
	void RemoveWnd(CWslWnd* pWnd);
	CWslWnd* GetWnd(HWND hWnd);

  //---------------------------------------------------------------------------
	// Various
	//---------------------------------------------------------------------------

	// Sets bold dialog font in the given child window.
	void SetBoldFont(CWslDialog* pDlg, int nChildId);
	void SetUnderlinedFont(CWslDialog* pDlg, int nChildId);
};

//*****************************************************************************
class CWslWaitCursor
{
public:
	CWslWaitCursor();
	~CWslWaitCursor();
};

//*****************************************************************************
inline CWslApp& WslGetApp() { ASSERT(g_pApp); return *g_pApp; }


