#pragma once
#include "CWindow/CWndSubclassBase.h"

class CDesignStaticWindow :
    public CWndSubclassBase
{
public:
    CDesignStaticWindow();
    ~CDesignStaticWindow();

    wnd_msg LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

    bool Init();
    void Uninit();
    HDC GetMemDC();

private:

    HDC m_hMemDC = NULL;
    HBITMAP m_MemBitmap = NULL;

    DECLARE_SUBCLASS_MESSAGE_MAP()
};
