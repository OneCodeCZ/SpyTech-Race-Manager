///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define DRV_FOLDER_MAX_CHARS		31

//*****************************************************************************
class CTIDrvFolder : public CWsfTINamedFolder
{
protected:
	CTIDrvFolder();			

public:
	CTIDrvFolder(const char *pcszTitle);

	// Finds tree item of the given driver.
	// It searches whole subtree if needed.

	CTIDriver* FindDriver(const char* pcszName, 
		const char* pcszSurname,
			const char* pcszNickname, 
			BOOL bSubTree);

	// Finds folder of a given name
	// It searches whole subtree if needed.

	CTIDrvFolder* FindFolder(const char *pcszName, BOOL bSubTree);

	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual BOOL OnRename(CEdit *pEditCtrl, const char *pcszNewTitle);
	virtual int OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2);
	virtual DROPEFFECT OnAcceptDataObject(COleDataObject* pDataObject);
	virtual void OnDropDataObject(COleDataObject* pDataObject, DROPEFFECT dropEffect);

public:
	// Serialization

	DECLARE_SERIAL(CTIDrvFolder);

	// Message map

	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CTIDrvFolder)
	afx_msg void OnNewFolder();
	afx_msg void OnUpdateNewFolder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfDelete(CCmdUI* pCmdUI);
	afx_msg void OnNewDriver();
	afx_msg void OnUpdateNewDriver(CCmdUI* pCmdUI);
	afx_msg void OnExportDrvFolder();
	afx_msg void OnUpdateExportDrvFolder(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG
};
