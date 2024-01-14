///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Text manipulation functiuns 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik and Adam J. Sporka
// | C |  |__|  |  Last changed: 2003 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslTextStuff.h"

//******************************************************
void __fastcall strcat1(char *, char)   //ecx, dl
{
	__asm
	{
		mov edi,ecx
		xor eax,eax
		mov ecx,-1
		xor dh,dh
		repne scasb
		mov [edi-1],dx 
	}
}

//******************************************************
void __fastcall strcat3(char *, DWORD)   //ecx, edx
{
	__asm
	{
		mov edi,ecx
		xor eax,eax
		mov ecx,-1
		repne scasb
		mov [edi-1],edx 
	}
}

//******************************************************
#pragma warning( disable : 4035 )
char *__fastcall strend(char *)  //in:ecx  / out:eax
{
	__asm
	{
		mov edi,ecx
		xor eax,eax
		mov ecx,-1
		repne scasb
		dec edi
		mov eax,edi
	}
}
#pragma warning( default : 4035 )

//******************************************************
#pragma warning( disable : 4035 )
extern char *__fastcall CopyFileName(char *, char *) //in:kam=ecx,co=edx  / out:eax
{
	__asm
	{
		mov edi,ecx
		mov esi,edx
		mov eax,esi
		xor ecx,ecx
			
l3: cmp [esi],'\\'         //najit zacatek jmena
    jne short l1        
		mov eax,esi
		inc eax
l1: cmp [esi],0
    je short l2
    inc esi
    inc ecx
    jmp short l3
		
l2: mov ebx,edi             //zkopirovat
    inc ecx
    mov esi,eax
    rep movsb
    mov eax,ebx
	}
}
#pragma warning( default : 4035 )

//******************************************************
#pragma warning( disable : 4035 )
char *__fastcall GetFileExt(char *)       //ecx=co
{
	__asm
	{
		mov edx,ecx
l2: mov al,[edx]          //najit tecku
    inc edx
    cmp al,'.'
    je short l1
    or al,al
    jnz short l2
    
    mov edx,ecx
l1: mov eax,edx
	}
}

#pragma warning( default : 4035 )


//******************************************************
char *strdot(char *s, int max)
{
	int i=strlen(s);
	if (i>max) { s[max-2]='.'; s[max-1]='.'; s[max]=0; }
	return s;
}

//******************************************************
char *strdot(char *to, char *what, int max)
{
	strcpy(to, what);
	int i=strlen(to);
	if (i>max) { to[max-2]='.'; to[max-1]='.'; to[max]=0; }
	return to;
}

//*****************************************************************************
char *strcpy(char *pszBuffer, DWORD dwId) 
{ 
#ifdef _MFC_VER
	
	CString s;
	s.LoadString(dwId);
	strcpy(pszBuffer, s);
	return pszBuffer;
	
#else
	
	ASSERT(false);				// You need MFC for that :-(
	return NULL;
	
#endif
}

//*****************************************************************************
void strcat(char *pszBuffer, DWORD dwId) 
{ 
#ifdef _MFC_VER
	CString s;
	s.LoadString(dwId);
	strcat(pszBuffer, s);
#else
	ASSERT(false);				// You need MFC for that :-(
#endif
}

//*****************************************************************************
char *WslGetLine(char *pszTarget, 
                 const char *pcszSource, 
                 int nLine)
{
	const BYTE *pszSrc = (const BYTE*) pcszSource;
	BYTE *pszTo = (BYTE*) pszTarget;
	
	// Find wanted row
	
	while (--nLine)
	{
		while (((BYTE)*pszSrc)>=32) pszSrc++;
		if (*pszSrc==0) { *pszTo = 0; return pszTarget; }
		pszSrc+=2;   // skip #13#10
	}
	
	// Copy the row
	
	while (*pszSrc>=32) *pszTo++ = *pszSrc++;
	*pszTo = 0;
	
	return pszTarget;
}

//*****************************************************************************
char* WslGetLine(const char* pcszSrc, char* pszDest)
{
	BYTE ch;
	ASSERT(pcszSrc);
	ASSERT(pszDest);

	for (; ch=*pcszSrc; pcszSrc++) 
	{
		if (ch == 13) { pcszSrc += 2; break; } // Windows EOL
		if (ch == 10) { pcszSrc++; break;}		 // Unix EOL
		if (ch >= 32) *pszDest++ = ch;				 // add only valid characters
	}

	*pszDest = 0;
	return (char*) pcszSrc;
}

