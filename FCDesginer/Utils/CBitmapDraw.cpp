#include "CBitmapDraw.h"
#include <new>
#include <memory.h>
#include <math.h>

#pragma intrinsic(memcpy)

CBitmapDraw::CBitmapDraw()
    :
    m_hDC(nullptr)
{
}

CBitmapDraw::CBitmapDraw(HDC hdc)
    :
    m_hDC(nullptr)
{
    this->Load(hdc);
}

CBitmapDraw::~CBitmapDraw()
{
    m_DrawData.Release();
}

void CBitmapDraw::Unload()
{
    m_DrawData.Release();
    m_hDC = nullptr;
}

bool CBitmapDraw::IsValid()
{
    return m_hDC != nullptr;
}

bool CBitmapDraw::Load(HDC hdc)
{
    bool bResult = false;

    if (nullptr != m_hDC)
    {
        m_DrawData.Release();
        m_hDC = nullptr;
    }

    if (nullptr == hdc)
    {
        return false;
    }

    do
    {
        m_hDC = hdc;
        int nScanlines = 0;

        //获取位图句柄
        m_DrawData.hBitmap = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
        if (nullptr == m_DrawData.hBitmap)
        {
            break;
        }

        //获取位图信息
        m_DrawData.binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        nScanlines = ::GetDIBits(hdc, m_DrawData.hBitmap, 0, 0, nullptr, &m_DrawData.binfo, DIB_RGB_COLORS);
        if (0 == nScanlines)
        {
            break;
        }

        m_DrawData.binfo.bmiHeader.biPlanes = 1;
        m_DrawData.binfo.bmiHeader.biBitCount = 32;

        //取得位图尺寸, 分配位图数据缓冲
        m_DrawData.width = m_DrawData.binfo.bmiHeader.biWidth;
        m_DrawData.height = ::abs(m_DrawData.binfo.bmiHeader.biHeight);

        //高度方向矫正
        m_DrawData.binfo.bmiHeader.biCompression = BI_RGB;
        if (m_DrawData.binfo.bmiHeader.biHeight > 0)
        {
            m_DrawData.binfo.bmiHeader.biHeight = 0 - m_DrawData.binfo.bmiHeader.biHeight;
        }

        m_DrawData.binfo.bmiHeader.biCompression = BI_RGB;

        bResult = true;

    } while (false);

    return bResult;
}

LONG CBitmapDraw::GetWidth() const
{
    return m_DrawData.width;
}

LONG CBitmapDraw::GetHeight() const
{
    return m_DrawData.height;
}

POINT CBitmapDraw::GetLeftTop() const
{
    return {0, 0};
}

POINT CBitmapDraw::GetRightBottom() const
{
    return { m_DrawData.width - 1, m_DrawData.height - 1 };
}

bool CBitmapDraw::BeginDraw()
{
    if (nullptr == m_hDC)
    {
        return false;
    }

    bool bRes = false;
    do
    {
        m_DrawData.Release();

        //获取位图句柄
        m_DrawData.hBitmap = (HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP);
        if (nullptr == m_DrawData.hBitmap)
        {
            break;
        }

        //获取位图信息
        m_DrawData.binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        if (0 == ::GetDIBits(m_hDC, m_DrawData.hBitmap, 0, 0, nullptr, &m_DrawData.binfo, DIB_RGB_COLORS))
        {
            break;
        }

        m_DrawData.binfo.bmiHeader.biPlanes = 1;
        m_DrawData.binfo.bmiHeader.biBitCount = 32;

        //取得位图尺寸, 分配位图数据缓冲
        m_DrawData.width = m_DrawData.binfo.bmiHeader.biWidth;
        m_DrawData.height = ::abs(m_DrawData.binfo.bmiHeader.biHeight);
        m_DrawData.lpBits = new (std::nothrow) DWORD[m_DrawData.width * m_DrawData.height];
        if (nullptr == m_DrawData.lpBits)
        {
            break;
        }

        memset(m_DrawData.lpBits, 0, (m_DrawData.width * m_DrawData.height) * sizeof(DWORD));

        //分配单行缓冲
        m_DrawData.lpLineBuf = new (std::nothrow) DWORD[m_DrawData.width];
        if (nullptr == m_DrawData.lpLineBuf)
        {
            break;
        }
        memset(m_DrawData.lpLineBuf, 0, m_DrawData.width * sizeof(DWORD));

        //高度方向矫正
        m_DrawData.binfo.bmiHeader.biCompression = BI_RGB;
        if (m_DrawData.binfo.bmiHeader.biHeight > 0)
        {
            m_DrawData.binfo.bmiHeader.biHeight = 0 - m_DrawData.binfo.bmiHeader.biHeight;
        }

        //取得位图数据
        int nScanlines = ::GetDIBits(m_hDC, m_DrawData.hBitmap, 0, m_DrawData.height, m_DrawData.lpBits, &m_DrawData.binfo, DIB_RGB_COLORS);
        if (0 == nScanlines)
        {
            break;
        }

        m_DrawData.binfo.bmiHeader.biCompression = BI_RGB;

        bRes = true;

    } while (false);

    return bRes;
}

