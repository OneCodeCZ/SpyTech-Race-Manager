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

#pragma once

/*****************************************************************************/
class CTestDispatchPtr : public CWsfTest
{
public:
   class CMyClass : public CCmdTarget
   {
   public:
      CMyClass() { EnableAutomation(); }
   };

public:
	CTestDispatchPtr(IWsfTestMsgOutput* o);

   virtual void Run() throw (CException*);
};
