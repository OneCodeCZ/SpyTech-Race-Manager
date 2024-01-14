///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslGenericFile
// | P |  /  |  |  ~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 09
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslGenericFile.h"
#include "..\generic\WslString.h"

#define WSL_GF_COPY_BUFFER_SIZE					(128*1024)						

/*****************************************************************************/
void CWslGenericFile::CloseNoException()
{
	try
	{
		Close();
	}
	catch (CWslException* e)
	{
		TRACE1("CWslGenericFile::CloseNoException(): %s\n", e->GetErrorMsg());
		e->Delete();
	}
}

//******************************************************************************
void CWslGenericFile::Copy(CWslGenericFile &TargetFile, 
													 int nLengthToCopy /*= -1*/,
													 BOOL *pbCancel /*= NULL*/,
													 COPY_CALLBACK *pfnCallback /*= NULL*/,
													 void *pData /*= NULL*/)
{
	CWslGenericFile &f = TargetFile;
	BYTE *pbyBuffer;
	int nToDo, nLength;
	BOOL bCancel;

	if (!pbCancel) { bCancel = false; pbCancel = &bCancel; }

	pbyBuffer = new BYTE[WSL_GF_COPY_BUFFER_SIZE];
	if (!pbyBuffer) throw new CWslWin32Exception(ERROR_NOT_ENOUGH_MEMORY);

	if (nLengthToCopy == -1) nLengthToCopy = GetLength();
	nLength = nLengthToCopy;

	try
	{
		while (nLengthToCopy && !*pbCancel)
		{
			nToDo = nLengthToCopy > WSL_GF_COPY_BUFFER_SIZE ? WSL_GF_COPY_BUFFER_SIZE : nLengthToCopy;
		
			Read(pbyBuffer, nToDo);
			f.Write(pbyBuffer, nToDo);

			nLengthToCopy -= nToDo;

			if (pfnCallback) pfnCallback(nLength, nToDo, pData);
		}
	}
	
	catch (CWslException* e) 
	{ 
		delete pbyBuffer; 
		throw e; 
	}

	delete pbyBuffer;
}

//******************************************************************************
void CWslGenericFile::Write(const char* pcszText)
{
	ASSERT(pcszText);
	Write(pcszText, strlen(pcszText));
}

//******************************************************************************
void CWslGenericFile::WriteLn(const char* pcszText /*= NULL*/)
{
	if (pcszText) Write(pcszText, strlen(pcszText));
	DWORD x = 0x0A0D; 
	Write(&x, 2); 
}

//******************************************************************************
void CWslGenericFile::WriteUnixLn(const char* pcszText /*= NULL*/)
{
	if (pcszText) Write(pcszText, strlen(pcszText));
	DWORD x = 0x0A; 
	Write(&x, 1); 
}

/*****************************************************************************/
void CWslGenericFile::ReadLn(CWslString& sBuffer)
{
  int n, i;
  BOOL bEOL = false;
  char temp[128+1], *q;
  sBuffer.Empty();

  while (!IsEOF() && !bEOL)
  {
    n = Read(temp, 128);

    for (i=0, q=temp; i<n; i++, q++)
    {
      if (*q==10) 
      { 
        if (n-i-1) Seek(-(n-i-1), FILE_CURRENT);
        bEOL = true;
        break; 
      }
      if (*q==13) *q = 0;
    }

    *q = 0;
    sBuffer += temp;
  }
} 

//*****************************************************************************
void CWslGenericFile::Format(const char* pcszFormat, ...)
{
	ASSERT(pcszFormat);

	char temp[1024];
	va_list l;
	va_start(l, pcszFormat);
	_vsnprintf(temp, 1024, pcszFormat, l);
	temp[1023] = 0;
	va_end(l);

	Write(temp);
}

//*****************************************************************************
void CWslGenericFile::FormatLn(const char* pcszFormat, ...)
{
	ASSERT(pcszFormat);

	char temp[1024];
	va_list l;
	va_start(l, pcszFormat);
	_vsnprintf(temp, 1024, pcszFormat, l);
	temp[1023] = 0;
	va_end(l);

	WriteLn(temp);
}