bool CBitmapDraw::EndDraw()
{
    if (nullptr == m_DrawData.lpBits)
    {
        return false;
    }

    if (m_DrawData.lpBits)
    {
        //设置兼容位图中的像素。
        int nScanlines = ::SetDIBits(m_hDC, m_DrawData.hBitmap, 0, m_DrawData.height, m_DrawData.lpBits, &m_DrawData.binfo, DIB_RGB_COLORS);
        if (0 != nScanlines)
        {
            return true;
        }
    }

    return false;
}

void CBitmapDraw::LimitPiont(POINT& ptLT, POINT& ptRB)
{
    LONG nWidth = m_DrawData.width;
    LONG nHeight = m_DrawData.height;

    //起点坐标限制
    if (ptLT.x < 0) ptLT.x = 0;
    if (ptLT.y < 0) ptLT.y = 0;
    if (ptLT.x > nWidth - 1) ptLT.x = nWidth - 1;
    if (ptLT.y > nHeight - 1) ptLT.y = nHeight - 1;

    //终点坐标限制
    if (ptRB.x < 0) ptRB.x = 0;
    if (ptRB.y < 0) ptRB.y = 0;
    if (ptRB.x > nWidth - 1) ptRB.x = nWidth - 1;
    if (ptRB.y > nHeight - 1) ptRB.y = nHeight - 1;
}

void CBitmapDraw::AdjustPiont(POINT& ptLT, POINT& ptRB)
{
    LONG x0 = ptLT.x < ptRB.x ? ptLT.x : ptRB.x;
    LONG y0 = ptLT.y < ptRB.y ? ptLT.y : ptRB.y;
    LONG x1 = ptLT.x > ptRB.x ? ptLT.x : ptRB.x;
    LONG y1 = ptLT.y > ptRB.y ? ptLT.y : ptRB.y;
    ptLT = { x0 , y0 };
    ptRB = { x1 , y1 };
}
bool CBitmapDraw::GridHitTest(POINT ptLT, POINT ptRB, DWORD xCount, DWORD yCount, POINT ptCheck, POINT& ptOut)
{
    //调节限制位置
    (void)AdjustPiont(ptLT, ptRB);

    //计算绘制区域宽高, 单行线条字节长度
    int nDrawWidth = ptRB.x - ptLT.x + 1;
    int nDrawHeight = ptRB.y - ptLT.y + 1;

    //格子数量限定
    if (xCount < 1) xCount = 1;
    if (yCount < 1) yCount = 1;

    //计算格子宽度
    int xGridWidth = nDrawWidth / xCount;
    int yGridHeight = nDrawHeight / yCount;

    //检查是否位于指定区域内(包含边框)
    if (!((ptCheck.x >= ptLT.x && ptCheck.x <= ptRB.x) && (ptCheck.y >= ptLT.y && ptCheck.y <= ptRB.y)))
    {
        return false;
    }

    ptOut.x = (ptCheck.x - ptLT.x) / xGridWidth;
    ptOut.y = (ptCheck.y - ptLT.y) / yGridHeight;

    return true;
}

