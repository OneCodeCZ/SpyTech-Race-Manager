
#ifndef __WSL_MATH__
#define __WSL_MATH__
#ifndef WSL_API
#define WSL_API
#endif

// Modul is NOT dependent on MFC :-)

#include <math.h>

#define PI	3.141592654f

//*****************************************************************************
class WSL_API CWsl3DPoint
{
public:
	float x,y,z;

public:
	void SetZero() { x = y = z = 0; }
	void Set(float nx, float ny, float nz) { x = nx; y = ny; z = nz; }
	void SetVector(CWsl3DPoint &A, CWsl3DPoint &B) { x = B.x - A.x; y = B.y - A.y; z = B.z - A.z; }
	void MakeVectorTo(float tx, float ty, float tz) { x = tx-x; y = ty-y; z = tz-z; }
	void Normalize();
	float GetAngle(CWsl3DPoint &p);
	float GetLength();
	BOOL IsZero() const { return x==0 && y==0 && z==0; }
	operator float*() { return &x; }       // Reurns pointer to the tree floats x,y,z (for OpenGL)
	operator += (CWsl3DPoint &a) { x += a.x; y += a.y; z += a.z; }
	operator *= (float f) { x *= f; y *= f; z *= f; }
};

//*****************************************************************************

// Calculates normal of the given triangle and adds it to the given normal
void WSL_API WslAddNormal(CWsl3DPoint *p1, CWsl3DPoint *p2, CWsl3DPoint *p3, CWsl3DPoint *pNormal);

// SIN/COS table
WSL_API extern float *g_fWslSinTab, *g_fWslCosTab;
void WSL_API WslInitSinCosTable(float fAngleFrom = 0.0f, float fAngleTo = 360.0f, float fStep = 1.0f);
void WSL_API WslDeinitSinCosTable();

#endif      //__WSL_MATH__
