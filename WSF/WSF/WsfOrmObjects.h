///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfOrmCollection.h"

/*****************************************************************************/
class WSL_API CWsfOrmObjects : public CWsfOrmCollection<CWsfOrmObjectPtr>
{
protected:
   CRuntimeClass* m_pClass;

public:
	CWsfOrmObjects(CWsfOrm& orm, CRuntimeClass* pClass);
	virtual ~CWsfOrmObjects();

   //---------------------------------------------------------------------------
   // FROM PARENT
   //---------------------------------------------------------------------------

   protected_ virtual void OnLoad(CWslDatabase& db) throw (CException*);
};
