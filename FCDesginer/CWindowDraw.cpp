#include "CWindowDraw.h"
#include <windowsx.h>
#include <strsafe.h>
#include "resource.h"
#include <shobjidl_core.h>
#include "CCustomMenuData.h"

BEGIN_WND_MESSAGE_MAP(CWindowDraw, CWindowBase)
    ON_WND_MESSAGE(WM_CREATE, &CWindowDraw::OnCreate)
    ON_WND_MESSAGE(WM_CLOSE, &CWindowDraw::OnClose)
    ON_WND_MESSAGE(WM_DESTROY, &CWindowDraw::OnDestroy)
    ON_WND_MESSAGE(WM_ERASEBKGND, &CWindowDraw::OnEraseBkgnd)
    ON_WND_MESSAGE(WM_PAINT, &CWindowDraw::OnPaint)
    ON_WND_MESSAGE(WM_MOUSEACTIVATE, &CWindowDraw::OnMouseActivate)
    ON_WND_MESSAGE(WM_NCHITTEST, &CWindowDraw::OnNcHitTest)
END_WND_MESSAGE_MAP()

CWindowDraw::CWindowDraw()
{

}

CWindowDraw::~CWindowDraw()
{

}

HDC CWindowDraw::GetMemDC()
{
    return m_hMemDC;
}

HDC CWindowDraw::GetBackDC()
{
    return m_hBackDC;
}

HBITMAP CWindowDraw::GetBitmap()
{
    return m_hMemBitmap;
}

HBITMAP CWindowDraw::GetBackBitmap()
{
    return m_hBackBitmap;
}

HDC CWindowDraw::GetDrawDC()
{
    return m_hDrawDC;
}

HBITMAP CWindowDraw::GetDrawBitmap()
{
    return m_hDrawBitmap;
}

int CWindowDraw::GetDCWidth()
{
    return m_nWidth;
}

int CWindowDraw::GetDCHeight()
{
    return m_nHeight;
}

void CWindowDraw::SetDrag(bool bDrag)
{
    m_bDrag = bDrag;
}

LRESULT CWindowDraw::OnMouseActivate(WPARAM wParam, LPARAM lParam)
{
    return MA_NOACTIVATE;
}

LRESULT CWindowDraw::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
    if (m_bDrag)
    {
        return HTCAPTION;
    }

    return FALSE;
}

bool CWindowDraw::BuildMemDcBitmap(HDC hdc, HDC* pDC, HBITMAP* pBitmap, int nW, int nH)
{
    *pDC = ::CreateCompatibleDC(hdc);
    if (!*pDC)
    {
        return false;
    }

    *pBitmap = ::CreateCompatibleBitmap(hdc, nW, nH);
    if (!*pBitmap)
    {
        return false;
    }

    ::SelectObject(*pDC, *pBitmap);

    RECT rect = { 0 };
    ::GetClientRect(m_hWnd, &rect);
    ::FillRect(*pDC, &rect, GetStockBrush(BLACK_BRUSH));

    return true;
}

void CWindowDraw::ReleaseDcBitmap(HDC* pDC, HBITMAP* pBitmap)
{
    if (*pBitmap)
    {
        ::DeleteObject(*pBitmap);
        *pBitmap = NULL;
    }

    if (*pDC)
    {
        ::DeleteObject(*pDC);
        *pDC = NULL;
    }
}

LRESULT CWindowDraw::OnCreate(WPARAM wParam, LPARAM lParam)
{
    __super::OnCreate(wParam, lParam);

    HDC hdc = ::GetDC(m_hWnd);

    LPCREATESTRUCT lpCreate = (LPCREATESTRUCT)lParam;
    m_nWidth = lpCreate->cx;
    m_nHeight = lpCreate->cy;
    BuildMemDcBitmap(hdc, &m_hMemDC, &m_hMemBitmap, lpCreate->cx, lpCreate->cy);
    BuildMemDcBitmap(hdc, &m_hBackDC, &m_hBackBitmap, lpCreate->cx, lpCreate->cy);
    BuildMemDcBitmap(hdc, &m_hDrawDC, &m_hDrawBitmap, lpCreate->cx, lpCreate->cy);

    ::ReleaseDC(m_hWnd, hdc);

    return (LRESULT)0;
}

LRESULT CWindowDraw::OnClose(WPARAM wParam, LPARAM lParam)
{
    ReleaseDcBitmap(&m_hMemDC, &m_hMemBitmap);
    ReleaseDcBitmap(&m_hBackDC, &m_hBackBitmap);
    ReleaseDcBitmap(&m_hDrawDC, &m_hDrawBitmap);

    return (LRESULT)TRUE;
}

LRESULT CWindowDraw::OnDestroy(WPARAM wParam, LPARAM lParam)
{
    ::DestroyWindow(m_hWnd);
    m_hWnd = nullptr;
    return (LRESULT)TRUE;
}

LRESULT CWindowDraw::OnEraseBkgnd(WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)TRUE;
}

LRESULT CWindowDraw::OnPaint(WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps = { 0 };
    HDC hdc = ::BeginPaint(m_hWnd, &ps);

    RECT rect = { 0 };
    ::GetClientRect(m_hWnd, &rect);

    {
        std::lock_guard<std::mutex>lock(m_Mutex);
        ::BitBlt(hdc, 0, 0, rect.right, rect.bottom, m_hMemDC, 0, 0, SRCCOPY);
    }

    ::EndPaint(m_hWnd, &ps);
    return 0;
}

std::mutex& CWindowDraw::Mutex()
{
    return m_Mutex;
}