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

//************************************************************************
WSL_API void DDX_Check(CDataExchange* pDX, int nIDC, DWORD& value, DWORD Bit)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
  { 
		int x = (int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L);
    ASSERT(x==0 || x==1);
    if (x)
      value|=Bit;
    else
      value&=~Bit;
	}
	else
	{
    DWORD x = value&Bit? 1:0;
		::SendMessage(hWndCtrl, BM_SETCHECK, (WPARAM)x, 0L);
	}
}

//************************************************************************
WSL_API void DDX_Check(CDataExchange* pDX, int nIDC, bool &bState)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
  { 
		int x = (int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L);
    ASSERT(x==0 || x==1);
    bState = x ? true : false;
	}
	else ::SendMessage(hWndCtrl, BM_SETCHECK, (WPARAM)bState, 0);
}


//************************************************************************
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, char *pszText, 
						 DWORD dwMaxLen, BOOL bCanBeEmpty /*= false*/)
{
	ASSERT(dwMaxLen > 0);
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);

	if (pDX->m_bSaveAndValidate)
	{
		if ((GetWindowLong(hWndCtrl, GWL_STYLE) & ES_READONLY)
			|| !IsWindowEnabled(hWndCtrl)) return;

		int nCopied = ::GetWindowText(hWndCtrl, pszText, dwMaxLen + 1);
		if (!nCopied && !bCanBeEmpty)
		{
	    AfxMessageBox(IDS_ITEM_MUST_BE_FILLED);
		  pDX->Fail();
		}
	}
	else
	{
		::SendMessage(hWndCtrl, EM_LIMITTEXT, dwMaxLen, 0);
		SetWindowText(hWndCtrl, pszText);
	}
}

//************************************************************************
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, CString &strText, 
						 DWORD dwMaxLen, BOOL bCanBeEmpty /*= false*/,
						 BOOL bComboBox /*= false*/)
{
   ASSERT(dwMaxLen > 0 && dwMaxLen < 1024*100);
   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
   
   if (pDX->m_bSaveAndValidate)
   {
      if ((GetWindowLong(hWndCtrl, GWL_STYLE) & ES_READONLY)
         || !IsWindowEnabled(hWndCtrl)) return;
      
      char *pszBuffer = new char[dwMaxLen + 1];
      int nCopied = GetWindowText(hWndCtrl, pszBuffer, dwMaxLen + 1);
      
      if (!nCopied && !bCanBeEmpty)
      {
         delete[] pszBuffer;
         AfxMessageBox(IDS_ITEM_MUST_BE_FILLED);
         pDX->Fail();
      }
      
      strText = pszBuffer;
      delete[] pszBuffer;
   }
   else
   {
      SendMessage(hWndCtrl, bComboBox ? CB_LIMITTEXT : EM_LIMITTEXT, dwMaxLen, 0);
      SetWindowText(hWndCtrl, strText);
   }
}

//************************************************************************
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, const char *pcszText)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	
	if (!pDX->m_bSaveAndValidate)
		SetWindowText(hWndCtrl, pcszText);
}

//************************************************************************
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, 
                         int &nValue, 
                         int nMin, 
                         int nMax)
{
	char temp[32];
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	
	if (pDX->m_bSaveAndValidate)
	{
		if ((GetWindowLong(hWndCtrl, GWL_STYLE) & ES_READONLY)
			|| !IsWindowEnabled(hWndCtrl)) return;

		GetWindowText(hWndCtrl, temp, 32);
		int n = atoi(temp);
		
		if (n < nMin || n > nMax)
		{
			CString s;
			s.Format(IDS_ENTER_NUMBER_IN_RANGE, nMin, nMax);
	    AfxMessageBox(s);
		  pDX->Fail();
		}

		nValue = n;
	}
	else 
	{
	//	ASSERT(nValue >= nMin && nValue <= nMax);
		itoa(nValue, temp, 10);
		SendMessage(hWndCtrl, EM_LIMITTEXT, 11, 0);		// 2^32 has 10 digits + sign
		SetWindowText(hWndCtrl, temp);
	}
}

//************************************************************************
WSL_API void DDX_WslText(CDataExchange* pDX, 
                         int nIDC, 
                         CWslDateTime& dt, 
                         BOOL bCanBeEmpty /*= false*/)
{
	char temp[64];
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	
	if (!pDX->m_bSaveAndValidate)
	{
		SetWindowText(hWndCtrl, dt.Get(temp));
	}
	else 
	{
		GetWindowText(hWndCtrl, temp, 64);

		if (!temp[0] && !bCanBeEmpty)
		{
			AfxMessageBox(IDS_ITEM_MUST_BE_FILLED);
			pDX->Fail();
		}

		if (!dt.Set(temp))
		{
			AfxMessageBox(IDS_BAD_DATETIME);
			pDX->Fail();
		}
		else 
		{
			// Set the date to have right formating
			SetWindowText(hWndCtrl, dt.Get(temp));
		}
	}
}

