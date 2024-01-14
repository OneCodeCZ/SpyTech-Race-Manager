///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslDateTime
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 12
// | H |  _||   |
// |___|________|	 Class for easy date time handling
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDateTime.h"

// 1.1.2038
#define ETERNITY_VALUE  (1 + (1 << 8) + (2038 << 16))   

// 1.1.1000
#define PAST_VALUE		(1 + (1 << 8) + (1000 << 16))   

// 1.1.100
// COM's date type's least value
#define NULL_VALUE      (1 + (1 << 8) + (100 << 16))    


//*****************************************************************************
CWslDateTime::CWslDateTime()
{
   SetNull();
}

/*****************************************************************************/
CWslDateTime::CWslDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond)
{
   Set(nYear, nMonth, nDay, nHour, nMinute, nSecond);
}

/*****************************************************************************/
CWslDateTime::CWslDateTime(DATE d)
{
   SetFromComDate(d);
}

/*****************************************************************************
*
* SET
*
*****************************************************************************/

/*****************************************************************************/
void CWslDateTime::SetNull()
{
	m_nDate = NULL_VALUE;
	m_nTime = 0;
}

//*****************************************************************************
void CWslDateTime::SetNow()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	Set(st);
}

/*****************************************************************************/
void CWslDateTime::SetTodayMidnight()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
   st.wHour = st.wMilliseconds = st.wMinute = st.wSecond = 0;
	Set(st);
}

/*****************************************************************************/
void CWslDateTime::SetToday()
{
	SetNow();
	m_nTime = 0;
}

/*****************************************************************************/
void CWslDateTime::SetEternity()
{
	m_nDate = ETERNITY_VALUE;
	m_nTime = 0;
}

/*****************************************************************************/
void CWslDateTime::SetPast()
{
	m_nDate = PAST_VALUE;
	m_nTime = 0;
}

//*****************************************************************************
void CWslDateTime::Set(SYSTEMTIME& st)
{
	m_nDate = st.wDay + (st.wMonth << 8) + (st.wYear << 16);
	m_nTime = st.wSecond + (st.wMinute << 8) + (st.wHour << 16);
}

//*****************************************************************************
void CWslDateTime::Set(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond)
{
	m_nDate = nDay + (nMonth << 8) + (nYear << 16);
	m_nTime = nSecond + (nMinute << 8) + (nHour << 16);
}

/*****************************************************************************/
BOOL CWslDateTime::SetFromComDate(DATE d)
{
   SYSTEMTIME st;
   if (!VariantTimeToSystemTime(d, &st)) return false;
   Set(st);
   return true;
}

/*****************************************************************************
*
* GET
*
*****************************************************************************/

/*****************************************************************************/
int CWslDateTime::IsNull() const 
{ 
   return m_nDate <= NULL_VALUE;
}

//*****************************************************************************
void CWslDateTime::Get(SYSTEMTIME& st) const
{
	st.wDayOfWeek = 0;
	st.wMilliseconds = 0;
	st.wHour = (WORD) (m_nTime >> 16);
	st.wMinute = (WORD) ((m_nTime >> 8) & 255);
	st.wSecond = (WORD) (m_nTime & 255);
	st.wDay = (WORD) (m_nDate & 255);
	st.wMonth = (WORD) ((m_nDate >> 8) & 255);
	st.wYear = (WORD) (m_nDate >> 16);
}

//*****************************************************************************
char* CWslDateTime::Get(char* pszBuffer) const
{
	if (!IsNull()) 
	{
		if (m_nDate >= ETERNITY_VALUE)
		{
			strcpy(pszBuffer, "Stále");
		}
		else if (m_nDate <= PAST_VALUE)
		{
			strcpy(pszBuffer, "Ihned");
		}
		else
		{
			if (m_nTime)
			{
				sprintf(pszBuffer, "%02d.%02d.%02d %02d:%02d:%02d", 
					GetDay(), GetMonth(), GetYear(), GetHour(), GetMinute(), GetSecond());
			}
			else
			{
				sprintf(pszBuffer, "%02d.%02d.%02d", GetDay(), GetMonth(), GetYear());
			}
		}
	}
	else pszBuffer[0] = 0;

	return pszBuffer;
}

