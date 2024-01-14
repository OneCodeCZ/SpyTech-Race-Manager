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

#include "stdafx.h"
#include "WslFS.h"
#include "WslFileSearch.h"
#include "WslFile.h"

//*****************************************************************************
void CWslFS::RemoveDirectory(const char* pcszPath, int nFlags)
{
	CWslString s;
	CWslFileSearch f;

	ASSERT(pcszPath);
	_RPT1(0,"CWslFS::RemoveDirectory(): %s\n", pcszPath);

	// Remove files in this folder and recurse for subfolders
	
	s = pcszPath;
	f.FindFirst(s + "\\*.*");

	for (; f.IsFound(); f.FindNext())
	{
		ASSERT(f.GetFileName());
		if (!strcmp(f.GetFileName(), ".") || !strcmp(f.GetFileName(), "..")) continue;

		s.Format("%s\\%s", pcszPath, f.GetFileName());
//		_RPT1(0,"CWslFS::RemoveDirectory(): %s\n", s);

		if (!f.IsDir())
		{
			if (!::DeleteFile(s)) throw new CWslWin32Exception();
		}
		else if (!(nFlags & NOT_RECURSIVE)) 
		{
			RemoveDirectory(s, nFlags & ~CONTENT_ONLY);
		}
	}

	f.Close();

	// Remove this folder.
	// It's OK if the folder doesn't exists.

	if(!(nFlags & (KEEP_SUBDIRS | CONTENT_ONLY)))
	{
		if(!::RemoveDirectory(pcszPath)) 
		{
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
				throw new CWslWin32Exception();
		}
	}
}

//*****************************************************************************
void CWslFS::CopyDirectory(const char* pcszSrcPath, 
													 const char* pcszDestPath, 
													 int nFlags)
{
	CWslString s, s2;
	CWslFileSearch f;

	ASSERT(pcszSrcPath);
	ASSERT(pcszDestPath);

	// Create the folder.

	if (!(nFlags & ROOT_MUST_EXIST))
	{
		if (!::CreateDirectory(pcszDestPath, NULL)) 
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS) throw new CWslWin32Exception();
		}
	}

	// Copy content.

	s = pcszSrcPath;
	f.FindFirst(s + "\\*.*");

	for (; f.IsFound(); f.FindNext())
	{
		ASSERT(f.GetFileName());
		if (!strcmp(f.GetFileName(), ".") || !strcmp(f.GetFileName(), "..")) continue;
		
		s = pcszDestPath; 
		s += '\\';
		s += f.GetFileName();

		s2 = pcszSrcPath; 
		s2 += '\\';
		s2 += f.GetFileName();

//		_RPT2(0, "CWslFS::CopyDirectory(): \"%s\" ->\n              \"%s\"\n", s2, s);

		if (!f.IsDir())
		{
			if (!::CopyFile(s2, s, nFlags & NO_OVERWRITE))
				throw new CWslWin32Exception();
		}
		else if (!(nFlags & NOT_RECURSIVE))
		{
			CopyDirectory(s2, s, nFlags & ~ROOT_MUST_EXIST);
		}
	}

	f.Close();
}

//*****************************************************************************
void CWslFS::__SetDirectoryTime(const char* pcszPath,
																FILETIME& ft, int nFlags)
																throw (CWslWin32Exception*)
{
	CWslString s;
	CWslFileSearch fs;

	ASSERT(pcszPath);

	s = pcszPath;
	fs.FindFirst(s + "\\*.*");

	for (; fs.IsFound(); fs.FindNext())
	{
		ASSERT(fs.GetFileName());
		if (!strcmp(fs.GetFileName(), ".") || !strcmp(fs.GetFileName(), "..")) continue;

		s = pcszPath; 
		s += '\\';
		s += fs.GetFileName();

	//	_RPT1(0, "CWslFS::SetDirectoryTime(): %s\n", s);

		if (!fs.IsDir())
		{
			CWslFile f;
			f.Open(s);
			if (!::SetFileTime(f.m_hHandle, &ft, &ft, &ft))
				throw new CWslWin32Exception();
		}
		else if (!(nFlags & NOT_RECURSIVE))
		{
			__SetDirectoryTime(s, ft, nFlags);
		}
	}

	fs.Close();
}

//*****************************************************************************
void CWslFS::SetDirectoryTime(const char* pcszPath, int nFlags)
															throw (CWslWin32Exception*)
{
	SYSTEMTIME st;
	FILETIME ft;

	ASSERT(pcszPath);

	GetSystemTime(&st);
	VERIFY(SystemTimeToFileTime(&st, &ft));

	__SetDirectoryTime(pcszPath, ft, nFlags);
}

//*****************************************************************************
void CWslFS::SetDirectoryAttributes(const char* pcszPath, DWORD dwAttr, int nFlags) 
																		throw (CWslWin32Exception*)
{
	CWslString s;
	CWslFileSearch fs;
	ASSERT(pcszPath);

	s = pcszPath;
	fs.FindFirst(s + "\\*.*");

	for (; fs.IsFound(); fs.FindNext())
	{
		ASSERT(fs.GetFileName());
		if (!strcmp(fs.GetFileName(), ".") || !strcmp(fs.GetFileName(), "..")) continue;

		s = pcszPath; 
		s += '\\';
		s += fs.GetFileName();

		//_RPT1(0, "CWslFS::SetDirectoryAttributes(): %s\n", s);

		if (!fs.IsDir())
		{
			if (!::SetFileAttributes(s, dwAttr)) throw new CWslWin32Exception();
		}
		else if (!(nFlags & NOT_RECURSIVE))
		{
			SetDirectoryAttributes(s, dwAttr, nFlags);
		}
	}

	fs.Close();
}

//*****************************************************************************
void CWslFS::CreatePath(const char* pcszPath) throw (CWslWin32Exception*)
{
	const char *p, *x = pcszPath;
	char ch;

	ASSERT(pcszPath);

	// Skip drive or network share

	if (x[0] && x[1])
	{
		if (x[1]==':')
		{
			x += 3;								/* skip c:\       */
		}
		else if (x[0]=='\\' && x[1]=='\\')
		{
			p = strchr(x+2, '\\');
			if (!p) return;				/* only \\xxxxxx  */
			x = strchr(p+1, '\\');
			if (!x) return;				/* only \\server\  */
			x++;									/* ok \\server\share\ skiped */
		}
	}

	// Create folders

	while (*x)
	{
		p = strchr(x, '\\');
		if (!p) p = x + strlen(x);

		ch = *p; *(char*)p = 0;			// DIRTY!!!!
		BOOL b = CreateDirectory(pcszPath, NULL);
		*(char*)p = ch;

		if (!b)
		{
			DWORD x = GetLastError();
			if (x != ERROR_ALREADY_EXISTS) throw new CWslWin32Exception();
		}

		x = p;
		if (ch) x++;
	}
}













