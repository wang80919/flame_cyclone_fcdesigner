#include "CDesignDialog.h"
#include "resource.h"
#include <thread>
#include "CCustomMenuData.h"
#include "Utils/CBitmapDraw.h"

void CDesignDialog::DrawBkgTile(
    HWND hWnd,
    HDC hdc,
    float nWidth,
    float nHeight,
    float nRow,
    float nCol,
    TILE_INFO tile,
    bool isTransparent/* = true*/
)
{
    int nSizeX = (int)(nWidth / nRow);
    int nSizeY = (int)(nHeight / nCol);

    int nPosX = (tile.data & 0x0F);
    int nPosY = ((tile.data & 0xF0) >> 4);

    DWORD dwIndex = GetNameTableColorIndex({ tile.pt.x, tile.pt.y });
    if (isTransparent)
    {
        std::lock_guard<std::mutex> lock(m_tileMutex);

        HDC hBackDC = m_wndTileDraw.GetBackDC();
        ::StretchBlt(
            hBackDC,
            0, 0, 16, 16,
            m_wndTileDraw.GetMemDC(),
            dwIndex * 128 + nPosX * 8, nPosY * 8, 8, 8, SRCCOPY);

        ::TransparentBlt(
            hdc,
            tile.pt.x * nSizeX, tile.pt.y * nSizeY, 16, 16,
            hBackDC,
            0, 0, 16, 16,
            GetTileBkgColor(tile)
        );
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_tileMutex);

        ::StretchBlt(
            hdc,
            tile.pt.x * nSizeX, tile.pt.y * nSizeY, 16, 16,
            m_wndTileDraw.GetMemDC(),
            dwIndex * 128 + nPosX * 8, nPosY * 8, 8, 8, SRCCOPY);
    }
}

void CDesignDialog::DrawSpriteTile(
    HWND hWnd,
    HDC hdc,
    TILE_INFO tile,
    bool isTransparent/* = true*/
)
{
    BYTE TileID = tile.data;

    int nPosX = (TileID & 0x0F);
    int nPosY = ((TileID & 0xF0) >> 4);

    if (tile.pt.y < 1 || tile.pt.y > 241 || tile.pt.x < 0 || tile.pt.x > 255)
    {
        return;
    }

    HDC hBackDC = m_wndTileDraw.GetBackDC();

    std::lock_guard<std::mutex> lock(m_tileMutex);

    if (m_bBigSprite)
    {
        DWORD dwBase = (TileID & 0x01) ? 128 : 256;
        nPosX &= ~TILE_FLAG_CHECKED;

        if (isTransparent)
        {
            if (tile.mode.bVerticalFlip)
            {
                ::StretchBlt(
                    hBackDC,
                    0, 0, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + nPosX * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    -8, SRCCOPY);

                ::TransparentBlt(
                    hdc,
                    tile.pt.x * 2, (tile.pt.y + 1) * 2 + 16, 16, 16,
                    hBackDC,
                    0, 0, 16, 16,
                    GetTileBkgColor(tile)
                );

                ::StretchBlt(
                    hBackDC,
                    0, 0, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + (nPosX + 1) * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    -8, SRCCOPY);

                ::TransparentBlt(
                    hdc,
                    tile.pt.x * 2, (tile.pt.y + 1) * 2, 16, 16,
                    hBackDC,
                    0, 0, 16, 16,
                    GetTileBkgColor(tile)
                );
            }
            else
            {
                ::StretchBlt(
                    hBackDC,
                    0, 0, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + nPosX * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    8, SRCCOPY);

                ::TransparentBlt(
                    hdc,
                    tile.pt.x * 2, (tile.pt.y + 1) * 2, 16, 16,
                    hBackDC,
                    0, 0, 16, 16,
                    GetTileBkgColor(tile)
                );

                ::StretchBlt(
                    hBackDC,
                    0, 0, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + (nPosX + 1) * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    8, SRCCOPY);

                ::TransparentBlt(
                    hdc,
                    tile.pt.x * 2, (tile.pt.y + 1) * 2 + 16, 16, 16,
                    hBackDC,
                    0, 0, 16, 16,
                    GetTileBkgColor(tile)
                );
            }

        }
        else
        {
            if (tile.mode.bVerticalFlip)
            {
                ::StretchBlt(
                    hdc,
                    tile.pt.x * 2, tile.pt.y * 2 + 16, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + nPosX * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    -8, SRCCOPY);

                ::StretchBlt(
                    hdc,
                    tile.pt.x * 2, tile.pt.y * 2, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + (nPosX + 1) * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + (nPosY) * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    -8, SRCCOPY);
            }
            else
            {
                ::StretchBlt(
                    hdc,
                    tile.pt.x * 2, tile.pt.y * 2, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + nPosX * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    8, SRCCOPY);

                ::StretchBlt(
                    hdc,
                    tile.pt.x * 2, tile.pt.y * 2 + 16, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    tile.color * 128 + (nPosX + 1) * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                    dwBase + (nPosY) * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                    tile.mode.bHorizontalFlip ? -8 : 8,
                    8, SRCCOPY);
            }
        }
    }
    else
    {
        if (isTransparent)
        {
            ::StretchBlt(
                hBackDC,
                0, 0, 16, 16,
                m_wndTileDraw.GetMemDC(),
                tile.color * 128 + nPosX * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                128 + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                tile.mode.bHorizontalFlip ? -8 : 8,
                tile.mode.bVerticalFlip ? -8 : 8, SRCCOPY);

            ::TransparentBlt(
                hdc,
                tile.pt.x * 2, (tile.pt.y + 1) * 2, 16, 16,
                hBackDC,
                0, 0, 16, 16,
                GetTileBkgColor(tile)
            );
        }
        else
        {
            ::StretchBlt(
                hdc,
                tile.pt.x * 2, (tile.pt.y + 1) * 2, 16, 16,
                m_wndTileDraw.GetMemDC(),
                tile.color * 128 + nPosX * 8 + (tile.mode.bHorizontalFlip ? 7 : 0),
                128 + nPosY * 8 + (tile.mode.bVerticalFlip ? 7 : 0),
                tile.mode.bHorizontalFlip ? -8 : 8,
                tile.mode.bVerticalFlip ? -8 : 8, SRCCOPY);
        }
    }
}

