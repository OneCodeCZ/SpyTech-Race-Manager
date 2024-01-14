///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfViewMultiplexor : public CView
{
protected:
	CWslArray<CView*> m_aViews;
	int               m_nActiveView; //-1=none
	int					m_nW, m_nH;
	COLORREF				m_rgbBackground;

public:
	CWsfViewMultiplexor();
	virtual ~CWsfViewMultiplexor();

	// Adds a new view and creates it.
	void AddView(CView* pView, const CString& sTitle);

	// Removes the givn view. The view is destroyed and deleted.
	void RemoveView(int nIdx);
   void RemoveView(CView* pView);

	// Sets the active view. 
   // If NULL or -1 is passed, no view is active.
	void SetActiveView(CView* pView);
	void SetActiveView(int nView);

	// Returns the number of views in the multiplexor.
	inline int GetNumViews() const { return m_aViews.GetSize(); }

   //---------------------------------------------------------------------------
   // Internals
   //---------------------------------------------------------------------------
protected:

   // Returns -1 if the view is not found.
   int GetViewIdx(CView* pView) const;

   //---------------------------------------------------------------------------
   // From MFC
   //---------------------------------------------------------------------------

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfViewMultiplexor)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfViewMultiplexor)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

