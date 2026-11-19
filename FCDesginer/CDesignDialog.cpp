#include "CDesignDialog.h"
#include "resource.h"
#include <strsafe.h>
#include <windowsx.h>
#include "CCustomMenuData.h"

#pragma comment(lib, "Msimg32.lib")
#pragma comment (lib ,"imm32.lib")

bool CDesignDialog::m_bBkgTileCheck[16 * 16] = { false };
bool CDesignDialog::m_bSpTileCheck[16 * 16] = { false };

bool operator == (const POINT& ptA, const POINT& ptB)
{
    return ptA.x == ptB.x && ptA.y == ptB.y;
}

bool operator != (const POINT& ptA, const POINT& ptB)
{
    return !(ptA.x == ptB.x && ptA.y == ptB.y);
}

CDesignDialog::CDesignDialog()
{
    m_AttributeData = g_AttributeData[0];

}

CDesignDialog::~CDesignDialog()
{
    if (m_hBrush)
    {
        ::DeleteObject(m_hBrush);
    }
}

BOOL CDesignDialog::PreTranslateMessage(LPMSG pMsg)
{
    HWND hWndCaption = GetDlgItem(m_hWnd, IDC_EDIT_CAPTION_TEXT);
    HWND hWndText = GetDlgItem(m_hWnd, IDC_EDIT_CUSTOM_TEXT);

    if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
    {
        PostMessage(WM_CLOSE, 0, 0);
        return TRUE;
    }

    if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
    {
        return TRUE;
    }

    if (WM_KEYDOWN == pMsg->message && VK_DELETE == pMsg->wParam)
    {
        DeleteCheckedTile();
    }

    if (WM_KEYUP == pMsg->message && VK_SHIFT == pMsg->wParam)
    {
        m_bSelectMode = false;
        m_bSelecting = false;
    }

    if (eClickType::eNone == m_Click)
    {
        if (WM_KEYDOWN == pMsg->message && VK_SHIFT == pMsg->wParam)
        {
            m_bSelectMode = true;
        }
    }

    if (hWndCaption == pMsg->hwnd && (WM_CHAR == pMsg->message || WM_KEYDOWN == pMsg->message))
    {
        if (VK_BACK == pMsg->wParam)
        {
            return FALSE;
        }

        if (m_strCaption.size() >= 32)
        {
            return TRUE;
        }

        m_strCaption = GetItemString(IDC_EDIT_CAPTION_TEXT);
        DrawNameTable();
    }

    if (hWndText == pMsg->hwnd && (WM_CHAR == pMsg->message || WM_KEYDOWN == pMsg->message))
    {
        if (VK_BACK == pMsg->wParam)
        {
            return FALSE;
        }

        _tstring strCustom = GetItemString(IDC_EDIT_CUSTOM_TEXT);
        if (strCustom.size() >= 32)
        {
            return TRUE;
        }
    }

    if (hWndCaption == pMsg->hwnd && WM_KEYUP == pMsg->message)
    {
        m_strCaption = GetItemString(IDC_EDIT_CAPTION_TEXT);
        DrawNameTable();
    }

    return FALSE;
}

void CDesignDialog::UpdateLicense()
{
    //修改标题
    if (m_ftType < Fcad_Type::FDT_Basic)
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_CAPTION_TEXT), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_RESET), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_PALETTE_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_PALETTE_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_PALETTE_RESET), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_PALETTE_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_PALETTE_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_PALETTE_RESET), FALSE);


        EnableWindow(m_hWndBkgPalette, FALSE);
        EnableWindow(m_hWndSpPalette, FALSE);
        EnableWindow(m_hWndPalette, FALSE);
    }

    //文本组
    if (m_ftType < Fcad_Type::FDT_Standard)
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_CUSTOM_TEXT), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_ADD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_RESET), FALSE);

        //图块编辑
        EnableWindow(m_wndBkgTileEdit, FALSE);
        EnableWindow(m_wndSpTileEdit, FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_TILE_SELECT), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TILE_SELECT_ALL), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TILE_SELECT_INVERSE), FALSE);

        HMENU hMenu = GetMenu(m_hWnd);
        ModifyMenuState(hMenu, ID_TILE_COLOR0123_MOVE_LEFT, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_COLOR0123_MOVE_RIGHT, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_COLOR123_MOVE_LEFT, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_COLOR123_MOVE_RIGHT, MFS_DISABLED, FALSE);

        ModifyMenuState(hMenu, ID_TILE_DELETE_COLOR_1, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_DELETE_COLOR_2, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_DELETE_COLOR_3, MFS_DISABLED, FALSE);

        ModifyMenuState(hMenu, ID_TILE_MOVE_UP, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_MOVE_DOWN, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_MOVE_LEFT, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_MOVE_RIGHT, MFS_DISABLED, FALSE);

        ModifyMenuState(hMenu, ID_TILE_SWAP_COLOR_1_2, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_SWAP_COLOR_1_3, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_TILE_SWAP_COLOR_2_3, MFS_DISABLED, FALSE);
    }

    //背景组精灵组
    if (m_ftType < Fcad_Type::FDT_Advanced)
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_BKG_LIST_FAST), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_SELECT_ALL), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_SELECT_INVERSE), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LIST_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LIST_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LIST_RESET), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_UP), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_DOWN), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LEFT), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_RIGHT), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_UP), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_DOWN), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_LEFT), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_RIGHT), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_SELECT_ALL), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_SELECT_INVERSE), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LIST_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LIST_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LIST_RESET), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_UP), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_DOWN), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LEFT), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_RIGHT), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SWAP_SP), FALSE);
    }

    if (m_ftType < Fcad_Type::FDT_Pro)
    {
        //存取菜单
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_RESET), FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAYOUT_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAYOUT_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAYOUT_RESET), FALSE);

        //存取CHR
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_CHR_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_CHR_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_CHR_RESET), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_CHR_LOAD), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_CHR_SAVE_AS), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_CHR_RESET), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SWAP_CHR), FALSE);
    }

    if (m_ftType < Fcad_Type::FDT_Full)
    {
        //字符映射
        HMENU hMenu = GetMenu(m_hWnd);
        ModifyMenuState(hMenu, ID_EDIT_CLEAR_UNUSE_BKG_TILE, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_EDIT_CLEAR_UNUSE_SP_TILE, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_EDIT_AUTO_CHAR_MAPPING, MFS_DISABLED, FALSE);
        ModifyMenuState(hMenu, ID_EDIT_BG_CHAR_MAPPING, MFS_DISABLED, FALSE);

        EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_BKG_CHR_FAST), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_SP_CHR_FAST), FALSE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_CHAR_MAP_FAST), FALSE);

        EnableWindow(m_wndCharMappingSrc, FALSE);
        EnableWindow(m_wndCharMappingDest, FALSE);
    }
}

