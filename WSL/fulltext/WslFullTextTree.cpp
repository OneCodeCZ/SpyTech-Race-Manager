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

#include "stdafx.h"
#include "../generic/wslArray.h"
#include "WslFullText.h"

#include "malloc.h"
#include <crtdbg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#include "windows.h"

/*******************************************************************************
*
*  CWslFullTextConstructedNode
*
*******************************************************************************/

CWslFullTextConstructedNode::CWslFullTextConstructedNode()
{
   ASSERT (m_apChildren.GetSize() == 0);
}

/******************************************************************************/

unsigned ToLowerWin1250(char c)
{
   switch (c) {
   case 'Á': return 'á';
   case 'Ä': return 'ä';
   case 'È': return 'è';
   case 'Ï': return 'ï';
   case 'É': return 'é';
   case 'Ì': return 'ì';
   case 'Í': return 'í';
   case 'Å': return 'å';
   case '¼': return '¾';
   case 'Ò': return 'ò';
   case 'Ó': return 'ó';
   case 'Ö': return 'ö';
   case 'Ô': return 'ô';
   case 'À': return 'à';
   case 'Ø': return 'ø';
   case 'Š': return 'š';
   case '': return '';
   case 'Ú': return 'ú';
   case 'Ù': return 'ù';
   case 'Ü': return 'ü';
   case 'Ý': return 'ý';
   case 'Ž': return 'ž';
   }
   return tolower(c);
}

/******************************************************************************/

// PARAMETERS:
//   pcszWord .. the word (or the rest of it) to be stored
//   art_id .. Article where the word was found
//   art_pos .. Article position
//   art_field .. 
void CWslFullTextConstructedNode::AddRecord(const unsigned short *pcszWord,
                                            int art_id, int art_structure,
                                            int art_field, int art_pos, int nLevel)
{
   // is this the last letter of the word?
   if (pcszWord[0] == 0) {
      if (nLevel >= 0) {
         CWslFullTextConstructedNode::REF ref;
         ref.m_nArticleId = art_id;
         ref.m_nPosition = art_pos;
         ref.m_nField = art_field;
         ref.m_nArticleStructure = art_structure;
         m_ArrRefs += ref;
      }
   }
   // find the branch and pass it further
   else
   {
      // modify here to get better handling of diacritics
//      unsigned char c = tolower(pcszWord[0]);
      // unsigned short int c = ToLowerWin1250(pcszWord[0]);
      unsigned short int c = pcszWord[0];
      int nIdx;

      CHILD *pChild = NULL;

      // Go through the existing children.
      ITERATE (m_apChildren, nIdx)
      {
         pChild = m_apChildren[nIdx];
         // If smaller, do nothing.

         // If found what wanted, do nothing else and leave the pChild set.
         if (pChild->c == c) break;

         // If we have enountered a bigger one, it means that we have to append ours
         // just in front of it.
         if (pChild->c > c)
         {
            pChild = new CHILD;
            pChild->c = c;
            pChild->pNode = new CWslFullTextConstructedNode;
            // pChild->pNode->AddRecord(pcszWord+1, art_id, art_structure, art_field, art_pos, nLevel+1);
            m_apChildren.InsertAt(nIdx, pChild);
            break;
         }
      }

      if (nIdx == m_apChildren.GetSize()) {
         pChild = new CHILD;
         pChild->c = c;
         pChild->pNode = new CWslFullTextConstructedNode;
         m_apChildren += pChild;
      }

      ASSERT(pChild);
      
      pChild->pNode->AddRecord(pcszWord+1, art_id, art_structure, art_field, art_pos, nLevel+1);
   }
}

/******************************************************************************/

CWslFullTextConstructedNode::~CWslFullTextConstructedNode()
{
//   for (int a=0; a<256; a++) if (m_pChildren[a]) delete m_pChildren[a];
   int nIdx;
   ITERATE (m_apChildren, nIdx) delete m_apChildren[nIdx];
}

/******************************************************************************/

void CWslFullTextConstructedNode::Display(int nLevel)
{
   int a;
   printf("(");
   ITERATE (m_apChildren, a)
   {
      printf("%c", m_apChildren[a]->c);
      m_apChildren[a]->pNode->Display(nLevel+1);
   }
   printf(")");
}

/******************************************************************************/

DWORD bopen  = 0xffffffff;
DWORD bclose = 0xfffffffe;

