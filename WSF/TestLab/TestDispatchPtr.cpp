///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 10
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestLab.h"
#include "TestDispatchPtr.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CTestDispatchPtr::CTestDispatchPtr(IWsfTestMsgOutput* o)
: CWsfTest(o)
{

}

/*****************************************************************************/
void CTestDispatchPtr::Run() throw (CException*)
{
   CWsfDispatchPtr<CMyClass> p, p2;

   // Try access a NULL pointer

   try
   {
      *p;
      ASSERT(false);
   }
   catch (CException* e)
   {
      e->Delete();
   }

   // Dereferencing

   CMyClass* pCmd = new CMyClass();

   p.Attach(pCmd);
   ASSERT(p);

   ASSERT_VALID(&*p);
   p->AssertValid();

   ((CCmdTarget*)p)->AssertValid();

   // Assigning

   p2 = p;
   p2 = NULL;
   ASSERT_VALID(pCmd);

   // Releasing

   p = NULL;
   ASSERT(!p);
}
