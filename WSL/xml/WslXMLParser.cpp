///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple XML Parser
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 12
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\files\WslFile.h"
#include "WslXMLTreeItem.h"
#include "WslXMLElement.h"
#include "WslXMLText.h"
#include "WslXMLParser.h"

#define TOKEN_EOF				0
#define TOKEN_STAG			1
#define TOKEN_ETAG			2
#define TOKEN_CHARDATA   	3
#define TOKEN_ERROR			4
#define TOKEN_COMMENT		5
#define TOKEN_EMPTY_TAG		6
#define TOKEN_ENTITY			7

/*****************************************************************************
*
* BUFFER
*
*****************************************************************************/

/*****************************************************************************/
void CWslXMLParser::CFastBuffer::Empty() 
{ 
	m_pchPos = GetData();
	m_pchEnd = GetData() + GetSize();
}

/*****************************************************************************/
void CWslXMLParser::CFastBuffer::operator +=(char ch)
{
	if (m_pchPos == m_pchEnd) 
	{
		int nPos = GetData()-m_pchPos;
		GrowBy(4*1024);
		m_pchPos = GetData() + nPos;
		m_pchEnd = GetData() + GetSize();
		TRACE0("Alloc\n");
	}
	*m_pchPos++ = ch;
}

/*****************************************************************************/
void CWslXMLParser::CFastBuffer::operator +=(const CWslString& s)
{
   for (int i=0; i < s.GetLength(); i++) 
   {
      operator +=(s.GetAt(i));
   }
}

/*****************************************************************************
*
* EXCEPTION
*
*****************************************************************************/

/*****************************************************************************/
CWslXMLParserException::CWslXMLParserException(BOOL bAutoDelete /*= true*/)
{
}

/*****************************************************************************/
CWslString CWslXMLParserException::GetErrorMsg()
{
	CWslString s;
	ASSERT(!m_sMsg.IsEmpty());
	
	if (m_nLine == -1)
		s = m_sMsg;
	else if (m_nCol == -1)
		s.Format("Line %d: %s", m_nLine+1, m_sMsg);
	else
		s.Format("Line %d, column %d: %s", m_nLine+1, m_nCol+1, m_sMsg);

	return s;
}

/*****************************************************************************/
void CWslXMLParser::FormatErrorUnxpectedCharacter(const char* pcszState, char ch)
{
	ASSERT(!m_pException);

	m_pException = new CWslXMLParserException();
	m_pException->m_nLine = m_nLine;
	m_pException->m_nCol = m_pcszXML+m_nPos-m_pcszLineStart-1; 

	m_pException->m_sMsg.Format("Unexpected character '%c'! (state: %s)\n"\
		"...%.20s => %.20s...\n\n\n",
		ch, pcszState, 
		m_pcszXML+m_nPos-21, 
		m_pcszXML+m_nPos-1);
}


/*****************************************************************************
*
* PARSER
*
*****************************************************************************/

//*****************************************************************************
CWslXMLParser::CWslXMLParser()
{
	m_dwFlags = HTML_MODE | SKIP_COMMENTS;
	SetNodeFactory(this);

   ASSERT(EncodeSpecialChars("\xC2\xA0", CDATA, CP_UTF8) == "&nbsp;");
}

//*****************************************************************************
CWslXMLParser::~CWslXMLParser()
{
}

//*****************************************************************************
CWslXMLElement* CWslXMLParser::ParseFile(const char* pcszFileName, 
                                         int nCodePage) throw(CWslException*)
{
	ASSERT(pcszFileName);
	CWslPtr<char> pszXML;

	CWslFile f;
	f.OpenReadOnly(pcszFileName);
	int n = f.GetLength();
	pszXML = new char[n+1];
	pszXML[n] = 0;
	f.Read(pszXML, n);
	f.Close();

	return ParseString(pszXML, nCodePage);
}