void CDesignDialog::FillGridText(
    HWND hWnd,
    HDC hdc,
    float nWidth,
    float nHeight,
    float nRow,
    float nCol,
    POINT pt,
    _tstring strText
)
{
    RECT rtClient = { 0 };
    ::GetClientRect(hWnd, &rtClient);

    float nW = (float)(rtClient.right - rtClient.left);
    float nH = (float)(rtClient.bottom - rtClient.top);

    rtClient.left = rtClient.left + (LONG)((nW - nWidth) / 2);
    rtClient.top = rtClient.top + (LONG)((nH - nHeight) / 2);
    rtClient.right = rtClient.left + (LONG)nWidth;
    rtClient.bottom = rtClient.top + (LONG)nHeight;

    int nSizeX = (LONG)(nWidth / nRow);
    int nSizeY = (LONG)(nHeight / nCol);

    RECT rtText = { 0 };

    if (!strText.empty())
    {
        rtText.left = pt.x * nSizeX;
        rtText.top = pt.y * nSizeY;
        rtText.right = (LONG)((pt.x + strText.size()) * nSizeX);
        rtText.bottom = (pt.y + 1) * nSizeY;
    }

    if (rtText.left < rtClient.left)
    {
        rtText.left = rtClient.left;
    }

    if (rtText.right > rtClient.right)
    {
        rtText.right = rtClient.right;
    }

    {
        std::lock_guard<std::mutex> lock(m_tileMutex);
        for (LONG i = 0; i < (LONG)strText.size(); i++)
        {
            if (_T(' ') == strText[i])
            {
                continue;
            }

            TCHAR ch = g_CharMapData[strText[i]];

            RECT rtFill = {
                rtClient.left + (pt.x + i) * nSizeX,
                rtClient.top + pt.y * nSizeY,
                rtClient.left + (pt.x + i) * nSizeX + nSizeX,
                rtClient.top + pt.y * nSizeY + nSizeY,
            };

            if (pt.x + i < 0)
            {
                continue;
            }

            if (pt.x + i > 31)
            {
                continue;
            }

            int nPosX = (ch & 0x0F);
            int nPosY = ((ch & 0xF0) >> 4);

            //绘制 Tile
            {
                DWORD dwIndex = GetNameTableColorIndex({ pt.x + i, pt.y });
                ::StretchBlt(
                    hdc,
                    rtFill.left, rtFill.top, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    dwIndex * 128 + nPosX * 8, nPosY * 8, 8, 8, SRCCOPY);
            }
        }
    }
}

void CDesignDialog::DrawItemList()
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();

    for (int i = 0; i < (LONG)m_strItemList.size(); i++)
    {
        if (m_strItemList[i].empty())
        {
            continue;
        }

        FillGridText(hWnd, hdc, 512, 480, 32, 30, { m_ptItem.x, m_ptItem.y + i }, m_strItemList[i]);
    }
}

void CDesignDialog::DrawTextList()
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();

    for (size_t i = 0; i < m_CustomTextList.size(); i++)
    {
        if (m_CustomTextList[i].strText.empty())
        {
            continue;
        }

        FillGridText(hWnd, hdc, 512, 480, 32, 30, m_CustomTextList[i].pt, m_CustomTextList[i].strText);
    }

    if (!m_CustomText.strText.empty())
    {
        FillGridText(hWnd, hdc, 512, 480, 32, 30, m_CustomText.pt, m_CustomText.strText);
    }
}

void CDesignDialog::DrawBkgTileList(bool bFocus/* = false*/)
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();
    HDC hDrawDC = m_hWndNameTable.GetDrawDC();

    if (!bFocus)
    {
        if (!m_bFillColor)
        {
            CBitmapDraw draw(hDrawDC);
            if (draw.BeginDraw())
            {
                draw.FillRect({ 0, 0 }, { 511, 479 }, GetBkgColor());

                draw.EndDraw();
            }

            RECT rect = { 0 };
            ::GetClientRect(hWnd, &rect);

            for (const auto& item : m_BkgTileList)
            {
                DrawBkgTile(hWnd, hDrawDC, 512, 480, 32, 30, item, false);
            }

            ::TransparentBlt(
                hdc,
                0, 0, rect.right, rect.bottom,
                hDrawDC,
                0, 0, rect.right, rect.bottom,
                GetBkgColor());
        }
        else
        {
            for (const auto& item : m_BkgTileList)
            {
                DrawBkgTile(hWnd, hdc, 512, 480, 32, 30, item, false);
            }
        }
    }
    else
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            for (const auto& item : m_BkgTileList)
            {
                if (!(item.flag & TILE_FLAG_CHECKED))
                {
                    continue;
                }
                POINT ptLT = { item.pt.x * 16, item.pt.y * 16 };
                POINT ptRB = { (item.pt.x + 1) * 16, (item.pt.y + 1) * 16 };

                obj.LimitPiont(ptLT, ptRB);
                obj.DrawRect(ptLT, ptRB, CHECK_TILE_COLOR);

            }
            obj.EndDraw();
        }
    }
}

void CDesignDialog::DrawSpriteTileList(bool bFocus/* = false*/, bool bBack/* = false*/)
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();

    if (!bFocus)
    {
        for (const auto& item : m_SpTileList)
        {
            if (item.mode.bBackgroundDisplay == bBack)
            {
                DrawSpriteTile(hWnd, hdc, item, true);
            }
        }
    }
    else
    {
        CBitmapDraw draw(hdc);

        if (draw.BeginDraw())
        {
            for (const auto& item : m_SpTileList)
            {
                if (!(item.flag & TILE_FLAG_CHECKED))
                {
                    continue;
                }

                POINT ptLT = { item.pt.x * 2, item.pt.y * 2 + 1 };
                POINT ptRB = { (item.pt.x + 8) * 2, (item.pt.y + 8) * 2 + 1 };

                if (m_bBigSprite)
                {
                    ptLT = { item.pt.x * 2, item.pt.y * 2 + 1 };
                    ptRB = { (item.pt.x + 8) * 2, (item.pt.y + 8 + 8) * 2 + 1 };
                }
                else
                {
                    ptLT = { item.pt.x * 2, item.pt.y * 2 + 1 };
                    ptRB = { ptLT.x + 8 * 2, ptLT.y + 8 * 2 + 1 };
                }

                draw.LimitPiont(ptLT, ptRB);

                draw.DrawRect(ptLT, ptRB, FOCUS_SP_COLOR);

            }

            draw.EndDraw();
        }
    }
}