void CBitmapDraw::DrawGrid(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor, LONG xCount/* = 1*/, LONG yCount/* = 1*/, bool bDotMode/* = false*/)
{
    dwColor = COLOR_RGB_TO_BGR(dwColor);
    if (nullptr == m_DrawData.lpBits)
    {
        return;
    }

    LPDWORD lpBits = m_DrawData.lpBits;
    LPDWORD lpLineBuf = m_DrawData.lpLineBuf;
    LONG nWidth = m_DrawData.width;
    LONG nHeight = m_DrawData.height;

    //调节限制位置
    (void)AdjustPiont(ptSrcLT, ptSrcRB);

    //填充单行颜色
    if (lpLineBuf[0] != dwColor)
    {
        LPDWORD lpBuf = lpLineBuf;
        for (int i = 0; i < nWidth; i++)
        {
            *lpBuf++ = dwColor;
        }
    }

    //计算绘制区域宽高, 单行线条字节长度
    int nDrawWidth = ptSrcRB.x - ptSrcLT.x + 1;
    int nDrawHeight = ptSrcRB.y - ptSrcLT.y + 1;

    //格子数量限定
    if (xCount < 1) xCount = 1;
    if (yCount < 1) yCount = 1;

    //计算格子宽度
    int xGridSize = nDrawWidth / xCount;
    int yGridSize = nDrawHeight / yCount;

    LPDWORD lpData = nullptr;

    LONG nLineXBegin = ptSrcLT.x;
    LONG nLineXEnd = ptSrcRB.x;
    LONG nLineYBegin = ptSrcLT.y;
    LONG nLineYEnd = ptSrcRB.y;
    LONG nLineXSize = 0;

    if (nLineXBegin < 0) nLineXBegin = 0;
    if (nLineXEnd >= nWidth) nLineXEnd = nWidth - 1;
    if (nLineYBegin < 0) nLineYBegin = 0;
    if (nLineYEnd >= nHeight) nLineYEnd = nHeight - 1;
    nLineXSize = (nLineXEnd - nLineXBegin) * sizeof(DWORD);

    //左边竖线
    {
        if (bDotMode)
        {
            int nIndex = 0;
            LONG nCount = 0;
            for (LONG i = ptSrcLT.x; i < ptSrcRB.x && nCount < xCount; i += xGridSize, nCount++)
            {
                if (i >= 0 && i < nWidth)
                {
                    lpData = lpBits + nLineYBegin * nWidth + i;
                    for (LONG j = nLineYBegin; j < nLineYEnd; j++)
                    {
                        if (nIndex & 0x01)
                        {
                            *lpData = dwColor;
                        }
                        lpData += nWidth;
                        nIndex++;
                    }
                }
            }
        }
        else
        {
            LONG nCount = 0;
            for (LONG i = ptSrcLT.x; i < ptSrcRB.x && nCount < xCount; i += xGridSize, nCount++)
            {
                if (i >= 0 && i < nWidth)
                {
                    lpData = lpBits + nLineYBegin * nWidth + i;
                    for (LONG j = nLineYBegin; j < nLineYEnd; j++)
                    {
                        *lpData = dwColor;
                        lpData += nWidth;
                    }
                }
            }
        }
    }

    //顶部横线
    {
        if (bDotMode)
        {
            LONG nCount = 0;
            int nSize = nLineXEnd - nLineXBegin;
            for (LONG i = ptSrcLT.y; i < ptSrcRB.y && nCount < yCount; i += yGridSize, nCount++)
            {
                if (i >= 0 && i < nHeight)
                {
                    lpData = lpBits + i * nWidth + nLineXBegin;

                    int nIndex = 0;
                    for (int j = 0; j < nSize; j++)
                    {
                        if (nIndex & 0x01)
                        {
                            lpData[j] = dwColor;
                        }
                        nIndex++;
                    }

                }
            }
        }
        else
        {
            LONG nCount = 0;
            for (LONG i = ptSrcLT.y; i < ptSrcRB.y && nCount < yCount; i += yGridSize, nCount++)
            {
                if (i >= 0 && i < nHeight)
                {
                    lpData = lpBits + i * nWidth + nLineXBegin;
                    ::memcpy_s(lpData, nLineXSize, lpLineBuf, nLineXSize);
                }
            }
        }
    }

    //右边竖线
    if (bDotMode)
    {
        if (ptSrcRB.x >= 0 && ptSrcRB.x < nWidth)
        {
            int nIndex = 0;
            lpData = lpBits + nLineYBegin * nWidth + ptSrcRB.x;
            for (LONG i = nLineYBegin; i <= nLineYEnd; i++)
            {
                if (nIndex & 0x01)
                {
                    *lpData = dwColor;
                }

                lpData += nWidth;
                nIndex++;
            }
        }
    }
    else
    {
        if (ptSrcRB.x >= 0 && ptSrcRB.x < nWidth)
        {
            lpData = lpBits + nLineYBegin * nWidth + ptSrcRB.x;
            for (LONG i = nLineYBegin; i <= nLineYEnd; i++)
            {
                *lpData = dwColor;
                lpData += nWidth;
            }
        }
    }

    //底部横线
    if (ptSrcRB.y >= 0 && ptSrcRB.y < nHeight)
    {
        lpData = lpBits + nLineYEnd * nWidth + nLineXBegin;
        if (bDotMode)
        {
            int nIndex = 0;
            int nSize = nLineXEnd - nLineXBegin;
            for (int i = 0; i < nSize; i++)
            {
                if (nIndex & 0x01)
                {
                    lpData[i] = dwColor;
                }
                nIndex++;
            }
        }
        else
        {
            ::memcpy_s(lpData, nLineXSize, lpLineBuf, nLineXSize);
        }
    }
}