//*****************************************************************************
CWslString CWslDateTime::Get() const
{
	CWslString s;
	char* p = s.GetBufferSetLength(32);
	Get(p);
	ASSERT(strlen(p) < 32);
	s.ReleaseBuffer();
	return s;
}

/*****************************************************************************/
CWslString CWslDateTime::GetTime() const 
{ 
   return Format("H:m:S"); 
}

/*****************************************************************************/
CWslString CWslDateTime::Format(const char* pcszFormat) const
{
	ASSERT(pcszFormat);

	CWslString s;
	char ch;
	char temp[16];

	for (const char* p=pcszFormat; ch=*p++;)
	{
		switch (ch)
		{
		case 'Y': s += itoa(GetYear(), temp, 10); break;
		case 'M': sprintf(temp, "%02d", GetMonth()); s += temp; break;
		case 'D': sprintf(temp, "%02d", GetDay()); s += temp; break;
		case 'H': sprintf(temp, "%02d", GetHour()); s += temp; break;
		case 'm': sprintf(temp, "%02d", GetMinute()); s += temp; break;
		case 'S': sprintf(temp, "%02d", GetSecond()); s += temp; break;
		default: s += ch; break;
		}
	}

	return s;
}

//*****************************************************************************
int CWslDateTime::GetNumber(const char*& p)
{
	int n;
	for (; *p && (*p<'0' || *p>'9'); p++) /*dummy*/;
	if (!*p) return 0;
	for (n=0; *p>='0' && *p<='9'; p++) n = n*10 + (*p - '0');
	return n;
}

//*****************************************************************************
BOOL CWslDateTime::Set(const char* pcsz)
{
	int n1,n2,n3;

	if (!pcsz[0]) { SetNull(); return true; }
	if (!stricmp(pcsz, "teï")) { SetNow(); return true; }
	if (!stricmp(pcsz, "dnes")) { SetToday(); return true; }
	if (!stricmp(pcsz, "ihned")) { SetPast(); return true; }
	if (!stricmp(pcsz, "stále")) { SetEternity(); return true; }

	// DD MM YYYY
	n1 = GetNumber(pcsz);
	if (!*pcsz) return false;
	if (n1<1 || n1>31) return false;
	
	n2 = GetNumber(pcsz);
	if (!*pcsz) return false;
	if (n2<1 || n2>12) return false;
	
	n3 = GetNumber(pcsz);
	if (n3<1 || n3>9999) return false;
	m_nDate = n1 + (n2 << 8) + (n3 << 16);
	m_nTime = 0;
	if (!*pcsz) return true;

	// HH MM SS
	n1 = GetNumber(pcsz);
	if (!*pcsz) return false;
	if (n1<0 || n1>23) return false;

	n2 = GetNumber(pcsz);
	if (n2<0 || n2>59) return false;
	m_nTime = (n2 << 8) + (n1 << 16);
	if (!*pcsz) return true;
	
	n3 = GetNumber(pcsz);
	if (n3<0 || n3>59) return false;
	m_nTime |= n3;

	return true;
}

/*****************************************************************************/
DATE CWslDateTime::GetComDate() const
{
   double d;
   SYSTEMTIME st;

   Get(st);
   SystemTimeToVariantTime(&st,&d);

   return d;
}

/*****************************************************************************
*
* ARITHMETIC
*
*****************************************************************************/

/*****************************************************************************/
__int64 CWslDateTime::operator -(const CWslDateTime& d) const
{
   __int64 TimeDelta = GetSecondsSinceMidnight() - d.GetSecondsSinceMidnight();
   __int64 DateDelta = GetDate() - d.GetDate();
   return TimeDelta + DateDelta*3600*24;
}

/*****************************************************************************/
void CWslDateTime::operator +=(int nSeconds)
{
   CWslDate d = GetDate();
   int t = GetSecondsSinceMidnight() + nSeconds;

   if (t >= 24*3600) 
   {
      d += 1;
      t -= 24*3600;
   }

   Set(d.GetYear(), d.GetMonth(), d.GetDay(), t/3600, t%3600/60, t%3600%60);
}

/*****************************************************************************
*
* SERIALIZATION
*
*****************************************************************************/

/*****************************************************************************/
#ifdef _MFC_VER
void CWslDateTime::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_nDate;
		ar << m_nTime;
	}
	else
	{
		ar >> m_nDate;
		ar >> m_nTime;
	}
}
#endif
