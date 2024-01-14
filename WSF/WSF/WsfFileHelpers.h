///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  File System Helpers
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 
// | H |  _||   |
// |___|________|   
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include <generic\WslWin32Exception.h>

///////////////////////////////////////////////////////////////////////////////
// Creates the given UNC path. 
// nPathOffset is offset from the string start where the path begins.
// Throws CWsfException*.
///////////////////////////////////////////////////////////////////////////////

void WSL_API WsfCreatePath(const char *pcszPath, int nPathOffset = 0);

///////////////////////////////////////////////////////////////////////////////
// Removes or just cleans the given directory and if specified all subdirs.
// throws CException*
// WSL_RMDIR_KEEP_SUBDIRS and WSL_RMDIR_CONTENT flags are mutally exclusive.
///////////////////////////////////////////////////////////////////////////////

#define WSL_RMDIR_RECURSIVE							1		
#define WSL_RMDIR_CONTENT								2			// deletes all content
#define WSL_RMDIR_KEEP_SUBDIRS					4			// deletes all content but subdirs

void WSL_API WsfRemoveDirectory(const char *pcszPath, DWORD dwFlags = WSL_RMDIR_RECURSIVE); 

///////////////////////////////////////////////////////////////////////////////
// Copies the given directory to other directory
// throws CException*
///////////////////////////////////////////////////////////////////////////////

#define WSL_CPYDIR_RECURSIVE						1
#define WSL_CPYDIR_OVERWRITE						2

void WSL_API WsfCopyDirectory(const char *pcszSrcPath, 
											const char *pcszDestPath, 
											DWORD dwFlags = WSL_CPYDIR_RECURSIVE | WSL_CPYDIR_OVERWRITE);


///////////////////////////////////////////////////////////////////////////////
// Sets attributes of all files in given directory.
// throws CException*
///////////////////////////////////////////////////////////////////////////////

#define WSL_SFA_RECURSIVE						1

void WSL_API WsfSetDirAttributes(const char *pcszPath, 
												 DWORD	dwAttr, 
												 DWORD dwFlags = WSL_SFA_RECURSIVE);

///////////////////////////////////////////////////////////////////////////////
// Checks if the given file exists.
///////////////////////////////////////////////////////////////////////////////

bool WSL_API WsfFileExists(const char *pcszFile);

///////////////////////////////////////////////////////////////////////////////
// Loads given file to memory
// The memory is allocated using malloc
///////////////////////////////////////////////////////////////////////////////

WSL_API char *WsfLoadDat(const char *pcszFileName, int &nNumRead);
WSL_API char *WsfLoadText(const char *pcszFileName, int &nNumRead);
