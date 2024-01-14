
#include "stdafx.h"
#include "..\generic\WslWin32Exception.h"
#include "../files/WslGenericFile.h"
#include "../files/WslFile.h"
#include "WslDIB.h"

//*****************************************
CWslDIB::CWslDIB()
{
	m_pbyData = NULL;
}

//*****************************************
CWslDIB::~CWslDIB()
{
	if (m_pbyData) delete[] m_pbyData;
}

//*****************************************
void CWslDIB::LoadDIB(int id)
{
	BITMAPFILEHEADER bh;
	BITMAPINFO bi;
	
	//load resource
	HRSRC hResData=FindResource(NULL,(char*)id,"Binary");
	if (!hResData) throw new CWslWin32Exception();
	
	HGLOBAL hBitmapData=LoadResource(NULL, hResData);
	if (!hBitmapData) throw new CWslWin32Exception();
	
	BYTE *x=(BYTE*)LockResource(hBitmapData);
	if (!x) throw new CWslWin32Exception();
	
	//copy headers
	memcpy(&bh,x,sizeof(bh));
	memcpy(&bi,x+sizeof(bh),sizeof(bi));
	
	//init variables
	m_nW=bi.bmiHeader.biWidth;
	m_nH=bi.bmiHeader.biHeight;
	if (m_pbyData) delete m_pbyData;
	m_pbyData=x + bh.bfOffBits;
}

//*****************************************
void CWslDIB::LoadDIB(const char *pcszName)
{
	CWslFile f;
	BITMAPFILEHEADER bh;
	BITMAPINFO bi;
	
	f.Open(pcszName);
	f.Read(&bh, sizeof(bh));
	f.Read(&bi, sizeof(bi));
	
	m_nW = bi.bmiHeader.biWidth;
	m_nH = bi.bmiHeader.biHeight;
	m_byBitDepth = (BYTE) bi.bmiHeader.biBitCount;
	DWORD dwSize = m_nW*m_nH*bi.bmiHeader.biBitCount/8;
	if (m_pbyData) delete m_pbyData;
	m_pbyData = (BYTE*)new BYTE[dwSize];
	
	f.Seek(bh.bfOffBits);
	f.Read(m_pbyData, dwSize);
}

//*****************************************************************************
void CWslDIB::Convert24to16()
{
#pragma pack (push, 1)
	struct RGB24 { BYTE b,g,r; } *p24;
#pragma pack (pop)
	WORD *p16;
	
	_ASSERT(m_byBitDepth==24);
	
	p24 = (RGB24*)m_pbyData;
	p16 = (WORD*)m_pbyData;
	
	for (int i=m_nW*m_nH; i--;)
	{
		*p16++=(((int)p24->b*32/256)<<0) + (((int)p24->g*32/256)<<5) + (((int)p24->r*32/256)<<10);
		p24++;
	}
}

//*****************************************************************************
void CWslDIB::SwapRB()
{
	BYTE *p = m_pbyData;
	
	for (int i=m_nW*m_nH; i--;)
	{
		BYTE x = p[0];
		p[0] = p[2];
		p[2] = x;
		p += 3;
	}
}





