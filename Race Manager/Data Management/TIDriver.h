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
class CTIDriver : public CWsfTreeItem
{
public:
	CDbDrv	m_Drv;
	BOOL		m_bInRace;				// true -> is part of a team in the race

public:
	CTIDriver();
	CTIDriver(CTIDriver *pTIDrv, BOOL bInRace);
	CTIDriver(const CTIDriver &drv);

	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual CString GetTitle() const;
	virtual void Serialize(CArchive &ar);
	virtual int GetDefaultCommand() { return ID_WSF_PROPERTIES; }
	virtual BOOL OnSubTreeDelete();
	virtual BOOL OnInitProperties(CWsfPropertySheet& s);
	virtual BOOL OnCloseProperties(CWsfPropertySheet& s, BOOL bOk);

protected:

	// Serialization
	DECLARE_SERIAL(CTIDriver);

	// Message map
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CTIDriver)
	afx_msg void OnUpdateWsfDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnAddToRace();
	afx_msg void OnUpdateAddToRace(CCmdUI* pCmdUI);
	afx_msg void OnNewDriver();
	afx_msg void OnUpdateNewDriver(CCmdUI* pCmdUI);
	afx_msg void OnNewFolder();
	afx_msg void OnUpdateNewFolder(CCmdUI* pCmdUI);
	//}}AFX_MSG
};
