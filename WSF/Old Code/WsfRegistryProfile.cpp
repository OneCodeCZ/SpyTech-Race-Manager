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

#include "stdafx.h"
#include "main.h"
#include "WsfRegistryProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfRegistryProfile::CWsfRegistryProfile()
{

}

/*****************************************************************************/
CWsfRegistryProfile::~CWsfRegistryProfile()
{

}


/*****************************************************************************
*
* VALUE
*
*****************************************************************************/

CWsfRegistryProfile::CValue::CValue(CWsfRegistryProfile& rp)
: m_rp(rp)
{
}

/*****************************************************************************/
void CWsfRegistryProfile::CValue::operator = (int nValue)
{
   VERIFY(WsfGetApp()->WriteProfileInt(m_rp.GetKey(), nValue));
}

/*****************************************************************************/
void CWsfRegistryProfile::CValue::operator = (const CString& sValue)
{
   VERIFY(WsfGetApp()->WriteProfileString(m_rp.GetKey(), sValue));
}

/*****************************************************************************/
void CWsfRegistryProfile::CValue::operator = (const CWslDate& Value)
{
   VERIFY(WsfGetApp()->WriteProfileInt(m_rp.GetKey(), Value.Get()));
}

/*****************************************************************************/
CWsfRegistryProfile::CValue::operator int ()
{
   WsfGetApp()->GetProfileInt(m_rp.GetKey(), nValue));
}

/*****************************************************************************/
CWsfRegistryProfile::CValue::operator CString ()
{

}

/*****************************************************************************/
CWsfRegistryProfile::CValue::operator CWslDate ()
{

}