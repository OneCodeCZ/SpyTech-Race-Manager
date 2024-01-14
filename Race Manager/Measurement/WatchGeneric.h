/******************
*                 *  WatchGeneric.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  03-05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"

/*******************************************************************************
*
*  LIBRARY OF SCREEN OUTPUT
*
*  Initial coding by Adam J. Sporka
*  03-MAR-2000
*
*  Well. Now it is 10-APR-2000 and the software is still not finnished.
*  Goin' to watch X-Files.
*
*  I'm back. It was stupid.
*
*  Now it is 03-MAY-2000 and the software is STILL not finnished.
*
*  Now it is 17-JUL-2000 and the software IS STILL NOT FINNISHED.
*  We have several betatests done at which I've decided to implement all the
*  vital functions again.
*
*  Now it is 26-AUG-2000 and ... huh? What're you saying? You say it is
*  finnished? I'm sorry to say that but it is not ... ouch
*
*******************************************************************************/

#if !defined(AFX_WATCHGENERIC_H__95D5980E_F08B_11D3_B030_0000B49E3BD7__INCLUDED_)
#define AFX_WATCHGENERIC_H__95D5980E_F08B_11D3_B030_0000B49E3BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WslBitmap.h"

#define C_BLACK     0
#define C_WHITE     1
#define C_BLUE1     2
#define C_BLUE2     3
#define C_BLUE3     4
#define C_GREY1     5
#define C_GREY2     6
#define C_RED1      7
#define C_GREEN1    8
#define C_RED2      9
#define C_GREEN2   10

/*******************************************************************************
*
*  CWslGdiStuff
*
*  This class is to manage all GDI objects used by the application
*
*******************************************************************************/

class CWslGdiStuff {
//    ~~~~~~~~~~~~
private:
	CWslArray<CFont *> m_apFonts;
	CWslArray<CPen *> m_apPens;
	CWslArray<CBrush *> m_apBrushes;
	CWslArray<COLORREF> m_aColorRefs;

	CWslArray<CGdiObject *> m_apTempObjects;

	DEVMODE m_OldVideoMode;
	DEVMODE m_CurrentVideoMode;

   BOOL m_bInitialized;

public:

	CWslBitmap m_SponsorBitmap;
   CWslBitmap m_RedDim;
   CWslBitmap m_GreenDim;
   CWslBitmap m_RedLit;
   CWslBitmap m_GreenLit;

	CWslGdiStuff();
	~CWslGdiStuff();

	// Register the object and return appropriate index value
	int RegisterPen(CPen *pPen) { m_apPens.Append(pPen); return m_apPens.GetSize()-1; };
	int RegisterBrush(CBrush *pBrush) { m_apBrushes.Append(pBrush); return m_apBrushes.GetSize()-1; };
	int RegisterGlobalColor(COLORREF cr);
	int RegisterFont(CFont *pFont) { m_apFonts.Append(pFont); return m_apFonts.GetSize()-1; };
	int RegisterColorRef(COLORREF cr) { m_aColorRefs.Append(cr); return m_aColorRefs.GetSize()-1; };
	int CreateFont(const char *pcszFaceName, int nPtSize, BOOL bBold=FALSE, BOOL bItalic=FALSE, BOOL bUnderlined=FALSE);

	// ChangeScreenResolution
	// PARAMTERS
	//   nWidth, nHeighth .. resolution of the screen
	//   nBitDepth .. number of bits per pixel
	//   nRefreshRate .. refresh rate
	// If nBitDepth and nRefreshRate are not specified, the function tries to use
	// the best settings possible
	// RETURNS 0 if buga, 1 if OK.
	int ChangeScreenResolution(int nWidth, int nHeight, int nBitDepth=-1, int nRefreshRate=-1);
	// Keeps the current resolution
	int SaveScreenResolution();
	// Restores the resolution kept
	int RestoreScreenResolution();

	// Get the object from database
	CPen *GetPen(int nIdx) { return m_apPens[nIdx]; };
	CBrush *GetBrush(int nIdx) { return m_apBrushes[nIdx]; };
	CFont *GetFont(int nIdx) { return m_apFonts[nIdx]; };
	COLORREF GetColorRef(int nIdx) { return m_aColorRefs[nIdx]; };

	int PushObject(CGdiObject *pObject) { m_apTempObjects.Push(pObject); return 1; };
	CGdiObject *PopObject() { return m_apTempObjects.Pop(); };

