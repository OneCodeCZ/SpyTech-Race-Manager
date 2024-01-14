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
class CTIRSFolder : public CWsfTINamedFolder
{
protected:
	CTIRSFolder();			

public:
	CTIRSFolder(const char *pcszTitle);

	// Finds RS by the given name.
	// (case insensitive search)
	CTIRS *FindRS(CString &strName);
  
	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual int OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2);

protected:

	DECLARE_SERIAL(CTIRSFolder);
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIRSFolder)
	afx_msg void OnNewRs();
	afx_msg void OnUpdateNewRs(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG
};