void CDesignDialog::DrawCursorTile()
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();

    //绘制箭头
    DrawSpriteTile(hWnd, hdc, m_tileCursor);

    if (m_bMarkCursor)
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            {
                POINT ptLT = { m_tileCursor.pt.x * 2, m_tileCursor.pt.y * 2 };
                POINT ptRB = { (m_tileCursor.pt.x) * 2 + 16, (m_tileCursor.pt.y) * 2 + 16 };

                if (m_bBigSprite)
                {
                    ptRB = { (m_tileCursor.pt.x) * 2 + 16, (m_tileCursor.pt.y) * 2 + 32 };
                }

                obj.LimitPiont(ptLT, ptRB);
                obj.DrawRect(ptLT, ptRB, MARK_TILE_COLOR);
            }

            obj.EndDraw();
        }
    }
}

void CDesignDialog::DrawCursorTileList(bool bFocus/* = false*/)
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();
    HDC hDrawDC = m_hWndNameTable.GetDrawDC();

    RECT rect = { 0 };
    ::GetClientRect(hWnd, &rect);
    {
        HBRUSH hBkgBrush = ::CreateSolidBrush(GetBkgColor());
        ::FillRect(hDrawDC, &rect, hBkgBrush);
        ::DeleteObject(hBkgBrush);
    }

    if (!bFocus)
    {
        for (const auto& item : m_CursorTileList)
        {
            DrawSpriteTile(hWnd, hDrawDC, item, false);
        }

        ::TransparentBlt(
            hdc,
            0, 0, rect.right, rect.bottom,
            hDrawDC,
            0, 0, rect.right, rect.bottom,
            GetBkgColor()
        );
    }
    else
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            for (const auto& item : m_CursorTileList)
            {
                if (!(item.flag & TILE_FLAG_CHECKED))
                {
                    continue;
                }

                POINT ptLT = { item.pt.x * 2, item.pt.y * 2 };
                POINT ptRB = { item.pt.x * 2 + 16, item.pt.y * 2 + 16 };

                if (m_bBigSprite)
                {
                    ptRB = { item.pt.x * 2 + 16, item.pt.y * 2 + 32 };
                }

                obj.LimitPiont(ptLT, ptRB);
                obj.DrawRect(ptLT, ptRB, FOCUS_CURSOR_COLOR);
            }

            obj.EndDraw();
        }
    }

    if (m_bMarkCursor)
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw() && !m_CursorTileList.empty())
        {
            POINT pt = m_CursorTileList.front().pt;

            POINT ptLT = { pt.x * 2, pt.y * 2 };
            POINT ptRB = { pt.x * 2 + 32, pt.y * 2 + 32 };

            if (m_bBigSprite)
            {
                ptRB = { pt.x * 2 + 32, pt.y * 2 + 64 };
            }

            obj.LimitPiont(ptLT, ptRB);
            obj.DrawRect(ptLT, ptRB, MARK_TILE_COLOR);

            obj.EndDraw();
        }
    }
}

void CDesignDialog::DrawAttribute()
{
    BYTE data = m_AttributeData;
    DWORD dwLTIndex = (data & 0x03) >> 0;
    DWORD dwRTIndex = (data & 0x0C) >> 2;
    DWORD dwLBIndex = (data & 0x30) >> 4;
    DWORD dwRBIndex = (data & 0xC0) >> 6;

    DWORD dwLT = g_PaletteColor[g_BkgPalette[dwLTIndex * 4 + 1]];
    DWORD dwRT = g_PaletteColor[g_BkgPalette[dwRTIndex * 4 + 1]];
    DWORD dwLB = g_PaletteColor[g_BkgPalette[dwLBIndex * 4 + 1]];
    DWORD dwRB = g_PaletteColor[g_BkgPalette[dwRBIndex * 4 + 1]];

    HWND hWnd = m_hWndAttribute;
    HDC hdc = m_hWndAttribute.GetMemDC();

    CBitmapDraw obj(hdc);
    if (obj.BeginDraw())
    {
        POINT ptLT = { 0,0 };
        POINT ptRB = { 128, 128 };

        obj.LimitPiont(ptLT, ptRB);

        //左上
        obj.FillGrid(ptLT, ptRB, dwLT, 2, 2, { 0, 0 });

        //右上
        obj.FillGrid(ptLT, ptRB, dwRT, 2, 2, { 1, 0 });

        //左下
        obj.FillGrid(ptLT, ptRB, dwLB, 2, 2, { 0, 1 });

        //右下
        obj.FillGrid(ptLT, ptRB, dwRB, 2, 2, { 1, 1 });

        //线条
        obj.DrawGrid(ptLT, ptRB, GRID_LINE_COLOR, 4, 4, true);

        //焦点
        {
            POINT ptFocus = m_ptCurAttribute;
            POINT ptCurLT = { ptFocus.x * 64, ptFocus.y * 64 };
            POINT ptCurRB = { (ptFocus.x + 1) * 64, (ptFocus.y + 1) * 64 };
            obj.LimitPiont(ptCurLT, ptCurRB);
            obj.DrawRect(ptCurLT, ptCurRB, FOCUS_COLOR);
        }

        obj.EndDraw();
    }

    ::InvalidateRect(hWnd, NULL, TRUE);
}

