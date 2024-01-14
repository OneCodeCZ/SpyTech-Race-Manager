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
class CTIResultsFolder : public CWsfTIFolder
{
protected:
	CString m_sName;

public:
	CTIResultsFolder();

	//---------------------------------------------------------------------------
	// From CWsfTreeItem
	//---------------------------------------------------------------------------

	virtual CString GetTitle() const { return m_sName; }

protected:
	DECLARE_SERIAL(CTIResultsFolder);
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIResultsFolder)
	afx_msg void OnUpdateWsfProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfOpen(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG
};

