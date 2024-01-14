///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfOrmModelDependency.h"

class CWsfOrmModelClassProperty;
typedef CWslPtr<CWsfOrmModelClassProperty> CWsfOrmModelClassPropertyPtr;

/*****************************************************************************/
class WSL_API CWsfOrmModelClassProperty 
{
public:
   CString m_sColumnName;

public:
   CWsfOrmModelClassProperty() { }
};
