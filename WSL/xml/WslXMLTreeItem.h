///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple XML Parser
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 11
// | H |  _||   |
// |___|________|	
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../generic/WslArray.h"
#include "../generic/WslString.h"
#include "../generic/WslException.h"
class CWslGenericFile;

//*****************************************************************************
class WSL_API CWslXMLContext
{
public:
	virtual ~CWslXMLContext() { };
};

//*****************************************************************************
class WSL_API CWslXMLNode	  
{
public:
	// Context for saving the tree into a XML file.
	class WSL_API CSaveContext : public CWslXMLContext
	{
	public:
		CWslGenericFile&	f;
		int				   nLevel;
		CWslString			sIndent;
      int               nCodePage;
      DWORD             dwFlags;
	public:
		CSaveContext(CWslGenericFile& f_, int nCodePage_, DWORD dwFlags_) : f(f_) 
      { 
         nLevel = 0; 
         nCodePage = nCodePage_; 
         dwFlags = dwFlags_;
      };
		void IncIndent() { sIndent = sIndent + "  "; nLevel++; }
		void DecIndent() { sIndent = sIndent.Left(sIndent.GetLength()-2); nLevel--; }
	};

protected:
	CWslArray<CWslXMLNode*>	m_aChildern;
	CWslXMLNode*   			m_pParent;

public:
	CWslXMLNode();
	virtual ~CWslXMLNode();

  //---------------------------------------------------------------------------
	// Name, value stuff
	//---------------------------------------------------------------------------

	_public virtual CWslString GetName() const = 0;
	_public virtual CWslString GetCharData() const = 0;
	_public virtual void SetName(const CWslString& s) = 0;
	_public virtual void SetCharData(const CWslString& s) = 0;

  //---------------------------------------------------------------------------
	// Tree manipulation stuff
	//---------------------------------------------------------------------------

	// Gets/sets the parent element
	CWslXMLNode* GetParent() const { return m_pParent; }
	void SetParent(CWslXMLNode* pParent);

	// Returns number of children elements
	int GetChildrenCount() const { return m_aChildern.GetSize(); }

	// Returns a child at given index
	CWslXMLNode* GetChild(int nIdx) const { return m_aChildern[nIdx]; }

	// Adds a new child
	void AddChild(CWslXMLNode* pChild, int nIdx = -1);

	// Removes the given child. 
   // Returns the index of the removed child.
   // Reutnrs -1 if pChild was not this node's child.
	int RemoveChild(CWslXMLNode* pChild);

	// Called when the tree is being traversed.
	// The default implementation calls OnTraverse() for all childern.
	virtual void OnTraverse(CWslXMLContext& context) throw (CWslException*) = 0;

	//---------------------------------------------------------------------------
	// Internal
	//---------------------------------------------------------------------------
protected:
	void IncIndent();
	void DecIndent();
};
