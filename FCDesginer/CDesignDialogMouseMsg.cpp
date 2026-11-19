#include "CDesignDialog.h"
#include "resource.h"
#include <strsafe.h>
#include <windowsx.h>

LRESULT CDesignDialog::OnMButtonDown(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (auto& item : m_CursorTileList)
    {
        item.flag |= TILE_FLAG_CHECKED;
    }

    m_tileCursor.flag |= TILE_FLAG_CHECKED;

    UpdateHistoryData(true);

    DrawNameTable();

    return TRUE;
}

LRESULT CDesignDialog::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);
    POINT ptOffset = { 0 };

    ::SetCapture(m_hWnd);

    RecordHistoryData(true);

    m_ClickFrom = eClickFromType::eFromNone;

    {
        POINT ptCharMapping = { xPos , yPos };
        POINT ptBkgTile = { xPos , yPos };
        if (!(GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, ptCharMapping, ptOffset) ||
            GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkgTile, ptOffset)))
        {
            m_bQuickCharMappingSrc = false;
            ::CheckDlgButton(m_hWnd, IDC_CHECK_CHAR_MAP_FAST, BST_UNCHECKED);
        }
    }

    {
        POINT ptCharMapping = { xPos , yPos };
        POINT ptBkgTile = { xPos , yPos };
        POINT ptSpTile = { xPos , yPos };
        if (!(GetGridClick(m_hWnd, m_wndCharMappingDest, 1024, 16, 64, 1, ptCharMapping, ptOffset) ||
            GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkgTile, ptOffset)))
        {
            m_bQuickBkgChrDest = false;
            ::CheckDlgButton(m_hWnd, IDC_CHECK_BKG_CHR_FAST, BST_UNCHECKED);
        }
    }

    {
        POINT ptCharMapping = { xPos , yPos };
        POINT ptBkgTile = { xPos , yPos };
        POINT ptSpTile = { xPos , yPos };
        if (!(GetGridClick(m_hWnd, m_wndCharMappingDest, 1024, 16, 64, 1, ptCharMapping, ptOffset) ||
            GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptBkgTile, ptOffset)))
        {
            m_bQuickSpChrDest = false;
            ::CheckDlgButton(m_hWnd, IDC_CHECK_SP_CHR_FAST, BST_UNCHECKED);
        }
    }

    if (m_bQuickCharMappingSrc)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_ptCharMapSrc = pt;
            DrawCharMappingSource();
            m_Click = eClickType::eCharMapSrc;

            BYTE data = g_CharMapData[0x20 + pt.x];
            m_ptCurBkgTile = { data & 0x0F,  (data & 0xF0) >> 4 };

            m_SelSpPalette = false;
            DrawBkgVram();
            DrawBkgPalette();
            DrawSpritePalette();
            UpdateBkgVramEdit();
        }
        else
        {
            ClickByFastCharMapping({ xPos , yPos });
        }

        return TRUE;
    }

    if (m_bQuickBkgChrDest)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingDest, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_Click = eClickType::eCharMapDest;
            m_ptCharMapDest = pt;
            ClickByFastBkgChr({ xPos , yPos });
        }
        else
        {
            POINT ptCur = { xPos , yPos };
            POINT ptCurOffset = { 0 };
            if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptCur, ptCurOffset))
            {
                m_ptCurBkgTile = ptCur;
                m_SelSpPalette = false;
                DrawBkgVram();
                UpdateBkgVramEdit();

                DrawBkgPalette();
                DrawSpritePalette();
            }
        }

        return TRUE;
    }

    if (m_bQuickSpChrDest)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingDest, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_Click = eClickType::eCharMapDest;
            m_ptCharMapDest = pt;
            ClickByFastSpChr({ xPos , yPos });
        }
        else
        {
            POINT ptCur = { xPos , yPos };
            POINT ptCurOffset = { 0 };
            if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptCur, ptCurOffset))
            {
                m_ptCurSpTile = ptCur;
                m_SelSpPalette = false;
                DrawSpVram();
                UpdateSpVramEdit();

                DrawBkgPalette();
                DrawSpritePalette();
            }
        }

        return TRUE;
    }

    if (m_bSelectMode)
    {
        POINT ptNt = { xPos , yPos };
        POINT ptBkg = { xPos , yPos };
        POINT ptSp = { xPos , yPos };

        {
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, ptNt, ptOffset))
            {
                m_ptNtSelectBegin = ptNt;
                m_ptNtSelectEnd = m_ptNtSelectBegin;
            }
            else
            {
                POINT ptCur = { xPos , yPos };
                GetCursorPos(&ptCur);
                ScreenToClient(m_hWndNameTable, &ptCur);
                m_ptNtSelectBegin = ptNt;
                m_ptNtSelectEnd = m_ptNtSelectBegin;
            }

            if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 256, 256, ptBkg, ptOffset))
            {
                m_ptBkgTileSelectBegin = ptBkg;
                m_ptBkgTileSelectEnd = m_ptBkgTileSelectBegin;
            }
            else
            {
                POINT ptCur = { xPos , yPos };
                GetCursorPos(&ptCur);
                ScreenToClient(m_wndBkgTile, &ptCur);
                m_ptBkgTileSelectBegin = ptBkg;
                m_ptBkgTileSelectEnd = m_ptBkgTileSelectBegin;
            }

            if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 256, 256, ptSp, ptOffset))
            {
                m_ptSpTileSelectBegin = ptSp;
                m_ptSpTileSelectEnd = m_ptSpTileSelectBegin;
            }
            else
            {
                POINT ptCur = { xPos , yPos };
                GetCursorPos(&ptCur);
                ScreenToClient(m_wndSpTile, &ptCur);
                m_ptSpTileSelectBegin = ptSp;
                m_ptSpTileSelectEnd = m_ptSpTileSelectBegin;
            }
        }

        m_bSelecting = true;

        return TRUE;
    }

    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_ptCharMapSrc = pt;
            DrawCharMappingSource();
            m_Click = eClickType::eCharMapSrc;

            BYTE data = g_CharMapData[0x20 + pt.x];
            m_ptCurBkgTile = { data & 0x0F,  (data & 0xF0) >> 4 };

            m_SelSpPalette = false;
            DrawBkgVram();
            DrawBkgPalette();
            DrawSpritePalette();
            UpdateBkgVramEdit();

            return TRUE;
        }
    }

    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingDest, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_ptCharMapDest = pt;
            DrawCharMappingTarget();
            m_Click = eClickType::eCharMapDest;

            m_SelSpPalette = false;
            DrawBkgVram();
            DrawBkgPalette();
            DrawSpritePalette();
            UpdateBkgVramEdit();

            return TRUE;
        }
    }

    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndBkgTileEdit, 128, 128, 8, 8, pt, ptOffset))
        {
            m_Click = eClickType::eBkgTileEdit;
            ClickBkgChrEdit({ xPos, yPos });
            m_SelSpPalette = false;
            DrawBkgPalette();
            DrawSpritePalette();
            return TRUE;
        }
    }

    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndSpTileEdit, 128, 128, 8, 8, pt, ptOffset))
        {

            m_Click = eClickType::eSpriteTileEdit;
            ClickSpriteChrEdit({ xPos, yPos });
            m_SelSpPalette = true;
            DrawBkgPalette();
            DrawSpritePalette();

            return TRUE;
        }
    }

    if (m_bShowCursor)
    {
        //箭头列表
        if (m_bBigCursor)
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
            {
                for (auto& item : m_CursorTileList)
                {
                    auto firstArrow = m_CursorTileList.front();
                    LONG lLeft = firstArrow.pt.x * 2;
                    LONG lTop = firstArrow.pt.y * 2;

                    RECT rtTile = { 0 };
                    rtTile.left = item.pt.x * 2;
                    rtTile.top = item.pt.y * 2;
                    rtTile.right = rtTile.left + 16;
                    rtTile.bottom = rtTile.top + 16;

                    if (m_bBigSprite)
                    {
                        rtTile.bottom = rtTile.top + 32;
                    }

                    if (PtInRect(&rtTile, pt))
                    {
                        m_tileCursor.flag = m_tileCursor.flag & ~TILE_FLAG_CHECKED;
                        ClearBkgTileListCheck();
                        ClearSpriteTileListCheck();

                        m_Click = eClickType::eCursorList;

                        m_ptCurSpTile = { item.data & 0x0F,  (item.data & 0xF0) >> 4 };
                        m_ptCurSpPalette.x = (item.color << 2) + 1;
                        m_SelSpPalette = true;
                        m_ptClickOffset.x = pt.x - lLeft;
                        m_ptClickOffset.y = pt.y - lTop;

                        DrawSpVram();
                        UpdateSpVramEdit();
                        DrawBkgPalette();
                        DrawSpritePalette();
                        DrawNameTable();

                        return TRUE;
                    }
                }

                {
                    m_tileCursor.flag = m_tileCursor.flag & ~TILE_FLAG_CHECKED;
                    DrawNameTable();
                }
            }
        }
        else
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
            {
                RECT rtTile = { 0 };
                rtTile.left = m_tileCursor.pt.x * 2;
                rtTile.top = m_tileCursor.pt.y * 2;
                rtTile.right = rtTile.left + 16;
                rtTile.bottom = rtTile.top + 16;

                if (m_bBigSprite)
                {
                    rtTile.bottom = rtTile.top + 32;
                }

                if (PtInRect(&rtTile, pt))
                {
                    ClearBkgTileListCheck();
                    ClearSpriteTileListCheck();
                    ClearCursorTileListCheck();

                    m_ptCurSpTile = { m_tileCursor.data & 0x0F,  (m_tileCursor.data & 0xF0) >> 4 };
                    m_ptCurSpPalette.x = (m_tileCursor.color << 2) + 1;
                    m_SelSpPalette = true;

                    DrawSpVram();
                    UpdateSpVramEdit();
                    DrawBkgPalette();
                    DrawSpritePalette();
                    DrawNameTable();

                    m_Click = eClickType::eCursor;

                    m_ptClickOffset.x = pt.x - rtTile.left;
                    m_ptClickOffset.y = pt.y - rtTile.top;

                    return TRUE;
                }
                else
                {
                    m_tileCursor.flag = m_tileCursor.flag & ~TILE_FLAG_CHECKED;
                    DrawNameTable();
                }
            }
        }
    }

    if (m_bQuickBkgList && !m_bSelectMode)
    {
        POINT ptBkg = { xPos , yPos };
        POINT ptSp = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkg, ptOffset))
        {
            m_ptCurBkgTile = ptBkg;
            ClickByFastBkgList({ xPos , yPos });
            return TRUE;
        }
        else if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptSp, ptOffset))
        {
            m_ptCurSpTile = ptSp;
            ClickByFastSpList({ xPos , yPos });
            return TRUE;
        }
        else
        {
            POINT pt = { xPos , yPos };
            POINT ptCurOffset = { 0 };
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, pt, ptCurOffset))
            {
                m_Click = eClickType::eBkgListQuick;
                m_ptNameTableTile = pt;
                m_ptCurNameTable = { pt.x / 4, pt.y / 4 };
                m_ptCurNameTableAttr = { pt.x / 2, pt.y / 2 };

                UpdateAttribute();

                DrawBkgVram();
                DrawAttribute();
                DrawNameTable();
                return TRUE;
            }
        }
    }

    //精灵组
    if (m_bDragText && !m_SpTileList.empty() && m_bShowSpTile)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
        {
            bool bFind = false;
            for (int i = (int)m_SpTileList.size() - 1; i >= 0; i--)
            {
                const auto& item = m_SpTileList[i];

                RECT rtTile = { 0 };
                rtTile.left = item.pt.x * 2;
                rtTile.top = (item.pt.y + 1) * 2;
                rtTile.right = rtTile.left + 16;
                rtTile.bottom = rtTile.top + 16;

                int nHeight = 18;

                if (m_bBigSprite)
                {
                    nHeight = 36;
                }

                if (m_bBigSprite)
                {
                    rtTile.bottom = rtTile.top + 32;
                }

                if (!PtInRect(&rtTile, pt))
                {
                    continue;
                }

                m_ptClickOffset.x = pt.x - rtTile.left;
                m_ptClickOffset.y = pt.y - rtTile.top;

                m_ptClickOffset.x = (m_ptClickOffset.x >> 1) << 1;
                m_ptClickOffset.y = (m_ptClickOffset.y >> 1) << 1;

                m_dragTile = item;

                INT nX = item.data & 0x0F;
                INT nY = (item.data & 0xF0) >> 4;

                RECT rtFill = { 0 };
                GetClientRect(m_wndDragTile, &rtFill);

                FillRect(m_wndDragTile.GetMemDC(), &rtFill, GetStockBrush(BLACK_BRUSH));

                if (m_bBigSprite)
                {
                    DWORD dwIndex = 256;
                    if (item.data & TILE_FLAG_CHECKED)
                    {
                        dwIndex = 128;
                    }

                    nX &= ~TILE_FLAG_CHECKED;

                    if (m_dragTile.mode.bVerticalFlip)
                    {
                        StretchBlt(
                            m_wndDragTile.GetMemDC(),
                            1, 2 + 16, 16, 16,
                            m_wndTileDraw.GetMemDC(),
                            m_dragTile.color * 128 + nX * 8 + (m_dragTile.mode.bHorizontalFlip ? 7 : 0),
                            dwIndex + nY * 8 + (m_dragTile.mode.bVerticalFlip ? 7 : 0),
                            m_dragTile.mode.bHorizontalFlip ? -8 : 8,
                            -8,
                            SRCCOPY);

                        StretchBlt(
                            m_wndDragTile.GetMemDC(),
                            1, 2, 16, 16,
                            m_wndTileDraw.GetMemDC(),
                            m_dragTile.color * 128 + (nX + 1) * 8 + (m_dragTile.mode.bHorizontalFlip ? 7 : 0),
                            dwIndex + nY * 8 + (m_dragTile.mode.bVerticalFlip ? 7 : 0),
                            m_dragTile.mode.bHorizontalFlip ? -8 : 8,
                            -8,
                            SRCCOPY);
                    }
                    else
                    {
                        StretchBlt(
                            m_wndDragTile.GetMemDC(),
                            1, 2, 16, 16,
                            m_wndTileDraw.GetMemDC(),
                            m_dragTile.color * 128 + nX * 8 + (m_dragTile.mode.bHorizontalFlip ? 7 : 0),
                            dwIndex + nY * 8 + (m_dragTile.mode.bVerticalFlip ? 7 : 0),
                            m_dragTile.mode.bHorizontalFlip ? -8 : 8,
                            8,
                            SRCCOPY);

                        StretchBlt(
                            m_wndDragTile.GetMemDC(),
                            1, 2 + 16, 16, 16,
                            m_wndTileDraw.GetMemDC(),
                            m_dragTile.color * 128 + (nX + 1) * 8 + (m_dragTile.mode.bHorizontalFlip ? 7 : 0),
                            dwIndex + nY * 8 + (m_dragTile.mode.bVerticalFlip ? 7 : 0),
                            m_dragTile.mode.bHorizontalFlip ? -8 : 8,
                            8,
                            SRCCOPY);
                    }


                }
                else
                {
                    StretchBlt(
                        m_wndDragTile.GetMemDC(),
                        1, 1, 16, 16,
                        m_wndTileDraw.GetMemDC(),
                        m_dragTile.color * 128 + nX * 8 + (m_dragTile.mode.bHorizontalFlip ? 7 : 0),
                        128 + nY * 8 + (m_dragTile.mode.bVerticalFlip ? 7 : 0),
                        m_dragTile.mode.bHorizontalFlip ? -8 : 8,
                        m_dragTile.mode.bVerticalFlip ? -8 : 8,
                        SRCCOPY);
                }

                if (m_dragTile.flag & TILE_FLAG_CHECKED)
                {
                    HDC hdc = m_wndDragTile.GetMemDC();
                    RECT rtLine = { 0 };
                    rtLine.left = 1;
                    rtLine.top = 0;

                    if (m_bBigSprite)
                    {
                        rtLine.left = 1;
                        rtLine.top = 1;
                        rtLine.right = rtLine.left + 16;
                        rtLine.bottom = rtLine.top + 32;
                    }
                    else
                    {
                        rtLine.left = 1;
                        rtLine.top = 0;
                        rtLine.right = rtLine.left + 16;
                        rtLine.bottom = rtLine.top + 16 + 1;
                    }

                    HPEN hPen = CreatePen(PS_SOLID, 1, FOCUS_SP_COLOR);
                    HPEN hOldPen = (HPEN)::SelectObject(hdc, hPen);

                    ::MoveToEx(hdc, rtLine.left, rtLine.top, nullptr);
                    ::LineTo(hdc, rtLine.right, rtLine.top);
                    ::LineTo(hdc, rtLine.right, rtLine.bottom);
                    ::LineTo(hdc, rtLine.left, rtLine.bottom);
                    ::LineTo(hdc, rtLine.left, rtLine.top);

                    ::SelectObject(hdc, hOldPen);
                    ::DeleteObject(hPen);
                }

                ::InvalidateRect(m_wndDragTile, nullptr, FALSE);

                m_Click = eClickType::eSpriteTile;
                m_ClickFrom = eClickFromType::eFromNameTable;

                m_ptCurSpTile = { m_dragTile.data & 0x0F,  (m_dragTile.data & 0xF0) >> 4 };
                m_ptCurSpPalette.x = { m_dragTile.color << 2 };

                POINT ptWndOffset = GetClientOffset(GetDesktopWindow(), m_hWndNameTable.GetWndHandle());

                m_ptDragOffset.x = xPos - (ptWndOffset.x + pt.x) + m_ptClickOffset.x;
                m_ptDragOffset.y = yPos - (ptWndOffset.y + pt.y) + m_ptClickOffset.y;
                ::MoveWindow(m_wndDragTile, xPos - m_ptDragOffset.x - 1, yPos - m_ptDragOffset.y, 18, nHeight, FALSE);

                m_SelSpPalette = true;
                DrawSpVram();
                DrawBkgPalette();
                DrawSpritePalette();
                UpdateSpVramEdit();

                bFind = true;
                break;
            }

            if (bFind)
            {
                if (!(GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
                {
                    auto itFind = std::find(m_SpTileList.begin(), m_SpTileList.end(), m_dragTile);
                    if (itFind != m_SpTileList.end())
                    {
                        m_SpTileList.erase(itFind);
                    }
                }

                ClearBkgTileListCheck();
                ClearCursorTileListCheck();

                ::ShowWindow(m_wndDragTile, SW_SHOWNOACTIVATE);
                DrawNameTable();

                return TRUE;
            }
        }
    }

    if (m_bDragText)
    {
        //菜单标题
        if (!m_strCaption.empty() && m_bShowCaption)
        {
            POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);
            RECT rt = { 0 };

            int nSizeX = 512 / 32;
            int nSizeY = 480 / 30;

            RECT rtCheck = { 0 };
            rtCheck.left += m_ptCaption.x * nSizeX + ptCurOffset.x;
            rtCheck.top += m_ptCaption.y * nSizeY + ptCurOffset.y;
            rtCheck.right = rtCheck.left + (LONG)m_strCaption.size() * nSizeX;
            rtCheck.bottom = rtCheck.top + 1 * nSizeY;

            if (::PtInRect(&rtCheck, { xPos,  yPos }))
            {
                m_ptDragOffset.x = xPos - rtCheck.left - ((xPos - rtCheck.left) % 16);
                m_ptDragOffset.y = yPos - rtCheck.top - ((yPos - rtCheck.top) % 16);

                m_Click = eClickType::eCaption;
                return TRUE;
            }
        }

        //页码
        if (m_bShowPage)
        {
            POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);
            RECT rt = { 0 };

            int nSizeX = 512 / 32;
            int nSizeY = 480 / 30;

            RECT rtCheck = { 0 };
            rtCheck.left += m_ptPageNumber.x * nSizeX + ptCurOffset.x;
            rtCheck.top += m_ptPageNumber.y * nSizeY + ptCurOffset.y;
            rtCheck.right = rtCheck.left + (LONG)m_strPageNumber.size() * nSizeX;
            rtCheck.bottom = rtCheck.top + 1 * nSizeY;

            if (::PtInRect(&rtCheck, { xPos,  yPos }))
            {
                m_ptDragOffset.x = xPos - rtCheck.left - ((xPos - rtCheck.left) % 16);
                m_ptDragOffset.y = yPos - rtCheck.top - ((yPos - rtCheck.top) % 16);

                m_Click = eClickType::ePageNumber;
                return TRUE;
            }
        }

        //节目名
        if (!m_strItemList.empty() && m_bShowList)
        {
            POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);
            CalcItemRect();

            POINT ptCheck = { xPos - ptCurOffset.x, yPos - ptCurOffset.y };
            for (int i = 0; i < (int)m_strItemList.size(); i++)
            {
                if (::PtInRect(&m_rtItems[i], { ptCheck.x,  ptCheck.y }))
                {
                    m_ptDragOffset.x = ptCheck.x - (m_rtItems[0].left) - ((ptCheck.x - m_rtItems[0].left) % 16);
                    m_ptDragOffset.y = ptCheck.y - (m_rtItems[0].top) - ((ptCheck.y - m_rtItems[0].top) % 16);

                    m_Click = eClickType::eItemText;
                    return TRUE;
                }
            }
        }

        //自定义文本
        if (!m_CustomTextList.empty() && m_bShowText)
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 16, 16, pt, ptOffset) && !m_CustomTextList.empty())
            {
                POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);

                int nSizeX = 512 / 32;
                int nSizeY = 480 / 30;

                bool bFind = false;

                for (int i = (int)m_CustomTextList.size() - 1; i >= 0; i--)
                {
                    const auto& item = m_CustomTextList[i];
                    RECT rtCheck = { 0 };
                    rtCheck.left = item.pt.x * nSizeX + ptCurOffset.x;
                    rtCheck.top = item.pt.y * nSizeY + ptCurOffset.y;
                    rtCheck.right = rtCheck.left + (LONG)item.strText.size() * nSizeX;
                    rtCheck.bottom = rtCheck.top + 1 * nSizeY;

                    if (::PtInRect(&rtCheck, { xPos,  yPos }))
                    {
                        m_ptDragOffset.x = xPos - rtCheck.left - ((xPos - rtCheck.left) % 16);
                        m_ptDragOffset.y = yPos - rtCheck.top - ((yPos - rtCheck.top) % 16);
                        bFind = true;
                        m_CustomText = item;
                        m_Click = eClickType::eCustomText;
                        break;
                    }
                }

                if (bFind)
                {
                    if (!(GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
                    {
                        auto itFind = std::find(m_CustomTextList.begin(), m_CustomTextList.end(), m_CustomText);
                        if (itFind != m_CustomTextList.end())
                        {
                            m_CustomTextList.erase(itFind);
                        }
                    }
                    else
                    {
                        m_CustomTextCount++;
                        m_CustomText.id++;
                    }

                    return TRUE;
                }
            }
        }
    }

    //背景组
    if (m_bDragText && !m_BkgTileList.empty() && m_bShowBgTile)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, pt, ptOffset))
        {
            bool bFind = false;

            for (int i = (int)m_BkgTileList.size() - 1; i >= 0; i--)
            {
                const auto& item = m_BkgTileList[i];
                if (item.pt != pt)
                {
                    continue;
                }

                m_dragTile = item;

                INT nX = item.data & 0x0F;
                INT nY = (item.data & 0xF0) >> 4;

                RECT rtFill = { 0 };
                GetClientRect(m_wndDragTile, &rtFill);
                ::FillRect(m_wndDragTile.GetMemDC(), &rtFill, GetStockBrush(BLACK_BRUSH));

                DWORD dwIndex = GetNameTableColorIndex(pt);
                ::StretchBlt(
                    m_wndDragTile.GetMemDC(),
                    1, 1, 16, 16,
                    m_wndTileDraw.GetMemDC(),
                    dwIndex * 128 + nX * 8, nY * 8, 8, 8, SRCCOPY);

                if (m_dragTile.flag & TILE_FLAG_CHECKED)
                {
                    HDC hdc = m_wndDragTile.GetMemDC();
                    RECT rtLine = { 0 };
                    rtLine.left = 0;
                    rtLine.top = 0;
                    rtLine.right = 17;
                    rtLine.bottom = 17;

                    HPEN hPen = CreatePen(PS_SOLID, 1, FOCUS_COLOR);
                    HPEN hOldPen = (HPEN)::SelectObject(hdc, hPen);

                    ::MoveToEx(hdc, rtLine.left, rtLine.top, nullptr);
                    ::LineTo(hdc, rtLine.right, rtLine.top);
                    ::LineTo(hdc, rtLine.right, rtLine.bottom);
                    ::LineTo(hdc, rtLine.left, rtLine.bottom);
                    ::LineTo(hdc, rtLine.left, rtLine.top);

                    ::SelectObject(hdc, hOldPen);
                    ::DeleteObject(hPen);
                }
                ::UpdateWindow(m_wndDragTile);

                m_Click = eClickType::eBkgTile;
                m_ClickFrom = eClickFromType::eFromNameTable;

                m_ptCurBkgTile = { m_dragTile.data & 0x0F,  (m_dragTile.data & 0xF0) >> 4 };

                m_SelSpPalette = false;
                DrawBkgVram();
                DrawBkgPalette();
                DrawSpritePalette();
                UpdateBkgVramEdit();

                POINT ptCurOffset = GetClientOffset(GetDesktopWindow(), m_hWndNameTable.GetWndHandle());

                m_ptDragOffset.x = xPos - (ptCurOffset.x + pt.x * 16);
                m_ptDragOffset.y = yPos - (ptCurOffset.y + pt.y * 16);
                ::MoveWindow(m_wndDragTile, xPos - m_ptDragOffset.x - 1, yPos - m_ptDragOffset.y - 1, 18, 18, FALSE);
                bFind = true;
                break;
            }

            if (bFind)
            {
                if (!(GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
                {
                    auto itFind = std::find(m_BkgTileList.begin(), m_BkgTileList.end(), m_dragTile);
                    if (itFind != m_BkgTileList.end())
                    {
                        m_BkgTileList.erase(itFind);
                    }
                }

                ClearSpriteTileListCheck();
                ClearCursorTileListCheck();

                ::ShowWindow(m_wndDragTile, SW_SHOWNOACTIVATE);
                DrawNameTable();

                return TRUE;
            }
        }
    }

    //背景图块点击
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, pt, ptOffset))
        {
            m_dragTile.pt = pt;
            m_dragTile.data = (BYTE)((pt.y << 4) | pt.x);
            m_dragTile.color = (BYTE)(m_ptCurBkgPalette.x / 4);

            StretchBlt(
                m_wndDragTile.GetMemDC(),
                1, 1, 16, 16,
                m_wndBkgTile.GetBackDC(),
                pt.x * 16, pt.y * 16, 16, 16, SRCCOPY);

            m_Click = eClickType::eBkgTile;
            m_ClickFrom = eClickFromType::eFromBgkTile;

            POINT ptCurOffset = GetClientOffset(GetDesktopWindow(), m_wndBkgTile);

            m_ptDragOffset.x = xPos - (ptCurOffset.x + pt.x * 16);
            m_ptDragOffset.y = yPos - (ptCurOffset.y + pt.y * 16);
            ::InvalidateRect(m_wndDragTile, NULL, FALSE);
            ::MoveWindow(m_wndDragTile, xPos - m_ptDragOffset.x, yPos - m_ptDragOffset.y, 16, 16, FALSE);
            ::ShowWindow(m_wndDragTile, SW_SHOWNOACTIVATE);

            m_ptCurBkgTile = pt;
            DrawBkgVram();
            UpdateBkgVramEdit();

            m_SelSpPalette = false;
            DrawBkgPalette();
            DrawSpritePalette();

            return TRUE;
        }
    }

    //精灵图块点击
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, pt, ptOffset))
        {
            m_dragTile.pt = pt;
            m_dragTile.data = (BYTE)((pt.y << 4) | pt.x);
            m_dragTile.color = (BYTE)(m_ptCurSpPalette.x / 4);
            m_dragTile.mode = 0;

            if (m_bBigSprite)
            {
                m_dragTile.data |= TILE_FLAG_CHECKED;
            }

            m_ptClickOffset = ptOffset;

            ::CheckDlgButton(m_hWnd, IDC_CHECK_SPRITE_V, m_dragTile.mode.bVerticalFlip ? BST_CHECKED : BST_UNCHECKED);
            ::CheckDlgButton(m_hWnd, IDC_CHECK_SPRITE_H, m_dragTile.mode.bHorizontalFlip ? BST_CHECKED : BST_UNCHECKED);
            ::CheckDlgButton(m_hWnd, IDC_CHECK_SPRITE_BACK, m_dragTile.mode.bBackgroundDisplay ? BST_CHECKED : BST_UNCHECKED);

            int nHeight = 18;

            if (m_bBigSprite) nHeight = 36;
            if (m_bBigSprite)
            {
                BYTE TileID = m_dragTile.data;
                if (TileID & TILE_FLAG_CHECKED)
                {
                    //hBackMemDC = m_wndBkgTile.GetBackMemDC();
                }
                else
                {
                    //hBackMemDC = m_wndSpriteTile.GetBackMemDC();
                }
                LONG nIndex = pt.x & ~TILE_FLAG_CHECKED;

                StretchBlt(
                    m_wndDragTile.GetMemDC(),
                    1, 1, 16, 16,
                    m_wndSpTile.GetBackDC(),
                    nIndex * 16, pt.y * 16, 16, 16, SRCCOPY);

                StretchBlt(
                    m_wndDragTile.GetMemDC(),
                    1, 17, 16, 16,
                    m_wndSpTile.GetBackDC(),
                    (nIndex + 1) * 16, pt.y * 16, 16, 16, SRCCOPY);
            }
            else
            {
                StretchBlt(
                    m_wndDragTile.GetMemDC(),
                    1, 1, 16, 16,
                    m_wndSpTile.GetBackDC(),
                    pt.x * 16, pt.y * 16, 16, 16, SRCCOPY);
            }

            m_Click = eClickType::eSpriteTile;
            m_ClickFrom = eClickFromType::eFromSpriteTile;

            POINT ptWndOffset = GetClientOffset(GetDesktopWindow(), m_wndSpTile);

            m_ptDragOffset.x = xPos - (ptWndOffset.x + pt.x * 16);
            m_ptDragOffset.y = yPos - (ptWndOffset.y + pt.y * 16);

            ::InvalidateRect(m_wndDragTile, NULL, FALSE);
            ::MoveWindow(m_wndDragTile, xPos - m_ptDragOffset.x - 1, yPos - m_ptDragOffset.y - 1, 18, nHeight, FALSE);
            ::ShowWindow(m_wndDragTile, SW_SHOWNOACTIVATE);

            m_ptCurSpTile = pt;
            DrawSpVram();
            UpdateSpVramEdit();

            m_SelSpPalette = true;
            DrawBkgPalette();
            DrawSpritePalette();
            return TRUE;
        }
    }

    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, pt, ptOffset))
        {
            m_ptNameTableTile = pt;
        }
    }

    //检查命名表点击
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 16, 15, pt, ptOffset))
        {
            ClearBkgTileListCheck();
            ClearSpriteTileListCheck();
            ClearCursorTileListCheck();
            m_tileCursor.flag = 0;

            m_Click = eClickType::eNameTable;
            return TRUE;
        }
    }

    //检查属性表点击
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndAttribute, 128, 128, 2, 2, pt, ptOffset))
        {
            m_Click = eClickType::eAttribute;
            return TRUE;
        }
    }

    //检查背景调色板点击
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndBkgPalette, 256, 16, 16, 1, pt, ptOffset))
        {
            m_Click = eClickType::eBkgPalette;
            return TRUE;
        }
    }

    //精灵调色板
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndSpPalette, 256, 16, 16, 1, pt, ptOffset))
        {
            m_Click = eClickType::eSpritePalette;
            return TRUE;
        }
    }

    //检查调色板点击
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_hWndPalette, 256, 64, 16, 4, pt, ptOffset))
        {
            m_Click = eClickType::ePalette;
            return TRUE;
        }
    }

    m_Click = eClickType::eNone;
    return TRUE;
}