	void Init();
};

extern CWslGdiStuff gdi;

/*******************************************************************************
*
*  CWslWatchGeneric
*
*  Implementation of generic watch object. These objects represents a view to
*  textual or graphic information contained in full screen nice looking outputs.
*  It is designed to provide very fast response but it is not designed to
*  handle animations.
*
*  The fast response for multiple data on the screen is achieved by drawing
*  only those objects that have not been modified since the last redraft.
*  In order not to endanger the time critical operations which can be the
*  primary task of the application, the multithreading issues are to be
*  considered.
*
*  Common features are maximum extent allowed, background color and content
*  alignment. Supported modes are left, right and centered.
*
*******************************************************************************/

#define W_ALIGN_DEFAULT 0
#define W_ALIGN_LEFT    1
#define W_ALIGN_RIGHT   2
#define W_ALIGN_CENTER  3

#define RW					RegisterWatch
#define NEW_TEXT        new CWslWatchText
#define NEW_PROGRESS    new CWslWatchProgress
#define SE              SetEnvironment

#define W_TYPE_DUMMY     0
#define W_TYPE_TEXT      1
#define W_TYPE_BITMAP    2
#define W_TYPE_COLORBOX  3

class CWslWatchGeneric  
//    ~~~~~~~~~~~~~~~~
{
public:
	DWORD m_dwID;

	// Position of the object
	int m_nPosX;
	int m_nPosY;

	// Extent allowed
	int m_nExtentX;
	int m_nExtentY;

	int m_nFlashCounter;

	BOOL m_bFlashing;
	BOOL m_bFramed;
	CRITICAL_SECTION m_CriSec;

	BOOL m_bHide;
	BOOL m_bHidden;

protected:
	// If this is set, the object has to be redrawn
	BOOL m_bModified;

public:
	CWslWatchGeneric();

	// Draws the object to screen if m_bModified flag has been set
	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);

	// Return the 'modified' flag
	virtual BOOL IsModified() { return m_bModified; };

	// nNewMode can be either SW_SHOW or SW_HIDE. The watch is hidden in
	// the next update.
	virtual void Visibility(int nNewMode) { m_bHide = (nNewMode==SW_SHOW) ? FALSE : TRUE; };

	virtual ~CWslWatchGeneric();

};

/*******************************************************************************
*
*  CWslWatchText
*
*  This is implementation of nice text
*
*******************************************************************************/

class CWslWatchSource;
//    ~~~~~~~~~~~~~~~

class CWslWatchText : public CWslWatchGeneric
//    ~~~~~~~~~~~~~
{
	int m_nFontIdx;
	int m_nBrushIdx;
	int m_nPenIdx;

public:
	// Foreground color
	int m_nFgColorIdx;

	CString m_sString;
	int m_nAlignMode;

	CWslWatchText();
	CWslWatchText(const char *pcszString, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner, BOOL bFlashing=FALSE);

	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);
	int SetText(CString sNewString);

	int ChangePos(int nX, int nY, int nSX, int nSY);
	int ChangeFont(int nFontIdx);

	// W_ALIGN_DEFAULT, W_ALIGN_LEFT, W_ALIGN_RIGHT, W_ALIGN_CENTER
	int ChangeAlignMode(int nAlignMode);

	// Display number. nDigits -- number of digits padded with zeroes. (-1 .. no padding)
	int SetText(int nNumber, int nDigits=-1);

	virtual ~CWslWatchText();
};

/*******************************************************************************
*
*  CWslWatchBitmap
*
*******************************************************************************/

class CWslWatchBitmap : public CWslWatchGeneric 
//    ~~~~~~~~~~~~~~~
{
	CWslBitmap *m_pBitmap;
public:
	CWslWatchBitmap(CWslBitmap *pBitmap, int nX, int nY, CWslWatchSource *pOwner);
	CWslWatchBitmap(CWslBitmap *pBitmap, int nX, int nY, int nExtentX, int nExtentY, CWslWatchSource *pOwner);

	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);
	int SetBitmap(CWslBitmap *pBitmap);

	~CWslWatchBitmap();
};

/*******************************************************************************
*
*  CWslWatchColorBox
*
*  This object is to display colors within a box of specified dimensions with
*  selected pen.
*
*******************************************************************************/

