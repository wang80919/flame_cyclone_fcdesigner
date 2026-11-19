#pragma once
#include "CWindow/CWindowBase.h"
#include <vector>
#include <mutex>
#include <functional>

class CWindowDraw: 
    public CWindowBase
{
public:

    CWindowDraw();
    CWindowDraw(const CWindowDraw& r) = delete;
    ~CWindowDraw();

    HDC GetMemDC();
    HBITMAP GetBitmap();
    HDC GetBackDC();
    HBITMAP GetBackBitmap();
    HDC GetDrawDC();
    HBITMAP GetDrawBitmap();
    int GetDCWidth();
    int GetDCHeight();
    void SetDrag(bool bDrag);
    std::mutex& Mutex();

protected:
    wnd_msg LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnEraseBkgnd(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnMouseActivate(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);

private:

    bool BuildMemDcBitmap(HDC hdc, HDC* pDC, HBITMAP* pBitmap, int nW, int nH);
    void ReleaseDcBitmap(HDC* pDC, HBITMAP* pBitmap);

private:

    HDC         m_hMemDC = NULL;
    HBITMAP     m_hMemBitmap = NULL;

    HDC         m_hBackDC = NULL;
    HBITMAP     m_hBackBitmap = NULL;

    HDC         m_hDrawDC = NULL;
    HBITMAP     m_hDrawBitmap = NULL;

    bool        m_bDrag = false;
    int         m_nWidth = 0;
    int         m_nHeight = 0;

    std::mutex  m_Mutex;
    DECLARE_WND_MESSAGE_MAP()
};