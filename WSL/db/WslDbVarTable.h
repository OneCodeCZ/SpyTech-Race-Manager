///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: AdamJ Sporka, Radek Tetik
// | C |  |__|  |  Last changed: 2001 12 21
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "..\generic\WslString.h"

//*****************************************************************************
class WSL_API CWslDbVarTable
{
protected:
	CWslDatabase*			m_pDb;
	CWslString				m_sTableName;
	CWslString				m_sVarNameCol;
	CWslString				m_sVarValCol;

public:
	CWslDbVarTable(CWslDatabase* pDb);
	virtual ~CWslDbVarTable();

	// Sets the table name and the column names
	void Setup(const CWslString& sTableName, 
		const CWslString& sVarNameCol, 
		const CWslString& sVarValCol);

	// Adds/removes the a variable
	void Add(const char* pcszName) throw (CWslException);
	void Remove(const char* pcszName) throw (CWslException);

	// Sets variable's value
	void Set(const char* pcszName, const CWslString& sNewValue) throw (CWslException);
	void Set(const char* pcszName, int nNewValue) throw (CWslException);

	// Gets variable's value
	int GetInt(const char* pcszName, int nDefVal = 0) throw (CWslException);
	CWslString GetString(const char* pcszName, const char* pcszDefVal = NULL) throw (CWslException);
};

/*********************************************************************** eof **/