void CDesignDialog::SetWndPos()
{
    MoveToCenter(::GetParent(m_hWnd));
    RECT rtWindow = { 0 };
    RECT rtParent = { 0 };
    GetWindowRect(m_hWnd, &rtWindow);
    GetWindowRect(GetParent(m_hWnd), &rtParent);

    rtWindow.right = rtWindow.left + 1088;
    rtWindow.bottom = rtWindow.top + 864;// +256;

    AdjustWindowRect(&rtWindow, (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE), TRUE);

    ::MoveWindow(m_hWnd,
        rtWindow.left,
        rtParent.top,
        rtWindow.right - rtWindow.left,
        rtWindow.bottom - rtWindow.top, TRUE);

    int nPosX = 8;
    int nPosY = 8;

    //字符映射
    {
        nPosX = 8;
        nPosY = 8;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_CHAR), nPosX, nPosY, 1024 + 44, 96, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_CHAR_DEST), nPosX + 4, nPosY + 16, 64, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_BKG_CHR_FAST), nPosX + 64, nPosY + 16, 200, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_SP_CHR_FAST), nPosX + 64 + 256, nPosY + 16, 256, 16, TRUE);
        ::MoveWindow(m_wndCharMappingDest, nPosX + 4, nPosY + 16 + 16, 1024, 16, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_CHAR_SRC), nPosX + 4, nPosY + 52, 64, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_CHAR_MAP_FAST), nPosX + 64, nPosY + 52, 200, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_EXPORT_CHAR_MAPPING), nPosX + 64 + 256, nPosY + 52, 48, 20, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_RESET_CHAR_MAPPING), nPosX + 64 + 256 + 64, nPosY + 52, 48, 20, TRUE);
        ::MoveWindow(m_wndCharMappingSrc, nPosX + 4, nPosY + 52 + 20, 1024, 16, TRUE);
    }

    //命名表
    {
        nPosX = 8;
        nPosY = 108;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_NAME_TABLE), nPosX, nPosY, 512 + 16, 720, TRUE);

        nPosX += 8;
        nPosY += 16;
        ::MoveWindow(m_hWndNameTable, nPosX, nPosY, 512, 480, TRUE);
    }

    //翻页按钮
    {
        nPosY += 480;

        nPosY += 4;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_FIRST_PAGE), 16, nPosY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_PRE_PAGE), 16 + 52, nPosY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_NEXT_PAGE), 16 + 52 * 2, nPosY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAST_PAGE), 16 + 52 * 3, nPosY, 48, 24, TRUE);
    }

    //网格线
    {
        nPosX = 272;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_FILL_COLOR), nPosX, nPosY, 80, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_DRAW_LINE), nPosX + 80, nPosY, 80, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_DRAG_TEXT), nPosX + 160, nPosY, 80, 24, TRUE);
    }

    //标题
    {
        nPosX = 16;
        nPosY = 648;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_CAPTION_TEXT), nPosX, nPosY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_EDIT_CAPTION_TEXT), nPosX + 56, nPosY - 4, 178, 24, TRUE);
        nPosY += 24;
    }

    // 文本组
    {
        nPosX = 16;
        nPosY = 670;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_TEXT_LIST), nPosX, nPosY, 238, 88, TRUE);

        int nBtnX = nPosX + 50;
        int nBtnY = nPosY + 20;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_CUSTOM_TEXT), nPosX + 4, nBtnY + 4, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_EDIT_CUSTOM_TEXT), 72, nBtnY, 128, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_CUSTOM_ADD), 206, nBtnY, 42, 24, TRUE);

        nBtnX = nPosX + 14;
        nBtnY += 36;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_LOAD), nBtnX + 56 * 0, nBtnY, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_SAVE_AS), nBtnX + 56 * 1, nBtnY, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TEXT_RESET), nBtnX + 56 * 2, nBtnY, 56, 22, TRUE);
    }

    // 背景组
    {
        nPosX = 264;
        nPosY = 638;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_BKG_LIST), nPosX, nPosY, 128, 152, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_BKG_LIST_FAST), nPosX + 4, nPosY + 16 + 22 * 0, 80, 20, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_SELECT_ALL), nPosX + 4, nPosY + 16 + 22 * 1, 80, 20, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_SELECT_INVERSE), nPosX + 4, nPosY + 16 + 22 * 2, 80, 20, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LIST_LOAD), nPosX + 4, nPosY + 16 + 22 * 3, 56, 20, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LIST_SAVE_AS), nPosX + 4, nPosY + 16 + 22 * 4, 56, 20, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LIST_RESET), nPosX + 4, nPosY + 16 + 22 * 5, 56, 20, TRUE);

        int nLeft = nPosX + 56 + 4;
        int nTop = nPosY + 68;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_UP), nLeft + 22, nTop, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_LEFT), nLeft, nTop + 22, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_RIGHT), nLeft + 44, nTop + 22, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_DOWN), nLeft + 22, nTop + 44, 22, 22, TRUE);
    }

    // 属性表
    {
        nPosX = 398;
        nPosY = 638;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_ATTR), nPosX, nPosY, 132, 92, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_LOAD), nPosX + 4, nPosY + 16 + 24 * 0, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_SAVE_AS), nPosX + 4, nPosY + 16 + 24 * 1, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_RESET), nPosX + 4, nPosY + 16 + 24 * 2, 56, 22, TRUE);

        int nLeft = nPosX + 56 + 8;
        int nTop = nPosY + 12;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_UP), nLeft + 22, nTop, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_LEFT), nLeft, nTop + 22, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_RIGHT), nLeft + 44, nTop + 22, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_ATTR_DOWN), nLeft + 22, nTop + 44, 22, 22, TRUE);
    }

    // 标记光标
    {
        nPosX = 402;
        nPosY = 738;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_MARK_CURSOR), nPosX, nPosY, 96, 20, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_BIG_CURSOR), nPosX, nPosY + 20, 120, 20, TRUE);
    }

    //图块选择
    {
        nPosX = 548;
        nPosY = 740;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_TILE_SELECT), nPosX, nPosY, 128, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TILE_SELECT_ALL), nPosX, nPosY + 24 * 1, 80, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_TILE_SELECT_INVERSE), nPosX, nPosY + 24 * 2, 80, 24, TRUE);
    }

    //布局存档
    {
        int nBtnX = 184;
        int nBtnY = 796;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAYOUT_LOAD), nBtnX + 58 * 0, nBtnY, 56, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAYOUT_SAVE_AS), nBtnX + 58 * 1, nBtnY, 56, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LAYOUT_RESET), nBtnX + 58 * 2, nBtnY, 56, 24, TRUE);
    }

    //CHR
    {
        nPosX = 540;
        nPosY = 108;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_CHR), nPosX, nPosY, 512 + 24, 318 + 128, TRUE);

        nPosY = 392;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_CHR_LOAD), nPosX + 6 + 52 * 0, nPosY + 128, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_CHR_SAVE_AS), nPosX + 6 + 52 * 1, nPosY + 128, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_CHR_RESET), nPosX + 6 + 52 * 2, nPosY + 128, 48, 24, TRUE);

        nPosX = 810;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SWAP_CHR), nPosX - 52 / 2, nPosY + 128, 48, 24, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_CHR_LOAD), nPosX + 52 * 2, nPosY + 128, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_CHR_SAVE_AS), nPosX + 52 * 3, nPosY + 128, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_CHR_RESET), nPosX + 52 * 4, nPosY + 128, 48, 24, TRUE);

        nPosX = 948;
        nPosY = 388;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_SPRITE_V), nPosX, nPosY + 0, 120, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_SPRITE_H), nPosX, nPosY + 20, 120, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_SPRITE_BACK), nPosX, nPosY + 40, 120, 16, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_CHECK_BIG_SPRITE), nPosX, nPosY + 60, 120, 16, TRUE);
    }

    //属性方格
    {
        nPosX = 540;
        nPosY = 418 + 16 + 128;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_COLOR_ATTRIBUTE), nPosX, nPosY, 128 + 12, 128 + 24, TRUE);

        nPosX = 546;
        nPosY = 418 + 32 + 128;
        ::MoveWindow(m_hWndAttribute, nPosX, nPosY, 128, 128, TRUE);
    }

    // 精灵组
    {
        nPosX = 970;
        nPosY = 418 + 16 + 128;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_SP_LIST), nPosX, nPosY, 102, 206, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_SELECT_ALL), nPosX + 4, nPosY + 16 + 24 * 0, 80, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_SELECT_INVERSE), nPosX + 4, nPosY + 16 + 24 * 1, 80, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LIST_LOAD), nPosX + 4, nPosY + 16 + 24 * 2, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LIST_SAVE_AS), nPosX + 4, nPosY + 16 + 24 * 3, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LIST_RESET), nPosX + 4, nPosY + 16 + 24 * 4, 56, 22, TRUE);

        int nBtnX = 974;
        int nBtnY = nPosY + 134;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_UP), nBtnX + 22, nBtnY, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_LEFT), nBtnX, nBtnY + 22, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_RIGHT), nBtnX + 44, nBtnY + 22, 22, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_DOWN), nBtnX + 22, nBtnY + 44, 22, 22, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SWAP_SP), nBtnX + 48, nBtnY + 44, 48, 22, TRUE);
    }

    //背景调色板
    {
        nPosX = 690;
        nPosY = 418 + 16 + 128;

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_BKG_PALETTE), nPosX, nPosY, 256 + 16, 58, TRUE);

        nPosX += 8;
        nPosY += 16;
        ::MoveWindow(m_hWndBkgPalette, nPosX, nPosY, 256, 16, TRUE);

        int nBtnX = 800;
        int nBtnY = 596;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_PALETTE_LOAD), nBtnX - 72, nBtnY, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_PALETTE_SAVE_AS), nBtnX, nBtnY, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_BKG_PALETTE_RESET), nBtnX + 72, nBtnY, 56, 22, TRUE);
    }

    //精灵调色板
    {
        nPosX = 690;
        nPosY += 42;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_SPRITE_PALETTE), nPosX, nPosY, 256 + 16, 58, TRUE);

        nPosX += 8;
        nPosY += 16;
        ::MoveWindow(m_hWndSpPalette, nPosX, nPosY, 256, 16, TRUE);

        int nBtnX = 800;
        int nBtnY = 654;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_PALETTE_LOAD), nBtnX - 72, nBtnY, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_PALETTE_SAVE_AS), nBtnX, nBtnY, 56, 22, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SP_PALETTE_RESET), nBtnX + 72, nBtnY, 56, 22, TRUE);
    }

    //调色板
    {
        nPosX = 690;
        nPosY += 44;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_STATIC_GROUP_PALETTE), nPosX, nPosY, 256 + 16, 88, TRUE);
        nPosX += 8;
        nPosY += 16;
        ::MoveWindow(m_hWndPalette, nPosX, nPosY, 256, 64, TRUE);
    }


    //底部按钮
    {
        int nBtnX = 500;
        int nBtnY = 832;
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_LOAD), nBtnX + 52 * 0, nBtnY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SAVE_AS), nBtnX + 52 * 1, nBtnY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_RESET), nBtnX + 52 * 2, nBtnY, 48, 24, TRUE);

        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_UNDO), nBtnX - 52 * 3, nBtnY, 48, 24, TRUE);
        ::MoveWindow(GetDlgItem(m_hWnd, IDC_BUTTON_REDO), nBtnX - 52 * 2, nBtnY, 48, 24, TRUE);
    }

}

void CDesignDialog::CalcItemRect()
{
    RECT rt = { 0 };

    int nSizeX = 512 / 32;
    int nSizeY = 480 / 30;

    m_ptItemMax.x = 0;
    m_ptItemMax.y = (LONG)m_strItemList.size();

    for (size_t i = 0; i < m_strItemList.size(); i++)
    {
        if (m_strItemList[i].empty())
        {
            continue;
        }

        if (i >= _countof(m_rtItems))
        {
            continue;
        }
        m_rtItems[i].left = m_ptItem.x * nSizeX;
        m_rtItems[i].top = (LONG)((m_ptItem.y + i) * nSizeY);
        m_rtItems[i].right = m_rtItems[i].left + (LONG)m_strItemList[i].size() * nSizeX;
        m_rtItems[i].bottom = m_rtItems[i].top + 1 * nSizeY;

        if ((LONG)m_strItemList[i].size() > m_ptItemMax.x)
        {
            m_ptItemMax.x = (LONG)m_strItemList[i].size();
        }
    }
}

void CDesignDialog::ResetCursorTileList()
{
    m_CursorTileList.clear();

    TILE_INFO info;
    info.data = 0x02;
    info.pt.x = (m_ptItem.x - 2) * 8 - 2;
    info.pt.y = m_ptItem.y * 8 - 4;

    m_CursorTileList.push_back(info);
    info.data = 0x03;
    m_CursorTileList.push_back(info);
    info.data = 0x12;
    m_CursorTileList.push_back(info);
    info.data = 0x13;
    m_CursorTileList.push_back(info);

    UpdateCursorTileList();
}

void CDesignDialog::UpdateCursorTileList()
{
    if (4 == m_CursorTileList.size())
    {
        auto& firstArrow = m_CursorTileList.front();
        if (firstArrow.pt.x < 0) firstArrow.pt.x = 0;
        if (firstArrow.pt.x > 255) firstArrow.pt.x = 255;

        if (firstArrow.pt.y < 1) firstArrow.pt.y = 1;
        if (firstArrow.pt.y > 239) firstArrow.pt.y = 239;

        m_CursorTileList[1].pt.x = firstArrow.pt.x + 8;
        m_CursorTileList[1].pt.y = firstArrow.pt.y;

        m_CursorTileList[2].pt.x = firstArrow.pt.x;
        m_CursorTileList[2].pt.y = firstArrow.pt.y + 8;
        if (m_bBigSprite)
        {
            m_CursorTileList[2].pt.y = firstArrow.pt.y + 16;
        }

        m_CursorTileList[3].pt.x = firstArrow.pt.x + 8;
        m_CursorTileList[3].pt.y = firstArrow.pt.y + 8;
        if (m_bBigSprite)
        {
            m_CursorTileList[3].pt.y = firstArrow.pt.y + 16;
        }
    }
}

void CDesignDialog::ClearBkgTileListCheck()
{
    for (auto& item : m_BkgTileList)
    {
        item.flag = 0;
    }
}

void CDesignDialog::ClearSpriteTileListCheck()
{
    for (auto& item : m_SpTileList)
    {
        item.flag = 0;
    }
}

void CDesignDialog::ClearCursorTileListCheck()
{
    for (auto& item : m_CursorTileList)
    {
        item.flag = 0;
    }
}

