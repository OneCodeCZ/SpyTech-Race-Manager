///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfHtmlDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfHtmlDoc, CDocument)
	//{{AFX_MSG_MAP(CWsfHtmlDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfHtmlDoc::CWsfHtmlDoc(BOOL bDeleteFileOnClose)
{
  m_bDeleteFileOnClose = bDeleteFileOnClose;
}

/*****************************************************************************/
CWsfHtmlDoc::~CWsfHtmlDoc()
{
  // NOTE: Should be in OnCloseDocument() method.
  try
  {
    if (m_bDeleteFileOnClose) CWslFile::Delete(GetPathName());
  }
  catch (CException* e)
  {
    e->Delete();
    ASSERT(false);
  }
}

/*****************************************************************************/
void CWsfHtmlDoc::OnCloseDocument() 
{
  // NOTE: This is not called by CWsfApp::CloseDoc(). See the method
  // on details
}

/*****************************************************************************/
void CWsfHtmlDoc::OnFileSave() 
{
   OnFileSaveAs();
}

/*****************************************************************************/
void CWsfHtmlDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(true);
}

/*****************************************************************************/
void CWsfHtmlDoc::OnFileSaveAs() 
{
   // Make filename by omitting invalid chars from the title

   CString s;
   CString sTitle = GetTitle();

   for (int i=0; i<sTitle.GetLength(); i++)
   {
      UINT n = PathGetCharType(sTitle.GetAt(i));
      if (n & (GCT_LFNCHAR|GCT_SHORTCHAR)) s += sTitle.GetAt(i);
   }

   s += ".html";

   // Choose file

   CString sFilter((char*)IDS_HTML_SAVE_FILE_FILTER);
   CFileDialog d(false, ".html", s, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);
   if (d.DoModal() != IDOK) return;

   // Save

   try
   {
      CWaitCursor wc;
      CWslFile::Copy(GetPathName(), d.GetPathName(), false);
   }
   catch (CException* e)
   {
      WsfReportError(e, IDS_OPERATION_FAILED, true);
   }
}

void CWsfHtmlDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(true);
}

