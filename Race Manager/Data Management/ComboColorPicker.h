#if !defined(AFX_COMBOCOLORPICKER_H__B2348841_5541_11D1_8756_00A0C9181E86__INCLUDED_)
#define AFX_COMBOCOLORPICKER_H__B2348841_5541_11D1_8756_00A0C9181E86__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ComboColorPicker.h : header file
// © 1997 Baldvin Hansson

/////////////////////////////////////////////////////////////////////////////
// CComboColorPicker window

class CComboColorPicker : public CComboBox
{
// Construction
public:
	CComboColorPicker();

// Attributes
private:
	bool m_bInitialized;
	static COLORREF m_rgStandardColors[];
public:

// Operations
private:
	void InitializeData();
public:
	COLORREF GetSelectedColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboColorPicker)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CComboColorPicker();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboColorPicker)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOCOLORPICKER_H__B2348841_5541_11D1_8756_00A0C9181E86__INCLUDED_)