// NODE ::= '(' num_refs ref1 ref2 ... refn char NODE char NODE ... ')'
void CWslFullTextConstructedNode::Save(FILE *pf)
{
   int a, nIdx;
   fwrite("(\0", 2, 1, pf);

   int nNum = m_ArrRefs.GetSize();
   fwrite(&nNum, 1, 4, pf);
   ITERATE (m_ArrRefs, nIdx) {
      fwrite(&m_ArrRefs[nIdx], 1, sizeof(CWslFullTextConstructedNode::REF), pf);
   }

   ITERATE (m_apChildren, nIdx) {
      unsigned short c = m_apChildren[nIdx]->c;
      ASSERT (c != '(');
      ASSERT (c != ')');
      fwrite(&c, 2, 1, pf);
      m_apChildren[nIdx]->pNode->Save(pf);
   }
   fwrite(")\0", 2, 1, pf);
}

/******************************************************************************/

int CWslFullTextConstructedNode::Save(const char *pcszFileName)
{
   char header[1024];
   memset(header, 0, 1024);
   strcpy(header, "SpyTech FTI format 1.3, CWslFullTextConstructedNode in WSL 5.0, (c) 1996-2002 SpyTech");

   FILE *pf = fopen(pcszFileName, "wb");
   if (!pf) return 0;
   fwrite(header, 1024, 1, pf);
   Save(pf);
   fclose(pf);

   return 1;
}

/******************************************************************************/

int CWslFullTextConstructedNode::GetNumRefs()
{
  return m_ArrRefs.GetSize();
}

/*******************************************************************************
*
*  CWslFullTextLoadedNode
*
*******************************************************************************/

CWslFullTextLoadedNode::CWslFullTextLoadedNode()
{
   m_pParent = NULL;
}

/******************************************************************************/

WCHAR CWslFullTextLoadedNode::GetLetter()
{
   if (!m_pParent) return 0;
   int nIdx;
   ITERATE (m_pParent->m_ArrPointers, nIdx) {
      if (m_pParent->m_ArrPointers[nIdx].pNode == this)
         return m_pParent->m_ArrPointers[nIdx].c;
   }
   return 0;
}

/******************************************************************************/

void CWslFullTextLoadedNode::_Dump(FILE *pf) 
{
   WCHAR wtemp[1024];
   char temp[1024];
   char line[16384];
   if (m_ArrRefs.GetSize())
   {
      GetWord(wtemp);
      _To8Bit(wtemp, temp);
      sprintf(line, "%d,\"%s\"", m_ArrRefs.GetSize(), temp);
      int a;
      ITERATE (m_ArrRefs, a)
      {
         char add[1024];
         sprintf(add, ",\"[%d %d %d %d]\"", m_ArrRefs[a].m_nArticleId, m_ArrRefs[a].m_nArticleStructure, m_ArrRefs[a].m_nField, m_ArrRefs[a].m_nPosition);
         strcat(line, add);
      }
      strcat(line, "\n");
      fputs(line, pf);
      printf("%s", line);
   }
   int nIdx;
   ITERATE (m_ArrPointers, nIdx)
   {
      m_ArrPointers[nIdx].pNode->_Dump(pf);
   }
}

/******************************************************************************/

void CWslFullTextLoadedNode::Dump(const char *pcszFileName)
{
   FILE *pf = fopen(pcszFileName, "wt");
   if (pf) {
      _Dump(pf);
      fclose(pf);
   }
}

/******************************************************************************/

WCHAR* CWslFullTextLoadedNode::GetWord(WCHAR *dest)
{
   if (m_ArrRefs.GetSize() == 0) {
      dest[0] = 0;
      return dest;
   }
   CWslFullTextLoadedNode *pNode = this;
   int nPos = 0;
   while (pNode) {
      dest[nPos++] = pNode->GetLetter();
      pNode = pNode->m_pParent;
   }
   dest[nPos]=0;
   _wcsrev(dest);
   return dest;
}

/******************************************************************************/

void CWslFullTextLoadedNode::Load(CWslFullText *pFullText, FILE *pf)
{
   // Read the left bracket
   unsigned short c; 
   fread(&c, 2, 1, pf);

   // Read the word references
   int nNum;
   fread(&nNum, 1, 4, pf);
   for (int a=0; a<nNum; a++)
   {
      CWslFullTextConstructedNode::REF ref;
      fread(&ref, 1, sizeof(CWslFullTextConstructedNode::REF), pf);
      m_ArrRefs += ref;
      pFullText->m_apWordOrder.Append(&m_ArrRefs.GetAt(m_ArrRefs.GetSize()-1));
   }

   // Read all the subnodes, if any.
   fread(&c, 2, 1, pf);
   while (c != ')')
   {
      CWslFullTextLoadedNode *pNewNode = new CWslFullTextLoadedNode;
      pNewNode->m_pParent = this;
      pNewNode->Load(pFullText, pf);

      NODE_PTR ptr;
      ptr.c = c;
      ptr.pNode = pNewNode;
      m_ArrPointers.Append(ptr);

      fread(&c, 2, 1, pf);
   }
}