void CBitmapDraw::FillGrid(POINT ptLT, POINT ptRB, DWORD dwColor, LONG xCount/* = 1*/, LONG yCount/* = 1*/, POINT pt/* = { 0, 0 }*/)
{
    dwColor = COLOR_RGB_TO_BGR(dwColor);
    if (nullptr == m_DrawData.lpBits)
    {
        return;
    }

    LPDWORD lpBits = m_DrawData.lpBits;
    LPDWORD lpLineBuf = m_DrawData.lpLineBuf;
    LONG nWidth = m_DrawData.width;

    //调节限制位置
    (void)AdjustPiont(ptLT, ptRB);
    (void)LimitPiont(ptLT, ptRB);

    //填充单行颜色
    if (lpLineBuf[0] != dwColor)
    {
        LPDWORD lpBuf = lpLineBuf;
        for (int i = 0; i < nWidth; i++)
        {
            *lpBuf++ = dwColor;
        }
    }

    //计算绘制区域宽高, 单行线条字节长度
    int nDrawWidth = ptRB.x - ptLT.x + 1;
    int nDrawHeight = ptRB.y - ptLT.y + 1;

    //格子数量限定
    if (xCount < 1) xCount = 1;
    if (yCount < 1) yCount = 1;

    //限定目标格子坐标
    if (pt.x < 0) pt.x = 0;
    if (pt.y < 0) pt.y = 0;
    if (pt.x >= xCount) pt.x = xCount - 1;
    if (pt.y >= yCount) pt.y = yCount - 1;

    //计算格子宽度
    int xGridWidth = nDrawWidth / xCount;
    int yGridHeight = nDrawHeight / yCount;
    int xGridWidthSize = xGridWidth * sizeof(DWORD);

    //填充单行颜色
    if (lpLineBuf[0] != dwColor)
    {
        LPDWORD lpBuf = lpLineBuf;
        for (int i = 0; i < nWidth; i++)
        {
            *lpBuf++ = dwColor;
        }
    }

    //填充
    LPDWORD lpData = lpBits + (ptLT.y + pt.y * yGridHeight) * nWidth + ptLT.x + pt.x * xGridWidth;
    for (int i = 0; i < yGridHeight; i++)
    {
        memcpy_s(lpData, xGridWidthSize, lpLineBuf, xGridWidthSize);
        lpData += nWidth;
    }
}

void CBitmapDraw::FillRect(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor)
{
    dwColor = COLOR_RGB_TO_BGR(dwColor);
    if (nullptr == m_DrawData.lpBits)
    {
        return;
    }

    LPDWORD lpBits = m_DrawData.lpBits;
    LPDWORD lpLineBuf = m_DrawData.lpLineBuf;
    LONG nWidth = m_DrawData.width;

    POINT ptLT = ptSrcLT;
    POINT ptRB = ptSrcRB;

    //调节限制位置
    (void)AdjustPiont(ptLT, ptRB);
    (void)LimitPiont(ptLT, ptRB);

    //填充单行颜色
    if (lpLineBuf[0] != dwColor)
    {
        LPDWORD lpBuf = lpLineBuf;
        for (int i = 0; i < nWidth; i++)
        {
            *lpBuf++ = dwColor;
        }
    }

    //计算绘制区域宽高, 单行线条字节长度
    int nDrawWidth = ptRB.x - ptLT.x + 1;
    int nDrawHeight = ptRB.y - ptLT.y + 1;
    int nDrawLineSize = nDrawWidth * sizeof(DWORD);

    LPDWORD lpData = lpBits + (ptLT.y) * nWidth + ptLT.x;
    for (int i = 0; i < nDrawHeight; i++)
    {
        memcpy_s(lpData, nDrawLineSize, lpLineBuf, nDrawLineSize);
        lpData += nWidth;
    }
}

