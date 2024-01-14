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
class CArea : public CShape
{
public:
   CWsfOrmInt  m_nBorderWidth;

protected:
	CArea();
public:
   DECLARE_SERIAL(CArea)
   DECLARE_ORM_MAP()

   void Construct(const char* pcszName, int x, int y, int nBorderWidth, CShapePtr pParent);
};
