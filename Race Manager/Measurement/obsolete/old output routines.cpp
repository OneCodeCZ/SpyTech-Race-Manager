/******************************************************************************/

void CTIResults::WriteStandardHeader(CArticle *pDoc, CTIMeasuredRace *pRace) {

	CString s;
	char temp[256];

//	INS_PAR;

   DSC;
		pDoc->Align(ALIGN_HORIZ_CENTER);
		pDoc->SetFontSize(24);
		s.Format("`(c9)%s", pRace->m_strName);
		WRT (s);
	ASC;

	INS_PAR;
		pDoc->Align(ALIGN_HORIZ_CENTER);
		pDoc->SetFontSize(18);
		switch (m_nType) {
		case RESULTS_TYPE_GROUP_CFG:
			s.Format("`(c9)%s – List of groups", GetRacePartName(m_nRacePart));
			break;
		case RESULTS_TYPE_INITIAL_LIST:
			s.Format("`(c9)List of Registered Racers");
			break;
		case RESULTS_TYPE_RACE_PART_RESULTS:
			s.Format("`(c9)%s", pRace->GetRacePartResultSheetTitle(m_nRacePart));
			break;
		case RESULTS_TYPE_GROUP:
			s.Format("`(c9)%s – Group %s", GetRacePartName(m_nRacePart), m_sGroupName);
			break;
		case RESULTS_TYPE_FINAL_RESULTS:
			s.Format("`(c9)Race Results");
			break;
		}
		pDoc->Write(s);
		pDoc->LF();
		pDoc->LF();
	ASC;

	INS_TAB (1);
		DSC_TAB (0);
			s.Format("`(c0)Place: `(b1)%s`(b0) ", pRace->m_strPlace);
			WRT (s); LFD;
			pRace->m_Date.Get(temp);
			s.Format("Date: `(b1)%s`(b0) ", temp);
			WRT (s); LFD;
			s.Format("Organizer: `(b1)%s`(b0) ", pRace->m_strOrganizer);
			WRT (s); LFD;
			s.Format("Race director: `(b1)%s`(b0) ", pRace->m_strJudge);
			WRT (s); LFD;
			s.Format("Track name: `(b1)%s`(b0) ", pRace->m_strTrackName);
			WRT (s); LFD;
		ASC;
	ASC;
}

/******************************************************************************/

void CTIResults::CreateGroupList(CArticle *pDoc, CTIMeasuredRace *pRace)
{
	CString s;

	WriteStandardHeader(pDoc, pRace);

	INS_TAB(3);

		SET_TAB_COL (0, 33);
		SET_TAB_COL (1, 33);
		SET_TAB_COL (2, 33);

		WRT_TAB (0, " ");
		ROW;

		int nNumGroups = m_nArrGroupBorders.GetSize();
		ASSERT(nNumGroups);
		if (!nNumGroups) {
			ASC;
			return;
		}

		int nNumRows = 1 + ((nNumGroups-1) / 3);
		int nPos = 0;

		for (int nRow = 0; nRow < nNumRows; nRow++) {
			ROW;
			for (int nCol = 0; nCol < 3; nCol++) {
				int nGroup = nRow * 3 + nCol;
				if (nGroup < nNumGroups) {
					DSC_TAB(nCol);
					WRT("Group ");
					WRT(GetGroupName(nGroup+1, nNumGroups));
					LFD;
					for (int a=nPos; a<m_nArrGroupBorders[nGroup]; a++) {
						LFD;
						CTIMeasuredEntity *pEntity = pRace->GetIdxEntity(m_nArrOrder[nPos]);
						WRT(pEntity->GetEntityName(CTIMeasuredEntity::NAME_SURNAME));
						nPos++;
					}
					LFD;
					LFD;
					ASC;
				}
			}
		}

	ASC;

}
/******************************************************************************/