void CBitmapDraw::DrawRect(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor, bool bDotMode/* = true*/)
{
    dwColor = COLOR_RGB_TO_BGR(dwColor);
    if (nullptr == m_DrawData.lpBits)
    {
        return;
    }

    LPDWORD lpBits = m_DrawData.lpBits;
    LPDWORD lpLineBuf = m_DrawData.lpLineBuf;
    LONG nWidth = m_DrawData.width;
    LONG nHeight = m_DrawData.height;

    //调节限制位置
    (void)AdjustPiont(ptSrcLT, ptSrcRB);

    //填充单行颜色
    if (lpLineBuf[0] != dwColor)
    {
        LPDWORD lpBuf = lpLineBuf;
        for (int i = 0; i < nWidth; i++)
        {
            *lpBuf++ = dwColor;
        }
    }

    LONG nLineXBegin = ptSrcLT.x;
    LONG nLineXEnd = ptSrcRB.x;
    LONG nLineYBegin = ptSrcLT.y;
    LONG nLineYEnd = ptSrcRB.y;
    LONG nLineXSize = 0;

    if (nLineXBegin < 0) nLineXBegin = 0;
    if (nLineXEnd >= nWidth) nLineXEnd = nWidth - 1;
    if (nLineYBegin < 0) nLineYBegin = 0;
    if (nLineYEnd >= nHeight) nLineYEnd = nHeight - 1;
    nLineXSize = (nLineXEnd - nLineXBegin) * sizeof(DWORD);

    LPDWORD lpData = nullptr;

    //顶部横线
    if (ptSrcLT.y >= 0 && ptSrcLT.y < nHeight && nLineXSize > 0)
    {
        lpData = lpBits + nLineYBegin * nWidth + nLineXBegin;

        if (bDotMode)
        {
            int nIndex = 0;
            int nSize = nLineXEnd - nLineXBegin;
            for (int i = 0; i < nSize; i++)
            {
                if (nIndex & 0x01)
                {
                    lpData[i] = dwColor;
                }
                nIndex++;
            }
        }
        else
        {
            ::memcpy_s(lpData, nLineXSize, lpLineBuf, nLineXSize);
        }

    }

    //底部横线
    if (ptSrcRB.y >= 0 && ptSrcRB.y < nHeight && nLineXSize > 0)
    {
        lpData = lpBits + nLineYEnd * nWidth + nLineXBegin;
        if (bDotMode)
        {
            int nIndex = 0;
            int nSize = nLineXEnd - nLineXBegin;
            for (int i = 0; i < nSize; i++)
            {
                if (nIndex & 0x01)
                {
                    lpData[i] = dwColor;
                }
                nIndex++;
            }
        }
        else
        {
            ::memcpy_s(lpData, nLineXSize, lpLineBuf, nLineXSize);
        }
    }

    //左边竖线
    if (ptSrcLT.x >= 0 && ptSrcLT.x < nWidth)
    {
        lpData = lpBits + nLineYBegin * nWidth + nLineXBegin;

        if (bDotMode)
        {
            int nIndex = 0;
            for (LONG i = nLineYBegin; i < nLineYEnd; i++)
            {
                if (nIndex & 0x01)
                {
                    *lpData = dwColor;
                }
                lpData += nWidth;
                nIndex++;
            }
        }
        else
        {
            for (LONG i = nLineYBegin; i < nLineYEnd; i++)
            {
                *lpData = dwColor;
                lpData += nWidth;
            }
        }
    }

    //右边竖线
    if (ptSrcRB.x >= 0 && ptSrcRB.x < nWidth)
    {
        lpData = lpBits + nLineYBegin * nWidth + nLineXEnd;

        if (bDotMode)
        {
            int nIndex = 0;
            for (LONG i = nLineYBegin; i <= nLineYEnd; i++)
            {
                if (nIndex & 0x01)
                {
                    *lpData = dwColor;
                }
                lpData += nWidth;
                nIndex++;
            }
        }
        else
        {
            for (LONG i = nLineYBegin; i <= nLineYEnd; i++)
            {
                *lpData = dwColor;
                lpData += nWidth;
            }
        }
    }
}

