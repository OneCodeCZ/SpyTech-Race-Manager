///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define DB_DRV_NAME_CHARS				31
#define DB_DRV_SURNAME_CHARS		31
#define DB_DRV_NICKNAME_CHARS		31
#define DB_DRV_CLUB_CHARS				15
#define DB_DRV_LICENCE_CHARS		15

//*****************************************************************************
struct CDbDrv
{
public:
	char	m_szName[DB_DRV_NAME_CHARS+1];
	char	m_szSurname[DB_DRV_SURNAME_CHARS+1];
	char	m_szNickname[DB_DRV_NICKNAME_CHARS+1];
	DWORD	m_dwDummy;			// former RC
	char	m_szClubCountry[DB_DRV_CLUB_CHARS+1];
	char	m_szLicence[DB_DRV_LICENCE_CHARS+1];
	char	m_szDummy[34];
	DWORD m_bMemberOfAssoc;			// former licence

public:
	void Init();
};
