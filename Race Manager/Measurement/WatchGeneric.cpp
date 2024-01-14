/******************
*                 *  WatchGeneric.cpp
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
#include "WatchGeneric.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWslWatchGeneric::CWslWatchGeneric()
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
	m_bHidden=FALSE;
	m_bHide=FALSE;
	InitializeCriticalSection(&m_CriSec);
}

CWslWatchGeneric::~CWslWatchGeneric()
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{

}

int CWslWatchGeneric::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	m_bModified=FALSE;
	return 1;
}

/******************************************************************************/

CWslWatchText::CWslWatchText() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	m_nFlashCounter=0;
	m_bFlashing=FALSE;
   CWslWatchGeneric::CWslWatchGeneric();
}

CWslWatchText::CWslWatchText(const char *pcszString, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner, BOOL bFlashing) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Set the text
	SetText(pcszString);
	m_nFlashCounter=0;

	// Set-up the geometry
	m_nExtentX=nSX;
	m_nExtentY=nSY;
	m_nPosX=nX;
	m_nPosY=nY;
	m_bFlashing=bFlashing;

	m_nFontIdx=pOwner->m_nFontIdx;
	m_nBrushIdx=pOwner->m_nBrushIdx;
	m_nAlignMode=pOwner->m_nAlignMode;
	m_nBrushIdx=pOwner->m_nBrushIdx;
	m_bFramed=pOwner->m_bFramed;
	m_nFgColorIdx=pOwner->m_nFontColorIdx;
	m_nPenIdx=pOwner->m_nPenIdx;
//	pOwner->RegisterText(this);
}

CWslWatchText::~CWslWatchText() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::~CWslWatchGeneric();
}

CWslWatchText::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	TRACE2("%d %d -- ", m_nPosX, m_nPosY);
//	TRACE2("%d %d\n", m_nExtentX-1, m_nExtentY-1);

