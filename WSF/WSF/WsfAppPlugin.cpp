///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfAppPlugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfPlugin, CCmdTarget)
	//{{AFX_MSG_MAP(CWsfPlugin)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfPlugin::CWsfPlugin()
{
}

/*****************************************************************************/
CWsfPlugin::~CWsfPlugin()
{
}

/*****************************************************************************/
BOOL CWsfPlugin::OnInitInstance()
{ 
   return true;
}

/*****************************************************************************/
void CWsfPlugin::OnExitInstance()
{ 
}




