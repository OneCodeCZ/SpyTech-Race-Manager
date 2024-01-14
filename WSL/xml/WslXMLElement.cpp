///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple XML Parser
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslXMLElement.h"
#include "..\generic\WslString.h"
#include "..\files\WslFile.h"
#include "WslXMLText.h"
#include "WslXMLParser.h"

/*****************************************************************************/
CWslXMLElement::CWslXMLElement()
{
	m_bHasEndTag = true;
}

//*****************************************************************************
CWslXMLElement::CWslXMLElement(const char* pcszName, BOOL bHasEndTag /*= true*/)
	: m_sName(pcszName), m_bHasEndTag(bHasEndTag)
{
}

//*****************************************************************************
CWslXMLElement::~CWslXMLElement()
{
	for (int i=m_aAttributes.GetSize(); i--;) delete m_aAttributes[i];
}

//*****************************************************************************
CWslString CWslXMLElement::GetCharData() const
{
	CWslString s;
	for (int i=0; i<GetChildrenCount(); i++) s += GetChild(i)->GetCharData();
	return s;
}

//*****************************************************************************
CWslString CWslXMLElement::GetImmediateCharData() const
{
   CWslString s;
   for (int i=0; i<GetChildrenCount(); i++) {
      if (dynamic_cast<CWslXMLText*>(GetChild(i))) s += GetChild(i)->GetCharData();
   }
	return s;
}
/*****************************************************************************
*
* ATTRIBUTES
*
*****************************************************************************/

//*****************************************************************************
void CWslXMLElement::SetAttribute(const char* pcszName, const CWslString& sValue)
{
   // Exists?

	for (int i=m_aAttributes.GetSize(); i--;) 
	{
		if (!stricmp(m_aAttributes[i]->GetName(), pcszName)) 
		{
         if (sValue.IsEmpty())
            m_aAttributes.RemoveAt(i);
         else
			   m_aAttributes[i]->SetValue(sValue);
			return;
		}
	}

   // New

	CWslXMLAttribute* pa = new CWslXMLAttribute(pcszName, sValue);
	m_aAttributes.GrowBy(1);
	m_aAttributes[m_aAttributes.GetSize()-1] = pa;
}

/*****************************************************************************/
void CWslXMLElement::SetAttribute(const char* pcszName, int nValue)
{
	CWslString s;
	s.Format("%d", nValue);

   // Exists?

	for (int i=m_aAttributes.GetSize(); i--;) 
	{
		if (!stricmp(m_aAttributes[i]->GetName(), pcszName)) 
		{
			m_aAttributes[i]->SetValue(s);
			return;
		}
	}

   // New

	CWslXMLAttribute* pa = new CWslXMLAttribute(pcszName, s);
	m_aAttributes.GrowBy(1);
	m_aAttributes[m_aAttributes.GetSize()-1] = pa;
}

//*****************************************************************************
CWslString CWslXMLElement::GetAttributeString(const char* pcszName) const
{
	for (int i=m_aAttributes.GetSize(); i--;) 
	{
		if (!stricmp(m_aAttributes[i]->GetName(), pcszName)) 
			return m_aAttributes[i]->GetValue();
	}
	return CWslString();
}

//*****************************************************************************
int CWslXMLElement::GetAttributeInt(const char* pcszName, int nDefaultValue) const
{
   CWslString s;
   return GetAttributeString(pcszName, s) ? atoi(s) : nDefaultValue;
}

//*****************************************************************************
double CWslXMLElement::GetAttributeDouble(const char* pcszName) const
{
   return atof(GetAttributeString(pcszName));
}

/*****************************************************************************/
BOOL CWslXMLElement::GetAttributeString(const char* pcszName, CWslString& sValue) const
{
	for (int i=m_aAttributes.GetSize(); i--;) 
	{
		if (!stricmp(m_aAttributes[i]->GetName(), pcszName)) 
		{
			sValue = m_aAttributes[i]->GetValue();
			return true;
		}
	}

	return false;
}

/*****************************************************************************/
BOOL CWslXMLElement::GetAttributeInt(const char* pcszName, int& nValue) const
{
	for (int i=m_aAttributes.GetSize(); i--;) 
	{
		if (!stricmp(m_aAttributes[i]->GetName(), pcszName)) 
		{
			nValue = atoi(m_aAttributes[i]->GetValue());
			return true;
		}
	}

	return false;
}

/*****************************************************************************/
BOOL CWslXMLElement::GetAttributeDouble(const char* pcszName, double& dblValue) const
{
	int i;
	ITERATE(m_aAttributes, i)
	{
		if (!stricmp(m_aAttributes[i]->GetName(), pcszName)) 
		{
			dblValue = atof(m_aAttributes[i]->GetValue());
			return true;
		}
	}
	return false;
}

