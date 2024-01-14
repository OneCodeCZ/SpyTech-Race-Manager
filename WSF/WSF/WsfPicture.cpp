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

#include "StdAfx.h"
#include "main.h"
#include "WsfPicture.h"

#include <atlbase.h>
#include <afxdisp.h>
#include <afxpriv2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
CWsfPicture::CWsfPicture()
{
}

/*****************************************************************************/
CWsfPicture::~CWsfPicture()
{
}

/*****************************************************************************/
BOOL CWsfPicture::Load(UINT nIDRes)
{
	// find resource in resource file
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nIDRes), "IMAGE");
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(nIDRes),
		"IMAGE"); // type
	if (!hRsrc)
		return FALSE;

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	HGLOBAL h = LoadResource(hInst, hRsrc);
	ASSERT(h);
	BYTE* lpRsrc = (BYTE*)LockResource(h);
	ASSERT(lpRsrc);

	// create memory file and load it
	CMemFile file(lpRsrc, len);
	BOOL bRet = Load(file);

//	GlobalFree(lpRsrc);
	//FreeResource(hRsrc);
	return bRet;
}

/*****************************************************************************/
BOOL CWsfPicture::Load(LPCTSTR pszPathName)
{
	CFile file;
	if (!file.Open(pszPathName, CFile::modeRead|CFile::shareDenyWrite))
		return FALSE;
	BOOL bRet = Load(file);
	file.Close();
	return bRet;
}

/*****************************************************************************/
BOOL CWsfPicture::Load(CFile& file)
{
	CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
	return Load(ar);
}

/*****************************************************************************/
BOOL CWsfPicture::Load(CArchive& ar)
{
	CArchiveStream arcstream(&ar);
	return Load((IStream*)&arcstream);
}

/*****************************************************************************/
BOOL CWsfPicture::Load(IStream* pstm)
{
	Free();
	HRESULT hr = OleLoadPicture(pstm, 0, FALSE,
		IID_IPicture, (void**)&m_spIPicture);
	if (!SUCCEEDED(hr)) return FALSE;
	if (!m_spIPicture) return FALSE;
//	ASSERT(SUCCEEDED(hr) && m_spIPicture);	
	return TRUE;
}

/*****************************************************************************/
BOOL CWsfPicture::Render(CDC* pDC, CRect rc, LPCRECT prcMFBounds) const
{
	ASSERT(pDC);
	if (!m_spIPicture) return FALSE;
	
	if (rc.IsRectNull()) {
		CSize sz = GetImageSize(pDC);
		rc.right = sz.cx;
		rc.bottom = sz.cy;
	}
	long hmWidth,hmHeight; // HIMETRIC units
	GetHIMETRICSize(hmWidth, hmHeight);
	m_spIPicture->Render(*pDC, rc.left, rc.top, rc.Width(), rc.Height(),
		0, hmHeight, hmWidth, -hmHeight, prcMFBounds);

	return TRUE;
}

/*****************************************************************************/
void CWsfPicture::Render(CDC* pDC, int x, int y)
{
	ASSERT(pDC);
	if (!m_spIPicture) return;

	CSize sz = GetImageSize(pDC);
	long hmWidth, hmHeight;
	GetHIMETRICSize(hmWidth, hmHeight);

	m_spIPicture->Render(*pDC, 
		x, y, sz.cx, sz.cy,
		0, hmHeight, hmWidth, -hmHeight, 
		NULL);
}

/*****************************************************************************/
CSize CWsfPicture::GetImageSize(CDC* pDC) const
{
	if (!m_spIPicture) return CSize(0,0);
	
	LONG hmWidth, hmHeight; // HIMETRIC units
	m_spIPicture->get_Width(&hmWidth);
	m_spIPicture->get_Height(&hmHeight);
	CSize sz(hmWidth,hmHeight);
	if (pDC==NULL) {
		CWindowDC dc(NULL);
		dc.HIMETRICtoDP(&sz); // convert to pixels
	} else {
		pDC->HIMETRICtoDP(&sz);
	}
	return sz;
}

/*****************************************************************************/
int CWsfPicture::GetHeight()
{
	if (!m_spIPicture) return 0;
	LONG n;
	HRESULT hr = m_spIPicture->get_Height(&n);
	ASSERT(SUCCEEDED(hr));	
	return n;
}

/*****************************************************************************/
int CWsfPicture::GetWidth()
{
	if (!m_spIPicture) return 0;
	LONG n;
	HRESULT hr = m_spIPicture->get_Width(&n);
	ASSERT(SUCCEEDED(hr));	
	return n; 
}