void CDesignDialog::DrawBkgPalette()
{
    HWND hWnd = m_hWndBkgPalette;
    HDC hdc = m_hWndBkgPalette.GetMemDC();

    CBitmapDraw obj(hdc);
    if (obj.BeginDraw())
    {
        //填充颜色
        for (int i = 0; i < 16; i++)
        {
            POINT ptLT = { i * 16, 0 };
            POINT ptRB = { (i + 1) * 16, 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.FillRect(ptLT, ptRB, g_PaletteColor[g_BkgPalette[i]]);

        }

        //绘制焦点
        if (!m_SelSpPalette)
        {
            POINT ptLT = { 0, 0 };
            POINT ptRB = { 256, 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.DrawGrid(ptLT, ptRB, FOCUS_COLOR);
        }

        //绘制选中点
        {
            POINT ptFocus = m_ptCurBkgPalette;
            POINT ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
            POINT ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.DrawRect(ptLT, ptRB, FOCUS_BKG_PALETTE_COLOR);
        }

        obj.EndDraw();
    }

    ::InvalidateRect(hWnd, NULL, TRUE);
}

void CDesignDialog::CDesignDialog::DrawSpritePalette()
{
    HWND hWnd = m_hWndSpPalette;
    HDC hdc = m_hWndSpPalette.GetMemDC();

    CBitmapDraw obj(hdc);
    if (obj.BeginDraw())
    {
        //填充颜色
        for (int i = 0; i < 16; i++)
        {
            POINT ptLT = { i * 16, 0 };
            POINT ptRB = { (i + 1) * 16, 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.FillRect(ptLT, ptRB, g_PaletteColor[g_SpritePalette[i]]);
        }

        //绘制焦点
        if (m_SelSpPalette)
        {
            POINT ptLT = { 0, 0 };
            POINT ptRB = { 256, 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.DrawGrid(ptLT, ptRB, FOCUS_COLOR);
        }

        //绘制选中点
        {
            POINT ptFocus = m_ptCurSpPalette;
            POINT ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
            POINT ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.DrawRect(ptLT, ptRB, FOCUS_BKG_PALETTE_COLOR);
        }

        obj.EndDraw();
    }

    ::InvalidateRect(hWnd, NULL, TRUE);
}

void CDesignDialog::DrawPalette()
{
    HWND hWnd = m_hWndPalette;
    HDC hdc = m_hWndPalette.GetMemDC();

    CBitmapDraw obj(hdc);
    if (obj.BeginDraw())
    {
        //填充颜色
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    POINT ptLT = { j * 16, i * 16 };
                    POINT ptRB = { (j + 1) * 16, (i + 1) * 16 };
                    obj.LimitPiont(ptLT, ptRB);
                    obj.FillRect(ptLT, ptRB, g_PaletteColor[i * 16 + j]);
                }
            }
        }

        //绘制焦点
        {
            POINT ptFocus = m_ptCurPalette;
            POINT ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
            POINT ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.DrawGrid(ptLT, ptRB, FOCUS_PALETTE_COLOR);
        }

        obj.EndDraw();
    }

    ::InvalidateRect(hWnd, NULL, TRUE);
}

void CDesignDialog::DrawTile(int nBkgBegin/* = 0*/, int nBkgCount/* = 4*/, int nSpriteBegin/* = 0*/, int nSpriteCount/* = 1*/)
{
    std::lock_guard<std::mutex> lock(m_tileMutex);

    HDC hdc = m_wndTileDraw.GetMemDC();
    HBITMAP hBitmap = m_wndTileDraw.GetBitmap();

    BITMAPINFO binfo = { 0 };
    binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    ::GetDIBits(hdc, hBitmap, 0, 0, NULL, &binfo, DIB_RGB_COLORS);
    binfo.bmiHeader.biPlanes = 1;
    binfo.bmiHeader.biBitCount = 32;

    LONG nWidth = binfo.bmiHeader.biWidth;
    LONG nHeight = abs(binfo.bmiHeader.biHeight);

    LPDWORD lpBits = new (std::nothrow) DWORD[nWidth * nHeight];
    if (nullptr == lpBits)
    {
        return;
    }

    binfo.bmiHeader.biCompression = BI_RGB;
    if (binfo.bmiHeader.biHeight > 0) binfo.bmiHeader.biHeight = 0 - binfo.bmiHeader.biHeight;

    ::GetDIBits(hdc, hBitmap, 0, nHeight, lpBits, &binfo, DIB_RGB_COLORS);
    binfo.bmiHeader.biCompression = BI_RGB;

    // 背景VRAM
    for (int count = nBkgBegin; count < nBkgBegin + nBkgCount; count++)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                INT color[8][8] = { 0 };
                LPBYTE pDataL = &g_ChrRam[i * 0x100 + j * 0x10 + 0];
                LPBYTE pDataH = &g_ChrRam[i * 0x100 + j * 0x10 + 8];

                for (int y = 0; y < 8; y++)
                {
                    //组合高低位颜色
                    color[y][0] = ((pDataH[y] & 0x80) >> 6 | (pDataL[y] & 0x80) >> 7);
                    color[y][1] = ((pDataH[y] & 0x40) >> 5 | (pDataL[y] & 0x40) >> 6);
                    color[y][2] = ((pDataH[y] & 0x20) >> 4 | (pDataL[y] & 0x20) >> 5);
                    color[y][3] = ((pDataH[y] & 0x10) >> 3 | (pDataL[y] & 0x10) >> 4);
                    color[y][4] = ((pDataH[y] & 0x08) >> 2 | (pDataL[y] & 0x08) >> 3);
                    color[y][5] = ((pDataH[y] & 0x04) >> 1 | (pDataL[y] & 0x04) >> 2);
                    color[y][6] = ((pDataH[y] & 0x02) >> 0 | (pDataL[y] & 0x02) >> 1);
                    color[y][7] = ((pDataH[y] & 0x01) << 1 | (pDataL[y] & 0x01) >> 0);
                }

                //8 * 8 像素
                for (int y = 0; y < 8; y++)
                {
                    for (int x = 0; x < 8; x++)
                    {
                        DWORD dwColor = g_PaletteColor[g_BkgPalette[count * 4 + color[y][x]]];
                        DWORD dwRGB = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
                        SetBitmap32Data(lpBits, nWidth * nHeight, nWidth, nHeight, count * 128 + j * 8 + x, i * 8 + y, dwRGB);
                    }
                }
            }
        }
    }

    // 大精灵右VRAM
    for (int count = nSpriteBegin; count < nSpriteBegin + nSpriteCount; count++)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                INT color[8][8] = { 0 };

                LPBYTE pDataL = &g_ChrRam[0x1000 + i * 0x100 + j * 0x10 + 0];
                LPBYTE pDataH = &g_ChrRam[0x1000 + i * 0x100 + j * 0x10 + 8];

                for (int y = 0; y < 8; y++)
                {
                    //组合高低位颜色
                    color[y][0] = ((pDataH[y] & 0x80) >> 6 | (pDataL[y] & 0x80) >> 7);
                    color[y][1] = ((pDataH[y] & 0x40) >> 5 | (pDataL[y] & 0x40) >> 6);
                    color[y][2] = ((pDataH[y] & 0x20) >> 4 | (pDataL[y] & 0x20) >> 5);
                    color[y][3] = ((pDataH[y] & 0x10) >> 3 | (pDataL[y] & 0x10) >> 4);
                    color[y][4] = ((pDataH[y] & 0x08) >> 2 | (pDataL[y] & 0x08) >> 3);
                    color[y][5] = ((pDataH[y] & 0x04) >> 1 | (pDataL[y] & 0x04) >> 2);
                    color[y][6] = ((pDataH[y] & 0x02) >> 0 | (pDataL[y] & 0x02) >> 1);
                    color[y][7] = ((pDataH[y] & 0x01) << 1 | (pDataL[y] & 0x01) >> 0);
                }

                //8 * 8 像素
                for (int y = 0; y < 8; y++)
                {
                    for (int x = 0; x < 8; x++)
                    {
                        DWORD dwColorIndex = g_SpritePalette[count * 4 + color[y][x]];
                        DWORD dwColor = g_PaletteColor[dwColorIndex];

                        DWORD dwRGB = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
                        if (dwRGB == 0 && (color[y][x]))
                        {
                            dwRGB = 0x00000001;
                        }

                        SetBitmap32Data(lpBits, nWidth * nHeight, nWidth, nHeight, count * 128 + j * 8 + x, 128 + i * 8 + y, dwRGB);
                    }
                }
            }
        }
    }

    // 大精灵左VRAM
    for (int count = nSpriteBegin; count < nSpriteBegin + nSpriteCount; count++)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                INT color[8][8] = { 0 };

                LPBYTE pDataL = &g_ChrRam[0x0000 + i * 0x100 + j * 0x10 + 0];
                LPBYTE pDataH = &g_ChrRam[0x0000 + i * 0x100 + j * 0x10 + 8];

                for (int y = 0; y < 8; y++)
                {
                    //组合高低位颜色
                    color[y][0] = ((pDataH[y] & 0x80) >> 6 | (pDataL[y] & 0x80) >> 7);
                    color[y][1] = ((pDataH[y] & 0x40) >> 5 | (pDataL[y] & 0x40) >> 6);
                    color[y][2] = ((pDataH[y] & 0x20) >> 4 | (pDataL[y] & 0x20) >> 5);
                    color[y][3] = ((pDataH[y] & 0x10) >> 3 | (pDataL[y] & 0x10) >> 4);
                    color[y][4] = ((pDataH[y] & 0x08) >> 2 | (pDataL[y] & 0x08) >> 3);
                    color[y][5] = ((pDataH[y] & 0x04) >> 1 | (pDataL[y] & 0x04) >> 2);
                    color[y][6] = ((pDataH[y] & 0x02) >> 0 | (pDataL[y] & 0x02) >> 1);
                    color[y][7] = ((pDataH[y] & 0x01) << 1 | (pDataL[y] & 0x01) >> 0);
                }

                //8 * 8 像素
                for (int y = 0; y < 8; y++)
                {
                    for (int x = 0; x < 8; x++)
                    {
                        DWORD dwColorIndex = g_SpritePalette[count * 4 + color[y][x]];
                        DWORD dwColor = g_PaletteColor[dwColorIndex];

                        DWORD dwRGB = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
                        if (dwRGB == 0 && (color[y][x]))
                        {
                            dwRGB = 0x00000001;
                        }

                        SetBitmap32Data(lpBits, nWidth * nHeight, nWidth, nHeight, count * 128 + j * 8 + x, 256 + i * 8 + y, dwRGB);
                    }
                }
            }
        }
    }

    SetDIBits(hdc, hBitmap, 0, nHeight, lpBits, &binfo, DIB_RGB_COLORS);
    delete[] lpBits;
    ::InvalidateRect(m_wndTileDraw, NULL, FALSE);
}

