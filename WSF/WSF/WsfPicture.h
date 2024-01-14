///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |	 Original author: Paul DiLascia (MSDN Magazine -- October 2001)
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlbase.h>

/*****************************************************************************/
class WSL_API CWsfPicture 
{
protected:
	CComQIPtr<IPicture>m_spIPicture;		 // ATL smart pointer to IPicture
	HRESULT m_hr;												// last error code

public:
	CWsfPicture();
	~CWsfPicture();

	operator IPicture*() { return m_spIPicture; }
	void Free() { if (m_spIPicture)  m_spIPicture.Release(); }

  //---------------------------------------------------------------------------
	// Load
	//---------------------------------------------------------------------------

	BOOL Load(UINT nIDRes);
	BOOL Load(LPCTSTR pszPathName);
	BOOL Load(CFile& file);
	BOOL Load(CArchive& ar);
	BOOL Load(IStream* pstm);

  //---------------------------------------------------------------------------
	// Rendering 
	//---------------------------------------------------------------------------
	
	BOOL Render(CDC* pDC, CRect rc=CRect(0,0,0,0), LPCRECT prcMFBounds=NULL) const;
	void Render(CDC* pDC, int x, int y);

  //---------------------------------------------------------------------------
	// Properties
	//---------------------------------------------------------------------------
	
	int GetHeight();
	int GetWidth();
	CSize GetImageSize(CDC* pDC=NULL) const;

	void GetHIMETRICSize(OLE_XSIZE_HIMETRIC& cx, OLE_YSIZE_HIMETRIC& cy) const 
	{
		cx = cy = 0;
		const_cast<CWsfPicture*>(this)->m_hr = m_spIPicture->get_Width(&cx);
		ASSERT(SUCCEEDED(m_hr));
		const_cast<CWsfPicture*>(this)->m_hr = m_spIPicture->get_Height(&cy);
		ASSERT(SUCCEEDED(m_hr));
	}
};