void CTIResults::CreateFinalResultList(CArticle *pDoc, CTIMeasuredRace *pRace)
{
	DWORD dw1, dw2;
	char temp[1204];

	CTIMeasuredEntity *pEntity;
	int nIdx;
	WriteStandardHeader(pDoc, pRace);

	BOOL bIsQual = pRace->GetRS()->m_bQual;
	BOOL bIsPoints = pRace->GetRS()->m_bEnablePoints;
   int nNumCols = bIsPoints ? 8 : 7;

//	MessageBeep(0xffffffff);

	INS_TAB (nNumCols);
		SET_TAB_COL (0, 10);
		SET_TAB_COL (1, 30);
		SET_TAB_COL (2, 10);
		SET_TAB_COL (3, 10);
		SET_TAB_COL (4, 10);
		SET_TAB_COL (5, 10);
		SET_TAB_COL (6, 10);
      if (bIsPoints) { SET_TAB_COL(7, 10); }

		WRT_TAB (0, " ");
		ROW;

		WRT_TAB (0, "Pos.");
		WRT_TAB (1, "Name");
		WRT_TAB (2, bIsQual ? "Qual." : " ");
		WRT_TAB (3, "Heats");
		WRT_TAB (4, "Quarter");
		WRT_TAB (5, "Semi.");
		WRT_TAB (6, "Final");
      if (bIsPoints) { WRT_TAB(7, "Pts"); }

		ROW;
		WRT_TAB (0, " ");
	
		ITERATE(m_nArrOrder, nIdx) {
			pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
//			CEventData *pEvent = pEntity->m_pArrEvents[GetEventIdx(m_nRacePart)];
			ASSERT(pEntity);
//			DWORD dwNumLaps = pEvent->GetTotalLaps();
//			DWORD dwNumHdts = pEvent->GetTotalHdts();
//			DWORD dwBestLap = pEvent->GetBestLap();
//			DWORD dwPenalty = 0;

			ROW;

			CString s;

			s.Format("%d.", nIdx+1);
			WRT_TAB (0, s);

			s.Format("%s", pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
			WRT_TAB (1, s);

			if (bIsQual) {
				pEntity->QualGetBestTimes(&dw1, &dw2);
				TimeFormat(dw1, temp, 3);
				WRT_TAB (2, temp);
			}

			for (int a=0; a<4; a++) {
				CEventData *pEvent = pEntity->m_pArrEvents[a];
				if (pEvent->m_pArrRides.GetSize()) {
					dw1 = pEvent->GetTotalLaps();
					dw2 = pEvent->GetTotalHdts();
					LapsFormat(dw1, dw2, pRace->m_nDivisionType, temp);
					WRT_TAB (3+a, temp);
				}
				else {
					WRT_TAB (3+a, "–");
				}
			}


//			LapsFormat(dwNumLaps, dwNumHdts, pRace->m_nDivisionType, temp);
//			WRT_TAB (2, temp);

//			TimeFormat(dwBestLap, temp, 3);
//			WRT_TAB (3, temp);

			if (bIsPoints) {
				int nPtsUntil = pRace->GetRS()->m_aPoints.GetSize();
				if (nIdx < nPtsUntil) {
					s.Format("%d", pRace->GetRS()->m_aPoints[nIdx]);
				}
				else s.Format("%d", 0);
				WRT_TAB(7, s);
			}
		}

	ASC;
}

/******************************************************************************/

void CTIResults::CreateGroupReport(CArticle *pDoc, CTIMeasuredRace *pRace)
{

	int nIdx;
	int nIdx2;
	char temp[1204];

	CTIMeasuredEntity *pEntity;
	WriteStandardHeader(pDoc, pRace);

	INS_TAB (14);
		SET_TAB_COL (0, 10);
		SET_TAB_COL (1, 40);
		for (int a=0; a<10; a++) {
			SET_TAB_COL (a+2, 10);
		}
		SET_TAB_COL (12, 15);
		SET_TAB_COL (13, 15);

		WRT_TAB (0, " ");
		ROW;

		WRT_TAB (0, "Pos.");
		WRT_TAB (1, "Name");
		WRT_TAB (2, "Laps");
		WRT_TAB (12, "Pnl.");
		WRT_TAB (13, "Total");

		ROW; WRT_TAB (0, " ");
	
		ITERATE(m_nArrOrder, nIdx) {
			pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
			CEventData *pEvent = pEntity->m_pArrEvents[GetEventIdx(m_nRacePart)];
			ASSERT(pEntity);
			DWORD dwNumLaps = pEvent->GetTotalLaps();
			DWORD dwNumHdts = pEvent->GetTotalHdts();
			DWORD dwBestLap = pEvent->GetBestLap();
			int nPenalty = pEvent->m_nPenalization;

			ROW;

			CString s;

			s.Format("%d.", nIdx+1);
			WRT_TAB (0, s);

			s.Format("%s", pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
			WRT_TAB (1, s);

			ITERATE(pEvent->m_pArrRides, nIdx2) {
				CRideData *pRide = pEvent->m_pArrRides[nIdx2];
				CString sValue;
				CString sLane;
				if (pRide->m_nTrackAssigned == -1) {
					sValue = "–";
					sLane = "P";
				}
				else {
					sValue.Format("%d", pRide->m_dwNumLaps);
					sLane.Format("%d", pRide->m_nTrackAssigned + 1);
				}

				DSC_TAB (nIdx2+2);
					WRT(sValue);
					LFD;
					WRT(sLane);
					LFD;
				ASC;
			}

			s.Format("%d", nPenalty);
			WRT_TAB (12, s);

			LapsFormat(dwNumLaps, dwNumHdts, pRace->m_nDivisionType, temp);
			WRT_TAB (13, temp);

		}

	ASC;
}

/******************************************************************************/

void CTIResults::CreateInitialListReport(CArticle *pDoc, CTIMeasuredRace *pRace)
{
	char temp[1204];

	CTIMeasuredEntity *pEntity;
	int nIdx;
	WriteStandardHeader(pDoc, pRace);

	INS_TAB (3);
		SET_TAB_COL (0, 10);
		SET_TAB_COL (1, 50);
		SET_TAB_COL (2, 40);

		WRT_TAB (0, " ");

		ROW;
		WRT_TAB (0, "Start #");
		WRT_TAB (1, "Racer");

		ROW;
		WRT_TAB (0, " ");
	
		ITERATE(m_nArrOrder, nIdx) {
			pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
			ASSERT(pEntity);
			if (pEntity) {
				ROW;
				sprintf(temp, "%d", nIdx+1);
				WRT_TAB (0, temp);
				WRT_TAB (1, pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
			}
		}

	ASC;
}

/******************************************************************************/

void CTIResults::CreateQualificationReportBestTime(CArticle *pDoc, CTIMeasuredRace *pRace)
{
	DWORD dw1, dw2, dw3, dw4;
	char temp[1204];
	int nIdx;

	CTIMeasuredEntity *pEntity;

//	BOOL bEnable = TRUE;
//	for (int a=0; a<m_nArrOrder.GetSize(); a++) {
//		pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[a]));
//		if (pEntity) {
//			pEntity->QualGetBestTimes(&dw1, &dw2);
//		}		
//		if (dw1 == 0 && bEnable) {
//			int nIdx = m_nArrOrder[a];
//			m_nArrOrder.RemoveAt(a);
//			m_nArrOrder.Append(nIdx);
//			a--;
//		}
//		if (dw1 != 0) bEnable = FALSE;
//	}

	WriteStandardHeader(pDoc, pRace);

	INS_TAB (4);

		SET_TAB_COL (0, 10);
		SET_TAB_COL (1, 50);
		SET_TAB_COL (2, 20);
		SET_TAB_COL (3, 20);

		WRT_TAB (0, " ");

		ROW;
		WRT_TAB (0, "`(c0)Order");
		WRT_TAB (1, "Racer");
		WRT_TAB (2, "Best time");
		WRT_TAB (3, "2nd time");

		ROW;
		WRT_TAB (0, " ");

		ITERATE(m_nArrOrder, nIdx) {
			ASSERT(nIdx>=0);
			pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
			ASSERT(pEntity);
			if (pEntity) {
				pEntity->QualGetBestTimes(&dw1, &dw2);
				pEntity->QualGetNumLaps(&dw3, &dw4);

				ROW;

				sprintf(temp, "%d.", nIdx+1);
				WRT_TAB (0, temp);

				WRT_TAB (1, pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));

				TimeFormat(dw1, temp, 3);
				WRT_TAB (2, temp);

				TimeFormat(dw2, temp, 3);
				WRT_TAB (3, temp);
			}
		}
	pDoc->ContextAscent();
}

/******************************************************************************/

void CTIResults::CreateQualificationReportNumLaps(CArticle *pDoc, CTIMeasuredRace *pRace)
{
	DWORD dw1, dw2, dw3, dw4;
	char temp[1204];
	int nIdx;

	CTIMeasuredEntity *pEntity;

	WriteStandardHeader(pDoc, pRace);

	INS_TAB (4);

		SET_TAB_COL (0, 10);
		SET_TAB_COL (1, 50);
		SET_TAB_COL (2, 20);
		SET_TAB_COL (3, 20);

		WRT_TAB (0, " ");

		ROW;
		WRT_TAB (0, "`(c0)Order");
		WRT_TAB (1, "Racer");
		WRT_TAB (2, "Number of Laps");
		WRT_TAB (3, "Best time");

		ROW;
		WRT_TAB (0, " ");

		ITERATE(m_nArrOrder, nIdx) {
			ASSERT(nIdx>=0);
			pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
			ASSERT(pEntity);
			if (pEntity) {
				pEntity->QualGetBestTimes(&dw1, &dw2);
				pEntity->QualGetNumLaps(&dw3, &dw4);

				ROW;

				sprintf(temp, "%d.", nIdx+1);
				WRT_TAB (0, temp);

				WRT_TAB (1, pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));

				LapsFormat(dw3, dw4, 0, temp);
				WRT_TAB (2, temp);

				TimeFormat(dw1, temp, 3);
				WRT_TAB (3, temp);
			}
		}
	pDoc->ContextAscent();
}

/******************************************************************************/

void CTIResults::CreateRacePartReport(CArticle *pDoc, CTIMeasuredRace *pRace)
{
	char temp[1204];
	int nIdx;
	CString s;

	CTIMeasuredEntity *pEntity;

	WriteStandardHeader(pDoc, pRace);

	INS_TAB (4);

		SET_TAB_COL (0, 10);  // poradi
		SET_TAB_COL (1, 50);  // jmeno
		SET_TAB_COL (2, 20);  // pocet kol
		SET_TAB_COL (3, 20);  // best time

		WRT_TAB (0, " ");
		ROW;

		WRT_TAB (0, "Pos.");
		WRT_TAB (1, "Name");
		WRT_TAB (2, "Total Laps");
		WRT_TAB (3, "Best Time");

		ROW;
		WRT_TAB (0, " ");

		ITERATE(m_nArrOrder, nIdx) {
			ASSERT(nIdx>=0);
			pEntity = DC_ME(pRace->GetIdxEntity(m_nArrOrder[nIdx]));
			ASSERT(pEntity);
			if (pEntity) {

				CEventData *pEvent = pEntity->m_pArrEvents[GetEventIdx(m_nRacePart)];
				DWORD dwNumLaps = pEvent->GetTotalLaps();
				DWORD dwNumHdts = pEvent->GetTotalHdts();
				DWORD dwBestLap = pEvent->GetBestLap();
				DWORD dwPenalty = 0;

				ROW;

				s.Format("%d.", nIdx+1);
				WRT_TAB (0, s);

				s.Format("%s", pEntity->GetEntityName(CTIRaceEntity::NAME_SURNAME));
				WRT_TAB (1, s);

				LapsFormat(dwNumLaps, dwNumHdts, pRace->m_nDivisionType, temp);
				WRT_TAB (2, temp);

				TimeFormat(dwBestLap, temp, 3);
				WRT_TAB (3, temp);
			}
		}
	ASC;
}

