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

/*******************************************************************************
*
*  CWslFullTextArtList
*
*******************************************************************************/

CWslFullTextArticleList::CWslFullTextArticleList()
{
}

/******************************************************************************/

CWslFullTextArticleList::~CWslFullTextArticleList()
{
}

/******************************************************************************/

void CWslFullTextArticleList::AddNewArticle(const char *pcszArtName, const char *pcszArtUrl, const char *pcszReportedSecName, int nArtId, int nSecId)
{
   ART *pArt = new ART;
	pArt->m_nArtId = nArtId;
	pArt->m_nSecId = nSecId;
	pArt->m_pszArtName = (char*)malloc(strlen(pcszArtName)+1);
   pArt->m_pszArtUrl = (char*)malloc(strlen(pcszArtUrl)+1);
   pArt->m_pszReportedSecName = (char*)malloc(strlen(pcszReportedSecName)+1);
	strcpy(pArt->m_pszArtName, pcszArtName);
   strcpy(pArt->m_pszArtUrl, pcszArtUrl);
   strcpy(pArt->m_pszReportedSecName, pcszReportedSecName);
	m_pArrArticles += pArt;
}

/******************************************************************************/

void CWslFullTextArticleList::AddNewSection(const char *pcszSecName, int nSecId)
{
	SEC *pSec = new SEC;
	pSec->m_nSecId = nSecId;
	pSec->m_pszSecName = (char*)malloc(strlen(pcszSecName)+1);
	strcpy(pSec->m_pszSecName, pcszSecName);
	m_pArrSections += pSec;
}

/******************************************************************************/

int CWslFullTextArticleList::Save(const char *pcszFileName)
{
	int nIdx;
	char header[1024];

   // Write header
	memset(header, 0, 1024);
	strcpy(header, "SpyTech ALI format v0.91, CWslFullTextArtList in WSL 5.0, (c) 1996-2002 SpyTech");
	FILE *pf = fopen(pcszFileName, "wb");
	if (!pf) return 0;
	fwrite(header, 1024, 1, pf);

   // Write sections
	memset(header, 0, 1024);
	strcpy(header, "sections");
	fwrite(header, 16, 1, pf);

	int nNumSections = m_pArrSections.GetSize();
	fwrite(&nNumSections, sizeof(int), 1, pf);
	ITERATE(m_pArrSections, nIdx) {
		fwrite(&m_pArrSections[nIdx]->m_nSecId, sizeof(int), 1, pf);
		int nLength = strlen(m_pArrSections[nIdx]->m_pszSecName);
		fwrite(&nLength, sizeof(int), 1, pf);
		fwrite(m_pArrSections[nIdx]->m_pszSecName, nLength, 1, pf);
	}

   // Write articles
	memset(header, 0, 1024);
	strcpy(header, "articles");
	fwrite(header, 16, 1, pf);

	int nNumArticles = m_pArrArticles.GetSize();
	fwrite(&nNumArticles, sizeof(int), 1, pf);
	ITERATE(m_pArrArticles, nIdx) {
		fwrite(&m_pArrArticles[nIdx]->m_nArtId, sizeof(int), 1, pf);
		fwrite(&m_pArrArticles[nIdx]->m_nSecId, sizeof(int), 1, pf);
		int nLength = strlen(m_pArrArticles[nIdx]->m_pszArtName);
		fwrite(&nLength, sizeof(int), 1, pf);
		fwrite(m_pArrArticles[nIdx]->m_pszArtName, nLength, 1, pf);
		nLength = strlen(m_pArrArticles[nIdx]->m_pszArtUrl);
		fwrite(&nLength, sizeof(int), 1, pf);
		fwrite(m_pArrArticles[nIdx]->m_pszArtUrl, nLength, 1, pf);
      nLength = strlen(m_pArrArticles[nIdx]->m_pszReportedSecName);
      fwrite(&nLength, sizeof(int), 1, pf);
      fwrite(m_pArrArticles[nIdx]->m_pszReportedSecName, nLength, 1, pf);
	}

   // Close the file
	fclose(pf);

	return 1;
}

/******************************************************************************/

int CWslFullTextArticleList::Load(const char *pcszFileName)
{
   char header[1024];

   FILE *pf = fopen(pcszFileName, "rb");
   if (!pf) return 0;
   fread(header, 1024, 1, pf);

   // Read sections
   fread(header, 16, 1, pf);

   int nNumSections;
   fread(&nNumSections, sizeof(int), 1, pf);
   for (int a=0; a<nNumSections; a++) {
      SEC *pSec = new SEC;
      fread(&pSec->m_nSecId, sizeof(int), 1, pf);
      int nLength;
      fread(&nLength, sizeof(int), 1, pf);
      fread(header, nLength, 1, pf);
      header[nLength] = 0;
      pSec->m_pszSecName = (char*)malloc(strlen(header) + 1);
      strcpy(pSec->m_pszSecName, header);

      m_pArrSections += pSec;
   }

   fread(header, 16, 1, pf);

   char temp[1024];

   int nNumArticles;
   fread(&nNumArticles, sizeof(int), 1, pf);
   for (a=0; a<nNumArticles; a++) {
      ART *pArt = new ART;
      fread(&pArt->m_nArtId, sizeof(int), 1, pf);
      fread(&pArt->m_nSecId, sizeof(int), 1, pf);
      int nLength;

      fread(&nLength, sizeof(int), 1, pf);
      fread(temp, nLength, 1, pf);
      temp[nLength] = 0;
      pArt->m_pszArtName = (char*)malloc(strlen(temp) + 1);
      strcpy(pArt->m_pszArtName, temp);

      fread(&nLength, sizeof(int), 1, pf);
      fread(temp, nLength, 1, pf);
      temp[nLength] = 0;
      pArt->m_pszArtUrl = (char*)malloc(strlen(temp) + 1);
      strcpy(pArt->m_pszArtUrl, temp);

      fread(&nLength, sizeof(int), 1, pf);
      fread(temp, nLength, 1, pf);
      temp[nLength] = 0;
      pArt->m_pszReportedSecName = (char*)malloc(strlen(temp) + 1);
      strcpy(pArt->m_pszReportedSecName, temp);

      m_pArrArticles += pArt;
   }

   fclose(pf);
   return 1;
}

