///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class WSL_API CWsfObjectProperties  
{
protected:
   class CWsfPropertySheet* m_pps;

public:
	CWsfObjectProperties();
	virtual ~CWsfObjectProperties();

   // Displays a modal property sheet. 
   BOOL DoModal();

   // Returns the PS
   CWsfPropertySheet* GetPS() { ASSERT(m_pps); return m_pps; }

   // Overload to load/save properties
   protected_ virtual BOOL OnInitProperties() = 0;
   protected_ virtual BOOL OnCloseProperties(BOOL bOk) = 0;

   //---------------------------------------------------------------------------
   // INTERNALS
   //---------------------------------------------------------------------------
private:
	static BOOL PSCallback(void* pData, CWsfPropertySheet& s, BOOL bOk);
};