void CBitmapDraw::DrawLine(POINT ptSrcLT, POINT ptSrcRB, DWORD dwColor, bool bDotMode/* = true*/)
{
    dwColor = COLOR_RGB_TO_BGR(dwColor);
    if (nullptr == m_DrawData.lpBits)
    {
        return;
    }

    LPDWORD lpBits = m_DrawData.lpBits;
    LONG nWidth = m_DrawData.width;
    LONG nHeight = m_DrawData.height;

    POINT ptLT = ptSrcLT;
    POINT ptRB = ptSrcRB;

    //计算绘制区域内相交点

    /*{
        int x0 = ptLT.x;
        int y0 = ptLT.y;
        int x1 = ptRB.x;
        int y1 = ptRB.y;

        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int erro = (dx > dy ? dx : -dy) / 2;

        do
        {
            lpBits[y0 * nWidth + x0] = dwColor;

            int e2 = erro;
            if (e2 > -dx)
            {
                erro -= dy;
                x0 += sx;
            }

            if (e2 < dy)
            {
                erro += dx;
                y0 += sy;
            }

        } while (x0 != x1 || y0 != y1);
    }*/

    //画任意斜率的直线（基于 Bresenham 算法）
    
    {
        int x1 = ptLT.x;
        int y1 = ptLT.y;
        int x2 = ptRB.x;
        int y2 = ptRB.y;

        int x = ptLT.x;
        int y = ptLT.y;

        int dx = ::abs(x2 - x1);
        int dy = ::abs(y2 - y1);
        int s1 = x2 > x1 ? 1 : -1;
        int s2 = y2 > y1 ? 1 : -1;

        bool interchange = false;// 默认不互换 dx、dy
        if (dy > dx)// 当斜率大于 1 时，dx、dy 互换
        {
            int temp = dx;
            dx = dy;
            dy = temp;
            interchange = true;
        }

        int p = 2 * dy - dx;

        if (!interchange)
        {
            if (bDotMode)
            {
                int nIndex = 0;
                for (int i = 0; i <= dx; i++)
                {
                    if (y >= 0 && y < nHeight && x >= 0 && x < nWidth)
                    {
                        if (nIndex & 0x01)
                        {
                            lpBits[y * nWidth + x] = dwColor;
                        }
                        nIndex++;
                    }

                    if (p >= 0)
                    {
                        y += s2;
                        p -= 2 * dx;
                    }

                    x += s1;// 当斜率 < 1 时，选取 x 为步长
                    p += 2 * dy;
                }
            }
            else
            {
                for (int i = 0; i <= dx; i++)
                {
                    if (y >= 0 && y < nHeight && x >= 0 && x < nWidth)
                    {
                        lpBits[y * nWidth + x] = dwColor;
                    }

                    if (p >= 0)
                    {
                        y += s2;
                        p -= 2 * dx;
                    }

                    x += s1;// 当斜率 < 1 时，选取 x 为步长
                    p += 2 * dy;
                }
            }
        }
        else
        {
            if (bDotMode)
            {
                int nIndex = 0;
                for (int i = 0; i <= dx; i++)
                {
                    if (y >= 0 && y < nHeight && x >= 0 && x < nWidth)
                    {
                        if (nIndex & 0x01)
                        {
                            lpBits[y * nWidth + x] = dwColor;
                        }
                        nIndex++;
                    }

                    if (p >= 0)
                    {
                        x += s1;
                        p -= 2 * dx;
                    }

                    y += s2;// 当斜率 > 1 时，选取 y 为步长
                    p += 2 * dy;
                }
            }
            else
            {
                for (int i = 0; i <= dx; i++)
                {
                    if (y >= 0 && y < nHeight && x >= 0 && x < nWidth)
                    {
                        lpBits[y * nWidth + x] = dwColor;
                    }

                    if (p >= 0)
                    {
                        x += s1;
                        p -= 2 * dx;
                    }

                    y += s2;// 当斜率 > 1 时，选取 y 为步长
                    p += 2 * dy;
                }
            }
        }
    }
}

