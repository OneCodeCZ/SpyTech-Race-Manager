/******************
*                 *  SrcNiceTable.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  Spawned from SrcQualBest.cpp JAN 1, 2002.
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CSrcNiceTable
*
*******************************************************************************/

CSrcNiceTable::CSrcNiceTable(CWslWatchSource *pOwner) : CWslWatchSource(pOwner) {
	m_nTotalWidth=0;
	m_Columns.SetSize(0);
}

/******************************************************************************/

CSrcNiceTable::~CSrcNiceTable() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/

void CSrcNiceTable::RegisterColumn(int nWidth, int nAlign, int nType, int nFgIdx, int nBkIdx, int nFontIdx, BOOL bShrinkable) {
	TABLE_COLUMN col;

	col.m_nWidth=nWidth;
	col.m_nAlign=nAlign;
	col.m_nType=nType;

	col.m_nPos=0;

	col.m_nFontIdx=nFontIdx;
	col.m_nFgIdx=nFgIdx;
	col.m_nBkIdx=nBkIdx;

	col.m_bShrinkable=bShrinkable;

	m_nTotalWidth+=nWidth;

	m_Columns.Append(col);
}

/******************************************************************************/

void CSrcNiceTable::SetParams(int nX, int nY, int nSX, int nSY, int nHeight, int nColumnGap) {
	m_nPosX=nX;
	m_nPosY=nY;
	m_nExtentX=nSX;
	m_nExtentY=nSY;
	m_nHeight=nHeight;
	m_nColumnGap=nColumnGap;
}

/******************************************************************************/

#define __LEFT                     (
#define __RIGHT                    )
#define __LESS_THAN__              <
#define __GREATER_THAN__           >
#define __LESS_THAN_OR_EQUALS__    <=
#define __GREATER_THAN_OR_EQUALS__ >=

