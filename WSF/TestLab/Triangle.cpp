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

IMPLEMENT_SERIAL(CTriangle, CShape, 1)

BEGIN_ORM_MAP(CTriangle, CShape, "tblTriangles")
   ORM_PROPERTY(m_Alpha, "alpha");
   ORM_PROPERTY(m_Beta, "beta");
   ORM_PROPERTY(m_Base, "base");
END_ORM_MAP()

/*****************************************************************************/
CTriangle::CTriangle()
{
   EnableAutomation();
}

/*****************************************************************************/
void CTriangle::Construct(const char* pcszName, int x, int y, int alpha, int beta, int base, CShapePtr pParent)
{
   CShape::Construct(pcszName, x,y,pParent);
   m_Alpha = alpha;
   m_Beta = beta;
   m_Base = base;
}
