///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  ORM TEST SUITE
// | P |  /  |  |  ~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestLab.h"
#include "OrmTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CColor, CWsfOrmObject, 1)

BEGIN_ORM_MAP(CColor, CWsfOrmObject, "tblColors")
   ORM_PROPERTY(m_r, "r");
   ORM_PROPERTY(m_g, "g");
   ORM_PROPERTY(m_b, "b");
   ORM_PROPERTY(m_sName, "name");
END_ORM_MAP()

/*****************************************************************************/
CColor::CColor()
{
   EnableAutomation();
}

/*****************************************************************************/
void CColor::Construct(const char* pcszName, int r, int g, int b)
{
   CString s;
   m_sName = pcszName;
   m_r = r;
   m_g = g;
   m_b = b;
}
