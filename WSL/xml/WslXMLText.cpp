///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple XML Parser
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 09
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslXMLText.h"
#include "../files/WslFile.h"
#include "WslXmlParser.h"

/*****************************************************************************/
void CWslXMLText::OnTraverse(CWslXMLContext& context) throw (CWslException*)
{
	ASSERT(GetChildrenCount()==0);

	CSaveContext* pContext = dynamic_cast<CSaveContext*>(&context);
	if (!pContext) return;

	if (!m_sValue.IsEmpty())
	{
      pContext->f.Write(CWslXMLParser::EncodeSpecialChars(m_sValue, CWslXMLParser::CDATA, pContext->nCodePage));
	}
}