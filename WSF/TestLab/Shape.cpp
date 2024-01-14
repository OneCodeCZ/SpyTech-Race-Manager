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

IMPLEMENT_SERIAL(CShape, CWsfOrmObject, 1)

BEGIN_ORM_MAP(CShape, CWsfOrmObject, "tblShapes")
   ORM_PROPERTY(m_x, "x");
   ORM_PROPERTY(m_y, "y");
   ORM_PROPERTY(m_sName, "name");
   ORM_RELATIONSHIP(m_pParent, "parent_id", CShape, true);
   ORM_RELATIONSHIP(m_pColor, "color_id", CShape, false);
END_ORM_MAP()

/*****************************************************************************/
CShape::CShape()
{
}

/*****************************************************************************/
void CShape::Construct(const char* pcszName, int x, int y, CShapePtr pParent)
{
   m_sName = pcszName;
   m_x = x;
   m_y = y;
   m_pParent = pParent;
}

