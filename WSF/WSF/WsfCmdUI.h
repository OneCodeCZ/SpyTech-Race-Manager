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
// CWsfCmdUI - used to test for disabled commands

class CWsfCmdUI : public CCmdUI
{
protected:
	CCmdUI *m_pCmdUI;
	BOOL m_bEnabled;

public:
	CWsfCmdUI(CCmdUI *pCmdUI) { m_pCmdUI = pCmdUI; m_bEnabled = TRUE; }
	inline BOOL IsEnabled() const { return m_bEnabled; }

	virtual void Enable(BOOL bOn) { m_bEnabled = bOn; if (m_pCmdUI) m_pCmdUI->Enable(bOn); }
	virtual void SetCheck(int nCheck) { if (m_pCmdUI) m_pCmdUI->SetCheck(nCheck); };
	virtual void SetRadio(BOOL bOn) { if (m_pCmdUI) m_pCmdUI->SetRadio(bOn); };
	virtual void SetText(LPCTSTR pcszText) { if (m_pCmdUI) m_pCmdUI->SetText(pcszText); };
};

//*****************************************************************************
// CWsfTestCmdUI - used to test for disabled commands

class CWsfTestCmdUI : public CCmdUI
{
protected:
	BOOL m_bEnabled;

public:
	CWsfTestCmdUI(int nCmdId) { m_nID = nCmdId; m_bEnabled = TRUE; }
	inline BOOL IsEnabled() const { return m_bEnabled; }

	virtual void Enable(BOOL bOn) { m_bEnabled = bOn; m_bEnableChanged = TRUE; }
	virtual void SetCheck(int nCheck) { };
	virtual void SetRadio(BOOL bOn) { };
	virtual void SetText(LPCTSTR) { };
};
