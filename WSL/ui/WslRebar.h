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

#pragma once

//*****************************************************************************
class CWslRebar : public CWslWnd
{
public:
	CWslRebar();
	virtual ~CWslRebar();

	BOOL Create(CWslWnd* pParent);

	BOOL AddBand(CWslWnd* pBand, 
		const char* pcszName, 
		int nWidth, 
		int nHeight,
		int nMinWidth,
		BOOL bShowName);

	
	// From WSL

	_protected virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	_protected virtual BOOL OnCreate(CREATESTRUCT* pcs);
};