//	m_bFramed = TRUE;

	if (m_bHide && !m_bHidden) {
		if (!bFull) pDC->FillSolidRect(m_nPosX, m_nPosY, m_nExtentX-1, m_nExtentY-1, RGB(0,0,0));
		m_bHidden=TRUE;
		return 1;
	}
	// Draw the text if we have full update
	if ( 
			(
				(bFull || (m_nFlashCounter && m_bModified || m_nFlashCounter==1))
				&& 
				(!m_bHidden)
			) 
		|| 
			(
				m_bHidden
				&&
				!m_bHide
			)
		) 
	
	{
		m_bHidden=FALSE;
		EnterCriticalSection(&m_CriSec);
//	if (m_bModified || bFull) {
		gdi.PushObject(pDC->SelectObject(gdi.GetFont(m_nFontIdx)));

		// Decide type of alignment and build up flags for draw text.
		int nMode;
		switch(m_nAlignMode) {
		case W_ALIGN_LEFT: nMode=DT_LEFT; break;
		case W_ALIGN_RIGHT: nMode=DT_RIGHT; break;
		case W_ALIGN_CENTER: nMode=DT_CENTER; break;
		}

		// If framed, draw frame first
		if (m_bFramed) {
			gdi.PushObject(pDC->SelectObject(gdi.GetPen(m_nPenIdx)));
			pDC->MoveTo(m_nPosX, m_nPosY);
			pDC->LineTo(m_nPosX+m_nExtentX-1, m_nPosY);
			pDC->LineTo(m_nPosX+m_nExtentX-1, m_nPosY+m_nExtentY-1);
			pDC->LineTo(m_nPosX, m_nPosY+m_nExtentY-1);
			pDC->LineTo(m_nPosX, m_nPosY);
			pDC->SelectObject(gdi.PopObject());
			pDC->FillSolidRect(LPRECT(CRect(m_nPosX+1, m_nPosY+1, m_nExtentX+m_nPosX-1, m_nExtentY+m_nPosY-1)), 
				gdi.GetColorRef(m_nFlashCounter>1 ? m_nFgColorIdx : m_nBrushIdx));
		}
		// if not framed, draw the full rectangle
		else {
			pDC->FillSolidRect(LPRECT(CRect(m_nPosX, m_nPosY, m_nExtentX+m_nPosX, m_nExtentY+m_nPosY)), 
				gdi.GetColorRef(m_nFlashCounter>1 ? m_nFgColorIdx : m_nBrushIdx));
		}

		// Transparent text? Just for sure.
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(gdi.GetColorRef(m_nFlashCounter>1 ? m_nBrushIdx : m_nFgColorIdx));

		// And now, draw the whole text.
		pDC->DrawText(
			m_sString, 
			LPRECT(CRect(m_nPosX, m_nPosY, m_nPosX+m_nExtentX, m_nPosY+m_nExtentY)), 
			DT_VCENTER | DT_SINGLELINE | nMode);
		pDC->SelectObject(gdi.PopObject());

		LeaveCriticalSection(&m_CriSec);
	}

	if (!bFull && m_bModified && !m_nFlashCounter && !m_bHidden) {
		EnterCriticalSection(&m_CriSec);
		gdi.PushObject(pDC->SelectObject(gdi.GetFont(m_nFontIdx)));

		CSize size=pDC->GetTextExtent(m_sString);

		int y1=(m_nExtentY-size.cy)/2;
		int y2=y1+size.cy-1;
		if (y1<0) y1=0;
		if (y2>m_nExtentY-1) y2=m_nExtentY-1;

		int x1, x2;

		int nMode;
		switch(m_nAlignMode) {
		case W_ALIGN_LEFT:
			nMode=DT_LEFT;
			x1=size.cx;
			if (x1>m_nExtentX-1) x1=m_nExtentX-1;
			x2=m_nExtentX-1;
			pDC->FillRect(LPRECT(CRect(m_nPosX+x1, m_nPosY+y1, m_nPosX+x2, m_nPosY+y2)), gdi.GetBrush(m_nBrushIdx));
			break;
		case W_ALIGN_RIGHT:
			nMode=DT_RIGHT;
			x1=0;
			x2=m_nExtentX-size.cx;
			if (x2<0) x2=0;
			pDC->FillRect(LPRECT(CRect(m_nPosX+x1, m_nPosY+y1, m_nPosX+x2, m_nPosY+y2)), gdi.GetBrush(m_nBrushIdx));
			break;
		case W_ALIGN_CENTER:
			nMode=DT_CENTER;
			x1=(m_nExtentX-size.cx)/2;
			x2=x1+size.cx-1;
			if (x1<1) x1=1;
			if (x2>m_nExtentX-2) x2=m_nExtentX-2;
			pDC->FillRect(LPRECT(CRect(m_nPosX+1, m_nPosY+y1, m_nPosX+x1, m_nPosY+y2)), gdi.GetBrush(m_nBrushIdx));
			pDC->FillRect(LPRECT(CRect(m_nPosX+x2, m_nPosY+y1, m_nPosX+m_nExtentX-1, m_nPosY+y2)), gdi.GetBrush(m_nBrushIdx));
			break;
		}

		// Transparent text? Just for sure.
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(gdi.GetColorRef(m_nBrushIdx));
		pDC->SetTextColor(gdi.GetColorRef(m_nFgColorIdx));

		// And now, draw the whole text.
		pDC->DrawText(
			m_sString, 
			LPRECT(CRect(m_nPosX, m_nPosY, m_nPosX+m_nExtentX, m_nPosY+m_nExtentY)), 
			DT_VCENTER | DT_SINGLELINE | nMode);
		pDC->SelectObject(gdi.PopObject());

		LeaveCriticalSection(&m_CriSec);
	}

	CWslWatchGeneric::Draw(pDC, bFull);

	if (m_nFlashCounter) {
		m_nFlashCounter--;
	}

	return 1;
}

CWslWatchText::SetText(CString sNewString) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	if (m_sString!=sNewString) {
		m_bModified=TRUE;
		m_sString=sNewString;
		if (m_bFlashing) m_nFlashCounter=2;
	}
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

