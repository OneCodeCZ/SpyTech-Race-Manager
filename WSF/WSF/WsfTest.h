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

#pragma once

#define WSFTEST(a) if (!(a)) { ASSERT(false); throw new CWslTextException("%s (%d): Test failed!", __FILE__, __LINE__); }

/*****************************************************************************/
class IWsfTestMsgOutput
{
public:
   virtual void WriteLog(const char* pcszFormat, va_list& l) = 0;
};

/*****************************************************************************/
class WSL_API CWsfTest  
{
private:
   IWsfTestMsgOutput* m_pOutput;

public:
	CWsfTest(IWsfTestMsgOutput* o);
	virtual ~CWsfTest();

   virtual void Run() throw (CException*) = 0;

   void WriteLog(const char* pcszLine, ...);
};
