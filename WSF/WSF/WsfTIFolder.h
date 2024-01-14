///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfTIFolder: public CWsfTreeItem
{
protected:
	int		m_nViewID;	// ID of a view of this item.

public:
	CWsfTIFolder();			

	// From WsfTreeItem
	virtual int GetDefaultCommand() { return ID_WSF_OPEN; }
	virtual BOOL OnDelete(DWORD dwData);

protected:
	DECLARE_SERIAL(CWsfTIFolder)
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CWsfTIFolder)
	afx_msg void OnUpdateWsfOpen(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG
};












