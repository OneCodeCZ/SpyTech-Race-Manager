///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslDate
// | P |  /  |  |  ~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WslString.h"

#ifndef WSL_API
#define WSL_API
#endif

//******************************************************************************
class WSL_API CWslDate  
{
protected:
   DWORD m_dwDate;
   
public:
   CWslDate();
   CWslDate(int nYear, int nMonth, int nDay);

   //---------------------------------------------------------------------------
   // Setting date
   //---------------------------------------------------------------------------

   void SetNow();
  
   // Format: dd.mm.yyyy or dd.mm. (year is set to the current one)
   // If the string is empty, date is set to null.
   // If the format was incorrect, date is set to null and FALSE is returned.
   BOOL Set(const char* pcszDate);
   
   void Set(SYSTEMTIME* pSysTime);
   inline void Set(DWORD dwDate) { m_dwDate = dwDate; }
   void Set(int nYear, int nMonth, int nDay);

   void SetNull() { m_dwDate=0; }
   inline BOOL IsNull() const { return m_dwDate==0; }
   
   char* Get(char* b);
   void Get(SYSTEMTIME* pSysTime);
   inline DWORD Get() const { return m_dwDate; }
   
   //---------------------------------------------------------------------------
   // Getting date
   //---------------------------------------------------------------------------
   
   // Month is formatted using rome numbers: 4.V.2003
   CWslString GetRomeFormat() const;
   
   // DD.MM.YYYY
   CWslString GetDMY(char chSep='.') const;
   
   CWslString GetYMD(char chSep='-') const;
   
   inline int GetYear() const { return m_dwDate >> 16; }
   inline int GetMonth() const { return (m_dwDate >> 8) & 255; }
   inline int GetDay() const { return m_dwDate & 255; }
   
   //---------------------------------------------------------------------------
   // Operators
   //---------------------------------------------------------------------------

   inline bool operator ==(const CWslDate& d) const { return m_dwDate==d.m_dwDate; }
   inline bool operator !=(const CWslDate& d) const { return m_dwDate!=d.m_dwDate; }
   inline bool operator <(const CWslDate& d) const { return m_dwDate<d.m_dwDate; }
   inline bool operator >(const CWslDate& d) const { return m_dwDate>d.m_dwDate; }
   inline bool operator <=(const CWslDate& d) const { return m_dwDate<=d.m_dwDate; }
   inline bool operator >=(const CWslDate& d) const { return m_dwDate>=d.m_dwDate; }
   
   inline void operator +=(int days) { Offset2Date(Date2Offset() + days); }
   inline void operator -=(int days) { Offset2Date(Date2Offset() - days); }
   inline void operator =(const char* s) { Set(s); }
   
   // Returns the number of days between the given date
   int operator -(const CWslDate& d) const;
 
   //---------------------------------------------------------------------------
   // Static
   //---------------------------------------------------------------------------

   static CWslDate GetNow();
   static int GetCurrentYear();
   static char* GetCurrentDate(char* s);

   //---------------------------------------------------------------------------
   // Internals
   //---------------------------------------------------------------------------
private:
   int Date2Offset() const;
   void Offset2Date(int o);
   BOOL IsYearSpecial(int y) const;
};
