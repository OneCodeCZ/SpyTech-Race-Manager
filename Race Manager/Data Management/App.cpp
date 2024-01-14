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

#include "stdafx.h"
#include "main.h"
#include "PgSetupSTM.h"
#include "PgSetupTrack.h"
#include "PgSetupSound.h"
#include "GroupsView.h"
#include "DocWorkspace.h"
#include "TIDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CApp* g_pApp;
char temp[1024], temp2[1024];

// Lane colors

COLORREF g_rgbColors[8] = 
{
	RGB(255, 0, 0),				// Red
	RGB(255, 255, 255),		// White
	RGB(0, 255, 0),				// Green
	RGB(255, 128, 0),			// Orange
	RGB(0, 0, 255),				// Blue
	RGB(255, 255, 0),			// Yellow
	RGB(200, 0, 255),			// Violet
	RGB(0, 0, 0),					// Black
};

BEGIN_MESSAGE_MAP(CApp, CWsfApp)
	//{{AFX_MSG_MAP(CApp)
	ON_COMMAND(ID_RACE_NEW, OnRaceNew)
	ON_UPDATE_COMMAND_UI(ID_RACE_NEW, OnUpdateRaceNew)
	ON_COMMAND(ID_RACE_OPEN, OnRaceOpen)
	ON_UPDATE_COMMAND_UI(ID_RACE_SAVE, OnUpdateRaceSave)
	ON_COMMAND(ID_RACE_SAVE, OnRaceSave)
	ON_UPDATE_COMMAND_UI(ID_RACE_SAVE_AS, OnUpdateRaceSaveAs)
	ON_COMMAND(ID_RACE_SAVE_AS, OnRaceSaveAs)
	ON_UPDATE_COMMAND_UI(ID_RACE_CLOSE, OnUpdateRaceClose)
	ON_COMMAND(ID_RACE_CLOSE, OnRaceClose)
	ON_COMMAND(ID_EXPORT_DRIVERS, OnExportDrivers)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_DRIVERS, OnUpdateExportDrivers)
	ON_COMMAND(ID_IMPORT_DRIVERS, OnImportDrivers)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_DRIVERS, OnUpdateImportDrivers)
	ON_COMMAND(ID_SETUP, OnSetup)
	ON_UPDATE_COMMAND_UI(ID_SETUP, OnUpdateSetup)
	ON_COMMAND(ID_EDIT_GROUPS, OnEditGroups)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GROUPS, OnUpdateEditGroups)
	ON_COMMAND(ID_EDIT_RIDES, OnEditRides)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RIDES, OnUpdateEditRides)
	ON_COMMAND(ID_RACE_START, OnRaceStart)
	ON_UPDATE_COMMAND_UI(ID_RACE_START, OnUpdateRaceStart)
	ON_COMMAND(ID_RACE_NEXT_PART, OnRaceNextPart)
	ON_UPDATE_COMMAND_UI(ID_RACE_NEXT_PART, OnUpdateRaceNextPart)
	ON_COMMAND(ID_RACE_FINISH_PART, OnRaceFinishPart)
	ON_UPDATE_COMMAND_UI(ID_RACE_FINISH_PART, OnUpdateRaceFinishPart)
	ON_COMMAND(ID_RACE_RETURN, OnRaceReturn)
	ON_UPDATE_COMMAND_UI(ID_RACE_RETURN, OnUpdateRaceReturn)
	ON_COMMAND(ID_BRIEF_HELP, OnBriefHelp)
	ON_UPDATE_COMMAND_UI(ID_BRIEF_HELP, OnUpdateBriefHelp)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP, OnUpdateHelp)
	ON_COMMAND(ID_SOUND_TEST, OnSoundTest)
	ON_UPDATE_COMMAND_UI(ID_SOUND_TEST, OnUpdateSoundTest)
	ON_COMMAND(ID_EB_TEST, OnEbTest)
	ON_UPDATE_COMMAND_UI(ID_EB_TEST, OnUpdateEbTest)
	ON_COMMAND(ID_GLOBAL_NEW_DRIVER, OnGlobalNewDriver)
	ON_UPDATE_COMMAND_UI(ID_GLOBAL_NEW_DRIVER, OnUpdateGlobalNewDriver)
	ON_COMMAND(ID_GLOBAL_NEW_FOLDER, OnGlobalNewFolder)
	ON_UPDATE_COMMAND_UI(ID_GLOBAL_NEW_FOLDER, OnUpdateGlobalNewFolder)
	ON_COMMAND(ID_STM_TEST, OnStmTest)
	ON_UPDATE_COMMAND_UI(ID_STM_TEST, OnUpdateStmTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CApp::CApp()
{
	m_pTIDbRoot = NULL;
	ASSERT(g_pApp == NULL);
	g_pApp = this;
	m_nGroupsViewID = 0;
	m_nDbTreeItemViewId = 0;
	m_nRaceTreeItemViewId = 0;
}

//*****************************************************************************
BOOL CApp::InitInstance()
{
	// Init

	if (!AfxOleInit())
	{
		AfxMessageBox(IDS_AFXOLEINIT_FAILED);
		return FALSE;
	}

	Enable3dControls();
	LoadStdProfileSettings();
	LoadSettings();
	AddIcons(IDB_SMALL_ICONS, IDB_NORMAL_ICONS);
   SetAppNameAndVersion(IDR_MAINFRAME, GetModuleHandle(WsfGetApp()->GetAppPath()+"dm.dll"));

   // Create data folder

   try
   {
      CWslFS::CreatePath(GetAppDataPath());
   }
   catch (CException* e)
   {
      WsfReportError(e, IDS_ERROR_CREATING_DATA_FOLDER, true);
      return false;
   }

	// Register commands

	RegisterStandardCommands();

	AddCmdSimple("ID_RACE_NEW", ID_RACE_NEW, 0, IDB_SMALL_ICONS);
	AddCmdSimple("ID_RACE_OPEN", ID_RACE_OPEN, 4, IDB_SMALL_ICONS);
	AddCmdSimple("ID_RACE_SAVE", ID_RACE_SAVE, 10, IDB_SMALL_ICONS);
	AddCmdSimple("ID_NEW_FOLDER", ID_NEW_FOLDER, 11, IDB_SMALL_ICONS);
	AddCmdSimple("ID_NEW_DRIVER", ID_NEW_DRIVER, 12, IDB_SMALL_ICONS);
	AddCmdSimple("ID_ADD_TO_RACE", ID_ADD_TO_RACE, 15, IDB_SMALL_ICONS, true, true);

   AddCmdSimple("ID_NEW_GLOBAL_RS", ID_NEW_GLOBAL_RS, 13, IDB_SMALL_ICONS);
	AddCmdSimple("ID_NEW_GLOBAL_TEAM", ID_NEW_GLOBAL_TEAM, 14, IDB_SMALL_ICONS);
	AddCmdSimple("ID_GLOBAL_NEW_FOLDER", ID_GLOBAL_NEW_FOLDER, 11, IDB_SMALL_ICONS);
	AddCmdSimple("ID_GLOBAL_NEW_DRIVER", ID_GLOBAL_NEW_DRIVER, 12, IDB_SMALL_ICONS);

	AddCmdSimple("ID_RACE_SAVE_AS", ID_RACE_SAVE_AS);
	AddCmdSimple("ID_EXPORT_DRIVERS", ID_EXPORT_DRIVERS);
	AddCmdSimple("ID_IMPORT_DRIVERS", ID_IMPORT_DRIVERS);
	AddCmdSimple("ID_RACE_START", ID_RACE_START);
	AddCmdSimple("ID_RACE_RETURN", ID_RACE_RETURN);
	AddCmdSimple("ID_RACE_CLOSE", ID_RACE_CLOSE);
	AddCmdSimple("ID_RACE_NEXT_PART", ID_RACE_NEXT_PART);
	AddCmdSimple("ID_EDIT_RIDES", ID_EDIT_RIDES);
	AddCmdSimple("ID_RACE_FINISH_PART", ID_RACE_FINISH_PART);
	AddCmdSimple("ID_EDIT_GROUPS", ID_EDIT_GROUPS);
	AddCmdSimple("ID_EXERCISE", ID_EXERCISE);
	AddCmdSimple("ID_SOUND_TEST", ID_SOUND_TEST);
	AddCmdSimple("ID_SETUP", ID_SETUP);
	AddCmdSimple("ID_BRIEF_HELP", ID_BRIEF_HELP);

   AddCmdSimple("ID_EB_TEST", ID_EB_TEST);
   AddCmdSimple("ID_STM_TEST", ID_STM_TEST);

	// Init window

	CMainFrame* pFrame = new CMainFrame();
	m_pMainWnd = pFrame;
	VERIFY(pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL));
   ProcessMenu(*GetFrame()->GetMenu());

	// Create toolbars

	CWsfToolBar* p = AddToolBar(CString((char*)IDS_STANDARD), 1, true);
	p->AddCmd(ID_RACE_NEW);
	p->AddCmd(ID_RACE_OPEN);
	p->AddCmd(ID_RACE_SAVE);
	p->AddSeparator();
	p->AddCmd(ID_FILE_PRINT);
	p->AddSeparator();
	p->AddCmd(ID_WSF_PROPERTIES);
	p->AddSeparator();
	p->AddCmd(ID_GLOBAL_NEW_FOLDER);
	p->AddCmd(ID_GLOBAL_NEW_DRIVER);
	p->AddCmd(ID_NEW_GLOBAL_RS);
	p->AddSeparator();
	p->AddCmd(ID_ADD_TO_RACE);
	p->AddCmd(ID_NEW_GLOBAL_TEAM);

	// Init database UI

	LoadDb();

	CDocument* pDoc = new CDocument();
	OpenDoc(pDoc);
   CString s;
	VERIFY(s.LoadString(IDS_DATABASE));
	OpenTreeView(m_pTIDbRoot, pDoc, s, -1, true, IDB_SMALL_ICONS, 5); 

	return CWsfApp::InitInstance();
}

