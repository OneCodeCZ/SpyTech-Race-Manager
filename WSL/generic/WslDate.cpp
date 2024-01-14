///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslDate
// | P |  /  |  |  ~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 10
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDate.h"

static DWORD NumDays[]={  0,
                        31,
                        31+28,
                        31+28+31,
                        31+28+31+30,
                        31+28+31+30+31,
                        31+28+31+30+31+30,
                        31+28+31+30+31+30+31,
                        31+28+31+30+31+30+31+31,
                        31+28+31+30+31+30+31+31+30,
                        31+28+31+30+31+30+31+31+30+31,
                        31+28+31+30+31+30+31+31+30+31+30,
                        31+28+31+30+31+30+31+31+30+31+30+31  };

//prestupne roky
static DWORD NumDays2[]={ 0,
                        31,
                        31+29,
                        31+29+31,
                        31+29+31+30,
                        31+29+31+30+31,
                        31+29+31+30+31+30,
                        31+29+31+30+31+30+31,
                        31+29+31+30+31+30+31+31,
                        31+29+31+30+31+30+31+31+30,
                        31+29+31+30+31+30+31+31+30+31,
                        31+29+31+30+31+30+31+31+30+31+30,
                        31+29+31+30+31+30+31+31+30+31+30+31  };

/*****************************************************************************/
CWslDate::CWslDate()
{ 
   ASSERT(sizeof(CWslDate)==sizeof(DWORD)); 
   m_dwDate = 0; 
}

/*****************************************************************************/
CWslDate::CWslDate(int nYear, int nMonth, int nDay)
{  
   Set(nYear, nMonth, nDay);
}

/*****************************************************************************/
CWslDate CWslDate::GetNow()
{
   CWslDate d;
   d.SetNow();
   return d;
}

/*****************************************************************************
*
* GETTING
*
*****************************************************************************/

/*****************************************************************************/
void CWslDate::Get(SYSTEMTIME* pSysTime)
{
	pSysTime->wDayOfWeek = 0;
	pSysTime->wHour = 0;
	pSysTime->wMilliseconds = 0;
	pSysTime->wMinute = 0;
	pSysTime->wSecond = 0;
	pSysTime->wDay = (WORD) (m_dwDate & 255);
	pSysTime->wMonth = (WORD) ((m_dwDate >> 8) & 255);
	pSysTime->wYear = (WORD) (m_dwDate >> 16);
}

/*****************************************************************************/
int CWslDate::GetCurrentYear()
{
  SYSTEMTIME s;
  GetLocalTime(&s);
  return s.wYear;
}

/*****************************************************************************/
char *CWslDate::GetCurrentDate(char *s)
{
  CWslDate d;
  d.SetNow();
  return d.Get(s);
}

/*****************************************************************************/
char* CWslDate::Get(char* b)
{
  if (!IsNull()) 
    sprintf(b, "%d.%d.%d", GetDay(), GetMonth(), GetYear());
  else 
    b[0] = 0;

  return b;
}

/*****************************************************************************/
CWslString CWslDate::GetRomeFormat() const
{
  static const char* aRomeNums[] = 
  { 
    "I","II","III","IV","V","VI","VII","VIII","IX","X","XI","XII" 
  };

  CWslString s;
  if (!IsNull())
  {
    ASSERT(GetMonth()>=1 && GetMonth()<=12);
    s.Format("%d.%s.%d", GetDay(), aRomeNums[GetMonth()-1], GetYear());
  }
  return s;
}

/*****************************************************************************/
CWslString CWslDate::GetDMY(char chSep) const
{
  CWslString s;
  if (!IsNull()) s.Format("%02d%c%02d%c%d", GetDay(), chSep, GetMonth(), chSep, GetYear());
  return s;
}

/*****************************************************************************/
CWslString CWslDate::GetYMD(char chSep) const
{
  CWslString s;
  if (!IsNull()) s.Format("%d%c%02d%c%02d", GetYear(), chSep, GetMonth(), chSep, GetDay());
  return s;
}


