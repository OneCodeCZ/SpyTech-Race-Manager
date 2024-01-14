/******************
*                 *  WSL Full Text Search
*   __|           *  ___________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  Created Jan 16 2002
*       \    '\   *  ___________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of World Spy Library source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                           */

/*******************************************************************************
*
*  Usage
*
*  To retrieve from a tree:
*    - create a CWslFullText object
*    - load a tree from disk with CWslFullText::Load
*    - create a CWslFullTextQuery object
*    - ask a word query
*    - create multiple CWslFullTextQueries as needed and operate with them
*      using ComputeAnd, ComputeOr and ComputeNear stuff
*
*  To create a tree:
*    - Have prepared a chunk of text to be indexed
*    - Create CWslFullTextConstructedNode object
*    - Call WslFullTextIndexBuffer (here the article ID might be specified)
*    - Save the CWslFullTextConstructedNode object using CWslFullTextConstructedNode::Save
*    - This tree might be then used by the retrieval process described above
*
*  Bugs
*    - Arrays are sorted using bubble sort -> sloww
*
*******************************************************************************/

#pragma once

/*******************************************************************************
*
*  CWslFullTextConstructedNode
*
*******************************************************************************/

class CWslFullTextConstructedNode
{
public:
   struct REF
   {
      int   m_nArticleId;
      int   m_nArticleStructure;
      int   m_nField;
      int   m_nPosition;
   };

   struct CHILD
   {
      unsigned short c;
      CWslFullTextConstructedNode *pNode;
   };

   CWslArray<REF> m_ArrRefs;
   CWslArray<CHILD*> m_apChildren;
//   CWslFullTextConstructedNode *m_pChildren[256];

public:
   CWslFullTextConstructedNode();
   ~CWslFullTextConstructedNode();

   void AddRecord(const unsigned short *pcszWord, int art_id, int art_structure,
      int art_field, int art_pos, int nLevel=0);
   void Display(int nLevel);

   int GetNumRefs();

private:
   void Save(FILE *pf);
public:
   int Save(const char *pcszFileName);
};

/*******************************************************************************
*
*  CWslFullTextLoadedNode
*
*******************************************************************************/

class CWslFullText;
class CWslFullTextLoadedNode
{
private:
   struct NODE_PTR
   {
      unsigned short         c;
      CWslFullTextLoadedNode *pNode;
   };

public:
   CWslArray<CWslFullTextConstructedNode::REF> m_ArrRefs;
   CWslArray<NODE_PTR> m_ArrPointers;
   CWslFullTextLoadedNode *m_pParent;

public:

   CWslFullTextLoadedNode();
   ~CWslFullTextLoadedNode();

   void Load(CWslFullText *pFullText, FILE *pf);
   int Load(CWslFullText *pFullText, const char *pcszFileName);

   void FindNode(
      // INPUT
      const unsigned short *pcszWord, 
      CWslArray<int> *pArrStructures, // allowed structures
      CWslArray<int> *pArrFields, // allowed fields
      // OUTPUT
      CWslArray<CWslFullTextConstructedNode::REF*> *pArrRefs,  // list of all occurrences
      CWslArray<CWslFullTextLoadedNode*> *pArrRefsNodes, // list of all nodes found
      CWslArray<CWslFullTextLoadedNode*> *pArrNodes);    // list of all unique nodes found

   void _Dump(FILE *pf);
   void Dump(const char *pcszFileName);

   WCHAR GetLetter();
   WCHAR* GetWord(WCHAR *dest);
   void _To8Bit(WCHAR *src, char *dest) {
      for (int a=0; src[a]; a++) dest[a] = src[a] < 256 ? src[a] : '?';
      dest[a] = 0;
   }
};

/*******************************************************************************
*
*  CWslFullText
*
*******************************************************************************/

class CWslFullText
{
public:
   struct QREF
   {
      int m_nArticleId;
      int m_nArticleStructure;
      int m_nField;
      int m_nPosition;
      CWslFullTextLoadedNode *m_pNode;
   };

public:
   CWslFullTextLoadedNode *m_pNode;
   void Free();

   CWslArray<CWslFullTextConstructedNode::REF*> m_apWordOrder;

public:
   CWslFullText();
   ~CWslFullText();

   int LoadTree(const char *pcszFileName);
   int IndexBuffer(int nArtId, const unsigned short *pcszBuffer);

   void WordQuery(const unsigned short *pcszWord,
      CWslArray<int> *pArrStructures, CWslArray<int> *pArrFields,
      CWslArray<QREF> *pArrRefs,
      CWslArray<CWslFullTextLoadedNode*> *pArrNodes);
};