//*****************************************************************************
int CApp::ExitInstance() 
{
	delete m_pTIDbRoot;
	m_pTIDbRoot = NULL;
	return CWsfApp::ExitInstance();
}

//*****************************************************************************
BOOL CApp::SaveAllModified()
{
	if (!SaveDb())
	{
		return (AfxMessageBox(IDS_EXIT_EVEN_WITH_DB_LOSS, MB_ICONQUESTION|MB_YESNO))==IDYES;
	}

	return CWsfApp::SaveAllModified();
}

/*****************************************************************************
*
* SETTINGS
*
*****************************************************************************/

/*****************************************************************************/
void CApp::LoadSettings()
{
	// STM/2008 settings page

	m_nComPort = GetProfileInt("Settings\\STM2008", "com_port", 2-1);
	m_bTrackMappingEnabled = GetProfileInt("Settings\\STM2008", "track_mapping_enabled", 0);

	for (int a=0; a<8; a++) 
	{
		wsprintf(temp, "track_map_%d", a);
		m_TrackMap[a] = GetProfileInt("Settings\\STM2008", temp, a);
	}

	// Track settings page

	m_strOwner = GetProfileString("Settings\\Track", "owner", CString((char*)IDS_UNKNOWN));
	m_strTrackName= GetProfileString("Settings\\Track", "name", CString((char*)IDS_UNKNOWN_TRACK));
	m_nTrackLength = GetProfileInt("Settings\\Track", "length", 4700);
	m_nNumLanes = GetProfileInt("Settings\\Track", "num_lanes_code", SETUP_NUM_LANES_8);
	m_nDivisionType = GetProfileInt("Settings\\Track", "division_type", SETUP_DIVISION_TYPE_UNITS);

	for (a=0; a<8; a++)
	{
		wsprintf(temp, "lane_color_%d", a);
		m_nLaneColor[a] = GetProfileInt("Settings\\Track", temp, a);
	}

   // Sound settings page

   m_nWaveOutDevId = GetProfileInt("Settings\\Sound", "wave_out_dev_id", 0);

}

