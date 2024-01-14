///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL Generic SQL Database Support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"

//*****************************************************************************
CWslDbCommand::CWslDbCommand()
{
}

//*****************************************************************************
CWslDbCommand::~CWslDbCommand()
{
}

/*****************************************************************************
*
* COLUMNS
*
*****************************************************************************/

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, 
													 const CWslString& sVal,
													 BOOL bUseNULL/*=false*/)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	if (bUseNULL && sVal.IsEmpty()) p->Value.SetNull(); else p->Value = sVal;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, 
													 const char* pcszVal,
													 BOOL bUseNULL/*=false*/)
{
	ASSERT(pcszVal);
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	if (bUseNULL && (!pcszVal || !pcszVal[0])) p->Value.SetNull(); else p->Value = pcszVal;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, int nVal, 
													 BOOL bUseNULL /*=false*/,
													 int nNullValue /*=0*/)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	if (bUseNULL && nVal==nNullValue) p->Value.SetNull(); else p->Value = nVal;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, bool bVal)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value = bVal;
	m_aParams += p;
}


//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, const CWslDate &d, BOOL bUseNULL/*=false*/)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	if (bUseNULL && d.IsNull()) p->Value.SetNull(); else p->Value = d;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, const CWslDateTime& d)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value = d;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, const TIMESTAMP_STRUCT *pts)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value.SetType(CWslVariant::VT_DATE);
	*p->Value.pDate = *pts;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, const CWslVariant &v)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value = v;
	m_aParams += p;}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, float fVal)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value.SetType(CWslVariant::VT_FLOAT);
	p->Value.fVal = fVal;
	m_aParams += p;
}

//*****************************************************************************
void CWslDbCommand::AddCol(const char* pcszCol, double dblVal)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value.SetType(CWslVariant::VT_DOUBLE);
	p->Value.dblVal = dblVal;
	m_aParams += p;
}

/*****************************************************************************/
void CWslDbCommand::AddCol(const char* pcszCol, BYTE* pbyData, int nSize)
{
	ASSERT(pcszCol);
	ASSERT(pbyData);
	ASSERT(nSize >= 0);

	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
	p->Value.SetType(CWslVariant::VT_BINARY);
	p->Value.pBinary->Attach(pbyData, nSize);
	m_aParams += p;
}

/*****************************************************************************/
void CWslDbCommand::AddColExpression(const char* pcszCol, const char* pcszVal)
{
	ASSERT(pcszVal && pcszVal[0]);
	CParam* p = new CParam();
	p->bInsertAsParam = false;
	p->sName = pcszCol;
	p->Value = pcszVal;
	m_aParams += p;
}

/*****************************************************************************/
void CWslDbCommand::AddCol(const char* pcszCol)
{
	CParam* p = new CParam();
	p->bInsertAsParam = true;
	p->sName = pcszCol;
   p->Value.SetNull();
	m_aParams += p;
}

/*****************************************************************************
*
* SETTING SQL
*
*****************************************************************************/

//*****************************************************************************
void CWslDbCommand::SetSQL(const char* pcszSQL, ...)
{
	ASSERT(pcszSQL);

	char temp[4096];
	va_list l;
	va_start(l, pcszSQL);
	BOOL b = _vsnprintf(temp, 4096, pcszSQL, l) >= 0;
	temp[4096-1] = 0;
	va_end(l);

   if (!b) throw new CWslTextException("CWslDbCommand::SetSQL(): SQL command too long!");

	m_sSql = temp;
}

