// WslTextPrinter.h: interface for the CWslTextPrinter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WSLTEXTPRINTER_H__EBEC3840_226B_11D4_AB05_004F49094CF9__INCLUDED_)
#define AFX_WSLTEXTPRINTER_H__EBEC3840_226B_11D4_AB05_004F49094CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWslTextPrinter  
{
	HANDLE m_hPrinter;
	CFile file;
	char m_szPrinterName[31+1];

	BOOL m_bPrintEnabled;

public:
	CWslTextPrinter();

	int BeginPrint();
	int EndPrint();

	int WriteBuffer(void *pBuffer, int nNumBytes);

	int Write(const char *pcszText);
	int Write(int nNum);
	int WriteTime(DWORD dwTime, int nFormat);

	int WriteLn(const char *pcszText);
#define Writeln WriteLn
	int LF();
	int FF();

	virtual ~CWslTextPrinter();

};

#endif // !defined(AFX_WSLTEXTPRINTER_H__EBEC3840_226B_11D4_AB05_004F49094CF9__INCLUDED_)