/******************************************************************************/

int CWslFullTextLoadedNode::Load(CWslFullText *pFullText, const char *pcszFileName)
{
   char header[1024];

   FILE *pf = fopen(pcszFileName, "rb");
   if (!pf) return 0;
   fread(header, 1024, 1, pf);
   Load(pFullText, pf);
   fclose(pf);
   return 1;
}

/******************************************************************************/

CWslFullTextLoadedNode::~CWslFullTextLoadedNode()
{
   int nIdx;
   ITERATE (m_ArrPointers, nIdx) {
      delete m_ArrPointers[nIdx].pNode;
   }
}

/******************************************************************************/

void CWslFullTextLoadedNode::FindNode(const unsigned short *pcszWord, 
                           CWslArray<int> *pArrStructures, // allowed structures
                           CWslArray<int> *pArrFields, // allowed fields
                           CWslArray<CWslFullTextConstructedNode::REF*> *pArrRefs, 
                           CWslArray<CWslFullTextLoadedNode*> *pArrRefsNodes,
                           CWslArray<CWslFullTextLoadedNode*> *pArrNodes)
{
   int nIdx;
   if (pcszWord[0] == 0) {
      if (pArrRefs || pArrRefsNodes) {
         ITERATE (m_ArrRefs, nIdx) {
            if (pArrRefs) (*pArrRefs).Append(&m_ArrRefs[nIdx]);
            if (pArrRefsNodes) (*pArrRefsNodes).Append(this);
         }
      }
      if (pArrNodes) (*pArrNodes).Append(this);
   }
   else if (pcszWord[0] == '*')
   {
      if (m_ArrRefs.GetSize()) {
         if (pArrRefs || pArrRefsNodes) {
            ITERATE (m_ArrRefs, nIdx) {
            if (pArrRefs) (*pArrRefs).Append(&m_ArrRefs[nIdx]);
            if (pArrRefsNodes) (*pArrRefsNodes).Append(this);
            }
         }
         if (pArrNodes) (*pArrNodes).Append(this);
      } 
      ITERATE (m_ArrPointers, nIdx) {
         m_ArrPointers[nIdx].pNode->FindNode(L"*", pArrStructures, pArrFields, pArrRefs, pArrRefsNodes, pArrNodes);
      }
   }
   else if (pcszWord[0] == '?')
   {
      ITERATE (m_ArrPointers, nIdx) {
         m_ArrPointers[nIdx].pNode->FindNode(pcszWord + 1, pArrStructures, pArrFields, pArrRefs, pArrRefsNodes, pArrNodes);
      }
   }
   else if (pcszWord[0] == '.')
   {
      FindNode(pcszWord + 1, pArrStructures, pArrFields, pArrRefs, pArrRefsNodes, pArrNodes);
      ITERATE (m_ArrPointers, nIdx) {
         m_ArrPointers[nIdx].pNode->FindNode(pcszWord + 1, pArrStructures, pArrFields, pArrRefs, pArrRefsNodes, pArrNodes);
      }
   }
   else {
      ITERATE (m_ArrPointers, nIdx) {
         if (ToLowerWin1250(m_ArrPointers[nIdx].c) == ToLowerWin1250(pcszWord[0])) {
            m_ArrPointers[nIdx].pNode->FindNode(pcszWord + 1, pArrStructures, pArrFields, pArrRefs, pArrRefsNodes, pArrNodes);
         }
      }
   }
}

/*******************************************************************************
*
*  UTILS
*
*******************************************************************************/

