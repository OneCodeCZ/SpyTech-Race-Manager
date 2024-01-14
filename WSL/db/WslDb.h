///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef _MFC_VER
#include "..\wsf\wsf\WsfSerialMacros.h"
#endif

#include <SQL.H>
#include <SQLEXT.H>

// Generic SQL Database
#include "WslDbVariant.h"
#include "WslDatabase.h"
#include "WslDbRecordset.h"
#include "WslDbCommand.h"
#include "WslDbVarTable.h"
#include "WslSqlScript.h"

// ODBC
#include "WslODBCException.h"
#include "WslODBCDatabase.h"
#include "WslODBCRecordset.h"

