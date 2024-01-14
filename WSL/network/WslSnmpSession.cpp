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

#include "stdafx.h"
#include "WslSnmpSession.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWslSnmpSession::CWslSnmpSession()
{
   m_pSession = 0;
}

/*****************************************************************************/
CWslSnmpSession::~CWslSnmpSession()
{
   Close();
}

/*****************************************************************************/
void CWslSnmpSession::Close()
{
   if (m_pSession) 
   {
      VERIFY(SnmpMgrClose(m_pSession));
      m_pSession = NULL;
   }
}

/*****************************************************************************/
void CWslSnmpSession::Open(const char* pcszAddress,
                           const char* pcszCommunity) throw (CWslWin32Exception*)
{
   Close();

   m_pSession = SnmpMgrOpen((char*) pcszAddress, (char*) pcszCommunity, 500, 2);

   if (!m_pSession) throw new CWslWin32Exception();
}

/*****************************************************************************/
void CWslSnmpSession::Request(SnmpVarBindList& vbl, 
                              BYTE byRequest,
                              const char* pcszObjectId) throw (CException*)
{
   AsnInteger nErrorStatus, nErrorIndex;

   BOOL b = SnmpMgrRequest(m_pSession, byRequest, &vbl, &nErrorStatus, &nErrorIndex);

   if (!b) 
   {
      DWORD dwError = GetLastError();

      SnmpUtilVarBindFree(vbl.list);

      if (dwError == SNMP_MGMTAPI_TIMEOUT)
         throw new CWslTextException("SNMP: The request timed-out. OID: %s", pcszObjectId);
      else
         throw new CWslWin32Exception(dwError);
   }

   if (nErrorStatus != SNMP_ERRORSTATUS_NOERROR) 
   {
      SnmpUtilVarBindFree(vbl.list);      
      throw new CWslSnmpException(nErrorStatus, pcszObjectId);
   }
}

/*****************************************************************************/
void CWslSnmpSession::Set(const char* pcszObjectId, 
                          const char* pcszValue) throw (CWslWin32Exception*)
{
   SnmpVarBindList vbl;
   AsnObjectIdentifier oid;

   if (!SnmpMgrStrToOid((char*) pcszObjectId, &oid))
      throw new CWslTextException("SNMP: Invalid object ID!");

   vbl.len = 1;
   vbl.list = (SnmpVarBind*) SnmpUtilMemAlloc(sizeof(SnmpVarBind));
   vbl.list->name = oid;
   vbl.list->value.asnType = ASN_OCTETSTRING;
   vbl.list->value.asnValue.string.dynamic = false;
   vbl.list->value.asnValue.string.length = strlen(pcszValue);
   vbl.list->value.asnValue.string.stream = (BYTE*) pcszValue;

   Request(vbl, SNMP_PDU_SET, pcszObjectId);

   SnmpUtilVarBindFree(vbl.list);      
}

/*****************************************************************************/
void CWslSnmpSession::Set(const char* pcszObjectId, 
                          int nValue) throw (CWslWin32Exception*)
{
   SnmpVarBindList vbl;
   AsnObjectIdentifier oid;

   if (!SnmpMgrStrToOid((char*) pcszObjectId, &oid))
      throw new CWslTextException("SNMP: Invalid object ID!");

   vbl.len = 1;
   vbl.list = (SnmpVarBind*) SnmpUtilMemAlloc(sizeof(SnmpVarBind));
   vbl.list->name = oid;
   vbl.list->value.asnType = ASN_INTEGER32;
   vbl.list->value.asnValue.number = nValue;

   Request(vbl, SNMP_PDU_SET, pcszObjectId);

   SnmpUtilVarBindFree(vbl.list);      
}

/*****************************************************************************/
void CWslSnmpSession::GetNext(const char* pcszObjectId, 
                              CWslString& sValueObjectId,
                              CWslVariant& vValue) throw (CWslWin32Exception*)
{
   SnmpVarBindList vbl;
   AsnObjectIdentifier oid;

   // Prepare params

   if (!SnmpMgrStrToOid((char*) pcszObjectId, &oid))
      throw new CWslTextException("SNMP: Invalid object ID!");

   vbl.len = 1;
   vbl.list = (SnmpVarBind*) SnmpUtilMemAlloc(sizeof(SnmpVarBind));
   vbl.list->name = oid;
   vbl.list->value.asnType = ASN_NULL;

   // Call

   Request(vbl, SNMP_PDU_GETNEXT, pcszObjectId);

   // Get result

   char* pszOID = SnmpUtilOidToA(&vbl.list->name);  // freed by SNMP automatically
   sValueObjectId = pszOID;

   VariableToVariant(vbl.list->value, vValue);
   SnmpUtilVarBindFree(vbl.list);
}

/*****************************************************************************/
void CWslSnmpSession::Get(const char* pcszObjectId, 
                          CWslVariant& vValue) throw (CWslWin32Exception*)
{
   SnmpVarBindList vbl;
   AsnObjectIdentifier oid;

   // Prepare params

   if (!SnmpMgrStrToOid((char*) pcszObjectId, &oid))
      throw new CWslTextException("SNMP: Invalid object ID!");

   vbl.len = 1;
   vbl.list = (SnmpVarBind*) SnmpUtilMemAlloc(sizeof(SnmpVarBind));
   vbl.list->name = oid;
   vbl.list->value.asnType = ASN_NULL;

   // Call

   Request(vbl, SNMP_PDU_GET, pcszObjectId);

   // Get params

   VariableToVariant(vbl.list->value, vValue);
   SnmpUtilVarBindFree(vbl.list);
}

/*****************************************************************************/
void CWslSnmpSession::VariableToVariant(AsnAny& Src, CWslVariant& Dest)
{
   switch (Src.asnType)
   {
   case ASN_INTEGER32:
      Dest = (int) Src.asnValue.number;
      break;
   
   case ASN_OCTETSTRING:
      Dest.SetType(CWslVariant::VT_STRING);

      memcpy(Dest.pString->GetBufferSetLength(Src.asnValue.string.length),
         Src.asnValue.string.stream,
         Src.asnValue.string.length);
      
      Dest.pString->ReleaseBuffer(Src.asnValue.string.length);
      break;

   default:
      ASSERT(false);
      Dest.SetNull();
   }
}

/*****************************************************************************
*
* EXCEPTION
*
*****************************************************************************/

/*****************************************************************************/
CWslSnmpException::CWslSnmpException(int nErrorCode, const CString& sOID)
{
   m_nErrorCode = nErrorCode;
   m_sOID = sOID;
}

/*****************************************************************************/
CWslString CWslSnmpException::GetErrorMsg()
{
   CWslString s;

   switch (m_nErrorCode)
   {
   case SNMP_ERRORSTATUS_TOOBIG:
      s = "SNMP: Too many data, can't construct a SNMP message!";
      break;
   case SNMP_ERRORSTATUS_NOSUCHNAME:
      s = "SNMP: An unknown object specified!";
      break;
   case SNMP_ERRORSTATUS_BADVALUE:
      s = "SNMP: Bad value for the specified object!";
      break;
   case SNMP_ERRORSTATUS_READONLY:
      s = "SNMP: The object is read only!";
      break;
   case SNMP_ERRORSTATUS_GENERR:
      s = "SNMP: General error!";
      break;
   default:
      s.Format("SNMP: Unknown error code %d!", m_nErrorCode);
      break;
   }

   return s + " OID: " + m_sOID;
}
