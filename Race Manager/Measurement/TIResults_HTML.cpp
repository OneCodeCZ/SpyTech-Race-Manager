/******************
*                 *  TIResults.cpp
*   __|           *  ___________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/29/04, four years later :-)
*       \    '\   *  ___________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  Implementation of ArtEdit, a control for use by software
*     ^^     ^^   *  of the SpyTech company. This file is property of the
*                 *  SpyTech company.
*******************                                                           */

#include "stdafx.h"
#include "main.h"
#include "TIResults.h"
#include "version.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************/

CString CTIResults::WriteHtmlStandardHeader(CTIMeasuredRace *pRace, CString &sDocument)
{
   sDocument += "<table align=center><tr><td align=center>" + pRace->m_strName + "</td></tr>";

	CString sReportName;
	switch (m_nType) {
	case RESULTS_TYPE_GROUP_CFG:
		sReportName.Format(IDS_HTML_LIST_OF_GROUPS, GetRacePartName(m_nRacePart));
		break;
	case RESULTS_TYPE_INITIAL_LIST:
		sReportName.Format(IDS_HTML_LIST_REGD_RACERS);
		break;
	case RESULTS_TYPE_RACE_PART_RESULTS:
		sReportName.Format("%s", pRace->GetRacePartResultSheetTitle(m_nRacePart));
		break;
	case RESULTS_TYPE_GROUP:
		sReportName.Format(IDS_HTML_RESULTS_GROUP, GetRacePartName(m_nRacePart), m_sGroupName);
		break;
	case RESULTS_TYPE_FINAL_RESULTS:
		sReportName.Format(IDS_HTML_FINAL_RESULTS);
		break;
   }

   sDocument += "<tr><td align=center><font size=5 color=blue>" + sReportName + "</font></td></tr>";
   sDocument += "<tr><td height=16></td></tr>";
   sDocument += "</table>";

   sDocument += "<table>";
   sDocument += "<tr><td width=150>" + GetIds(IDS_HTML_VENUE) + ":</td><td>" + pRace->m_strPlace + "</td></tr>";
   sDocument += "<tr><td width=150>" + GetIds(IDS_HTML_ORGANIZER) + ":</td><td>" + pRace->m_strOrganizer + "</td></tr>";
   sDocument += "<tr><td width=150>" + GetIds(IDS_HTML_RACE_DIRECTOR) + ":</td><td>" + pRace->m_strJudge + "</td></tr>";
   sDocument += "<tr><td width=150>" + GetIds(IDS_HTML_TRACK_NAME) + ":</td><td>" + pRace->m_strTrackName + "</td></tr>";
   sDocument += "</table>";

   return sReportName;
}

/******************************************************************************/

void CTIResults::WriteHtmlHeader(CTIMeasuredRace *pRace, CString &sDocument, CString sPageTitle)
{
   CString sCharSet;
   sCharSet.LoadString(IDS_HTML_CHARSET);

   sDocument = 
"<html>\
	<head>\
		<link rel='stylesheet' type='text/css' href='styles.css'>\
		<meta http-equiv='Content-Type' content='text/html; charset=" + sCharSet + "'>\
		<title>" + sPageTitle + "</title>\
   </head>\
   <body>\
";
}

/******************************************************************************/

void CTIResults::WriteHtmlFooter(CString &sDocument)
{
   sDocument += 
"<br>&nbsp;<span style='font-size:8pt;font-face:verdana'>Powered by <a href='http://www.spytech.cz' target=_new>SpyTech Race Manager</a></span></center>\
   </body>\
</html>\
";
}

/******************************************************************************/

void CTIResults::CreateHtmlGroupList(CTIMeasuredRace *pRace, CString &sDocument)
{
	CString s;

   sDocument += "<table width=* style='table-layout:fixed'>";
   sDocument += "<tr><td>&nbsp;</td></tr>";

	int nNumGroups = m_nArrGroupBorders.GetSize();
	ASSERT(nNumGroups);
	if (!nNumGroups) {
      sDocument += "<tr><td>" + GetIds(IDS_HTML_NO_GROUPS) + "</td></tr>";
		return;
	}

	int nNumRows = 1 + ((nNumGroups-1) / 3);
	int nPos = 0;

	for (int nRow = 0; nRow < nNumRows; nRow++)
   {
      sDocument += "<tr>";
		for (int nCol = 0; nCol < 3; nCol++)
      {
			int nGroup = nRow * 3 + nCol;
			if (nGroup < nNumGroups)
         {
            sDocument += "<td width=33%>";
            sDocument += GetIds(IDS_HTML_GROUP) + " ";
            sDocument += GetGroupName(nGroup+1, nNumGroups);
				for (int a=nPos; a<m_nArrGroupBorders[nGroup]; a++) {
               sDocument += "<br>";
					CTIMeasuredEntity *pEntity = pRace->GetIdxEntity(m_nArrOrder[nPos]);
               sDocument += pEntity->GetEntityName(CTIMeasuredEntity::NAME_SURNAME);
					nPos++;
				}
            sDocument += "<br><br>";
				sDocument += "</td>";
			}
		}
      sDocument += "</tr>";
	}
   sDocument += "</table>";

}

