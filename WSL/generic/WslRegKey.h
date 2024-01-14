///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslRegKey
// | P |  /  |  |  ~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

//class CWslString;
class CWslWin32Exception;
class CWslDate;

//*****************************************************************************
class WSL_API CWslRegKey
{
protected:
	HKEY m_hKey;

public:
	CWslRegKey();
	~CWslRegKey();

   // Opens the given pcszSubKey of the given key
   // Returns FALSE, if the key doesn't exist.
   // Throws an exception on other errors
	bool Open(HKEY hKey, const char* pcszSubKey, bool bReadOnly) throw (CWslWin32Exception*);

   // Creates the given subkey. The subkey can be multilevel.
	void Create(HKEY hKey, const char* pcszSubKey) throw (CWslWin32Exception*);

   // Closes the key
	void Close();

   bool IsOpened() const { return m_hKey != NULL; }

   //---------------------------------------------------------------------------
   // DELETING 
   //---------------------------------------------------------------------------

	// Deletes the given key recursively
	static void DeleteKey(HKEY hKey, const char* pcszSubKey) throw (CWslWin32Exception*);

	// Deletes the given value
	void DeleteValue(const char* pcszName) throw (CWslWin32Exception*);

   //---------------------------------------------------------------------------
   // GETTING VALUES
   // If the key is not opened or the value is not found, the default value is used.
   //---------------------------------------------------------------------------

   // Native types
   CWslString GetString(const char* pcszValName, const char* pcszDefVal = NULL) throw (CWslWin32Exception*);
	int GetInt(const char* pcszValName, int nDefValue) throw (CWslWin32Exception*);

   // Types mapped to the native types 
   CWslDate GetDate(const char* pcszValName, const CWslDate& DefDate) throw (CWslWin32Exception*);

   //---------------------------------------------------------------------------
   // SETTING VALUES
   //---------------------------------------------------------------------------

   // Native types
   // If the pcszValName is an empty string, the key's default valus is set.
	void Set(const char* pcszValName, const CWslString& sValue) throw (CWslWin32Exception*);
	void Set(const char* pcszValName, int nValue) throw (CWslWin32Exception*);

   // Types mapped to the native types 
   void Set(const char* pcszValName, const CWslDate& Date) throw (CWslWin32Exception*);

   //---------------------------------------------------------------------------
   // VARIOUS
   //---------------------------------------------------------------------------

	// Returns the number of subkeys.
	int GetNumSubKeys() throw (CWslWin32Exception*);
	
	// Returns the name of the subkey at the given index.
	// Returns FALSE if there is no key with the given index.
	// Used for subkeys enumerating.
	BOOL GetSubKeyName(int nIndex, CWslString& sName) throw (CWslWin32Exception*);

	// Returns the name of the value at the given index.
	// Returns FALSE if there is no value with the given index.
	// Used for values enumerating.
	BOOL GetValueName(int nIndex, 
		CWslString& sName,
		DWORD* pdwType = NULL) 
		throw (CWslWin32Exception*);

	// Various helpers
	operator HKEY() { return m_hKey; }
};
