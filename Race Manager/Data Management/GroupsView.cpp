///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "GroupsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CGroupsView, CFormView)
	//{{AFX_MSG_MAP(CGroupsView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_AUTO, OnAuto)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_LBN_SELCHANGE(IDC_GROUPS_LIST, OnSelchangeGroupsList)
	ON_BN_CLICKED(IDC_ADD_GROUP, OnAddGroup)
	ON_BN_CLICKED(IDC_REMOVE_GROUP, OnRemoveGroup)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_LBN_DBLCLK(IDC_MEMBERS_LIST, OnDblclkMembersList)
	ON_LBN_DBLCLK(IDC_UNDIVIDED_LIST, OnDblclkUndividedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CGroupsView::CGroupsView(CTIRace *pRace) 
	: CFormView(IDD), m_Race(*pRace)
{
	ASSERT_VALID(pRace);

	//{{AFX_DATA_INIT(CGroupsView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
CGroupsView::~CGroupsView()
{
}

/*****************************************************************************/
BOOL CGroupsView::PreCreateWindow(CREATESTRUCT& cs) 
{
	BOOL b = CFormView::PreCreateWindow(cs);
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return b;
}

//*****************************************************************************
void CGroupsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupsView)
	DDX_Control(pDX, IDC_GROUPS_LIST, m_GroupList);
	DDX_Control(pDX, IDC_UNDIVIDED_LIST, m_UndividedList);
	DDX_Control(pDX, IDC_MEMBERS_LIST, m_MemberList);
	//}}AFX_DATA_MAP
}

//*****************************************************************************
void CGroupsView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_Resize.Add(IDC_UNDIVIDED_LIST, 0,.5,0,1);

	m_Resize.Add(IDC_GROUPS_TEXT, .5,0,0,0);
	m_Resize.Add(IDC_GROUPS_LIST, .5,.5,0,.5);
	m_Resize.Add(IDC_ADD_GROUP, .75,0,.5,0);
	m_Resize.Add(IDC_REMOVE_GROUP, .75,0,.5,0);

	m_Resize.Add(IDC_MEMBERS_TEXT, .5,0,.5,0);
	m_Resize.Add(IDC_MEMBERS_LIST, .5,.5,.5,.5);
	m_Resize.Add(IDC_UP, .75,0,1,0);
	m_Resize.Add(IDC_DOWN, .75,0,1,0);

	m_Resize.Add(IDC_AUTO, .5,0,0,0);
	m_Resize.Add(IDC_PRINT, .5,0,0,0);
	m_Resize.Add(IDC_ADD, .5,0,.75,0);
	m_Resize.Add(IDC_REMOVE, .5,0,.75,0);
	m_Resize.Add(IDC_REMOVE_ALL, .5,0,.75,0);
}

//*****************************************************************************
void CGroupsView::OnSize(UINT nType, int cx, int cy) 
{
	m_Resize.Update(cx, cy);
	CFormView::OnSize(nType, cx, cy);
}

//*****************************************************************************
int CGroupsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1) return -1;
	
	m_Resize.SetParent(this);	// FormView has now size of the dlg template
	return 0;
}

//*****************************************************************************
void CGroupsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch (lHint)
	{
	case UV_GROUPS_HAS_CHANGED:
		// fall through

	// Sent by OnInitialUpdate
	case 0:
		BuildGroupList();
		BuildUndividedList();
		UpdateControls();
		break;
	}
}

//*****************************************************************************
CString CGroupsView::GetDispText(CTIRaceEntity *pRE)
{
	CString s;
	s.Format("%s (%d)", 
		pRE->GetEntityName(CTIRaceEntity::NICKNAME),
		pRE->m_nRanking);
	return s;
}

//*****************************************************************************
void CGroupsView::BuildGroupList()
{
	CString s;
	char chGroup = m_Race.m_nNumGroups + 'A' - 1;
	m_GroupList.ResetContent();

	for (int i=0; i < m_Race.m_nNumGroups; i++)
	{
		s.Format(IDS_LETTER_GROUP, chGroup--);
		m_GroupList.AddString(s);
	}

	m_GroupList.SetCurSel(0);
	OnSelchangeGroupsList();
}

