///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CTIDrvFolder;
class CTIRSFolder;
class CTITSFolder;
class CTIRace;
class CTIRaceManager;
class CTIMeasuredRace;
class CTIDB;

//*****************************************************************************
class RMDM_API CApp : public CWsfApp, public CSetupTrack
{
public:
	// Database tree item structure
	CTIDB*					m_pTIDbRoot;
	CTIDrvFolder*		m_pTIDbDrvFolder;
	CTIRSFolder*		m_pTIDbRSFolder;

	// STM page in setup
	int							m_nComPort;						// Zero based index (0-3)
	BOOL						m_bTrackMappingEnabled;
	BYTE						m_TrackMap[8];				// values: 0-7

   // Sound setup
   int                  m_nWaveOutDevId;

public:
	int							m_nGroupsViewID;
	int							m_nDbTreeItemViewId;
	int							m_nRaceTreeItemViewId;

public:
	CApp();

	// --------------------------------------------------------------------------
	// Database
	// --------------------------------------------------------------------------

protected:
	void LoadDb();
	BOOL SaveDb();
	void BuildDb();
public:

	// Exports the given driver folder to a file.
	void ExportDrivers(CTIDrvFolder* pFolder);

	// Merges the given driver folders.
	// If some items are the same, it displayes "what to do" dialog.
	// Doesn't update UI.
	BOOL ImportDrivers(CTIDrvFolder* pSrcDrvFolder, CTIDrvFolder* pDestDrvFolder);

   // Creates a new folder in the given folder
   CTIDrvFolder* CreateDriverFolder(CTIDrvFolder* pParentFolder);

   // Creates a new driver in the given folder.
   // Returns NULL, if no driver has been created.
   CTIDriver* CreateDriver(CTIDrvFolder* pParentFolder);

protected:
  //---------------------------------------------------------------------------
	// Various
	//---------------------------------------------------------------------------

	void LoadSettings();
	void SaveSettings();

  //---------------------------------------------------------------------------
	// From WSF
	//---------------------------------------------------------------------------

	// Saves also the database when race is about to be saved
	_public virtual BOOL SaveAllModified();

	// Used only for MRU
	_protected virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);

public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

public:
	//{{AFX_MSG(CApp)
	afx_msg void OnRaceNew();
	afx_msg void OnUpdateRaceNew(CCmdUI* pCmdUI);
	afx_msg void OnRaceOpen();
	afx_msg void OnUpdateRaceSave(CCmdUI* pCmdUI);
	afx_msg void OnRaceSave();
	afx_msg void OnUpdateRaceSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnRaceSaveAs();
	afx_msg void OnUpdateRaceClose(CCmdUI* pCmdUI);
	afx_msg void OnRaceClose();
	afx_msg void OnExportDrivers();
	afx_msg void OnUpdateExportDrivers(CCmdUI* pCmdUI);
	afx_msg void OnImportDrivers();
	afx_msg void OnUpdateImportDrivers(CCmdUI* pCmdUI);
	afx_msg void OnSetup();
	afx_msg void OnUpdateSetup(CCmdUI* pCmdUI);
	afx_msg void OnEditGroups();
	afx_msg void OnUpdateEditGroups(CCmdUI* pCmdUI);
	afx_msg void OnEditRides();
	afx_msg void OnUpdateEditRides(CCmdUI* pCmdUI);
	afx_msg void OnRaceStart();
	afx_msg void OnUpdateRaceStart(CCmdUI* pCmdUI);
	afx_msg void OnRaceNextPart();
	afx_msg void OnUpdateRaceNextPart(CCmdUI* pCmdUI);
	afx_msg void OnRaceFinishPart();
	afx_msg void OnUpdateRaceFinishPart(CCmdUI* pCmdUI);
	afx_msg void OnRaceReturn();
	afx_msg void OnUpdateRaceReturn(CCmdUI* pCmdUI);
	afx_msg void OnBriefHelp();
	afx_msg void OnUpdateBriefHelp(CCmdUI* pCmdUI);
	afx_msg void OnHelp();
	afx_msg void OnUpdateHelp(CCmdUI* pCmdUI);
	afx_msg void OnSoundTest();
	afx_msg void OnUpdateSoundTest(CCmdUI* pCmdUI);
   afx_msg void OnEbTest();
   afx_msg void OnUpdateEbTest(CCmdUI *pCmdUI);
	afx_msg void OnGlobalNewDriver();
	afx_msg void OnUpdateGlobalNewDriver(CCmdUI* pCmdUI);
	afx_msg void OnGlobalNewFolder();
	afx_msg void OnUpdateGlobalNewFolder(CCmdUI* pCmdUI);
	afx_msg void OnStmTest();
	afx_msg void OnUpdateStmTest(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