bool CDesignDialog::ClickNameTableAttributes(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_hWndNameTable, 512, 480, 16, 15, pt, ptOffset))
    {
        return false;
    }

    //命名表属性更新
    {
        m_ptCurNameTable = { pt.x / 2, pt.y / 2 };
        m_ptCurNameTableAttr = pt;

        int nIndex = (m_ptCurNameTable.y) * 8 + (m_ptCurNameTable.x);
        if (nIndex < _countof(g_AttributeData))
        {
            m_AttributeData = g_AttributeData[nIndex];
        }
    }

    bool bCtrlDown = false;

    //批量设置命名表颜色
    if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
    {
        bCtrlDown = true;
    }

    if (bCtrlDown && m_designEnable && (m_ftType > Fcad_Type::FDT_None))
    {
        UpdateNameTableColor((pt.y % 2) * 2 + (pt.x % 2));
        DrawAttribute();
        return true;
    }

    UpdateAttribute();
    DrawAttribute();

    DrawNameTable();
    DrawBkgPalette();
    DrawPalette();
    DrawBkgVram();

    return true;
}

bool CDesignDialog::ClickAttribute(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_hWndAttribute, 128, 128, 2, 2, pt, ptOffset))
    {
        return false;
    }

    //属性表更新
    {
        m_ptCurAttribute = pt;
        m_ptCurNameTableAttr.x = (m_ptCurNameTableAttr.x & ~TILE_FLAG_CHECKED) + m_ptCurAttribute.x;
        m_ptCurNameTableAttr.y = (m_ptCurNameTableAttr.y & ~TILE_FLAG_CHECKED) + m_ptCurAttribute.y;
        DrawAttribute();
        DrawNameTable();
    }

    //背景调色版更新
    {
        int nIndex = (m_ptCurAttribute.y * 2 + m_ptCurAttribute.x);
        if (0 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0x03) >> 0) * 4 + 1;
        }
        else if (1 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0x0C) >> 2) * 4 + 1;
        }
        else if (2 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0x30) >> 4) * 4 + 1;
        }
        else if (3 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0xC0) >> 6) * 4 + 1;
        }

        DrawBkgPalette();
    }

    //调色版更新
    {
        BYTE bData = g_BkgPalette[m_ptCurBkgPalette.x];
        m_ptCurPalette.x = bData % 16;
        m_ptCurPalette.y = bData / 16;
        DrawPalette();
    }

    return true;
}

void CDesignDialog::UpdateNameTableColor(int nAttributeIndex)
{
    {
        int nIndex = m_ptCurBkgPalette.x >> 2;
        if (0 == nAttributeIndex)
        {
            m_AttributeData = m_AttributeData & ~0x03;
            m_AttributeData |= nIndex << 0;
        }

        if (1 == nAttributeIndex)
        {
            m_AttributeData = m_AttributeData & ~0x0C;
            m_AttributeData |= nIndex << 2;
        }

        if (2 == nAttributeIndex)
        {
            m_AttributeData = m_AttributeData & ~0x30;
            m_AttributeData |= nIndex << 4;
        }

        if (3 == nAttributeIndex)
        {
            m_AttributeData = m_AttributeData & ~0xC0;
            m_AttributeData |= nIndex << 6;
        }
    }

    int nIndex = (m_ptCurNameTable.y) * 8 + (m_ptCurNameTable.x);
    if (nIndex < _countof(g_AttributeData))
    {
        g_AttributeData[nIndex] = m_AttributeData;
    }

    DrawNameTable();
}

bool CDesignDialog::ClickBkgPalette(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_hWndBkgPalette, 256, 16, 16, 1, pt, ptOffset))
    {
        return false;
    }

    if (m_ptLastBkgPal == pt) return true;
    m_ptLastBkgPal = pt;

    {
        m_ptCurBkgPalette = pt;
        DrawBkgVram();
        DrawBkgPalette();
        UpdateBkgVramEdit();
    }

    //调色版更新
    {
        BYTE bData = g_BkgPalette[m_ptCurBkgPalette.x];

        m_ptCurPalette.x = bData % 16;
        m_ptCurPalette.y = bData / 16;
        DrawPalette();
    }

    //更新命名表颜色
    UpdateNameTableColor(m_ptCurAttribute.y * 2 + m_ptCurAttribute.x);

    //属性
    {
        DrawAttribute();
    }

    DrawCharMappingSource();

    return true;
}

bool CDesignDialog::ClickSpritePalette(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_hWndSpPalette, 256, 16, 16, 1, pt, ptOffset))
    {
        return false;
    }

    if (m_ptLastSpPal == pt) return true;
    m_ptLastSpPal = pt;

    m_ptCurSpPalette = pt;

    bool bUpdate = false;
    for (auto& item : m_SpTileList)
    {
        if (item.flag & TILE_FLAG_CHECKED)
        {
            item.color = (BYTE)(m_ptCurSpPalette.x >> 2);
            bUpdate = true;
        }
    }

    for (auto& item : m_CursorTileList)
    {
        if (item.flag & TILE_FLAG_CHECKED)
        {
            item.color = (BYTE)(m_ptCurSpPalette.x >> 2);
            bUpdate = true;
        }
    }

    if (m_tileCursor.flag & TILE_FLAG_CHECKED)
    {
        m_tileCursor.color = (BYTE)(m_ptCurSpPalette.x >> 2);
        bUpdate = true;
    }


    if (bUpdate)
    {
        DrawNameTable();
    }

    DrawSpVram();
    DrawSpritePalette();
    UpdateSpVramEdit();
    //调色版更新
    {
        BYTE bData = g_SpritePalette[m_ptCurSpPalette.x];

        m_ptCurPalette.x = bData % 16;
        m_ptCurPalette.y = bData / 16;
        DrawPalette();
    }


    return true;
}

bool CDesignDialog::ClickBkgChrEdit(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_wndBkgTileEdit, 128, 128, 8, 8, pt, ptOffset))
    {
        return false;
    }

    //当前修改点
    m_ptCurBkgTileEdit = pt;

    //得到当前修改点的颜色
    DWORD dwColor = m_ptCurBkgPalette.x % 4;

    //取出当前Tile数据
    DWORD dwDataPos = ((m_ptCurBkgTile.y) << 4 | m_ptCurBkgTile.x) * 0x10;
    LPBYTE pDataL = &g_ChrRam[dwDataPos];
    LPBYTE pDataH = &g_ChrRam[dwDataPos + 8];

    BYTE dataBit = 0x80 >> m_ptCurBkgTileEdit.x;
    BYTE colorBitL = ((dwColor & 0x01) << 7) >> m_ptCurBkgTileEdit.x;
    BYTE colorBitH = ((dwColor & 0x02) << 6) >> m_ptCurBkgTileEdit.x;

    //修改颜色
    pDataL[m_ptCurBkgTileEdit.y] = (pDataL[m_ptCurBkgTileEdit.y] & ~dataBit) | colorBitL;
    pDataH[m_ptCurBkgTileEdit.y] = (pDataH[m_ptCurBkgTileEdit.y] & ~dataBit) | colorBitH;

    DrawTile(0, 4, 0, 0);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();
    return true;
}

bool CDesignDialog::ClickSpriteChrEdit(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_wndSpTileEdit, 128, 128, 8, 8, pt, ptOffset))
    {
        return false;
    }


    //当前修改点
    m_ptCurSpTileEdit = pt;

    //得到当前修改点的颜色
    DWORD dwColor = m_ptCurSpPalette.x % 4;

    //取出当前Tile数据
    DWORD dwDataPos = ((m_ptCurSpTile.y) << 4 | m_ptCurSpTile.x) * 0x10;
    LPBYTE pDataL = &g_ChrRam[0x1000 + dwDataPos];
    LPBYTE pDataH = &g_ChrRam[0x1000 + dwDataPos + 8];

    BYTE dataBit = 0x80 >> m_ptCurSpTileEdit.x;
    BYTE colorBitL = ((dwColor & 0x01) << 7) >> m_ptCurSpTileEdit.x;
    BYTE colorBitH = ((dwColor & 0x02) << 6) >> m_ptCurSpTileEdit.x;

    //修改颜色
    pDataL[m_ptCurSpTileEdit.y] = (pDataL[m_ptCurSpTileEdit.y] & ~dataBit) | colorBitL;
    pDataH[m_ptCurSpTileEdit.y] = (pDataH[m_ptCurSpTileEdit.y] & ~dataBit) | colorBitH;

    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return true;
}

bool CDesignDialog::ClickPalette(POINT pt)
{
    POINT ptOffset = { 0 };
    if (!GetGridClick(m_hWnd, m_hWndPalette, 256, 64, 16, 4, pt, ptOffset))
    {
        return false;
    }

    static clock_t lastTime = 0;
    clock_t curTime = clock();

    if (curTime - lastTime < 20)
    {
        return true;
    }

    lastTime = curTime;

    //调色板
    {
        m_ptCurPalette = pt;
        DrawPalette();
    }


    if (!m_SelSpPalette)
    {
        //背景调色板
        {
            g_BkgPalette[m_ptCurBkgPalette.x] = (BYTE)(pt.y * 16 + pt.x);
            DrawBkgPalette();
        }

        //属性
        {
            DrawAttribute();
        }

        DrawTile(m_ptCurBkgPalette.x >> 2, 1, 0, 0);
        InvalidateRect(m_wndTileDraw, nullptr, FALSE);

        DrawBkgVram();
        UpdateBkgVramEdit();
        InvalidateRect(m_wndBkgTile, nullptr, FALSE);
    }
    else
    {
        //精灵调色板
        {
            g_SpritePalette[m_ptCurSpPalette.x] = (BYTE)(pt.y * 16 + pt.x);
            DrawSpritePalette();
        }

        DrawTile(0, 0, m_ptCurSpPalette.x >> 2, 1);
        InvalidateRect(m_wndTileDraw, nullptr, FALSE);

        DrawSpVram();
        UpdateSpVramEdit();
        InvalidateRect(m_wndSpTile, nullptr, FALSE);
    }

    //命名表
    {
        DrawNameTable();
    }

    return true;
}

DWORD CDesignDialog::GetBkgColor()
{
    return g_PaletteColor[g_SpritePalette[0]];
}

DWORD CDesignDialog::GetTileBkgColor(const TILE_INFO& tile)
{
    return g_PaletteColor[g_SpritePalette[tile.color << 2]];
}

DWORD CDesignDialog::GetNameTableColorIndex(POINT pt)
{
    //取出数据位置
    BYTE data = g_AttributeData[((pt.y >> 2) * 8) + (pt.x >> 2)];

    DWORD dwLTIndex = (data & 0x03) >> 0;
    DWORD dwRTIndex = (data & 0x0C) >> 2;
    DWORD dwLBIndex = (data & 0x30) >> 4;
    DWORD dwRBIndex = (data & 0xC0) >> 6;
    if (pt.x % 4 < 2)
    {
        if (pt.y % 4 < 2)
        {
            return dwLTIndex;
        }
        else
        {
            return dwLBIndex;
        }
    }
    else
    {
        if (pt.y % 4 < 2)
        {
            return dwRTIndex;
        }
        else
        {
            return dwRBIndex;
        }
    }
}

