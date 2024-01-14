/******************
*                 *  DlgHdts.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include "DlgHdts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************************
*
*  CDlgHdts
*
*******************************************************************************/

CDlgHdts::CDlgHdts(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHdts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHdts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	for (int a=0; a<MAXRACERS; a++) {
		m_dwArrNumHdts[a]=0;
		m_dwArrNumLaps[a]=0;
		m_sArrNames[a]="\0";
	}
}

/******************************************************************************/

void CDlgHdts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/******************************************************************************/

BEGIN_MESSAGE_MAP(CDlgHdts, CDialog)
	//{{AFX_MSG_MAP(CDlgHdts)
	ON_WM_DESTROY()
	ON_COMMAND(IDOK, OnOK)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int nYOffset = 12;

/*******************************************************************************
*
*  HANDLERS
*
*******************************************************************************/

int CDlgHdts::DoModal(const char *pcszCaption, int nNumRacers) {
	m_bQualification = FALSE;
	if (nNumRacers == -1) {
		m_bQualification = TRUE;
		m_dwNumRacers = 1;
	}
	else m_dwNumRacers = nNumRacers;
	m_sCaption=pcszCaption;
	return CDialog::DoModal();
}

/******************************************************************************/

BOOL CDlgHdts::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pArrControls.SetSize(0);

	char temp[1024];
	CEdit *pEdit;
	CStatic *pStatic;

	SetWindowText(m_sCaption);

   CString s;

	pStatic=new CStatic;
   s.LoadString(IDS_TRACK_SEGMENTS);
	pStatic->Create(s, WS_CHILD | WS_VISIBLE, CRect(208, nYOffset, 208+32, nYOffset+8+20), this);
	pStatic->SetFont(gdi.GetFont(12));
	m_pArrControls.Append(pStatic);

	pStatic=new CStatic;
   s.LoadString(IDS_PENALTY_ABRV);
	pStatic->Create(s, m_bQualification ? WS_CHILD : WS_CHILD | WS_VISIBLE, CRect(248, nYOffset, 248+32, nYOffset+8+20), this);
	pStatic->SetFont(gdi.GetFont(12));
	m_pArrControls.Append(pStatic);

	for (int a=0; a<m_dwNumRacers; a++) {
		pStatic=new CStatic;
		pStatic->Create(LPCTSTR(m_sArrNames[a]), WS_CHILD | WS_VISIBLE, CRect(16+24, nYOffset+20+a*24, 16+124, nYOffset+16+a*24+20), this);
		pStatic->SetFont(gdi.GetFont(12));
		m_pArrControls.Append(pStatic);

		itoa(m_dwArrNumLaps[a], temp, 10);
		pStatic=new CStatic;
		pStatic->Create(temp, WS_CHILD | WS_VISIBLE, CRect(140+16, nYOffset+20+a*24, 160+24+16, nYOffset+16+a*24+20), this);
		pStatic->SetFont(gdi.GetFont(12));
		m_pArrControls.Append(pStatic);

		pEdit=new CEdit;
		pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), "0", WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(208, nYOffset+16+a*24, 208+32, nYOffset+16+a*24+20), this, 16+a);
		pEdit->SetFont(gdi.GetFont(12));
		m_pArrControls.Append(pEdit);

		CString s;
		s.Format("%d", m_nArrPenalization[a]);
		pEdit=new CEdit;
		pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), s, m_bQualification ? WS_CHILD : WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(248, nYOffset+16+a*24, 248+32, nYOffset+16+a*24+20), this, 32+a);
		pEdit->SetFont(gdi.GetFont(12));
		m_pArrControls.Append(pEdit);
	}

	CButton *pButton;
	pButton=new CButton;
   s.LoadString(IDS_CONFIRM);
	pButton->Create(s, WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(90+20, nYOffset+24+m_dwNumRacers*24, 165+20, nYOffset+24+m_dwNumRacers*24+22), this, IDOK);
	pButton->SetFont(gdi.GetFont(12));
	m_pArrControls.Append(pButton);

	SetWindowPos(NULL, 0, 0, 300, nYOffset+16+m_dwNumRacers*24+64, SWP_NOZORDER | SWP_NOMOVE);
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************/

void CDlgHdts::OnDestroy() 
{
	int nIdx;
	ITERATE(m_pArrControls, nIdx) {
		m_pArrControls[nIdx]->DestroyWindow();
	}
	CDialog::OnDestroy();
}

/******************************************************************************/

void CDlgHdts::OnOK()
{
	if (AfxMessageBox(IDS_ARE_DATA_OK, MB_YESNO)==IDNO) return;

	char temp[1024];
	int a;
	for (a=0; a<m_dwNumRacers; a++) {
		(CEdit*)(GetDlgItem(a+16))->GetWindowText(temp, 1024);
		CString s;
		for (int b=0; b<strlen(temp); b++) if (temp[b]=='-' || (temp[b]>='0' && temp[b]<='9')) s+=temp[b];
		m_dwArrNumHdts[a]=atoi(s);
	}
	for (a=0; a<m_dwNumRacers; a++) {
		(CEdit*)(GetDlgItem(a+32))->GetWindowText(temp, 1024);
		CString s;
		for (int b=0; b<strlen(temp); b++) if (temp[b]=='-' || (temp[b]>='0' && temp[b]<='9')) s+=temp[b];
		m_nArrPenalization[a]=atoi(s);
	}
	EndDialog(IDOK);
}

/******************************************************************************/

int CDlgHdts::PrepareData(CEntityArray &rArray, CTIMeasuredRace *pRace) {
	m_dwNumRacers = rArray.GetSize();
	for (int a=0; a<m_dwNumRacers; a++) {
		CTIMeasuredEntity *pEntity = DC_ME(rArray[a]);
		m_sArrNames[a] = pEntity->GetEntityName(CTIRaceEntity::NICKNAME);
		m_dwArrNumLaps[a] = pEntity->GetCurrentEvent()->GetTotalLaps();
		m_dwArrNumHdts[a] = 0;
		m_nArrPenalization[a] = pEntity->GetCurrentEvent()->m_nPenalization;
		if (pEntity->m_nCurrentTrack != -1) {
			m_crArrColors.Append(pRace->GetLaneColor(pEntity->m_nCurrentTrack));
		}
		else {
			m_crArrColors.Append(-1);
		}
	}
	return 1;
}

/******************************************************************************/

int CDlgHdts::GatherData(CEntityArray &rArray) {
	for (int a=0; a<m_dwNumRacers; a++) {
		CTIMeasuredEntity *pEntity = DC_ME(rArray[a]);
		pEntity->GetCurrentEvent()->GetCurrentRide()->m_dwNumHdts = m_dwArrNumHdts[a];
		pEntity->GetCurrentEvent()->m_nPenalization = m_nArrPenalization[a];
	}
	return 1;
}

/******************************************************************************/

void CDlgHdts::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen *old_pen = dc.SelectObject(&pen);

	for (int a=0; a<m_dwNumRacers; a++) {

		if (m_crArrColors[a] != -1) {
			int x1 = 16;
			int y1 = nYOffset+20+a*24;
			int x2 = x1 + 12;
			int y2 = y1 + 12;

			CRect rect = CRect(x1, y1, x2, y2);
			dc.FillSolidRect(LPRECT(rect), m_crArrColors[a]);

			dc.MoveTo(x1, y1);
			dc.LineTo(x2, y1);
			dc.LineTo(x2, y2);
			dc.LineTo(x1, y2);
			dc.LineTo(x1, y1);
		}
	}

	dc.SelectObject(old_pen);
	pen.DeleteObject();
}
/*********************************************************************** eof **/