/******************************************************************************/

void CTIResults::CreateHtmlInitialListReport(CTIMeasuredRace *pRace, CString &sDocument)
{
   int nIdx;
	CTIMeasuredEntity *pEntity;
   CString temp;

   sDocument += "<table>";
   sDocument += "<tr><td width=150>" + GetIds(IDS_HTML_STARTNO) + "</td><td>" + GetIds(IDS_HTML_RACER) + "</td></tr>";

   sDocument += "<tr><td>&nbsp;</td></tr>";

	ITERATE(m_nArrOrder, nIdx)
   {
		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
		ASSERT(pEntity);
		if (pEntity)
      {
         temp.Format("%d", nIdx+1);
         sDocument += "<tr>";
         sDocument += "<td>" + temp + "</td>";
         sDocument += "<td>" + pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME) + "</td>";
         sDocument += "</tr>";
		}
	}

   sDocument += "</table>";
}

/******************************************************************************/

void CTIResults::CreateHtmlQualificationReportNumLaps(CTIMeasuredRace *pRace, CString &sDocument)
{
	DWORD dw1, dw2, dw3, dw4;
   CString s;
	char temp[1204];
	int nIdx;

	CTIMeasuredEntity *pEntity;

   sDocument += "<table width=* style='table-layout:fixed'>";
   sDocument += "<tr>";
   sDocument += "<td width=10% valign=bottom>" + GetIds(IDS_HTML_RANK) + "</td>";
   sDocument += "<td width=50% valign=bottom>" + GetIds(IDS_HTML_RACER) + "</td>";
   sDocument += "<td width=20% valign=bottom align=right>" + GetIds(IDS_HTML_LAP_COUNT) + "</td>";
   sDocument += "<td width=20% valign=bottom align=right>" + GetIds(IDS_HTML_BEST_TIME) + "</td>";
   sDocument += "</tr>";

   sDocument += "<tr><td>&nbsp;</td></tr>";

	ITERATE(m_nArrOrder, nIdx)
   {
		ASSERT(nIdx>=0);
		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
		ASSERT(pEntity);
		if (pEntity)
      {
			pEntity->QualGetBestTimes(&dw1, &dw2);
			pEntity->QualGetNumLaps(&dw3, &dw4);

         sDocument += "<tr>";
         
			sprintf(temp, "%d.", nIdx+1); s = temp;
         sDocument += "<td>" + s + "</td>";

         sDocument += "<td>" + pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME) + "</td>";

			LapsFormat(dw3, dw4, 0, temp); s = temp;
         sDocument += "<td align=right>" + s + "</td>";

			TimeFormat(dw1, temp, 3); s = temp;
         sDocument += "<td align=right>" + s + "</td>";

         sDocument += "</tr>";
      }
	}

   sDocument += "</table>";
}

/******************************************************************************/

void CTIResults::CreateHtmlQualificationReportBestTime(CTIMeasuredRace *pRace, CString &sDocument)
{
	DWORD dw1, dw2, dw3, dw4;
	char temp[1204];
	int nIdx;

	CTIMeasuredEntity *pEntity;

   sDocument += "<table width=* style='table-layout:fixed'>";
   sDocument += "<tr>";
   sDocument += "<td width=10% valign=bottom>" + GetIds(IDS_HTML_RANK) + "</td>";
   sDocument += "<td width=50% valign=bottom>" + GetIds(IDS_HTML_RACER) + "</td>";
   sDocument += "<td width=20% valign=bottom align=right>" + GetIds(IDS_HTML_BESTLAP) + "</td>";
   sDocument += "<td width=20% valign=bottom align=right>" + GetIds(IDS_HTML_2NDBEST) + "</td>";
   sDocument += "</tr>";

   sDocument += "<tr><td>&nbsp;</td></tr>";

   ITERATE (m_nArrOrder, nIdx)
   {
      sDocument += "<tr>";
		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
		ASSERT(pEntity);
		if (pEntity)
      {
			pEntity->QualGetBestTimes(&dw1, &dw2);
			pEntity->QualGetNumLaps(&dw3, &dw4);

         CString s; s.Format("%d.", nIdx+1);
         sDocument += "<td>" + s + "</td>";
         sDocument += "<td>" + pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME) + "</td>";

         TimeFormat(dw1, temp, 3); s = temp;
         sDocument += "<td align=right>" + s + "</td>";

			TimeFormat(dw2, temp, 3); s = temp;
         sDocument += "<td align=right>" + s + "</td>";
		}
      sDocument += "</tr>";
   }

   sDocument += "</table>";
}