/*****************************************************************************/
void CApp::SaveSettings()
{
	// STM/2008 settings page

	WriteProfileInt("Settings\\STM2008", "com_port", m_nComPort);
	WriteProfileInt("Settings\\STM2008", "track_mapping_enabled", m_bTrackMappingEnabled);

	for (int a=0; a<8; a++) 
	{
		wsprintf(temp, "track_map_%d", a);
		WriteProfileInt("Settings\\STM2008", temp, m_TrackMap[a]);
	}

	// Track settings page

	WriteProfileString("Settings\\Track", "owner", m_strOwner);
	WriteProfileString("Settings\\Track", "name", m_strTrackName);
	WriteProfileInt("Settings\\Track", "length", m_nTrackLength);
	WriteProfileInt("Settings\\Track", "num_lanes_code", m_nNumLanes);
	WriteProfileInt("Settings\\Track", "division_type", m_nDivisionType);

	for (a=0; a<8; a++)
	{
		wsprintf(temp, "lane_color_%d", a);
		WriteProfileInt("Settings\\Track", temp, m_nLaneColor[a]);
	}

   // Sound settings page

   WriteProfileInt("Settings\\Sound", "wave_out_dev_id", m_nWaveOutDevId);
}

/*****************************************************************************
*
* DATABASE
*
*****************************************************************************/

