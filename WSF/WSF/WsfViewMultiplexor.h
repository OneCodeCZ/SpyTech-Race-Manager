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
class WSL_API CWsfViewMultiplexor : public CView
{
protected:
	CWslArray<CView*> m_aViews;
	int								m_nActiveView; //-1=none
	int								m_nW, m_nH;
	CString						m_sNoViewText;
	COLORREF					m_rgbBackground;

public:
	CWsfViewMultiplexor(const char* pcszNoViewText);
	virtual ~CWsfViewMultiplexor();

	// Adds new view. The view is created, so it must not be create
	// before.
	void AddView(CView* pView);

	// Removes view. The view is deleted.
	void RemoveView(int nIdx);

	// Sets active view. If NULL or -1 is passed, no view is active.
	void SetActiveView(CView* pView);
	void SetActiveView(int nView);

	// Returns the number of views in the multiplexor
	inline int GetNumViews() const { return m_aViews.GetSize(); }

	// From MFC

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfViewMultiplexor)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfViewMultiplexor)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