DWORD CDesignDialog::GetNameTableColor(POINT pt)
{
    //取出数据位置
    BYTE data = g_AttributeData[((pt.y >> 2) * 8) + (pt.x >> 2)];

    DWORD dwLTIndex = (data & 0x03) >> 0;
    DWORD dwRTIndex = (data & 0x0C) >> 2;
    DWORD dwLBIndex = (data & 0x30) >> 4;
    DWORD dwRBIndex = (data & 0xC0) >> 6;

    DWORD dwLT = g_PaletteColor[g_BkgPalette[dwLTIndex * 4 + 1]];
    DWORD dwRT = g_PaletteColor[g_BkgPalette[dwRTIndex * 4 + 1]];
    DWORD dwLB = g_PaletteColor[g_BkgPalette[dwLBIndex * 4 + 1]];
    DWORD dwRB = g_PaletteColor[g_BkgPalette[dwRBIndex * 4 + 1]];

    if (pt.x % 4 < 2)
    {
        if (pt.y % 4 < 2)
        {
            return dwLT;
        }
        else
        {
            return dwLB;
        }
    }
    else
    {
        if (pt.y % 4 < 2)
        {
            return dwRT;
        }
        else
        {
            return dwRB;
        }
    }
}

bool CDesignDialog::GetGridClick(
    HWND hParent,
    HWND hWnd,
    float nWidth,
    float nHeight,
    float nRow,
    float nCol,
    POINT& ptCheck,
    POINT& ptOffset
)
{
    if (!::IsWindowEnabled(hWnd))
    {
        return false;
    }

    RECT rtParent = { 0 };
    ::GetClientRect(hParent, &rtParent);
    POINT ptParetn = { rtParent.left, rtParent.top };
    ::ClientToScreen(hParent, &ptParetn);

    RECT rtChild = { 0 };
    ::GetClientRect(hWnd, &rtChild);
    POINT ptChild = { rtChild.left, rtChild.top };
    ::ClientToScreen(hWnd, &ptChild);

    int nOffsetX = ptChild.x - ptParetn.x;
    int nOffsetY = ptChild.y - ptParetn.y;

    ptCheck.x = (ptCheck.x - nOffsetX);
    ptCheck.y = (ptCheck.y - nOffsetY);

    RECT rtClient = { 0 };
    ::GetClientRect(hWnd, &rtClient);

    float nW = (float)(rtClient.right - rtClient.left);
    float nH = (float)(rtClient.bottom - rtClient.top);

    rtClient.left = rtClient.left + (LONG)((nW - nWidth) / 2);
    rtClient.top = rtClient.top + (LONG)((nH - nHeight) / 2);
    rtClient.right = rtClient.left + (LONG)nWidth;
    rtClient.bottom = rtClient.top + (LONG)nHeight;

    if (!::PtInRect(&rtClient, ptCheck))
    {
        return false;
    }

    POINT ptSrc = ptCheck;
    float nSizeX = nWidth / nRow;
    float nSizeY = nHeight / nCol;

    ptCheck.x = (LONG)((float)(ptCheck.x - rtClient.left) / nSizeX);
    ptCheck.y = (LONG)((float)(ptCheck.y - rtClient.top) / nSizeY);

    ptOffset.x = ptSrc.x - (LONG)(nSizeX * ptCheck.x);
    ptOffset.y = ptSrc.y - (LONG)(nSizeY * ptCheck.y);

    return true;
}

POINT CDesignDialog::GetClientOffset(HWND hFirst, HWND hSecond)
{
    POINT pt = { 0 };
    RECT rtFirst = { 0 };
    ::GetClientRect(hFirst, &rtFirst);
    POINT ptFirst = { rtFirst.left, rtFirst.top };
    ::ClientToScreen(hFirst, &ptFirst);

    RECT rtSecond = { 0 };
    ::GetClientRect(hSecond, &rtSecond);
    POINT ptSecond = { rtSecond.left, rtSecond.top };
    ::ClientToScreen(hSecond, &ptSecond);

    int nOffsetX = ptSecond.x - ptFirst.x;
    int nOffsetY = ptSecond.y - ptFirst.y;

    return { nOffsetX , nOffsetY };
}

bool CDesignDialog::SetBitmap32Data(LPDWORD lpData, DWORD dwSize, DWORD dwWidth, DWORD dwHeight, DWORD dwX, DWORD dwY, DWORD dwColor)
{
    DWORD dwIndex = dwWidth * dwY + dwX;
    if (dwIndex >= dwSize)
    {
        return false;
    }

    lpData[dwIndex] = dwColor;
    return true;
}

void CDesignDialog::MoveTilePixel(LPBYTE lpData, eMoveTilePixelType nType/* = eMoveTilePixelType::eMtpLeft*/)
{
    BYTE dataColor[0x10] = { 0 };

    //拷贝Tile数据
    memcpy_s(dataColor, 0x10, lpData, sizeof(dataColor));

    if (nType == eMoveTilePixelType::eMtpLeft)
    {
        for (int y = 0; y < 8; y++)
        {
            BYTE colorH = dataColor[y + 8];
            BYTE colorL = dataColor[y];

            colorH = colorH << 1;
            colorL = colorL << 1;

            dataColor[y + 8] = colorH;
            dataColor[y] = colorL;
        }
    }

    if (nType == eMoveTilePixelType::eMtpRight)
    {
        for (int y = 0; y < 8; y++)
        {
            BYTE colorH = dataColor[y + 8];
            BYTE colorL = dataColor[y];

            colorH = colorH >> 1;
            colorL = colorL >> 1;

            dataColor[y + 8] = colorH;
            dataColor[y] = colorL;
        }
    }

    if (nType == eMoveTilePixelType::eMtpUp)
    {
        memmove_s(&dataColor[0], 7, &dataColor[1], 7);
        memmove_s(&dataColor[8], 7, &dataColor[9], 7);
        dataColor[7] = 0;
        dataColor[15] = 0;
    }

    if (nType == eMoveTilePixelType::eMtpDown)
    {
        memmove_s(&dataColor[1], 7, &dataColor[0], 7);
        memmove_s(&dataColor[9], 7, &dataColor[8], 7);

        dataColor[0] = 0;
        dataColor[8] = 0;
    }

    //拷贝回去
    memcpy_s(lpData, 0x10, dataColor, sizeof(dataColor));
}

void CDesignDialog::ResetBkgChecked()
{
    memset(m_bBkgTileCheck, 0, sizeof(m_bBkgTileCheck));
}

void CDesignDialog::ResetSpChecked()
{
    memset(m_bSpTileCheck, 0, sizeof(m_bSpTileCheck));
}

void CDesignDialog::SwapTileColor(LPBYTE lpData, BYTE c1, BYTE c2)
{
    BYTE dataColor[0x10] = { 0 };

    //拷贝Tile数据
    memcpy_s(dataColor, 0x10, lpData, sizeof(dataColor));

    //每一行颜色提取
    for (int y = 0; y < 8; y++)
    {
        //提取颜色
        BYTE colorH = dataColor[y + 8];
        BYTE colorL = dataColor[y];

        BYTE color0 = ((colorH & 0x80) >> 6) | ((colorL & 0x80) >> 7);
        BYTE color1 = ((colorH & 0x40) >> 5) | ((colorL & 0x40) >> 6);
        BYTE color2 = ((colorH & 0x20) >> 4) | ((colorL & 0x20) >> 5);
        BYTE color3 = ((colorH & 0x10) >> 3) | ((colorL & 0x10) >> 4);
        BYTE color4 = ((colorH & 0x08) >> 2) | ((colorL & 0x08) >> 3);
        BYTE color5 = ((colorH & 0x04) >> 1) | ((colorL & 0x04) >> 2);
        BYTE color6 = ((colorH & 0x02) >> 0) | ((colorL & 0x02) >> 1);
        BYTE color7 = ((colorH & 0x01) << 1) | ((colorL & 0x01) >> 0);

        if (color0 == c1)
        {
            color0 = c2;
        }
        else if (color0 == c2)
        {
            color0 = c1;
        }

        if (color1 == c1)
        {
            color1 = c2;
        }
        else if (color1 == c2)
        {
            color1 = c1;
        }

        if (color2 == c1)
        {
            color2 = c2;
        }
        else if (color2 == c2)
        {
            color2 = c1;
        }

        if (color3 == c1)
        {
            color3 = c2;
        }
        else if (color3 == c2)
        {
            color3 = c1;
        }

        if (color4 == c1)
        {
            color4 = c2;
        }
        else if (color4 == c2)
        {
            color4 = c1;
        }

        if (color5 == c1)
        {
            color5 = c2;
        }
        else if (color5 == c2)
        {
            color5 = c1;
        }

        if (color6 == c1)
        {
            color6 = c2;
        }
        else if (color6 == c2)
        {
            color6 = c1;
        }

        if (color7 == c1)
        {
            color7 = c2;
        }
        else if (color7 == c2)
        {
            color7 = c1;
        }

        colorH = 0;
        colorL = 0;

        colorH |= ((color0 & 0x02) << 6);
        colorL |= (color0 & 0x01) << 7;

        colorH |= ((color1 & 0x02) << 5);
        colorL |= (color1 & 0x01) << 6;

        colorH |= ((color2 & 0x02) << 4);
        colorL |= (color2 & 0x01) << 5;

        colorH |= ((color3 & 0x02) << 3);
        colorL |= (color3 & 0x01) << 4;

        colorH |= ((color4 & 0x02) << 2);
        colorL |= (color4 & 0x01) << 3;

        colorH |= ((color5 & 0x02) << 1);
        colorL |= (color5 & 0x01) << 2;

        colorH |= ((color6 & 0x02) << 0);
        colorL |= (color6 & 0x01) << 1;

        colorH |= ((color7 & 0x02) >> 1);
        colorL |= (color7 & 0x01) << 0;

        dataColor[y + 8] = colorH;
        dataColor[y] = colorL;
    }

    //拷贝回去
    memcpy_s(lpData, 0x10, dataColor, sizeof(dataColor));
}

