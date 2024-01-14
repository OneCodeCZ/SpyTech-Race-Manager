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
// |___|________|  Last changed: 2004
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class CTestXML : public CWsfTest 
{
public:
	CTestXML(IWsfTestMsgOutput* o);
	virtual ~CTestXML();

   virtual void Run() throw (CException*);

protected:
};
