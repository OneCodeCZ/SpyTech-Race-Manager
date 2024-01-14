///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple XML Parser
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslXMLCData.h"
#include "../files/WslFile.h"
#include "WslXmlParser.h"

/*****************************************************************************/
void CWslXMLCData::OnTraverse(CWslXMLContext& context) throw (CWslException*)
{
	ASSERT(GetChildrenCount()==0);

	CSaveContext* pContext = dynamic_cast<CSaveContext*>(&context);
	if (!pContext) return;

   if (!m_sValue.IsEmpty())
   {
      pContext->f.Format("<![CDATA[%s]]>", m_sValue);
   }
}