void CDesignDialog::MoveTileColor123(LPBYTE lpData, bool bRight)
{
    BYTE dataColor[0x10] = { 0 };

    //拷贝Tile数据
    memcpy_s(dataColor, 0x10, lpData, sizeof(dataColor));

    //每一行颜色提取
    for (int y = 0; y < 8; y++)
    {
        //提取颜色
        BYTE colorH = dataColor[y + 8];
        BYTE colorL = dataColor[y];

        BYTE color0 = ((colorH & 0x80) >> 6) | ((colorL & 0x80) >> 7);
        BYTE color1 = ((colorH & 0x40) >> 5) | ((colorL & 0x40) >> 6);
        BYTE color2 = ((colorH & 0x20) >> 4) | ((colorL & 0x20) >> 5);
        BYTE color3 = ((colorH & 0x10) >> 3) | ((colorL & 0x10) >> 4);
        BYTE color4 = ((colorH & 0x08) >> 2) | ((colorL & 0x08) >> 3);
        BYTE color5 = ((colorH & 0x04) >> 1) | ((colorL & 0x04) >> 2);
        BYTE color6 = ((colorH & 0x02) >> 0) | ((colorL & 0x02) >> 1);
        BYTE color7 = ((colorH & 0x01) << 1) | ((colorL & 0x01) >> 0);

        if (bRight)
        {
            if (0 != color0)
            {
                color0++;
                color0 = color0 == 4 ? 1 : color0;
            }
            if (0 != color1)
            {
                color1++;
                color1 = color1 == 4 ? 1 : color1;
            }
            if (0 != color2)
            {
                color2++;
                color2 = color2 == 4 ? 1 : color2;
            }
            if (0 != color3)
            {
                color3++;
                color3 = color3 == 4 ? 1 : color3;
            }
            if (0 != color4)
            {
                color4++;
                color4 = color4 == 4 ? 1 : color4;
            }
            if (0 != color5)
            {
                color5++;
                color5 = color5 == 4 ? 1 : color5;
            }
            if (0 != color6)
            {
                color6++;
                color6 = color6 == 4 ? 1 : color6;
            }
            if (0 != color7)
            {
                color7++;
                color7 = color7 == 4 ? 1 : color7;
            }
        }
        else
        {
            if (0 != color0)
            {
                color0--;
                color0 = color0 == 0 ? 3 : color0;
            }
            if (0 != color1)
            {
                color1--;
                color1 = color1 == 0 ? 3 : color1;
            }
            if (0 != color2)
            {
                color2--;
                color2 = color2 == 0 ? 3 : color2;
            }
            if (0 != color3)
            {
                color3--;
                color3 = color3 == 0 ? 3 : color3;
            }
            if (0 != color4)
            {
                color4--;
                color4 = color4 == 0 ? 3 : color4;
            }
            if (0 != color5)
            {
                color5--;
                color5 = color5 == 0 ? 3 : color5;
            }
            if (0 != color6)
            {
                color6--;
                color6 = color6 == 0 ? 3 : color6;
            }
            if (0 != color7)
            {
                color7--;
                color7 = color7 == 0 ? 3 : color7;
            }
        }

        colorH = 0;
        colorL = 0;
        colorH |= ((color0 & 0x02) << 6);
        colorL |= (color0 & 0x01) << 7;

        colorH |= ((color1 & 0x02) << 5);
        colorL |= (color1 & 0x01) << 6;

        colorH |= ((color2 & 0x02) << 4);
        colorL |= (color2 & 0x01) << 5;

        colorH |= ((color3 & 0x02) << 3);
        colorL |= (color3 & 0x01) << 4;

        colorH |= ((color4 & 0x02) << 2);
        colorL |= (color4 & 0x01) << 3;

        colorH |= ((color5 & 0x02) << 1);
        colorL |= (color5 & 0x01) << 2;

        colorH |= ((color6 & 0x02) << 0);
        colorL |= (color6 & 0x01) << 1;

        colorH |= ((color7 & 0x02) >> 1);
        colorL |= (color7 & 0x01) << 0;

        dataColor[y + 8] = colorH;
        dataColor[y] = colorL;
    }

    //拷贝回去
    memcpy_s(lpData, 0x10, dataColor, sizeof(dataColor));
}

void CDesignDialog::MoveTileColor0123(LPBYTE lpData, bool bRight)
{
    BYTE dataColor[0x10] = { 0 };

    //拷贝Tile数据
    memcpy_s(dataColor, 0x10, lpData, sizeof(dataColor));

    //每一行颜色提取
    for (int y = 0; y < 8; y++)
    {
        //提取颜色
        BYTE colorH = dataColor[y + 8];
        BYTE colorL = dataColor[y];

        BYTE color0 = ((colorH & 0x80) >> 6) | ((colorL & 0x80) >> 7);
        BYTE color1 = ((colorH & 0x40) >> 5) | ((colorL & 0x40) >> 6);
        BYTE color2 = ((colorH & 0x20) >> 4) | ((colorL & 0x20) >> 5);
        BYTE color3 = ((colorH & 0x10) >> 3) | ((colorL & 0x10) >> 4);
        BYTE color4 = ((colorH & 0x08) >> 2) | ((colorL & 0x08) >> 3);
        BYTE color5 = ((colorH & 0x04) >> 1) | ((colorL & 0x04) >> 2);
        BYTE color6 = ((colorH & 0x02) >> 0) | ((colorL & 0x02) >> 1);
        BYTE color7 = ((colorH & 0x01) << 1) | ((colorL & 0x01) >> 0);

        if (bRight)
        {
            color0 = (color0 + 1) & 0x03;
            color1 = (color1 + 1) & 0x03;
            color2 = (color2 + 1) & 0x03;
            color3 = (color3 + 1) & 0x03;
            color4 = (color4 + 1) & 0x03;
            color5 = (color5 + 1) & 0x03;
            color6 = (color6 + 1) & 0x03;
            color7 = (color7 + 1) & 0x03;
        }
        else
        {
            color0 = (color0 - 1) & 0x03;
            color1 = (color1 - 1) & 0x03;
            color2 = (color2 - 1) & 0x03;
            color3 = (color3 - 1) & 0x03;
            color4 = (color4 - 1) & 0x03;
            color5 = (color5 - 1) & 0x03;
            color6 = (color6 - 1) & 0x03;
            color7 = (color7 - 1) & 0x03;
        }

        colorH = 0;
        colorL = 0;
        colorH |= ((color0 & 0x02) << 6);
        colorL |= (color0 & 0x01) << 7;

        colorH |= ((color1 & 0x02) << 5);
        colorL |= (color1 & 0x01) << 6;

        colorH |= ((color2 & 0x02) << 4);
        colorL |= (color2 & 0x01) << 5;

        colorH |= ((color3 & 0x02) << 3);
        colorL |= (color3 & 0x01) << 4;

        colorH |= ((color4 & 0x02) << 2);
        colorL |= (color4 & 0x01) << 3;

        colorH |= ((color5 & 0x02) << 1);
        colorL |= (color5 & 0x01) << 2;

        colorH |= ((color6 & 0x02) << 0);
        colorL |= (color6 & 0x01) << 1;

        colorH |= ((color7 & 0x02) >> 1);
        colorL |= (color7 & 0x01) << 0;

        dataColor[y + 8] = colorH;
        dataColor[y] = colorL;
    }

    //拷贝回去
    memcpy_s(lpData, 0x10, dataColor, sizeof(dataColor));
}

void CDesignDialog::DeleteTileColor(LPBYTE lpData, BYTE dwColorIndex)
{
    BYTE dataColor[0x10] = { 0 };

    //拷贝Tile数据
    memcpy_s(dataColor, 0x10, lpData, sizeof(dataColor));

    //每一行颜色提取
    for (int y = 0; y < 8; y++)
    {
        //提取颜色
        BYTE colorH = dataColor[y + 8];
        BYTE colorL = dataColor[y];

        BYTE color0 = ((colorH & 0x80) >> 6) | ((colorL & 0x80) >> 7);
        BYTE color1 = ((colorH & 0x40) >> 5) | ((colorL & 0x40) >> 6);
        BYTE color2 = ((colorH & 0x20) >> 4) | ((colorL & 0x20) >> 5);
        BYTE color3 = ((colorH & 0x10) >> 3) | ((colorL & 0x10) >> 4);
        BYTE color4 = ((colorH & 0x08) >> 2) | ((colorL & 0x08) >> 3);
        BYTE color5 = ((colorH & 0x04) >> 1) | ((colorL & 0x04) >> 2);
        BYTE color6 = ((colorH & 0x02) >> 0) | ((colorL & 0x02) >> 1);
        BYTE color7 = ((colorH & 0x01) << 1) | ((colorL & 0x01) >> 0);

        if (color0 == dwColorIndex)
        {
            colorH = (colorH & ~0x80);
            colorL = (colorL & ~0x80);
        }
        if (color1 == dwColorIndex)
        {
            colorH = (colorH & ~0x40);
            colorL = (colorL & ~0x40);
        }
        if (color2 == dwColorIndex)
        {
            colorH = (colorH & ~0x20);
            colorL = (colorL & ~0x20);
        }
        if (color3 == dwColorIndex)
        {
            colorH = (colorH & ~0x10);
            colorL = (colorL & ~0x10);
        }
        if (color4 == dwColorIndex)
        {
            colorH = (colorH & ~0x08);
            colorL = (colorL & ~0x08);
        }
        if (color5 == dwColorIndex)
        {
            colorH = (colorH & ~0x04);
            colorL = (colorL & ~0x04);
        }
        if (color6 == dwColorIndex)
        {
            colorH = (colorH & ~0x02);
            colorL = (colorL & ~0x02);
        }
        if (color7 == dwColorIndex)
        {
            colorH = (colorH & ~0x01);
            colorL = (colorL & ~0x01);
        }
        dataColor[y + 8] = colorH;
        dataColor[y] = colorL;
    }

    //拷贝回去
    memcpy_s(lpData, 0x10, dataColor, sizeof(dataColor));
}

void CDesignDialog::ClickByFastBkgList(POINT ptMouse)
{
    TILE_INFO info;
    info.id = (DWORD)m_BkgTileIdCount++;
    info.data = (BYTE)(m_ptCurBkgTile.y * 0x10) + (BYTE)m_ptCurBkgTile.x;
    info.pt = m_ptNameTableTile;
    m_BkgTileList.push_back(info);

    if (m_ptNameTableTile.y * 30 + m_ptNameTableTile.x < 959)
    {
        if (31 == m_ptNameTableTile.x)
        {
            m_ptNameTableTile.y++;
            m_ptNameTableTile.x = 0;
        }
        else
        {
            m_ptNameTableTile.x++;
        }
    }

    {
        POINT pt = m_ptNameTableTile;
        m_ptCurNameTable = { pt.x / 4, pt.y / 4 };
        m_ptCurNameTableAttr = { pt.x / 2, pt.y / 2 };

        UpdateAttribute();
    }

    DrawBkgVram();
    DrawAttribute();
    DrawNameTable();
}

