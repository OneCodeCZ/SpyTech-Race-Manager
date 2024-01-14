#if !defined(AFX_PGENTITYDEBUG_H__FA4AE840_78D4_11D4_B035_0000B49E3BD7__INCLUDED_)
#define AFX_PGENTITYDEBUG_H__FA4AE840_78D4_11D4_B035_0000B49E3BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgEntityDebug.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgEntityDebug dialog

class CPgEntityDebug : public CWsfPropertyPage
{
// Construction
public:
	CTIMeasuredEntity *m_pEntity;
	CPgEntityDebug();
	CPgEntityDebug(CTIMeasuredEntity *pEntity);
	~CPgEntityDebug();

// Dialog Data
	//{{AFX_DATA(CPgEntityDebug)
	enum { IDD = IDD_ENTITY_DEBUG };
	CListBox	m_wndList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgEntityDebug)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPgEntityDebug)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGENTITYDEBUG_H__FA4AE840_78D4_11D4_B035_0000B49E3BD7__INCLUDED_)