//*****************************************************************************
CWslXMLElement* CWslXMLParser::ParseString(const char* pcszXML,
                                           int nCodePage) throw(CWslException*)
{
	ASSERT(pcszXML);
	ASSERT(m_pNodeFactory);

   m_pcszXML = (char*) pcszXML;

	// Init

	m_nPos = 0;
	m_nLine = 0;
	m_bTokenParsed = false;
	m_bCharRead = false;
	m_pcszLineStart = m_pcszXML;
	m_pException = NULL;
   m_nCodePage = nCodePage;

	// Parse

	CWslXMLElement* pRoot = SA_Document();
	if (!pRoot) throw m_pException;
	ASSERT(!m_pException);

	// Deinit

	m_LABuffer.Empty();
	m_RPBuffer.Empty();

	return pRoot;
}


/*****************************************************************************
*
* NODE FACTORY
*
*****************************************************************************/

//*****************************************************************************
CWslXMLElement* CWslXMLParser::CreateElementNode(const char* pcszName)
{
	ASSERT(pcszName);
	ASSERT(pcszName[0]);
	return new CWslXMLElement(pcszName);
}

//*****************************************************************************
CWslXMLText* CWslXMLParser::CreateTextNode(const char* pcszData)
{
	ASSERT(pcszData);
	return new CWslXMLText(pcszData);
}


/*****************************************************************************
*
* SYNTACTIC ANALYZER
*
*****************************************************************************/

//*****************************************************************************
CWslXMLElement* CWslXMLParser::SA_Document()
{
	// Document ::= Prolog Element
	
	// Prolog
   // Read all data until the root element

	int n;
   BOOL bQuit = false;

	while (!bQuit)
	{
      n = GetToken();

		switch (n)
		{
      case TOKEN_EMPTY_TAG:
      case TOKEN_STAG:
      	ReturnToken();
         bQuit = true;
         break;

		case TOKEN_COMMENT:
		case TOKEN_CHARDATA:
			break;
	
		default:
			m_pException = new CWslXMLParserException();
			m_pException->m_nLine = m_nLine;
			m_pException->m_nCol = -1;
			m_pException->m_sMsg = "Expecting the root tag.";
			return false;
		}
	}

	// Root element

	CWslXMLElement* pTempRoot = new CWslXMLElement();
	CWslXMLElement* pRoot = NULL;

	if (SA_Element(pTempRoot) && pTempRoot->GetChildrenCount()) 
	{		
		pRoot = (CWslXMLElement*) pTempRoot->GetChild(0);
		pRoot->SetParent(NULL);
	}
	
	delete pTempRoot;
	return pRoot;
}

//*****************************************************************************
BOOL CWslXMLParser::SA_Element(CWslXMLElement* pParent)
{
	// Element:: = EmptyElementTag | STag Content ETag

	CWslXMLElement* p = NULL;

	switch (GetToken())
	{
	case TOKEN_STAG:
		if (m_pElement->HasNoEndTag())
		{
			pParent->AddChild(m_pElement);
			//TRACE1("<%s> with no end tag\n", m_pElement->GetName());
			return true;
		}
		p = m_pElement;
		//TRACE1("<%s>\n", p->GetName());
		break;

	case TOKEN_EMPTY_TAG:
		pParent->AddChild(m_pElement);
		//TRACE1("<%s/>\n", m_pElement->GetName());
		return true;

	case TOKEN_EOF:
		return true;

	case TOKEN_ERROR:
		return false;

	default:
		m_pException = new CWslXMLParserException();
		m_pException->m_nLine = m_nLine;
		m_pException->m_nCol = -1;
		m_pException->m_sMsg = "Unexpected token.";
		return false;
	}

	if (!SA_Content(p))
	{
		delete p;
		return false;
	}

	switch (GetToken())
	{
	case TOKEN_ETAG:
		if (stricmp(p->GetName(), m_LABuffer))
		{
			m_pException = new CWslXMLParserException();
			m_pException->m_nLine = m_nLine;
			m_pException->m_nCol = -1;
			m_pException->m_sMsg.Format("Matching end tag for <%s> not found. Found </%s> instead.", p->GetName(), (const char*) m_LABuffer);
			delete p;
			return false;
		}
		pParent->AddChild(p);
		return true;

	case TOKEN_ERROR:
		return false;
		
	default:
		m_pException = new CWslXMLParserException();
		m_pException->m_nLine = m_nLine;
		m_pException->m_nCol = -1;
		m_pException->m_sMsg.Format("Expecting the end tag </%s>.", p->GetName());
		delete p;
		return false;
	}
}

