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

/*****************************************************************************/
class CShape : public CWsfOrmObject
{
public:
   CWsfOrmInt    m_x;
   CWsfOrmInt    m_y;
   CWsfOrmString m_sName;
   CShapePtr     m_pParent;
   CColorPtr     m_pColor;

protected:
	CShape();
public:
   DECLARE_SERIAL(CShape)
   DECLARE_ORM_MAP()

   void Construct(const char* pcszName, int x, int y, CShapePtr pParent);
};
