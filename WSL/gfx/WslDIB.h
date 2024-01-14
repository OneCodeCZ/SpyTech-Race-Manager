
#ifndef __WslDIB__
#define __WslDIB__
#ifndef WSL_API
#define WSL_API
#endif

// Modul is NOT dependent on MFC :-)

//*************************************************************
class WSL_API CWslDIB
{
public:
	int m_nW, m_nH;
	BYTE m_byBitDepth;
	BYTE *m_pbyData;
	
public:
	CWslDIB();
	~CWslDIB();
	void LoadDIB(const char *pcszName);
	void LoadDIB(int id);
	void SwapRB();				// RGB <-> BGR
	void Convert24to16();
};


#endif   //__WslDIB__