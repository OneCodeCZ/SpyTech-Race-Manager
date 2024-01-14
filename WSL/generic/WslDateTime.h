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

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "WslString.h"
#include "WslDate.h"

//******************************************************************************
class WSL_API CWslDateTime
{
protected:
   int m_nDate, m_nTime;
   
public:
   CWslDateTime();
   CWslDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
   CWslDateTime(DATE d);
   
   //---------------------------------------------------------------------------
   // Setting data
   //---------------------------------------------------------------------------
   
   void SetNow();
   void SetTodayMidnight();
   void SetEternity();
   void SetPast();
   void SetToday();
   void Set(SYSTEMTIME& st);
   void Set(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
  
   // Special state. When converted into string it results in an empty string.
   void SetNull();
   
   // Sets the date-time from the Czech formatted string.
   // Format: DD.MM.YYYY HH:MM:SS or just DD.MM.YYYY or empty string.
   // Returns true if format is OK.
   // NOTE: Every non-numeric char is considered as separator :-)
   // NOTE: Time can be ommited. In such case it's set to 0.
   // NOTE: Seconds can be ommited.
   // Supports the following keywords:
   // ihned = SetPast();
   // stale = SetEternity();
   // ted = SetNow();
   // dnes = SetToday();
   BOOL Set(const char* pcsz);
   
   // Setting by using a = operator. Set() used.
   inline void operator =(const char* pcsz) { Set(pcsz); }

   // Sets date from the COM type DATE
   // Returns FALSE if the date is invalid.
   BOOL SetFromComDate(DATE d);
   
   //---------------------------------------------------------------------------
   // Retrieving data
   //---------------------------------------------------------------------------
   
   // Returns the date-time in the Czech format (D.M.Y H:M:S)
   // If the time component is 0:0:0 it's not present in the
   // output.
   // If the time has a special meaning e.g. eternity, the 
   // proper keyword is returned.
   char* Get(char* pszBuffer) const;
   CWslString Get() const;
   
   // Returns formated date-time string.
   // Format: Y=year, M=month, D=day, H=24-hour, m=minute, S=second
   // e.g. "Y:M:D H.m.S"
   CWslString Format(const char* pcszFormat) const;
   
   // Inits various structures with the date-time info.
   void Get(SYSTEMTIME& st) const;

   // Returns only the time part in H:m:S format.
   CWslString GetTime() const;

   // Get single date-time components
   inline int GetYear() const { return m_nDate >> 16; }
   inline int GetMonth() const { return (m_nDate >> 8) & 255; }
   inline int GetDay() const { return m_nDate & 255; }
   inline int GetHour() const { return m_nTime >> 16; }
   inline int GetMinute() const { return (m_nTime >> 8) & 255; }
   inline int GetSecond() const { return m_nTime & 255; }
   
   // Is the date-time null?
   inline int IsNull() const;
   
   inline int GetSecondsSinceMidnight() const { return GetHour()*3600 + GetMinute()*60 + GetSecond(); }

   CWslDate GetDate() const { return CWslDate(GetYear(), GetMonth(), GetDay()); }

   // Returns the COM date type DATE
   DATE GetComDate() const;
   
   operator DATE() const { return GetComDate(); }

   //---------------------------------------------------------------------------
   // Comparsion
   //---------------------------------------------------------------------------
   
   inline bool operator ==(const CWslDateTime& d) { return m_nDate==d.m_nDate && m_nTime==d.m_nTime; }
   inline bool operator !=(const CWslDateTime& d) { return m_nDate!=d.m_nDate || m_nTime!=d.m_nTime; }
   inline bool operator <(const CWslDateTime& d) { return m_nDate<d.m_nDate || (m_nDate==d.m_nDate && m_nTime<d.m_nTime); }
   inline bool operator >(const CWslDateTime& d) { return m_nDate>d.m_nDate || (m_nDate==d.m_nDate && m_nTime>d.m_nTime); }
   inline bool operator <=(const CWslDateTime& d) { return m_nDate<d.m_nDate || (m_nDate==d.m_nDate && m_nTime<=d.m_nTime); }
   inline bool operator >=(const CWslDateTime& d) { return m_nDate>d.m_nDate || (m_nDate==d.m_nDate && m_nTime>=d.m_nTime); }
   
   //---------------------------------------------------------------------------
   // Arithmetic
   //---------------------------------------------------------------------------

   // Returns the difference in seconds
   __int64 operator -(const CWslDateTime& d) const;

   // Adding a time span in seconds
   void operator +=(int nSeconds);
   inline void operator -=(int nSeconds) { operator +=(-nSeconds); }

   //---------------------------------------------------------------------------
   // MFC Serialization
   //---------------------------------------------------------------------------
   
#ifdef _MFC_VER
   void Serialize(CArchive& ar);
#endif
   
   //---------------------------------------------------------------------------
   // Internal stuff
   //---------------------------------------------------------------------------
private:
   static int GetNumber(const char*& p);
};



