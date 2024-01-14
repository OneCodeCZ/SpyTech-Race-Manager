///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfTINamed : public CWsfTreeItem
{
private:
	CString		m_sName;

protected:
	CWsfTINamed();			// Can be called from serialization only
public:
	CWsfTINamed(const char* pcszName);
	CWsfTINamed(const CString& sName);

	// From WsfTreeItem

	virtual void Serialize(CArchive &ar);
	virtual CString GetTitle() const { return m_sName; }
	virtual void SetTitle(const char *pcszTitle) { m_sName = pcszTitle; }

protected:
	DECLARE_SERIAL(CWsfTINamed)
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CWsfTINamed)
	//}}AFX_MSG
};