//*****************************************************************************
WSL_API void DDX_WslText(CDataExchange* pDX, 
                         int nIDC, 
                         CWslDate& date,
                         BOOL bCanBeEmpty)
{
	char temp[64];
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	
	if (!pDX->m_bSaveAndValidate)
	{
		SetWindowText(hWndCtrl, date.GetDMY());
	}
	else 
	{
		GetWindowText(hWndCtrl, temp, 64);
		if (!date.Set(temp) || (!bCanBeEmpty && date.IsNull()) )
		{
			AfxMessageBox(IDS_BAD_DATE);
			pDX->Fail();
		}
		else 
		{
			// Set the date to have right formating
			SetWindowText(hWndCtrl, date.GetDMY());
		}
	}

}


//************************************************************************
WSL_API void DDX_WslDate(CDataExchange* pDX, int nIDC, CWslDate &Date)
{
	CDateTimeCtrl *pCtrl;
	SYSTEMTIME st;

	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	pCtrl = (CDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
  { 
		pCtrl->GetTime(&st);
		Date.Set(&st);
	}
	else
	{
		Date.Get(&st);
 		pCtrl->SetTime(&st);
	}
}

//************************************************************************
WSL_API void DDX_WslTime(CDataExchange* pDX, int nIDC, 
							    CTime& value, CTime& MinTime, CTime& MaxTime)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CDateTimeCtrl* pWnd = (CDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
	{
		pWnd->GetTime(value);
		
		if (value < MinTime)
		{
			CString s;
			s.Format(IDS_TIME_IN_RANGE, MinTime.Format("%H:%M:%S"), MaxTime.Format("%H:%M:%S"));
			AfxMessageBox(s);
		  pDX->Fail();
		}
	}
	else 
	{
		pWnd->SetTime(&value);
	}
}

//************************************************************************
WSL_API void DDX_WslTime(CDataExchange* pDX, int nIDC, 
                         CTimeSpan& value, 
                         CTimeSpan& MinTime, CTimeSpan& MaxTime)
{
	SYSTEMTIME st;
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CDateTimeCtrl* pWnd = (CDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
	{
		pWnd->GetTime(&st);
		value = CTimeSpan(0,st.wHour, st.wMinute, st.wSecond);

		if (value < MinTime)
		{
			CString s;
			s.Format(IDS_TIME_IN_RANGE, MinTime.Format("%H:%M:%S"), MaxTime.Format("%H:%M:%S"));
			AfxMessageBox(s);
		  pDX->Fail();
		}
	}
	else 
	{
		st.wHour = (WORD) value.GetHours();
		st.wMinute = (WORD) value.GetMinutes();
		st.wSecond = (WORD) value.GetSeconds();
		st.wMilliseconds = 0;
		st.wYear = 2000;			// Some date must be set!
		st.wMonth = 1;
		st.wDay = 1;

		pWnd->SetTime(&st);
	}
}


//************************************************************************
WSL_API void DDX_WslCombo(CDataExchange* pDX, int nIDC, 
                          int& nIndex, BOOL bCanBeEmpty /*= false*/)
                          
{
   HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
   CComboBox* pWnd = (CComboBox*) CWnd::FromHandle(hWndCtrl);
   
   if (pDX->m_bSaveAndValidate)
   {
      if (pWnd->IsWindowEnabled())
      {
         int n = pWnd->GetCurSel();
         if (n==CB_ERR && !bCanBeEmpty)
         {
            AfxMessageBox(IDS_ITEM_MUST_BE_FILLED);
            pDX->Fail();
         }
         nIndex = n;
      }
   }
   else 
   {
      ASSERT(nIndex >= -1 && nIndex < pWnd->GetCount());	// bounds check failed
      pWnd->SetCurSel(nIndex);
   }
}

/*****************************************************************************/
WSL_API void DDX_CBItemData(CDataExchange* pDX, int nIDC, int& nData, BOOL bCanBeEmpty)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CComboBox* pWnd = (CComboBox*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
	{
		int n = pWnd->GetCurSel();
		if (n==CB_ERR)
		{
			if (!bCanBeEmpty)
			{
				AfxMessageBox(IDS_ITEM_MUST_BE_FILLED);
				pDX->Fail();
			}
			nData = 0;
		}
		else nData = pWnd->GetItemData(n);
	}
	else 
	{
		for (int i=pWnd->GetCount(); i--;)
		{
			if (pWnd->GetItemData(i) == nData)
			{
				pWnd->SetCurSel(i);
				return;
			}
		}
		pWnd->SetCurSel(-1);
	}
}

//************************************************************************
WSL_API void DDX_WslPrice(CDataExchange* pDX, int nIDC, 
                          double& dPrice)
{
	char temp[64], temp2[64];
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	
	if (pDX->m_bSaveAndValidate)
	{
		if ((GetWindowLong(hWndCtrl, GWL_STYLE) & ES_READONLY)
			|| !IsWindowEnabled(hWndCtrl)) return;

		GetWindowText(hWndCtrl, temp, 64);

    if (!strlen(temp))
    {
	    AfxMessageBox(IDS_BAD_CURRENCY);
		  pDX->Fail();
    }

    // Remove the thousends separator
    // Get value

    struct lconv* pLconv = localeconv();
    char* s=temp;
    char* d=temp2;
    char ch;

    for (; ch=*s; s++) if (ch != *pLconv->thousands_sep) *d++ = ch;
    *d = 0;

		dPrice = atof(temp2);

    // Update string representation

    WslPriceToString(dPrice, temp, 64);
		SetWindowText(hWndCtrl, temp);
	}
	else 
	{
    WslPriceToString(dPrice, temp, 64);
		SendMessage(hWndCtrl, EM_LIMITTEXT, 32, 0);
		SetWindowText(hWndCtrl, temp);
	}
}

/*****************************************************************************/
WSL_API void DDX_WsfWidthHeight(CDataExchange* pDX, 
                                int nEditCtrlId,
                                int& nW, int& nH,
                                int nMaxW, int nMaxH)
{
   CWnd* pWnd = CWnd::FromHandle(pDX->PrepareEditCtrl(nEditCtrlId));
   ASSERT_VALID(pWnd);

   if (pDX->m_bSaveAndValidate)
   {
      CString s;
      pWnd->GetWindowText(s);	

      char temp[64];
      const char* pcsz = s;

      pcsz = WslGetToken(pcsz, temp, 64, " x");
      pcsz = WslSkipSpaces(pcsz, " x");
      int w = atoi(temp);

      pcsz = WslGetToken(pcsz, temp, 64, "");
      int h = atoi(temp);

      if (w<=0 || w>nMaxW || h<=0 || h>=nMaxH)
      {
         CString s((const char*)IDS_BAD_WIDTH_HEIGHT), s2;
         s2.Format(s, nMaxW, nMaxH);
         AfxMessageBox(s2);
         pDX->Fail();
      }

      nW = w;
      nH = h;

      s.Format("%d x %d", nW, nH);
      pWnd->SetWindowText(s);	
   }
   else
   {
      CString s;
      s.Format("%d x %d", nW, nH);
      
      pWnd->SetWindowText(s);	   
      pWnd->SendMessage(EM_LIMITTEXT, 16);
   }
}

/*****************************************************************************
*
* USING VARIANT
*
*****************************************************************************/

//************************************************************************
WSL_API void DDX_WsfVariantText(CDataExchange* pDX, 
                                int nEditId, int nNullCheckId,
                                CWslVariant& v,
                                int nMaxLen, 
                                BOOL bCanBeEmpty)
{
	ASSERT(nMaxLen > 0 && nMaxLen < 1024);		// 1024 for the alloc max.
	char temp[1024+1];
	HWND hWndCtrl = pDX->PrepareEditCtrl(nEditId);

	if (pDX->m_bSaveAndValidate)
	{
		if (nNullCheckId && ((CButton*)pDX->m_pDlgWnd->GetDlgItem(nNullCheckId))->GetCheck())
		{
			v.SetNull(); 
			return; 
		}

		if ((GetWindowLong(hWndCtrl, GWL_STYLE) & ES_READONLY) || !IsWindowEnabled(hWndCtrl)) return;

		int nCopied = GetWindowText(hWndCtrl, temp, 1024);
		if (!nCopied && !bCanBeEmpty)
		{
	    AfxMessageBox(IDS_ITEM_MUST_BE_FILLED);
		  pDX->Fail();
		}

		v = temp;
	}
	else
	{
		if (nNullCheckId)
		{
			SendMessage(hWndCtrl, EM_SETREADONLY, v.IsNull(), 0);
			((CButton*)pDX->m_pDlgWnd->GetDlgItem(nNullCheckId))->SetCheck(v.IsNull());
			if (v.IsNull()) { SetWindowText(hWndCtrl, ""); return; }
		}

		SendMessage(hWndCtrl, EM_LIMITTEXT, nMaxLen, 0);
		SetWindowText(hWndCtrl, (CString) v);
	}
}




