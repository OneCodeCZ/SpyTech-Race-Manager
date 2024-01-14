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

#pragma once

#define WSL_API __declspec(dllimport)
#undef AFX_API
#define AFX_API WSL_API

#include <afxtempl.h>
#include <locale.h>
#include "wsf\common.h"			// Common definitions shared with the DLL build
#include "WsfCmdId.h"			// Standard WSF command IDs

// Helper classes from WSL

#include <generic\WslArray.h>
#include <generic\WslDate.h>
#include <generic\WslRegKey.h>
#include <generic\WslException.h>
#include <generic\WslWin32Exception.h>
#include <generic\WslTextStuff.h>
#include <xml\WslXML.h>
#include <security\WslMD5.h>
#include <generic\WslPtr.h>
#include <security\WslWinDataProtection.h>

#include <files\WslGenericFile.h>
#include <files\WslFile.h>
#include <files\WslFS.h>
#include <files\WslGenericFileSearch.h>
#include <files\WslFileSearch.h>
#include <files\WslMemFile.h>
#include <files\WslFileMapping.h>


// others on demand :-)

// Forward declarations

class CWsfFrame;
class CWsfVSplitter;
class CWsfTabWnd;
class CWsfApp;
class CWsfDocument;

// Basic stuff

#include "wsf\WsfSerialMacros.h"
#include "wsf\WsfTest.h"
//#include "wsf\WsfFileHelpers.h"
#include "wsf\WsfList.h"
#include "wsf\WsfObList.h"
#include "wsf\WsfPicture.h"

// DB

#include <db\WslDb.h>

// COM

#include "wsf\WsfDispatchPtr.h"

// ORM

#include <wsf\WsfOrm.h>

// Network

#include <network\WslNetwork.h>
#include <network\WslHttpSession.h>
#include <network\WslSnmpSession.h>

// Windows specific / OS specific

#include <windows\WslWinEventLog.h>
#include <windows\WslWinService.h>

// Main UI and framework classes

#include "wsf\WsfCmd.h"
#include "wsf\WsfToolbar.h"
#include "wsf\WsfStatusBar.h"
#include "wsf\WsfWorkspaceDoc.h"
#include "wsf\WsfFrame.h"
#include "wsf\WsfAppPlugin.h"
#include "wsf\WsfApp.h"
#include "wsf\WsfHSplitter.h"
#include "wsf\WsfTabWnd.h"

// Controls, dialogs, etc.

#include "wsf\WsfSelectTreeItem.h"
#include "wsf\WsfMfcDDXV.h"
#include "wsf\WsfChildernResizer.h"
#include "wsf\WsfWaitDlg.h"
#include "wsf\WsfFlatButton.h"
#include "wsf\WsfPropertyPage.h"
#include "wsf\WsfPropertySheet.h"
#include "wsf\WsfToolBarComboBox.h"
#include "wsf\WsfViewMultiplexor.h"
#include "wsf\WsfHtmlView.h"
#include "wsf\WsfFormView.h"
#include "wsf\WsfDialog.h"
#include "wsf\WsfObjectProperties.h"

// Table

#include "wsf\WsfTableCtrl.h"
#include "wsf\WsfTableCtrl2.h"
#include "wsf\WsfTableView.h"

// Tree items

#include "wsf\WsfTreeItem.h"
#include "wsf\WsfTINamed.h"
#include "wsf\WsfTIFolder.h"
#include "wsf\WsfTINamedFolder.h"
#include "wsf\WsfTreeView.h"
#include "wsf\WsfTreeItemView.h"

#undef AFX_API
#define AFX_API
