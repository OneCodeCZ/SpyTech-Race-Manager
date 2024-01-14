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

#pragma once

#include <prerelease\dinput.h>

#define WSL_JOY_XAXIS			0
#define WSL_JOY_YAXIS			1
#define WSL_JOY_ZAXIS			2

class CWslArray;

//*****************************************************************************
class CWslJoystick
{
protected:
	IDirectInput8*				m_pdi;
	IDirectInputDevice8*	m_pJoy;
	int										m_nNumAxes;
	int										m_nNumButtons;
	DIJOYSTATE2						m_State;


public:
  //---------------------------------------------------------------------------
	// Creation
	//---------------------------------------------------------------------------
	
	// Creates the class
	CWslJoystick(IDirectInput8* pdi);
	~CWslJoystick();

	// Enumerates all available joysticks
	BOOL EnumJoysticks(CWslArray<DIDEVICEINSTABCE>& aDevices);

	// Inits the class to use the specified joystick (creates the DI device).
	// Alse sets the cooperation level. See IDirectInputDevice.
	// GUID is gained by calling EnumJoysticks() method.
	// hWnd is the main app window.
	BOOL Init(GUID& guid, HWND hWnd,
		DWORD dwCoopLevelFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	// Destroys the DI device.
	BOOL Deinit();

	// Returns the number of axes and buttons.
	inline int GetNumAxes() { return m_nNumAxes; }
	inline int GetNumButtons() { return m_nNumButtons; }

	// Sets the value range of the given axes.
	BOOL SetAxisRange(int nAxis, int nMin, int nMax);

  //---------------------------------------------------------------------------
	// State acquiring
	//---------------------------------------------------------------------------

	// Gets the current joystick state. Polls the devices and then
	// reads its state. If the app has lost the device, it will
	// re-acquire it.
	BOOL Update();

	// Tests the state of buttons and axis.
	BOOL IsButtonPressed(int nButton);
	int	GetAxisPos(int nAxis);

protected:
	//---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------

	// Callback for enumeration
	static BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
};



