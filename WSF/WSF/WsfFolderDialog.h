///////////////////////////////////////////////////////////////////////////////
//
// CWsfFolderDialog
// ~~~~~~~~~~~~~~~~
// Copyright (C)2000 Radek Tetik / SpyTech
// Based on code from Mihai Filimon.
// Last updated: 2000-08-07
//
// Displays dialog for folder selection.
//
///////////////////////////////////////////////////////////////////////////////
#pragma once


//*****************************************************************************
class CWsfFolderDialog : public CFileDialog
{
public:
	CString  &m_strPath;

public:
	CWsfFolderDialog(CString &pPath);

protected:
	virtual void OnInitDone();

protected:
	//{{AFX_MSG(CWsfFolderDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
