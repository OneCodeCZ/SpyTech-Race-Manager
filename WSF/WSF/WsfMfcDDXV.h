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

#pragma once

#ifndef WSL_API
	#define WSL_API
#endif

#include <fcntl.h>
#include <float.h>
#include <generic/WslDate.h>
#include <generic/WslDateTime.h>

WSL_API void DDX_Check(CDataExchange* pDX, int nIDC, DWORD& value, DWORD Bit);
inline WSL_API void DDX_Check(CDataExchange* pDX, int nIDC, int& value, DWORD Bit) { DDX_Check(pDX,nIDC,(DWORD&)value,Bit); }
WSL_API void DDX_Check(CDataExchange* pDX, int nIDC, bool &bState);

WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, char *pszText, DWORD dwMaxLen, BOOL bCanBeEmpty = false);
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, const char *pcszText);
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, int &nValue, int nMin = 0, int nMax = 0x7FFFFFFF);
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, CWslDateTime& dt, BOOL bCanBeEmpty = false);
WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, CWslDate& date, BOOL bCanBeEmpty = false);

WSL_API void DDX_WslText(CDataExchange* pDX, int nIDC, CString &strText, 
												 DWORD dwMaxLen, BOOL bCanBeEmpty = false, 
												 BOOL bComboBox = false);

WSL_API void DDX_WslTime(CDataExchange* pDX, int nIDC, CTime& value, CTime& MinTime, CTime& MaxTime);
WSL_API void DDX_WslTime(CDataExchange* pDX, int nIDC, CTimeSpan& value, CTimeSpan& MinTime, CTimeSpan& MaxTime);
WSL_API void DDX_WslDate(CDataExchange* pDX, int nIDC, CWslDate &Date);

WSL_API void DDX_WslCombo(CDataExchange* pDX, int nIDC, int& nIndex, BOOL bCanBeEmpty = false);

// Selects an item according item data. 
// If the combo-box is empty and it's allowed, 0 is returned.
// NOTE: In the set-mod, there is a for-cycle on all items;
WSL_API void DDX_CBItemData(CDataExchange* pDX, int nIDC, int& nData, BOOL bCanBeEmpty);

WSL_API void DDX_WslPrice(CDataExchange* pDX, int nIDC, double& dPrice);

WSL_API void DDX_WsfWidthHeight(CDataExchange* pDX, 
                                int nEditCtrlId,
                                int& nW, int& nH,
                                int nMaxW, int nMaxH);

/*****************************************************************************
*
* USING VARIANT
*
*****************************************************************************/

WSL_API void DDX_WsfVariantText(CDataExchange* pDX, 
                                int nEditId, int nNullCheckId,
                                CWslVariant& v,
                                int nMaxLen, 
                                BOOL bCanBeEmpty);