void CDesignDialog::DrawDefaultTile()
{
    HDC hdc = m_wndDefaultTile.GetMemDC();
    HBITMAP hBitmap = m_wndDefaultTile.GetBitmap();

    BITMAPINFO binfo = { 0 };
    binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    ::GetDIBits(hdc, hBitmap, 0, 0, NULL, &binfo, DIB_RGB_COLORS);
    binfo.bmiHeader.biPlanes = 1;
    binfo.bmiHeader.biBitCount = 32;

    LONG nWidth = binfo.bmiHeader.biWidth;
    LONG nHeight = abs(binfo.bmiHeader.biHeight);

    LPDWORD lpBits = new (std::nothrow) DWORD[nWidth * nHeight];
    if (nullptr == lpBits)
    {
        return;
    }

    binfo.bmiHeader.biCompression = BI_RGB;
    if (binfo.bmiHeader.biHeight > 0) binfo.bmiHeader.biHeight = 0 - binfo.bmiHeader.biHeight;

    ::GetDIBits(hdc, hBitmap, 0, nHeight, lpBits, &binfo, DIB_RGB_COLORS);
    binfo.bmiHeader.biCompression = BI_RGB;

    DWORD dwDefaultColor[4] = { RGB(0,0,0), RGB(255,255,255), RGB(0,0,0), RGB(0,0,0) };

    for (int count = 0; count < 1; count++)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                INT color[8][8] = { 0 };
                LPBYTE pDataL = &g_ChrRamDefault[i * 0x100 + j * 0x10 + 0];
                LPBYTE pDataH = &g_ChrRamDefault[i * 0x100 + j * 0x10 + 8];

                for (int y = 0; y < 8; y++)
                {
                    //组合高低位颜色
                    color[y][0] = ((pDataH[y] & 0x80) >> 6 | (pDataL[y] & 0x80) >> 7);
                    color[y][1] = ((pDataH[y] & 0x40) >> 5 | (pDataL[y] & 0x40) >> 6);
                    color[y][2] = ((pDataH[y] & 0x20) >> 4 | (pDataL[y] & 0x20) >> 5);
                    color[y][3] = ((pDataH[y] & 0x10) >> 3 | (pDataL[y] & 0x10) >> 4);
                    color[y][4] = ((pDataH[y] & 0x08) >> 2 | (pDataL[y] & 0x08) >> 3);
                    color[y][5] = ((pDataH[y] & 0x04) >> 1 | (pDataL[y] & 0x04) >> 2);
                    color[y][6] = ((pDataH[y] & 0x02) >> 0 | (pDataL[y] & 0x02) >> 1);
                    color[y][7] = ((pDataH[y] & 0x01) << 1 | (pDataL[y] & 0x01) >> 0);
                }

                //8 * 8 像素
                for (int y = 0; y < 8; y++)
                {
                    for (int x = 0; x < 8; x++)
                    {
                        DWORD dwColor = dwDefaultColor[color[y][x]];
                        DWORD dwRGB = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
                        SetBitmap32Data(lpBits, nWidth * nHeight, nWidth, nHeight, count * 128 + j * 8 + x, i * 8 + y, dwRGB);
                    }
                }
            }
        }
    }

    SetDIBits(hdc, hBitmap, 0, nHeight, lpBits, &binfo, DIB_RGB_COLORS);
    delete[] lpBits;
    ::InvalidateRect(m_wndDefaultTile, NULL, FALSE);
}

