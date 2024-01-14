// ComboColorPicker.cpp : implementation file
// © 1997 Baldvin Hansson

#include "stdafx.h"
#include "main.h"
#include "ComboColorPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboColorPicker


CComboColorPicker::CComboColorPicker()
{
	m_bInitialized = false;
}

CComboColorPicker::~CComboColorPicker()
{
}


BEGIN_MESSAGE_MAP(CComboColorPicker, CComboBox)
	//{{AFX_MSG_MAP(CComboColorPicker)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboColorPicker message handlers

int CComboColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitializeData();

	return 0;
}

void CComboColorPicker::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();
	InitializeData();
}

void CComboColorPicker::InitializeData()
{
	int nItem;

	if (m_bInitialized)
		return;

	for (int nColor = 0; nColor < sizeof(g_rgbColors)/sizeof(COLORREF); nColor++)
	{
		nItem = AddString((LPCTSTR)g_rgbColors[nColor]);	// Here we could affect the sort order at run-time

		if (CB_ERRSPACE == nItem)
			break;
	}

	m_bInitialized = true;
}

void CComboColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	CBrush brushBlack;
	brushBlack.CreateStockObject(BLACK_BRUSH);
	
	if (!dc.Attach(lpDrawItemStruct->hDC))
		return;

	COLORREF rgbTextColor = dc.GetTextColor();
	COLORREF rgbBkColor = dc.GetBkColor();

	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
	{
		dc.DrawFocusRect(&lpDrawItemStruct->rcItem);
	}
	else if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			dc.DrawFocusRect(&lpDrawItemStruct->rcItem);
		else
			dc.ExtTextOut(0, 0, ETO_OPAQUE, &lpDrawItemStruct->rcItem, _T(""), 0, NULL);
	}

	if (0 <= (int)lpDrawItemStruct->itemID)	// Any item selected?
	{
		::InflateRect(&lpDrawItemStruct->rcItem, -2, -2);
		
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, (COLORREF)lpDrawItemStruct->itemData);
		dc.FrameRect(&lpDrawItemStruct->rcItem, &brushBlack);
	}

	// Restore the DC state
	dc.SetTextColor(rgbTextColor);
	dc.SetBkColor(rgbBkColor);
	dc.Detach();
}

COLORREF CComboColorPicker::GetSelectedColor()
{
	int nItem = GetCurSel();
	if (CB_ERR == nItem)
		return RGB(0, 0, 0);	// Default to black if nothing is selected

	return g_rgbColors[nItem];
}
