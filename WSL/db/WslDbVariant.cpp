///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"

//*****************************************************************************
CWslVariant::CWslVariant()
{
	m_eType = VT_NULL;
}

//*****************************************************************************
CWslVariant::~CWslVariant()
{
	SetNull();
}

//*****************************************************************************
void CWslVariant::SetNull()
{
	if (m_eType == VT_NULL) return;

	switch (m_eType)
	{
	case VT_DATE	: delete pDate; break;
	case VT_STRING: delete pString; break;
	case VT_BINARY: delete pBinary; break;
	}
	m_eType = VT_NULL;
}

//*****************************************************************************
void* CWslVariant::GetData(int *pnSize)
{
	switch (m_eType)
	{
	case VT_NULL	: *pnSize = 0; return NULL;
	case VT_BOOL	: *pnSize = sizeof(bVal); return &bVal;
	case VT_BYTE	: *pnSize = sizeof(byVal); return &byVal;
	case VT_SHORT	: *pnSize = sizeof(wVal); return &wVal;
	case VT_LONG	: *pnSize = sizeof(nVal); return &nVal;
	case VT_BIGINT : *pnSize = sizeof(qVal); return &qVal;
	case VT_FLOAT	: *pnSize = sizeof(fVal); return &fVal;
	case VT_DOUBLE : *pnSize = sizeof(dblVal); return &dblVal;
	case VT_DATE	: *pnSize = sizeof(*pDate); return pDate;
	case VT_STRING : *pnSize = pString->GetLength(); return (void*)(const char*) *pString;
	case VT_BINARY : *pnSize = pBinary->GetSize(); return pBinary->GetData();
	default: ASSERT(false); return NULL;
	}
}

//*****************************************************************************
void CWslVariant::SetType(ETypes eType)
{
	SetNull();

	switch (m_eType = eType)
	{
	case VT_DATE	: pDate = new TIMESTAMP_STRUCT; break;
	case VT_STRING: pString = new CWslString(); break;
	case VT_BINARY: pBinary = new CWslLongBinary(0); break;
	}
}

#ifdef _MFC_VER
//*****************************************************************************
void CWslVariant::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_eType;

		switch (m_eType)
		{
		case VT_NULL	: break;
		case VT_BOOL	: ar.Write(&bVal, sizeof(bVal)); break;
		case VT_BYTE	:	ar << byVal; break;
		case VT_SHORT	: ar << wVal; break;
		case VT_LONG	: ar << nVal; break;
		case VT_BIGINT: ar.Write(&qVal, sizeof(qVal)); break;
		case VT_FLOAT	: ar << fVal; break;
		case VT_DOUBLE: ar << dblVal; break;
		case VT_DATE	: ar.Write(pDate, sizeof(TIMESTAMP_STRUCT)); break;
		case VT_STRING: ar << *pString; break;
		case VT_BINARY: 
			ar << pBinary->GetSize();
			if (pBinary->GetSize()) ar.Write(pBinary->GetData(), pBinary->GetSize()); 
			break;
		default: ASSERT(false); break;
		}
	}
	else
	{
		int n;
		ar >> n;
		SetType((ETypes)n);

		switch (m_eType)
		{
		case VT_NULL	: break;
		case VT_BOOL	: ar.Read(&bVal, sizeof(bVal)); break;
		case VT_BYTE	:	ar >> byVal; break;
		case VT_SHORT	: ar >> wVal; break;
		case VT_LONG	: ar >> nVal; break;
		case VT_BIGINT: ar.Read(&qVal, sizeof(qVal)); break;
		case VT_FLOAT	: ar >> fVal; break;
		case VT_DOUBLE: ar >> dblVal; break;
		case VT_DATE	: ar.Read(pDate, sizeof(TIMESTAMP_STRUCT)); break;
		case VT_STRING: ar >> *pString; break;
		case VT_BINARY: 
			ar >> n;
			pBinary->Alloc(n);
			if (n) ar.Read(pBinary->GetData(), n); 
			break;
		default: ASSERT(false); break;
		}
	}
}
#endif


/*****************************************************************************
*
* ASSIGNMENT
*
*****************************************************************************/