//*****************************************************************************
#define NEW_RACER(NAME, SURNAME) \
	d = new CTIDriver(); \
	strcpy(d->m_Drv.m_szSurname, (SURNAME)); \
	strcpy(d->m_Drv.m_szNickname, (SURNAME)); \
	strcpy(d->m_Drv.m_szName, (NAME)); \
	m_pTIDbDrvFolder->AddChild(d);

//*****************************************************************************
void CApp::BuildDb()
{
	CWsfTreeItem *p;
	ASSERT(m_pTIDbRoot == NULL);

	m_pTIDbRoot = new CTIDB();
		
	m_pTIDbDrvFolder = new CTIDrvFolder(CString((char*)IDS_DRIVERS));
	m_pTIDbRoot->AddChild(m_pTIDbDrvFolder);
	
	p = new CTIRSFolder(CString((char*)IDS_RACE_SYSTEMS));
	m_pTIDbRoot->AddChild(p);

/*	CTIDriver *d;
	NEW_RACER("Josef", "Korec")
	NEW_RACER("Lýdie", "Horácková")
	NEW_RACER("Radek", "Tetík")
	NEW_RACER("Adam J.", "Sporka")
	NEW_RACER("Jan", "Valta")
	NEW_RACER("Eva", "Valtová")
	NEW_RACER("Šárka", "Svobodová")
	NEW_RACER("Eliška", "Svobodová")
	NEW_RACER("Jan", "Zahradník")
	NEW_RACER("Jakub", "Ahmadyar")
	NEW_RACER("Jana", "Zdráhalová")
	NEW_RACER("Markéta", "Zahradníková")
	NEW_RACER("Markéta", "Holubová")
	NEW_RACER("Karel", "Kinter")
	NEW_RACER("Václav", "Helebrant")
	NEW_RACER("Martina", "Dostálová")
	NEW_RACER("Martina", "Nechanická")
	NEW_RACER("Bedrich", "Svoboda")
	NEW_RACER("Dan", "Langr")
	NEW_RACER("Jack", "Opata")
	NEW_RACER("Jitka", "Houfková")
	NEW_RACER("Jitka", "Krásná")
	NEW_RACER("Josef", "Vojtech")
	NEW_RACER("Marek", "Stefanidis")
	NEW_RACER("Ondrej", "Spinka")
	NEW_RACER("Radek", "Hladík")
	NEW_RACER("Tonda", "Mareš")
	NEW_RACER("Honza", "Rosen")
	NEW_RACER("Martin", "Šolc")
	NEW_RACER("Murat", "Acar")
	NEW_RACER("Jan", "Vosátka")
	NEW_RACER("Katerina", "Vosátková")
	NEW_RACER("Hana", "Svetlíková")
	NEW_RACER("Ludek", "Hnilicka")
	NEW_RACER("Vítek", "Švachoucek")
	NEW_RACER("Petr", "Nejedlý")
	NEW_RACER("Tomáš", "Síegl")
	NEW_RACER("Michal", "Švor")
	NEW_RACER("Vladimír", "Nemec")
	NEW_RACER("Milan", "Rüther")
	NEW_RACER("Michal", "Kyndl")
	NEW_RACER("Martin", "Hacecký")
	NEW_RACER("Daniel", "Binko")
	NEW_RACER("Martin", "Zelenka")
	NEW_RACER("Vojta", "Capek")
	NEW_RACER("Jan", "Waraus")
	NEW_RACER("Tomáš", "Vavrda")
	NEW_RACER("Václav", "Vanek")
	NEW_RACER("Jan", "Strachota")
	NEW_RACER("Tomáš", "Gorc")
	NEW_RACER("Josef", "Reichman")
	NEW_RACER("Daniel", "Sýkora")
	NEW_RACER("Pavel", "Žikovský")*/
}

