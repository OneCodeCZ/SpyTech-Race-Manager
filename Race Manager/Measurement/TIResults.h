/******************
*                 *  TIResults.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  18-AUG-2000, 05/29/2004
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#pragma once

/*******************************************************************************
*
*  CTIResults
*
*******************************************************************************/

#define RESULTS_TYPE_INITIAL_LIST       1
#define RESULTS_TYPE_RACE_PART_RESULTS  2
#define RESULTS_TYPE_GROUP_CFG          3
#define RESULTS_TYPE_GROUP              4
#define RESULTS_TYPE_FINAL_RESULTS      5

class CTIResults : public CWsfTreeItem
{
private:
	int m_nType;
	int m_nRacePart;   // (Radek's codework)
	CString m_sGroupName; // only if RESULTS_TYPE_GROUP
	int m_nViewID;				// View ID this item is using

public:
	CWslArray<int> m_nArrGroupBorders;
	CWslArray<int> m_nArrOrder;

public:
	CTIResults();
	CTIResults(int nRacePart);
	virtual ~CTIResults();

	// Gather the data from m_pArrSortedEntities
	void GatherData();
	// Gather the data from m_aPartEntities
	void GatherDataPartEntities();
	// Gather data about current groups configuration
	void GatherDataGroups(CTIMeasuredRace *pRace);
	void GatherDataGroupRide(CTIMeasuredRace *pRace, int nGroupIdx=-1);
	void GatherDataFinalResults(CTIMeasuredRace *pRace);

	// void WriteStandardHeader(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateQualificationReportBestTime(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateQualificationReportNumLaps(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateRacePartReport(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateInitialListReport(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateGroupList(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateGroupReport(CArticle *pDoc, CTIMeasuredRace *pRace);
	// void CreateFinalResultList(CArticle *pDoc, CTIMeasuredRace *pRace);

   // --------------------------------------------------------------------------
   // New HTML data output routines

   void WriteHtmlHeader(CTIMeasuredRace *pRace, CString &sDocument, CString sPageTitle);
	CString WriteHtmlStandardHeader(CTIMeasuredRace *pRace, CString &sDocument);
   void WriteHtmlFooter(CString &sDocument);

   void CreateHtmlGroupList(CTIMeasuredRace *pRace, CString &sDocument);
   void CreateHtmlInitialListReport(CTIMeasuredRace *pRace, CString &sDocument);
   void CreateHtmlQualificationReportNumLaps(CTIMeasuredRace *pRace, CString &sDocument);
   void CreateHtmlQualificationReportBestTime(CTIMeasuredRace *pRace, CString &sDocument);
   void CreateHtmlGroupReport(CTIMeasuredRace *pRace, CString &sDocument);
	void CreateHtmlRacePartReport(CTIMeasuredRace *pRace, CString &sDocument);
	void CreateHtmlFinalResultList(CTIMeasuredRace *pRace, CString &sDocument);

	void OnFilePrint();
	void OnUpdateFilePrint(CCmdUI *pCmdUI);

   void ShowHtmlPage(CString sDocument, CString sReportName);

   //---------------------------------------------------------------------------
	// From WSF
	//---------------------------------------------------------------------------

	virtual CString GetTitle() const;
	virtual void Serialize(CArchive &ar);
	virtual int GetDefaultCommand() { return ID_WSF_OPEN; };


public:
	//{{AFX_MSG(CTIResults)
	afx_msg void OnUpdateWsfOpen(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG

	DECLARE_SERIAL(CTIResults)
	DECLARE_MESSAGE_MAP()
};
