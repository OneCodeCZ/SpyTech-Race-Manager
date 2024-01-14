///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: AdamJ Sporka, Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "generic\WslString.h"
#include "db/wsldb.h"
#include "WslDbVarTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//*****************************************************************************
CWslDbVarTable::CWslDbVarTable(CWslDatabase* pDb)
{
	ASSERT(pDb);
	m_pDb = pDb;
}

/******************************************************************************/
CWslDbVarTable::~CWslDbVarTable()
{
}

//*****************************************************************************
void CWslDbVarTable::Setup(const CWslString& sTableName, 
													 const CWslString& sVarNameCol, 
													 const CWslString& sVarValCol)
{
	m_sTableName = sTableName;
	m_sVarNameCol = sVarNameCol;
	m_sVarValCol = sVarValCol;
}

/******************************************************************************/
void CWslDbVarTable::Add(const char* pcszName)
{
	ASSERT(pcszName);
	CWslString s;
	s.Format("INSERT INTO %s (%s, %s) VALUES ('%s', '')", 
		m_sTableName, m_sVarNameCol, m_sVarValCol, pcszName);
	m_pDb->Execute(s);
}

/******************************************************************************/
void CWslDbVarTable::Remove(const char* pcszName)
{
	ASSERT(pcszName);
	CWslString s;
	s.Format("DELETE FROM %s WHERE %s = '%s'", m_sTableName, m_sVarNameCol, pcszName);
	m_pDb->Execute(s);
}

/******************************************************************************/
void CWslDbVarTable::Set(const char* pcszName, int nNewValue)
{
	ASSERT(pcszName);
	CWslString s;
	s.Format("UPDATE %s SET %s = %d WHERE %s = '%s'", 
		m_sTableName, m_sVarValCol, nNewValue, m_sVarNameCol, pcszName);
	m_pDb->Execute(s);
}

/******************************************************************************/
void CWslDbVarTable::Set(const char* pcszName, const CWslString& sNewValue)
{
	ASSERT(pcszName);
	CWslDbCommand cmd;
	CWslString s;

	cmd.AddParam(sNewValue);
	s.Format("UPDATE %s SET %s=? WHERE %s='%s'", 
		m_sTableName, m_sVarValCol, m_sVarNameCol, pcszName);
	cmd.SetSQL(s);

	m_pDb->Execute(cmd);
}

/******************************************************************************/
int CWslDbVarTable::GetInt(const char* pcszName, int nDefVal /*=0*/)
{
	ASSERT(pcszName);
	CWslString s;
	int nVal;
	CWslDbRecordsetPtr rs = m_pDb->CreateRecordsetObject();

	s.Format("SELECT %s FROM %s WHERE %s = '%s'", m_sVarValCol, m_sTableName, m_sVarNameCol, pcszName);
	rs->Open(s);
	if (rs->IsEOF()) nVal = nDefVal; else rs->GetFieldValue(0, nVal);
	rs->Close();

	return nVal;
}

/******************************************************************************/
CWslString CWslDbVarTable::GetString(const char* pcszName, const char* pcszDefVal /*= NULL*/)
{
	ASSERT(pcszName);
	CWslString s;
	CWslDbRecordsetPtr rs = m_pDb->CreateRecordsetObject();

	s.Format("SELECT %s FROM %s WHERE %s = '%s'", m_sVarValCol, m_sTableName, m_sVarNameCol, pcszName);
	rs->Open(s);
	if (rs->IsEOF()) s = pcszDefVal; else rs->GetFieldValue(0, s);
	rs->Close();

	return s;
}

/*********************************************************************** eof **/
