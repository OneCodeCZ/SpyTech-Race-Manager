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
class CWslButton : public CWslWnd
{
public:
	CWslButton();
	virtual ~CWslButton();

	// Sets the image for the button.
	// The button must have the BS_BITMAP or BS_ICON style set.
	// nType can be IMAGE_BITMAP or IMAGE_ICON 
	// Returns the previous image handle.
	HANDLE SetImage(HANDLE hImage, int nType);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