/******************************************************************************/

void CTIResults::CreateHtmlGroupReport(CTIMeasuredRace *pRace, CString &sDocument)
{
	int nIdx;
	int nIdx2;
	char temp[1204];

	CTIMeasuredEntity *pEntity;

   sDocument += "<table width=* style='table-layout:fixed'>";

   sDocument += "<tr>";
   sDocument += "<td width=10% valign=bottom>" + GetIds(IDS_HTML_RANK) + "</td>";
   sDocument += "<td width=20% valign=bottom>" + GetIds(IDS_HTML_RACER) + "</td>";
   sDocument += "<td align=right>" + GetIds(IDS_HTML_LAPS) + "</td>";
   sDocument += "<td colspan=9></td>";
   sDocument += "<td width=10% align=right>" + GetIds(IDS_HTML_PENALIZATION_SHORT) + "</td>";
   sDocument += "<td width=10% align=right>" + GetIds(IDS_HTML_TOTAL) + "</td>";
   sDocument += "</tr>";

   sDocument += "<tr><td>&nbsp;</td></tr>";

	ITERATE(m_nArrOrder, nIdx)
   {
		CString s;

		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
		CEventData *pEvent = pEntity->m_pArrEvents[GetEventIdx(m_nRacePart)];
		ASSERT(pEntity);
		DWORD dwNumLaps = pEvent->GetTotalLaps();
		DWORD dwNumHdts = pEvent->GetTotalHdts();
		DWORD dwBestLap = pEvent->GetBestLap();
		int nPenalty = pEvent->m_nPenalization;

      sDocument += "<tr>";

		s.Format("%d.", nIdx+1);
		sDocument += "<td valign=top>" + s + "</td>";

		s.Format("%s", pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
		sDocument += "<td valign=top>" + s + "</td>";

		ITERATE(pEvent->m_pArrRides, nIdx2)
      {
			CRideData *pRide = pEvent->m_pArrRides[nIdx2];
			CString sValue;
			CString sLane;
			if (pRide->m_nTrackAssigned == -1)
         {
				sValue = "–";
				sLane = "P";
			}
			else
         {
				sValue.Format("<b>%d</b>", pRide->m_dwNumLaps);
				sLane.Format("%d", pRide->m_nTrackAssigned + 1);
			}

         sDocument += "<td valign=top align=right>";
         sDocument += sValue;
         sDocument += "<br>";
         sDocument += sLane;
         sDocument += "</td>";
		}

      for (int a=nIdx2; a<10; a++) sDocument += "<td></td>";

		s.Format("%d", nPenalty);
      sDocument += "<td valign=top align=right>" + s + "</td>";

		LapsFormat(dwNumLaps, dwNumHdts, pRace->m_nDivisionType, temp);
      s = temp;
      sDocument += "<td valign=top align=right>" + s + "</td>";
      sDocument += "</tr>";
	}
   sDocument += "</table>";
}

/******************************************************************************/

void CTIResults::CreateHtmlRacePartReport(CTIMeasuredRace *pRace, CString &sDocument)
{
	char temp[1204];
	int nIdx;
	CString s;

	CTIMeasuredEntity *pEntity;

   sDocument += "<table width=* style='table-layout:fixed'>";
   sDocument += "<tr>";
   sDocument += "<td width=10% valign=bottom>" + GetIds(IDS_HTML_RANK) + "</td>";
   sDocument += "<td width=50% valign=bottom>" + GetIds(IDS_HTML_RACER) + "</td>";
   sDocument += "<td width=20% align=right>" + GetIds(IDS_HTML_TOTAL_LAPS) + "</td>";
   sDocument += "<td width=20% align=right>" + GetIds(IDS_HTML_BESTLAP) + "</td>";
   sDocument += "</tr>";

   sDocument += "<tr><td>&nbsp;</td></tr>";

   ITERATE(m_nArrOrder, nIdx)
   {
		ASSERT(nIdx>=0);
		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
		ASSERT(pEntity);
		if (pEntity)
      {
			CEventData *pEvent = pEntity->m_pArrEvents[GetEventIdx(m_nRacePart)];
			DWORD dwNumLaps = pEvent->GetTotalLaps();
			DWORD dwNumHdts = pEvent->GetTotalHdts();
			DWORD dwBestLap = pEvent->GetBestLap();
			DWORD dwPenalty = 0;

         sDocument += "<tr>";

			s.Format("%d.", nIdx+1);
         sDocument += "<td>" + s + "</td>";

			s.Format("%s", pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
         sDocument += "<td>" + s + "</td>";

			LapsFormat(dwNumLaps, dwNumHdts, pRace->m_nDivisionType, temp);
         s = temp;
         sDocument += "<td align=right>" + s + "</td>";

			TimeFormat(dwBestLap, temp, 3);
         s = temp;
         sDocument += "<td align=right>" + s + "</td>";

         sDocument += "</tr>";
		}
   }

   sDocument += "</table>";
}

/******************************************************************************/

void CTIResults::CreateHtmlFinalResultList(CTIMeasuredRace *pRace, CString &sDocument)
{
	DWORD dw1, dw2;
	char temp[1204];

	CTIMeasuredEntity *pEntity;
	int nIdx;

	BOOL bIsQual = pRace->GetRS()->m_bQual;
	BOOL bIsPoints = pRace->GetRS()->m_bEnablePoints;
   int nNumCols = bIsPoints ? 8 : 7;

   sDocument += "<table width=* style='table-layout:fixed'>";
   sDocument += "<tr>";
   sDocument += "<td width=10% valign=bottom>" + GetIds(IDS_HTML_RANK) + "</td>";
   sDocument += "<td width=30% valign=bottom>" + GetIds(IDS_HTML_RACER) + "</td>";
   sDocument += "<td align=right>" + CString(bIsQual ? GetIds(IDS_HTML_QUALIFICATION_SHORT) : " ") + "</td>";
   sDocument += "<td align=right>" + GetIds(IDS_HTML_HEATS) + "</td>";
   sDocument += "<td align=right>" + GetIds(IDS_HTML_QUARTER) + "</td>";
   sDocument += "<td align=right>" + GetIds(IDS_HTML_SEMIFINAL_SHORT) + "</td>";
   sDocument += "<td align=right>" + GetIds(IDS_HTML_FINAL) + "</td>";
   if (bIsPoints) sDocument += "<td align=right>" + GetIds(IDS_HTML_POINTS_SHORT) + "</td>";
   sDocument += "</tr>";

   sDocument += "<tr><td>&nbsp;</td></tr>";

	ITERATE(m_nArrOrder, nIdx)
   {
		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
		ASSERT(pEntity);

		sDocument += "<tr>";

		CString s;

		s.Format("%d.", nIdx+1);
      sDocument += "<td>" + s + "</td>";

		s.Format("%s", pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
      sDocument += "<td>" + s + "</td>";

		if (bIsQual)
      {
			pEntity->QualGetBestTimes(&dw1, &dw2);
			TimeFormat(dw1, temp, 3);

         s = temp;
         sDocument += "<td align=right>" + s + "</td>";
		}
      else sDocument += "<td></td>";

		for (int a=0; a<4; a++)
      {
			CEventData *pEvent = pEntity->m_pArrEvents[a];
			if (pEvent->m_pArrRides.GetSize()) {
				dw1 = pEvent->GetTotalLaps();
				dw2 = pEvent->GetTotalHdts();
				LapsFormat(dw1, dw2, pRace->m_nDivisionType, temp);
            s = temp;
            sDocument += "<td align=right>" + s + "</td>";
			}
			else {
            sDocument += "<td align=right>–</td>";
			}
		}

		if (bIsPoints)
      {
			int nPtsUntil = pRace->GetRS()->m_aPoints.GetSize();
			if (nIdx < nPtsUntil) {
				s.Format("%d", pRace->GetRS()->m_aPoints[nIdx]);
			}
			else s.Format("%d", 0);
			sDocument += "<td align=right>" + s + "</td>";
		}

 		sDocument += "</tr>";
   }
   sDocument += "</table>";
}

/*********************************************************************** eof **/
