// Area.cpp: implementation of the CArea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestLab.h"
#include "OrmTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CArea, CShape, 1)

BEGIN_ORM_MAP(CArea, CShape, "tblAreas")
   ORM_PROPERTY(m_nBorderWidth, "border_width")
END_ORM_MAP()

/*****************************************************************************/
CArea::CArea()
{
   EnableAutomation();
}

/*****************************************************************************/
void CArea::Construct(const char* pcszName, int x, int y, int nBorderWidth, CShapePtr pParent)
{
   CShape::Construct(pcszName, x,y,pParent);
   m_nBorderWidth = nBorderWidth;
}
