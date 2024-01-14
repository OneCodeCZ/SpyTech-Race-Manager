/******************
*                 *  WslStdSerialCom.h
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

#if !defined(AFX_WSLSTDSERIALCOM_H__8010FB80_0F90_11D4_AAEF_004F49094CF9__INCLUDED_)
#define AFX_WSLSTDSERIALCOM_H__8010FB80_0F90_11D4_AAEF_004F49094CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PORT_COM1 0
#define PORT_COM2 1
#define PORT_COM3 2
#define PORT_COM4 3

/*******************************************************************************
*
*  CWslStdSerialCom
*
*  Typical sequence of calls:
*  - SelectPort(PORT_COM2);
*  - Connect();
*  - SetupDialog(m_hWnd);
*  - Write, Read..
*  - Disconnect();
*
*******************************************************************************/

class CWslStdSerialCom  
{
	COMMTIMEOUTS   m_Timeouts;
	COMMCONFIG     m_Config;
	CString        m_sName;
	HANDLE         m_hCom;

	BOOL           m_bConnected;
public:
	CWslStdSerialCom();

	int SelectPort(int nPortIdx);

	// Invokes the standard port set-up dialog and stores the result into m_Config.
	int SetupDialog(HWND hParent);
	int Setup();
	// Connects to the port. Returns 0 if failure.
	int Connect(BOOL bDisplayError = TRUE);

	// Returns number of bytes written
	int Write(const void *pcBuffer, int nNumBytes);
	// Returns number of bytes read
	int Read(void *pBuffer, int nNumBytes);
	// Disconnects from port
	int Disconnect();

	int SaveConfig();
	int LoadConfig();

	virtual ~CWslStdSerialCom();

};

#endif // !defined(AFX_WSLSTDSERIALCOM_H__8010FB80_0F90_11D4_AAEF_004F49094CF9__INCLUDED_)
