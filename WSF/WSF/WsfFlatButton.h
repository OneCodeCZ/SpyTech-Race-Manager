///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

// NOTE: Everything possible is recomputed just-in-time to reduce memory usage.

#pragma once

// m_dwFlags

#define WSF_FLB_LBUTTON_DOWN		1
#define WSF_FLB_BORDER					2
#define WSF_FLB_CAPTURE					4
#define WSF_FLB_INSIDE					8

//*****************************************************************************
class WSL_API CWsfFlatButton : public CWnd
{
protected:
  DWORD			m_dwFlags;
  HBITMAP		m_hBitmap;
	HBITMAP		m_hBitmapDisabled;

public:
	CWsfFlatButton();
	~CWsfFlatButton();

	// Creates the flat button.

	BOOL Create(DWORD dwStyle, 
		RECT *pRect, 
		int nBitmap, 
		int nBitmapDisabled,
		CWnd *pParent, 
		int	nId);

protected:

	// Draws border around the button.

  void DrawBorder(HDC hDC, RECT *pRect);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfFlatButton)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWsfFlatButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
