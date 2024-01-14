///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class WSL_API CWsfSearchPaneClient
{
private:
	BOOL     m_bSearchPane;
	CView*	m_pSearchView;
	CView*	m_pClientView;
	int		m_nHeight;
	CRuntimeClass*	m_pSearchViewClass;

public:
	// The client view is used to display search results.
	// When the pane is to be visible, the pSearchViewClass view is created
	// and displayed as a slave view of the given master view.
	CWsfSearchPaneClient(CView* pClientView,
		CRuntimeClass* pSearchViewClass,
		int nHeight);

   // Two step construction
	CWsfSearchPaneClient();
   void Construct(CView* pClientView, CRuntimeClass* pSearchViewClass, int nHeight);

	virtual ~CWsfSearchPaneClient();

	// Shows/hides the search pane.
	// bShow is TRUE: 
	// - if the pane is already visible, it's activated.
	// - if the pane is hidden, the view is created and activated.
	// bShow is FALSE:
	// - if the pane is visible, the view is closed
	void ShowSearchPane(BOOL bShow);

	// Returns TRUE if the search pane is visible
	inline BOOL IsSearchPaneVisible() const { return m_bSearchPane; }

	// Sets the pane height.
	// If the pane is visible the height is udpated immediatelly.
	void SetHeight(int nHeight);

	// Returns the pane height.
	// If the pane is not visible, the height at the time it has been last opened
	// is returned.
	int GetHeight();

   // Returns the client view (search results)
   CView* GetClientView() { return m_pClientView; }
};

/*****************************************************************************/
class WSL_API CWsfSearchPane
{
private:
   CWsfSearchPaneClient*  m_pClient;

public:
   CWsfSearchPane() { m_pClient = NULL; }

   void SetClient(CWsfSearchPaneClient* pClient) { ASSERT(pClient); m_pClient = pClient; }
   CWsfSearchPaneClient* GetClient() { ASSERT(m_pClient); return m_pClient; }

   CView* GetClientView() { return GetClient()->GetClientView(); }

   void HideSearchPane() { GetClient()->ShowSearchPane(false); }
};
