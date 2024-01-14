///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfCmd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//*****************************************************************************
CWsfCmd::CWsfCmd(int nId, const char* pcszTextId, BOOL bShowText, BOOL bShowImage)
{
	ASSERT(nId>0 && nId<65535);
	m_nId = nId;
	m_bShowText = bShowText;
	m_bShowImage = bShowImage;
	m_sTextId = pcszTextId;

   LoadTextAndAccel();
}

/*****************************************************************************/
CWsfCmd::CWsfCmd(const CWsfCmd* pCmd, int nNewId, const char* pcszNewTextId)
{
   ASSERT(pCmd);

   *this = *pCmd;
   m_nId = nNewId;
   m_sTextId = pcszNewTextId;
}

//*****************************************************************************
CWsfCmd::~CWsfCmd()
{
}

//*****************************************************************************
void CWsfCmd::InitSimple(int nImgIdx)
{
	m_nType = SIMPLE;
	m_nImgIdx = nImgIdx;
}

//*****************************************************************************
void CWsfCmd::InitComboBox(CRuntimeClass* pComboClass, int nWidth)
{
	// params check is left up to CWsfApp
	m_nType = COMBO_BOX;
	m_nImgIdx = -1;
	m_pClass = pComboClass;
	m_nWidth = nWidth;
}

//*****************************************************************************
void CWsfCmd::InitDropDown(int nDefCmdId, HMENU hMenu)
{
	// params check is left up to CWsfApp
	m_nType = DROP_DOWN_MENU;
	m_nImgIdx = -1;
	m_hMenu	= hMenu;
	m_nDefCmdId = nDefCmdId;
	m_bSeparateArrow = true;
}

/*****************************************************************************/
void CWsfCmd::InitDropDownOnly(int nImgIdx, HMENU hMenu)
{
	// params check is left up to CWsfApp
	m_nType = DROP_DOWN_MENU_ONLY;
	m_nImgIdx = nImgIdx;
	m_hMenu	= hMenu;
	m_bSeparateArrow = false;
}

//*****************************************************************************
void CWsfCmd::InitDropDown(int nImgIdx, CRuntimeClass* pWndClass)
{
	// params check is left up to CWsfApp
	m_nType = DROP_DOWN_CUSTOM;
	m_nImgIdx = nImgIdx;
	m_pClass = pWndClass;
}

/*****************************************************************************/
CString CWsfCmd::GetTextAndAccel() const
{
   CString s;
   s.Format("%s\t%s", m_sText, m_sAccel);
   return s;
}

/*****************************************************************************/
CString CWsfCmd::GetToolTip() const
{
   CString s;

   if (m_sAccel.IsEmpty())
      s = m_sText;
   else
      s.Format("%s (%s)", m_sText, m_sAccel);

   return s;
}

/*****************************************************************************/
void CWsfCmd::LoadTextAndAccel()
{
	CString s;
	s.LoadString(m_nId);
   if (s.IsEmpty()) return;

   // Get desc

	int n = s.Find('\n');
   if (n==-1) { ASSERT(false); return; }  // Missing command description
   m_sDesc = s.Left(n);

   // Get title & accel

   int n2 = s.Find('\t');
   
   if (n2==-1) 
   {
      m_sText = s.Mid(n+1, s.GetLength()-n-1);
   }
   else
   {
      m_sText = s.Mid(n+1, n2-n-1);
      m_sAccel = s.Right(s.GetLength()-n2-1);
   }
}



