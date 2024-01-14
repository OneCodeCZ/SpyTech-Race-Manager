///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __TIRS_H__
#define __TIRS_H__

#define DB_RS_NAME_MAX_CHARS		31

// m_nDistributionType

#define RS_DISTR_TYPE_WORST_BEST		0
#define RS_DISTR_TYPE_EVEN					1
#define RS_DISTR_TYPE_RANDOM				2

// m_nPrimaryEval

#define RS_EVAL_NUM_LAPS						0
#define RS_EVAL_FIRST_TIME					1

// m_nAdvancesQual indexes

#define RS_ADV_QUAL_QUATER					0
#define RS_ADV_QUAL_SEMI						1
#define RS_ADV_QUAL_FINAL						2

// m_nAdvance indexes

#define RS_ADV_CONSI								0
#define RS_ADV_QUATER								1
#define RS_ADV_SEMI									2

	
//*****************************************************************************
class RMDM_API CTIRS : public CWsfTreeItem
{
public:
	// Basic page
	CString	m_strName;
	BOOL		m_bQual;
	// m_nFirst part
	// Eg. if it is equal to SEMI, the enabled parts are SEMI & FINAL
	// -> all higher parts are enabled.
	int			m_nFirstPart;						// RACE_PART_XXX values
	int			m_nDistributionType;
	CString	m_strChanging;

	// Qual page
	int			m_nPrimaryEval;
	int			m_nNumRides;
	int			m_nNumAttempts;

	// Advances page
	int			m_nAdvancesQual[3];				// 0=quater, 1=semi, 2=final
	int			m_nAdvances[3][2];				// [y][x]: y: 0=consi, 1=quater, 2=semi
																		// x: 0=total, 1=group

	// Time System page
	CTimeSpan		m_Times[5][3];				// 0-4 = qual-final, 0-2=warmup, race, pause

	// Points page
	BOOL						m_bEnablePoints;
	int							m_nPointsType;
	CWslArray<int>	m_aPoints;

	// Other
	BOOL		m_bInRace;								// true -> this item is part of a race

public:
	CTIRS();
	void operator =(CTIRS &RS);

	// Returns values from the m_nAdvances array according the race part.
	int GetAdvanceGroupBest(int nRacePart);
	int GetAdvanceTotal(int nRacePart);

	// Returns time in ms for the given race part & time type
	int GetTimeSettings(int nPart, int nType);

	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual void SetTitle(const char *pcszTitle) { m_strName = pcszTitle; }
	virtual CString GetTitle() const { return m_strName; }
	virtual void Serialize(CArchive &ar);
	virtual int GetDefaultCommand() { return ID_WSF_PROPERTIES; }
	virtual BOOL OnSubTreeDelete();
	virtual BOOL OnInitProperties(CWsfPropertySheet &s);
	virtual BOOL OnCloseProperties(CWsfPropertySheet &s, BOOL bOk);

protected:

	DECLARE_SERIAL(CTIRS);
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIRS)
	afx_msg void OnUpdateWsfDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnAddToRace();
	afx_msg void OnUpdateAddToRace(CCmdUI* pCmdUI);
	//}}AFX_MSG
};

#endif // __TIRS_H__