/*****************************************************************************
*
* SETTING
*
*****************************************************************************/

/*****************************************************************************/
void CWslDate::SetNow()
{
  SYSTEMTIME s;
  GetLocalTime(&s);
  m_dwDate=s.wDay + s.wMonth*256 + s.wYear*65536;
}

/*****************************************************************************/
BOOL CWslDate::Set(const char* pcszDate)
{
  char *p,*t;
  DWORD x, dwNewDate;
  char temp[16];

  m_dwDate = 0;
  dwNewDate = 0;

  if (pcszDate[0]==0) return true;
  strcpy(temp, pcszDate);
  t=temp;

  //den
  if (!(p=strchr(t,'.')) && !(p=strchr(t,','))) return false;
  p[0]=0;
  x=atoi(t);
  if (x<1 || x>31) return FALSE;
  dwNewDate = x;
    
  //mesic
  t=p+1;
  if (!(p=strchr(t,'.')) && !(p=strchr(t,','))) return false;
  p[0]=0;
  x=atoi(t);
  if (x<1 || x>12) return false;
  dwNewDate |= x<<8;
 
  //rok
  x=atoi(p+1);
  if (x<=0) x=GetCurrentYear();
  dwNewDate |= x<<16;

  //zkontrolovat pocet dni v mesici
  DWORD *m= IsYearSpecial((dwNewDate>>16)) ? NumDays2 : NumDays;
  x=m[ ((dwNewDate>>8)&255) ]  - m[ ((dwNewDate>>8)&255)-1];
  if ( (dwNewDate&255) > x ) return false;

  m_dwDate = dwNewDate;
  return true;
}

/*****************************************************************************/
void CWslDate::Set(SYSTEMTIME* pSysTime)
{
	m_dwDate = pSysTime->wDay + (pSysTime->wMonth << 8) + (pSysTime->wYear << 16);
}

/*****************************************************************************/
void CWslDate::Set(int nYear, int nMonth, int nDay)
{
  if (nMonth < 1 || nMonth > 12)
    m_dwDate = 0;
  else if (nDay < 1 || nDay > 31)
    m_dwDate = 0;
  else if (nYear < 0 || nYear > 65535)
    m_dwDate = 0;
  else
  	m_dwDate = nDay + (nMonth << 8) + (nYear << 16);
}

/*****************************************************************************
*
* 
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWslDate::IsYearSpecial(int y) const
{
  if (y%4) return false;
  if (y%100==0 && y%1000) return false;
  return true;
}

/*****************************************************************************/
int CWslDate::Date2Offset() const
{
//dni
  int ofs=(m_dwDate & 255);

//rok na dni
  int cur=(m_dwDate>>16);
  int spec=cur/4 - cur/100 + cur/1000 + 1;
  int norm=cur - spec;
  ofs+=spec*366 + norm*365;

//mesic na dni
  int month=((m_dwDate>>8) & 255); 
  DWORD *p= IsYearSpecial(cur) ? NumDays2 : NumDays;
  ofs+=p[month - 1];
    
  return ofs;
}

/*****************************************************************************/
void CWslDate::Offset2Date(int o)
{
  DWORD y = 1000*(o-1) / 365241;
  DWORD r = 1000*(o-1) % 365241;
  r=(r+500)/1000;

  DWORD *p= IsYearSpecial(y) ? NumDays2 : NumDays;
  DWORD m=0;
  while (r > p[m]) m++;
  DWORD d=r - p[m-1];

  m_dwDate = (y<<16) | (m<<8) | d;
}

/*****************************************************************************
*
* ARITMETHICS
*
*****************************************************************************/

/*****************************************************************************/
int CWslDate::operator -(const CWslDate& d) const
{
  return Date2Offset() - d.Date2Offset();
}