int CWslWatchText::ChangeFont(int nFontIdx) {
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	m_nFontIdx=nFontIdx;
	m_bModified=TRUE;
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

int CWslWatchText::ChangeAlignMode(int nAlignMode) {
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	m_nAlignMode=nAlignMode;
	m_bModified=TRUE;
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

int CWslWatchText::ChangePos(int nX, int nY, int nSX, int nSY) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nSX;
	m_nExtentY=nSY;
	m_bModified=TRUE;
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

CWslWatchText::SetText(int nNumber, int nDigits) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ASSERT(nDigits<17);
	char fmt[16];
	char temp[16];
	if (nDigits==-1) nDigits=1;
	strcpy(fmt, "%");
	strcat(fmt, itoa(nDigits, temp, 10));
	strcat(fmt, "d");
	wsprintf(temp, fmt, nNumber);
	for (int a=0; a<strlen(temp); a++) {
		if (temp[a]==' ') temp[a]='0';
	}
	SetText(temp);
	return 1;
}

/******************************************************************************/

CWslWatchBitmap::CWslWatchBitmap(CWslBitmap *pBitmap, int nX, int nY, CWslWatchSource *pOwner) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::CWslWatchGeneric();
	SetBitmap(pBitmap);
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=0;
	m_nExtentY=0;
	m_bHidden=FALSE;
}

CWslWatchBitmap::CWslWatchBitmap(CWslBitmap *pBitmap, int nX, int nY, int nExtentX, int nExtentY, CWslWatchSource *pOwner) {
	CWslWatchGeneric::CWslWatchGeneric();
	SetBitmap(pBitmap);
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nExtentX;
	m_nExtentY=nExtentY;
	m_bHidden=FALSE;
}	

CWslWatchBitmap::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//   pDC->FillSolidRect(m_nPosX, m_nPosY, 5, 5, RGB(255, 255, 255));

	if (!m_pBitmap) return 1;
	if (m_bHide && !m_bHidden) {
//		pDC->FillSolidRect(m_nPosX, m_nPosY, m_nPosX+m_nExtentX-1, m_nPosY+m_nExtentY-1, RGB(0,0,0));
		pDC->FillSolidRect(m_nPosX, m_nPosY, m_nExtentX, m_nExtentY, RGB(0, 0, 0));
		m_bHidden=TRUE;
//      pDC->FillSolidRect(m_nPosX, m_nPosY, 5, 5, RGB(255, 255, 0));
		return 1;
	}
	if (((m_bModified || bFull) && (!m_bHidden)) || (m_bHidden && !m_bHide))
   {
		EnterCriticalSection(&m_CriSec);

		if (m_bHide) m_bHidden=TRUE;
		if (!m_bHide) m_bHidden=FALSE;

//	if (m_bModified || bFull || (m_bHidden && !m_bHide)) {
//		EnterCriticalSection(&m_CriSec);
		if (m_pBitmap) {
			if (m_nExtentX && m_nExtentY) {
				int nPosX=(m_nExtentX - m_pBitmap->GetSizeX())/2+m_nPosX;
				int nPosY=(m_nExtentY - m_pBitmap->GetSizeY())/2+m_nPosY;
				m_pBitmap->Paint(pDC, nPosX, nPosY);
			}
			else {
//            pDC->FillSolidRect(m_nPosX, m_nPosY, 5, 5, RGB(255, 0, 0));
				m_pBitmap->Paint(pDC, m_nPosX, m_nPosY);
			}
		}

		LeaveCriticalSection(&m_CriSec);
	}
	CWslWatchGeneric::Draw(pDC, bFull);
	return 1;
}

CWslWatchBitmap::SetBitmap(CWslBitmap *pBitmap) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	m_pBitmap=pBitmap;
	m_bModified=TRUE;
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

CWslWatchBitmap::~CWslWatchBitmap() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::~CWslWatchGeneric();
}

/******************************************************************************/

CWslWatchColorBox::CWslWatchColorBox() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

int CWslWatchColorBox::ChangePos(int nX, int nY, int nSX, int nSY) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nSX;
	m_nExtentY=nSY;
	m_bModified=TRUE;
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

CWslWatchColorBox::CWslWatchColorBox(COLORREF crColor, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::CWslWatchGeneric();

	m_nPenIdx=pOwner->m_nPenIdx;
	m_crColor=crColor;
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nSX;
	m_nExtentY=nSY;
}