LRESULT CDesignDialog::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    ::ReleaseCapture();
    if (m_bSelectMode)
    {
        m_bSelecting = false;
        UpdateHistoryData(true);
        DrawNameTable();
        DrawBkgVram();
        DrawSpVram();

        return TRUE;
    }

    POINT ptOffset = { 0 };
    if (eClickType::eNone == m_Click)
    {
        CheckSpriteAttributeButton();
        UpdateHistoryData(true);
        return TRUE;
    }

    if (eClickType::eBkgTile == m_Click)
    {
        RECT rtNameTable = { 0 };
        POINT ptNt = { xPos, yPos };
        POINT ptBkg = { xPos, yPos };
        POINT ptSprite = { xPos, yPos };
        POINT ptChar = { xPos, yPos };

        if (GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, ptChar, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_wndCharMappingSrc, &rtWnd);
            g_CharMapData[0x20 + ptChar.x] = m_dragTile.data;
            DrawCharMappingSource();
        }

        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset))
        {
            DWORD dwIndex = GetNameTableColorIndex(ptNt);
            m_dragTile.id = (DWORD)m_BkgTileIdCount++;
            m_dragTile.pt = ptNt;
            m_dragTile.color = (BYTE)dwIndex;

            m_BkgTileList.push_back(m_dragTile);
            m_dragTile.id = (DWORD)(-1);
            m_dragTile.flag = 0;
        }
        else
        {
            auto itFind = std::find(m_BkgTileList.begin(), m_BkgTileList.end(), m_dragTile);
            if (itFind != m_BkgTileList.end())
            {
                m_BkgTileList.erase(itFind);
            }
            m_dragTile.id = (DWORD)(-1);
            m_dragTile.flag = 0;
        }

        if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkg, ptOffset))
        {
            LPBYTE lpAddrSrc = &g_ChrRam[m_dragTile.data * 0x10];
            LPBYTE lpAddrDest = &g_ChrRam[((ptBkg.y << 4) | ptBkg.x) * 0x10];
            uint8_t srcId = m_dragTile.data;
            uint8_t destId = uint8_t((ptBkg.y << 4) | ptBkg.x);

            BYTE dataSrc[0x10] = { 0 };
            BYTE dataEmpty[0x10] = { 0 };

            //备份源数据
            memcpy_s(dataSrc, 0x10, lpAddrSrc, 0x10);

            //清除源数据
            if (eClickFromType::eFromBgkTile == m_ClickFrom)
            {
                memcpy_s(lpAddrSrc, 0x10, dataEmpty, 0x10);

                // 替换精灵组
                for (auto& item : m_BkgTileList)
                {
                    if (item.data == srcId)
                    {
                        item.data = destId;
                    }
                }

                // 替换字符映射
                for (auto& item : g_CharMapData)
                {
                    if (item == srcId)
                    {
                        item = destId;
                    }
                }
            }

            //拷贝到目的数据
            memcpy_s(lpAddrDest, 0x10, dataSrc, 0x10);

            //拷贝源数据
            if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
            {
                memcpy_s(lpAddrSrc, 0x10, dataSrc, 0x10);
            }

            m_ptCurBkgTile = ptBkg;
            DrawTile(0, 4, 0, 0);
            DrawBkgVram();
            UpdateBkgVramEdit();
            DrawNameTable();
        }

        if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptSprite, ptOffset))
        {
            LPBYTE lpAddrSrc = &g_ChrRam[m_dragTile.data * 0x10];
            LPBYTE lpAddrDest = &g_ChrRam[0x1000 + ((ptSprite.y << 4) | ptSprite.x) * 0x10];

            memcpy_s(lpAddrDest, 0x10, lpAddrSrc, 0x10);

            m_ptCurSpTile = ptSprite;
            DrawTile(0, 0, 0, 4);
            DrawSpVram();
            UpdateSpVramEdit();
            DrawNameTable();
        }

        DrawNameTable();
        ::UpdateWindow(m_hWndNameTable);
        ::ShowWindow(m_wndDragTile, SW_HIDE);
    }

    if (eClickType::eSpriteTile == m_Click)
    {
        RECT rtNameTable = { 0 };
        POINT pt = { xPos, yPos };
        POINT ptBkg = { xPos, yPos };
        POINT ptSprite = { xPos, yPos };

        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
        {
            m_dragTile.id = (DWORD)m_SpTileIdCount++;

            if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
            {
                POINT ptCur = { xPos - m_ptClickOffset.x, yPos - m_ptClickOffset.y };
                GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptCur, ptOffset);
                ptCur = { xPos - m_ptClickOffset.x + ptOffset.x, yPos - m_ptClickOffset.y + ptOffset.y };
                GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptCur, ptOffset);

                m_dragTile.pt.x = ptCur.x * 8;
                m_dragTile.pt.y = ptCur.y * 8;

                if (0 == m_dragTile.pt.y)
                {
                    m_dragTile.pt.y = 1;
                }
            }
            else
            {
                m_dragTile.pt = pt;
                m_dragTile.pt.x -= m_ptClickOffset.x;
                m_dragTile.pt.y -= m_ptClickOffset.y;
                m_dragTile.pt.x = m_dragTile.pt.x / 2;
                m_dragTile.pt.y = m_dragTile.pt.y / 2;

                if (0 == m_dragTile.pt.y)
                {
                    m_dragTile.pt.y = 1;
                }
            }

            m_ptClickOffset = { 0,0 };

            //更新模式
            {
                CheckDlgButton(m_hWnd, IDC_CHECK_SPRITE_V, m_dragTile.mode.bVerticalFlip ? BST_CHECKED : BST_UNCHECKED);
                CheckDlgButton(m_hWnd, IDC_CHECK_SPRITE_H, m_dragTile.mode.bHorizontalFlip ? BST_CHECKED : BST_UNCHECKED);
                CheckDlgButton(m_hWnd, IDC_CHECK_SPRITE_BACK, m_dragTile.mode.bBackgroundDisplay ? BST_CHECKED : BST_UNCHECKED);
            }

            if (m_SpTileList.size() < SPRITE_MAX_SIZE)
            {
                if ((m_dragTile.pt.y >= 0 && m_dragTile.pt.y < 240) && (m_dragTile.pt.x >= 0 && m_dragTile.pt.x < 256))
                {
                    m_dragTile.pt.y -= 1;
                    m_SpTileList.push_back(m_dragTile);
                }
            }

            m_dragTile.id = (DWORD)(-1);
            m_dragTile.flag = 0;
        }
        else
        {
            auto itFind = std::find(m_SpTileList.begin(), m_SpTileList.end(), m_dragTile);
            if (itFind != m_SpTileList.end())
            {
                m_SpTileList.erase(itFind);
            }
            m_dragTile.id = (DWORD)(-1);
            m_dragTile.flag = 0;
        }

        if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptSprite, ptOffset))
        {
            BYTE TileID = m_dragTile.data;
            DWORD dwSize = 0x10;
            DWORD dwOraMask = 0;
            if (m_bBigSprite)
            {
                TileID &= ~TILE_FLAG_CHECKED;
                ptSprite.x &= ~TILE_FLAG_CHECKED;
                dwSize = 0x20;
                dwOraMask = TILE_FLAG_CHECKED;
            }

            LPBYTE lpAddrSrc = &g_ChrRam[0x1000 + TileID * 0x10];
            LPBYTE lpAddrDest = &g_ChrRam[0x1000 + ((ptSprite.y << 4) | ptSprite.x) * 0x10];
            uint8_t srcId = m_dragTile.data;
            uint8_t destId = uint8_t((ptSprite.y << 4) | ptSprite.x);

            BYTE dataSrc[0x20] = { 0 };
            BYTE dataEmpty[0x20] = { 0 };

            //备份源数据
            memcpy_s(dataSrc, dwSize, lpAddrSrc, dwSize);

            //清除源数据
            if (eClickFromType::eFromSpriteTile == m_ClickFrom)
            {
                memcpy_s(lpAddrSrc, dwSize, dataEmpty, dwSize);

                // 精灵组
                for (auto& item : m_SpTileList)
                {
                    if (item.data == srcId)
                    {
                        item.data = destId;
                        item.data |= dwOraMask;
                    }
                }

                // 替换光标组
                for (auto& item : m_CursorTileList)
                {
                    if (item.data == srcId)
                    {
                        item.data = destId;
                        item.data |= dwOraMask;
                    }
                }

                // 替换光标
                if (m_tileCursor.data == srcId)
                {
                    m_tileCursor.data = destId;
                    m_tileCursor.data |= dwOraMask;
                }
            }

            //拷贝到目的数据
            memcpy_s(lpAddrDest, dwSize, dataSrc, dwSize);

            //拷贝源数据
            if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
            {
                memcpy_s(lpAddrSrc, dwSize, dataSrc, dwSize);
            }

            m_ptCurSpTile = ptSprite;
            DrawTile(0, 0, 0, 4);
            DrawSpVram();
            UpdateSpVramEdit();
            DrawNameTable();
        }

        if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkg, ptOffset))
        {
            LPBYTE lpAddrSrc = &g_ChrRam[0x1000 + m_dragTile.data * 0x10];
            LPBYTE lpAddrDest = &g_ChrRam[((ptBkg.y << 4) | ptBkg.x) * 0x10];

            memcpy_s(lpAddrDest, 0x10, lpAddrSrc, 0x10);

            m_ptCurBkgTile = ptBkg;
            DrawTile(0, 4, 0, 0);
            DrawBkgVram();
            UpdateBkgVramEdit();
            DrawNameTable();
        }

        DrawNameTable();
        ::UpdateWindow(m_hWndNameTable);
        ::ShowWindow(m_wndDragTile, SW_HIDE);
    }

    if (eClickType::eCustomText == m_Click)
    {
        if ((m_CustomText.pt.x + (LONG)m_CustomText.strText.size()) > 0
            && m_CustomText.pt.x <= 31 &&
            (m_CustomText.pt.y >= 0 && m_CustomText.pt.y <= 29)
            )
        {
            m_CustomTextList.push_back(m_CustomText);
        }

        m_CustomText.strText.clear();
        DrawNameTable();
    }

    do
    {
        POINT pt = { xPos, yPos };

        //检查命名表点击
        if (eClickType::eNameTable == m_Click && ClickNameTableAttributes(pt))
        {
            m_SelSpPalette = false;
            DrawBkgPalette();
            DrawSpritePalette();
            //DrawAttribute();
            break;
        }

        //检查属性表点击
        if (eClickType::eAttribute == m_Click && ClickAttribute(pt))
        {
            m_SelSpPalette = false;
            DrawBkgPalette();
            DrawSpritePalette();
            break;
        }

        //检查背景调色板点击
        if ((eClickType::eBkgPalette == m_Click) && ClickBkgPalette(pt))
        {
            m_SelSpPalette = false;
            DrawSpritePalette();
            DrawBkgPalette();
            break;
        }

        //精灵调色板
        if ((eClickType::eSpritePalette == m_Click) && ClickSpritePalette(pt))
        {
            m_SelSpPalette = true;
            DrawSpritePalette();
            DrawBkgPalette();
            break;
        }

        //检查调色板点击
        if (eClickType::ePalette == m_Click && ClickPalette(pt))
        {
            break;
        }
    } while (FALSE);

    m_Click = eClickType::eNone;

    UpdateHistoryData(true);
    CheckSpriteAttributeButton();

    return TRUE;
}

