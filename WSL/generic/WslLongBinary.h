///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWslLongBinary
{
protected:
	int		m_nSize;
	BYTE*	m_pbyData;			// memory allocated by the NEW operator;

public:
	CWslLongBinary();
	CWslLongBinary(int nSize);
	~CWslLongBinary();
	
	void Alloc(int nSize);
	void Attach(void* pData, int nSize);
	void Realloc(int nSize);
	BYTE* Detach();
	void Free();

	inline BYTE* GetData() { return m_pbyData; }
	inline int GetSize() const { return m_nSize; }
};
