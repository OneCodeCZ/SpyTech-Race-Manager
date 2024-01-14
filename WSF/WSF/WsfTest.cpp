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
// |___|________|  Last changed: 2004 01
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfTest::CWsfTest(IWsfTestMsgOutput* o)
{
   m_pOutput = o;
}

/*****************************************************************************/
CWsfTest::~CWsfTest()
{
}

/*****************************************************************************/
void CWsfTest::WriteLog(const char* pcszFormat, ...)
{
   if (m_pOutput)
   {
	   va_list l;
	   va_start(l, pcszFormat);
      m_pOutput->WriteLog(pcszFormat, l);
	   va_end(l);
   }
}
