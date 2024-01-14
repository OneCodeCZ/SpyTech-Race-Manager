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
class CColor : public CWsfOrmObject
{
public:
   CWsfOrmInt  m_r, m_g, m_b;
   CWsfOrmString m_sName;

protected:
	CColor();
public:
   DECLARE_SERIAL(CColor)
   DECLARE_ORM_MAP()

   void Construct(const char* pcszName, int r, int g, int b);
};
