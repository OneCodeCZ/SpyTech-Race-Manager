///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 06
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "PgSetupSound.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgSetupSound, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgSetupSound)
	ON_BN_CLICKED(IDC_TEST_SOUND, OnTestSound)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*****************************************************************************/
CPgSetupSound::CPgSetupSound() : CWsfPropertyPage(CPgSetupSound::IDD)
{
	//{{AFX_DATA_INIT(CPgSetupSound)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
CPgSetupSound::~CPgSetupSound()
{
}

/*****************************************************************************/
void CPgSetupSound::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgSetupSound)
	DDX_Control(pDX, IDC_SELECT_SOUND_CARD, m_SelectSoundCard);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
BOOL CPgSetupSound::OnInitDialog() 
{
	CWsfPropertyPage::OnInitDialog();

   DWORD dwNumDevices = waveOutGetNumDevs();

   for (DWORD id=0; id<dwNumDevices; id++)
   {
      WAVEOUTCAPS woc;
      MMRESULT res = waveOutGetDevCaps(id, &woc, sizeof(WAVEOUTCAPS));

      if (res == MMSYSERR_NOERROR)
      {
         int nItem = m_SelectSoundCard.AddString(woc.szPname);
         m_SelectSoundCard.SetItemData(nItem, id);
      }
   }

   for (int a=0; a<m_SelectSoundCard.GetCount(); a++) 
   {
      int nId = m_SelectSoundCard.GetItemData(a);
      if (nId == WsfGetCApp()->m_nWaveOutDevId) {
         m_SelectSoundCard.SetCurSel(a);
         break;
      }
   }
   
	return TRUE;
}

/*****************************************************************************/
BOOL CPgSetupSound::OnOkButton()
{
   int nItem = m_SelectSoundCard.GetCurSel();
   if (nItem != -1) {
      int nId = m_SelectSoundCard.GetItemData(nItem);
      WsfGetCApp()->m_nWaveOutDevId = nId;
   }

   return true;
}

/*****************************************************************************/
BOOL CPgSetupSound::OnKillActive(int &nNewPage)
{
   return true;
}

/*****************************************************************************/
void CPgSetupSound::OnTestSound() 
{
   UpdateData();
   
   int nItem = m_SelectSoundCard.GetCurSel();
   int nId = m_SelectSoundCard.GetItemData(nItem);

   SetTimer(0, 3000, NULL);
   GetDlgItem(IDC_TEST_SOUND)->EnableWindow(FALSE);
   m_pps->EnableOkDoneNextButton(FALSE);
   m_pps->EnableCancelButton(FALSE);

   CWaitCursor wc;
   MAPI_TestSound(nId);
}

/*****************************************************************************/
void CPgSetupSound::OnTimer(UINT nIDEvent) 
{
   KillTimer(0);
   GetDlgItem(IDC_TEST_SOUND)->EnableWindow(TRUE);
   m_pps->EnableOkDoneNextButton(TRUE);
   m_pps->EnableCancelButton(TRUE);

	CWsfPropertyPage::OnTimer(nIDEvent);
}
