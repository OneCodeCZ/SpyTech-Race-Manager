///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// This file is included only in the DLL build.

#pragma once

// TODO: To be removed
#pragma warning(disable: 4996)

#include "resource.h" 
#include "..\WsfCmdId.h"						// Standard WSF command IDs

// Helper classes from WSL

#include <generic/WslArray.h>
#include <generic/WslDate.h>
#include <generic/WslRegKey.h>
#include <generic/WslTextStuff.h>
#include <generic/WslPtr.h>
#include <db/WslDbVariant.h>
#include <xml\WslXML.h>
#include <files\WslGenericFile.h>
#include <files\WslFile.h>
#include <files\WslFS.h>
#include <files\WslGenericFileSearch.h>
#include <files\WslFileSearch.h>
#include <files\WslMemFile.h>

// DLL build only constants

#define WSF_TIME_TO_NODE_EXPAND				1500				// Used in drag & drop. In ms.
#define WSF_CONTROL_BARS_REG_KEY			"wsf\\ControlBars"

// Framework

class CWsfTreeItem;
class CWsfFrame;
class CWsfWorkspaceDoc;

#include "WsfSerialMacros.h"
//#include "WsfFileHelpers.h"
#include "WsfMfcDDXV.h"

#include "WsfAppPlugin.h"
#include "WsfChildernResizer.h"
#include "WsfListCtrl.h"
#include "WsfTreeCtrl.h"
#include "WsfTableCtrl.h"
#include "WsfWorkspaceDoc.h"
#include "WsfApp.h"

#include "WsfTreeItem.h"
#include "WsfTINamed.h"
#include "WsfTIFolder.h"
#include "WsfTINamedFolder.h"

#include "WsfTabWnd.h"
#include "WsfVSplitter.h"
#include "WsfTreeView.h"
#include "WsfTreeItemView.h"
#include "WsfStatusBar.h"
#include "WsfFrame.h"

// Global variables

extern HINSTANCE g_hInstance;
extern RECT WsfZeroRect;
extern char temp[1024], temp2[1024], temp3[1024];
extern CWsfPluginsArray	g_aAppPlugins;
