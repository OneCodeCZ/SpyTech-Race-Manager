///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslFS - File System Helpers
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\generic\WslWin32Exception.h"
 
//*****************************************************************************
class WSL_API CWslFS
{
public:
	enum Flags
	{
		NOT_RECURSIVE=1,		// Process just the root folder.
		CONTENT_ONLY=2,			// Delete just the content, not the root folder.
		KEEP_SUBDIRS=4,			// Delete just the content, not the folders.
		NO_OVERWRITE=8,			// Fails if the target file already exists.
		ROOT_MUST_EXIST=16	// Fails if the target root folder doesn't exists.
	};

public:
  //---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------
	// NOTE: All paths are without the trailing slash!!!

	// Removes the the given directory. 
	// NOTE: File cannot be deleted if it's read only!
	static void RemoveDirectory(const char* pcszPath, int nFlags = 0)
		throw (CWslWin32Exception*);

	// Copies the given directory content.
	static void CopyDirectory(const char* pcszSrcPath, 
		const char* pcszDestPath, int nFlags = 0)
		throw (CWslWin32Exception*);

	// Sets file times of all files in the given directory to the
	// current time.
	static void SetDirectoryTime(const char* pcszPath, int nFlags = 0)
		throw (CWslWin32Exception*);

	// Sets attributes of all files in the given directory.
	static void SetDirectoryAttributes(const char* pcszPath, 
		DWORD dwAttr = FILE_ATTRIBUTE_ARCHIVE, 
		int nFlags = 0)
		throw (CWslWin32Exception*);

	// Creates the given path. The path can be either on a local disk or
	// on a network share.
	static void CreatePath(const char* pcszPath) throw (CWslWin32Exception*);

protected:

  //---------------------------------------------------------------------------
	// Internal Stuff
	//---------------------------------------------------------------------------

	static void __SetDirectoryTime(const char* pcszPath, FILETIME& ft, int nFlags) 
		throw (CWslWin32Exception*);
};












