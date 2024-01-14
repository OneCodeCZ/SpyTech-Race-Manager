///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Simple loaders
// | P |  /  |  |  ~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
//if *pSize > 0 then it specifies maximum size to read
//if *pSize = 0 then *pSize will be equal to the size of the read file
//pBuffer = buffer for data, if NULL buffer is allocated and returned as return value

void* WslLoadDAT(const char* pcszName, 
								 void* pBuffer = NULL, 
								 int* pnSize = NULL) throw(CWslWin32Exception*);

void WslSaveDAT(const char *pcszName, void *pWhat, DWORD dwSize);


