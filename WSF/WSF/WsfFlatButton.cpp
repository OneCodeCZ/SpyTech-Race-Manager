///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfFlatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfFlatButton, CWnd)
//{{AFX_MSG_MAP(CWsfFlatButton)
   ON_WM_MOUSEMOVE()
   ON_WM_KILLFOCUS()
   ON_WM_SETFOCUS()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_PAINT()
   ON_WM_DESTROY()
   ON_WM_SHOWWINDOW()
   ON_WM_CAPTURECHANGED()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*****************************************************************************
CWsfFlatButton::CWsfFlatButton()
{
   m_dwFlags = 0;
}

//*****************************************************************************
CWsfFlatButton::~CWsfFlatButton()
{
}

//*****************************************************************************
BOOL CWsfFlatButton::Create(DWORD dwStyle, 
                            RECT *pRect, 
                            int nBitmap, 
                            int nBitmapDisabled,
                            CWnd *pParent,
                            int	nId) 
{
   ASSERT(nBitmap > 0);
   
   // Init bitmap
   
   m_hBitmap = (HBITMAP) LoadImage(g_hInstance, MAKEINTRESOURCE(nBitmap), 
      IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS | LR_SHARED);
   ASSERT(m_hBitmap);
   if (!m_hBitmap) return false;
   
   m_hBitmapDisabled = (HBITMAP) LoadImage(g_hInstance, MAKEINTRESOURCE(nBitmapDisabled), 
      IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS | LR_SHARED);
   ASSERT(m_hBitmapDisabled);
   if (!m_hBitmapDisabled) return false;
   
   // Create window class
   
   const char *pClass = AfxRegisterWndClass(0,    
      ::LoadCursor(NULL, IDC_ARROW),                      
      (HBRUSH)(COLOR_3DSHADOW+1), 0);                         
   
   // Create window
   
   return CreateEx(0, pClass, NULL, dwStyle, *pRect, pParent, nId);
}

//*****************************************************************************
void CWsfFlatButton::OnDestroy() 
{
   CWnd::OnDestroy();
   
   if (m_hBitmap) 
   { 
      DeleteObject(m_hBitmap); m_hBitmap = NULL; 
      DeleteObject(m_hBitmapDisabled); m_hBitmapDisabled = NULL; 
   }
}

//*****************************************************************************
void CWsfFlatButton::OnPaint() 
{
   RECT r;
   PAINTSTRUCT ps;
   BITMAP bi;
   HDC hMemDC, hDC;
   HBITMAP hBmp = IsWindowEnabled() ? m_hBitmap : m_hBitmapDisabled;
   
   // Init
   
   hDC = ::BeginPaint(m_hWnd, &ps);
   hMemDC = CreateCompatibleDC(hDC);
   ASSERT(hMemDC);
   SelectObject(hMemDC, hBmp);
   
   // Draw bitmap
   
   GetClientRect(&r);
   GetObject(hBmp, sizeof(bi), &bi);
   
   BitBlt(hDC, (r.right - bi.bmWidth) / 2, (r.bottom - bi.bmHeight) / 2,
      bi.bmWidth, bi.bmHeight, hMemDC, 0,0, SRCCOPY);
   
   // Draw border
   
   DrawBorder(hDC, &r);
   
   // End
   
   DeleteDC(hMemDC);
   ::EndPaint(m_hWnd, &ps);
}

//*****************************************************************************
void CWsfFlatButton::DrawBorder(HDC hDC, RECT *pRect)
{
   RECT r;
   
   if (!hDC) hDC = ::GetDC(m_hWnd);
   if (!pRect) { GetClientRect(&r); pRect = &r; }
   
   if (m_dwFlags & WSF_FLB_BORDER)
   {
   /*		bool b = (m_dwFlags & (WSF_FLB_LBUTTON_DOWN|WSF_FLB_INSIDE)) == (WSF_FLB_LBUTTON_DOWN|WSF_FLB_INSIDE);
   DrawEdge(hDC, pRect, 
			b ? BDR_SUNKENOUTER : BDR_RAISEDINNER, 
      BF_RECT);*/
      HBRUSH h = ::GetSysColorBrush(COLOR_CAPTIONTEXT);
      FrameRect(hDC, pRect, h);
      DeleteObject(h);
   }
   else
   {
      HBRUSH h = ::GetSysColorBrush(COLOR_3DSHADOW);
      FrameRect(hDC, pRect, h);
      DeleteObject(h);
   }
}

