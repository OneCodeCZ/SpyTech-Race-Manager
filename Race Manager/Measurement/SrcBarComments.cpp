/******************
*                 *  SrcBarComments.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
// #include "WndRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************/

CSrcBarComments::CSrcBarComments(CWslWatchSource *pOwner) {
	SetEnvironment(C_WHITE, 3, W_ALIGN_CENTER, C_BLACK, C_WHITE, FALSE);
	RW(NEW_TEXT(" ", 0, 540, 800, 36, this));
}

CSrcBarComments::~CSrcBarComments() {
}

void CSrcBarComments::SetLine(CString s) {
	TRACE0("SETTING THE LINE\n");
	GetText(0)->SetText(s);
}