int CWslWatchColorBox::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (((m_bModified || bFull) && (!m_bHidden)) || (m_bHidden && !m_bHide)) {
		EnterCriticalSection(&m_CriSec);

		if (m_bHide) m_bHidden=TRUE;
		if (!m_bHide) m_bHidden=FALSE;

		m_bModified=FALSE;

		if (m_crColor!=-1) {

			gdi.PushObject(pDC->SelectObject(gdi.GetPen(m_nPenIdx)));
			pDC->MoveTo(m_nPosX, m_nPosY);
			pDC->LineTo(m_nPosX+m_nExtentX-1, m_nPosY);
			pDC->LineTo(m_nPosX+m_nExtentX-1, m_nPosY+m_nExtentY-1);
			pDC->LineTo(m_nPosX, m_nPosY+m_nExtentY-1);
			pDC->LineTo(m_nPosX, m_nPosY);
			pDC->SelectObject(gdi.PopObject());

			pDC->FillSolidRect(m_nPosX+1, m_nPosY+1, m_nExtentX-2, m_nExtentY-2, m_crColor);
		}

		LeaveCriticalSection(&m_CriSec);
	}
	return 1;
}

int CWslWatchColorBox::SetColor(COLORREF crColor) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	if (crColor!=m_crColor) {
		m_crColor=crColor;
		m_bModified=TRUE;
	}
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

CWslWatchColorBox::~CWslWatchColorBox() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::~CWslWatchGeneric();
}

/******************************************************************************/

CWslWatchColorCircle::CWslWatchColorCircle() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

CWslWatchColorCircle::CWslWatchColorCircle(int nBrushIdx, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::CWslWatchGeneric();

	m_nPenIdx=pOwner->m_nPenIdx;
	m_nBrushIdx=nBrushIdx;
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nSX;
	m_nExtentY=nSY;
}

int CWslWatchColorCircle::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if ((m_bHide || m_bHidden) && bFull) {
		m_bHidden = TRUE;
		return 1;
	}

	if (((m_bModified || bFull) && (!m_bHidden)) || (m_bHidden && !m_bHide)) {
		EnterCriticalSection(&m_CriSec);

		if (m_bHide) m_bHidden=TRUE;
		if (!m_bHide) m_bHidden=FALSE;

		m_bModified=FALSE;

		if (m_nBrushIdx!=-1) {

			gdi.PushObject(pDC->SelectObject(gdi.GetPen(m_nPenIdx)));
			gdi.PushObject(pDC->SelectObject(gdi.GetBrush(m_nBrushIdx)));

			pDC->Ellipse(m_nPosX, m_nPosY, m_nPosX+m_nExtentX, m_nPosY+m_nExtentY);

			pDC->SelectObject(gdi.PopObject());
			pDC->SelectObject(gdi.PopObject());

		}

		LeaveCriticalSection(&m_CriSec);
	}
	return 1;
}

int CWslWatchColorCircle::SetColor(int nBrushIdx) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	if (m_nBrushIdx!=nBrushIdx) {
		m_nBrushIdx=nBrushIdx;
		m_bModified=TRUE;
	}
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

CWslWatchColorCircle::~CWslWatchColorCircle() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::~CWslWatchGeneric();
}

/******************************************************************************/

CWslWatchSource::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);

	m_bHidden=m_bHide;

	if (!m_bHidden) {
		int nIdx;
		ITERATE(m_apWatches, nIdx) {
			//		TRACE("source (%x): hidden %d", (DWORD)this, nIdx);
			if (m_apWatches[nIdx]->m_bHide) m_apWatches[nIdx]->Draw(pDC, bFull);
		}
		ITERATE(m_apWatches, nIdx) {
			//		TRACE("source (%x): normal %d", (DWORD)this, nIdx);
			m_apWatches[nIdx]->Draw(pDC, bFull);
		}
	}
	
	LeaveCriticalSection(&m_CriSec);
	return 1;
}

void CWslWatchSource::SetEnvironment(int nFontColorIdx, int nFontIdx, int nAlignMode, int nBrushIdx, int nPenIdx, int nFrame) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (nFontColorIdx!=-1) m_nFontColorIdx=nFontColorIdx;
	if (nFontIdx!=-1) m_nFontIdx=nFontIdx;
	if (nAlignMode!=-1) m_nAlignMode=nAlignMode;
	if (nBrushIdx!=-1) m_nBrushIdx=nBrushIdx;
	if (nPenIdx!=-1) m_nPenIdx=nPenIdx;
	if (nFrame!=-1) m_bFramed=nFrame;
}

