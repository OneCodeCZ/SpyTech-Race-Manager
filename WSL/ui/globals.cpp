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
#include "globals.h"

//*****************************************************************************
int WslMsgBox(const char* pcszText, int nFlags /*= MB_ICONECLAMATION|MB_OK*/)
{
	ASSERT(pcszText);
	ASSERT(g_pApp);
	return MessageBox(::GetLastActivePopup(g_pApp->m_hMainWnd), 
		pcszText, g_pApp->m_sAppName, nFlags);
}

