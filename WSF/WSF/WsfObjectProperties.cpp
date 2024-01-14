///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfPropertySheet.h"
#include "WsfObjectProperties.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfObjectProperties::CWsfObjectProperties()
{
   m_pps = NULL;
}

/*****************************************************************************/
CWsfObjectProperties::~CWsfObjectProperties()
{
   ASSERT(!m_pps);
}

/*****************************************************************************/
BOOL CWsfObjectProperties::PSCallback(void* pData, CWsfPropertySheet& s, BOOL bOk)
{
   CWsfObjectProperties* p = (CWsfObjectProperties*) pData;
   return p->OnCloseProperties(bOk);
}

/*****************************************************************************/
BOOL CWsfObjectProperties::DoModal()
{
   ASSERT(!m_pps);
   m_pps = new CWsfPropertySheet(false, PSCallback, this);
   BOOL b = false;
   
   if (OnInitProperties())
   {
      b = m_pps->DoModal()==IDOK;
   }

   //m_pps->RemoveAllPages(false);
   delete m_pps;
   m_pps = NULL;
   return b;
}











