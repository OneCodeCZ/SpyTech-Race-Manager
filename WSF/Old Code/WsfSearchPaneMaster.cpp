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

#include "stdafx.h"
#include "main.h"
#include "WsfSearchPaneMaster.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfSearchPaneClient::CWsfSearchPaneClient(CView* pClientView,
                                             CRuntimeClass* pSearchViewClass,
                                             int nHeight)
{
	ASSERT(pSearchViewClass);
	m_pSearchViewClass = pSearchViewClass;
	m_pClientView = pClientView;
	m_bSearchPane = false;
	m_nHeight = nHeight;
}

/*****************************************************************************/
CWsfSearchPaneClient::CWsfSearchPaneClient()
{
	m_pSearchViewClass = NULL;
	m_pClientView = NULL;
	m_bSearchPane = false;
	m_nHeight = 0;
}

/*****************************************************************************/
void CWsfSearchPaneClient::Construct(CView* pClientView, 
                                      CRuntimeClass* pSearchViewClass, 
                                      int nHeight)
{
	ASSERT(pSearchViewClass);
	m_pSearchViewClass = pSearchViewClass;
	m_pClientView = pClientView;
	m_bSearchPane = false;
	m_nHeight = nHeight;
}

/*****************************************************************************/
CWsfSearchPaneClient::~CWsfSearchPaneClient()
{

}

/*****************************************************************************/
void CWsfSearchPaneClient::ShowSearchPane(BOOL bShow)
{
   ASSERT(m_pClientView);
   ASSERT(m_pSearchViewClass);

	if (bShow)
	{
		if (!m_bSearchPane)
		{
			m_pSearchView = (CView*) m_pSearchViewClass->CreateObject();
         
         CWsfSearchPane* psp = dynamic_cast<CWsfSearchPane*>(m_pSearchView);
         ASSERT(psp);   // Your search pane view must inherit from CWsfSearchPane!!!
         psp->SetClient(this);

			WsfGetApp()->OpenSlaveView(m_pSearchView, WsfGetWorkspace(), 
				m_pClientView, false, m_nHeight);
			m_bSearchPane = true;
		}
		else
		{
//			m_pSearchView->OnInitialUpdate();
		}
		WsfGetApp()->ActivateView(m_pSearchView);
	}
	else
	{
		if (m_bSearchPane) 
		{
			WsfGetApp()->CloseView(m_pSearchView, true);
			m_bSearchPane = false;
		}
	}
}

/*****************************************************************************/
void CWsfSearchPaneClient::SetHeight(int nHeight)
{
	ASSERT(!IsSearchPaneVisible()); // TODO:
	m_nHeight = nHeight;
}

/*****************************************************************************/
int CWsfSearchPaneClient::GetHeight()
{
	ASSERT(!IsSearchPaneVisible()); // TODO:
	return m_nHeight;
}


