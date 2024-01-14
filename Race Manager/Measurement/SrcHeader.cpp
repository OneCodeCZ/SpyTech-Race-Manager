/******************
*                 *  SrcHeader.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
// #include "WndRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************/

CSrcHeader::CSrcHeader(CWslWatchSource *pOwner) : CWslWatchSource(pOwner)
{
	SE(C_WHITE, 5, W_ALIGN_CENTER, C_GREY1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 0, 0, 800, 80, this));

	SE(C_WHITE, 3, W_ALIGN_LEFT, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT("", 17, 8, 400, 26, this));
	SE(C_WHITE, 5, W_ALIGN_LEFT, C_GREY1, C_BLUE3, FALSE);
	RW(NEW_TEXT("", 17, 34, 400, 36, this));
	SE(C_WHITE, 5, W_ALIGN_RIGHT, C_GREY1, C_BLUE3, FALSE);
	RW(NEW_TEXT("", 400, 34, 380, 36, this));

//	SE(C_WHITE, 1, W_ALIGN_LEFT, C_BLUE1, C_WHITE, FALSE);
//	RW(NEW_TEXT("Lane",       9+8,  92,  54, 26, this));
}

/******************************************************************************/

CSrcHeader::~CSrcHeader() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/

/*
void CSrcHeader::SetCaptionQualification(const char *pcszRacerName, int nEvalMethod, int nThis, int nTotal) {
	char temp[1024];
	wsprintf(temp, "Kvalifikace – %s – %s %d/%d", 
		pcszRacerName,
		nEvalMethod==RS_EVAL_FIRST_TIME ? "pokus" : "jízda",
		nThis,
		nTotal
	);
	GetText(0)->SetText(temp);
}
*/

void CSrcHeader::SetCaptionQualification(CTIMeasuredRace *pRace, CTIMeasuredEntity *pEntity)
{
	char temp[1024];

	/*
	CString s = "Qualification – ";
	s += pEntity->GetEntityName(CTIRaceEntity::NICKNAME);
	s += " – ";
	*/

	GetText(1)->SetText("Qualification");
	GetText(2)->SetText(pEntity->GetEntityName(CTIRaceEntity::NICKNAME));

	CString s;
	switch(pRace->GetRS()->m_nPrimaryEval) {
	case RS_EVAL_FIRST_TIME:
		s += "Attempt ";
		s += itoa(pEntity->m_nCurrentRide, temp, 10);
		break;
	case RS_EVAL_NUM_LAPS:
		s += "Ride ";
		s += itoa(pEntity->m_nCurrentRide, temp, 10);
		s += "/";
		s += itoa(pRace->GetRS()->m_nNumRides, temp, 10);
		break;
	}

	GetText(3)->SetText(s);
}

/******************************************************************************/

void CSrcHeader::SetCaptions(const char *s1, const char *s2, const char *s3) {
	GetText(1)->SetText(s1);
	GetText(2)->SetText(s2);
	GetText(3)->SetText(s3);
}

/******************************************************************************/

void CSrcHeader::SetCaptionRacePart(const char *pcszPartName, int nGroup, int nTotalGroups, int nSegment, int nTotalSegments)\
{
	char temp[1024];

	GetText(1)->SetText(pcszPartName);
	wsprintf(temp, "Group %s", GetGroupName(nGroup, nTotalGroups));
	GetText(2)->SetText(temp);
	wsprintf(temp, "Segment %d/%d", nSegment, nTotalSegments);
	GetText(3)->SetText(temp);
}

/******************************************************************************/

void CSrcHeader::SetCaptionRacePart(CTIMeasuredRace *pRace, BOOL bDispSegment) {
	char temp[1024];
	if (pRace->m_nRacePart == RACE_PART_QUAL)
   {
      CString sQual; sQual.LoadString(IDS_RACE_CAPTION_QUALIFICATION);
      CString sCurrRes; sCurrRes.LoadString(IDS_RACE_CAPTION_CURRENT_RESULTS);

		GetText(1)->SetText(sQual);
		GetText(2)->SetText(sCurrRes);
	}
	else {
      CString sGroup; sGroup.LoadString(IDS_RACE_CAPTION_GROUP);
      CString sSegment; sSegment.LoadString(IDS_RACE_CAPTION_SEGMENT);

		GetText(1)->SetText(GetRacePartName(pRace->m_nRacePart));
		wsprintf(temp, "%s %s", sGroup, GetGroupName(pRace->m_nActiveGroup + 1, pRace->GetNumGroups()));
		GetText(2)->SetText(temp);
		wsprintf(temp, "%s %d/%d", 
         sSegment,
			pRace->m_nCurrentRide + 1,
			pRace->GetNumRides());
		if (bDispSegment) GetText(3)->SetText(temp);
		else GetText(3)->SetText(" ");
	}
}

/******************************************************************************/

void CSrcHeader::SetCaptionRacePartPause(CTIMeasuredRace *pRace) {
	char temp[1024];
   CString sGroup; sGroup.LoadString(IDS_RACE_CAPTION_GROUP);
	GetText(1)->SetText(GetRacePartName(pRace->m_nRacePart));
	wsprintf(temp, "%s %s", sGroup, GetGroupName(pRace->m_nActiveGroup + 1, pRace->GetNumGroups()));
	GetText(2)->SetText(temp);
	GetText(3)->SetText(" ");
}

/*********************************************************************** eof **/
