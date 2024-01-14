///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslZipFile.h"
#include "WslZipArchive.h"

/*****************************************************************************/
CWslZipFile::CWslZipFile()
{
	m_hZipArchive = NULL;
}

/*****************************************************************************/
CWslZipFile::~CWslZipFile()
{
	CloseNoException();
}

/*****************************************************************************/
void CWslZipFile::Open(unzFile hZipArchive) throw (CWslException*)
{
	ASSERT(hZipArchive);
	Close();

	m_hZipArchive = hZipArchive;
	int n = unzOpenCurrentFile(hZipArchive);
	if (n!=UNZ_OK) throw new CWslTextException("Error in CWslZipFile::Open()!");
}

/*****************************************************************************/
void CWslZipFile::Open(CWslZipArchive& ZipArchive) throw (CWslException*)
{
	Open(ZipArchive.GetHandle());
}

/*****************************************************************************/
void CWslZipFile::Close() throw (CWslException*)
{
	if (m_hZipArchive)
	{
		int n = unzCloseCurrentFile(m_hZipArchive);
		if (n!=UNZ_OK) throw new CWslTextException("Error in CWslZipFile::Close()!"); 
		m_hZipArchive = NULL;
	}
}

/*****************************************************************************/
int CWslZipFile::GetLength() throw (CWslException*)
{
	ASSERT(m_hZipArchive);
	unz_file_info i;
	int n = unzGetCurrentFileInfo(m_hZipArchive, &i, NULL,0, NULL,0, NULL,0);
	if (n!=UNZ_OK) throw new CWslTextException("Error in CWslZipFile::GetLength()!");
	return i.uncompressed_size;
}

/*****************************************************************************/
int CWslZipFile::GetPosition() throw (CWslException*)
{
	ASSERT(m_hZipArchive);
	return unztell(m_hZipArchive);
}

/*****************************************************************************/ 
int CWslZipFile::Read(void *pBuffer, int nSize) throw (CWslException*)
{
	int n = unzReadCurrentFile(m_hZipArchive, pBuffer, nSize);
	if (n<0) throw new CWslTextException("Error in CWslZipFile::Read()!");
	return n;
}

/*****************************************************************************/
CWslString CWslZipFile::GetName() const
{
	ASSERT(m_hZipArchive);
	char temp[MAX_PATH+1];
	int n = unzGetCurrentFileInfo(m_hZipArchive, NULL, temp,MAX_PATH, NULL,0, NULL,0);
	if (n!=UNZ_OK) throw new CWslTextException("Error in CWslZipFile::GetName()!");
	return CWslString(temp);
}