void CDesignDialog::DrawBkgVram()
{
    HDC hMemDC = m_wndBkgTile.GetMemDC();
    HDC hBackMemDC = m_wndBkgTile.GetBackDC();
    HDC hSrcMemDC = m_wndTileDraw.GetMemDC();

    {
        std::lock_guard<std::mutex> lock(m_tileMutex);
        StretchBlt(hBackMemDC, 0, 0, 256, 256, hSrcMemDC, m_ptCurBkgPalette.x / 4 * 128, 0, 128, 128, SRCCOPY);
    }

    BitBlt(hMemDC, 0, 0, 256, 256, hBackMemDC, 0, 0, SRCCOPY);

    CBitmapDraw draw(hMemDC);
    if (draw.BeginDraw())
    {
        //网格
        draw.DrawGrid({ 0, 0 }, { 255, 255 }, GRID_LINE_COLOR, 16, 16, true);

        for (int y = 0; y < 16; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                if (m_bBkgTileCheck[(y * 16) + x])
                {
                    POINT ptCurLT = { x * 16, y * 16 };
                    POINT ptCurRB = { (x + 1) * 16, (y + 1) * 16 };
                    draw.DrawRect(ptCurLT, ptCurRB, CHECK_TILE_COLOR);
                }
            }
        }

        if (m_bSelecting)
        {
            draw.DrawRect({ m_ptBkgTileSelectBegin.x, m_ptBkgTileSelectBegin.y }, { m_ptBkgTileSelectEnd.x, m_ptBkgTileSelectEnd.y }, SELECTING_LINE_COLOR);
        }

        //焦点
        POINT ptLT = { m_ptCurBkgTile.x * 16, m_ptCurBkgTile.y * 16 };
        POINT ptRB = { m_ptCurBkgTile.x * 16 + 16, m_ptCurBkgTile.y * 16 + 16 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);
        ptLT.x--;
        ptLT.y--;
        ptRB.x++;
        ptRB.y++;
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);

        draw.DrawLine({ 0, 64 }, { 256, 64 }, GRID_PART_LINE_COLOR, true);
        draw.DrawLine({ 0, 128 }, { 256, 128 }, GRID_PART_LINE_COLOR, true);
        draw.DrawLine({ 0, 192 }, { 256, 192 }, GRID_PART_LINE_COLOR, true);

        draw.EndDraw();
    }

    ::InvalidateRect(m_wndBkgTile, NULL, FALSE);
}

void CDesignDialog::DrawSpVram()
{
    HDC hMemDC = m_wndSpTile.GetMemDC();
    HDC hBackMemDC = m_wndSpTile.GetBackDC();
    HDC hSrcMemDC = m_wndTileDraw.GetMemDC();

    {
        std::lock_guard<std::mutex> lock(m_tileMutex);
        StretchBlt(hBackMemDC, 0, 0, 256, 256, hSrcMemDC, m_ptCurSpPalette.x / 4 * 128, 128, 128, 128, SRCCOPY);
    }

    BitBlt(hMemDC, 0, 0, 256, 256, hBackMemDC, 0, 0, SRCCOPY);

    CBitmapDraw draw(hMemDC);
    if (draw.BeginDraw())
    {
        //网格
        draw.DrawGrid({ 0, 0 }, { 255, 255 }, GRID_LINE_COLOR, 16, 16, true);

        for (int y = 0; y < 16; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                if (m_bSpTileCheck[(y * 16) + x])
                {
                    POINT ptLT = { x * 16, y * 16 };
                    POINT ptRB = { (x + 1) * 16, (y + 1) * 16 };
                    draw.DrawRect(ptLT, ptRB, CHECK_TILE_COLOR);
                }
            }
        }

        if (m_bSelecting)
        {
            draw.DrawRect({ m_ptSpTileSelectBegin.x, m_ptSpTileSelectBegin.y }, { m_ptSpTileSelectEnd.x, m_ptSpTileSelectEnd.y }, SELECTING_LINE_COLOR);
        }

        //焦点
        if (m_bBigSprite)
        {
            POINT ptFocus = m_ptCurSpTile;
            ptFocus.x /= 2;
            POINT ptLT = { ptFocus.x * 32, ptFocus.y * 16 };
            POINT ptRB = { ptFocus.x * 32 + 32, ptFocus.y * 16 + 16 };
            draw.LimitPiont(ptLT, ptRB);
            draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);
            ptLT.x--;
            ptLT.y--;
            ptRB.x++;
            ptRB.y++;
            draw.LimitPiont(ptLT, ptRB);
            draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);

        }
        else
        {
            POINT ptLT = { m_ptCurSpTile.x * 16, m_ptCurSpTile.y * 16 };
            POINT ptRB = { m_ptCurSpTile.x * 16 + 16, m_ptCurSpTile.y * 16 + 16 };
            draw.LimitPiont(ptLT, ptRB);
            draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);
            ptLT.x--;
            ptLT.y--;
            ptRB.x++;
            ptRB.y++;
            draw.LimitPiont(ptLT, ptRB);
            draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);
        }

        draw.DrawLine({ 0, 64 }, { 256, 64 }, GRID_PART_LINE_COLOR, true);
        draw.DrawLine({ 0, 128 }, { 256, 128 }, GRID_PART_LINE_COLOR, true);
        draw.DrawLine({ 0, 192 }, { 256, 192 }, GRID_PART_LINE_COLOR, true);

        draw.EndDraw();
    }

    ::InvalidateRect(m_wndSpTile, NULL, FALSE);
}

void CDesignDialog::UpdateBkgVramEdit()
{
    HDC hMemDC = m_wndBkgTileEdit.GetMemDC();
    HDC hBackMemDC = m_wndBkgTileEdit.GetBackDC();
    HDC hSrcMemDC = m_wndBkgTile.GetBackDC();

    StretchBlt(hBackMemDC, 0, 0, 128, 128, hSrcMemDC, m_ptCurBkgTile.x * 16, m_ptCurBkgTile.y * 16, 16, 16, SRCCOPY);
    BitBlt(hMemDC, 0, 0, 128, 128, hBackMemDC, 0, 0, SRCCOPY);

    CBitmapDraw draw(hMemDC);
    if (draw.BeginDraw())
    {
        POINT ptLT = { 0,0 };
        POINT ptRB = { 128, 128 };

        draw.LimitPiont(ptLT, ptRB);
        draw.DrawGrid(ptLT, ptRB, GRID_LINE_COLOR, 8, 8, true);

        POINT ptFocus = m_ptCurBkgTileEdit;
        ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
        ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);

        draw.EndDraw();
    }

    ::InvalidateRect(m_wndBkgTileEdit, NULL, FALSE);
}

