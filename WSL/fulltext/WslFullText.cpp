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
#include "stdio.h"
#include <crtdbg.h>

#include "../generic/wslstring.h"
// #include "../ui/wslui.h"

/*******************************************************************************
*
*  CWslFullText
*
*******************************************************************************/

CWslFullText::CWslFullText()
{
   m_pNode = new CWslFullTextLoadedNode;
}

/******************************************************************************/

CWslFullText::~CWslFullText()
{
   Free();
}

/******************************************************************************/

void CWslFullText::Free()
{
   if (m_pNode) delete (m_pNode);
   m_pNode = NULL;
}

/******************************************************************************/

BOOL SortTwoRefs(void *pItem1, void *pItem2, void *pUserData)
{
   CWslFullTextConstructedNode::REF *pNode1 = (CWslFullTextConstructedNode::REF*)pItem1;
   CWslFullTextConstructedNode::REF *pNode2 = (CWslFullTextConstructedNode::REF*)pItem2;

   if (pNode1->m_nPosition > pNode2->m_nPosition)
   {
      CWslFullTextConstructedNode::REF temp;
      memcpy(&temp, pNode1, sizeof(CWslFullTextConstructedNode::REF));
      memcpy(pNode1, pNode2, sizeof(CWslFullTextConstructedNode::REF));
      memcpy(pNode2, &temp, sizeof(CWslFullTextConstructedNode::REF));
      return TRUE;
   }
   return FALSE;
}

int CWslFullText::LoadTree(const char *pcszFileName)
{
   int nResult = m_pNode->Load(this, pcszFileName);
   printf("%d refs\n", m_apWordOrder.GetSize());
   // m_apWordOrder.Sort(&SortTwoRefs, NULL);
   return nResult;
}

/******************************************************************************/

void CWslFullText::WordQuery(const WCHAR *pcszWord,
                             CWslArray<int> *pArrStructures, CWslArray<int> *pArrFields,
                            CWslArray<CWslFullText::QREF> *refs,
                            CWslArray<CWslFullTextLoadedNode*> *nodes)
{
   CWslArray<CWslFullTextConstructedNode::REF*> arrRefs;
   CWslArray<CWslFullTextLoadedNode*> arrRefsNodes;
   m_pNode->FindNode(pcszWord, pArrStructures, pArrFields, &arrRefs, &arrRefsNodes, nodes);

   int nIdx;
   ITERATE(arrRefs, nIdx)
   {
      // Decide, whether a good structure...
      BOOL bEnabledField = TRUE;
      BOOL bEnabledStructure = TRUE;

      if (pArrFields) {
         bEnabledField = FALSE;
         int nIdx2;
         ITERATE ((*pArrFields), nIdx2) {
            if ((*pArrFields)[nIdx2] == arrRefs[nIdx]->m_nField) {
               bEnabledField = TRUE;
               break;
            }
         }
      }

      if (pArrStructures) {
         bEnabledStructure = FALSE;
         int nIdx2;
         ITERATE ((*pArrStructures), nIdx2) {
            if ((*pArrStructures)[nIdx2] == arrRefs[nIdx]->m_nArticleStructure) {
               bEnabledStructure = TRUE;
               break;
            }
         }
      }

      if (!bEnabledStructure) continue;
      if (!bEnabledField) continue;

      // If passed, store.
      CWslFullText::QREF qr;
      qr.m_nArticleId = arrRefs[nIdx]->m_nArticleId;
      qr.m_nField = arrRefs[nIdx]->m_nField;
      qr.m_nPosition = arrRefs[nIdx]->m_nPosition;
      qr.m_nArticleStructure = arrRefs[nIdx]->m_nArticleStructure;
      qr.m_pNode = arrRefsNodes[nIdx];
      refs->Append(qr);
   }
}

/*******************************************************************************
*
*  CWslFullTextQuery
*
*******************************************************************************/

CWslFullTextQuery::CWslFullTextQuery(CWslFullText *pFullText)
{
   m_pFullText = pFullText;
}

/******************************************************************************/

CWslFullTextQuery::~CWslFullTextQuery()
{
}

/******************************************************************************/

BOOL SortRefs_Callback(void *p1, void *p2, void *pUserData)
{
   CWslFullText::QREF *r1 = (CWslFullText::QREF*) p1;
   CWslFullText::QREF *r2 = (CWslFullText::QREF*) p2;

   BOOL bNeedSwap = false;
   if (r1->m_nArticleId > r2->m_nArticleId) bNeedSwap = true;
   else if (r1->m_nArticleId == r2->m_nArticleId) {
      if (r1->m_nPosition > r2->m_nPosition) bNeedSwap = true;
   }

   if (bNeedSwap)
   {
      CWslFullText::QREF rt = *r1;
      *r1 = *r2;
      *r2 = rt;
   }

   return 0; // no swaps so far.
};