void CDesignDialog::ClickByFastSpList(POINT ptMouse)
{
    TILE_INFO info;
    info.id = (DWORD)m_SpTileIdCount++;
    info.data = (BYTE)(m_ptCurSpTile.y * 0x10) + (BYTE)m_ptCurSpTile.x;

    POINT ptSprite = m_ptNameTableTile;
    ptSprite.x *= 8;
    ptSprite.y *= 8;
    info.pt = ptSprite;
    m_SpTileList.push_back(info);

    if (m_ptNameTableTile.y * 30 + m_ptNameTableTile.x < 959)
    {
        if (31 == m_ptNameTableTile.x)
        {
            m_ptNameTableTile.y++;
            m_ptNameTableTile.x = 0;
        }
        else
        {
            m_ptNameTableTile.x++;
        }
    }

    {
        POINT pt = m_ptNameTableTile;
        m_ptCurNameTable = { pt.x / 4, pt.y / 4 };
        m_ptCurNameTableAttr = { pt.x / 2, pt.y / 2 };

        UpdateAttribute();
    }

    DrawSpVram();
    DrawAttribute();
    DrawNameTable();
}

void CDesignDialog::ClickByFastBkgChr(POINT ptMouse)
{
    DrawCharMappingSource();
    DrawCharMappingTarget();

    LPBYTE lpAddrSrc = &g_ChrRamDefault[(m_ptCharMapDest.x + 0x20) * 0x10];
    LPBYTE lpAddrDest = &g_ChrRam[(m_ptCurBkgTile.y * 0x10 + m_ptCurBkgTile.x) * 0x10];

    //拷贝到目的数据
    memcpy_s(lpAddrDest, 0x10, lpAddrSrc, 0x10);

    if (m_ptCurBkgTile.y * 16 + m_ptCurBkgTile.x < 255)
    {
        if (15 == m_ptCurBkgTile.x)
        {
            m_ptCurBkgTile.y++;
            m_ptCurBkgTile.x = 0;
        }
        else
        {
            m_ptCurBkgTile.x++;
        }
    }

    m_SelSpPalette = false;
    DrawTile(0, 4, 0, 0);
    DrawBkgVram();
    DrawBkgPalette();
    DrawSpritePalette();
    UpdateBkgVramEdit();
    DrawNameTable();
}

void CDesignDialog::ClickByFastSpChr(POINT ptMouse)
{
    DrawCharMappingSource();
    DrawCharMappingTarget();

    LPBYTE lpAddrSrc = &g_ChrRamDefault[(m_ptCharMapDest.x + 0x20) * 0x10];
    LPBYTE lpAddrDest = &g_ChrRam[0x1000 + (m_ptCurSpTile.y * 0x10 + m_ptCurSpTile.x) * 0x10];

    //拷贝到目的数据
    memcpy_s(lpAddrDest, 0x10, lpAddrSrc, 0x10);

    if (m_ptCurSpTile.y * 16 + m_ptCurSpTile.x < 255)
    {
        if (15 == m_ptCurSpTile.x)
        {
            m_ptCurSpTile.y++;
            m_ptCurSpTile.x = 0;
        }
        else
        {
            m_ptCurSpTile.x++;
        }
    }

    m_SelSpPalette = false;
    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    DrawBkgPalette();
    DrawSpritePalette();
    UpdateSpVramEdit();
    DrawNameTable();
}

void CDesignDialog::ClickByFastCharMapping(POINT ptMouse)
{
    POINT pt = ptMouse;
    POINT ptOffset = { 0 };
    if (GetGridClick(m_hWnd, m_wndBkgTile, 256, 256, 16, 16, pt, ptOffset))
    {
        m_ptCurBkgTile = pt;

        g_CharMapData[0x20 + m_ptCharMapSrc.x] = (BYTE)(pt.y * 0x10 + pt.x);

        if (m_ptCharMapSrc.x + 1 < 0x40)
        {
            m_ptCharMapSrc.x++;
        }

        DrawBkgVram();
        DrawCharMappingSource();
    }

    DrawNameTable();
}

void CDesignDialog::DeleteCheckedBkgTileItem()
{
    for (auto it = m_SpTileList.begin(); it != m_SpTileList.end();)
    {
        if (it->flag & 0x01)
        {
            it = m_SpTileList.erase(it);
            continue;
        }

        it++;
    }

    for (auto it = m_BkgTileList.begin(); it != m_BkgTileList.end();)
    {
        if (it->flag & 0x01)
        {
            it = m_BkgTileList.erase(it);
            continue;
        }

        it++;
    }

}

void CDesignDialog::DeleteCheckedBkgVramTile()
{
    BYTE vramEmpty[0x10] = { 0 };
    LPBYTE lpBase = &g_ChrRam[0x0000];

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            int nIndex = y * 16 + x;
            if (m_bBkgTileCheck[nIndex])
            {
                LPBYTE lpData = lpBase + (nIndex * 0x10);
                memcpy_s(lpData, 0x10, vramEmpty, sizeof(vramEmpty));

                m_bBkgTileCheck[nIndex] = false;
            }
        }

        OutputDebugString(_T("\r\n"));
    }

    DrawTile();
    DrawBkgVram();
}

void CDesignDialog::DeleteCheckedSpVramTile()
{
    BYTE vramEmpty[0x10] = { 0 };
    LPBYTE lpBase = &g_ChrRam[0x1000];

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            int nIndex = y * 16 + x;
            if (m_bSpTileCheck[nIndex])
            {
                LPBYTE lpData = lpBase + (nIndex * 0x10);
                memcpy_s(lpData, 0x10, vramEmpty, sizeof(vramEmpty));
                m_bSpTileCheck[nIndex] = false;
            }
        }
    }

    DrawTile();
    DrawSpVram();
}

void CDesignDialog::DeleteCheckedTile()
{
    RecordHistoryData(true);

    DeleteCheckedBkgTileItem();
    DeleteCheckedBkgVramTile();
    DeleteCheckedSpVramTile();

    DrawNameTable();

    UpdateHistoryData(true);
}

void CDesignDialog::CheckSelectedNameTableTile(POINT ptLT, POINT ptRB)
{
    CBitmapDraw::AdjustPiont(ptLT, ptRB);
    RECT rtCheck = { ptLT.x , ptLT.y, ptRB.x , ptRB.y };

    bool bCtrlDown = false;
    if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
    {
        bCtrlDown = true;
    }

    if (m_bShowBgTile)
    {
        for (auto& item : m_BkgTileList)
        {
            RECT rtTile = { item.pt.x * 16, item.pt.y * 16, (item.pt.x + 1) * 16, (item.pt.y + 1) * 16 };
            RECT rtOut = { 0 };
            if (::IntersectRect(&rtOut, &rtCheck, &rtTile))
            {
                item.flag |= TILE_FLAG_CHECKED;
            }
            else
            {
                if (!bCtrlDown)
                {
                    item.flag &= ~TILE_FLAG_CHECKED;
                }
            }
        }
    }

    if (m_bShowSpTile)
    {
        for (auto& item : m_SpTileList)
        {
            RECT rtTile = { item.pt.x * 2, item.pt.y * 2, (item.pt.x) * 2 + 16, (item.pt.y) * 2 + 16 };
            RECT rtOut = { 0 };
            if (::IntersectRect(&rtOut, &rtCheck, &rtTile))
            {
                item.flag |= TILE_FLAG_CHECKED;
            }
            else
            {
                if (!bCtrlDown)
                {
                    item.flag &= ~TILE_FLAG_CHECKED;
                }
            }
        }
    }

    CheckSpriteAttributeButton();
}

void CDesignDialog::CheckSelectedBkgTile(POINT ptLT, POINT ptRB)
{
    CBitmapDraw::AdjustPiont(ptLT, ptRB);
    RECT rtCheck = { ptLT.x , ptLT.y, ptRB.x , ptRB.y };

    bool bCtrlDown = false;
    if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
    {
        bCtrlDown = true;
    }

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            RECT rtTile = { x * 16, y * 16 , (x + 1) * 16, (y + 1) * 16 };
            RECT rtOut = { 0 };
            if (::IntersectRect(&rtOut, &rtCheck, &rtTile))
            {
                m_bBkgTileCheck[y * 16 + x] = true;
            }
            else
            {
                if (!bCtrlDown)
                {
                    m_bBkgTileCheck[y * 16 + x] = false;
                }
            }
        }
    }
}

void CDesignDialog::CheckSelectedSpTile(POINT ptLT, POINT ptRB)
{
    CBitmapDraw::AdjustPiont(ptLT, ptRB);
    RECT rtCheck = { ptLT.x , ptLT.y, ptRB.x , ptRB.y };

    bool bCtrlDown = false;
    if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
    {
        bCtrlDown = true;
    }

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            RECT rtTile = { x * 16, y * 16 , (x + 1) * 16, (y + 1) * 16 };
            RECT rtOut = { 0 };
            if (::IntersectRect(&rtOut, &rtCheck, &rtTile))
            {
                m_bSpTileCheck[y * 16 + x] = true;
            }
            else
            {
                if (!bCtrlDown)
                {
                    m_bSpTileCheck[y * 16 + x] = false;
                }
            }
        }
    }
}

void CDesignDialog::UpdateAttribute()
{
    //命名表属性更新
    {
        int nIndex = (m_ptCurNameTable.y) * 8 + (m_ptCurNameTable.x);
        if (nIndex < _countof(g_AttributeData))
        {
            m_AttributeData = g_AttributeData[nIndex];
        }
    }

    //属性表更新
    {
        m_ptCurAttribute.x = m_ptCurNameTableAttr.x % 2;
        m_ptCurAttribute.y = m_ptCurNameTableAttr.y % 2;
    }

    //背景调色版更新
    {
        int nIndex = (m_ptCurAttribute.y * 2 + m_ptCurAttribute.x);
        if (0 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0x03) >> 0) * 4 + 1;
        }
        else if (1 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0x0C) >> 2) * 4 + 1;
        }
        else if (2 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0x30) >> 4) * 4 + 1;
        }
        else if (3 == nIndex)
        {
            m_ptCurBkgPalette.x = ((m_AttributeData & 0xC0) >> 6) * 4 + 1;
        }

        m_ptLastBkgPal = { -1,-1 };
        m_ptLastSpPal = { -1,-1 };
    }

    //调色版更新
    {
        BYTE bData = g_BkgPalette[m_ptCurBkgPalette.x];
        m_ptCurPalette.x = bData % 16;
        m_ptCurPalette.y = bData / 16;
    }
}

