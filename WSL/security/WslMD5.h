///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  RSA MD5
// | P |  /  |  |  ~~~~~~~
// | Y | /-  |  |  Copyright (C) Langfine Ltd 
// | T |  |  \  |
// | E |  | _/  |  Modified by: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// NOTE: see md5.txt for legal stuff :-)

#include "../generic/WslString.h"

//*****************************************************************************
class WSL_API CWslMD5  
{
private:
	BYTE  m_lpszBuffer[64];		//input buffer
	ULONG m_nCount[2];				//number of bits, modulo 2^64 (lsb first)
	ULONG m_lMD5[4];					//MD5 checksum

public:
  //---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------
	
	static CWslString GetMD5(const CWslString& s);
	static CWslString GetMD5(const char* pcsz);
	static CWslString GetMD5(BYTE* pBuf, UINT nLength);

protected:
	//---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------

	CWslMD5();
	virtual ~CWslMD5() {};

	//RSA MD5 implementation
	void Transform(BYTE Block[64]);
	void Update(BYTE* Input, ULONG nInputLen);
	CWslString Final();
	inline DWORD RotateLeft(DWORD x, int n);
	inline void FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);

	//utility functions
	void DWordToByte(BYTE* Output, DWORD* Input, UINT nLength);
	void ByteToDWord(DWORD* Output, BYTE* Input, UINT nLength);
};