//*****************************************************************************
void CGroupsView::BuildUndividedList()
{
	CTIMeasuredEntity **ppRE;
	int i, t;

	// Clear mark

	for (i = m_Race.m_aPartEntities.GetSize(); i--;)
	{
		m_Race.m_aPartEntities[i]->RemoveFlags(REF_MARK);
	}

	// Mark items that are part of some group

	for (i = m_Race.m_nNumGroups; i--;)
	{
		for (t = m_Race.m_aGroup[i].GetSize(); t--;)
		{
			m_Race.m_aGroup[i][t]->AddFlags(REF_MARK);
		}
	}

	// Fill list with unmarked entities

	m_UndividedList.ResetContent();
	ppRE = &m_Race.m_aPartEntities[0];

	for (i = m_Race.m_aPartEntities.GetSize(); i--; ppRE++)
	{
		if (!((*ppRE)->GetFlags() & REF_MARK))
		{
			t = m_UndividedList.AddString(GetDispText(*ppRE));
			m_UndividedList.SetItemDataPtr(t, *ppRE);
		}
	}
}

//*****************************************************************************
void CGroupsView::OnAuto() 
{
	if (AfxMessageBox(IDS_REALLY_AUTODIVIDE, MB_ICONQUESTION|MB_YESNO) == IDYES)
	{
		m_Race.BuildGroups();	// Function will update this view automaticly
	}
}

//*****************************************************************************
void CGroupsView::OnSelchangeGroupsList() 
{
	int i, nLen;
	CTIRaceEntity *pRE;
	
	// Get sel

	m_MemberList.ResetContent();
	
	i = m_GroupList.GetCurSel();
	if (i == LB_ERR) i = -1;
	m_nActiveGroup = i;

	UpdateControls();

	if (m_nActiveGroup == -1) return;

	// Fill member list with members of the active group

	nLen = m_Race.m_aGroup[m_nActiveGroup].GetSize();

	for (i=0; i<nLen; i++)
	{
		pRE = m_Race.m_aGroup[m_nActiveGroup][i];
		m_MemberList.AddString(GetDispText(pRE));
	}

	UpdateControls();
}

//*****************************************************************************
void CGroupsView::RemoveFromGroupList(bool bRemoveAll) 
{
	int i,t;
	CTIMeasuredEntity *pRE, *p;
	ASSERT(m_nActiveGroup >= 0 && m_nActiveGroup < m_Race.m_nNumGroups);

	// Removes items from group list, addes them to the undivided list
	// and removes items from "m_Race.m_aGroup[]" array

	for (i = m_MemberList.GetCount(); i--;)
	{
		if (bRemoveAll || m_MemberList.GetSel(i))
		{
			pRE = m_Race.m_aGroup[m_nActiveGroup][i];
			if (!bRemoveAll) m_MemberList.DeleteString(i);

			// Insert entity at the right position (sorted by ranking)

			for (t = m_UndividedList.GetCount(); t--;)
			{
				p = (CTIMeasuredEntity*) m_UndividedList.GetItemDataPtr(t);
				if (p->m_nRanking < pRE->m_nRanking) break;
			}

			ASSERT(pRE);
			m_UndividedList.InsertString(t+1, GetDispText(pRE));
			m_UndividedList.SetItemDataPtr(t+1, pRE);

			// Remove from group

			m_Race.m_aGroup[m_nActiveGroup].RemoveAt(i);
		}
	}

	if (bRemoveAll) m_MemberList.ResetContent();

	// Update stuff

	g_pApp->SetWorkspaceModified();
	UpdateControls();
}

//*****************************************************************************
void CGroupsView::OnRemoveAll() 
{
	RemoveFromGroupList(true);
}

//*****************************************************************************
void CGroupsView::OnRemove() 
{
	RemoveFromGroupList(false);
}

//*****************************************************************************
void CGroupsView::OnAdd() 
{
	CTIMeasuredEntity *pRE;
	ASSERT(m_nActiveGroup >= 0 && m_nActiveGroup < m_Race.m_nNumGroups);

	// Removes item from undivided list and adds it to the group list.
	// Then it adds race entity pointer to the "m_Race.m_aGroup[?]" array.

	for (int i = m_UndividedList.GetCount(); i--;)
	{
		if (m_UndividedList.GetSel(i))
		{
			pRE = (CTIMeasuredEntity*) m_UndividedList.GetItemDataPtr(i);
			m_UndividedList.DeleteString(i);

			m_MemberList.AddString(GetDispText(pRE));
			m_Race.m_aGroup[m_nActiveGroup].Append(pRE);
		}
	}

	// Update stuff

	g_pApp->SetWorkspaceModified();
	UpdateControls();
}

//*****************************************************************************
BOOL CGroupsView::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		WsfDisplayPopupHelp("GroupsView.txt", (HWND)pHelpInfo->hItemHandle);
	else 
		ASSERT(false);

	return true;
}