//*****************************************************************************
void CApp::LoadDb()
{
	ASSERT(m_pTIDbRoot == NULL);

	try
	{
      CString s = GetAppDataPath() + "db.dat";
      if (!CWslFile::Exists(s)) s = GetAppPath() + "data\\db.dat";

		CFile f(s, CFile::modeRead);
		CArchive ar(&f, CArchive::load);
		ar >> m_pTIDbRoot;
		ar.Close();
	}
	
	catch (CException* e)
	{
		WsfReportError(e, IDS_DATA_LOAD_ERROR, true);
		delete m_pTIDbRoot;
		BuildDb();
	}

	// Init stuff
		
	m_pTIDbDrvFolder = (CTIDrvFolder*) m_pTIDbRoot->GetChild();
	ASSERT(m_pTIDbDrvFolder);
	m_pTIDbRSFolder = (CTIRSFolder*) m_pTIDbDrvFolder->GetNextSibling();
	ASSERT(m_pTIDbRSFolder);
}

//*****************************************************************************
BOOL CApp::SaveDb()
{
	try
	{
      CFile f(GetAppDataPath() + "db.dat", CFile::modeCreate | CFile::modeWrite);
		CArchive ar(&f, CArchive::store);
		ar << m_pTIDbRoot;
		ar.Close();
	}
	
	catch (CException* e)
	{
		WsfReportError(e, IDS_DATA_SAVE_ERROR, true);
		return false;
	}

	return true;
}

/*****************************************************************************/
CTIDrvFolder* CApp::CreateDriverFolder(CTIDrvFolder* pParentFolder)
{
	CTIDrvFolder *pItem = new CTIDrvFolder(CString((char*)IDS_NEW_FOLDER));
	pParentFolder->AddChild(pItem);

	WsfGetApp()->TreeHasChanged(pItem, CWsfApp::ETreeChanges::ITEM_ADDED);

   return pItem;
}

/*****************************************************************************/
CTIDriver* CApp::CreateDriver(CTIDrvFolder* pParentFolder)
{
	CTIDriver* pItem = new CTIDriver();

	// NOTE: Item must be in the tree, because the properties
	// must know the driver's folder
	pParentFolder->AddChild(pItem);

	if (pItem->DisplayProperties(true) != IDOK) 
	{
		delete pItem;
		return NULL;
	}

	pItem->Reorder();
	WsfGetApp()->TreeHasChanged(pItem, CWsfApp::ETreeChanges::ITEM_ADDED);

   return pItem;
}


/*****************************************************************************
*
* RACE/WORKSPACE FILE HANDLING 
*
*****************************************************************************/

/*****************************************************************************/
CDocument* CApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	ASSERT(lpszFileName);

	// Close current workspace

	if (IsWorkspaceOpened())
	{
		// NOTE: (CDocument*)1 is returned, because NULL causes
		// the lpszFileName to be removed from the MRU
		if (!SaveModifiedWorkspace()) return (CDocument*)1;
		CloseWorkspace();
	}

	// Open workspace

	return OpenWorkspace(new CWorkspace(), lpszFileName) ? GetWorkspaceDoc() : NULL;
}

//*****************************************************************************
void CApp::OnRaceNew() 
{
	// Close current workspace

	if (IsWorkspaceOpened())
	{
		if (!SaveModifiedWorkspace()) return;
		CloseWorkspace();
	}

	// Init

	NewWorkspace(new CWorkspace());
}

//*****************************************************************************
void CApp::OnUpdateRaceNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CApp::OnRaceOpen() 
{
	// Get file name

	CFileDialog d(true, RACE_EXT, NULL, 
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER, FILTER_TEMPLATE, WsfGetFrame());
	if (d.DoModal() != IDOK) return;

	// Close current workspace

	if (IsWorkspaceOpened())
	{
		if (!SaveModifiedWorkspace()) return;
		CloseWorkspace();
	}

	// Open workspace

	OpenWorkspace(new CWorkspace(), d.GetPathName());
}

//*****************************************************************************
void CApp::OnRaceSave() 
{
	SaveWorkspace(false);
}

//*****************************************************************************
void CApp::OnUpdateRaceSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsWorkspaceOpened());
}

//*****************************************************************************
void CApp::OnRaceSaveAs() 
{
	SaveWorkspace(true);
}

//*****************************************************************************
void CApp::OnUpdateRaceSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsWorkspaceOpened());
}

//*****************************************************************************
void CApp::OnRaceClose() 
{
	if (IsWorkspaceOpened())
	{
		if (!SaveModifiedWorkspace()) return;
		CloseWorkspace();
	}
}

//*****************************************************************************
void CApp::OnUpdateRaceClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsWorkspaceOpened());
}


