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

#pragma once

#include "WslXMLTreeItem.h"
#include "../generic/WslArray.h"
#include "../generic/WslString.h"
#include "../generic/WslPtr.h"
#include "../generic/WslException.h"

class CWslXMLElement;
typedef CWslPtr<CWslXMLElement> CWslXMLElementPtr;

//*****************************************************************************
class WSL_API CWslXMLAttribute
{
protected:
	CWslString m_sName;
	CWslString m_sValue;

public:
   CWslXMLAttribute() { };
	CWslXMLAttribute(const CWslString& sName, const CWslString& sValue) 
		: m_sName(sName), m_sValue(sValue)
	{
		m_sName.MakeLower(); 
	}

   // Name
   const CWslString& GetName() const { return m_sName; }

   // Value
   const CWslString& GetValue() const { return m_sValue; }
   void SetValue(const CWslString& sValue) { m_sValue = sValue; }
};

//*****************************************************************************
class WSL_API CWslXMLElement : public CWslXMLNode
{
public:
	enum ESaveFlags 
   { 
      NONE=0, 
      NO_HEADER=1,   // <?xml ... ?> is not generated
      NO_NEWLINES=2  // Doesn't format the output to be nice for human readers
   };

protected:
	CWslArray<CWslXMLAttribute*>	m_aAttributes;
	CWslString		m_sName;
	BOOL				m_bHasEndTag;
	
public:
   //---------------------------------------------------------------------------
	// General
	//---------------------------------------------------------------------------
	
	CWslXMLElement();
	CWslXMLElement(const char* pcszName, BOOL bHasEndTag = true);
	virtual ~CWslXMLElement();

	// Save the tree to an XML file.
	// NOTE: The default implementation of CWslXMLElement::OnTraverse() and 
	//   CWslXMLCharData::OnTraverse() methods store the tree to the XML file.
	//   CSaveContext class is used.
   // pcszCodePage is stored in the <?xms ... encoding="xxx" ?>.
   //   No charset conversion is performed!
	void Save(const char* pcszPathName, int nCodePage, DWORD dwFlags = 0) throw (CWslException*);
	void Save(CWslGenericFile& f, int nCodePage, DWORD dwFlags = 0) throw (CWslException*);

	// If the element has no end tag, it returns true.
	// NOTE: This is only for HTML compatibility. According
	// XML specs, all elements must have an end tag.
	virtual BOOL HasNoEndTag() const { return !m_bHasEndTag; }

   //---------------------------------------------------------------------------
	// Attributes stuff
	//---------------------------------------------------------------------------
	
	// Returns the attribute's value. If the attribute is not
	// present, empty string or 0 is returned.
	CWslString GetAttributeString(const char* pcszName) const;
	int GetAttributeInt(const char* pcszName, int nDefaultValue = 0) const;
	double GetAttributeDouble(const char* pcszName) const;

	// Returns TRUE if the attribute is present and stores its
	// value into the "value" parameter.
	// If the attribute is not present, FALSE is returned and
	// the "value" parameter is not changed.
	BOOL GetAttributeString(const char* pcszName, CWslString& sValue) const;
	BOOL GetAttributeInt(const char* pcszName, int& nValue) const;
	BOOL GetAttributeDouble(const char* pcszName, double& dblValue) const;

	// Sets an attribute's value. If the attribute already
	// exists, its value is replaced, otherwise a new attribute is created.
	void SetAttribute(const char* pcszName, const CWslString& sValue);
	void SetAttribute(const char* pcszName, int nValue);

   // Removes the given attribute
   void RemoveAttribute(CWslXMLAttribute* pAttrib);
   void RemoveAttribute(const char* pcszName);

	// Number of attributes
	int GetAttributesCount() const { return m_aAttributes.GetSize(); }

   // Returns an object representing the given attribute. NULL if none.
	CWslXMLAttribute* GetAttribute(int nIdx) const { ASSERT(nIdx>=0 && nIdx < GetAttributesCount()); return m_aAttributes[nIdx]; }
   CWslXMLAttribute* GetAttribute(const char* pcszName) const;

   //---------------------------------------------------------------------------
	// Child elements
	//---------------------------------------------------------------------------

	// Returns iteratively all child elements that match
	// the given name. If the name is NULL all elements are accepted.
	// Returns -1 if there is no more elements.
	int GetNextElementPos(int nPos, const char* pcszName = NULL) const;
	inline int GetFirstElementPos(const char* pcszName = NULL) const { return GetNextElementPos(-1, pcszName); }

	// Returns an element at the givem pos
	CWslXMLElement* GetElement(int nPos) { return dynamic_cast<CWslXMLElement*>(m_aChildern[nPos]); }

	// Returns the first child element of the given name 
	CWslXMLElement* GetElement(const char* pcszName) const;
  
   //---------------------------------------------------------------------------
	// From CWslXMLNode
	//---------------------------------------------------------------------------
	
	_public virtual CWslString GetName() const { return m_sName; }
	_public virtual void SetName(const CWslString& s) { m_sName = s; m_sName.MakeLower(); }
	_public virtual void SetCharData(const CWslString& s) { ASSERT(false); }
	_public virtual void OnTraverse(CWslXMLContext& context) throw (CWslException*);

	// Returns all char data in this element and it's subelements
	_public virtual CWslString GetCharData() const;
   _public virtual CWslString GetImmediateCharData() const;
};