void CDesignDialog::UpdateSpVramEdit()
{
    HDC hMemDC = m_wndSpTileEdit.GetMemDC();
    HDC hBackMemDC = m_wndSpTileEdit.GetBackDC();
    HDC hSrcMemDC = m_wndSpTile.GetBackDC();

    StretchBlt(hBackMemDC, 0, 0, 128, 128, hSrcMemDC, m_ptCurSpTile.x * 16, m_ptCurSpTile.y * 16, 16, 16, SRCCOPY);
    BitBlt(hMemDC, 0, 0, 128, 128, hBackMemDC, 0, 0, SRCCOPY);

    CBitmapDraw draw(hMemDC);
    if (draw.BeginDraw())
    {
        POINT ptLT = { 0,0 };
        POINT ptRB = { 128, 128 };

        draw.LimitPiont(ptLT, ptRB);
        draw.DrawGrid(ptLT, ptRB, GRID_LINE_COLOR, 8, 8, true);

        POINT ptFocus = m_ptCurSpTileEdit;
        ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
        ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);

        draw.EndDraw();
    }

    ::InvalidateRect(m_wndSpTileEdit, NULL, FALSE);
}

void CDesignDialog::DrawCharMappingTarget()
{
    HDC hSrcMemDC = m_wndDefaultTile.GetMemDC();
    HDC hSrcBackDC = m_wndDefaultTile.GetBackDC();
    HDC hDestMemDC = m_wndCharMappingDest.GetMemDC();

    StretchBlt(hSrcBackDC, 0, 0, 256, 256, hSrcMemDC, 0, 0, 128, 128, SRCCOPY);

    for (int i = 0; i < 4; i++)
    {
        BitBlt(hDestMemDC, i * 256, 0, 256, 256, hSrcBackDC, 0, (2 + i) * 16, SRCCOPY);
    }

    CBitmapDraw draw(hDestMemDC);
    if (draw.BeginDraw())
    {
        POINT ptLT = { 0,0 };
        POINT ptRB = { 1024, 64 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawGrid(ptLT, ptRB, GRID_LINE_COLOR, 64, 1);

        POINT ptFocus = m_ptCharMapDest;
        ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
        ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);

        draw.EndDraw();
    }

    ::InvalidateRect(m_wndCharMappingDest, NULL, FALSE);
}

