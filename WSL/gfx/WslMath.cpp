
#include "stdafx.h"
#include "WslMath.h"
#include <math.h>

WSL_API float *g_fWslSinTab = NULL, *g_fWslCosTab = NULL;

//*****************************************************************************
void CWsl3DPoint::Normalize()
{
	double s = 1 / sqrt(x*x + y*y + z*z);
	x = (float) (x * s);
	y = (float) (y * s);
	z = (float) (z * s);
}

//*****************************************************************************
float CWsl3DPoint::GetAngle(CWsl3DPoint &p)
{
	return (float) acos( fabs( (x*p.x + y*p.y + z*p.z) / ( sqrt(x*x + y*y + z*z) * sqrt(p.x*p.x + p.y*p.y + p.z*p.z)) ));
}

//*****************************************************************************
float CWsl3DPoint::GetLength()
{
	return (float) sqrt(x*x + y*y + z*z);
}

//*****************************************************************************
void WSL_API WslAddNormal(CWsl3DPoint *p1, CWsl3DPoint *p2, CWsl3DPoint *p3, CWsl3DPoint *pNormal)
{
	CWsl3DPoint A, B, N;

	A.SetVector(*p1, *p2);
	B.SetVector(*p1, *p3);

	N.Set(A.y*B.z  -  A.z*B.y, A.z*B.x  -  A.x*B.z, A.x*B.y  -  A.y*B.x);

	N.Normalize();
	*pNormal += N;
	pNormal->Normalize();
}

//*****************************************************************************
void WSL_API WslInitSinCosTable(float fAngleFrom /*= 0.0f*/, 
								float fAngleTo /*= 360.0f*/, 
								float fStep /*= 1.0f*/)
{
	int nCount = (int)((fAngleTo - fAngleFrom)  / fStep);
	_ASSERT(nCount > 0);

	if (g_fWslSinTab) delete g_fWslSinTab;
	if (g_fWslCosTab) delete g_fWslCosTab;
	g_fWslSinTab = new float[nCount];
	g_fWslCosTab = new float[nCount];

	for (int i = 0; i < nCount; i++)
	{
		float f = fAngleFrom * 2.0f * PI / 360.0f;
		fAngleFrom += fStep;
		g_fWslSinTab[i] = (float) sin(f);
		g_fWslCosTab[i] = (float) cos(f);
	}
}

//*****************************************************************************
void WSL_API WslDeinitSinCosTable()
{
	if (g_fWslSinTab) delete g_fWslSinTab;
	if (g_fWslCosTab) delete g_fWslCosTab;
	g_fWslSinTab = NULL;
	g_fWslCosTab = NULL;
}














