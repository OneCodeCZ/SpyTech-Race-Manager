/******************
*                 *  WslGdiStuff.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of World Spy Library.
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

CWslGdiStuff gdi;

/******************************************************************************/

int CWslGdiStuff::CreateFont(const char *pcszFaceName, int nPtSize, BOOL bBold, BOOL bItalic, BOOL bUnderlined) {

	// Create a new font object
	CFont *m_pFont=new CFont();

	// Create the GDI object
	int result=m_pFont->CreateFont(
		-nPtSize,                           // height
		0,                                  // width (0=dont care)
		0,                                  // escapement
		0,                                  // orientation
		bBold ? FW_BOLD : FW_DONTCARE,      // weight
		bItalic ? 1 : 0,                    // italic
		bUnderlined ? 1 : 0,                // underlined
		0,                                  // strike-out
		ANSI_CHARSET,                       // character set
		OUT_DEVICE_PRECIS,                  // precision of rendering
		CLIP_TT_ALWAYS,                     // clipping. who knows...
		DEFAULT_QUALITY,                    // font quality
		FF_DONTCARE,                        // font family specification.
		pcszFaceName
	);

	// If failed, return error notification
	if (!result) return -1;

	// Otherwise register a new font
	return RegisterFont(m_pFont);
}

/******************************************************************************/

CWslGdiStuff::~CWslGdiStuff() {
	int nIdx;
	ITERATE (m_apFonts, nIdx) { delete m_apFonts[nIdx]; }
	ITERATE (m_apBrushes, nIdx) { delete m_apBrushes[nIdx]; }
	ITERATE (m_apPens, nIdx) { delete m_apPens[nIdx]; }
}

#define FONT_NAME "Arial CE"

CWslGdiStuff::CWslGdiStuff() {
	CreateFont("Arial CE", 10, TRUE);
	CreateFont(FONT_NAME, 12, TRUE);
	CreateFont(FONT_NAME, 16, TRUE);
	CreateFont(FONT_NAME, 20);
	CreateFont(FONT_NAME, 24);
	CreateFont(FONT_NAME, 32);
	CreateFont(FONT_NAME, 40);
	CreateFont(FONT_NAME, 64);
	CreateFont(FONT_NAME, 96);
	CreateFont(FONT_NAME, 128);
	CreateFont(FONT_NAME, 144);
	CreateFont("STM2000 Calibration Bar Font", 6);
	CreateFont("MS Sans Serif", 8);

/*
	RegisterGlobalColor(RGB(0x00, 0x00, 0x00));  // #define C_BLACK     0
	RegisterGlobalColor(RGB(0xff, 0xff, 0xff));	// #define C_WHITE     1
	RegisterGlobalColor(RGB(0, 24, 96));			// #define C_BLUE1     2
	RegisterGlobalColor(RGB(0, 32, 192));			// #define C_BLUE2     3
	RegisterGlobalColor(RGB(39, 146, 253));		// #define C_BLUE3     4
	RegisterGlobalColor(RGB(64, 64, 64));			// #define C_GREY1     5
	RegisterGlobalColor(RGB(160, 160, 160));		// #define C_GREY2     6

	RegisterGlobalColor(RGB(128, 0, 0));		   // #define C_RED1      9
	RegisterGlobalColor(RGB(0, 128, 0));		   // #define C_GREEN1   10
	RegisterGlobalColor(RGB(255, 0, 64));		   // #define C_RED2      7
	RegisterGlobalColor(RGB(0, 192, 64));		   // #define C_GREEN2    8
*/


	RegisterGlobalColor(RGB(  0,   0,   0));  // #define C_BLACK     0
	RegisterGlobalColor(RGB(255, 255, 255));	// #define C_WHITE     1
	RegisterGlobalColor(RGB(  0,   0, 128));			// #define C_BLUE1     2
	RegisterGlobalColor(RGB(  0,   0, 255));			// #define C_BLUE2     3
	RegisterGlobalColor(RGB(  0, 255, 255));		// #define C_BLUE3     4
	RegisterGlobalColor(RGB( 64,  64,  64));				// #define C_GREY1     5
	RegisterGlobalColor(RGB(128, 128, 128));		// #define C_GREY2     6

	RegisterGlobalColor(RGB(128,   0,   0));		   // #define C_RED1      9
	RegisterGlobalColor(RGB(0,   128,   0));		   // #define C_GREEN1   10
	RegisterGlobalColor(RGB(255,   0,   0));		   // #define C_RED2      7
	RegisterGlobalColor(RGB(0,   255,   0));		   // #define C_GREEN2    8

   m_bInitialized = FALSE;
}

void CWslGdiStuff::Init()
{
   if (m_bInitialized) return;
	m_SponsorBitmap.Load(WsfGetAppPath() + "/Data/graphics/sponsor.bmp");
   m_RedDim.Load(WsfGetAppPath() + "/Data/graphics/red_dim.bmp");
   m_RedLit.Load(WsfGetAppPath() + "/Data/graphics/red_lit.bmp");
   m_GreenDim.Load(WsfGetAppPath() + "/Data/graphics/green_dim.bmp");
   m_GreenLit.Load(WsfGetAppPath() + "/Data/graphics/green_lit.bmp");
   m_bInitialized = TRUE;
}

CWslGdiStuff::RegisterGlobalColor(COLORREF cr) {
//	CDC dc;
//	dc.CreateCompatibleDC(NULL);
//	COLORREF nearest=dc.GetNearestColor(cr);
	COLORREF nearest=cr;
	RegisterBrush(new CBrush(nearest));
	RegisterColorRef(nearest);
	RegisterPen(new CPen(PS_SOLID, 1, nearest));
//	dc.DeleteDC();
	return 1;
}

CWslGdiStuff::SaveScreenResolution() {
	EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &m_OldVideoMode); 
	return 1;
}

CWslGdiStuff::RestoreScreenResolution() {
	ChangeDisplaySettings(&m_OldVideoMode, CDS_FULLSCREEN);
	return 1;
}

CWslGdiStuff::ChangeScreenResolution(int nWidth, int nHeight, int nBitDepth, int nRefreshRate) 
{
	if (!WsfGetCApp()->GetProfileInt("settings", "use_full_screen", true)) return 1;

	DWORD dwDepth[] = { 32, 24, 16, 8 };
	DWORD dwRefresh[] = { 75, 72, 60 };

	m_CurrentVideoMode.dmSize = sizeof(DEVMODE);
	m_CurrentVideoMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
	m_CurrentVideoMode.dmPelsWidth = nWidth;
	m_CurrentVideoMode.dmPelsHeight = nHeight;

	for (int r=0; r<3; r++) {
		for (int d=0; d<4; d++) {
			m_CurrentVideoMode.dmDisplayFrequency = dwRefresh[r];
			m_CurrentVideoMode.dmBitsPerPel = dwDepth[d];

			if (ChangeDisplaySettings(&m_CurrentVideoMode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL) {
				ChangeDisplaySettings(&m_CurrentVideoMode, CDS_FULLSCREEN);
				return 1;
			}
		}
	}

	return 0;
}

/******************************************************************************/