class CWslWatchColorBox : public CWslWatchGeneric
//    ~~~~~~~~~~~~~~~~~
{
	COLORREF m_crColor;
	int m_nPenIdx;
public:
	CWslWatchColorBox();
	CWslWatchColorBox(COLORREF crColor, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner);

	int ChangePos(int nX, int nY, int nSX, int nSY);

	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);
	int SetColor(COLORREF crColor);

	~CWslWatchColorBox();
};

/*******************************************************************************
*
*  CWslWatchColorCircle
*
*  This object is to display colors within a box of specified dimensions with
*  selected pen.
*
*******************************************************************************/

class CWslWatchColorCircle : public CWslWatchGeneric
//    ~~~~~~~~~~~~~~~~~~~~
{
	int m_nBrushIdx;
	int m_nPenIdx;
public:
	CWslWatchColorCircle();
	CWslWatchColorCircle(int nBrushIdx, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner);

	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);
	int SetColor(int nBrushIdx);

	// nNewMode can be either SW_SHOW or SW_HIDE. The watch is hidden in
	// the next update.
	virtual void Visibility(int nNewMode) { 
		m_bHide = (nNewMode==SW_SHOW) ? FALSE : TRUE; 
		m_bModified = TRUE;
		if (nNewMode == SW_SHOW) m_bHidden = FALSE;
	};


	~CWslWatchColorCircle();
};

/*******************************************************************************
*
*  CWslWatchProgress
*
*******************************************************************************/

class CWslWatchProgress : public CWslWatchGeneric {
//    ~~~~~~~~~~~~~~~~~
	// Min. value, max. value and number of division lines
	int m_nMin, m_nMax, m_nDiv;
	int m_nCurrent;

	int m_nLinePenIdx;
	int m_nRemainBrushIdx;
	int m_nElapsedBrushIdx;
public:
	CWslWatchProgress(int nMin, int nMax, int nDiv, int nX, int nY, int nSX, int nSY, CWslWatchSource *pOwner);
	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);
	int SetCurrent(int nCurrent);
	int SetCurrent(int nCurrent, int nMin, int nMax, int nDiv);
	~CWslWatchProgress();
};

/*******************************************************************************
*
*  CWslWatchSource
*
*  This class implements the functionality of watch-updating trhead. It is
*  designed to have information about all watches and perform their update
*  from time to time
*
*******************************************************************************/

class CWslWatchSource : public CWslWatchGeneric {
//    ~~~~~~~~~~~~~~~
public:
    CWslArray<CWslWatchGeneric*> m_apWatches;
public:
	// Align mode
	int m_nAlignMode;

	// Index to font that will be default within the source
	int m_nFontIdx;

	// Index to brush that will be used as background
	int m_nBrushIdx;

	int m_nPenIdx;

	int m_nFontColorIdx;

	CWslWatchSource(CWslWatchSource *pOwner);
	CWslWatchSource();
	~CWslWatchSource();

	// Add a new watch into list of watches
	int RegisterWatch(CWslWatchGeneric *pWatch);

	void EmptyWatchList();

//	// Render all watches registered
//	int RenderWatches(CDC *pDC, BOOL bFull=FALSE);

	// Get watch of given index
	CWslWatchGeneric *GetWatch(int nIdx) { CWslWatchGeneric *pWatch=m_apWatches[nIdx]; return pWatch; };

	// Get watch of given index and specific type
	CWslWatchText *GetText(int nIdx) { return (CWslWatchText*) m_apWatches[nIdx]; };
	CWslWatchBitmap *GetBitmap(int nIdx) { return (CWslWatchBitmap*) m_apWatches[nIdx]; };
	CWslWatchProgress *GetProgress(int nIdx) { return (CWslWatchProgress*) m_apWatches[nIdx]; };
	CWslWatchSource *GetSource(int nIdx) { return (CWslWatchSource*) m_apWatches[nIdx]; };
	CWslWatchColorBox *GetBox(int nIdx) { return (CWslWatchColorBox*) m_apWatches[nIdx]; };

	void DeleteLastWatch();
	void DestroyAll();

	// Set the environment properties
	void SetEnvironment(int nFontColorIdx=-1, int nFontIdx=-1, int nAlignMode=-1, int nBrushIdx=-1, int nPenIdx=-1, int nFrame=-1);

	virtual void Visibility(int nNewMode);

	virtual int Draw(CDC *pDC, BOOL bFull=FALSE);
};