void CDesignDialog::SetLoadData(CByteBuffer& data)
{
    CByteBuffer& cBuffer = data;

    m_CustomTextList.clear();
    m_BkgTileList.clear();
    m_SpTileList.clear();
    m_CursorTileList.clear();
    m_CustomTextCount = 0;
    m_SpTileIdCount = 0;
    m_BkgTileIdCount = 0;

    if (nullptr == cBuffer.GetBuffer())
    {
        return;
    }

    PCUSTOM_MENU_DATA lpCustomData = nullptr;
    PCUSTOM_MENU_DATA lpRootData = (PCUSTOM_MENU_DATA)cBuffer.GetBuffer();
    if (eMenuDataType::eMDTHistoryRoot != lpRootData->dwType)
    {
        return;
    }

    unsigned long long ullSize = lpRootData->dwElementSize;
    LPBYTE lpDataGebin = lpRootData->data;

    //读取解析
    lpCustomData = (PCUSTOM_MENU_DATA)lpDataGebin;

    while ((LPBYTE)lpCustomData < lpDataGebin + ullSize)
    {
        DWORD dwType = lpCustomData->dwType;
        LPBYTE lpData = lpCustomData->data;
        DWORD dwElementSize = lpCustomData->dwElementSize;

        if (eMenuDataType::eMDTAttributes == dwType)
        {
            memcpy_s(g_AttributeData, sizeof(g_AttributeData), lpData, dwElementSize);
        }

        if (eMenuDataType::eMDTBkgPalette == dwType)
        {
            memcpy_s(g_BkgPalette, sizeof(g_BkgPalette), lpData, dwElementSize);
        }

        if (eMenuDataType::eMDTSpritePalette == dwType)
        {
            memcpy_s(g_SpritePalette, sizeof(g_SpritePalette), lpData, dwElementSize);
        }

        if (eMenuDataType::eMDTCaptionText == dwType)
        {
            TCHAR szBuf[MAX_PATH] = { 0 };
            memcpy_s(szBuf, sizeof(szBuf), lpData, dwElementSize);
            m_strCaption = szBuf;
            SetItemString(IDC_EDIT_CAPTION_TEXT, m_strCaption);
        }

        if (eMenuDataType::eMDTCaptionPos == dwType)
        {
            m_ptCaption = *(POINT*)lpData;
        }

        if (eMenuDataType::eMDTPageNumberPos == dwType)
        {
            m_ptPageNumber = *(POINT*)lpData;
        }

        if (eMenuDataType::eMDTItemPos == dwType)
        {
            m_ptItem = *(POINT*)lpData;
        }

        if (eMenuDataType::eMDTCursorTile == dwType)
        {
            m_tileCursor = *(PTILE_INFO)lpData;
        }

        if (eMenuDataType::eMDTChrData == dwType)
        {
            memcpy_s(g_ChrRam, sizeof(g_ChrRam), lpData, dwElementSize);
        }

        if (eMenuDataType::eMDTCustomText == dwType)
        {
            PCUSTOM_TEXT lpCustomText = (PCUSTOM_TEXT)lpData;

            TEXT_INFO info;
            info.id = (DWORD)m_CustomTextCount++;
            info.pt = lpCustomText->pt;
            TCHAR szBuf[MAX_PATH] = { 0 };
            memcpy_s(szBuf, sizeof(szBuf), lpCustomText->data, lpCustomText->dwSize);
            info.strText = szBuf;

            m_CustomTextList.push_back(info);
        }

        if (eMenuDataType::eMDTBkgTile == dwType)
        {
            TILE_INFO info;
            memcpy_s(&info, sizeof(info), lpData, dwElementSize);
            info.id = (DWORD)m_BkgTileIdCount++;
            m_BkgTileList.push_back(info);
        }

        if (eMenuDataType::eMDTSpriteTile == dwType)
        {
            TILE_INFO info;
            memcpy_s(&info, sizeof(info), lpData, dwElementSize);
            info.id = (DWORD)m_SpTileIdCount++;
            m_SpTileList.push_back(info);
        }

        if (eMenuDataType::eMDTCursorTileList == dwType)
        {
            TILE_INFO info;
            memcpy_s(&info, sizeof(info), lpData, dwElementSize);
            m_CursorTileList.push_back(info);
        }

        if (eMenuDataType::eMDTHistoryNtPos == dwType) memcpy_s(&m_ptCurNameTable, sizeof(m_ptCurAttribute), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistoryNtExPos == dwType)memcpy_s(&m_ptCurNameTableAttr, sizeof(m_ptCurNameTableAttr), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistoryAttrPos == dwType)memcpy_s(&m_ptCurAttribute, sizeof(m_ptCurAttribute), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistoryBkgChrPos == dwType)memcpy_s(&m_ptCurBkgTile, sizeof(m_ptCurBkgTile), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistoryBkgTilePos == dwType)memcpy_s(&m_ptCurBkgTileEdit, sizeof(m_ptCurBkgTileEdit), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistoryBkgPalettePos == dwType)memcpy_s(&m_ptCurBkgPalette, sizeof(m_ptCurBkgPalette), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistorySpChrPos == dwType) memcpy_s(&m_ptCurSpTile, sizeof(m_ptCurSpTile), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistorySpTilePos == dwType) memcpy_s(&m_ptCurSpTileEdit, sizeof(m_ptCurSpTileEdit), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistorySpPalettePos == dwType)memcpy_s(&m_ptCurSpPalette, sizeof(m_ptCurSpPalette), lpData, dwElementSize);
        if (eMenuDataType::eMDTHistoryPalettePos == dwType) memcpy_s(&m_ptCurPalette, sizeof(m_ptCurPalette), lpData, dwElementSize);
        if (eMenuDataType::eMDTCharMap == dwType) memcpy_s(&g_CharMapData, sizeof(g_CharMapData), lpData, dwElementSize);
        if (eMenuDataType::eMDTBigCursor == dwType) memcpy_s(&m_bBigCursor, sizeof(m_bBigCursor), lpData, dwElementSize);
        if (eMenuDataType::eMDTBigSprite == dwType) memcpy_s(&m_bBigSprite, sizeof(m_bBigSprite), lpData, dwElementSize);
        if (eMenuDataType::eMDTCharMapSrcPos == dwType)memcpy_s(&m_ptCharMapSrc, sizeof(m_ptCharMapSrc), lpData, dwElementSize);
        if (eMenuDataType::eMDTBkgChrQuickPos == dwType) memcpy_s(&m_ptCharMapDest, sizeof(m_ptCharMapDest), lpData, dwElementSize);
        if (eMenuDataType::eMDTNTTilePos == dwType) memcpy_s(&m_ptNameTableTile, sizeof(m_ptNameTableTile), lpData, dwElementSize);
        if (eMenuDataType::eMDTBkgTileChecked == dwType) memcpy_s(&m_bBkgTileCheck, sizeof(m_bBkgTileCheck), lpData, dwElementSize);
        if (eMenuDataType::eMDTSpTileChecked == dwType) memcpy_s(&m_bSpTileCheck, sizeof(m_bSpTileCheck), lpData, dwElementSize);

        lpCustomData = (PCUSTOM_MENU_DATA)((LPBYTE)lpCustomData + dwElementSize + sizeof(CUSTOM_MENU_DATA));
    }

    ::CheckDlgButton(m_hWnd, IDC_CHECK_BIG_CURSOR, m_bBigCursor ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_BIG_SPRITE, m_bBigSprite ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_CHAR_MAP_FAST, m_bQuickCharMappingSrc ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_BKG_CHR_FAST, m_bQuickBkgChrDest ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_SP_CHR_FAST, m_bQuickSpChrDest ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_BKG_LIST_FAST, m_bQuickBkgList ? BST_CHECKED : BST_UNCHECKED);

    if (m_CursorTileList.empty())
    {
        ResetCursorTileList();
    }

    POINT ptTemp = m_ptCurBkgPalette;
    UpdateAttribute();
    m_ptCurBkgPalette = ptTemp;

    DrawTile();
    DrawBkgVram();
    DrawSpVram();
    DrawNameTable();
    DrawBkgPalette();
    DrawSpritePalette();
    DrawAttribute();
    DrawPalette();
    DrawCharMappingTarget();
    DrawCharMappingSource();
    UpdateBkgVramEdit();
    UpdateSpVramEdit();
}

