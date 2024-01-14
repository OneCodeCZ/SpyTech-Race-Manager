///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CTIRaceDrv : public CTIMeasuredEntity
{
public:
	CDbDrv m_Drv;

protected:
	CTIRaceDrv();			// for serialization only
public:
	CTIRaceDrv(DWORD dwId, CTIDriver *pTIDrv);

	// From CTIRaceEntity

	virtual CString GetEntityName(FORMAT eFormat) const;
	virtual void SetEntityName(CString &str) { ASSERT(false); }

	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual void Serialize(CArchive &ar);
	virtual int GetDefaultCommand() { return ID_WSF_PROPERTIES; }
	virtual BOOL OnSubTreeDelete();
	virtual BOOL OnInitProperties(CWsfPropertySheet &s);
	virtual BOOL OnCloseProperties(CWsfPropertySheet &s, BOOL bOk);

protected:

	// Serialization
	DECLARE_SERIAL(CTIRaceDrv);

	// Message map
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIRaceDrv)
	afx_msg void OnUpdateWsfDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	//}}AFX_MSG
};