//******************************************************
DWORD atod16(char *s)
{
	DWORD x=0;
	BYTE z;
	char *p=s;
	
	while (z=*p++)
	{
		x<<=4;
		if (z<57) z=z-48;
		else
			if (z<90) z=z-65 + 10;
			else
				if (z<122) z=z-97 + 10;
				x|=z;
	}
	
	return x;
}

//***************************************************************
const char *WslSkipSpaces(const char *p)
{
	while (((BYTE)*p)<33 && ((BYTE)*p)>13) p++;
	return p;
}

//***************************************************************
const char* WslSkipSpaces(const char* pcszSrc,
													const char* pcszSeparators)
{
	BYTE ch, ch2;
	ASSERT(pcszSrc);
	ASSERT(pcszSeparators);

	for (; ch = *pcszSrc; pcszSrc++)
	{
	  for (int i=0; ; i++) 
		{
			ch2 = (BYTE) pcszSeparators[i];
			if (!ch2) goto L;
			if (ch==ch2) break;
		}
	}
L:
	return pcszSrc;
}

//***************************************************************
const char *WslNextLine(const char *p)
{
	while (*(BYTE*)p > 13) p++;
	return p==0 ? p : p+2;
}

//***************************************************************
//returns pointer behind word
//"/" is regarded as remark begin
//!!!OBSOLETE - use WslGetWord2()!!!
const char *WslGetWord(const char *p, char *pBuffer)
{
	ASSERT(false);
	p=WslSkipSpaces(p);
	
	//string enclosed in braces
	if (*p=='"')
	{
		while (*(++p)>13)
		{
			if (*p=='"' && *(++p)!='"') break;
			*pBuffer++=*p;
		}
	}
	else
		//normal string
	{ //TODO: optimize char tests, let pass only 0-1, a-z, A-Z, '-', '_'
		while ((*p>=48 || *p=='-'|| *p=='#') && *p!=']' && *p!='[' && *p!='=') 
			*pBuffer++=*p++;
	}
	
	*pBuffer=0;
	return p;
}

//*****************************************************************************
const char* WslGetToken(const char* pcszSrc, 
												char* pszDest, 
												int nDestSize,
												const char* pcszSeparators)
											  
{
	BYTE ch, ch2;
	ASSERT(pcszSrc);
	ASSERT(pszDest);
	ASSERT(pcszSeparators);
	ASSERT(nDestSize > 0);

	for (; ch=*pcszSrc; pcszSrc++)
	{
		for (int i=0; ch2 = pcszSeparators[i]; i++) if (ch == ch2) goto L;
		if (nDestSize-- == 1) break;
		*pszDest++ = ch;
	}
L:
	*pszDest = 0;

	return pcszSrc;
}

//*****************************************************************************
BOOL WslCompare(const char *pcszWhat, 
                const char *pcszPattern, 
                DWORD dwNumChars /*= 0xFFFFFFFF*/, 
                DWORD dwFlags /*= 0*/)
{
	switch (dwFlags)
	{
	case 0:
		while (dwNumChars--) 
		{
			char ch = *pcszPattern++;
			if (*pcszWhat++ != ch) return false;
			if (ch==0) return true;
		}
		break;
		
	case WSL_COMPARE_WILDCHARS:
		{
			while (dwNumChars--)
			{
				char pch = *pcszPattern++;
				
				switch (pch)
				{
				case '*':
					// If '*' is the last char then strings are same
					
					if (dwNumChars-- == 0) return true;
					pch = *pcszPattern++;
					if (pch==0) return true;
					
					// if not, then search for the char following '*'
					
					while (*pcszWhat && *pcszWhat != pch) pcszWhat++;
					
					// If we have reached the end, then char was not found -> false
					
					if (*pcszWhat==0) return false;
					pcszWhat++;
					break;
					
				case '?':
					pcszWhat++;
					break;
					
				default:
					if (pch != *pcszWhat) return false;
					pcszWhat++;
					break;
				}
				
				if (pch==0) return true;
			}
		}
		break;
		
	default:
		ASSERT(false);    // Given combination of flags is not implemented yet, sorry :-)
		return false;
	}
	
	return true;
}

