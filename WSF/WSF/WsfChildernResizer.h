///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// This class resizes the child windows according to the parent's size.
// Each child has its own resizing rules.

#pragma once

//*****************************************************************************
class WSL_API CWsfChildrenResizer  
{
protected:
	// Information about a child window.
	class CChildInfo
	{
	protected:	
		DWORD m_dwFlags;
		float m_fMoveX, m_fResizeX, m_fMoveY, m_fResizeY;
		int	m_nX, m_nY, m_nW, m_nH;		// Initial child size and position
		HWND	m_hWnd;

	public:
		CChildInfo();
      void Construct(HWND hWnd, 
         int nX, int nY, int nW, int nH, 
			float fMoveX, float fResizeX, 
         float fMoveY, float fResizeY);
		
      void Update(int nMinParentW, int nMinParentH, int nParentW, int nParentH);

      HWND GetWnd() const { return m_hWnd; }
	};

protected:
	CWslObArray<CWslPtr<CChildInfo> >	m_aWnds;
	HWND m_hParent;					
	int  m_nMinPW, m_nMinPH;	// Minimal size of the parent wnd

public:
	CWsfChildrenResizer();
	~CWsfChildrenResizer();

	// Sets the parent window for the childern that will be added.
	// The current size of the parent window will be set as the minimim size.
	// Must be called before any other method is called.
	void SetParent(CWnd* pWnd);

	// Adds a new child window to be managed.
   // - The current window size and position within the parent window 
   //   is remembered. These parameters will be kept during
   //   resizing with respect to the resizing parameters.
	// - The current size of the window will be set as the minimum size.
	// - Float values are from the interval <0,1>. 
   //   They specify how much the given parameter will be changed 
   //   depending on the parent window size change.
	void Add(int nId, float fMoveX, float fResizeX, float fMoveY, float fResizeY);
   void Add(HWND hWnd, float fMoveX, float fResizeX, float fMoveY, float fResizeY);

   // The given window will no longer be managed.
   void Remove(HWND hWnd);

	// Resizes all added controls according the specified parent size.
	void Update(int nParentW, int nParentH);

   // Updates size of all controls according the current parent size.
   void Update();
};






