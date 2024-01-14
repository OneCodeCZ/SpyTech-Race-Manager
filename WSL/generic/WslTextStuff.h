///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Text manipulation functiuns 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik and Adam J. Sporka
// | C |  |__|  |  Last changed: 2004 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "WslString.h"

// String operations
WSL_API void __fastcall strcat1(char* , char);
WSL_API void __fastcall strcat3(char* , DWORD);
WSL_API char* __fastcall strend(char* );
WSL_API char* strdot(char* s, int max);
WSL_API char* strdot(char* to, char* what, int max);
WSL_API DWORD atod16(char* s);

// Finds a word. At first it skips any spaces. It handles
// the parsing in a special way, much like in INI files. See code.
WSL_API const char* WslGetWord(const char* p, char* pBuffer);

// Finds a token from the begining of P ended by one of the
// specified separators. Zero is a default separator. Separator
// list is zero terminated.
// pszDest buffer has space for nDestSize chars including "\0".
// Returns pointer after the found word.
WSL_API const char* WslGetToken(const char* pcszSrc, 
																char* pszDest,
																int	nDestSize,
																const char* pcszSeparators);

// Skips chars <33 and >13.
WSL_API const char* WslSkipSpaces(const char* p);

// Skips specified characters. The separator list must be
// zero terminated.
WSL_API const char* WslSkipSpaces(const char* pcszSrc,
																	const char* pcszSeparators);

// Finds the given line and copies it to the buffer. 
// Lines are separated by chars < 32.
// Returns pointer to the buffer.
WSL_API char* WslGetLine(char* pszTarget, const char* pcszSource, int nLine);

// Copies the line to the pszDest and returns pointer 
// to the next line. It handles both the UNIX and Windows EOLs.
// Only chars >=32 are added to the pszDest.
WSL_API char* WslGetLine(const char* pcszSrc, char* pszDest); 

// Finds the beginning of the next line
WSL_API const char* WslNextLine(const char* p);

// .....
WSL_API char* strcpy(char* pszBuffer, DWORD dwId); 
WSL_API void strcat(char* pszBuffer, DWORD dwId);

// WSL version of C library function "stricmp"
// Returns '0' when strings are the same, '1' when they are different
WSL_API int WslStricmp(const char* pcsz1, const char* pcsz2);

// Decodes the given URL (translats %xx)
// NOTE: Do not decode the URL twice!
// NOTE: %00 is skipped to avoid zeros inside a string.
WSL_API CWslString WslUrlDecode(const CWslString& sUrl);



/*******************************************************************************
*
*  File names
*
*******************************************************************************/

// given a file name, this function returns the pointer to the extension or
// NULL if there is no extension.
WSL_API char* GetFileNameExt(const char* pcszFileName);


//*****************************************************************************
// WslCompare
//
// DESC: Compares given number of chars in given strings using warious methods
//*****************************************************************************

// dwFlags

// Wild chars are used in pattern string. It behaves like filename wildchars.
#define WSL_COMPARE_WILDCHARS     1           

WSL_API BOOL WslCompare(const char* pcszWhat, 
                        const char* pcszPattern, 
                        DWORD dwNumChars = 0xFFFFFFFF, 
                        DWORD dwFlags = 0);

//*****************************************************************************

//file path operations
WSL_API char* __fastcall CopyFileName(char* , char* );
WSL_API char* __fastcall GetFileExt(char* );

//*****************************************************************************

// Translates the given buffer using the given table.
WSL_API void WslXlat(char* pBuffer, int nLength, const char* pXlatTable);

// 1024 chars only !!!!!!
WSL_API int WslXlatCompare(const char* c1, const char* c2, char* pXlatTable);

// RETURNS:
// Czech language declination according to counts
//   0x01 .. nominative singular
//   0x11 .. nominative plural
//   0x12 .. genitive plural
WSL_API int WslGetGrammarClass(int nNum);

WSL_API const char* WslGetCorrectForm(int nCount, const char* nomsng, const char* nomplu, const char* genplu);

// Converts the given number to a well formate price string
WSL_API char* WslPriceToString(double dPrice, char* pszBuffer, int nBufSize);
