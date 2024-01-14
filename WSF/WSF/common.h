///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// Definitions common to both the DLL build and the client application

#pragma once

#define MAKETIMET(h,m,s) (h*60*60 + m*60 + s)
#define MAKEDWORD(b3,b2,b1,b0) (((BYTE)(b3)<<24)|((BYTE)(b2)<<16)|((BYTE)(b1)<<8)|((BYTE)(b0)))
#define DLUX(x) ((x)*155/100)
#define DLUY(y) ((y)*160/100)

#define _public public:
#define _protected protected:
#define _private private:
#define public_ public:
#define protected_ protected:
#define private_ private:

// Ignore "throws(..,...) not implemented" warning
#pragma warning( disable : 4290 ) 

// Private MFC messages made public ;-)

#define WM_IDLEUPDATECMDUI  0x0363  // wParam == bDisableIfNoHandler
#define WM_INITIALUPDATE    0x0364  // (params unused) - sent to children

