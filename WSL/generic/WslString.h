///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslString
// | P |  /  |  |  ~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// NOTE: MFC CString and CWslString MUST be compatible!!!
// More precisely, CWslString must support CString API.

#pragma once
#include "WslArray.h"

#ifndef _MFC_VER

//*****************************************************************************
class CWslString
{
private:
	class CData
	{
	public:
		int nLen;						// string length
		int nBufferSize;		// includes the termination char
		int nRefCount;
		// Here follows an array of the nBufferSize size.
		// m_pszData points here
	public:
		char* GetData() { return (char*)(this+1); }
	};

private:
	// Pointer to the class must be the same as (char**) 
	// to be printf compatible
	// CData* = m_pszData - sizeof(CData)
	char*  m_pszData;

	// Empty string, there is only one
	static CData	m_EmptyString;
	static int		m_nDummy;			// string data (zero) for empty string

public:
	CWslString();
	CWslString(const CWslString& s);
	CWslString(const char* pcsz);			// pcsz can be NULL
	CWslString(char ch);
   CWslString(const unsigned short* pcwsz, int nCharSet);

	~CWslString();

   //---------------------------------------------------------------------------
	// General
	//---------------------------------------------------------------------------

	// Returns length of the string
	inline int GetLength() const { return GetCData()->nLen; }

	// Returns TRUE if the string is empty.
	inline BOOL IsEmpty() const { return GetCData()->nLen==0; }

   //---------------------------------------------------------------------------
	// Buffer management
	//---------------------------------------------------------------------------

	// Allocates a buffer of the given size + 1 for the terminator char
	// and returns a pointer to it.
	// You can directly access the buffer. 
   // Call ReleaseBuffer() after you have finished using the buffer.
	// The current string content is preserved if nSize is >= to the current string size.
	char* GetBuffer(int nSize);
	char* GetBufferSetLength(int nSize) { return GetBuffer(nSize); }

	// Updates internal info (length,...) after direct access
	// to the buffer is finished. Buffer is obtained by GetBuffer() method.
	// If nSize is -1 the zero terminated string in the buffer is assumed.
	// Otherwise the string of length nSize is assumed. Zero is appended
	// at the end of the string.
	void ReleaseBuffer(int nSize = -1);

   //---------------------------------------------------------------------------
	// Setting a new value
	//---------------------------------------------------------------------------

	// Empties the string
	void Empty();

	// Assignment operator
	const CWslString& operator =(const char* pcsz);  // pcsz can be NULL
	const CWslString& operator =(const CWslString& s);
	const CWslString& operator =(char ch);
	const CWslString& operator =(char* psz) { return operator =((const char*) psz); }

	// Formats a string. The length of the resulting string is truncated to
	// 1024 chars including the terminating character.
	void Format(const char* pcszFormat, ...);
	void Format(const char* pcszFormat, va_list& l);
	void AppendFormat(const char* pcszFormat, ...);

	// Concatenates a new string to the end of an existing string.
	// NOTE: "CWslString s; s+=s;" not supported yet, use "s=s+s".
	const CWslString& operator +=(const char* pcsz);
	const CWslString& operator +=(const CWslString& s);
	const CWslString& operator +=(char ch);

  //---------------------------------------------------------------------------
	// Concatenation of two strings
	//---------------------------------------------------------------------------
	
	friend const CWslString operator +(const CWslString& l, const CWslString& r);
	friend const CWslString operator +(const CWslString& s, const char* pcsz);
	friend const CWslString operator +(const char* pcsz, const CWslString& s);
	friend const CWslString operator +(const CWslString& s, char ch);
	friend const CWslString operator +(char ch, const CWslString& s);

  //---------------------------------------------------------------------------
	// Modifications
	//---------------------------------------------------------------------------
	
	// Converts to lowercase/uppercase using _strlwr(), _strupr() functions.
	// These functions use the current locale information.
	void MakeLower();
	void MakeUpper();

	// Translates chars of the string using the given 256 byte large table.
	void Translate(const void* pTable);

	// Replaces all occurences of the given char with another given char
	// The matches are case-sensitive.
	// Returns the number of replacements.
	int Replace(char chOld, char chNew);

  //---------------------------------------------------------------------------
	// Comparing
	//---------------------------------------------------------------------------

	// If pcsz is NULL false is always returned.
	bool operator ==(const char* pcsz);
	bool operator ==(const CWslString& s);
	bool operator ==(char ch);

	inline bool operator !=(const char* pcsz) { return ! operator ==(pcsz); }
	inline bool operator !=(const CWslString& s) { return ! operator ==(s); }
	inline bool operator !=(char ch) { return ! operator ==(ch); }

	bool operator < (const CWslString& s);
	bool operator > (const CWslString& s);
	bool operator <= (const CWslString& s);
	bool operator >= (const CWslString& s);

  //---------------------------------------------------------------------------
	// Various
	//---------------------------------------------------------------------------

	// Conversion operator
	operator const char*() const { return (const char*) m_pszData; }

  //---------------------------------------------------------------------------
	// Extraction
	//---------------------------------------------------------------------------

	// Explodes the string into tokens separated by a set
	// of delimiters.
	CWslObArray<CWslString> Explode(const char* pcszDelimiters) const;

	// Returns nCount of chars from the left side of the string.
	CWslString Left(int nCount) const;

   // Returns nCount characters beginning with nFirst.
   // PARAMETERS:
   //   nCount .. if -1, the rest of the string is returned
   CWslString Mid(int nFirst, int nCount = -1);

   // Get character at position
   char GetAt(int nPos) const { ASSERT(nPos < GetLength()); return m_pszData[nPos]; }

   int Find(const char *pcszPattern);

protected:
  //---------------------------------------------------------------------------
	// Internal
	//---------------------------------------------------------------------------

	// Allocates buffer memory of the given size.
	// It take care about referencing.
	// If bKeepData is TRUE the char data are copied to the
	// newly allocated memory.
	void Alloc(int nSize, BOOL bKeepData);

	// Returns the size of the allocsted buffer
	const int GetAllocLength() const { GetCData()->nBufferSize; }

	// Sets/gets the CData pointer
	inline CData* GetCData() const { return (CData*)((BYTE*)m_pszData-sizeof(CData)); }
	inline void SetCData(CData* pData) { m_pszData = (char*) (pData+1); }

	// Duplicates the CData structure and set's the new CData
	// as the string's CData. The previous CData is released.
	void DuplicateData();
};

#else
#define CWslString CString
#endif

/*****************************************************************************
*
* CWslStringArray
*
*****************************************************************************/

class WSL_API CWslStringArray : public CWslObArray<CWslString>
{
public:
  // Explodes the given string into an array of strings.
  // Strings are delimited by the given list of delimiters.
  // Max. length of a single string is 1023.
  void ExplodeString(const char* pcszText, const char* pcszDelimiters);
};

