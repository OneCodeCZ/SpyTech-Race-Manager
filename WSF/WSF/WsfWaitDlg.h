///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// Displays a dialog while performing lengthy task in the background.

#pragma once

#include "WsfDialog.h"

//*****************************************************************************
class WSL_API CWsfWaitDialog : public CWsfDialog
{
public:
   class CContext
   {
   friend class CWsfWaitDialog;
   public:
      CProgressCtrl Progress;
      CStatic       Text;
   protected:
      bool&         bCanceled;

   public:
      CContext(bool& __bCanceled) : bCanceled(bCanceled) { }

      // Call to check if the user has canceled the task
      bool IsCanceled() const { return bCanceled; }
   };

public:
   typedef int WORKER_CALLBACK(void* pClass, CContext& ctx);						

protected:
   bool              m_bCancelFlag;   // TRUE = cancel button was pressed
   CWinThread*			m_pThread;        
   void*				   m_pClass;
   WORKER_CALLBACK*	m_pCallback;
   bool					m_bCanBeCanceled;
   CString				m_sText;
   DWORD					m_dwAnimId;
   int					m_nResult;			// to be returned from DoModal()
   
public:
   //---------------------------------------------------------------------------
   // API
   //---------------------------------------------------------------------------
   
   // Display the wait dialog and perform the given task
   // If pcszText is NULL, the default text "Prosim cekejte..." is used.
   static int DoModal(void* pClass,
      WORKER_CALLBACK* pCallback,
      const char* pcszText = NULL,  
      bool bCanBeCanceled = true,
      DWORD dwAnimId = 0);
   
   //---------------------------------------------------------------------------
   // Internal stuff
   //---------------------------------------------------------------------------
protected:
   CWsfWaitDialog(int nIDD);
   virtual void OnOK();
   
   // Worker thread entry
   void DoIt();
   static DWORD __ThreadEntry(CWsfWaitDialog* p);
   
   //{{AFX_DATA(CWsfWaitDialog)
   //}}AFX_DATA
   
   //{{AFX_VIRTUAL(CWsfWaitDialog)
   //}}AFX_VIRTUAL
   
protected:
   //{{AFX_MSG(CWsfWaitDialog)
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

