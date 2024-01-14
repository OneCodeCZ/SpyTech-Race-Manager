///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 01
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"

//*****************************************************************************
CWslDbRecordset::CWslDbRecordset()
{
	m_pDb = NULL;
}

//*****************************************************************************
CWslDbRecordset::CWslDbRecordset(CWslDatabase* pDb)
{
	m_pDb = pDb;
}

//*****************************************************************************
CWslDbRecordset::CWslDbRecordset(CWslDatabase& db)
{
	m_pDb = &db;
}

//*****************************************************************************
CWslDbRecordset::~CWslDbRecordset()
{
}

//*****************************************************************************
void CWslDbRecordset::SetDb(CWslDatabase* pDb)
{
	Close();
	m_pDb = pDb;
}

//*****************************************************************************
void CWslDbRecordset::SetDb(CWslDatabase& db)
{
	Close();
	m_pDb = &db;
}

//*****************************************************************************
void CWslDbRecordset::Open(CWslDbCommand& cmd)
{
	ASSERT(m_pDb && m_pDb->IsOpened());
	Close();

	m_pDb->OnExecute(cmd, this);
	OnOpen();

	m_bEOF = false;
	MoveNext();
}

//*****************************************************************************
void CWslDbRecordset::Open(const char* pcszSQL, ...) throw (CWslException*)
{
	ASSERT(pcszSQL);
	ASSERT(m_pDb);

	char temp[4096];
	va_list l;
	va_start(l, pcszSQL);
	BOOL b = _vsnprintf(temp, 4096, pcszSQL, l) >= 0;
	temp[4096-1] = 0;
	va_end(l);

   if (!b) throw new CWslTextException("CWslDbRecordset::Open(): SQL command too long!");

	m_pDb->OnExecute(temp, this);
	OnOpen();

	m_bEOF = false;
	MoveNext();
}

//*****************************************************************************
void CWslDbRecordset::OpenNoFormat(const char* pcszSQL) throw (CWslException*)
{
	ASSERT(pcszSQL);
	ASSERT(m_pDb);

	m_pDb->OnExecute(pcszSQL, this);
	OnOpen();

	m_bEOF = false;
	MoveNext();
}

//*****************************************************************************
void CWslDbRecordset::Close()
{
	if (!m_pDb) return;
	OnClose();
	for (int i=m_aCols.GetSize(); i--;) delete m_aCols[i];
	m_aCols.RemoveAll();
}