/*****************************************************************************
*
* EXPORT/IMPORT DRIVERS
*
*****************************************************************************/

/*****************************************************************************/
void CApp::ExportDrivers(CTIDrvFolder* pFolder)
{
	CFileDialog d(false, DRIVERS_EXT, 
		NULL, 
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
		DRIVERS_FILTER_TEMPLATE, WsfGetFrame());

	if (d.DoModal() != IDOK) return;

	try
	{
		pFolder->Save(d.GetPathName());
	}
	catch (CException* e)
	{
		WsfReportError(e, IDS_ERROR_DRIVERS_EXPORT, true);
	}

}

//*****************************************************************************
BOOL CApp::ImportDrivers(CTIDrvFolder* pSrcDrvFolder, CTIDrvFolder* pDestDrvFolder)
{
	CTIDriver* pDrv, *pDrv2;
	CTIDrvFolder* pFolder, *pFolder2;
	CString s;

	for (CWsfTreeItem* p=pSrcDrvFolder->GetChild(); p; p=p->GetNextSibling())
	{
		if (p->IsKindOf(RUNTIME_CLASS(CTIDrvFolder)))
		{
			pFolder = (CTIDrvFolder*) p;

			// Create the folder if it doesn't already exist

			s = pFolder->GetTitle();
			pFolder2 = pDestDrvFolder->FindFolder(s, false);
			
			if (!pFolder2)
			{
				s = pFolder->GetTitle();
				pFolder2 = new CTIDrvFolder(s);
				pDestDrvFolder->AddChild(pFolder2);
   			TreeHasChanged(pFolder2, CWsfApp::ETreeChanges::ITEM_ADDED);
			}
			
			// Let's recurse...

			if (!ImportDrivers(pFolder, pFolder2)) return false;
		}
		else
		{
			ASSERT_KINDOF(CTIDriver, p);
			pDrv = (CTIDriver*) p;

			// Check if the driver exists in the current folder 
			// of the local DB

			pDrv2 = pDestDrvFolder->FindDriver(pDrv->m_Drv.m_szName, 
				pDrv->m_Drv.m_szSurname, 
				pDrv->m_Drv.m_szNickname,
				false);

			if (pDrv2)
			{
				s.Format(IDS_REWRITE_DRIVER, 
					pDrv2->m_Drv.m_szName, 
					pDrv2->m_Drv.m_szSurname, 
					pDrv2->m_Drv.m_szNickname,
					pDestDrvFolder->GetPathTitle());

				switch (AfxMessageBox(s, MB_ICONQUESTION|MB_YESNOCANCEL))
				{
				case IDCANCEL	: return false;
				case IDYES		: DeleteTreeItem(pDrv2, true, 0); break;
				case IDNO			: continue;
				}
			}

			// Add driver

			pDrv2 = new CTIDriver(*pDrv);
			pDestDrvFolder->AddChild(pDrv2);
			TreeHasChanged(pDrv2, CWsfApp::ETreeChanges::ITEM_ADDED);
		}
	}

	return true;
}

//*****************************************************************************
void CApp::OnImportDrivers() 
{
	CTIDrvFolder *pTI;

	// Load drivers
		
	CFileDialog d(true, DRIVERS_EXT, 
		NULL, 
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER, 
		DRIVERS_FILTER_TEMPLATE, WsfGetFrame());

	if (d.DoModal() != IDOK) return;

	try
	{
		pTI = (CTIDrvFolder*) CWsfTreeItem::Load(d.GetPathName());
		ASSERT_KINDOF(CTIDrvFolder, pTI);
	}
	catch (CException *pErr)
	{
		WsfReportError(pErr, IDS_ERROR_DRIVERS_IMPORT, true);
	}

	// Merge with current drivers tree

	ImportDrivers(pTI, m_pTIDbDrvFolder);
}

//*****************************************************************************
void CApp::OnUpdateImportDrivers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CApp::OnExportDrivers() 
{
	ExportDrivers(m_pTIDbDrvFolder);
}

//*****************************************************************************
void CApp::OnUpdateExportDrivers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}


/*****************************************************************************
*
* VARIOUS COMMANDS
*
*****************************************************************************/

//*****************************************************************************
void CApp::OnSetup() 
{
	CWsfPropertySheet s(false);

	s.AddPage(new CPgSetupTrack(this, false));
	s.AddPage(new CPgSetupSTM(this));
	s.AddPage(new CPgSetupSound());
	s.SetTitle(CString((char*)IDS_SETUP));

	s.DoModal();
	
	s.RemoveAllPages();

	SaveSettings();
}