/*******************************************************************************
*
*  CWslFullTextQuery
*
*******************************************************************************/

class CWslFullTextQuery
{
public:
   CWslArray<CWslFullText::QREF> refs; 
   // relevant ONLY when only word query is exectd
   CWslArray<CWslFullTextLoadedNode*> nodes;
   CWslFullText *m_pFullText;

public:
   CWslFullTextQuery(CWslFullText *pFullText);
   ~CWslFullTextQuery();

   // refs will contain all the references of given word in the whole tree
   // nodes will contain all the nodes of the word in the whole tree
   int WordQuery(const unsigned short *pcszWord, CWslArray<int> *pArrStructures,
      CWslArray<int> *pArrFields, BOOL bSortRefs = TRUE);

   // One word leads to an ordinary WordQuery
   // Multiple words are automatically ANDed amongst
   int SimpleFullTextQuery(const unsigned short *pcszQueryString,
      CWslArray<int> *pArrStructures, CWslArray<int> *pArrFields);

   // refs are sorted by increasing art_id and word position
   void SortRefs();

   // TODO: The inputs to the following functions must be SORTED by the
   //       increasing article ID. Otherwise they won't work properly.

   // list of all the word references of the q1 and q2 that have common
   // article id.
   int ComputeAnd(CWslFullTextQuery *pQuery1, CWslFullTextQuery *pQuery2);

   // merges two reference lists
   int ComputeOr(CWslFullTextQuery *pQuery1, CWslFullTextQuery *pQuery2);

   void Reset();

   // computes AND constrained by the distance of the words
   // int ComputeNear(CWslFullTextQuery *pQuery1, CWslFullTextQuery *pQuery2, int nNearness);
};

/*******************************************************************************
*
*  CWslFullTextArticleList
*
*******************************************************************************/

class CWslFullTextArticleList
{
public:
	struct ART {
		char* m_pszArtName;
      char* m_pszArtUrl;
      char* m_pszReportedSecName;
		int m_nArtId;
		int m_nSecId;
	};
	struct SEC {
		char* m_pszSecName;
		int m_nSecId;		
	};

	CWslArray<ART*> m_pArrArticles;
	CWslArray<SEC*> m_pArrSections;

public:
	CWslFullTextArticleList();
	~CWslFullTextArticleList();

   // Add a new record to the list of articles/sections
	void AddNewArticle(const char *pcszArtName, const char *pcszArtUrl,
      const char *pcszReportedSecName, int nArtId, int nSecId);
	void AddNewSection(const char *pcszSecName, int nSecId);

   // Sort both tables. Ssllooww bubble sort used.
	void SortRecords();

   // Load/save the tables from/into ALI file
   // RETURNS: 0 if error.
   int Load(const char *pcszFileName);
   int Save(const char *pcszFileName);

   // RETURNS: 0 if nothing found
   int FindSectionById(int nSecId,
                       char *pszSecNameOut);
   int FindArticleById(int nArtId, 
                       char *pszArtNameOut, 
                       char *pszSecNameOut=NULL, 
                       char *pszReptdSecNameOut=NULL, 
                       char *pszUrlOut=NULL);
};

/*******************************************************************************
*
*  Utilities
*
*******************************************************************************/

// RETURNS:
//   New buffer containing the result of the conversion.
unsigned short *WslFullTextConvertString(const unsigned short *src);

// INPUT PARAMETERS:
//   src .. source string. Only SPACE can be the delimiter. Use a filter
//     to convert all delimiters to SPACE.
// RETURNS:
//   the pointer to the begin of the next word in the given text
//   word_end .. (if given) is set to the first character that does not
//     belong to the word (ie. SPACE or EOF)
const unsigned short *WslFullTextFindFirstWord(const unsigned short *src, const unsigned short **word_end = NULL);
const unsigned short *WslFullTextFindNextWord(const unsigned short *src, const unsigned short **word_end = NULL);

// Pass the valid pointers from the previous functions to this function
// in order to obtain a new buffer containing only the word
unsigned short* WslFullTextExtractWord(const unsigned short *pStart, const unsigned short *pSpaceAfter);

// FUNCTION:
//   Appends all the words in the pcszSrc into the tree index represented by
//   the CWslFullTextConstructedNode
void WslFullTextIndexBuffer(CWslFullTextConstructedNode *pNode, int art_id,
                            int art_structure, int art_field, const unsigned short *pcszSrc);

char* WslFullTextLoadBuffer(const char *pcszFileName, int *pLength = NULL);

/*********************************************************************** eof **/
