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

#include "stdafx.h"
#include "main.h"
#include "WslButton.h"

//*****************************************************************************
CWslButton::CWslButton()
{
}

//*****************************************************************************
CWslButton::~CWslButton()
{
}

//*****************************************************************************
BOOL CWslButton::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = "BUTTON";
	return true;
}

/*****************************************************************************/
HANDLE CWslButton::SetImage(HANDLE hImage, int nType)
{
	return (HANDLE) SendMessage(BM_SETIMAGE, (WPARAM) nType, (LPARAM) hImage);
}
