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

class CWsfOrmModelDependency;
typedef CWslPtr<CWsfOrmModelDependency> CWsfOrmModelDependencyPtr;

/*****************************************************************************/
class WSL_API CWsfOrmModelDependency  
{
public:
   CWsfOrmModelClass* m_pMasterClass;
   CWsfOrmModelClass* m_pSlaveClass;
   CString m_sColumn;
   BOOL    m_bOnDeleteSetNull;

public:
	CWsfOrmModelDependency();
	virtual ~CWsfOrmModelDependency();
};
