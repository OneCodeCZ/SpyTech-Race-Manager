///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 09 ??
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CWslTabCtrl : public CWslWnd
{
public:
	CWslTabCtrl();
	virtual ~CWslTabCtrl();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
