///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WslRebar.h"

//*****************************************************************************
CWslRebar::CWslRebar()
{

}

//*****************************************************************************
CWslRebar::~CWslRebar()
{

}

//*****************************************************************************
BOOL CWslRebar::PreCreateWindow(CREATESTRUCT& cs)
{
	CWslWnd::PreCreateWindow(cs);
	cs.lpszClass = REBARCLASSNAME;
	return true;
}

//*****************************************************************************
BOOL CWslRebar::Create(CWslWnd* pParent)
{
	return CWslWnd::Create(NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		WS_EX_TOOLWINDOW,
		0,0,0,0,
		pParent,
		0);
}

//*****************************************************************************
BOOL CWslRebar::OnCreate(CREATESTRUCT* pcs)
{
	if (!CWslWnd::OnCreate(pcs)) return false;

	// Init rebar	

  REBARINFO     rbi;
	rbi.cbSize = sizeof(REBARINFO);
  rbi.fMask  = 0;
  rbi.himl   = NULL;
  if(!SendMessage(RB_SETBARINFO, 0, (LPARAM)&rbi)) return false;

	return true;
}

//*****************************************************************************
BOOL CWslRebar::AddBand(CWslWnd* pBand, 
												const char* pcszName, 
												int nWidth, 
												int nHeight,
												int nMinWidth,
												BOOL bShowName)
{
	REBARBANDINFO x;
	ASSERT(pcszName);
	ASSERT(pBand);

	x.cbSize = sizeof(x);
	x.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_STYLE;
	if (bShowName) x.fMask |= RBBIM_TEXT;
	x.fStyle = RBBS_GRIPPERALWAYS;
	x.lpText = (char*) pcszName;
	x.hwndChild = *pBand;
	x.cx = nWidth;
	x.cxMinChild = nMinWidth;
	x.cyMinChild = nHeight;

  return SendMessage(RB_SETBANDINFO, 0, (LPARAM)&x);
}