//*****************************************************************************
void CGroupsView::OnPrint() 
{
	MAPI_PrintGroups(&m_Race);
}

//*****************************************************************************
void CGroupsView::OnAddGroup() 
{
	CString s;

	// Insert new group at the begining

	for (int i = m_Race.m_nNumGroups - 1; i >= 0; i--)
	{
		m_Race.m_aGroup[i+1] = m_Race.m_aGroup[i];
	}

	m_Race.m_nNumGroups++;
	m_Race.m_aGroup[0].RemoveAll();

	// Add new list item
	
	s.Format(IDS_LETTER_GROUP, 	m_Race.m_nNumGroups + 'A' - 1);
	m_GroupList.InsertString(0, s);

	OnSelchangeGroupsList();
}

//*****************************************************************************
void CGroupsView::OnRemoveGroup() 
{
	// Delete selected group

	for (int i = m_GroupList.GetCurSel() + 1; i < m_Race.m_nNumGroups; i++)
	{
		m_Race.m_aGroup[i-1] = m_Race.m_aGroup[i];
	}
	
	m_Race.m_nNumGroups--;
	m_Race.m_aGroup[m_Race.m_nNumGroups].RemoveAll();

	// Delete list item
	
	m_GroupList.DeleteString(0);

	// Shift selection just to provide better UI feedback

	m_GroupList.SetCurSel(min(m_GroupList.GetCurSel()+1, m_Race.m_nNumGroups));

	// Update stuff

	BuildUndividedList();
	OnSelchangeGroupsList();
}

//*****************************************************************************
void CGroupsView::OnUp() 
{
	CTIMeasuredEntity *pRE;
	ASSERT(m_nActiveGroup >= 0 && m_nActiveGroup < m_Race.m_nNumGroups);
	CEntityArray &aGroup = m_Race.m_aGroup[m_nActiveGroup];
	
	for (int i = 1, n = m_MemberList.GetCount(); i < n; i++)
	{
		if (m_MemberList.GetSel(i))
		{
			pRE = aGroup[i];
			aGroup[i] = aGroup[i-1];
			aGroup[i-1] = pRE;

			m_MemberList.DeleteString(i);
			m_MemberList.InsertString(i-1, GetDispText(pRE));
			m_MemberList.SetSel(i-1);
		}
	}
}

//*****************************************************************************
void CGroupsView::OnDown() 
{
	CTIMeasuredEntity *pRE;
	ASSERT(m_nActiveGroup >= 0 && m_nActiveGroup < m_Race.m_nNumGroups);
	CEntityArray &aGroup = m_Race.m_aGroup[m_nActiveGroup];
	
	for (int i = m_MemberList.GetCount() - 2; i >= 0; i--)
	{
		if (m_MemberList.GetSel(i))
		{
			pRE = aGroup[i];
			aGroup[i] = aGroup[i+1];
			aGroup[i+1] = pRE;

			m_MemberList.DeleteString(i);
			m_MemberList.InsertString(i+1, GetDispText(pRE));
			m_MemberList.SetSel(i+1);
		}
	}
}

//*****************************************************************************
void CGroupsView::UpdateControls()
{
	bool b, bAnyIsActive = m_nActiveGroup != -1;

	m_MemberList.EnableWindow(bAnyIsActive);

	b = m_MemberList.GetCount() > 0;
	GetDlgItem(IDC_REMOVE)->EnableWindow(b);
	GetDlgItem(IDC_REMOVE_ALL)->EnableWindow(b);
	GetDlgItem(IDC_UP)->EnableWindow(b && bAnyIsActive);
	GetDlgItem(IDC_DOWN)->EnableWindow(b && bAnyIsActive);

	b = (m_UndividedList.GetCount() > 0) && bAnyIsActive;
	GetDlgItem(IDC_ADD)->EnableWindow(b);

	GetDlgItem(IDC_ADD_GROUP)->EnableWindow(m_Race.m_nNumGroups < MAX_GROUPS);
	GetDlgItem(IDC_REMOVE_GROUP)->EnableWindow(m_Race.m_nNumGroups > 0 && bAnyIsActive);
}

//*****************************************************************************
void CGroupsView::OnFilePrint() 
{
	OnPrint();
}

//*****************************************************************************
void CGroupsView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CGroupsView::OnDblclkMembersList() 
{
	OnRemove();
}

//*****************************************************************************
void CGroupsView::OnDblclkUndividedList() 
{
	if (m_nActiveGroup != -1) OnAdd();
}