void CWslFullTextQuery::SortRefs()
{
   refs.Sort(&SortRefs_Callback, NULL);
}

/******************************************************************************/

void CWslFullTextQuery::Reset()
{
   refs.SetSize(0);
   nodes.SetSize(0);
}

/******************************************************************************/

int CWslFullTextQuery::WordQuery(const WCHAR *pcszWord, CWslArray<int> *pArrStructures,
                                 CWslArray<int> *pArrFields, BOOL bSortRefs)
{
   m_pFullText->WordQuery(pcszWord, pArrStructures, pArrFields, &refs, &nodes);
   if (bSortRefs && refs.GetSize() > 1) SortRefs();
   return refs.GetSize();
}

/******************************************************************************/

int CWslFullTextQuery::SimpleFullTextQuery(const WCHAR *pcszQueryString, CWslArray<int> *pArrStructures, CWslArray<int> *pArrFields)
{
   CWslArray<WCHAR*> pArrWords;

   const WCHAR *pSpaceAfter;
   const WCHAR *pStart;
   pStart = WslFullTextFindFirstWord(pcszQueryString, &pSpaceAfter);
   while (pStart) {
      WCHAR *word = WslFullTextExtractWord(pStart, pSpaceAfter);
      pArrWords += word;
      pStart = WslFullTextFindNextWord(pStart, &pSpaceAfter);
   }

   if (pArrWords.GetSize() == 0) {
      return 0;
   }
   else if (pArrWords.GetSize() == 1) {
      int nResult = WordQuery(pArrWords[0], pArrStructures, pArrFields);
      free(pArrWords[0]);
      return nResult;
   }
   else {
      // Get the first operand
      CWslFullTextQuery *p1 = new CWslFullTextQuery(m_pFullText);
      p1->WordQuery(pArrWords[0], pArrStructures, pArrFields);
      int nIdx = 1;

      while (nIdx < pArrWords.GetSize())
      { 
         // Get the next operand
         CWslFullTextQuery *p2 = new CWslFullTextQuery(m_pFullText);
         p2->WordQuery(pArrWords[nIdx], pArrStructures, pArrFields);
       
         CWslFullTextQuery *pResult;
         // The result Query is either some temporary ...
         if (nIdx != pArrWords.GetSize() - 1) pResult = new CWslFullTextQuery(m_pFullText);
         // ... or this, if this is the last round
         else pResult = this;
         pResult->ComputeAnd(p1, p2);

         // Delete the temporary results
         delete p1;
         delete p2;
         p1 = pResult;

         // Advance counter
         nIdx++;
      }
      ITERATE (pArrWords, nIdx) free(pArrWords[nIdx]);
      return refs.GetSize();
   }
   return 0;
}

/******************************************************************************/

int CWslFullTextQuery::ComputeAnd(CWslFullTextQuery *pQuery1, CWslFullTextQuery *pQuery2)
{
   CWslFullText::QREF *r1, *r2;
   bool bSecondIsFirst;

   int last_put1 = -1;
   int last_put2 = -1;
   int i1 = 0;
   int i2 = 0;

   int len1 = pQuery1->refs.GetSize();
   int len2 = pQuery2->refs.GetSize();
   
   while (i1 < len1 && i2 < len2) {
      r1 = &pQuery1->refs[i1];
      r2 = &pQuery2->refs[i2];

      if (r1->m_nArticleId == r2->m_nArticleId) {

         bSecondIsFirst = false;
         if (r1->m_nPosition > r2->m_nPosition) bSecondIsFirst = true;

         if (!bSecondIsFirst && last_put1 != i1) { refs.Append(*r1); last_put1 = i1; }
         if (last_put2 != i2) { refs.Append(*r2); last_put2 = i2; }
         if (bSecondIsFirst && last_put1 != i1) { refs.Append(*r1); last_put1 = i1; }

         if (i1+1 < len1) {
            if (pQuery1->refs[i1+1].m_nArticleId == r2->m_nArticleId) {
               i1++;
               continue;
            }
         }

         if (i2+1 < len2) {
            if (pQuery2->refs[i2+1].m_nArticleId == r1->m_nArticleId) {
               i2++;
               continue;
            }
         }

         i1++; i2++;
      }
      else if (r1->m_nArticleId < r2->m_nArticleId) i1++;
      else i2++;
   }

   SortRefs();
   return refs.GetSize();
}

/******************************************************************************/

int CWslFullTextQuery::ComputeOr(CWslFullTextQuery *pQuery1, CWslFullTextQuery *pQuery2)
{
   int nIdx;
   ITERATE (pQuery1->refs, nIdx) refs.Append(pQuery1->refs[nIdx]);
   ITERATE (pQuery2->refs, nIdx) refs.Append(pQuery2->refs[nIdx]);
   SortRefs();
   return refs.GetSize();
}

/*********************************************************************** eof **/
