///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Displayes dialog with the subtree specified by pTreeItem and
// allows to select a tree item whose runtime class is the same
// as one of classes specified in the "paClasses" array.
// On OK it returns the selected item.

WSL_API CWsfTreeItem* WsfSelectTreeItem(int nTitleID, int nTextID,
																				CWsfTreeItem *pTreeItem,
																				CWslArray<CRuntimeClass*> *paClasses,
																				CWnd *pParent = NULL);