//*****************************************************************************
BOOL CWslXMLParser::SA_Content(CWslXMLElement* pItem)
{
	// Content ::= CharData? (Element CharData?)*

	while (1)
	{
		switch (GetToken())
		{	
		case TOKEN_CHARDATA:
			pItem->AddChild(m_pNodeFactory->CreateTextNode(m_LABuffer));
			break;

		case TOKEN_EOF:
			return true;

		case TOKEN_STAG:
		case TOKEN_EMPTY_TAG:
			ReturnToken();
			if (!SA_Element(pItem)) return false;
			break;

		case TOKEN_ERROR:
			return false;
		
		default:
			ReturnToken();
			return true;
		}
	}

}


/*****************************************************************************
*
* LEXICAL ANALYZER
*
*****************************************************************************/

//*****************************************************************************
BOOL CWslXMLParser::GetChar(char& ch)
{
	if (m_bCharRead) { ch = m_chChar; m_bCharRead = false; return ch!=0; }

	m_chChar = ch = m_pcszXML[m_nPos];
	if (!ch) return false;
	
	m_nPos++;

	// Convert all 0xD,0xA to 0xA and all standalone 0xD to 0xA
	// Complies to XML standard (chapter 2.11)

	if (ch==0xD)
	{
		if (m_pcszXML[m_nPos] == 0xA) m_nPos++;
		ch = 0xA;
		m_nLine++;
		m_pcszLineStart = &m_pcszXML[m_nPos];
	}

	return true;
}

//*****************************************************************************
BOOL CWslXMLParser::IsChar(char ch)
{
	BYTE b = (BYTE) ch;
	return b==0x9 || b==0xA || b==0xD || b>=' ';
}

//*****************************************************************************
BOOL CWslXMLParser::IsCharDataChar(char ch)
{
   BYTE b = (BYTE) ch;
   return b>32 && b!='&' && b!='<' && b!='>';
}

//*****************************************************************************
BOOL CWslXMLParser::IsWhiteSpace(char ch)
{
	BYTE b = (BYTE) ch;
	return b==0x9 || b==0xA || b==0xD || b==' ';
}

//*****************************************************************************
BOOL CWslXMLParser::IsIdChar(char ch)
{
	BYTE b = (BYTE) ch;
	return (b>='0' && b<='9') || (b>='a' && b<='z') || 
		(b>='A' && b<='Z') || (b == '_') || (b == ':') || (b == '-');
}

//*****************************************************************************
BOOL CWslXMLParser::IsAttributeChar(char ch)
{
	BYTE b = (BYTE) ch;
	return b>=32 && b!='&' && b!='\'' && b!='"';
}

//*****************************************************************************
BOOL CWslXMLParser::IsDigit(char ch)
{
	BYTE b = (BYTE) ch;
	return b>='0' && b<='9';
}

//*****************************************************************************
BOOL CWslXMLParser::IsHexDigit(char ch)
{
	BYTE b = (BYTE) ch;
	return (b>='0' && b<='9') || (b>='A' && b<='F') || (b>='a' && b<='f');
}


