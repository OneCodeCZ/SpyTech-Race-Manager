///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWsfObList
// | P |  /  |  |  ~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 02 04 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WsfList.h"
#include "WsfObList.h"

//*****************************************************************************
void CWsfObList::Serialize(CArchive& ar)
{
	POSITION pos;
	CObject *pObj;
	int n;

	if (ar.IsStoring())
	{
		ar << m_nNumItems;
		for (pos=GetHead(); pos; GetNext(pos)) 
		{
			ar << GetAt(pos);
		}
	}
	else
	{
		RemoveAll();
		ar >> n;

		for (int i=0; i<n; i++)
		{
			ar >> pObj;
			AddTail(pObj);
		}
	}
}

