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

#include "stdafx.h"
#include "../generic/WslWin32Exception.h"
#include "WslGenericFile.h"
#include "WslFile.h"

//******************************************************************************
void* WslLoadDAT(const char* pcszName, 
								 void* pBuffer/*=NULL*/, 
								 int* pnSize/*=NULL*/)  throw(CWslWin32Exception*)
{
  CWslFile f;
	BYTE* p;

  f.Open((char*)pcszName);

	// Manage size

  int nToDo = (int) f.GetLength();

  if (pnSize) 
  {
    if (*pnSize > 0 && *pnSize < nToDo) 
      nToDo = *pnSize; 
    else 
      *pnSize = nToDo;
  }

	// Load

  if (nToDo)
  {
    p = pBuffer ? (BYTE*) pBuffer : new BYTE[nToDo];
		ASSERT(p);

		try
		{
			f.Read(p, nToDo);
		}
		catch (CWslWin32Exception*)
		{
			if (!pBuffer) delete p;
			throw;
		}
  }

  f.Close();

  return p;
}

//******************************************************************************
void WslSaveDAT(const char *pcszName, void *pWhat, DWORD dwSize)
{
  CWslFile f;
  f.Create((char*)pcszName, CREATE_ALWAYS, 0);
  f.Write(pWhat, dwSize);
  f.Close();
}



