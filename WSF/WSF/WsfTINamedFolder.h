///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

// Class implements a simple folder with name.

#pragma once

//*****************************************************************************
class WSL_API CWsfTINamedFolder: public CWsfTIFolder
{
protected:
	CString m_sName;

protected:
	CWsfTINamedFolder();			// Called from serialization only

public:
	CWsfTINamedFolder(const char* pcszName);
	CWsfTINamedFolder(const CString& sName);

	virtual CString GetTitle() const { return m_sName; }
	virtual void SetTitle(const char *pcszTitle) { m_sName = pcszTitle; }
	virtual void Serialize(CArchive &ar);

protected:
	DECLARE_SERIAL(CWsfTINamedFolder)
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CWsfTINamedFolder)
	//}}AFX_MSG
};