//*****************************************************************************
int CWslXMLParser::GetToken()
{
	char ch;
	int n;

	// If a token is already parsed, return it

	if (m_bTokenParsed) { m_bTokenParsed = false; return m_nTokenType; }

	// Get new token

AGAIN:
	m_LABuffer.Empty();
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='&')
	{
		if ((n=ParseReference())!=TOKEN_ENTITY) return n;
		m_LABuffer += m_sReferenceResult;
		n = LA_S13();
	}
	else if (ch=='<')
	{
		n = LA_S1();
	}
	else if (IsCharDataChar(ch))
	{
		m_LABuffer += ch;
		n = LA_S13();
	}
	else if (IsWhiteSpace(ch))
	{
		if (m_dwFlags & HTML_MODE)
		{
			m_LABuffer += ' ';
			n = LA_S15();
		}
		else
		{
			m_LABuffer += ch;
			n = LA_S13();
		}
	}
	else
	{
		FormatErrorUnxpectedCharacter("GetToken", ch);
		n = TOKEN_ERROR;
	}

	m_nTokenType = n;

	// Check for comment. Comments are parsed, but ignored.
	// Complies to the XML standard.

	if (m_nTokenType == TOKEN_COMMENT)
	{
		TRACE1("Comment: %s\n", (m_dwFlags & SKIP_COMMENTS) ? "SKIP_COMMENTS flag" : (const char*) m_LABuffer);
		goto AGAIN;
	}

	return m_nTokenType;
}