CByteBuffer CDesignDialog::GetSaveData()
{
    CByteBuffer tmpData;
    size_t dwTotalSize = 0;
    size_t dwMetaSize = 0;
    LPBYTE lpDataPos = nullptr;

    {
        //背景Tile
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            dwMetaSize += dwDataSize * m_BkgTileList.size();
        }

        //精灵Tile
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            dwMetaSize += dwDataSize * m_SpTileList.size();
        }

        //属性表
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_AttributeData);
            dwMetaSize += dwDataSize;
        }

        //背景调色板
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_BkgPalette);
            dwMetaSize += dwDataSize;
        }

        //精灵调色板
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_SpritePalette);
            dwMetaSize += dwDataSize;
        }

        //标题坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCaption);
            dwMetaSize += dwDataSize;
        }

        //标题文本
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + m_strCaption.size() * sizeof(TCHAR);
            dwMetaSize += dwDataSize;
        }

        //页号坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptPageNumber);
            dwMetaSize += dwDataSize;
        }

        //节目坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptItem);
            dwMetaSize += dwDataSize;
        }

        //箭头坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            dwMetaSize += dwDataSize;
        }

        //箭头列表
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            dwMetaSize += dwDataSize * m_CursorTileList.size();
        }

        //CHR
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_ChrRam);
            dwMetaSize += dwDataSize;
        }

        //自定义文本
        {
            for (auto& item : m_CustomTextList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt);
                dwMetaSize += dwDataSize;
            }
        }

        //命名表焦点焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurNameTable);

        //命名表焦点Ex焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurNameTableAttr);

        //属性焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurAttribute);

        //背景Tile焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurBkgTile);

        //背景Tile编辑焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurBkgTileEdit);

        //背景调色板焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurBkgPalette);

        //精灵Tile焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurSpTile);

        //精灵Tile编辑焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurSpTileEdit);

        //精灵调色板焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurSpPalette);

        //调色板焦点坐标
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurPalette);

        //字符映射
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(g_CharMapData);

        //光标模式
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBigCursor);

        //精灵模式
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBigSprite);

        //字符映射位置
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCharMapSrc);

        //字符映射位置
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCharMapDest);

        //命名表点击位置
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptNameTableTile);

        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBkgTileCheck);
        dwMetaSize += sizeof(CUSTOM_MENU_DATA) + sizeof(m_bSpTileCheck);
    }

    dwTotalSize = dwMetaSize + sizeof(CUSTOM_MENU_DATA);
    tmpData.ResetSize(dwTotalSize);

    PCUSTOM_MENU_DATA lpData = (PCUSTOM_MENU_DATA)tmpData.GetBuffer();
    lpData->dwType = eMenuDataType::eMDTHistoryRoot;
    lpData->dwElementSize = (DWORD)dwMetaSize;
    lpData->dwVersion = 2;
    lpData->dwElementCount = 1;

    lpDataPos = lpData->data;

    //背景Tile
    {
        for (auto item : m_BkgTileList)
        {
            DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
            ZeroMemory(lpCustomData, dwDataSize);
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTBkgTile;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = sizeof(TILE_INFO);
                item.id = 0;
                memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));
                lpDataPos += dwDataSize;
            }
        }
    }

    //精灵Tile
    {
        for (auto item : m_SpTileList)
        {
            DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
            ZeroMemory(lpCustomData, dwDataSize);
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTSpriteTile;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = sizeof(TILE_INFO);
                item.id = 0;
                memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));
                lpDataPos += dwDataSize;
            }
        }
    }

    //属性表
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_AttributeData);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTAttributes;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(g_AttributeData);
            memcpy(lpCustomData->data, g_AttributeData, sizeof(g_AttributeData));
            lpDataPos += dwDataSize;
        }
    }

    //背景调色板
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_BkgPalette);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTBkgPalette;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(g_BkgPalette);
            memcpy(lpCustomData->data, g_BkgPalette, sizeof(g_BkgPalette));
            lpDataPos += dwDataSize;
        }
    }

    //精灵调色板
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_SpritePalette);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTSpritePalette;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(g_SpritePalette);
            memcpy(lpCustomData->data, g_SpritePalette, sizeof(g_SpritePalette));
            lpDataPos += dwDataSize;
        }
    }

    //标题坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCaption);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTCaptionPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCaption);
            memcpy(lpCustomData->data, &m_ptCaption, sizeof(m_ptCaption));
            lpDataPos += dwDataSize;
        }
    }

    //标题文本
    {
        size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + m_strCaption.size() * sizeof(TCHAR);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTCaptionText;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = (DWORD)(m_strCaption.size() * sizeof(TCHAR));
            memcpy(lpCustomData->data, m_strCaption.c_str(), m_strCaption.size() * sizeof(TCHAR));
            lpDataPos += dwDataSize;
        }
    }

    //页号坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptPageNumber);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTPageNumberPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptPageNumber);
            memcpy(lpCustomData->data, &m_ptPageNumber, sizeof(m_ptPageNumber));
            lpDataPos += dwDataSize;
        }
    }

    //节目坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptItem);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTItemPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptItem);
            memcpy(lpCustomData->data, &m_ptItem, sizeof(m_ptItem));
            lpDataPos += dwDataSize;
        }
    }

    //箭头坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTCursorTile;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(TILE_INFO);
            memcpy(lpCustomData->data, &m_tileCursor, sizeof(m_tileCursor));
            lpDataPos += dwDataSize;
        }
    }

    //箭头列表
    for (const auto item : m_CursorTileList)
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTCursorTileList;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(TILE_INFO);
            memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));
            lpDataPos += dwDataSize;
        }
    }

    //CHR
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_ChrRam);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTChrData;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(g_ChrRam);
            memcpy(lpCustomData->data, &g_ChrRam, sizeof(g_ChrRam));
            lpDataPos += dwDataSize;
        }
    }

    //自定义文本
    for (auto item : m_CustomTextList)
    {
        DWORD dwDataSize = (DWORD)(sizeof(CUSTOM_MENU_DATA) + sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt));
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMDTCustomText;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = (DWORD)(sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt));

            PCUSTOM_TEXT pData = (PCUSTOM_TEXT)lpCustomData->data;
            pData->pt = item.pt;
            item.id = 0;
            pData->dwSize = (DWORD)(item.strText.size() * sizeof(TCHAR));
            memcpy(pData->data, item.strText.c_str(), item.strText.size() * sizeof(TCHAR));
            lpDataPos += dwDataSize;
        }
    }

    //命名表焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurNameTable);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryNtPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurNameTable);
            memcpy(lpCustomData->data, &m_ptCurNameTable, sizeof(m_ptCurNameTable));
            lpDataPos += dwDataSize;
        }
    }

    //命名表焦点Ex坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurNameTableAttr);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryNtExPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurNameTableAttr);
            memcpy(lpCustomData->data, &m_ptCurNameTableAttr, sizeof(m_ptCurNameTableAttr));
            lpDataPos += dwDataSize;
        }
    }

    //属性焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurAttribute);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryAttrPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurAttribute);
            memcpy(lpCustomData->data, &m_ptCurAttribute, sizeof(m_ptCurAttribute));
            lpDataPos += dwDataSize;
        }
    }

    //背景Chr焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurBkgTile);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryBkgChrPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurBkgTile);
            memcpy(lpCustomData->data, &m_ptCurBkgTile, sizeof(m_ptCurBkgTile));
            lpDataPos += dwDataSize;
        }
    }

    //背景Tile焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurBkgTileEdit);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryBkgTilePos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurBkgTileEdit);
            memcpy(lpCustomData->data, &m_ptCurBkgTileEdit, sizeof(m_ptCurBkgTileEdit));
            lpDataPos += dwDataSize;
        }
    }

    //背景调色板焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurBkgPalette);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryBkgPalettePos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurBkgPalette);
            memcpy(lpCustomData->data, &m_ptCurBkgPalette, sizeof(m_ptCurBkgPalette));
            lpDataPos += dwDataSize;
        }
    }

    //精灵Chr焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurSpTile);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistorySpChrPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurSpTile);
            memcpy(lpCustomData->data, &m_ptCurSpTile, sizeof(m_ptCurSpTile));
            lpDataPos += dwDataSize;
        }
    }

    //精灵Tile焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurSpTileEdit);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistorySpTilePos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurSpTileEdit);
            memcpy(lpCustomData->data, &m_ptCurSpTileEdit, sizeof(m_ptCurSpTileEdit));
            lpDataPos += dwDataSize;
        }
    }

    //精灵调色板焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurSpPalette);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistorySpPalettePos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurSpPalette);
            memcpy(lpCustomData->data, &m_ptCurSpPalette, sizeof(m_ptCurSpPalette));
            lpDataPos += dwDataSize;
        }
    }

    //调色板焦点坐标
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCurPalette);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTHistoryPalettePos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCurPalette);
            memcpy(lpCustomData->data, &m_ptCurPalette, sizeof(m_ptCurPalette));
            lpDataPos += dwDataSize;
        }
    }

    //字符映射
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_CharMapData);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTCharMap;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(g_CharMapData);
            memcpy(lpCustomData->data, &g_CharMapData, sizeof(g_CharMapData));
            lpDataPos += dwDataSize;
        }
    }

    //光标模式
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBigCursor);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTBigCursor;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_bBigCursor);
            memcpy(lpCustomData->data, &m_bBigCursor, sizeof(m_bBigCursor));
            lpDataPos += dwDataSize;
        }
    }

    //精灵模式
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBigSprite);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTBigSprite;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_bBigSprite);
            memcpy(lpCustomData->data, &m_bBigSprite, sizeof(m_bBigSprite));
            lpDataPos += dwDataSize;
        }
    }

    //字符映射位置
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCharMapSrc);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTCharMapSrcPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCharMapSrc);
            memcpy(lpCustomData->data, &m_ptCharMapSrc, sizeof(m_ptCharMapSrc));
            lpDataPos += dwDataSize;
        }
    }

    //命名表点击位置
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptNameTableTile);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTNTTilePos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptNameTableTile);
            memcpy(lpCustomData->data, &m_ptNameTableTile, sizeof(m_ptNameTableTile));
            lpDataPos += dwDataSize;
        }
    }

    //快速操作字符源
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCharMapDest);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTBkgChrQuickPos;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_ptCharMapDest);
            memcpy(lpCustomData->data, &m_ptCharMapDest, sizeof(m_ptCharMapDest));
            lpDataPos += dwDataSize;
        }
    }

    //背景Tile选中
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBkgTileCheck);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTBkgTileChecked;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_bBkgTileCheck);
            memcpy(lpCustomData->data, &m_bBkgTileCheck, sizeof(m_bBkgTileCheck));
            lpDataPos += dwDataSize;
        }
    }

    //精灵Tile选中
    {
        DWORD dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_bSpTileCheck);
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpDataPos;
        ZeroMemory(lpCustomData, dwDataSize);
        if (lpCustomData)
        {
            lpCustomData->dwType = eMenuDataType::eMDTSpTileChecked;
            lpCustomData->dwElementCount = 1;
            lpCustomData->dwElementSize = sizeof(m_bSpTileCheck);
            memcpy(lpCustomData->data, &m_bSpTileCheck, sizeof(m_bSpTileCheck));
            lpDataPos += dwDataSize;
        }
    }

    return tmpData;
}

bool CDesignDialog::IsSameData(CByteBuffer& data1, CByteBuffer& data2)
{
    bool isSame = true;
    if (data1.GetSize() != data2.GetSize())
    {
        isSame = false;
    }
    else if (0 != memcmp(data1.GetBuffer(), data2.GetBuffer(), data1.GetSize()))
    {
        isSame = false;
    }

    return isSame;
}

void CDesignDialog::RecordHistoryData(bool bManual)
{
    if (!bManual)
    {
        return;
    }

    m_HistoryBuf = GetSaveData();
}

void CDesignDialog::CheckSpriteAttributeButton()
{
    bool bEnable = false;

    for (const auto& item : m_SpTileList)
    {
        if (item.flag)
        {
            bEnable = true;
            break;
        }
    }

    if (m_bBigCursor)
    {
        for (const auto& item : m_CursorTileList)
        {
            if (item.flag)
            {
                bEnable = true;
                break;
            }
        }
    }
    else
    {

        if (m_tileCursor.flag)
        {
            bEnable = true;
        }
    }

    EnableSpriteAttributeButton(bEnable);
}

void CDesignDialog::EnableSpriteAttributeButton(bool bEnable)
{
    ::EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_SPRITE_V), bEnable);
    ::EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_SPRITE_H), bEnable);
    ::EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_SPRITE_BACK), bEnable);
}

void CDesignDialog::UpdateHistoryData(bool bManual)
{
    if (!bManual || nullptr == m_HistoryBuf.GetBuffer())
    {
        return;
    }

    CByteBuffer dataBuf = GetSaveData();

    if (!IsSameData(m_HistoryBuf, dataBuf))
    {
        //覆盖记录
        if ((m_nHistoryIndex + 1) < m_HistoryData.size())
        {
            DWORD nIndex = 0;
            auto itDelBegin = m_HistoryData.end();
            for (auto it = m_HistoryData.begin(); it != m_HistoryData.end(); it++)
            {
                if (nIndex == (m_nHistoryIndex + 1))
                {
                    itDelBegin = it;
                }

                nIndex++;
            }

            if (itDelBegin != m_HistoryData.end())
            {
                m_HistoryData.erase(itDelBegin, m_HistoryData.end());
            }
        }

        if (!m_HistoryData.empty())
        {
            m_HistoryData.pop_back();
        }

        m_HistoryData.push_back(m_HistoryBuf);
        m_nHistoryIndex = m_HistoryData.size();

        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_UNDO), TRUE);
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_REDO), FALSE);

        m_HistoryData.push_back(dataBuf);
    }
}