int CSrcNiceTable::BuildFields(int nMinimum) {

	EnterCriticalSection(&m_CriSec);

	// **************************************************************************
	// Calculation phase

	// Number of columns
//	int nMaxCols=m_nExtentX / m_nTotalWidth;

//	int nMaxCols=(m_nExtentX * m_nTotalWidth) / ((m_nColumnGap * m_nTotalWidth) + 1);
	int nMaxCols=(m_nExtentX + m_nColumnGap) / (m_nTotalWidth + m_nColumnGap);
	// Maximum number of rows
	int nMaxRows=m_nExtentY / m_nHeight;

	// For iterations
	int nIdx;
	BOOL bError=FALSE;

	// If we do not have at least minimal number of items displayable
	// i.e. --
	while __LEFT
				__LEFT
					// If count of items
					(nMaxCols * nMaxRows)
					/* IS */ __LESS_THAN__
					// The minimal count required, perform the cycle bellow
					(nMinimum) 
				__RIGHT
			&& 
				(!bError)
			__RIGHT
	{
		// Try to shrink the shrinkable ones
		ITERATE(m_Columns, nIdx) {
			// If shrnikable,
			if (m_Columns[nIdx].m_bShrinkable) {
				// shrink it
				m_Columns[nIdx].m_nWidth--;
				m_nTotalWidth--;
				// but be carefull with the width. If it gets beneath a threshold,
				// report bug.
				if (m_Columns[nIdx].m_nWidth < 0) bError=TRUE;
			}
		}
//		nMaxCols=(m_nExtentX - ((nMaxCols-1) * m_nColumnGap) ) / m_nTotalWidth;
//		nMaxCols=(m_nExtentX * m_nTotalWidth) / ((m_nColumnGap * m_nTotalWidth) + 1);
		nMaxCols=(m_nExtentX + m_nColumnGap) / (m_nTotalWidth + m_nColumnGap);

	}

	// The total size MUST be less than m_nExtetnX of the source

	ASSERT (
				(nMaxCols*m_nTotalWidth + (nMaxCols-1)*m_nColumnGap)
			 __LESS_THAN_OR_EQUALS__
				m_nExtentX
			 );

	// Now we have created maximum number of columns.
	// Nevertheless, if dimensions of columns will be much smaller than
	// m_nExtentX of this source, it may happen that more columns that it
	// is needed, is created.

	// **************************************************************************
	// Creation phase

	// First, let's create the background strips
	for (int a=0; a<nMaxRows; a++) {
		// Set environment for the strip
		SetEnvironment(1, 1, -1, (a%2) ? C_BLUE1 : C_BLUE2, C_WHITE, FALSE);
		// And now, register the watch itself.
		RegisterWatch(new CWslWatchText("", m_nPosX, m_nPosY+a*m_nHeight, m_nExtentX, m_nHeight, this));
	}
	m_nNumRows=nMaxRows;

	// Well, and now let's proceed with the rest.
	// First, Let's calculate the positions of the columns with respect

	int nSum=0;
	m_nNumColumns=0;
	ITERATE(m_Columns, nIdx) {
		m_Columns[nIdx].m_nPos=nSum;
		nSum+=m_Columns[nIdx].m_nWidth;
		if (m_Columns[nIdx].m_nType!=W_TYPE_DUMMY) m_nNumColumns++;
	}
	m_nTotalWidth=nSum;

	// Now, this variable's gonna contain the x-coord of current column
	nSum=m_nPosX;

	int nCntr=0;
	CWslWatchText *pWatchText;

	for (int b=0; b<nMaxCols; b++) {
		for (int a=0; a<nMaxRows; a++) {
			// Go through the subscribed columns
			ITERATE(m_Columns, nIdx) {
				// Set the environment for given table item
				SetEnvironment(m_Columns[nIdx].m_nFgIdx, m_Columns[nIdx].m_nFontIdx, m_Columns[nIdx].m_nAlign,
					(a%2) ? C_BLUE1 : C_BLUE2, C_WHITE, FALSE);
				switch (m_Columns[nIdx].m_nType) {
				case W_TYPE_TEXT:
					char temp[1024];
					sprintf(temp, "<%d>", nCntr++);
					pWatchText = new CWslWatchText(temp,
						m_Columns[nIdx].m_nPos + nSum, m_nPosY + a*m_nHeight, m_Columns[nIdx].m_nWidth, m_nHeight, this);
					pWatchText->SetText("");
					RegisterWatch(pWatchText);
					break;
				case W_TYPE_BITMAP:
					// Finnish this: Make the bitmap handling !!
					break;
				case W_TYPE_COLORBOX:
					RegisterWatch(new CWslWatchColorBox(
//						(rand()%2) ? COLORREF(-1) : RGB (rand()%2*255, rand()%2*255, rand()%2*255),
//						RGB (rand()%2*255, rand()%2*255, rand()%2*255),
						COLORREF(-1),
						m_Columns[nIdx].m_nPos + nSum,
						m_nPosY + a*m_nHeight + (m_nHeight - m_Columns[nIdx].m_nWidth)/2,
						m_Columns[nIdx].m_nWidth,
						m_Columns[nIdx].m_nWidth,
						this));
					break;
				case W_TYPE_DUMMY:
					// Just dummy. Nothing to be done here :-)
					// Columns containing dummy gaps only are not counted.
					break;
				}
			}
		}
		nSum+=m_nTotalWidth + m_nColumnGap;
	}

	m_nMaxNumItems = nMaxRows * nMaxCols;

//	CString s;
//	s.Format("w%d r%d c%d t%d", m_nTotalWidth, nMaxRows, nMaxCols, m_nMaxNumItems);
//	AfxMessageBox(s);

	LeaveCriticalSection(&m_CriSec);

	return nMaxRows * nMaxCols;

}

/******************************************************************************/

CWslWatchText *CSrcNiceTable::GetTextField(int nRow, int nColumn) {
//	ASSERT(m_Columns[nColumn].m_nType==W_TYPE_TEXT);
	if (nColumn>=m_nNumColumns) {
		return NULL;
	}
	return (CWslWatchText*) m_apWatches[nColumn + nRow * m_nNumColumns + m_nNumRows];
}

CWslWatchColorBox *CSrcNiceTable::GetColorBox(int nRow, int nColumn) {
//	ASSERT(m_Columns[nColumn].m_nType==W_TYPE_COLORBOX);
	return (CWslWatchColorBox*) m_apWatches[nColumn + nRow * m_nNumColumns + m_nNumRows];
}

/*********************************************************************** eof **/
