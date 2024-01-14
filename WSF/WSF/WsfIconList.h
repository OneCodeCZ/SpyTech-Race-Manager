///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfIconList : public CImageList
{
protected:
	struct CBitmapInfo
	{
		int			nBmpId;
		int			nFirstIconIdx;
	};

protected:
	CWslArray<CBitmapInfo> m_aBmps;

public:
	CWsfIconList();
	~CWsfIconList();

	// Adds a bitmap with icons to the image list.
	// Background must be the color RGB(0,255,255).
	// Returns the index of the first icon.
	int AddIcons(int nBmpIdx, BOOL bMakeGrayScale);

	// Returns an index in the image list of the given icon
	// Returns -1 if the icon has not been found.
	int GetIconIdx(int nBmpIdx, int nIconIdx);
};