//*****************************************************************************
int CWslXMLParser::LA_S1()
{
	char ch;
	int n;
	m_LABuffer.Empty();

TOP:
	GetChar(ch);

	if (ch=='/')
		n = LA_S9();
	else if (ch=='!')
		n = LA_S16();
	else if (ch=='?')
	{
		// Temporary <?xml...?> tag handling
		while (1)
		{
			GetChar(ch);
			if (ch=='?' && GetChar(ch) && ch=='>') { n = TOKEN_COMMENT; break; }
		}
	}
	else if (IsIdChar(ch))
	{ 
		m_LABuffer += ch; 
		n = LA_S2(); 
	}
	else if (IsWhiteSpace(ch))
		goto TOP;
	else
	{
		FormatErrorUnxpectedCharacter("LA_S1", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S2()
{
	char ch;
	int n;

TOP:
	GetChar(ch);

	if (IsIdChar(ch))
		{ m_LABuffer += ch; goto TOP; }
	else if (IsWhiteSpace(ch))
	{
		m_LABuffer += 0;
		m_pElement = m_pNodeFactory->CreateElementNode(m_LABuffer);
		n = LA_S3();
		if (n==TOKEN_ERROR) delete m_pElement;
	}
	else if (ch=='>')
	{
		m_LABuffer += 0;
		m_pElement = m_pNodeFactory->CreateElementNode(m_LABuffer);
		n = LA_S8(false);
		if (n==TOKEN_ERROR) delete m_pElement;
	}
	else if (ch=='/')
	{
		m_LABuffer += 0;
		m_pElement = m_pNodeFactory->CreateElementNode(m_LABuffer);
		n = LA_S8a();
		if (n==TOKEN_ERROR) delete m_pElement;
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S2", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S3()
{
	char ch;
	int n;

	m_LABuffer.Empty();

TOP:
	GetChar(ch);

	if (IsIdChar(ch))
	{ 
		m_LABuffer += ch;
		n = LA_S4();
	}
	else if (IsWhiteSpace(ch))
		goto TOP;
	else if (ch=='>')
		n = LA_S8(false);
	else if (ch=='/')
		n = LA_S8a();
	else
	{
		FormatErrorUnxpectedCharacter("LA_S3", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S4()
{
	char ch;
	int n;

TOP:
	GetChar(ch);

	if (IsIdChar(ch))
	{ 
		m_LABuffer += ch;
		goto TOP;
	}
	else if (IsWhiteSpace(ch))
		n = LA_S4a();
	else if (ch == '=')
	{
		m_LABuffer += 0;
		m_pElement->SetAttribute(m_LABuffer, NULL);
		n = LA_S5();
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S4", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S4a()
{
	char ch;
	int n;

TOP:
	GetChar(ch);

	if (IsWhiteSpace(ch))
		goto TOP;
	else if (ch=='=')
	{
		m_LABuffer += 0;
		m_pElement->SetAttribute(m_LABuffer, NULL);
		n = LA_S5();
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S4a", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S5()
{
	char ch;
	int n;

TOP:
	GetChar(ch);

	if (IsWhiteSpace(ch))
		goto TOP;
	else if (ch=='\'' || ch=='"')
	{
		n = LA_S6(ch);
	}
	// for HTML compatibility allow attbibutes without quotation
	else if ((m_dwFlags & HTML_MODE) && IsAttributeChar(ch))
	{
		ReturnChar();
		n = LA_S6('\0');
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S5", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S6(char chQuotationChar)
{
	char ch;
	int n;
	m_LABuffer.Empty();

TOP:
	GetChar(ch);

	if (ch==chQuotationChar)
	{
		m_LABuffer += 0;
		m_pElement->GetAttribute(m_pElement->GetAttributesCount()-1)->SetValue((const char*)m_LABuffer);
		n = LA_S7();
	}
	else if (chQuotationChar==0 && (ch=='>' || ch=='/' || IsWhiteSpace(ch)))
	{
		ReturnChar();
		m_LABuffer += 0;
		m_pElement->GetAttribute(m_pElement->GetAttributesCount()-1)->SetValue((const char*)m_LABuffer);
		n = LA_S7();
	}
	else if (IsAttributeChar(ch))
	{
		m_LABuffer += ch;
		goto TOP;
	}
	else if (ch=='&')
	{
		if ((n=ParseReference())!=TOKEN_ENTITY) return n;
		m_LABuffer += m_sReferenceResult;
		goto TOP;
	}
	else if (ch==10)
	{
		goto TOP;
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S6", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S7()
{
	char ch;
	int n;

	GetChar(ch);

	if (IsWhiteSpace(ch))
		n = LA_S3();
	else if (ch=='>')
		n = LA_S8(false);
	else if (ch=='/')
		n = LA_S8a();
	else
	{
		FormatErrorUnxpectedCharacter("LA_S7", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S8(BOOL bEmptyTag)
{
	return bEmptyTag ? TOKEN_EMPTY_TAG : TOKEN_STAG;
}

//*****************************************************************************
int CWslXMLParser::LA_S8a()
{
	char ch;
	int n;
	GetChar(ch);

	if (ch == '>')
		n = LA_S8(true);
	else
	{
		FormatErrorUnxpectedCharacter("LA_S8a", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S9()
{
	char ch;
	int n;
	m_LABuffer.Empty();

	GetChar(ch);

	if (IsIdChar(ch))
	{
		m_LABuffer += ch;
		n = LA_S10();
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S9", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S10()
{
	char ch;
	int n;

TOP:
	GetChar(ch);

	if (IsIdChar(ch))
	{
		m_LABuffer += ch;
		goto TOP;
	}
	else if (ch=='>')
	{
		m_LABuffer += 0;
		n = LA_S11();
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S10", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S11()
{
	return TOKEN_ETAG;
}

//*****************************************************************************
int CWslXMLParser::LA_S12()
{
	char ch;
	int n;

TOP:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (IsCharDataChar(ch))
	{
		m_LABuffer += ch;
		n = LA_S13();
	}
	else if (ch=='&')
	{
		if ((n=ParseReference())!=TOKEN_ENTITY) return n;
		m_LABuffer += m_sReferenceResult;
		n = LA_S13();
	}
	else if (IsWhiteSpace(ch))
		goto TOP;
	else if (ch=='<')
	{
		n = LA_S1(); 
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S12", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S13()
{
	char ch;
	int n;

TOP:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (IsCharDataChar(ch))
	{
		m_LABuffer += ch;
		goto TOP;
	}
	else if (ch=='&')
	{
		if ((n=ParseReference())!=TOKEN_ENTITY) return n;
		m_LABuffer += m_sReferenceResult;
		goto TOP;
	}
	else if (IsWhiteSpace(ch))
	{
		if (m_dwFlags & HTML_MODE)
		{
			m_LABuffer += ' ';
			n = LA_S15();
		}
		else
		{
			m_LABuffer += ch;
			goto TOP;
		}
	}
	else if (ch=='<')
	{
		ReturnChar();
		m_LABuffer += 0;
		n = LA_S14(); 
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S13", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S14()
{
	return TOKEN_CHARDATA;
}

//*****************************************************************************
int CWslXMLParser::LA_S15()
{
	char ch;
	int n;

TOP:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (IsCharDataChar(ch))
	{
		m_LABuffer += ch;
		n = LA_S13();
	}
	else if (ch=='&')
	{
		if ((n=ParseReference())!=TOKEN_ENTITY) return n;
		m_LABuffer += m_sReferenceResult;
		n = LA_S13();
	}
	else if (IsWhiteSpace(ch))
		goto TOP;
	else if (ch=='<')
	{
		ReturnChar();
		m_LABuffer += 0;
		n = LA_S14(); 
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S15", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S16()
{
	char ch;
	int n;

	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='-')
		n = LA_S17();
	else if (ch=='[')
	{
		m_LABuffer.Empty();
		n = LA_S25();
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S16", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S17()
{
	char ch;
	int n;

	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='-')
	{
		m_LABuffer.Empty();
		n = LA_S18();
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S17", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S18()
{
	char ch;
	int n;

TOP:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='-')
		n = LA_S19();
	else if (IsChar(ch))
	{
		if (!(m_dwFlags & SKIP_COMMENTS)) m_LABuffer += ch;
		goto TOP;
	} 
	else
	{
		FormatErrorUnxpectedCharacter("LA_S18", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S19()
{
	char ch;
	int n;

	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='-')
		n = LA_S20();
	else if (IsChar(ch))
	{
		m_LABuffer += '-';
		m_LABuffer += ch;
		n = LA_S18();
	} 
	else
	{
		FormatErrorUnxpectedCharacter("LA_S19", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::LA_S20()
{
	char ch;
	int n;

	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch == '>')
	{
		m_LABuffer += 0;
		n = TOKEN_COMMENT;
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S20", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

/*****************************************************************************
*
* <![..[..]..]> STUFF
*
*****************************************************************************/

/*****************************************************************************/
int CWslXMLParser::LA_S25()
{
	char ch;
	int n;

TOP:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='[')
	{
		if (LA_S25()==TOKEN_ERROR) return TOKEN_ERROR;
		goto TOP;
	}
	else if (ch==']')
		n = LA_S26();
	else if (IsChar(ch) || IsWhiteSpace(ch))
	{
		m_LABuffer += ch;
		goto TOP;
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S25", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

/*****************************************************************************/
int CWslXMLParser::LA_S26()
{
	char ch;
	int n;

	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='>')
	{
		m_LABuffer += 0;
		n = TOKEN_COMMENT;
	}
	else
	{
		FormatErrorUnxpectedCharacter("LA_S26", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

/*****************************************************************************
*
* REFERENCE PARSER
*
*****************************************************************************/

//*****************************************************************************
int CWslXMLParser::ParseReference()
{
	int n;
	char ch;
	m_RPBuffer.Empty();
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='#')
		n = RP_S2();
	else if (IsIdChar(ch))
	{
		m_RPBuffer += ch;
		n = RP_S5();
	}
	else
	{
		FormatErrorUnxpectedCharacter("ParseReference", ch);
		n = TOKEN_ERROR;
	}

	m_RPBuffer.Empty();
	return n;
}

//*****************************************************************************
int CWslXMLParser::RP_S2()
{
	char ch;
	int n;
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch=='x' || ch=='X')
		n = RP_S3();
	else if (IsDigit(ch))
	{
		m_RPBuffer += ch;
		n = RP_S4();
	}
	else
	{
		FormatErrorUnxpectedCharacter("RP_S2", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::RP_S3()
{
	char ch;
	int n;
AGAIN:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch==';')
	{
		sscanf(m_RPBuffer, "%h", &n);
		m_sReferenceResult = (char) n;
		n = TOKEN_ENTITY;
	}
	else if (IsHexDigit(ch))
	{
		m_RPBuffer += ch;
		goto AGAIN;
	}
	else
	{
		FormatErrorUnxpectedCharacter("RP_S3", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::RP_S4()
{
	int n;
	char ch;
AGAIN:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch==';')
	{
		m_sReferenceResult = (char) atoi(m_RPBuffer);
		n = TOKEN_ENTITY;
	}
	else if (IsDigit(ch))
	{
		m_RPBuffer += ch;
		goto AGAIN;
	}
	else
	{
		FormatErrorUnxpectedCharacter("RP_S4", ch);
		n = TOKEN_ERROR;
	}

	return n;
}

//*****************************************************************************
int CWslXMLParser::RP_S5()
{
	char ch;
	int n;
AGAIN:
	if (!GetChar(ch)) return TOKEN_EOF;

	if (ch==';')
	{
		n = TOKEN_ENTITY;
		m_RPBuffer += 0;
		const char* pcsz = m_RPBuffer;
		
		if (!stricmp(pcsz, "gt"))
			m_sReferenceResult = ">";
		else if (!stricmp(pcsz, "lt"))
			m_sReferenceResult = "<";
		else if (!stricmp(pcsz, "amp"))
			m_sReferenceResult = "&";
		else if (!stricmp(pcsz, "apos"))
			m_sReferenceResult = "'";
		else if (!stricmp(pcsz, "quot"))
			m_sReferenceResult = "\"";
		// for HTML compatibility
		else if ((m_dwFlags & HTML_MODE) && !stricmp(pcsz, "nbsp"))		
      {
         switch (m_nCodePage)
         {
         case CP_UTF8: m_sReferenceResult = "\xC2\xA0"; break;
         default:      m_sReferenceResult = "\xA0"; break;
         }
      }
		else
		{
			m_pException = new CWslXMLParserException();
			m_pException->m_nLine = m_nLine;
			m_pException->m_nCol = -1;
			m_pException->m_sMsg.Format("Unknown entity '%s'! (state: RP_S5)\n", pcsz);
			n = TOKEN_ERROR;
		}
	}
	else if (IsIdChar(ch))
	{
		m_RPBuffer += ch;
		goto AGAIN;
	}
	else
	{
		FormatErrorUnxpectedCharacter("RP_S5", ch);
		n = TOKEN_ERROR;
	}

	return n;
}


/*****************************************************************************
*
* ENTITY ENCODING
*
*****************************************************************************/

/*****************************************************************************/
// Indexed by ASCII code from <0,69>
// x = tab[ascii_code]
// Each row is 10 items.
static const char* AttributeSpecialCharsEncoding[128-58] = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,"&quot;",0,0,0,"&amp;","&apos;",
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	"&lt;",0,"&gt;",0,0,0,0,0,0,0
};

static const char* CDataSpecialCharsEncoding[128-58] = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,"&amp;",0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	"&lt;",0,"&gt;",0,0,0,0,0,0,0
};

/*****************************************************************************/
CWslString CWslXMLParser::EncodeSpecialChars(const char* pcsz, 
                                             EEncodingContexts eContext,
                                             int nCodePage)
{
	ASSERT(pcsz);

   // Get special chars table according the encoding context
   
   const char* (*pTab)[128-58];

   switch (eContext)
   {
   case ATTRIBUTE: pTab = &AttributeSpecialCharsEncoding; break;
   default:        ASSERT(false); // fall through
   case CDATA:     pTab = &CDataSpecialCharsEncoding; break;
   }

	// Convert special characters to character references
   // A simple automaton.
   // UTF8: 0xC2, 0xA0 = &nbsp;

	BYTE ch;
	CWslString s;
   int nState = 0;

	for (const char* p=pcsz; ch=*p++;)
	{
      switch (nState)
      {
      case 0:
   		if (ch <= 69 && (*pTab)[ch])
         {
			   s += (*pTab)[ch];
         }
         else if (nCodePage != CP_UTF8 && ch == 0xA0)
         {
            s += "&nbsp;";
         }
         else if (nCodePage == CP_UTF8 && ch == 0xC2)
         {
            nState = 1;
         }
         else
         {
            s += ch;
         }
         break;

      case 1:
         if (ch == 0xA0)
         {
            s += "&nbsp;";
         }
         else 
         {
            s += (char) 0xC2;
            s += ch;
         }

         nState = 0;
         break;

      default:
         ASSERT(false);
         break;
      }
	}

	return s;
}
