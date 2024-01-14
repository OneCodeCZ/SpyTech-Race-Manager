///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple XML Parser
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

/*
TODO
~~~~
- proper <?xml version="1.0"?> handling, see pg. 12
- proper <![..]> handling (CDATA sections)
- Comply to W3C DOM specification
*/

#pragma once

#include "..\generic\WslString.h"
#include "..\generic\WslPtr.h"

class CWslException;
class CWslXMLElement;
class CWslXMLText;

typedef CWslPtr<CWslXMLElement> CWslXMLElementPtr;
typedef CWslPtr<CWslXMLText> CWslXMLTextPtr;

//*****************************************************************************
class WSL_API CWslXMLParserException : public CWslException
{
public:
	int			m_nLine;	  // -1 = not relevant; zero based
	int			m_nCol;    // -1 = not relevant; zero based; when <>-1 => m_nLine <> -1
	CWslString	m_sMsg;
	
public:
	CWslXMLParserException(BOOL bAutoDelete = true);
	virtual CWslString GetErrorMsg();
};

//*****************************************************************************
class IWslXMLNodeFactory
{
public:
	// Creates an instance of CWslXMLElement class using a new operator.
	virtual CWslXMLElement* CreateElementNode(const char* pcszName) = 0;

	// Creates an instance of CWslXMLText class using a new operator.
	virtual CWslXMLText* CreateTextNode(const char* pcszData) = 0;
};

//*****************************************************************************
class WSL_API CWslXMLParser : private IWslXMLNodeFactory
{
public:
	enum EFlags { HTML_MODE=1, SKIP_COMMENTS=2 };

   // Entity encoding contexts
   enum EEncodingContexts
   {
      ATTRIBUTE,
      CDATA
   };

private:
	// This class provides fast growable buffer. This is
	// essential, because the lexical analyzer's speed
	// depends heavily on the ability to fastly add
	// a char to an unlimited buffer
	class WSL_API CFastBuffer : protected CWslArray<char>
	{
	protected:
		char* m_pchPos; 
		char* m_pchEnd;
	public:
		void Empty();
		void operator +=(char ch);
      void operator +=(const CWslString& s);
		inline operator const char*() { return GetData(); }
	};

private:
	DWORD					m_dwFlags;
	const char*			m_pcszXML;
	int					m_nPos;
	int					m_nLine;
	const char*			m_pcszLineStart;
	IWslXMLNodeFactory*	m_pNodeFactory;
   int               m_nCodePage;         // 1250,1251,..., CP_UTF8

	// Lexial analyzer
	CFastBuffer			m_LABuffer;
	CWslXMLElement*	m_pElement;
	BOOL					m_bTokenParsed;
	int					m_nTokenType;
	BOOL					m_bCharRead;
	char					m_chChar;

	// Reference parser
	CFastBuffer			m_RPBuffer;
	CWslString  		m_sReferenceResult;

	// Errors
	CWslXMLParserException* m_pException;

public:
	CWslXMLParser();
	~CWslXMLParser();

   //---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------

	// Sets flags the affect the parsing process
	void SetFlags(DWORD dwFlags) { m_dwFlags = dwFlags; }

	// Parses the given XML document and returns pointer to the root element.
	// Parsing errors are thrown using the CWslXMLParserException exception.
   // nCodePage = 1250, 1251, ..., CP_UTF8
	CWslXMLElement* ParseFile(const char* pcszFileName, int nCodePage) throw (CWslException*);
	CWslXMLElement* ParseString(const char* pcszXML, int nCodePage) throw (CWslException*);

	// Node factory. Set your own node foactory to be able
	// to create instances of different classes for diferent elements.
	// The default factory creates isntances of CWslXMLElement and CWslXMLText
	// classes.
	inline IWslXMLNodeFactory* GetNodeFactory() { return m_pNodeFactory; }
	inline void SetNodeFactory(IWslXMLNodeFactory* pNodeFactory) { ASSERT(pNodeFactory); m_pNodeFactory = pNodeFactory; }

   //---------------------------------------------------------------------------
   // Helpers
   //---------------------------------------------------------------------------

   // Encodes special characters in the given string (e.g. < > etc.) in order
   // not to corrupt the XML file
	static CWslString EncodeSpecialChars(const char* pcsz, 
      EEncodingContexts eContext,
      int nCodePage);

private:
  //---------------------------------------------------------------------------
	// Node factory
	//---------------------------------------------------------------------------
	
	virtual CWslXMLElement* CreateElementNode(const char* pcszName);
	virtual CWslXMLText* CreateTextNode(const char* pcszData);

private:
  //---------------------------------------------------------------------------
	// Syntactic analyzer
	//---------------------------------------------------------------------------

	CWslXMLElement* SA_Document();		
	BOOL SA_Element(CWslXMLElement* pParent);
	BOOL SA_Content(CWslXMLElement* pItem);

  //---------------------------------------------------------------------------
	// Lexical analyzer
	//---------------------------------------------------------------------------

	// Reads a char from the input.
	BOOL GetChar(char& ch);

	// Returns the last char read to the input.
	void ReturnChar() { _ASSERT(!m_bCharRead); m_bCharRead = true; } 

	// Test a char if it is in a given class
	BOOL IsChar(char ch);
	BOOL IsCharDataChar(char ch);
	BOOL IsWhiteSpace(char ch);
	BOOL IsIdChar(char ch);
	BOOL IsAttributeChar(char ch);
	BOOL IsDigit(char ch);
	BOOL IsHexDigit(char ch);

	// Returns the most recently parsed token to the input 
	void ReturnToken() { _ASSERT(!m_bTokenParsed); m_bTokenParsed = true; } 

	// Reads a token from the input
	int GetToken();
	
	// Tag parsing
	int LA_S1();
	int LA_S2();
	int LA_S3();
	int LA_S4();
	int LA_S4a();
	int LA_S5();
	int LA_S6(char chQuotationChar);
	int LA_S7();
	int LA_S8(BOOL bEmptyTag);	//bEmptyTag=false->start tag; =true->empty tag
	int LA_S8a();
	int LA_S9();
	int LA_S10();
	int LA_S11();

	// Content parsing
	int LA_S12();
	int LA_S13();
	int LA_S14();
	int LA_S15();

	// Commnent parsing
	int LA_S16();
	int LA_S17();
	int LA_S18();
	int LA_S19();
	int LA_S20();

	// <![..[..]..]> stuff. 
	// NOTE: This is not according the XML standard, but it works just fine.
	//       It simply skips it and treats it as a comment.
	int LA_S25();
	int LA_S26();

	//---------------------------------------------------------------------------
	// Reference parser
	//---------------------------------------------------------------------------

	// Parses a reference &xxxxx;
	int ParseReference();
	int RP_S2();
	int RP_S3();
	int RP_S4();
	int RP_S5();

  //---------------------------------------------------------------------------
	// Error stuff
	//---------------------------------------------------------------------------

	void FormatErrorUnxpectedCharacter(const char* pcszState, char ch);
};