//*****************************************************************************
const CWslVariant& CWslVariant::operator =(const CWslVariant& dbv)
{
	if (this == &dbv) return *this;
	
	SetNull();
	m_eType = dbv.m_eType;

	switch (m_eType)
	{
	case VT_BOOL	: bVal = dbv.bVal; break;
	case VT_BYTE	:	byVal = dbv.byVal; break;
	case VT_SHORT	: wVal = dbv.wVal; break;
	case VT_LONG	: nVal = dbv.nVal; break;
	case VT_BIGINT: qVal = dbv.qVal; break;
	case VT_FLOAT	: fVal = dbv.fVal; break;
	case VT_DOUBLE: dblVal = dbv.dblVal; break;
	case VT_DATE	: pDate = new TIMESTAMP_STRUCT(*dbv.pDate); break;
	case VT_STRING: pString = new CWslString(*dbv.pString); break;
	case VT_BINARY: ASSERT(false); break;
	default							: break;
	}

	return *this;
}

/*****************************************************************************/
const CWslDateTime& CWslVariant::operator =(const CWslDateTime& dt)
{
	if (m_eType != VT_DATE) SetType(VT_DATE);
	ASSERT(pDate);

	pDate->hour = dt.GetHour();
	pDate->minute = dt.GetMinute();
	pDate->second = dt.GetSecond();
	pDate->fraction = 0;

	pDate->day = dt.GetDay();
	pDate->month = dt.GetMonth();
	pDate->year = dt.GetYear();

	return dt;
}

/*****************************************************************************/
const CWslDate& CWslVariant::operator =(const CWslDate& d)
{
	if (m_eType != VT_DATE) SetType(VT_DATE);
	ASSERT(pDate);

	pDate->hour = 0;
	pDate->minute = 0;
	pDate->second = 0;
	pDate->fraction = 0;

	pDate->day = d.GetDay();
	pDate->month = d.GetMonth();
	pDate->year = d.GetYear();

	return d;
}

/*****************************************************************************/
int CWslVariant::operator =(int n)
{
	if (m_eType != VT_LONG) SetType(VT_LONG);
	nVal = n;
	return n;
}

/*****************************************************************************/
DWORD CWslVariant::operator =(DWORD dw)
{
	if (m_eType != VT_LONG) SetType(VT_LONG);
	nVal = dw;
	return dw;
}

/*****************************************************************************/
bool CWslVariant::operator =(bool b)
{
	if (m_eType != VT_BOOL) SetType(VT_BOOL);
	bVal = b;
	return b;
}

/*****************************************************************************/
const CWslString& CWslVariant::operator =(const CWslString& s)
{
	if (m_eType != VT_STRING) SetType(VT_STRING);
	*pString = s;
	return *pString;
}

/*****************************************************************************/
const char* CWslVariant::operator =(const char* pcsz)
{
	if (m_eType != VT_STRING) SetType(VT_STRING);
	*pString = pcsz;
	return pcsz;
}

/*****************************************************************************
*
* GETTING VALUE
*
*****************************************************************************/

/*****************************************************************************/
CWslVariant::operator bool() const
{
   switch (m_eType)
   {
   case VT_LONG: return nVal ? true:false;
   case VT_BYTE: return byVal ? true:false;
   case VT_SHORT: return wVal ? true:false;
   case VT_BOOL: return bVal;
   case VT_NULL:
   default:
      return false;
   }
}

/*****************************************************************************/
CWslVariant::operator int() const
{ 
   switch (m_eType)
   {
   case VT_LONG: return nVal;
   case VT_BYTE: return byVal;
   case VT_SHORT: return wVal;
   case VT_BOOL: return bVal;
   case VT_NULL:
   default:
      return 0;
   }
}

/*****************************************************************************/
CWslVariant::operator CWslDate() const
{
   switch (m_eType)
   {
   case VT_DATE: ASSERT(pDate); return CWslDate(pDate->year, pDate->month, pDate->day);
   case VT_NULL:
   default:
      return CWslDate();
   }
}

/*****************************************************************************/
CWslVariant::operator CWslDateTime() const
{
   switch (m_eType)
   {
   case VT_DATE: 
      ASSERT(pDate); 
      return CWslDateTime(pDate->year, pDate->month, pDate->day, pDate->hour, pDate->minute, pDate->second);

   default:
   case VT_NULL:
      return CWslDateTime();
   }
}




