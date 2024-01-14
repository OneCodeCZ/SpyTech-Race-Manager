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

#pragma once

#include "WslXMLTreeItem.h"
#include "../generic/WslString.h"
#include "../generic/WslPtr.h"
#include "../generic/WslException.h"

class CWslXMLComment;
typedef CWslPtr<CWslXMLComment> CWslXMLCommentPtr;

//*****************************************************************************
class WSL_API CWslXMLComment : public CWslXMLNode
{
protected:
	CWslString		m_sValue;

public:
	CWslXMLComment() {};
	CWslXMLComment(const char* pcszValue) : m_sValue(pcszValue) {};
	virtual ~CWslXMLComment() {};

   //---------------------------------------------------------------------------
   // From CWslXMLNode
   //---------------------------------------------------------------------------
	
	_public virtual CWslString GetCharData() const { return m_sValue; }
	_public virtual void SetCharData(const CWslString& s) { m_sValue = s; }
	_public virtual CWslString GetName() const { _ASSERT(false); return CWslString(); }
	_public virtual void SetName(const CWslString& s) { _ASSERT(false); }
	_public virtual void OnTraverse(CWslXMLContext& context) throw (CWslException*);
};








