///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 07
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfTableCtrl.h"
#include "WsfTableView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
CWsfTableCtrl2::CUpdateManager::CUpdateManager()
{
   Clear();
}

/*****************************************************************************/
void CWsfTableCtrl2::CUpdateManager::Clear()
{
	m_aUpdateRequests.RemoveAll();
	m_bRebuild = false;
	m_nSetSelPrimaryKey = -1;
}

/*****************************************************************************/
void CWsfTableCtrl2::CUpdateManager::AddAction(EActions eAction, int nKey)
{
	if (m_bRebuild) return;

	if (eAction != REBUILD) 
	{	
		m_aUpdateRequests.GrowBy(1);
		CUpdateRequest& ui = m_aUpdateRequests.GetLast();
		ui.eAction = eAction;
		ui.nPrimaryKey = nKey;
	} 
	else 
	{
		m_bRebuild = true;
		m_aUpdateRequests.RemoveAll();
	}
}

/*****************************************************************************/
void CWsfTableCtrl2::CUpdateManager::AddAction(const CWsfDbEvent& e)
{
   CUpdateManager::EActions eAction;

	switch (e.eEvent)
	{
   default : ASSERT(false); /* fall-through */
   case CWsfDbEvent::GENERAL     : eAction = CUpdateManager::REBUILD; break;
	case CWsfDbEvent::ROW_ADDED	: eAction = CUpdateManager::ADD_ROW; break;
	case CWsfDbEvent::ROW_UPDATED	: eAction = CUpdateManager::UPDATE_ROW; break;
	case CWsfDbEvent::ROW_DELETED	: eAction = CUpdateManager::DELETE_ROW; break;
   }

   AddAction(eAction, e.nPrimaryKey);
}

/*****************************************************************************/
void CWsfTableCtrl2::CUpdateManager::AddSelectSingleRow(int nKey)
{
	m_nSetSelPrimaryKey = nKey;
}

/*****************************************************************************/
void CWsfTableCtrl2::CUpdateManager::Refresh()
{
	m_bRebuild = true;
	m_aUpdateRequests.RemoveAll();
}

/*****************************************************************************/
void CWsfTableCtrl2::CUpdateManager::Update(CWsfTableCtrl2& tbl)
{
	// Complete rebuild or single row operations

	if (m_bRebuild)
	{
      ASSERT(!m_aUpdateRequests.GetSize());
		tbl.FillTable(true);
	}
   else
   {
	   for (int i=0; i<m_aUpdateRequests.GetSize(); i++)
	   {
		   switch (m_aUpdateRequests[i].eAction)
		   {
		   case ADD_ROW:
			   tbl.AddUpdateRow(m_aUpdateRequests[i].nPrimaryKey, false);
			   break;

		   case UPDATE_ROW:
			   tbl.AddUpdateRow(m_aUpdateRequests[i].nPrimaryKey, true);
			   break;
			   
		   case DELETE_ROW:
			   {
				   int n = tbl.GetRowIdxById(m_aUpdateRequests[i].nPrimaryKey);
				   if (n != -1) tbl.DeleteRow(n);
			   }
			   break;
		   }
	   }
   }

	// Selection

	if (m_nSetSelPrimaryKey != -1) 
	{
		int n = tbl.GetRowIdxById(m_nSetSelPrimaryKey);
		if (n != -1) tbl.SelectSingleRow(n, true);
	}

	// Clear

	Clear();
	tbl.SetMessageString();
}

