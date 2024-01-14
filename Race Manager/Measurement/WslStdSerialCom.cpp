/******************
*                 *  WslStdSerialCom.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "WslStdSerialCom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  Construction / destruction
*
*******************************************************************************/

CWslStdSerialCom::CWslStdSerialCom()
{
	m_bConnected=FALSE;
	m_sName="\0";

	m_Timeouts.ReadIntervalTimeout = 100;
	m_Timeouts.ReadTotalTimeoutConstant = 100;
	m_Timeouts.ReadTotalTimeoutMultiplier = 1;
	m_Timeouts.WriteTotalTimeoutConstant = 100;
	m_Timeouts.WriteTotalTimeoutMultiplier = 1;

	LoadConfig();
}

/******************************************************************************/

CWslStdSerialCom::~CWslStdSerialCom()
{

}

/******************************************************************************/

CWslStdSerialCom::SelectPort(int nPortIdx) {
	switch(nPortIdx) {
	case 0: m_sName="COM1"; break;
	case 1: m_sName="COM2"; break;
	case 2: m_sName="COM3"; break;
	case 3: m_sName="COM4"; break;
	}
	return 1;
}

/******************************************************************************/

CWslStdSerialCom::Connect(BOOL bDisplayError) {
	m_hCom = CreateFile(
		m_sName,                 // Name of the port
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		0, 
		OPEN_EXISTING,
		0, 
		0
		);
	if (m_hCom == INVALID_HANDLE_VALUE) {
		if (bDisplayError) AfxMessageBox(IDS_ERROR_OPENIN_COMPORT, MB_ICONERROR | MB_OK);
		return 0;
	}
	// Port opened
	else
	{
		m_bConnected=TRUE;
		SetCommTimeouts(m_hCom, &m_Timeouts);
	}
	return 1;
}

/******************************************************************************/

CWslStdSerialCom::Disconnect() {
	if (m_bConnected) {
		CloseHandle(m_hCom);
		m_bConnected=FALSE;
	}
	return 1;
}

/******************************************************************************/

CWslStdSerialCom::SetupDialog(HWND hParent) {

	if (m_bConnected) {
		if (0 == GetCommConfig(m_hCom, &m_Config, &m_Config.dwSize)) {
			AfxMessageBox(IDS_COULD_NOT_READ_COMPORT_SETTINGS, MB_ICONERROR | MB_OK);
			return 0;
		}
		else {
			CommConfigDialog(m_sName, hParent, &m_Config);
			m_Config.dcb.fDtrControl = DTR_CONTROL_DISABLE;
			SetCommConfig(m_hCom, &m_Config, m_Config.dwSize);
			return 1;
		}
	}

	return 0;
}

/******************************************************************************/

CWslStdSerialCom::Setup() {
	if (m_bConnected) {
		if (!LoadConfig()) {
			GetCommConfig(m_hCom, &m_Config, &m_Config.dwSize);
			CommConfigDialog(m_sName, NULL, &m_Config);
		}
		m_Config.dcb.fDtrControl = DTR_CONTROL_DISABLE;
		SaveConfig();
		if (SetCommConfig(m_hCom, &m_Config, m_Config.dwSize)) return 1;
	}
	return 0;
}

/******************************************************************************/

int CWslStdSerialCom::LoadConfig() 
{
   try
   {
   	CWslFile f;
      CString s;

      s = WsfGetApp()->GetAppDataPath() + "comm.dat";
      if (!CWslFile::Exists(s)) s = WsfGetApp()->GetAppPath() + "data\\comm.dat";

      f.OpenReadOnly(s);
		f.Read(&m_Config, sizeof(m_Config));
		f.Close();
   }
   
   catch (CException* e)
   {
      ASSERT(false);
      e->Delete();
      return false;
   }
	
   return true;
}

/******************************************************************************/

int CWslStdSerialCom::SaveConfig() 
{
   try
   {
	   CWslFile f;
      f.Create(WsfGetApp()->GetAppDataPath() + "comm.dat");
		f.Write(&m_Config, sizeof(m_Config));
		f.Close();
   }

   catch (CException* e)
   {
      ASSERT(false);
      e->Delete();
      return false;
   }

   return true;
}

/******************************************************************************/

CWslStdSerialCom::Write(const void *pcBuffer, int nNumBytes) {
	DWORD nNumWritten;
	if (WriteFile(
		m_hCom,				// handle to the device
		pcBuffer,			// buffer to write
		nNumBytes,			// num to write
		&nNumWritten,     // number of bytes that have been written
		NULL)				// we do not use overlapped I/O
	) return nNumWritten;
	return 0;
}

/******************************************************************************/

CWslStdSerialCom::Read(void *pBuffer, int nNumBytes) {
	DWORD nNumRead;
	if (ReadFile(
		m_hCom,
		pBuffer,
		nNumBytes,
		&nNumRead,
		NULL)
	) return nNumRead;

	char temp[1024];

	DWORD dwError=GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, temp, 1024, NULL);
	AfxMessageBox(temp);


	return 0;
}

/*********************************************************************** eof **/
