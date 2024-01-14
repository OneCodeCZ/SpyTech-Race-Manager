// TestLabDoc.h : interface of the CTestLabDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTLABDOC_H__B4B91A6C_4A0E_4D0A_BB9F_79C4E4B2890C__INCLUDED_)
#define AFX_TESTLABDOC_H__B4B91A6C_4A0E_4D0A_BB9F_79C4E4B2890C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTestLabDoc : public CDocument
{
protected: // create from serialization only
	CTestLabDoc();
	DECLARE_DYNCREATE(CTestLabDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestLabDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestLabDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTestLabDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTLABDOC_H__B4B91A6C_4A0E_4D0A_BB9F_79C4E4B2890C__INCLUDED_)