/******************************************************************************/

int __BHFindArticlesCallback(void *pObject, void *pArtList, void *pUserData)
{
	CWslFullTextArticleList::ART *pArt1 = (CWslFullTextArticleList::ART*) pObject;
	CWslFullTextArticleList::ART **pArt2 = (CWslFullTextArticleList::ART**) pArtList;

   if ((pArt1)->m_nArtId > (*pArt2)->m_nArtId) return 1;
   else if ((pArt1)->m_nArtId == (*pArt2)->m_nArtId) return 0;
   return -1;
}

/******************************************************************************/

BOOL __SortArticlesCallback(void *pItem1, void *pItem2, void *pUserData)
{
	CWslFullTextArticleList::ART **pArt1 = (CWslFullTextArticleList::ART**) pItem1;
	CWslFullTextArticleList::ART **pArt2 = (CWslFullTextArticleList::ART**) pItem2;

	if ((*pArt1)->m_nArtId < (*pArt2)->m_nArtId) {
		CWslFullTextArticleList::ART swp;
		memcpy(&swp, *pArt2, sizeof(CWslFullTextArticleList::ART));
		memcpy(*pArt2, *pArt1, sizeof(CWslFullTextArticleList::ART));
		memcpy(*pArt1, &swp, sizeof(CWslFullTextArticleList::ART));
		return TRUE;
	}

	return FALSE;
}

/******************************************************************************/

int __BHFindSectionsCallback(void *pObject, void *pArtList, void *pUserData)
{
	CWslFullTextArticleList::SEC *pSec1 = (CWslFullTextArticleList::SEC*) pObject;
	CWslFullTextArticleList::SEC **pSec2 = (CWslFullTextArticleList::SEC**) pArtList;

   if ((pSec1)->m_nSecId > (*pSec2)->m_nSecId) return 1;
   else if ((pSec1)->m_nSecId == (*pSec2)->m_nSecId) return 0;
   return -1;
}

/******************************************************************************/

BOOL __SortSectionsCallback(void *pItem1, void *pItem2, void *pUserData)
{
	CWslFullTextArticleList::SEC **pSec1 = (CWslFullTextArticleList::SEC**) pItem1;
	CWslFullTextArticleList::SEC **pSec2 = (CWslFullTextArticleList::SEC**) pItem2;

	if ((*pSec1)->m_nSecId > (*pSec2)->m_nSecId) {
		CWslFullTextArticleList::ART swp;
		memcpy(&swp, *pSec2, sizeof(CWslFullTextArticleList::ART));
		memcpy(*pSec2, *pSec1, sizeof(CWslFullTextArticleList::ART));
		memcpy(*pSec1, &swp, sizeof(CWslFullTextArticleList::ART));
		return TRUE;
	}

	return FALSE;
}

/******************************************************************************/

void CWslFullTextArticleList::SortRecords()
{
	m_pArrArticles.Sort(&__SortArticlesCallback, NULL);
	m_pArrSections.Sort(&__SortSectionsCallback, NULL);
}

/******************************************************************************/

int CWslFullTextArticleList::FindSectionById(int nSecId, char *pszSecNameOut)
{
   SEC s;
   s.m_nSecId = nSecId;
   int nResult = m_pArrSections.FindBinHalving(&__BHFindSectionsCallback, &s);
   if (nResult == -1) return 0;

   strcpy(pszSecNameOut, m_pArrSections[nResult]->m_pszSecName);
   return 1;
}

/******************************************************************************/

int CWslFullTextArticleList::FindArticleById(int nArtId,
                                             char *pszArtNameOut, 
                                             char *pszSecNameOut, 
                                             char *pszReptdSecNameOut,
                                             char *pszUrlOut)
{
   ART a;
   a.m_nArtId = nArtId;
   int nResult = m_pArrArticles.FindBinHalving(&__BHFindArticlesCallback, &a);
   if (nResult == -1) return 0;
   strcpy(pszArtNameOut, m_pArrArticles[nResult]->m_pszArtName);

   if (pszReptdSecNameOut) strcpy(pszReptdSecNameOut, m_pArrArticles[nResult]->m_pszReportedSecName);

   if (pszUrlOut) strcpy(pszUrlOut, m_pArrArticles[nResult]->m_pszArtUrl);

   SEC s;
   s.m_nSecId = m_pArrArticles[nResult]->m_nSecId;
   nResult = m_pArrSections.FindBinHalving(&__BHFindSectionsCallback, &s);

	 if (pszSecNameOut) 
	 {
		if (nResult == -1) strcpy(pszSecNameOut, "-");
		else strcpy(pszSecNameOut, m_pArrSections[nResult]->m_pszSecName);
	 }

   return 1;
}

/*********************************************************************** eof **/
