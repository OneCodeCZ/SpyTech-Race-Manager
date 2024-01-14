///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class CTIDB : public CWsfTINamedFolder
{
public:
	CTIDB(); 
	virtual ~CTIDB();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTIDB)
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CTIDB)
	afx_msg void OnWsfOpen();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_SERIAL(CTIDB);
};
