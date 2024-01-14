///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfCmd;
class CWsfApp;

//*****************************************************************************
class WSL_API CWsfToolBar : public CControlBar
{
public:
   enum EShow { DEFAULT=2, YES=1, NO=0 };
   
protected:
	// A command on the toolbar (button, combo, etc.)
	class CToolBarCmd
	{
   public:
		const CWsfCmd*	pCmd;			// WSF Command
		int				nCurCmdId;	// The ID can change e.g. drop down command
		CObject*			pObject;    // object associated with the command
											// e.g. child window like combo-box
	};

	// Represents a classic toolbar button
	class CToolCmdUI : public CCmdUI        
	{
   protected:
      BOOL  m_bCheckChanged;
      BOOL  m_bEnabled;
      int   m_nCheck;

	public:
      CToolCmdUI();

      // State changers
		virtual void Enable(BOOL bOn);
		virtual void SetCheck(int nCheck);
		virtual void SetText(LPCTSTR lpszText);

      // State changes are delayed and performed by this method
      // on the end of the UI update procedure. This avoids flicker.
      virtual void PerformChanges();
	};

	// Represents a toolbar item that is a child window.
	// (e.g. combo-box)
	// m_pOther member represents the child window
	class CWindowToolCmdUI : public CToolCmdUI        
	{
	public:
      virtual void PerformChanges();
	};

protected:
	CWslArray<CToolBarCmd> m_aCmds;

public:
	//---------------------------------------------------------------------------
	// Construction
	//---------------------------------------------------------------------------

	CWsfToolBar();
	virtual ~CWsfToolBar();

	// Creates the toolbar window.
	BOOL Create(CWnd* pParent, DWORD dwStyle, int nID);

   // Creates the toolbar supposing it will be used as a control in a dialog
   // If the nId control already exists. The toolbar will have 
   // its size and position, and the control will be destroyed.
   BOOL CreateControl(CWnd* pParent, int nId);

	// Sets toolbar bitmaps to bitmaps of the commands.
	void SetCmdBitmaps();

	//---------------------------------------------------------------------------
	// Commands
	//---------------------------------------------------------------------------

   void AddCmd(int nCmdId, 
	   EShow eShowText = DEFAULT, 
      int nPos = -1);

	void AddSeparator(int nPos = -1);
   
   int IsEmpty();
   void Empty();

	//---------------------------------------------------------------------------
	// From CControlBar
	//---------------------------------------------------------------------------

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void SetOwner(CWnd* pOwnerWnd);
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz );
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode );

	//---------------------------------------------------------------------------
	// ToolBar control wrappers
	//---------------------------------------------------------------------------
protected:
	int CmdToIdx(int nID) { return DefWindowProc(TB_COMMANDTOINDEX, nID, 0); }
	int GetButtonCount() { return DefWindowProc(TB_BUTTONCOUNT, 0,0); }
	BOOL GetButton(int nIdx, TBBUTTON *pButton) { return DefWindowProc(TB_GETBUTTON, nIdx, (LPARAM) pButton); }
	BOOL GetButtonRect(int nIdx, RECT *pRect) { return DefWindowProc(TB_GETITEMRECT, nIdx, (LPARAM) pRect); }

	//---------------------------------------------------------------------------
	// Various stuff
	//---------------------------------------------------------------------------

	void OnDropDown(NMHDR *p, LRESULT *pResult);
	void RecalcChildLayout(int nFromIdx);

	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfToolBar)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};