/*******************************************************************************
*
*  CSrcNiceTable
*
*******************************************************************************/

struct TABLE_COLUMN {
//     ~~~~~~~~~~~~
	int m_nType;         // W_TYPE_TEXT, W_TYPE_BITMAP, W_TYPE_COLORBOX
	int m_nPos;          // Offset from left
	int m_nWidth;        // width of the item
	int m_nAlign;        // Alignment mode
	int m_nFontIdx;      // Font
	int m_nBkIdx;        // Background color
	int m_nFgIdx;        // Foreground color
	BOOL m_bShrinkable;  // May be shrinked if not enough room
};

class CSrcNiceTable : public CWslWatchSource {
//    ~~~~~~~~~~~~~
public:
	int m_nMaxNumItems;
	int m_nHeight;
	int m_nNumColumns;
	int m_nNumRows;

	int m_nColumnGap;

private:
	CWslArray<TABLE_COLUMN> m_Columns;
	int m_nTotalWidth;

	// Indices to watches registered.
	CWslArray<int> m_Indices;

public:
	// Constructor only
	CSrcNiceTable(CWslWatchSource *pOwner);

	// Set the rectangle which is to be filled up with the table
	void SetParams(int nX, int nY, int nSX, int nSY, int nHeight, int nColumnGap);

	// Create system of collumns
	void RegisterColumn(int nWidth, int nAlign, int nType, int nFgIdx, int nBkIdx, int nFontIdx, BOOL bShrinkable=FALSE);

	// Create fields according to parameters specified.
	// RETURNS: Number of table entries that may be displayed
	int BuildFields(int nMinimum);

	// Get the appripriate field
	CWslWatchText *GetTextField(int nRow, int nColumn);
	CWslWatchColorBox *GetColorBox(int nRow, int nColumn);
	
	~CSrcNiceTable();
};

/*******************************************************************************
* 
*  The following API functions implement operations that control display thread
*
*******************************************************************************/

// IMPLEMENTATION PART
// Programmers using this system may ignore this part of src code.
// I wonder, who else will read this, since I'm the only supposed programmer...

// Definitions of commands
#define W_CMD_NOTHING       0	  // This command causes the thread to do nothing
#define W_CMD_CHANGE_SRC    1   // Thread has to change the source of information
#define W_CMD_FULL_REDRAW   2   // Thread is asked to redraw the whole screen
#define W_CMD_END         255   // Thread is kindly asked to die

// Input from..
extern volatile CWslWatchGeneric *Watch_pCurrentSource;  // Current source of watches
extern volatile CWslWatchGeneric *Watch_pNewSource;      // The new source of watches

// Output to..
extern CWnd *Watch_pWnd;        // Destination of displays

// Status and ctrl variables
extern volatile BOOL Watch_bKill;		// The thread is to be shut down
extern volatile BOOL Watch_bFull;		// The thread ought to redraw all the watches
extern volatile DWORD Watch_dwMessage;	// A command for the thread to change behaviour
extern volatile DWORD Watch_dwParam;	    // A parameter for the command

// Required by CreateThread function
extern DWORD Watch_dwReturnVal; // A value that will be returned by the threadproc
extern HANDLE Watch_hThread;	// Thread handle
extern DWORD Watch_dwThreadID;	// Required by CreateThread function

/******************************************************************************/

// INTERFACE PART

// Creates the display thread that will update watches in given CWnd.
// The pointer MUST be valid. This window is supposed to serve as source
// of pointer to its device context.
int Watch_CreateThread(CWnd *pWnd);
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Destroys the display thread. This is the blocking call. The value
// is returned not until the thread is stopped.
int Watch_KillThread();
//  ~~~~~~~~~~~~~~~~~~~

// This function causes the thread to change the source of watches.
int Watch_SetSource(CWslWatchGeneric *pSource);
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// This function busy-waits to end of one update cycle.
// WARNING: Do NOT call this function unless the watch thread does not
//          run. Otherwise you will busy wait till the end of Universe :-).
void Watch_WaitOneUpdate();
//   ~~~~~~~~~~~~~~~~~~~~~~

// Call this to force all watches to be updated within the next update
// cycle.
int Watch_RedrawAll();
//  ~~~~~~~~~~~~~~~~~~

#endif // !defined(AFX_WATCHGENERIC_H__95D5980E_F08B_11D3_B030_0000B49E3BD7__INCLUDED_)
