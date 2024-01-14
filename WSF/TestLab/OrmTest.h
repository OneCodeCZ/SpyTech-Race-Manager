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

class CShape;
class CTriangle;
class CArea;
class CColor;

typedef CWsfOrmPtr<CShape> CShapePtr;
typedef CWsfOrmPtr<CTriangle> CTrianglePtr;
typedef CWsfOrmPtr<CArea> CAreaPtr;
typedef CWsfOrmPtr<CColor> CColorPtr;

#include "Shape.h"
#include "Color.h"
#include "Triangle.h"
#include "Area.H"

/*****************************************************************************/
class CSession
{
protected:
   CWslOdbcDatabase  m_db;
   CWsfOrm           m_orm;

public:
   CSession() throw (CException*);
   virtual ~CSession();

   operator CWsfOrm&() { return m_orm; }
   CWsfOrm& GetOrm() { return m_orm; }

   CColorPtr CreateColor(const char* pcszName, int r, int g, int b) throw (CException*);
   CTrianglePtr CreateTriangle(const char* pcszName, int x, int y, int alpha, int beta, int base, CShapePtr pParent) throw (CException*);
   CAreaPtr CreateArea(const char* pcszName, int x, int y, int nBorderWidth, CShapePtr pParent) throw (CException*);

   void CreateTestData() throw (CException*);
};