//*****************************************************************************
int WslStricmp(const char *pcsz1, const char *pcsz2)
{
	char a,b;
	while (1)
	{
		a = *pcsz1++; 
		b = *pcsz2++;
		if (!a || !b) break;

		if (a!=b)
		{
			if (a>=65 && a<=90) a+=32;
			if (b>=65 && b<=90) b+=32;
			if (a!=b) return 1;
		}
	}
	return a || b;
}

//*****************************************************************************
char *GetFileNameExt(const char *pcszFileName)
{
	int nIdx;
	int nSize = strlen(pcszFileName);
	int nLastPos = -1;
	for (nIdx = 0; nIdx < nSize; nIdx++)
		if (pcszFileName[nIdx] == '.') nLastPos = nIdx;
	if (nLastPos == -1) return NULL;
	return (char*)(pcszFileName + nLastPos);
}

//*****************************************************************************
void WslXlat(char* pBuffer, int nLength, const char* pXlatTable)
{
	ASSERT(pXlatTable);
	for (int nIdx=0; nIdx<nLength; nIdx++) pBuffer[nIdx] = pXlatTable[pBuffer[nIdx]];
}

//*****************************************************************************
int WslXlatCompare(const char *c1, const char *c2, char *pXlatTable)
{
	int nLen1 = strlen(c1);
	int nLen2 = strlen(c2);

	ASSERT(nLen1 < 1024);
	ASSERT(nLen2 < 1024);

	char s1[1024];
	char s2[1024];

	strcpy(s1, c1); WslXlat(s1, strlen(s1), pXlatTable);
	strcpy(s2, c2); WslXlat(s2, strlen(s2), pXlatTable);

	return strcmp(s1, s2);
}

/******************************************************************************/

int WslGetGrammarClass(int nNum)
{
	int nHd = nNum % 100;
	if (nHd == 0) return 0x12;      // nula mist
	if (nHd > 10 && nHd < 20) {  // deset, jedenact, dvanact.. mist
		return 0x12;
	}
	int nDec = nHd % 10;
	if (nDec == 0) return 0x12; // tricet mist, deset mist
	if (nDec == 1) return 0x01; // dvacet jedno misto
	if (nDec < 5) return 0x11; // dvacet ctyri mista
	return 0x12;
}

//*****************************************************************************
const char *WslGetCorrectForm(int nCount, const char *nomsng, const char *nomplu, const char *genplu)
{
	switch (WslGetGrammarClass(nCount)) {
	case 0x01: return nomsng;
	case 0x11: return nomplu;
	case 0x12: return genplu;
	}
	return nomplu;
}

//******************************************************
char* WslPriceToString(double dPrice, char* pszBuffer, int nBufSize)
{
	ASSERT(pszBuffer);

  // Prepare currency format

  struct lconv* pLconv = localeconv();
  CURRENCYFMT fmt = { 2, 1, 3, pLconv->decimal_point, pLconv->thousands_sep, 1, 3, "" };
  char temp[64];

  // Convert price to format expected by the GetCurrencyFormat() function

  sprintf(temp, "%f", dPrice);
  for (int i=strlen(temp); i--; ) if (temp[i]==*pLconv->decimal_point) temp[i] = '.';

  // Do it!

  GetCurrencyFormat(NULL, 0, temp, &fmt, pszBuffer, nBufSize);
  return pszBuffer;
}

/*****************************************************************************/
#define SPC_BASE16_TO_10(x) (((x) >= '0' && (x) <= '9') ? ((x) - '0') : (toupper((x)) - 'A' + 10))

CWslString WslUrlDecode(const CWslString& sUrl)
{
   // See: http://www.oreillynet.com/pub/a/network/excerpt/spcookbook_chap03/index2.html
   CWslString s;

   for (int i=0; i<sUrl.GetLength(); i++)
   {
      if (sUrl[i]=='%' && i < sUrl.GetLength()-2)
      {
         ASSERT(isxdigit(sUrl[i+1]) && isxdigit(sUrl[i+2]));   // not URL encoded string!
         s += (char)((SPC_BASE16_TO_10(sUrl[i+1]) * 16) + (SPC_BASE16_TO_10(sUrl[i+2])));
         i += 2;
      }
      else s += sUrl[i];
   }

   return s;
}






