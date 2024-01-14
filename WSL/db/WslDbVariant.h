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

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "..\generic\WslString.h"
#include "..\generic\WslLongBinary.h"
#include "..\generic\WslDateTime.h"
#include "..\generic\WslDate.h"

//*****************************************************************************
class WSL_API CWslVariant
{
public:
	enum ETypes 
   { 
		VT_NULL, VT_BOOL, VT_BYTE, VT_SHORT,
		VT_LONG, VT_FLOAT, VT_DOUBLE, VT_DATE, 
		VT_STRING, VT_BINARY, VT_BIGINT
	};

protected:
	ETypes	m_eType;

public:
	union
	{
      bool					bVal;
      BYTE					byVal;
      short             wVal;
      long              nVal;
      __int64           qVal;
      float             fVal;
      double            dblVal;
      TIMESTAMP_STRUCT*	pDate;
      CWslString*			pString;
      CWslLongBinary*	pBinary;
	};

public:
	CWslVariant();
	~CWslVariant();

	// Empties the variant. 
	void SetNull();

	// Sets/gets the variant type.
	void SetType(ETypes eType);
	inline ETypes GetType() const { return m_eType; }

	// Returns pointer to a buffer and size of the buffer, where a value 
	// of m_dwTyte can be stored.
	void* GetData(int* pnSize);

	// Returns TRUE if the variant is set to NULL.
	inline BOOL IsNull() const { return m_eType == VT_NULL; }

	//---------------------------------------------------------------------------
	// Assignment of values. 
	// The type is automatically set.
	//---------------------------------------------------------------------------
	
	const CWslVariant& operator =(const CWslVariant& dbv);
	const CWslDateTime& operator =(const CWslDateTime& dt);
	const CWslDate& operator =(const CWslDate& d);
	const CWslString& operator =(const CWslString& s);
	const char* operator =(const char* pcsz);
	int operator =(int n);
	DWORD operator =(DWORD  dw);
	bool operator =(bool b);

  //---------------------------------------------------------------------------
	// Retrieving the value
	//---------------------------------------------------------------------------

	operator CWslString() const { return m_eType==VT_STRING ? *pString : ""; }
   operator int() const;
   operator bool() const;
   operator CWslDate() const;
   operator CWslDateTime() const;

  //---------------------------------------------------------------------------
	// Serialization
	//---------------------------------------------------------------------------

#ifdef _MFC_VER
	// Searialization
	void Serialize(CArchive& ar);		// throws
#endif
};





