void CBitmapDraw::DrawCircle(POINT ptLT, POINT ptRB, DWORD dwColor, bool bDotMode/* = true*/)
{
    dwColor = COLOR_RGB_TO_BGR(dwColor);
    if (nullptr == m_DrawData.lpBits)
    {
        return;
    }

    LPDWORD lpBits = m_DrawData.lpBits;
    LONG nWidth = m_DrawData.width;
    LONG nHeight = m_DrawData.height;

    //调节限制位置
    (void)AdjustPiont(ptLT, ptRB);

    LONG nW = ptRB.x - ptLT.x;
    LONG nH = ptRB.y - ptLT.y;

    LONG r = (LONG)::sqrt((nW * nW) + (nH * nH)) / 2;

    LONG x = ptLT.x;
    LONG y = ptLT.y;

    x += nW / 2;
    y += nH / 2;

    {
        int  xi;
        int  yi;
        int  di;
        di = 1 - r;
        xi = 0;
        yi = r;
        LONGLONG dwOffset = 0;
        
        int nXPox = 0;
        int nYPos = 0;

        if (bDotMode)
        {
            int nIndex = 0;
            while (yi >= xi)
            {
                //右下 1/4
                nXPox = y + yi;
                nYPos = x + xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                nXPox = y + xi;
                nYPos = x + yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                //左下 1/4
                nXPox = y + yi;
                nYPos = x - xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                nXPox = y + xi;
                nYPos = x - yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                //左上 1/4
                nXPox = y - yi;
                nYPos = x - xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                nXPox = y - xi;
                nYPos = x - yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                //右上 1/4
                nXPox = y - yi;
                nYPos = x + xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                nXPox = y - xi;
                nYPos = x + yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth && nIndex & 0x01) lpBits[dwOffset] = dwColor;

                if (di < 0)
                {
                    di += 3 + (xi << 1);
                }
                else
                {
                    di += 5 + ((xi - yi) << 1);
                    yi--;
                }
                xi++;
                nIndex++;
            }
        }
        else
        {
            while (yi >= xi)
            {
                //右下 1/4
                nXPox = y + yi;
                nYPos = x + xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                nXPox = y + xi;
                nYPos = x + yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                //左下 1/4
                nXPox = y + yi;
                nYPos = x - xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                nXPox = y + xi;
                nYPos = x - yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                //左上 1/4
                nXPox = y - yi;
                nYPos = x - xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                nXPox = y - xi;
                nYPos = x - yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                //右上 1/4
                nXPox = y - yi;
                nYPos = x + xi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                nXPox = y - xi;
                nYPos = x + yi;
                dwOffset = nXPox * nWidth + nYPos;
                if (nXPox >= 0 && nXPox < nHeight && nYPos >= 0 && nYPos < nWidth) lpBits[dwOffset] = dwColor;

                if (di < 0)
                {
                    di += 3 + (xi << 1);
                }
                else
                {
                    di += 5 + ((xi - yi) << 1);
                    yi--;
                }
                xi++;
            }
        }
    }
}

bool CBitmapDraw::GetCrossPoint(POINT& out, POINT p1, POINT p2, POINT p3, POINT p4)
{
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;

    int x3 = p3.x;
    int y3 = p3.y;
    int x4 = p4.x;
    int y4 = p4.y;

    double k1 = 0.0f;
    double b1 = 0.0f;
    bool k1_Vertical = false;
    if (x2 == x1)
    {
        k1_Vertical = true;
    }
    else
    {
        k1 = (double)(y2 - y1) / (double)(x2 - x1);
        b1 = (double)y1 - (double)x1 * k1;
    }

    double k2 = 0.0f;
    double b2 = 0.0f;
    bool k2_Vertical = false;
    if (x4 == x3)
    {
        k2_Vertical = true;
        b2 = 0;
    }
    else
    {
        k2 = (double)(y4 - y3) / (double)(x4 - x3);
        b2 = (double)y3 - (double)x3 * k2;
    }

    if (k1_Vertical)
    {
        if (!k2_Vertical)
        {
            out.x = x1;
            out.y = (LONG)((k2 * x1) + b2);
            return true;
        }
    }
    else if (k2_Vertical)
    {
        out.x = x3;
        out.y = (LONG)((k1 * x3) + b1);
        return true;
    }
    else if (k2 != k1)
    {
        out.x = (LONG)((double)(b2 - b1) / (double)(k1 - k2));
        out.y = (LONG)(k1 * (double)out.x + b1);
        return true;
    }

    out.x = 0;
    out.y = 0;
    return false;
}
