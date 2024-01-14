///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech C++ Library - SNMP Session
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 11
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <Mgmtapi.h>
#include <Snmp.h>
#include "..\generic\WslException.h"
#include "..\generic\WslWin32Exception.h"
#include "..\db\WslDbVariant.h"

/*****************************************************************************/
class WSL_API CWslSnmpException : public CWslException
{
protected:
   int      m_nErrorCode;
   CString  m_sOID;

public:
   CWslSnmpException(int nErrorCode, const CString& sOID);

   int GetCode() { return m_nErrorCode; }

   virtual CWslString GetErrorMsg();
};

/*****************************************************************************/
class WSL_API CWslSnmpSession  
{
protected:
   LPSNMP_MGR_SESSION m_pSession;

public:
	CWslSnmpSession();
	virtual ~CWslSnmpSession();

   //------------------------------------------------------------------------
   // Connection
   //------------------------------------------------------------------------

   void Open(const char* pcszAddress, 
      const char* pcszCommunity) throw (CWslWin32Exception*);

   void Close();

   //------------------------------------------------------------------------
   // Set/Get
   //------------------------------------------------------------------------

   void Set(const char* pcszObjectId, const char* pcszValue) throw (CWslWin32Exception*);
   void Set(const char* pcszObjectId, int nValue) throw (CWslWin32Exception*);

   void GetNext(const char* pcszObjectId, 
      CWslString& sValueObjectId,
      CWslVariant& vValue) throw (CWslWin32Exception*);

   void Get(const char* pcszObjectId, 
      CWslVariant& vValue) throw (CWslWin32Exception*);

   //------------------------------------------------------------------------
   // INTERNAL
   //------------------------------------------------------------------------
protected:

   void Request(SnmpVarBindList& vbl, BYTE byRequest, const char* pcszObjectId) throw (CException*);
   void VariableToVariant(AsnAny& Src, CWslVariant& Dest);
};