//*****************************************************************************
void CWsfFlatButton::OnKillFocus(CWnd* pNewWnd) 
{
   if (m_dwFlags & WSF_FLB_BORDER)
   {
      m_dwFlags &= ~WSF_FLB_BORDER;
      DrawBorder(NULL, NULL);
   }
   CWnd::OnKillFocus(pNewWnd);
}

//*****************************************************************************
void CWsfFlatButton::OnSetFocus(CWnd* pOldWnd) 
{
   if (!(m_dwFlags & WSF_FLB_BORDER)) 
   {
      m_dwFlags |= WSF_FLB_BORDER;
      DrawBorder(NULL, NULL);
   }
   CWnd::OnSetFocus(pOldWnd);
}

//*****************************************************************************
void CWsfFlatButton::OnMouseMove(UINT nFlags, CPoint point) 
{
   CWnd::OnMouseMove(nFlags, point);
   
   DWORD dwBak = m_dwFlags;
   RECT r;
   GetClientRect(&r);
   
   if (PtInRect(&r, point))
   {
      m_dwFlags |= (WSF_FLB_INSIDE | WSF_FLB_BORDER);
      if (!(m_dwFlags & WSF_FLB_CAPTURE)) { m_dwFlags |= WSF_FLB_CAPTURE; SetCapture(); }
   }
   else 
   {	
      if (!(m_dwFlags & WSF_FLB_LBUTTON_DOWN))
      {
         m_dwFlags &= ~(WSF_FLB_BORDER | WSF_FLB_CAPTURE);
         ReleaseCapture();
      }
      m_dwFlags &= ~WSF_FLB_INSIDE;
   }
   
   if (m_dwFlags != dwBak) DrawBorder(NULL, &r);
}

//*****************************************************************************
void CWsfFlatButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
   SetCapture();
   m_dwFlags |= WSF_FLB_LBUTTON_DOWN;
   DrawBorder(NULL, NULL);
}

//*****************************************************************************
void CWsfFlatButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
   RECT r;
   GetClientRect(&r);
   
   if ((!m_dwFlags & WSF_FLB_LBUTTON_DOWN)) return;
   m_dwFlags &= ~WSF_FLB_LBUTTON_DOWN;
   
   if (PtInRect(&r, point))
   {
      ::SendMessage(::GetParent(m_hWnd), 
         WM_COMMAND,
         MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED),
         (LPARAM) m_hWnd);
   }
   else
   {
      m_dwFlags &= ~(WSF_FLB_BORDER | WSF_FLB_CAPTURE);
      ReleaseCapture();
   }
   
   DrawBorder(NULL, &r);
}

//*****************************************************************************
void CWsfFlatButton::OnShowWindow(BOOL bShow, UINT nStatus) 
{
   CWnd::OnShowWindow(bShow, nStatus);
   
   if (!bShow)
   {
      if (m_dwFlags & WSF_FLB_CAPTURE) ReleaseCapture();
      m_dwFlags &= ~(WSF_FLB_CAPTURE|WSF_FLB_BORDER|WSF_FLB_LBUTTON_DOWN); 
   }
}

//*****************************************************************************
void CWsfFlatButton::OnCaptureChanged(CWnd *pWnd) 
{
   if (m_dwFlags & WSF_FLB_BORDER)
   {
      m_dwFlags &= ~WSF_FLB_BORDER;
      DrawBorder(NULL, NULL);
   }
   CWnd::OnCaptureChanged(pWnd);
}
