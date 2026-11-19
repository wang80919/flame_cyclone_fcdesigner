#pragma once
#include <wtypes.h>
#include <type_traits>

#define COLOR_RGB_TO_BGR(_rgb) ( (_rgb & 0xFF) << 16)  |  (_rgb & 0x00FF00)  | ( (_rgb >> 16) & 0xFF)

typedef struct _BITMAP_DRAW_DATA
{
    BITMAPINFO binfo = { 0 };       //位图原始信息
    HBITMAP hBitmap = nullptr;      //位图句柄
    LONG width = 0;                 //位图宽度
    LONG height = 0;                //位图高度
    LPDWORD lpBits = nullptr;       //位图全部数据缓冲 宽度 * 高度 * sizeof(DWORD)
    LPDWORD lpLineBuf = nullptr;    //位图单行数据缓冲 宽度 * sizeof(DWORD)

    _BITMAP_DRAW_DATA(const _BITMAP_DRAW_DATA&) = delete;

    _BITMAP_DRAW_DATA()
    {
        ZeroMemory(this, sizeof(_BITMAP_DRAW_DATA));
    }

    ~_BITMAP_DRAW_DATA()
    {
        this->Release();
    }

    _BITMAP_DRAW_DATA(_BITMAP_DRAW_DATA&& r) noexcept
    {
        this->binfo = r.binfo;
        this->hBitmap = r.hBitmap;
        this->width = r.width;
        this->height = r.height;
        this->lpBits = r.lpBits;
        this->lpLineBuf = r.lpLineBuf;
        
        *this = std::move(r);
    }

    _BITMAP_DRAW_DATA operator = (const _BITMAP_DRAW_DATA&) = delete;
    _BITMAP_DRAW_DATA& operator = (_BITMAP_DRAW_DATA&& r) noexcept
    {
        this->binfo = r.binfo;
        this->hBitmap = r.hBitmap;
        this->width = r.width;
        this->height = r.height;
        this->lpBits = r.lpBits;
        this->lpLineBuf = r.lpLineBuf;

        r.lpBits = nullptr;
        r.lpLineBuf = nullptr;
        r.hBitmap = nullptr;

        return *this;
    }

    void Release()
    {
        if (nullptr != lpBits)
        {
            delete[] lpBits;
            lpBits = nullptr;
        }
        if (nullptr != lpLineBuf)
        {
            delete[] lpLineBuf;
            lpLineBuf = nullptr;
        }

        ZeroMemory(this, sizeof(_BITMAP_DRAW_DATA));
    }
}BITMAP_DRAW_DATA, * PBITMAP_DRAW_DATA;

class CBitmapDraw
{
public:

    CBitmapDraw();

    CBitmapDraw(HDC hdc);

    ~CBitmapDraw();

    //加载HDC
    bool Load(HDC hdc);

    void Unload();

    bool IsValid();

    //获取位图宽度
    LONG GetWidth() const;

    //获取位图高度
    LONG GetHeight() const;

    //获取位图左上坐标
    POINT GetLeftTop() const;

    //获取位图右下坐标
    POINT GetRightBottom() const;

    //准备绘制
    bool BeginDraw();

    //结束绘制
    bool EndDraw();

    //绘制矩形边框
    void DrawRect(POINT ptLT, POINT ptRB, DWORD dwColor, bool bDotMode = false);

    //绘制线条
    void DrawLine(POINT ptLT, POINT ptRB, DWORD dwColor, bool bDotMode = false);

    //绘制圆形
    void DrawCircle(POINT ptLT, POINT ptRB, DWORD dwColor, bool bDotMode = false);

    //绘制网格
    void DrawGrid(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor, LONG xCount = 1, LONG yCount = 1, bool bDotMode = false);

    //填充网格
    void FillGrid(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor, LONG xCount = 1, LONG yCount = 1, POINT pt = { 0, 0 });

    //填充矩形区域
    void FillRect(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor);

    //限制坐标
    void LimitPiont(POINT& ptLT, POINT& ptRB);

public:

    //调节起始坐标点, 确保起始点坐标在终止坐标的左上方
    static void AdjustPiont(POINT& ptLT, POINT& ptRB);

    //网格命中测试
    static bool GridHitTest(POINT ptLT, POINT ptRB, DWORD xCount, DWORD yCount, POINT ptCheck, POINT& ptOut);

    //获取两线交点
    static bool GetCrossPoint(POINT& out, POINT p1, POINT p2, POINT p3, POINT p4);

private:

    BITMAP_DRAW_DATA m_DrawData;    //绘制数据
    HDC m_hDC = nullptr;            //DC句柄
};

