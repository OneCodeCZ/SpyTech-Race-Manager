///////////////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class CWsfTISelContext  
{
public:
	// Tree item views MUST init these variables when processing commands.
	// These variables are STATIC -> another reason that parallel selection 
	// processing is not possible! Nevertheless who would need it :-)
	// NOTE: CHFlags = command handling flags
	enum ECHFlags { MULTI=1, FIRST=2, LAST=4, CANCEL=8 };
	
	DWORD		dwCHFlags;		// ECHFlags
	int			nSelCount;		// number of items in selection
	int			nSelPos;			// position of an item in the selection
												// being handled
};
