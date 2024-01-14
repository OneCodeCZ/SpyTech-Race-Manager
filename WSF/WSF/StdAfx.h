///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define VC_EXTRALEAN
//#define _WIN32_WINNT    0x0500      // Win2000+
#define _WIN32_IE       0x600       // IE 6.0+

#include <afxwin.h>         
#include <afxext.h>         
#include <afxole.h>         
#include <afxdtctl.h>		
#include <afxcmn.h>			
//#include <htmlhelp\Htmlhelp.h>
#include <Shlwapi.h>
#include <afxhtml.h>
#include <SQL.H>
#include <SQLEXT.H>
#include <typeinfo.h>
#include <winver.h>
#include <locale.h>
#include <afxtempl.h>
#include <shlobj.h>        // neccessary for the WSF to use shfolder.dll
#include <Winsock2.h>
#include <atlbase.h>
#include <Wincrypt.h>

#define WSL_API __declspec(dllexport)
#undef AFX_API
#define AFX_API WSL_API

#define WSF_DLL_BUILD

// Included here because it prepars environment for files from WSL
#include "common.h"

