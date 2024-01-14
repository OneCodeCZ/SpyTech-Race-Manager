///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL Common Definitions
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001-2002
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define WSL_VERSION		0x0501
#define WSL_API

#pragma warning( disable : 4290 )		// Ignore "throw (... , ...) not implemented" warning

#ifndef ASSERT
#define ASSERT _ASSERT
#endif

#ifndef VERIFY
#ifdef _DEBUG
#define VERIFY _ASSERT
#else
#define VERIFY(x) (x);
#endif
#endif

#define _public public:
#define _protected protected:
#define _private private:
#define public_ public:
#define protected_ protected:
#define private_ private:

#ifndef TRACE0
#define TRACE0(a)			_RPT0(0, a)
#define TRACE1(a,b)		_RPT1(0, a,b)
#define TRACE2(a,b,c)	_RPT2(0, a,b,c);
#define TRACE3(a,b,c,d)	_RPT3(0, a,b,c,d);
#endif
