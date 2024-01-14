///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Flags

#define	REF_QUATER				CWsfTreeItem::USER1			// Direct advance to quater
#define	REF_SEMI					CWsfTreeItem::USER2			// Direct advance to semi
#define	REF_FINAL					CWsfTreeItem::USER3			// Direct advance to final
#define REF_MARK					CWsfTreeItem::USER4			// General purpose mark


//*****************************************************************************
class RMDM_API CTIRaceEntity : public CWsfTIFolder
{
public:
	enum FORMAT { NAME_SURNAME, SURNAME_NAME, SURNAME_SHORT_NAME, NICKNAME };

public:
	// Start info page
	int					m_nStartRanking;

	// Other
	DWORD				m_dwId;								// For identification after load
	int					m_nLane;							// Lane the entity is currently using.
																		// = Zero base lane indexes or LANE_PAUSE.
	int					m_nRanking;						// Ranking from the last part.

protected:
	CTIRaceEntity();	// for serialization
public:
	CTIRaceEntity(DWORD dwId);

	// Resets entity to the race building state
	void Reset();

	// Set/Get entity name
   virtual CString GetEntityName(FORMAT eFormat) const { ASSERT(false); return ""; } //override
	//virtual void SetEntityName(CString &str) = 0;

	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual int GetId() const { return m_dwId; }
	virtual CString GetTitle() const;
	virtual void Serialize(CArchive &ar);

protected:

	// Serialization
	DECLARE_SERIAL(CTIRaceEntity)

	// Message map
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIRaceEntity)
	//}}AFX_MSG
};