unsigned short *WslFullTextConvertString(const unsigned short *src)
{
   int len = wcslen(src);
   unsigned short *res = (unsigned short*)malloc(len+1);

   for (int a=0; a<len; a++) {
      res[a+1] = 0;
      unsigned int c = src[a] > 0 ? src[a] : (src[a] + 256);
      if (c < 32) { res[a] = 32; continue; }
      if (c >= '!' && c <= '&') { res[a] = 32; continue; }
      if (c >= '(' && c <= ',') { res[a] = 32; continue; }
      if (c >= '.' && c <= '/') { res[a] = 32; continue; }
      if (c >= ':' && c <= '?') { res[a] = 32; continue; }
      if (c >= '[' && c <= '^') { res[a] = 32; continue; }
      if (c == '`') { res[a] = 32; continue; }
      if (c >= '{' && c <= '}') { res[a] = 32; continue; }
      if (c == 130) { res[a] = 32; continue; }
      if (c >= 132 && c <= 137) { res[a] = 32; continue; }
      if (c >= 145 && c <= 151) { res[a] = 32; continue; }
      res[a] = src[a];
   }
   return res;
}

/******************************************************************************/

const unsigned short *WslFullTextFindNextWord(const unsigned short *src, const unsigned short **word_end)
{
   int nStatus = 0;
   int nPos = 0;

   int nWordBegin = -1;
   int nWordEnd = -1;

   while (nStatus != -1)
   {
      unsigned short c = src[nPos];
      switch (nStatus)
      {
      case 0:
         if (c == 0) nStatus = -1;
         else if (c > 32) nPos++;
         else nStatus = 1;
         break;
      case 1:
         if (c == 0) nStatus = -1;
         else if (c <= 32) nPos++;
         else {
            nWordBegin = nPos;
            nStatus = 2;
         }
         break;
      case 2:
         if (c <= 32) nStatus = -1;
         else nPos++;
         break;
      }
   }

   if (nWordBegin == -1) return NULL;
   if (word_end) *word_end = src + nPos;
   return src + nWordBegin;
}

/******************************************************************************/

const unsigned short *WslFullTextFindFirstWord(const unsigned short *src, const unsigned short **word_end)
{
   int nStatus = 1;
   int nPos = 0;

   int nWordBegin = -1;
   int nWordEnd = -1;

   while (nStatus != -1)
   {
      unsigned short c = src[nPos];
      switch (nStatus)
      {
      case 1:
         if (c == 0) nStatus = -1;
         else if (c <= 32) nPos++;
         else {
            nWordBegin = nPos;
            nStatus = 2;
         }
         break;
      case 2:
         if (c <= 32) nStatus = -1;
         else nPos++;
         break;
      }
   }

   if (nWordBegin == -1) return NULL;
   if (word_end) *word_end = src + nPos;
   return src + nWordBegin;
}

/******************************************************************************/

void WslFullTextIndexBuffer(CWslFullTextConstructedNode *pNode, int art_id,
                            int art_structure, int art_field, const unsigned short *pcszInput)
{
   const unsigned short *pSpaceAfter, *pWord;
   unsigned short *pConverted = WslFullTextConvertString(pcszInput);
   int nWordPos = 0;

#ifdef FULLTEXT_DEBUG
   int nArticle = 0; // debug
#endif

   pWord = WslFullTextFindFirstWord(pConverted, &pSpaceAfter);
   while (pWord)
   {
      int nWordLen = pSpaceAfter - pWord;
      nWordPos++;
      unsigned short sSingleWord[1024];
      nWordLen = min(nWordLen, 1023);
      memcpy(sSingleWord, pWord, nWordLen);
      sSingleWord[nWordLen] = 0;

#ifdef FULLTEXT_DEBUG
      if ((nWordPos % 200) == 0) nArticle++;
      pNode->AddRecord(sSingleWord, nArticle, art_structure, art_field, nWordPos);
#else
      pNode->AddRecord(sSingleWord, art_id, art_structure, art_field, nWordPos);
#endif

      pWord = WslFullTextFindNextWord(pWord, &pSpaceAfter);
   }

   free(pConverted);
}

/******************************************************************************/

char* WslFullTextLoadBuffer(const char *pcszFileName, int *pLength)
{
   int fh = _open(pcszFileName, _O_RDONLY | _O_BINARY);
   if (fh == -1) return 0;
   int nSize = _filelength(fh);
   char *pBuffer = (char*)malloc(nSize+1);
   pBuffer[nSize] = 0;
   _read(fh, pBuffer, nSize);
   _close(fh);
   if (pLength) *pLength = nSize;
   return pBuffer;
}

/******************************************************************************/

unsigned short* WslFullTextExtractWord(const unsigned short *pStart, const unsigned short *pSpaceAfter)
{
   int len = pSpaceAfter - pStart;
   unsigned short *word = (unsigned short*)malloc((len+1)*sizeof(unsigned short));
   memcpy(word, pStart, len*sizeof(unsigned short));
   word[len] = 0;
   return word;
}

/*********************************************************************** eof **/
