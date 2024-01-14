///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_TEAM_NAME_CHARS			63


//*****************************************************************************
class CTIRaceTeam : public CTIMeasuredEntity
{
public:
	CString	m_strName;

protected:
	CTIRaceTeam();							// Serialization
public:
	CTIRaceTeam(DWORD dwId);			

	// Finds the driver in this team.
	CTIDriver *FindDrv(const char *pcszName, const char *pcszSurname);

	// Counts number of drivers in the team
	int GetDrvCount();

  //---------------------------------------------------------------------------
	// From CTIRaceEntity
	//---------------------------------------------------------------------------

	virtual CString GetEntityName(FORMAT eFormat) const { return m_strName; }
	//virtual void SetEntityName(CString &str) { m_strName = str; }

  //---------------------------------------------------------------------------
	// From CWslTreeItem
	//---------------------------------------------------------------------------

	virtual void Serialize(CArchive &ar);
	virtual int GetDefaultCommand() { return ID_WSF_OPEN; }
	virtual BOOL OnSubTreeDelete();
	virtual BOOL OnInitProperties(CWsfPropertySheet &s);
	virtual BOOL OnCloseProperties(CWsfPropertySheet &s, BOOL bOk);
	virtual int OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2);
	virtual DROPEFFECT OnAcceptDataObject(COleDataObject *pDataObject);
	virtual void OnDropDataObject(COleDataObject *pDataObject, DROPEFFECT dropEffect);

protected:
	DECLARE_SERIAL(CTIRaceTeam);
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIRaceTeam)
	afx_msg void OnUpdateWsfDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfOpen(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG
};

