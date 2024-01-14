/******************
*                 *  ScreenCfgCodes.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  26-AUG-2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

/*******************************************************************************
*
*  SCREEN CONFIGURATION CODES
*
*  Purpose of separation of these constants: NOT to force the quasi rebuild all
*  anytime the programmer wants add new code.
*
*******************************************************************************/

// Blank screen
#define CFG_NONE                    0
// Default title screen
#define CFG_TITLE                   1

/******************************************************************************/

#define CFG_QUAL_LIST_CURRENT   0x100
#define CFG_QUAL_LIST_FINAL     0x101
#define CFG_QUAL_MAIN           0x102
#define CFG_QUAL_FINAL          0x103

#define CFG_QUAL_LOW_LIMIT      0x100
#define CFG_QUAL_HIGH_LIMIT     0x1FF

/******************************************************************************/

#define CFG_RACE_GROUP_RESULTS  0x200
#define CFG_RACE_MAIN           0x201
#define CFG_RACE_FINAL          0x202

#define CFG_RACE_LOW_LIMIT      0x200
#define CFG_RACE_HIGH_LIMIT     0x2FF

/******************************************************************************/

#define CFG_CALIBRATION         0x300

/******************************************************************************/

#define CFG_TRAINING            0x400

/*********************************************************************** eof **/
