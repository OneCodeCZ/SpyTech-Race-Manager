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
class WSL_API CWsfVSplitter : public CWnd
{
public:
   enum EPanes { LEFT=0, RIGHT=1 };

protected:
	int   m_nWW, m_nWH;		// Window size
	int	m_nSplitterX;		// Position of the splitter
	bool	m_bTracking;		// Are we draging the splitter?
	CWnd* m_pPane[2]; 		// Panes
	HDC	m_hDC;				// Own DC handle

public:
	CWsfVSplitter();
	virtual ~CWsfVSplitter();

	// Sets window for the given pane (NULL = no window).
	// Returns previous pane window.
	CWnd* SetPane(EPanes ePane, CWnd *pWnd, bool bRedraw = true);

	// Settting manualy the splitter position
	void SetSplitterPos(int nX);

	// Retrieving the splitter pos
	inline int GetSplitterPos() const { return m_nSplitterX; }

protected:
	
	// Recalculates size of the panes according the splitter position
	void RecalcLayout();

	// Draws the ghost of the splitter in XOR mode

	void InvertTracker();

	// Draws the splitter

	void DrawSplitter();

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfVSplitter)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfVSplitter)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
