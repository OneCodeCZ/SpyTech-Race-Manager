///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004
//
///////////////////////////////////////////////////////////////////////////////

// CWsfRegistryProfile represents application's storage space 
// in the Windows register.

#pragma once

/*****************************************************************************/
class CWsfRegistryProfileKey
{
protected:
   class CValue
   {
   protected:
      CWsfRegistryProfile& m_rp;

   public:
      CValue(CWsfRegistryProfile& rp);

      void operator = (int nValue);
      void operator = (const CString& sValue);
      void operator = (const CWslDate& Value);

      operator int ();
      operator CString ();
      operator CWslDate ();
   };

protected:
   CString  m_sKey;

public:
	CWsfRegistryProfile();
	virtual ~CWsfRegistryProfile();
   
   void SetFolder(const char* pcszFolder) { m_sKey = pcszFolder; }
   const CString& GetFolder() const { return m_sKey; }

   CValue operator [] (const char* pcszValue);
};