/*****************************************************************************
*
* RETRIEVING DATA
*
*****************************************************************************/

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(const char* pcszCol, CWslVariant &v)
{
	ASSERT(pcszCol);
   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(int nCol, CWslVariant &v)
{
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 
	OnGetFieldValue(nCol, v);
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(int nCol, CWslString &s)
{
	CWslVariant v;

	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 

	OnGetFieldValue(nCol, v);

	if (v.GetType() == CWslVariant::VT_NULL)
	{
		s.Empty();
	}
	else
	{
		ASSERT(v.GetType() == CWslVariant::VT_STRING);
		s = *v.pString;
	}
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(const char* pcszCol, CWslString &s)
{
	CWslVariant v;

	ASSERT(pcszCol);

   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);

   switch (v.GetType())
   {
   case CWslVariant::VT_STRING:  ASSERT(v.pString); s = *v.pString; break;
   case CWslVariant::VT_NULL:    s.Empty(); break;
   default:                      ASSERT(false); s.Empty(); break;
	}
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(const char* pcszCol, float& fVal, float fNullVal /*= 0*/)
{
	CWslVariant v;

	ASSERT(pcszCol);

   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);

	if (v.GetType() == CWslVariant::VT_NULL)
	{
		fVal = fNullVal;
	}
	else
	{
		ASSERT(v.GetType() == CWslVariant::VT_FLOAT);
		fVal = v.fVal;
	}
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(const char* pcszCol, double& dblVal, double dblNullVal /*= 0*/)
{
	CWslVariant v;
	ASSERT(pcszCol);

   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);

	if (v.GetType() == CWslVariant::VT_NULL)
	{
		dblVal = dblNullVal;
	}
	else
	{
		if (v.GetType() == CWslVariant::VT_DOUBLE)
			dblVal = v.dblVal;
		else if (v.GetType() == CWslVariant::VT_FLOAT)
			dblVal = (double) v.fVal;
		else ASSERT(false);		// other conversion on demand :-)
	}
}

/*****************************************************************************/
void CWslDbRecordset::GetFieldValue(const char* pcszCol, CWslLongBinary& lb) throw (CWslException*)
{
	CWslVariant v;
	ASSERT(pcszCol);

   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);
	
	if (v.GetType() == CWslVariant::VT_BINARY)
	{
		int n = v.pBinary->GetSize();
		lb.Attach(v.pBinary->Detach(), n);
	}
	else 
	{
		lb.Free();
		ASSERT(false);
	}
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(const char* pcszCol, int& nVal, int nNullVal /*= 0*/)
{
	ASSERT(pcszCol);
	GetFieldValue(GetColIdx(pcszCol), nVal, nNullVal);
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(int nCol, CWslDateTime& dt)
{
	CWslVariant v;
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 

	OnGetFieldValue(nCol, v);

	if (v.GetType() == CWslVariant::VT_NULL)
    dt.SetNull();
  else if (v.GetType() == CWslVariant::VT_DATE)
		dt.Set(v.pDate->year,v.pDate->month,v.pDate->day,v.pDate->hour,v.pDate->minute,v.pDate->second);
  else
  {
    ASSERT(false);
    dt.SetNull();
  }
}

/*****************************************************************************/
void CWslDbRecordset::GetFieldValue(const char* pcszCol, CWslDate& date) throw (CWslException*)
{
   CWslVariant v;
   ASSERT(pcszCol);
   
   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);
   
   if (v.GetType() == CWslVariant::VT_NULL)
      date.SetNull();
   else if (v.GetType() == CWslVariant::VT_DATE)
      date.Set(v.pDate->year,v.pDate->month,v.pDate->day);
   else
   {
      ASSERT(false);
      date.SetNull();
   }
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(const char* pcszCol, CWslDateTime& dt)
{
	ASSERT(pcszCol);
	GetFieldValue(GetColIdx(pcszCol), dt);
}

//*****************************************************************************
void CWslDbRecordset::GetFieldValue(int nCol, int &nVal, int nNullVal /*= 0*/)
{
   CWslVariant v;
   ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 
   
   OnGetFieldValue(nCol, v);
   
   switch (v.GetType())
   {
   case CWslVariant::VT_NULL:  nVal = nNullVal; break;
   case CWslVariant::VT_LONG:  nVal = v.nVal; break;
   case CWslVariant::VT_BYTE:  nVal = v.byVal; break;
   case CWslVariant::VT_SHORT: nVal = v.wVal; break;
   case CWslVariant::VT_BOOL:  nVal = v.bVal; break;
   default: throw new CWslTextException("CWslDbRecordset::GetFieldValue(): Type mismatch! Cannot convert to 32b int.");
   }
}

/*****************************************************************************/
void CWslDbRecordset::GetFieldValue(const char* pcszCol, bool& bVal, bool bNullVal) throw (CWslException*)
{
   CWslVariant v;

   int nIdx = GetColIdx(pcszCol);
   if (nIdx == -1) { ASSERT(false); throw new CWslTextException("CWslDbRecordset: Unknown field \"%s\"", pcszCol); }
	OnGetFieldValue(nIdx, v);

   bVal = (bool) v;
}

/*****************************************************************************/	
void CWslDbRecordset::GetFieldValue(int nCol, 
                                    __int64& qVal, 
                                    __int64 qNullVal /*= 0*/) throw (CWslException*)
{
	CWslVariant v;
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 

	OnGetFieldValue(nCol, v);

  switch (v.GetType())
  {
  case CWslVariant::VT_NULL:    qVal = qNullVal; break;
  case CWslVariant::VT_LONG:    qVal = v.nVal; break;
  case CWslVariant::VT_BYTE:    qVal = v.byVal; break;
  case CWslVariant::VT_SHORT:   qVal = v.wVal; break;
  case CWslVariant::VT_BOOL:    qVal = v.bVal; break;
  case CWslVariant::VT_BIGINT:  qVal = v.qVal; break;
  default: throw new CWslTextException("CWslDbRecordset::GetFieldValue(): Type mismatch! Cannot convert to 64b int.");
  }
}

/*****************************************************************************/	
void CWslDbRecordset::GetFieldValue(const char* pcszCol, 
                                    __int64& qVal, 
                                    __int64 qNullVal /*= 0*/) throw (CWslException*)
{
   ASSERT(pcszCol);
	GetFieldValue(GetColIdx(pcszCol), qVal, qNullVal);
}

/*****************************************************************************
*
* COLUMNS
*
*****************************************************************************/

//*****************************************************************************
int CWslDbRecordset::GetColType(int nCol)
{
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 
	if (!m_aCols[nCol]) OnInitColumn(nCol);
	return m_aCols[nCol]->nSQLType;
}

//*****************************************************************************
int	CWslDbRecordset::GetColIdx(const char* pcszCol)
{
	ASSERT(pcszCol);

	for (int i=m_aCols.GetSize(); i--;)
	{
		if (!m_aCols[i]) OnInitColumn(i);
		if (!stricmp(pcszCol, m_aCols[i]->sName)) return i;
	}

	return -1;
}

//*****************************************************************************
const CWslString& CWslDbRecordset::GetColName(int nCol)
{
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 
	if (!m_aCols[nCol]) OnInitColumn(nCol);
	return m_aCols[nCol]->sName;
}












