///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*
This class represents an UI command. An UI command can be visualizied as a
toolbar button or a menu item.

Command text and description is loaded from the string table. The string
must have the same ID as the command. Format of the string is as follows:<BR><BR>

command_description\ncommand_text[\taccelerator]
*/

class CWsfApp;

/******************************************************************************/
class WSL_API CWsfCmd  
{
friend CWsfApp;

public:
	enum TYPES 
	{ 
		SIMPLE, COMBO_BOX, 
		DROP_DOWN_MENU,					// Last used command accessible
		DROP_DOWN_MENU_ONLY,		// No last used command feature
		DROP_DOWN_CUSTOM
	};

public:
	TYPES		m_nType;
	int		m_nId;
	CString	m_sTextId;

	int		m_nImgIdx;
	BOOL		m_bShowText;
	BOOL		m_bShowImage;
	CRuntimeClass*	m_pClass;	// combo box or custom drop down menu
	HMENU		m_hMenu;				// drop down menu
	int		m_nDefCmdId;		// for drop down menu
	int		m_nWidth;			// width of the combo box
	BOOL		m_bSeparateArrow;	// true=the arrow is separated from the button

protected:
   CString  m_sText;
   CString  m_sAccel;
   CString  m_sDesc;

protected:
	//---------------------------------------------------------------------------
	// Creation
	//---------------------------------------------------------------------------

	// Construction of an generic UI command
	CWsfCmd(int nId, const char* pcszTextId, BOOL bShowText, BOOL bShowImage);
   
   // Cloning a command
   CWsfCmd(const CWsfCmd* pCmd, int nNewId, const char* pcszNewTextId);

	~CWsfCmd();	

	// Creates simple command.
	void InitSimple(int nImgIdx);		

	// Creates combo-box command.
	void InitComboBox(CRuntimeClass* pComboClass, int nWidth);

	// Creates command with dropdown arrow. When the arrow is pressed
	// dropdown menu is displayed. The default command is then updated
	// to the command selected from the menu. 
	void InitDropDown(int nDefCmdId, HMENU hMenu);
	void InitDropDownOnly(int nImgIdx, HMENU hMenu);

	// Creates command with dropdown arrow. When the arrow is pressed
	// custom window is displayed.
	void InitDropDown(int nImgIdx, CRuntimeClass* pWndClass);
	
public:
	//---------------------------------------------------------------------------
	// WSF User API
	//---------------------------------------------------------------------------

   const CString& GetText() const { return m_sText; }
   CString GetTextAndAccel() const;
   CString GetToolTip() const;
   const CString& GetDesc() const { return m_sDesc; }
	inline BOOL IsDropDown() const { return m_nType==DROP_DOWN_MENU || m_nType==DROP_DOWN_CUSTOM; }

   //---------------------------------------------------------------------------
   // INTERNALS
   //---------------------------------------------------------------------------
protected:

   void LoadTextAndAccel();
};




