//*****************************************************************************
void CWslDbCommand::BuildInsert(const char* pcszTbl, BOOL bUseReplace /*= false*/)
{
	CWslString s;
	int i;
	BOOL bFirst;

	ASSERT(m_aParams.GetSize());
	ASSERT(pcszTbl);

	// First part - column names

	s.Format("%s INTO %s (", (bUseReplace ? "REPLACE":"INSERT"), pcszTbl);

	for (i=0,bFirst=true; i<m_aParams.GetSize(); i++)
	{
		if (!m_aParams[i]->sName.IsEmpty())
		{
			if (bFirst) bFirst = false; else s += ',';
			s += m_aParams[i]->sName;
		}
	}

	// Second part - column values

	s += ") VALUES (";

	for (i=0,bFirst=true; i<m_aParams.GetSize(); i++)
	{
		if (!m_aParams[i]->sName.IsEmpty())
		{
			if (!bFirst) 
				s += ',';
			else
				bFirst = false;

			if (m_aParams[i]->bInsertAsParam)
				s += '?';
			else
				s += (CWslString) m_aParams[i]->Value;
		}
	}

	s += ')';
	m_sSql = s;
}

/*****************************************************************************/
void CWslDbCommand::BuildInsert(const char* pcszTbl, 
                                const char* pcszPrimaryKeyColumn, 
                                int nPrimaryKey)
{
	ASSERT(pcszPrimaryKeyColumn && pcszTbl);
   AddCol(pcszPrimaryKeyColumn, nPrimaryKey);
   BuildInsert(pcszTbl, false);
}

//*****************************************************************************
void CWslDbCommand::BuildUpdate(const char* pcszTbl, 
                                const char* pcszPrimaryKeyColumn, 
                                int nPrimaryKey)
{
	CWslString s;

	ASSERT(pcszPrimaryKeyColumn && pcszTbl);
	s.Format("%s=%d", pcszPrimaryKeyColumn, nPrimaryKey); 

	BuildUpdate(pcszTbl, s);
}

//*****************************************************************************
void CWslDbCommand::BuildUpdate(const char* pcszTbl, const char* sWhere)
{
	CWslString s;
	int i;
	BOOL bFirst;

   if (m_aParams.IsEmpty()) return;
	ASSERT(pcszTbl);

	// Build UPDATE part
	
	s.Format("UPDATE %s SET ", pcszTbl);

	for (i=0,bFirst=true; i<m_aParams.GetSize(); i++)
	{
		if (!m_aParams[i]->sName.IsEmpty())
		{
			if (bFirst) bFirst = false; else s += ',';
			s += m_aParams[i]->sName;

			if (m_aParams[i]->bInsertAsParam)
				s += "=?";
			else
				s += "=" + (CWslString) m_aParams[i]->Value;
		}
	}

	// Add WHERE part

	m_sSql = s + " WHERE " + sWhere;
}

//*****************************************************************************
void CWslDbCommand::BuildSave(BOOL bNew,
                              const char* pcszTbl,
                              const char* pcszPrimaryKeyColumn, 
                              int nPrimaryKey)
{
   if (bNew)
      BuildInsert(pcszTbl, pcszPrimaryKeyColumn, nPrimaryKey);
   else
      BuildUpdate(pcszTbl, pcszPrimaryKeyColumn, nPrimaryKey);
}

//*****************************************************************************
void CWslDbCommand::Clear()
{
	m_aParams.RemoveAll();
}

/*****************************************************************************
*
* MFC SERIALIZATION
*
*****************************************************************************/

#ifdef _MFC_VER

IMPLEMENT_SERIAL(CWslDbCommand, CObject, 0)

//*****************************************************************************
void CWslDbCommand::Serialize(CArchive &ar)
{
	int n;

	if (ar.IsStoring())
	{
		ar << m_sSql;
		ar << m_aParams.GetSize();

		for (int i=0; i<m_aParams.GetSize(); i++)
		{
			ar << m_aParams[i]->sName;
			m_aParams[i]->Value.Serialize(ar);
		}
	}
	else
	{
		ar >> m_sSql;
		ar >> n;
		m_aParams.RemoveAll();

		for (int i=0; i<n; i++)
		{
			m_aParams += new CParam();
			ar >> m_aParams[i]->sName;
			m_aParams[i]->Value.Serialize(ar);
		}
	}
}

#endif







