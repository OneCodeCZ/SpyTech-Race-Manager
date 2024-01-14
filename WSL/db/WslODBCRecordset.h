///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 09
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

class CWslDatabase;
class CWslVariant;
class CWslDbCommand;

//*****************************************************************************
class WSL_API CWslOdbcRecordset : public CWslDbRecordset
{
friend CWslOdbcDatabase;

protected:
	HSTMT	m_hstmt;

public:
	CWslOdbcRecordset();
	CWslOdbcRecordset(CWslDatabase* pDb);
	CWslOdbcRecordset(CWslDatabase& db);
	virtual ~CWslOdbcRecordset();

  //---------------------------------------------------------------------------
	// From CWslDbRecordset
	//---------------------------------------------------------------------------

	_public virtual void OnClose();
	_public virtual void MoveNext() throw (CWslException*);
	_protected virtual void OnGetFieldValue(int nCol, CWslVariant& v) throw (CWslException*);
	_protected virtual void OnOpen() throw (CWslException*);
	_protected virtual void OnInitColumn(int nCol) throw (CWslException*);

  //---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------
protected:
	// Gets all data for the given column
	void GetLongBinaryData(short nCol, long nActualSize, int nLen, CWslLongBinary *pBin);
	void GetLongCharData(short nCol, long nActualSize, int nLen, CWslString& sValue);

	// Common to both OnOpen() versions.
	void CommonOnOpenStuff() throw (CWslException*);
};