void CWslWatchSource::DeleteLastWatch() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);
	if (m_apWatches.GetSize()) {
		CWslWatchGeneric *pWatch=m_apWatches[m_apWatches.GetSize()-1];			
		delete pWatch;
		m_apWatches.ShrinkBy(1);
	}
	LeaveCriticalSection(&m_CriSec);
}

void CWslWatchSource::DestroyAll() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EnterCriticalSection(&m_CriSec);

	int nIdx;
	ITERATE(m_apWatches, nIdx) {
		CWslWatchGeneric *pWatch=m_apWatches[nIdx];
		delete pWatch;
	}
	m_apWatches.SetSize(0);

	LeaveCriticalSection(&m_CriSec);
}

void CWslWatchSource::Visibility(int nNewMode) {
	m_bHide=nNewMode==SW_HIDE;
//	ITERATE(m_apWatches, nIdx)	m_apWatches[nIdx]->Visibility(nNewMode);
}

/******************************************************************************/

CWslWatchProgress::CWslWatchProgress(int nMin, int nMax, int nDiv, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::CWslWatchGeneric();
	m_nMin=nMin;
	m_nMax=nMax;
	m_nDiv=nDiv;
	m_nCurrent=m_nMin;
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nSX;
	m_nExtentY=nSY;

	m_bHidden=FALSE;
	m_bHide=FALSE;

	m_nElapsedBrushIdx=C_GREY2;
	m_nRemainBrushIdx=C_GREY1;
	m_nLinePenIdx=C_WHITE;

//	pOwner->RegisterProgress(this);
}

int CWslWatchProgress::Draw(CDC *pDC, BOOL bFull) {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (m_bHide && !m_bHidden) {
		pDC->FillSolidRect(m_nPosX, m_nPosY, m_nPosX+m_nExtentX-1, m_nPosY+m_nExtentY-1, RGB(0,0,0));
		m_bHidden=TRUE;
		return 1;
	}

	if ((m_bHidden && !m_bHide) || (m_bModified && !m_bHidden) || (bFull && !m_bHidden)) {
		ASSERT(m_nMax != m_nMin);
		m_bHidden=FALSE;
		int x=m_nExtentX*m_nCurrent/(m_nMax-m_nMin);
		if (x<0) x=1;
		if (x>m_nExtentX-1) x=m_nExtentX-2;
		pDC->FillRect(LPRECT(CRect(m_nPosX, m_nPosY+1, m_nPosX+x, m_nPosY+m_nExtentY-2)), gdi.GetBrush(m_nElapsedBrushIdx));
		pDC->FillRect(LPRECT(CRect(m_nPosX+x, m_nPosY+1, m_nPosX+m_nExtentX-1, m_nPosY+m_nExtentY-2)), gdi.GetBrush(m_nRemainBrushIdx));
		
		gdi.PushObject(pDC->SelectObject(gdi.GetPen(m_nLinePenIdx)));
		
		pDC->MoveTo(m_nPosX, m_nPosY);
		pDC->LineTo(m_nPosX+m_nExtentX-1, m_nPosY);
		pDC->LineTo(m_nPosX+m_nExtentX-1, m_nPosY+m_nExtentY-1);
		pDC->LineTo(m_nPosX, m_nPosY+m_nExtentY-1);
		pDC->LineTo(m_nPosX, m_nPosY);
		
		for (int a=1; a<m_nDiv; a++) {
			x=a*m_nExtentX/m_nDiv;
			pDC->MoveTo(m_nPosX+x, m_nPosY);
			pDC->LineTo(m_nPosX+x, m_nPosY+m_nExtentY-1);
		}
		
		pDC->SelectObject(gdi.PopObject());
		CWslWatchGeneric::Draw(pDC, bFull);
	}
	return 1;
}

int CWslWatchProgress::SetCurrent(int nCurrent) {
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	m_nCurrent=nCurrent;
	m_bModified=TRUE;
	return 1;
}

int CWslWatchProgress::SetCurrent(int nCurrent, int nMin, int nMax, int nDiv) {
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ASSERT(nDiv);
	ASSERT(nMax>nMin);
	m_bModified=TRUE;
	m_nCurrent=nCurrent;
	m_nMax=nMax;
	m_nMin=nMin;
	m_nDiv=nDiv;
	return 1;
}

CWslWatchProgress::~CWslWatchProgress() {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CWslWatchGeneric::~CWslWatchGeneric();
}

/*********************************************************************** eof **/
