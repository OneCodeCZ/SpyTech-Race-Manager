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

//*****************************************************************************
class CWslWnd  
{
protected:
	static CWslWnd*	m_pWndBeingCreated;
	static BOOL			m_bGenericClassRegistered;
	static HHOOK		m_hCreateWindowHook;

public:
	HWND			m_hWnd;
	WNDPROC 	m_pfnDefWndProc;

public:
  //---------------------------------------------------------------------------
	// Create & destroy stuff
	//---------------------------------------------------------------------------

	// Creates unattached window class
	CWslWnd();

	// Attaches the given handle to the CWslWnd object.
	// See Attach() for details.
	CWslWnd(HWND hWnd);

	// Destroys the window handle! Call Detach() if you
	// don't want to.
	virtual ~CWslWnd();		

	// Creates the Windows window and attaches to this class.
	// Parameters that are missing should be set in the PreCreateWindow() method.
	BOOL Create(CREATESTRUCT& cs);
	BOOL Create(CWslWnd* pParent = NULL, int nId = 0);
	BOOL Create(const char* pcszName, int nStyles, int nExStyles, 
		RECT &r, CWslWnd* pParent, int nId);
	BOOL Create(const char* pcszName, int nStyles, int nExStyles, 
		int x, int y, int w, int h, CWslWnd* pParent, int nId);

	// Destroyes the window
	BOOL DestroyWindow();

	// Attaches the given handle to the CWslWnd object.
	// hWnd can be NULL.
	void Attach(HWND hWnd);

	// Detaches the hwnd form the class. Can be called
	// even if the class is not attached to any handle.
	void Detach();
	
	// Called before the window is about to be created
	_protected virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Called when window is created, but not displayed yet.
	_protected virtual BOOL OnCreate(CREATESTRUCT* pcs);

	// Called when the window is about to be destroyed
	_protected virtual void OnDestroy();

  //---------------------------------------------------------------------------
	// Commands and notify
	//---------------------------------------------------------------------------

	// Here put your command handlers. Return TRUE if the
	// command has been handled.
	_public virtual BOOL OnCommand(int nCmdId);

	// Here put your notify handlers.
	_protected virtual DWORD OnNotify(int nId, NMHDR* pData);

  //---------------------------------------------------------------------------
	// Window management
	//---------------------------------------------------------------------------
public:
	// Win32 functions wrappers
	inline BOOL EnableWindow(BOOL bEnable) { _ASSERT(m_hWnd); return ::EnableWindow(m_hWnd, bEnable); }
	inline BOOL ShowWindow(int nCmd) { _ASSERT(m_hWnd); return ::ShowWindow(m_hWnd, nCmd); }
	inline HWND SetFocus() { _ASSERT(m_hWnd); return ::SetFocus(m_hWnd); }
	inline int GetWindowText(char* pszBuffer, int nBufferSize) { ASSERT(m_hWnd); return ::GetWindowText(m_hWnd, pszBuffer, nBufferSize); }
	inline void SetWindowText(const char* pcszText) { ASSERT(m_hWnd); ::SetWindowText(m_hWnd, pcszText); }

	// Centers the window on the desktop
	void CenterWindow();

  //---------------------------------------------------------------------------
	// Messages handling
	//---------------------------------------------------------------------------

	// Sends message to the window
	inline int SendMessage(int nMsg, WPARAM wParam, LPARAM lParam)
		{ _ASSERT(m_hWnd); return ::SendMessage(m_hWnd, nMsg, wParam, lParam); }

	// Post message to the message queue
	inline BOOL PostMessage(int nMsg, WPARAM wParam, LPARAM lParam)
		{ _ASSERT(m_hWnd); return ::PostMessage(m_hWnd, nMsg, wParam, lParam); }

	// Window procedure. Overload to handle special messages not already
	// handled by some virtual method.
	// Returns 0 if the message was processed.
	_protected virtual int WndProc(int nMsg, DWORD wParam, DWORD lParam); 

	// Calls the default window procedure. Always use this
	// to call the default wndproc!!!!
	_protected int DefWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Passed to the window class.
	_private static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  //---------------------------------------------------------------------------
	// Various
	//---------------------------------------------------------------------------
public:
	operator HWND() { return m_hWnd; }

protected:
	// Hook stuff for creating the window
	HHOOK HookCreateWindow();
	static LRESULT CALLBACK CBTHook(int nCode, WPARAM wParam, LPARAM lParam);
};




