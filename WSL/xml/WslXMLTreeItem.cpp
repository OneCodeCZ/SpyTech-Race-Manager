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
#include "WslXMLTreeItem.h"
#include "../generic/WslString.h"


//*****************************************************************************
CWslXMLNode::CWslXMLNode()
{
	m_pParent = NULL;
}

//*****************************************************************************
CWslXMLNode::~CWslXMLNode()
{
	int i;
	for (i=m_aChildern.GetSize(); i--;) delete m_aChildern[i];
}

//*****************************************************************************
void CWslXMLNode::SetParent(CWslXMLNode* pParent)
{
	if (m_pParent) m_pParent->RemoveChild(this);
   
   if (pParent)
	   pParent->AddChild(this);
   else
      m_pParent = NULL;

   ASSERT(m_pParent == pParent);
}

//*****************************************************************************
void CWslXMLNode::AddChild(CWslXMLNode* pChild, int nIdx /*= -1*/)
{
	ASSERT(nIdx>=-1 && nIdx<=GetChildrenCount());
	if (nIdx == -1) nIdx = GetChildrenCount();

	m_aChildern.InsertAt(nIdx);
	m_aChildern[nIdx] = pChild;
	pChild->m_pParent = this;
}

//*****************************************************************************
int CWslXMLNode::RemoveChild(CWslXMLNode* pChild)
{
	ASSERT(pChild);

	for (int i=m_aChildern.GetSize(); i--;)
	{
		if (m_aChildern[i] == pChild)
		{
			m_aChildern.RemoveAt(i);
			pChild->m_pParent = NULL;
			return i;
		}
	}

   return -1;
}

/*****************************************************************************/
void CWslXMLNode::OnTraverse(CWslXMLContext& context) throw (CWslException*)
{
	for (int i=0; i<m_aChildern.GetSize(); i++)
		m_aChildern[i]->OnTraverse(context);
}
