///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  DirectX Joystick Wrapper
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslJoystick.h"

//*****************************************************************************
CWslJoystick::CWslJoystick(IDirectInput8* pdi)
{
	ASSERT(pdi);
	m_pdi = pdi;
	pdi->AddRef();
}

//*****************************************************************************
CWslJoystick::~CWslJoystick();
{
	pdi->Release();
}

//*****************************************************************************
BOOL CALLBACK CWslJoystick::DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, 
																									LPVOID pvRef)
{
	CWslArray<DIDEVICEINSTABCE>* pDevices = (CWslArray<DIDEVICEINSTABCE>*) pvRef;
	*pDevices += *lpddi;
	return DIENUM_CONTINUE;
}

//*****************************************************************************
BOOL CWslJoystick::EnumJoysticks(CWslArray<DIDEVICEINSTABCE>& aDevices)
{
	HRESULT hr;

	hr = m_pdi->EnumDevices(DI8DEVCLASS_GAMECTRL, 
		DIEnumDevicesCallback, 
		&aDevices,
		DIEDFL_ATTACHEDONLY);	

	if (hr != DI_OK)
	{
		_RPT1(0,"CWslJoystick::EnumJoysticks: EnumDevices() failed (%d)!\n", hr);
		return false;
	}

	return true;
}

//*****************************************************************************
BOOL CWslJoystick::Init(GUID& guid, HWND hWnd, 
												DWORD dwCoopLevelFlags /*= DISCL_EXCLUSIVE | DISCL_FOREGROUND*/)
{
	HRESULT hr;

	ASSERT(!m_pJoy); // already inited

	hr = m_pdi->CreateDevice(guid, &m_pJoy, NULL);
	if (hr != DI_OK)
	{
		_RPT1(0,"CWslJoystick::Init: CreateDevice() failed (%d)!\n", hr);
		return false;
	}

	hr = m_pJoy->SetDataFormat(&c_dfDIJoystick2);
	if (hr != DI_OK)
	{
		m_pJoy->Release();
		m_pJoy = NULL;
		_RPT1(0,"CWslJoystick::Init: SetDataFormat() failed (%d)!\n", hr);
		return false;
	}

	hr = g_pJoystick->SetCooperativeLevel(hWnd, dwCoopLevelFlags); 
	if (hr != DI_OK)
	{
		m_pJoy->Release();
		m_pJoy = NULL;
		_RPT1(0,"CWslJoystick::Init: SetCooperativeLevel() failed (%d)!\n", hr);
		return false;
	}


}