void CDesignDialog::DrawCharMappingSource()
{
    HDC hDestMemDC = m_wndCharMappingSrc.GetMemDC();
    HDC hTileMemDC = m_wndTileDraw.GetMemDC();

    std::lock_guard<std::mutex> lock(m_tileMutex);
    for (int i = 0x20; i < 0x60; i++)
    {
        int nIndex = g_CharMapData[i];
        ::StretchBlt(
            hDestMemDC,
            (i - 0x20) * 16, 0, 16, 16,
            hTileMemDC,
            (m_ptCurBkgPalette.x >> 2) * 128 + (nIndex % 0x10) * 8, (nIndex / 0x10) * 8, 8, 8, SRCCOPY);
    }

    CBitmapDraw draw(hDestMemDC);
    if (draw.BeginDraw())
    {
        POINT ptLT = { 0,0 };
        POINT ptRB = { 1024, 64 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawGrid(ptLT, ptRB, GRID_LINE_COLOR, 64, 1);

        POINT ptFocus = m_ptCharMapSrc;
        ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
        ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
        draw.LimitPiont(ptLT, ptRB);
        draw.DrawRect(ptLT, ptRB, FOCUS_COLOR);

        draw.EndDraw();
    }

    ::InvalidateRect(m_wndCharMappingSrc, NULL, FALSE);
}

void CDesignDialog::DrawNameTableElements()
{
    HWND hWnd = m_hWndNameTable;
    HDC hdc = m_hWndNameTable.GetBackDC();

    RECT rect = { 0 };
    ::GetClientRect(hWnd, &rect);

    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            obj.FillRect({ 0, 0 }, { 511, 479 }, GetBkgColor());
            obj.EndDraw();
        }
    }

    // 8 * 4 * 8 * 4
    if (m_bFillColor)
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    BYTE data = g_AttributeData[(i * 8 + j)];
                    DWORD dwLTIndex = (data & 0x03) >> 0;
                    DWORD dwRTIndex = (data & 0x0C) >> 2;
                    DWORD dwLBIndex = (data & 0x30) >> 4;
                    DWORD dwRBIndex = (data & 0xC0) >> 6;

                    DWORD dwLT = g_PaletteColor[g_BkgPalette[dwLTIndex * 4 + 1]];
                    DWORD dwRT = g_PaletteColor[g_BkgPalette[dwRTIndex * 4 + 1]];
                    DWORD dwLB = g_PaletteColor[g_BkgPalette[dwLBIndex * 4 + 1]];
                    DWORD dwRB = g_PaletteColor[g_BkgPalette[dwRBIndex * 4 + 1]];

                    //左上
                    obj.FillGrid({ 0,0 }, { 511, 479 }, dwLT, 16, 15, { j * 2 + 0, i * 2 + 0 });

                    //右上
                    obj.FillGrid({ 0,0 }, { 511, 479 }, dwRT, 16, 15, { j * 2 + 1, i * 2 + 0 });

                    if ((i * 4 + 2) < 30)
                    {
                        //左下
                        obj.FillGrid({ 0,0 }, { 511, 479 }, dwLB, 16, 15, { j * 2 + 0, i * 2 + 1 });

                        //右下
                        obj.FillGrid({ 0,0 }, { 511, 479 }, dwRB, 16, 15, { j * 2 + 1, i * 2 + 1 });
                    }
                }
            }
            obj.EndDraw();
        }
    }

    //绘制后台精灵
    if (m_bShowSpTile)
    {
        DrawSpriteTileList(false, true);
    }

    //绘制自定义Tile
    if (m_bShowBgTile)
    {
        DrawBkgTileList(false);
    }

    //自定义文本
    if (m_bShowText)
    {
        DrawTextList();
    }

    //绘制节目名
    if (m_bShowList)
    {
        DrawItemList();
    }

    //绘制页码
    if (m_bShowPage)
    {
        FillGridText(hWnd, hdc, 512, 480, 32, 30, m_ptPageNumber, m_strPageNumber);
    }

    //绘制前台精灵
    if (m_bShowSpTile)
    {
        DrawSpriteTileList(false, false);
    }

    //绘制标题
    if (!m_strCaption.empty() && m_bShowCaption)
    {
        FillGridText(hWnd, hdc, 512, 480, 32, 30, m_ptCaption, m_strCaption);
    }

    if (m_bShowCursor)
    {
        if (m_bBigCursor)
        {
            //绘制光标列表
            DrawCursorTileList(false);
        }
        else
        {
            //绘制光标
            DrawCursorTile();
        }
    }

    //命名表方格线
    if (m_bDrawLine)
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            obj.DrawGrid({ 0,0 }, { 511, 479 }, GRID_LINE_COLOR, 32, 30, true);
            obj.DrawLine({ 0, 16 }, { 512, 16 }, GRID_PART_LINE_COLOR, true);
            obj.DrawLine({ 256, 16 }, { 256, 464 }, GRID_CENTER_LINE_COLOR);
            obj.DrawLine({ 0, 240 }, { 512, 240 }, GRID_CENTER_LINE_COLOR);
            obj.DrawLine({ 0, 464 }, { 512, 464 }, GRID_PART_LINE_COLOR, true);
            obj.EndDraw();
        }
    }

    //绘制选中区域
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            POINT ptFocus = m_ptCurNameTable;
            POINT ptLT = { ptFocus.x * 64, ptFocus.y * 64 };
            POINT ptRB = { (ptFocus.x + 1) * 64, (ptFocus.y + 1) * 64 };

            obj.LimitPiont(ptLT, ptRB);
            obj.DrawGrid(ptLT, ptRB, FOCUS_COLOR, 1, 1);

            ptFocus = m_ptCurNameTableAttr;
            ptLT = { ptFocus.x * 32, ptFocus.y * 32 };
            ptRB = { (ptFocus.x + 1) * 32, (ptFocus.y + 1) * 32 };

            obj.LimitPiont(ptLT, ptRB);
            obj.DrawGrid(ptLT, ptRB, FOCUS_COLOR, 1, 1);

            ptFocus = m_ptNameTableTile;
            ptLT = { ptFocus.x * 16, ptFocus.y * 16 };
            ptRB = { (ptFocus.x + 1) * 16, (ptFocus.y + 1) * 16 };
            obj.LimitPiont(ptLT, ptRB);
            obj.DrawGrid(ptLT, ptRB, FOCUS_COLOR, 1, 1);

            obj.EndDraw();
        }
    }

    //绘制后台精灵焦点
    DrawSpriteTileList(true, true);

    //绘制自定义Tile焦点
    DrawBkgTileList(true);

    //绘制后台精灵焦点
    DrawSpriteTileList(true, false);

    if (m_bShowCursor)
    {
        if (m_bBigCursor)
        {
            //绘制光标列表
            DrawCursorTileList(true);
        }
        else
        {
            TILE_INFO tile = m_tileCursor;
            //绘制箭头焦点
            if (tile.pt.y > 0)
            {
                if (tile.flag & TILE_FLAG_CHECKED)
                {
                    RECT rtLine = { 0 };
                    rtLine.left = tile.pt.x * 2 - 1;
                    rtLine.top = tile.pt.y * 2 - 1;
                    rtLine.right = tile.pt.x * 2 + 16;
                    rtLine.bottom = tile.pt.y * 2 + 16 - 1;

                    if (m_bBigSprite)
                    {
                        rtLine.bottom = tile.pt.y * 2 + 32 + 0;
                    }

                    CBitmapDraw obj(hdc);
                    if (obj.BeginDraw())
                    {
                        POINT ptLT = { rtLine.left, rtLine.top };
                        POINT ptRB = { rtLine.right, rtLine.bottom };
                        obj.DrawRect(ptLT, ptRB, FOCUS_CURSOR_COLOR);

                        obj.EndDraw();
                    }
                }
            }
        }
    }

    //绘制选中区域
    if (m_bSelecting)
    {
        CBitmapDraw obj(hdc);
        if (obj.BeginDraw())
        {
            obj.DrawRect({ m_ptNtSelectBegin.x, m_ptNtSelectBegin.y }, { m_ptNtSelectEnd.x, m_ptNtSelectEnd.y }, SELECTING_LINE_COLOR);
            obj.EndDraw();
        }
    }

    DrawCharMappingSource();
}

void CDesignDialog::DrawNameTableSync()
{
    DrawNameTableElements();
    //绘制完成拷贝DC
    {
        HDC hBackDC = m_hWndNameTable.GetBackDC();
        HDC hMemDC = m_hWndNameTable.GetMemDC();
        std::lock_guard<std::mutex>lock(m_hWndNameTable.Mutex());
        ::BitBlt(hMemDC, 0, 0, m_hWndNameTable.GetDCWidth(), m_hWndNameTable.GetDCHeight(), hBackDC, 0, 0, SRCCOPY);
    }

    ::InvalidateRect(m_hWndNameTable, NULL, FALSE);
}

void CDesignDialog::DrawNameTable()
{
    if (0)
    {
        static clock_t lastClock = 0;
        clock_t curClock = clock();

        if (curClock - lastClock < 20)
        {
            return;
        }

        lastClock = curClock;
        DrawNameTableSync();
    }
    else
    {
        //通知绘制
        m_cvDraw.notify_one();
    }
}

void CDesignDialog::DrawTask()
{
    std::mutex tmpMutex;
    std::unique_lock<std::mutex> lock(tmpMutex);

    while (!m_bQuit)
    {
        m_cvDraw.wait(lock);
        if (m_bQuit)
        {
            break;
        }

        DrawNameTableElements();
        //绘制完成拷贝DC
        {
            std::lock_guard<std::mutex> drawLock(m_hWndNameTable.Mutex());
            HDC hBackDC = m_hWndNameTable.GetBackDC();
            HDC hMemDC = m_hWndNameTable.GetMemDC();

            ::BitBlt(hMemDC, 0, 0, m_hWndNameTable.GetDCWidth(), m_hWndNameTable.GetDCHeight(), hBackDC, 0, 0, SRCCOPY);
        }
        ::InvalidateRect(m_hWndNameTable, NULL, FALSE);
    }
}