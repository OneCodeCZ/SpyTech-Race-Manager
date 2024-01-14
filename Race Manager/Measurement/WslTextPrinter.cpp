// WslTextPrinter.cpp: implementation of the CWslTextPrinter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslTextPrinter.h"
#include "winspool.h"
#include "utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWslTextPrinter::CWslTextPrinter()
{
	m_bPrintEnabled=FALSE;

	if (m_bPrintEnabled) {
	
		PRINTER_INFO_1 Info[256];
		DWORD NeededSize, count;
		
		EnumPrinters(
			PRINTER_ENUM_LOCAL,   //type of enum
			NULL,         //name
			1,            //structure level
			(BYTE*)&Info, //info buffer
			sizeof(Info), //size of info
			&NeededSize,  //needed size of buffer
			&count);      //number of printers in buffer
		
		if (!count) AfxMessageBox("No printer found!");
		strcpy(m_szPrinterName, Info[0].pName);
		
	}
}

CWslTextPrinter::~CWslTextPrinter()
{

}

int CWslTextPrinter::LF() {
	char temp[2];
	temp[0]=13;
	temp[1]=10;
	return WriteBuffer(temp, 2);
}

int CWslTextPrinter::FF() {
	char c=12;
	return WriteBuffer(&c, 1);
}

int CWslTextPrinter::WriteBuffer(void *pBuffer, int nNumBytes) {
	DWORD dwWritten=nNumBytes;
	if (m_bPrintEnabled) {
		WritePrinter(m_hPrinter, pBuffer, nNumBytes, &dwWritten);
	}
	else {
		file.Write(pBuffer, nNumBytes);
	}
	return dwWritten;
}

int CWslTextPrinter::Write(const char *pcszText) {
	DWORD dwWritten;
	dwWritten = WriteBuffer((void*)pcszText, strlen(pcszText));
	return dwWritten;
}

int CWslTextPrinter::Write(int nNum) {
	char temp[64];
	itoa(nNum, temp, 10);
	return Write(temp);
}

int CWslTextPrinter::WriteTime(DWORD dwTime, int nFormat) {
	char temp[64];
	TimeFormat(dwTime, temp, nFormat);
	return Write(temp);
}

int CWslTextPrinter::WriteLn(const char *pcszText) {
	Write(pcszText);
	return LF();
}

int CWslTextPrinter::BeginPrint() {
	if (m_bPrintEnabled) {
		if (!OpenPrinter(m_szPrinterName, &m_hPrinter, NULL)) throw 0;

		DOC_INFO_1 d;
		d.pDocName="CWslTextPrinter output";
		d.pOutputFile=NULL;
		d.pDatatype=NULL;
		if (!StartDocPrinter(m_hPrinter, 1, (BYTE*)&d)) throw 0;
	}
	else {
		srand(GetTickCount());
		char temp[1024];
		for (int a=0; a<100; a++) {
			temp[a]=rand()%10+'0';
		}
		temp[100]=0;

		file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}
	
	return 1;
}

int CWslTextPrinter::EndPrint() {
	if (m_bPrintEnabled) {
		EndDocPrinter(m_hPrinter);
		ClosePrinter(m_hPrinter);
	}
	else {
		file.Close();
	}

	return 1;
}
