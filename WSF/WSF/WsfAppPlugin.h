///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// Generally the main application DLL or EXE file contains a CWsfApp 
// derivate class CApp. This class is a root of the message handling mechanism
// and receives important event notifications regarding the UI. 
// CWsfPlugin class enables DLLs to hook into the message handling and event
// handling of the CWsfApp class. 

#pragma once

/*****************************************************************************/
class WSL_API CWsfPlugin : public CCmdTarget
{
protected: 
   CString  m_sName;

public:
	CWsfPlugin();
	virtual ~CWsfPlugin();

   void SetName(const char* pcszName) { m_sName = pcszName; }
   const CString& GetName() const { return m_sName; }

	// Called from the CWsfApp::InitInstance() resp. CWsfApp::ExitInstance()
	virtual BOOL OnInitInstance();
	virtual void OnExitInstance();

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfPlugin)
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CWsfPlugin)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/*****************************************************************************/
typedef CWslArray<CWsfPlugin*> CWsfPluginsArray;


