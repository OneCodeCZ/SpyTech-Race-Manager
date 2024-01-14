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

#include "stdafx.h"
#include "main.h"
#include "WsfFileHelpers.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

//*****************************************************************************
void WsfCreatePath(const char *pcszPath, int nPathOffset /*= 0*/)
{
	const char *p, *x = pcszPath + nPathOffset;
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
		ch = *p; *(char*)p = 0;

		if (!CreateDirectory(pcszPath, NULL)) 
		{
			DWORD x = GetLastError();
			if (x != ERROR_ALREADY_EXISTS) WsfThrowWin32Exception();
		}

		*(char*)p = ch;
		x = p;
		if (ch) x++;
	}
}

//*****************************************************************************
void WsfRemoveDirectory(const char *pcszPath, DWORD dwFlags /*= WSL_RMDIR_RECURSIVE*/)
{
	CFileFind f;
	BOOL b;

	ASSERT(pcszPath);
	{
		CString s(pcszPath);
		s += "\\*.*";
		b = f.FindFile(s);
	}

	// Remove files in this folder and recurse for subfolders

	while (b) 
	{
		b = f.FindNextFile();

		TRACE1("Deleting %s\n", f.GetFilePath());

		if (f.GetFileName()[0] != '.') 
		{
			if (!f.IsDirectory())
			{
				if (!DeleteFile(f.GetFilePath())) WsfThrowWin32Exception();
			}
			else if (dwFlags & WSL_RMDIR_RECURSIVE) 
			{
				WsfRemoveDirectory(f.GetFilePath(), dwFlags & ~WSL_RMDIR_CONTENT);
			}
		}
	}

	f.Close();

	// Remove this folder

	if(!(dwFlags & (WSL_RMDIR_KEEP_SUBDIRS | WSL_RMDIR_CONTENT)))
	{
		if(!RemoveDirectory(pcszPath)) WsfThrowWin32Exception();
	}
}

//*****************************************************************************
void WsfCopyDirectory(const char *pcszSrcPath, 
											const char *pcszDestPath, 
											DWORD dwFlags /*= WSL_CPYDIR_RECURSIVE | WSL_CPYDIR_OVERWRITE*/)
{
	CFileFind f;
	BOOL b;
	CString s;

	ASSERT(pcszSrcPath);
	ASSERT(pcszDestPath);

	if (!CreateDirectory(pcszDestPath, NULL)) 
	{
		DWORD x = GetLastError();
		if (x != ERROR_ALREADY_EXISTS) WsfThrowWin32Exception(x);
	}

	s = pcszSrcPath;
	s += "\\*.*";
	b = f.FindFile(s);

	while (b) 
	{
		b = f.FindNextFile();
		
		s = pcszDestPath; 
		s += '\\';
		s += f.GetFileName();

		TRACE2("Copy %s to %s\n", f.GetFilePath(), s);

		if (f.GetFileName()[0] != '.') 
		{
			if (!f.IsDirectory())
			{
				if (!CopyFile(f.GetFilePath(), s, !(dwFlags & WSL_CPYDIR_OVERWRITE)) != ERROR_SUCCESS)
					WsfThrowWin32Exception();
			}
			else if (dwFlags & WSL_CPYDIR_RECURSIVE) 
			{
				WsfCopyDirectory(f.GetFilePath(), s, dwFlags);
			}
		}
	}

	f.Close();
}

//*****************************************************************************
void WsfSetDirAttributes(const char *pcszPath, 
												 DWORD dwAttr, 
												 DWORD dwFlags /*= WSL_SFA_RECURSIVE*/)
{
	CFileFind f;
	BOOL b;
	ASSERT(pcszPath);

	{
		CString s(pcszPath);
		s += "\\*.*";
		b = f.FindFile(s);
	}

	while (b) 
	{
		b = f.FindNextFile();

		TRACE1("Setting attributes of %s\n", f.GetFilePath());

		if (f.GetFileName()[0] != '.') 
		{
			if (!f.IsDirectory())
			{
				if (!SetFileAttributes(f.GetFilePath(), dwAttr)) WsfThrowWin32Exception();
			}
			else if (dwFlags & WSL_RMDIR_RECURSIVE) 
			{
				WsfSetDirAttributes(f.GetFilePath(), dwAttr, dwFlags);
			}
		}
	}

	f.Close();
}

//*****************************************************************************
bool WsfFileExists(const char *pcszFile)
{
	CFileFind f;
	if (f.FindFile(pcszFile)) return TRUE;
	return FALSE;
}

//*****************************************************************************
char *WsfLoadDat(const char *pcszFileName, int &nNumRead)
{
	int fh = _open(pcszFileName, _O_RDONLY | _O_BINARY);
	if (fh == -1) return NULL;
	else {
		int nSize = _filelength(fh);
		char *p = (char*)malloc(nSize);
		if (!p) {
			_close(fh);
			return NULL;
		}
		_read(fh, p, nSize);
		nNumRead = nSize;
		return p;
	}
}

//*****************************************************************************
char *WsfLoadText(const char *pcszFileName, int &nNumRead)
{
	int fh = _open(pcszFileName, _O_RDONLY | _O_BINARY);
	if (fh == -1) return NULL;
	else {
		int nSize = _filelength(fh);
		char *p = (char*)malloc(nSize+1);
		if (!p) {
			_close(fh);
			return NULL;
		}
		_read(fh, p, nSize);
		p[nSize] = 0;
		nNumRead = nSize;
		return p;
	}
}

//*****************************************************************************
