///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  XML TEST SUITE
// | P |  /  |  |  ~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 11
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestLab.h"
#include "TestXML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CTestXML::CTestXML(IWsfTestMsgOutput* o) : CWsfTest(o)
{

}

/*****************************************************************************/
CTestXML::~CTestXML()
{

}

/*****************************************************************************/
void CTestXML::Run() throw (CException*)
{  
   CWslXMLParser p;
   CWslXMLElementPtr e;
   CString s;

   e = p.ParseString("<div/>", 1250);
   e = p.ParseString("<div></div>", 1250);

   e = p.ParseString("<a href=\"1&amp;2\"/>", 1250);
   s = e->GetAttributeString("href");
   ASSERT(s == "1&2");
}
