///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfQueryDoc;

//*****************************************************************************
class WSL_API CWsfTIQuery : public CWsfTreeItem
{
friend CWsfQueryDoc;
friend CWsfQueryView;

private:
	// Query view profile
	BOOL		m_bUseFilter;
	void*		m_pDocData;				
	int			m_nDocDataSize;

public:
	CWsfTIQuery();	
	virtual ~CWsfTIQuery();

	// Standard OnOpen() implementation. 
	// It opens the given document and the query view.
	// If something fails the document is deleted.
	//BOOL StandardOnOpenImpl(CRuntimeClass* pDocClass);

	// Loads/saves the query view profile.
	// Call this from CWsfQueryDoc's OnLoadProfile and OnSaveProfile.
	BOOL LoadProfile(void **ppData, int *pnSize);
	BOOL SaveProfile(void *pData, int nSize);

	//---------------------------------------------------------------------------	
	// From WsfTreeItem
	//---------------------------------------------------------------------------	

	virtual void Serialize(CArchive &ar);
	virtual int GetDefaultCommand() { return ID_WSF_OPEN; }

protected:
	WSL_DECLARE_SERIAL_DLL(WSL_API, CWsfTIQuery)
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CWsfTIQuery)
	//}}AFX_MSG
};


