#include "CDesignStaticWindow.h"
#include <windowsx.h>

BEGIN_SUBCLASS_MESSAGE_MAP(CDesignStaticWindow, CWndSubclassBase)
    ON_SUBCLASS_MESSAGE(WM_PAINT, &CDesignStaticWindow::OnPaint)
END_SUBCLASS_MESSAGE_MAP()

CDesignStaticWindow::CDesignStaticWindow()
{

}

CDesignStaticWindow::~CDesignStaticWindow()
{

}

bool CDesignStaticWindow::Init()
{
    HDC hdc = ::GetDC(m_hWnd);
    m_hMemDC = ::CreateCompatibleDC(hdc);
    if (!m_hMemDC)
    {
        return false;
    }

    int nScreenX = GetSystemMetrics(SM_CXSCREEN);
    int nScreenY = GetSystemMetrics(SM_CYSCREEN);
    m_MemBitmap = ::CreateCompatibleBitmap(hdc, nScreenX, nScreenY);
    if (!m_MemBitmap)
    {
        return false;
    }

    ::SelectObject(m_hMemDC, m_MemBitmap);

    RECT rect = { 0 };
    ::GetClientRect(m_hWnd, &rect);
    ::FillRect(m_hMemDC, &rect, GetStockBrush(WHITE_BRUSH));

    ::ReleaseDC(m_hWnd, hdc);
}

void CDesignStaticWindow::Uninit()
{
    if (m_MemBitmap)
    {
        ::DeleteObject(m_MemBitmap);
        m_MemBitmap = NULL;
    }
    if (m_hMemDC)
    {
        ::DeleteObject(m_hMemDC);
        m_hMemDC = NULL;
    }
}

HDC CDesignStaticWindow::GetMemDC()
{
    return m_hMemDC;
}

LRESULT CDesignStaticWindow::OnPaint(WPARAM wParam, LPARAM lParam)
{
    if (m_hMemDC)
    {
        PAINTSTRUCT ps = { 0 };
        HDC hdc = ::BeginPaint(m_hWnd, &ps);

        RECT rect = { 0 };
        ::GetClientRect(m_hWnd, &rect);

        ::BitBlt(hdc, 0, 0, rect.right, rect.bottom, m_hMemDC, 0, 0, SRCCOPY);

        ::EndPaint(m_hWnd, &ps);
    }
    else
    {
        return (LRESULT)FALSE;
    }

    return (LRESULT)TRUE;
}