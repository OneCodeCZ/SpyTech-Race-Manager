///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfHtmlView.h"
#include <mshtmcid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfHtmlView, CHtmlView)
	//{{AFX_MSG_MAP(CWsfHtmlView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_COMMAND(ID_WSF_REFRESH, OnWsfRefresh)
	ON_UPDATE_COMMAND_UI(ID_WSF_REFRESH, OnUpdateWsfRefresh)
	ON_WM_DESTROY()
	ON_COMMAND(ID_WSF_BROWSE_BACK, OnWsfBrowseBack)
	ON_UPDATE_COMMAND_UI(ID_WSF_BROWSE_BACK, OnUpdateWsfBrowseBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfHtmlView::CWsfHtmlView()
{
	//{{AFX_DATA_INIT(CWsfHtmlView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
CWsfHtmlView::~CWsfHtmlView()
{
}

/*****************************************************************************/
BOOL CWsfHtmlView::PreCreateWindow(CREATESTRUCT& cs) 
{
	BOOL b = CHtmlView::PreCreateWindow(cs);
	cs.dwExStyle |= WS_EX_CLIENTEDGE;

	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	return b;
}

/*****************************************************************************/
void CWsfHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWsfHtmlView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
void CWsfHtmlView::OnDestroy() 
{
	CHtmlView::OnDestroy();

	// NOTE: CHtmlView::OnDestroy() doesn't call the CView::OnDestroy()
	// which causes severe problems
	CFormView::OnDestroy();
}

/*****************************************************************************/
void CWsfHtmlView::OnInitToolbar(CWsfToolBar& tb)
{
   //tb.AddCmd(ID_WSF_BROWSE_BACK, CWsfToolBar::YES);
   //tb.AddCmd(ID_WSF_BROWSE_FORWARD, CWsfToolBar::YES);
   //tb.AddSeparator();
   tb.AddCmd(ID_FILE_PRINT, CWsfToolBar::YES);
   tb.AddCmd(ID_FILE_SAVE_AS, CWsfToolBar::YES);
   //tb.AddSeparator();
   //tb.AddCmd(ID_WSF_WINDOW_CLOSE, CWsfToolBar::YES);
}

/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfHtmlView::OnEditCopy() 
{
	ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

/*****************************************************************************/
void CWsfHtmlView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(QueryStatusWB(OLECMDID_COPY) & OLECMDF_ENABLED);
}

/*****************************************************************************/
void CWsfHtmlView::OnEditSelectAll() 
{
	ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

/*****************************************************************************/
void CWsfHtmlView::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(QueryStatusWB(OLECMDID_SELECTALL) & OLECMDF_ENABLED);
}

/*****************************************************************************/
void CWsfHtmlView::OnEditFind() 
{
	// Doesn't work very well
	//ExecWB((OLECMDID)IDM_FIND, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

/*****************************************************************************/
void CWsfHtmlView::OnUpdateEditFind(CCmdUI* pCmdUI) 
{
	// Doesn't work very well
	pCmdUI->Enable(false);
	//pCmdUI->Enable(QueryStatusWB((OLECMDID)IDM_FIND) & OLECMDF_ENABLED);
}

/*****************************************************************************/
void CWsfHtmlView::OnWsfRefresh() 
{
	Refresh();
}

/*****************************************************************************/
void CWsfHtmlView::OnUpdateWsfRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(QueryStatusWB(OLECMDID_REFRESH) & OLECMDF_ENABLED);
}

/*****************************************************************************/
void CWsfHtmlView::OnWsfBrowseBack() 
{
   this->GoBack();
}

/*****************************************************************************/
void CWsfHtmlView::OnUpdateWsfBrowseBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}