LRESULT CDesignDialog::OnLButtonDbClick(WPARAM wParam, LPARAM lParam)
{
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    POINT ptOffset = { 0 };

    bool bCheck = false;

    if (m_bQuickCharMappingSrc)
    {
        return TRUE;
    }

    do
    {
        if (m_bQuickBkgList && !m_bSelectMode)
        {
            POINT ptBkg = { xPos , yPos };
            POINT ptSp = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkg, ptOffset))
            {
                m_ptCurBkgTile = ptBkg;
                ClickByFastBkgList({ xPos , yPos });
            }
            else if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptSp, ptOffset))
            {
                m_ptCurSpTile = ptSp;
                ClickByFastSpList({ xPos , yPos });
            }
            else
            {
                POINT pt = { xPos , yPos };
                POINT ptCurOffset = { 0 };
                if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, pt, ptCurOffset))
                {
                    m_Click = eClickType::eBkgListQuick;
                    m_ptNameTableTile = pt;
                    m_ptCurNameTable = { pt.x / 4, pt.y / 4 };
                    m_ptCurNameTableAttr = { pt.x / 2, pt.y / 2 };

                    UpdateAttribute();

                    DrawBkgVram();
                    DrawAttribute();
                    DrawNameTable();
                }
            }

            return TRUE;
        }

        //字符映射

        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, pt, ptOffset))
            {
                m_ptCharMapSrc = pt;
                DrawCharMappingSource();
            }
        }

        if (m_bShowCursor)
        {
            //箭头列表
            if (m_bBigCursor)
            {
                POINT pt = { xPos , yPos };
                if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
                {
                    for (auto& item : m_CursorTileList)
                    {
                        RECT rtTile = { 0 };
                        rtTile.left = item.pt.x * 2;
                        rtTile.top = item.pt.y * 2;
                        rtTile.right = rtTile.left + 16;
                        rtTile.bottom = rtTile.top + 16;

                        if (m_bBigSprite)
                        {
                            rtTile.bottom = rtTile.top + 32;
                        }

                        if (PtInRect(&rtTile, pt))
                        {
                            item.flag = item.flag ^ TILE_FLAG_CHECKED;
                            bCheck = true;
                            DrawNameTable();

                            break;
                        }
                    }
                }
            }
            else
            {
                POINT pt = { xPos , yPos };
                if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
                {
                    RECT rtTile = { 0 };
                    rtTile.left = m_tileCursor.pt.x * 2;
                    rtTile.top = m_tileCursor.pt.y * 2;
                    rtTile.right = rtTile.left + 16;
                    rtTile.bottom = rtTile.top + 16;

                    if (m_bBigSprite)
                    {
                        rtTile.bottom = rtTile.top + 32;
                    }

                    if (PtInRect(&rtTile, pt))
                    {
                        m_tileCursor.flag = m_tileCursor.flag ^ TILE_FLAG_CHECKED;
                        bCheck = true;
                        DrawNameTable();

                        break;
                    }
                }
            }
        }

        if (bCheck)
        {
            break;
        }

        //精灵组
        if (m_bShowBgTile)
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
            {
                for (auto& item : m_SpTileList)
                {
                    RECT rtTile = { 0 };
                    rtTile.left = item.pt.x * 2;
                    rtTile.top = item.pt.y * 2;
                    rtTile.right = rtTile.left + 16;
                    rtTile.bottom = rtTile.top + 16;

                    if (m_bBigSprite)
                    {
                        rtTile.bottom = rtTile.top + 32;
                    }

                    if (!PtInRect(&rtTile, pt))
                    {
                        continue;
                    }

                    item.flag = item.flag ^ TILE_FLAG_CHECKED;
                    bCheck = true;
                    DrawNameTable();

                    break;
                }
            }
        }

        if (bCheck)
        {
            break;
        }

        //背景组
        if (m_bShowBgTile)
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, pt, ptOffset))
            {
                for (auto& item : m_BkgTileList)
                {
                    if (item.pt != pt)
                    {
                        continue;
                    }

                    item.flag = item.flag ^ TILE_FLAG_CHECKED;
                    bCheck = true;
                    DrawNameTable();

                    break;
                }
            }
        }

        if (bCheck)
        {
            break;
        }

        //背景图案表
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, pt, ptOffset))
            {

                for (auto& item : m_BkgTileList)
                {
                    if (item.flag & TILE_FLAG_CHECKED)
                    {
                        item.data = (BYTE)((pt.y << 4) | pt.x);
                    }
                }

                DrawNameTable();

                break;
            }
        }

        //精灵图案表
        {
            POINT pt = { xPos , yPos };
            if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, pt, ptOffset))
            {
                BYTE bOrqForBigSprite = m_bBigSprite ? 0x01 : 0x00;

                for (auto& item : m_SpTileList)
                {
                    if (item.flag & TILE_FLAG_CHECKED)
                    {
                        item.data = (BYTE)((pt.y << 4) | pt.x | bOrqForBigSprite);
                    }
                }

                if (m_bBigCursor)
                {
                    for (auto& item : m_CursorTileList)
                    {
                        if (item.flag & TILE_FLAG_CHECKED)
                        {
                            item.data = (BYTE)((pt.y << 4) | pt.x | bOrqForBigSprite);
                        }
                    }
                }
                else
                {
                    if (m_tileCursor.flag & TILE_FLAG_CHECKED)
                    {
                        m_tileCursor.data = (BYTE)((pt.y << 4) | pt.x | bOrqForBigSprite);
                    }
                }

                DrawNameTable();

                break;
            }
        }
    } while (FALSE);
    CheckSpriteAttributeButton();

    return TRUE;
}