/******************************************************************************/
CWslXMLAttribute* CWslXMLElement::GetAttribute(const char* pcszName) const
{
   int i;
   ITERATE(m_aAttributes, i) 
   {
      if (!stricmp(m_aAttributes[i]->GetName(), pcszName))
      {
         return m_aAttributes[i];
      }
   }
   
   return NULL;
}

/*****************************************************************************/
void CWslXMLElement::RemoveAttribute(const char* pcszName)
{
   int i;
   ITERATE(m_aAttributes, i) 
   {
      if (!stricmp(m_aAttributes[i]->GetName(), pcszName))
      {
         m_aAttributes.RemoveAt(i);
         break;
      }
   }
}

/******************************************************************************/
void CWslXMLElement::RemoveAttribute(CWslXMLAttribute* pAttrib)
{
   ASSERT(pAttrib);

   int i;
   ITERATE(m_aAttributes, i)
   {
      if (m_aAttributes[i] == pAttrib) 
      {
         m_aAttributes.RemoveAt(i);
         break;
      }
   }
}

/*****************************************************************************
*
* CHILDERN ELEMENTS
*
*****************************************************************************/

//*****************************************************************************
CWslXMLElement* CWslXMLElement::GetElement(const char* pcszName) const
{
	ASSERT(pcszName);
	int n = GetFirstElementPos	(pcszName);
	return n==-1 ? NULL : (CWslXMLElement*) m_aChildern[n];
}

//*****************************************************************************
int CWslXMLElement::GetNextElementPos(int nPos, const char* pcszName) const 
{
	for (int i=nPos+1; i<m_aChildern.GetSize(); i++)
	{
		CWslXMLElement* p = dynamic_cast<CWslXMLElement*>(m_aChildern[i]);
		if (p && (!pcszName || !stricmp(pcszName, p->GetName()))) return i;
	}
	return -1;
}

/*****************************************************************************
*
* PERSISTANCE
*
*****************************************************************************/

/*****************************************************************************/
void CWslXMLElement::OnTraverse(CWslXMLContext& context) throw (CWslException*)
{
   // Save context?

	CSaveContext* pContext = dynamic_cast<CSaveContext*>(&context);
	if (!pContext) { CWslXMLNode::OnTraverse(context); return; }

   // Tag

   if (!(pContext->dwFlags & ESaveFlags::NO_NEWLINES)) 
   {
      pContext->f.Format("\r\n%s", pContext->sIndent);
   }
	pContext->f.Format("<%s", m_sName);

   // Attributes

	for (int i=0; i<m_aAttributes.GetSize(); i++)
	{
		pContext->f.Format(" %s=\"%s\"", 
			m_aAttributes[i]->GetName(), 
         CWslXMLParser::EncodeSpecialChars(m_aAttributes[i]->GetValue(), 
            CWslXMLParser::ATTRIBUTE, 
            pContext->nCodePage)
         );
	}

   // Children

	if (GetChildrenCount())
	{
		pContext->f.Write(">");
		
		pContext->IncIndent();
		CWslXMLNode::OnTraverse(context);
		pContext->DecIndent();

		if (!(pContext->dwFlags & ESaveFlags::NO_NEWLINES) &&
         dynamic_cast<CWslXMLElement*>(m_aChildern[m_aChildern.GetSize()-1])) 
		{
			pContext->f.Format("\r\n%s", pContext->sIndent);
		}
		pContext->f.Format("</%s>", m_sName);
	}
	else
	{
		pContext->f.Write("/>");
	}
}

/*****************************************************************************/
void CWslXMLElement::Save(CWslGenericFile& f, 
                          int nCodePage,
                          DWORD dwFlags) 
                          throw (CWslException*)
{
	// Write header

	if (!(dwFlags & ESaveFlags::NO_HEADER))
	{
		f.WriteLn("<!-- This file was generated by the SpyTech XML Engine. See http://www.spytech.cz -->");

      CWslString s;

      switch (nCodePage)
      {
      case CP_UTF8: s = "utf-8"; break;
      case 1250:    s = "win1250"; break;
      default: ASSERT(false);
      }

		f.FormatLn("<?xml version=\"1.0\" encoding=\"%s\"?>", s);
	}

	// Save tree
	
	CSaveContext c(f, nCodePage, dwFlags);
	OnTraverse(c);
}

/*****************************************************************************/
void CWslXMLElement::Save(const char* pcszPathName, 
                          int nCodePage, 
                          DWORD dwFlags) 
                          throw (CWslException*)
{
	ASSERT(pcszPathName);
	CWslFile f;
	f.Create(pcszPathName);
	Save(f, nCodePage, dwFlags);
	f.Close();
}







