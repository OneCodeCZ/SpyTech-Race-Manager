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

#pragma once
#include "Shape.h"

/*****************************************************************************/
class CTriangle : public CShape
{
public:
   CWsfOrmInt  m_Alpha;
   CWsfOrmInt  m_Beta;
   CWsfOrmInt  m_Base;

protected:
	CTriangle();
public:
   DECLARE_SERIAL(CTriangle)
   DECLARE_ORM_MAP()

   void Construct(const char* pcszName, int x, int y, int alpha, int beta, int base, CShapePtr pParent);
};