//*****************************************************************************
void CApp::OnUpdateSetup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CApp::OnEditGroups() 
{
	ASSERT(IsWorkspaceOpened());

	if (!ActivateView(m_nGroupsViewID))
	{
		CString s((char*)IDS_GROUPS);
		CGroupsView* pView = new CGroupsView(WsfGetCWorkspace()->m_pTIRace);
		m_nGroupsViewID = OpenView(pView, WsfGetWorkspace(), s, false, true, -1, true, IDB_SMALL_ICONS, 7);
		ASSERT(m_nGroupsViewID);
	}
}

//*****************************************************************************
void CApp::OnUpdateEditGroups(CCmdUI* pCmdUI) 
{
	if (IsWorkspaceOpened())
	{
		CTIRace* p = WsfGetCWorkspace()->m_pTIRace;
		pCmdUI->Enable(p->m_nRacePart > RACE_PART_QUAL && p->m_nRacePart < RACE_PART_DONE);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CApp::OnEditRides() 
{
	ASSERT(IsWorkspaceOpened());
	MAPI_EditRides(WsfGetCWorkspace()->m_pTIRace);
}

//*****************************************************************************
void CApp::OnUpdateEditRides(CCmdUI* pCmdUI) 
{
	if (IsWorkspaceOpened())
	{
		CTIRace* p = WsfGetCWorkspace()->m_pTIRace;
		pCmdUI->Enable(p->m_nRacePart > RACE_PART_QUAL && p->m_bPartInProgress);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CApp::OnRaceStart() 
{
	ASSERT(IsWorkspaceOpened());
	WsfGetCWorkspace()->m_pTIRace->OnRaceStart();
}

//*****************************************************************************
void CApp::OnUpdateRaceStart(CCmdUI* pCmdUI) 
{
	if (IsWorkspaceOpened())
	{
		CTIRace* p = WsfGetCWorkspace()->m_pTIRace;
		pCmdUI->Enable(p->m_nRacePart == RACE_PART_NONE);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CApp::OnRaceNextPart() 
{
	ASSERT(IsWorkspaceOpened());
	WsfGetCWorkspace()->m_pTIRace->OnRaceNextPart();
	//WsfGetCWorkspace()->m_pTIRace->CTIRace::OnRaceNextPart();	// to skip meassuring
}

//*****************************************************************************
void CApp::OnUpdateRaceNextPart(CCmdUI* pCmdUI) 
{
	if (IsWorkspaceOpened())
	{
		CTIMeasuredRace* p = (CTIMeasuredRace*) WsfGetCWorkspace()->m_pTIRace;
		pCmdUI->Enable(
			p->m_nRacePart > RACE_PART_NONE && 
			p->m_nRacePart <= RACE_PART_FINAL && 
			!p->m_bPartFinished &&
			!p->m_bPartEndedButNotAccepted);
		
		// This is not clean solution!
		//CString s;
		//VERIFY(s.LoadString(p->m_bPartInProgress ? IDS_CONTINUE_MEASURE_RACE_PART : IDS_START_MEASURE_RACE_PART));
		//pCmdUI->SetText(s);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CApp::OnRaceFinishPart() 
{
	ASSERT(IsWorkspaceOpened());

	if (AfxMessageBox(IDS_REALLY_FINISH_PART, MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		CTIMeasuredRace* p = (CTIMeasuredRace*) WsfGetCWorkspace()->m_pTIRace;
		p->OnRacePartFinished();			
		
		if (p->m_nRacePart == RACE_PART_DONE) 
		{
			AfxMessageBox(IDS_RACE_DONE, MB_OK|MB_ICONINFORMATION);
			p->CreateFinalResultsList();
			OnRaceSave();
		}
	}
}

//*****************************************************************************
void CApp::OnUpdateRaceFinishPart(CCmdUI* pCmdUI) 
{
	if (IsWorkspaceOpened())
	{
		CTIMeasuredRace* p = (CTIMeasuredRace*) WsfGetCWorkspace()->m_pTIRace;
		pCmdUI->Enable(p->m_bPartFinished || p->m_bPartEndedButNotAccepted);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CApp::OnRaceReturn() 
{
	ASSERT(IsWorkspaceOpened());
	CTIMeasuredRace* pRace = (CTIMeasuredRace*) WsfGetCWorkspace()->m_pTIRace;
	ASSERT(pRace);

	if (!pRace->m_bPartInProgress) 
	{
		if (AfxMessageBox(IDS_RESET_RACE, MB_YESNO|MB_ICONQUESTION)==IDYES) 
			pRace->OnResetRace();
		return;
	}

	switch (pRace->m_nRacePart)
	{
	case RACE_PART_NONE:
		if (AfxMessageBox(IDS_RESET_RACE, MB_YESNO|MB_ICONQUESTION)==IDYES) 
			pRace->OnResetRace();
		break;

	case RACE_PART_QUAL:
		if (AfxMessageBox(IDS_RESET_RACE, MB_YESNO|MB_ICONQUESTION)==IDYES) 
			pRace->OnResetRace();
		break;

	default:
		pRace->m_bNeedRecovery = TRUE;
		pRace->OnRaceNextPart();
		break;
	}
}

//*****************************************************************************
void CApp::OnUpdateRaceReturn(CCmdUI* pCmdUI) 
{
	if (IsWorkspaceOpened())
	{
		pCmdUI->Enable(WsfGetCWorkspace()->m_pTIRace->m_nRacePart != RACE_PART_NONE);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CApp::OnBriefHelp() 
{
	OpenHtmlFile(GetAppPath() + "help\\pruvodce_programem.html",
		CString((char*)IDS_BRIEF_HELP), IDB_SMALL_ICONS, 9, false);
}

//*****************************************************************************
void CApp::OnUpdateBriefHelp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CApp::OnHelp() 
{
	OpenHtmlFile(GetAppPath() + "help\\index.html",
		CString((char*)IDS_HELP), IDB_SMALL_ICONS, 9, false);
}

//*****************************************************************************
void CApp::OnUpdateHelp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

/*****************************************************************************/
void CApp::OnSoundTest() 
{
	CWaitCursor wc;
	AfxMessageBox(IDS_SOUND_TEST, MB_ICONINFORMATION|MB_OK);
	MAPI_TestSound(WsfGetCApp()->m_nWaveOutDevId);
}

/*****************************************************************************/
void CApp::OnUpdateSoundTest(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}


void CApp::OnEbTest()
{
   MAPI_TestEB();
}

void CApp::OnUpdateEbTest(CCmdUI *pCmdUI)
{
}

/*****************************************************************************/
void CApp::OnGlobalNewDriver() 
{
	CWslArray<CRuntimeClass*> aClasses;
	aClasses += RUNTIME_CLASS(CTIDrvFolder);

	CTIDrvFolder* pParent = (CTIDrvFolder*) WsfSelectTreeItem(IDS_SELECT_FOLDER, 
      IDS_SELECT_FOLDER_FOR_DRIVER,
		g_pApp->m_pTIDbRoot,
		&aClasses,
		WsfGetFrame());

	if (!pParent) return;

   CTIDriver* p = CreateDriver(pParent);
   if (!p) return;

   EnsureTreeItemVisible(p);
   pParent->OnWsfOpen();
}

void CApp::OnUpdateGlobalNewDriver(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}

/*****************************************************************************/
void CApp::OnGlobalNewFolder() 
{
	CWslArray<CRuntimeClass*> aClasses;
	aClasses += RUNTIME_CLASS(CTIDrvFolder);

	CTIDrvFolder* pParent = (CTIDrvFolder*) WsfSelectTreeItem(IDS_SELECT_FOLDER, 
      IDS_SELECT_FOLDER_FOR_FOLDER,
		g_pApp->m_pTIDbRoot,
		&aClasses,
		WsfGetFrame());

	if (!pParent) return;

   CTIDrvFolder* p = CreateDriverFolder(pParent);
   ASSERT_VALID(p);
   EnsureTreeItemVisible(p);

   p->OnWsfOpen();
}

void CApp::OnUpdateGlobalNewFolder(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}

/*****************************************************************************/
void CApp::OnStmTest() 
{
   if (MAPI_Stm2008Probe() != -1)
   {
      AfxMessageBox(IDS_STM_TEST_ERROR, MB_ICONEXCLAMATION|MB_OK);
   }
   else
   {
      AfxMessageBox(IDS_STM_TEST_OK, MB_ICONINFORMATION|MB_OK);
   }
}

void CApp::OnUpdateStmTest(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
}