LRESULT CDesignDialog::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);
    POINT ptOffset = { 0 };

    if (eClickType::eNone == m_Click && !m_bSelecting)
    {
        return 0;
    }

    if (m_bQuickBkgList && (eClickType::eBkgListQuick == m_Click))
    {
        POINT pt = { xPos, yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, pt, ptOffset))
        {
            m_ptNameTableTile = pt;
            m_ptCurNameTable = { pt.x / 4, pt.y / 4 };
            m_ptCurNameTableAttr = { pt.x / 2, pt.y / 2 };

            UpdateAttribute();

            DrawBkgVram();
            DrawAttribute();
            DrawNameTable();
        }
    }

    if (m_bSelecting)
    {
        POINT ptNtLT = m_ptNtSelectBegin;
        POINT ptNtRB = m_ptNtSelectEnd;
        POINT ptBkgLT = m_ptBkgTileSelectBegin;
        POINT ptBkgRB = m_ptBkgTileSelectEnd;
        POINT ptSpLT = m_ptSpTileSelectBegin;
        POINT ptSpRB = m_ptSpTileSelectEnd;

        POINT ptNt = { xPos, yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, ptNt, ptOffset))
        {
            m_ptNtSelectEnd = ptNt;
            ptNtRB = ptNt;
        }
        else
        {
            POINT ptCur = { xPos , yPos };
            GetCursorPos(&ptCur);
            ScreenToClient(m_hWndNameTable, &ptCur);
            m_ptNtSelectEnd = ptNt;
        }

        POINT ptBkg = { xPos, yPos };
        if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 256, 256, ptBkg, ptOffset))
        {
            m_ptBkgTileSelectEnd = ptBkg;
            ptBkgRB = ptBkg;
        }
        else
        {
            POINT ptCur = { xPos , yPos };
            GetCursorPos(&ptCur);
            ScreenToClient(m_wndBkgTile, &ptCur);
            m_ptBkgTileSelectEnd = ptBkg;
        }

        POINT ptSp = { xPos, yPos };
        if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 256, 256, ptSp, ptOffset))
        {
            m_ptSpTileSelectEnd = ptSp;
            ptSpRB = ptSp;
        }
        else
        {
            POINT ptCur = { xPos , yPos };
            GetCursorPos(&ptCur);
            ScreenToClient(m_wndSpTile, &ptCur);
            m_ptSpTileSelectEnd = ptSp;
        }

        CheckSelectedNameTableTile(ptNtLT, ptNtRB);
        CheckSelectedBkgTile(ptBkgLT, ptBkgRB);
        CheckSelectedSpTile(ptSpLT, ptSpRB);

        DrawBkgVram();
        DrawSpVram();
        DrawNameTable();

        return TRUE;
    }

    if (eClickType::eBkgTileEdit == m_Click)
    {
        ClickBkgChrEdit({ xPos, yPos });
    }

    if (eClickType::eSpriteTileEdit == m_Click)
    {
        ClickSpriteChrEdit({ xPos, yPos });
    }

    if (eClickType::eCharMapSrc == m_Click)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_ptCharMapSrc = pt;
            DrawCharMappingSource();

            BYTE data = g_CharMapData[0x20 + pt.x];
            m_ptCurBkgTile = { data & 0x0F,  (data & 0xF0) >> 4 };

            m_SelSpPalette = false;
            DrawBkgVram();
            DrawBkgPalette();
            DrawSpritePalette();
            UpdateBkgVramEdit();

            return TRUE;
        }
    }

    if (eClickType::eCharMapDest == m_Click)
    {
        POINT pt = { xPos , yPos };
        if (GetGridClick(m_hWnd, m_wndCharMappingDest, 1024, 16, 64, 1, pt, ptOffset))
        {
            m_ptCharMapDest = pt;
            DrawCharMappingTarget();

            m_SelSpPalette = false;
            DrawBkgVram();
            DrawBkgPalette();
            DrawSpritePalette();
            UpdateBkgVramEdit();

            return TRUE;
        }
    }

    if (eClickType::eCursor == m_Click)
    {
        POINT ptNt = { xPos, yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, ptNt, ptOffset))
        {
            m_tileCursor.pt.x = (ptNt.x - m_ptClickOffset.x) / 2;
            m_tileCursor.pt.y = (ptNt.y - m_ptClickOffset.y) / 2;

            if (m_tileCursor.pt.x < 0) m_tileCursor.pt.x = 0;
            if (m_tileCursor.pt.x > 255) m_tileCursor.pt.x = 255;

            if (m_tileCursor.pt.y < 1) m_tileCursor.pt.y = 1;
            if (m_tileCursor.pt.y > 239) m_tileCursor.pt.y = 239;

            if (!m_CursorTileList.empty())
            {
                auto& firstItem = m_CursorTileList.front();
                firstItem.pt.x = m_tileCursor.pt.x - 8;
                firstItem.pt.y = m_tileCursor.pt.y - 4;

                UpdateCursorTileList();
            }

            DrawNameTable();
        }

        return TRUE;
    }

    if (eClickType::eCursorList == m_Click)
    {
        POINT ptNt = { xPos, yPos };

        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, ptNt, ptOffset))
        {
            auto& firstArrow = m_CursorTileList.front();

            firstArrow.pt.x = (ptNt.x - m_ptClickOffset.x) / 2;
            firstArrow.pt.y = (ptNt.y - m_ptClickOffset.y) / 2;

            m_tileCursor.pt.x = firstArrow.pt.x + 8;
            m_tileCursor.pt.y = firstArrow.pt.y + 4;
            UpdateCursorTileList();

            DrawNameTable();
        }

        return TRUE;
    }

    if (eClickType::eBkgTile == m_Click)
    {
        POINT ptNt = { xPos, yPos };
        POINT ptBkg = { xPos, yPos };
        POINT ptSprite = { xPos, yPos };
        POINT ptChar = { xPos, yPos };

        if (GetGridClick(m_hWnd, m_wndCharMappingSrc, 1024, 16, 64, 1, ptChar, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_wndCharMappingSrc, &rtWnd);

            DWORD dwX = m_dragTile.data & 0x0F;
            DWORD dwY = (m_dragTile.data & 0xF0) >> 4;

            StretchBlt(
                m_wndDragTile.GetMemDC(),
                1, 1, 16, 16,
                m_wndTileDraw.GetMemDC(),
                0 * 128 + dwX * 8, dwY * 8, 8, 8, SRCCOPY);

            ::InvalidateRect(m_wndDragTile, NULL, FALSE);

            ::MoveWindow(m_wndDragTile, rtWnd.left + ptChar.x * 16 - 1, rtWnd.top + ptChar.y * 16 - 1, 18, 18, FALSE);
            return TRUE;
        }

        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_hWndNameTable, &rtWnd);

            DWORD dwX = m_dragTile.data & 0x0F;
            DWORD dwY = (m_dragTile.data & 0xF0) >> 4;

            DWORD dwIndex = GetNameTableColorIndex(ptNt);
            StretchBlt(
                m_wndDragTile.GetMemDC(),
                1, 1, 16, 16,
                m_wndTileDraw.GetMemDC(),
                dwIndex * 128 + dwX * 8, dwY * 8, 8, 8, SRCCOPY);

            ::InvalidateRect(m_wndDragTile, NULL, FALSE);
            ::MoveWindow(m_wndDragTile, rtWnd.left + ptNt.x * 16 - 1, rtWnd.top + ptNt.y * 16 - 1, 18, 18, FALSE);
            return TRUE;
        }
        else if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkg, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_wndBkgTile, &rtWnd);

            DWORD dwX = m_dragTile.data & 0x0F;
            DWORD dwY = (m_dragTile.data & 0xF0) >> 4;

            DWORD dwIndex = m_ptCurBkgPalette.x >> 2;
            StretchBlt(
                m_wndDragTile.GetMemDC(),
                1, 1, 16, 16,
                m_wndTileDraw.GetMemDC(),
                dwIndex * 128 + dwX * 8, dwY * 8, 8, 8, SRCCOPY);

            ::InvalidateRect(m_wndDragTile, NULL, FALSE);
            ::MoveWindow(m_wndDragTile, rtWnd.left + ptBkg.x * 16 - 1, rtWnd.top + ptBkg.y * 16 - 1, 18, 18, FALSE);
            return TRUE;
        }
        else if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptSprite, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_wndSpTile, &rtWnd);

            DWORD dwX = m_dragTile.data & 0x0F;
            DWORD dwY = (m_dragTile.data & 0xF0) >> 4;

            DWORD dwIndex = m_ptCurSpPalette.x >> 2;
            StretchBlt(
                m_wndDragTile.GetMemDC(),
                1, 1, 16, 16,
                m_wndTileDraw.GetMemDC(),
                dwIndex * 128 + dwX * 8, dwY * 8, 8, 8, SRCCOPY);

            ::InvalidateRect(m_wndDragTile, NULL, FALSE);
            ::MoveWindow(m_wndDragTile, rtWnd.left + ptSprite.x * 16 - 1, rtWnd.top + ptSprite.y * 16 - 1, 18, 18, FALSE);
            return TRUE;
        }

        ::MoveWindow(m_wndDragTile, xPos - m_ptDragOffset.x - 1, yPos - m_ptDragOffset.y - 1, 18, 18, FALSE);

        return TRUE;
    }

    if (eClickType::eSpriteTile == m_Click)
    {
        POINT pt = { xPos, yPos };
        POINT ptBkg = { xPos, yPos };
        POINT ptSprite = { xPos, yPos };

        int nHeight = 18;
        int nYOffset = 1;

        if (m_bBigSprite)
        {
            nHeight = 36;
            nYOffset = 2;
        }

        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 512, 480, pt, ptOffset))
        {
            LONG lPosX = xPos - m_ptDragOffset.x;
            LONG lPosY = yPos - m_ptDragOffset.y;

            POINT ptClient = { 0 };
            ::ClientToScreen(m_hWndNameTable, &ptClient);

            if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
            {
                POINT ptCur = { xPos - m_ptClickOffset.x, yPos - m_ptClickOffset.y };
                GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptCur, ptOffset);
                ptCur = { xPos - m_ptClickOffset.x + ptOffset.x, yPos - m_ptClickOffset.y + ptOffset.y };
                GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptCur, ptOffset);

                lPosX = ptCur.x * 16;
                lPosY = ptCur.y * 16;

                ::MoveWindow(m_wndDragTile, lPosX + ptClient.x - 1, lPosY + ptClient.y - nYOffset, 18, nHeight, FALSE);
            }
            else
            {
                POINT ptCur = { xPos, yPos };
                GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 256, 240, ptCur, ptOffset);

                lPosX = ptCur.x * 2 - m_ptClickOffset.x;
                lPosY = ptCur.y * 2 - m_ptClickOffset.y;

                ::MoveWindow(m_wndDragTile, lPosX + ptClient.x - 1, lPosY + ptClient.y - nYOffset, 18, nHeight, FALSE);
            }

            return TRUE;
        }
        else if (GetGridClick(m_hWnd, m_wndSpTile, 256, 256, 16, 16, ptSprite, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_wndSpTile, &rtWnd);

            ::MoveWindow(m_wndDragTile, rtWnd.left + ptSprite.x * 16 - 1, rtWnd.top + ptSprite.y * 16 - 1, 18, nHeight, FALSE);
            return TRUE;
        }
        else if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, ptBkg, ptOffset))
        {
            RECT rtWnd = { 0 };
            GetWindowRect(m_wndBkgTile, &rtWnd);

            ::MoveWindow(m_wndDragTile, rtWnd.left + ptBkg.x * 16 - 1, rtWnd.top + ptBkg.y * 16 - 1, 18, nHeight, FALSE);
            return TRUE;
        }

        ::MoveWindow(m_wndDragTile, xPos - m_ptDragOffset.x - 1, yPos - m_ptDragOffset.y - 1, 18, nHeight, FALSE);

        return TRUE;
    }

    if (eClickType::eCaption == m_Click)
    {
        POINT ptNt = { xPos, yPos };
        GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset);

        POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);

        int nSizeX = 512 / 32;
        int nSizeY = 480 / 30;

        m_ptCaption.x = (xPos - ptCurOffset.x - m_ptDragOffset.x) / nSizeX;
        m_ptCaption.y = (yPos - ptCurOffset.y - m_ptDragOffset.y) / nSizeY;

        if (m_ptCaption.x < 0) m_ptCaption.x = 0;
        if (m_ptCaption.y < 0) m_ptCaption.y = 0;
        if (m_ptCaption.x > 31)m_ptCaption.x = 31;
        if (m_ptCaption.y > 29)m_ptCaption.y = 29;

        DrawNameTable();
        return TRUE;
    }

    if (eClickType::ePageNumber == m_Click)
    {
        POINT ptNt = { xPos, yPos };
        GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset);

        POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);

        int nSizeX = 512 / 32;
        int nSizeY = 480 / 30;

        m_ptPageNumber.x = (xPos - ptCurOffset.x - m_ptDragOffset.x) / nSizeX;
        m_ptPageNumber.y = (yPos - ptCurOffset.y - m_ptDragOffset.y) / nSizeY;

        if (m_ptPageNumber.x < 0)
        {
            m_ptPageNumber.x = 0;
        }
        if (m_ptPageNumber.y < 0)
        {
            m_ptPageNumber.y = 0;
        }

        if (m_ptPageNumber.x + m_strPageNumber.size() >= 32)
        {
            m_ptPageNumber.x = 32 - (LONG)m_strPageNumber.size();
        }
        if (m_ptPageNumber.y + 1 >= 30)
        {
            m_ptPageNumber.y = 29;
        }

        DrawNameTable();
        return TRUE;
    }

    if (eClickType::eItemText == m_Click)
    {
        POINT ptNt = { xPos, yPos };
        GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset);
        POINT ptWndOffset = GetClientOffset(m_hWnd, m_hWndNameTable);

        int nSizeX = 512 / 32;
        int nSizeY = 480 / 30;

        m_ptItem.x = (xPos - ptWndOffset.x - m_ptDragOffset.x) / nSizeX;
        m_ptItem.y = (yPos - ptWndOffset.y - m_ptDragOffset.y) / nSizeY;

        if (m_ptItem.x < 0)
        {
            m_ptItem.x = 0;
        }
        if (m_ptItem.y < 0)
        {
            m_ptItem.y = 0;
        }

        if (m_ptItem.x >= 32)
        {
            m_ptItem.x = 31;
        }
        if (m_ptItem.y + m_ptItemMax.y >= 30)
        {
            m_ptItem.y = 30 - m_ptItemMax.y;
        }

        DrawNameTable();
        return TRUE;
    }

    if (eClickType::eCustomText == m_Click)
    {
        POINT ptNt = { xPos, yPos };
        GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset);

        POINT ptCurOffset = GetClientOffset(m_hWnd, m_hWndNameTable);

        int nSizeX = 512 / 32;
        int nSizeY = 480 / 30;

        m_CustomText.pt.x = (xPos - ptCurOffset.x - m_ptDragOffset.x) / nSizeX;
        m_CustomText.pt.y = (yPos - ptCurOffset.y - m_ptDragOffset.y) / nSizeY;

        DrawNameTable();
        return TRUE;
    }

    //检查命名表点击
    if (eClickType::eNameTable == m_Click)
    {

        POINT ptNt = { xPos, yPos };
        if (GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 32, 30, ptNt, ptOffset))
        {
            m_ptNameTableTile = ptNt;
        }

        ClickNameTableAttributes({ xPos , yPos });
        m_SelSpPalette = false;
        DrawBkgPalette();
        DrawSpritePalette();
        return TRUE;
    }

    //检查属性表点击
    if (eClickType::eAttribute == m_Click)
    {
        ClickAttribute({ xPos , yPos });
        m_SelSpPalette = false;
        DrawAttribute();
        DrawBkgPalette();
        DrawSpritePalette();
        return TRUE;
    }

    //检查背景调色板点击
    if (eClickType::eBkgPalette == m_Click)
    {
        ClickBkgPalette({ xPos , yPos });
        m_SelSpPalette = false;
        DrawSpritePalette();
        DrawBkgPalette();
        return TRUE;
    }

    //精灵调色板
    if (eClickType::eSpritePalette == m_Click)
    {
        ClickSpritePalette({ xPos , yPos });
        m_SelSpPalette = true;
        DrawSpritePalette();
        DrawBkgPalette();
        return TRUE;
    }

    //检查调色板点击
    if (eClickType::ePalette == m_Click)
    {
        ClickPalette({ xPos , yPos });
        return TRUE;
    }

    return TRUE;
}
