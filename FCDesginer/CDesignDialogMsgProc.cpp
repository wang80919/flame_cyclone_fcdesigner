#include "CDesignDialog.h"
#include "resource.h"
#include <strsafe.h>
#include "Win32Utils/CFileVersion.h"
#include "Win32Utils/CRegUtils.h"
#include "Utils/CRegisterUtils.h"
#include <commctrl.h>
#include <windowsx.h>
#include <algorithm>
#include <thread>
#include "CCustomMenuData.h"
#include "Win32Utils/CComFileDlg.h"
#include "Win32Utils/CPathUtils.h"
#include "Utils/CBitmapDraw.h"
#include <imm.h>
#include "Utils/CDesignRegisterUtils.h"
#include <deque>

LRESULT CDesignDialog::OnWindowPosChanging(WPARAM wParam, LPARAM lParam)
{
    LPWINDOWPOS pWPos = (LPWINDOWPOS)lParam;

    if (fSizeInit)
    {
        pWPos->flags |= SWP_NOSIZE;
    }

    return 0;
}

LRESULT CDesignDialog::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
    fSizeInit = false;
    m_taskDraw = std::move(std::thread([this]() {
        DrawTask();
        }
    ));

    OnLanguageChange(m_LangId, 0);

    UINT uDpi = 96;
    {
        HMODULE hModule = ::LoadLibrary(_T("User32.dll"));
        if (NULL != hModule)
        {
            typedef UINT(WINAPI* pFn)(HWND hwnd);
            pFn pDpiForWindow = (pFn)::GetProcAddress(hModule, "GetDpiForWindow");
            if (pDpiForWindow)
            {
                uDpi = pDpiForWindow(m_hWnd);
            }

            ::FreeLibrary(hModule);
        }
    }

    m_nWndScale = (double)uDpi / (double)USER_DEFAULT_SCREEN_DPI;

    m_ftType = CDesignRegisterUtils::Instance().GetType();
    m_designEnable = CDesignRegisterUtils::Instance().IsEnable();

    _tstring strCaption = LoadString(IDS_STRING_CUSTOM_MENU_TEXT);
    if (CDesignRegisterUtils::Instance().IsRegistered())
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        for (const auto& item : g_FcaLimitForDesign)
        {
            if (m_ftType != item.dwType)
            {
                continue;
            }

            ::StringCchPrintf(szBuf, _countof(szBuf), LoadString(IDS_STRING_LICENSE).c_str(),
                CDesignRegisterUtils::Instance().GetUser().c_str(),
                LoadString(item.dwData).c_str()
            );

            _tstring strText = szBuf;
            if (!CDesignRegisterUtils::Instance().IsEnable())
            {
                strText += LoadString(IDS_STRING_PRODUCT_DISABLE);
            }

            strCaption += _T(" ") + strText;
            SetWindowText(m_hWnd, strCaption.c_str());
            break;
        }
    }
    else
    {
        strCaption += + _T(" (") +
            LoadString(IDS_STRING_DESIGN_UNREGISTER) + _T(")");
        SetWindowText(m_hWnd, strCaption.c_str());
    }

    if (CDesignRegisterUtils::Instance().IsFree() && !CDesignRegisterUtils::Instance().IsRegistered())
    {
        strCaption += _T(" (");
        strCaption += LoadString(IDS_STRING_FREE_DESIGN);
        strCaption +=  + _T(")");
        SetWindowText(m_hWnd, strCaption.c_str());
    }

    //快捷键
    m_hAccel = LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_ACCELERATOR_DESIGN));

    // 设置图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
    if (hIcon)
    {
        ::SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        ::SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }

    if (!CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_DESIGN_SHOW_BG_TILE, &m_bShowBgTile, sizeof(m_bShowBgTile)))
    {
        m_bShowBgTile = true;
    }

    if (!CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_DESIGN_SHOW_SP_TILE, &m_bShowSpTile, sizeof(m_bShowSpTile)))
    {
        m_bShowSpTile = true;
    }

    if (!CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_DESIGN_SHOW_LIST, &m_bShowList, sizeof(m_bShowList)))
    {
        m_bShowList = true;
    }

    if (!CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_DESIGN_SHOW_CUSTOM_TEXT, &m_bShowText, sizeof(m_bShowText)))
    {
        m_bShowText = true;
    }

    if (!CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_DESIGN_SHOW_CAPTION, &m_bShowCaption, sizeof(m_bShowCaption)))
    {
        m_bShowCaption = true;
    }

    if (!CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_DESIGN_SHOW_CURSOR, &m_bShowCursor, sizeof(m_bShowCursor)))
    {
        m_bShowCursor = true;
    }

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_BKG_PALETTE_POS, &m_ptCurBkgPalette, sizeof(m_ptCurBkgPalette));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_SP_PALETTE_POS, &m_ptCurSpPalette, sizeof(m_ptCurSpPalette));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_CAPTION_POS, &m_ptCaption, sizeof(m_ptCaption));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_PAGE_NUMBER_POS, &m_ptPageNumber, sizeof(m_ptPageNumber));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_FILL_COLOR, &m_bFillColor, sizeof(m_bFillColor));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_DRAW_LINE, &m_bDrawLine, sizeof(m_bDrawLine));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_DRAG_TEXT, &m_bDragText, sizeof(m_bDragText));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_ITEM_POS, &m_ptItem, sizeof(m_ptItem));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_BIG_CURSOR, &m_bBigCursor, sizeof(m_bBigCursor));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_BIG_SPRITE, &m_bBigSprite, sizeof(m_bBigSprite));

    ::CheckDlgButton(m_hWnd, IDC_CHECK_BIG_CURSOR, m_bBigCursor ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_BIG_SPRITE, m_bBigSprite ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_FILL_COLOR, m_bFillColor ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_DRAW_LINE, m_bDrawLine ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_DRAG_TEXT, m_bDragText ? BST_CHECKED : BST_UNCHECKED);

    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_CAPTION_TEXT, m_strCaption);

    SetItemString(IDC_EDIT_CAPTION_TEXT, m_strCaption);

    LoadCustomMenu();

    m_tileCursor = g_tileArrow;
    m_BkgTileList = g_BkgTileList;
    m_SpTileList = g_SpriteTileList;
    m_CustomTextList = g_CustomTextList;
    m_CursorTileList = g_ArrowTileList;

    if (m_CursorTileList.size() < 4)
    {
        ResetCursorTileList();
    }

    m_SpTileIdCount = m_SpTileList.size();
    m_BkgTileIdCount = g_BkgTileList.size();
    m_CustomTextCount = g_CustomTextList.size();

    {
        m_wndTileDraw.Register(_T("FCVramDraw"));
        m_wndTileDraw.Create(m_hWnd, _T(""), SW_HIDE, WS_POPUPWINDOW, 0, 0, 0, 512, 384, true);
        m_wndTileDraw.SetDrag(true);
        DrawTile();

        m_wndDefaultTile.Register(_T("FCVramDraw"));
        m_wndDefaultTile.Create(m_hWnd, _T(""), SW_HIDE, WS_POPUPWINDOW, 0, 0, 0, 256, 384, true);
        m_wndDefaultTile.SetDrag(true);
        DrawDefaultTile();

        int nPosX = 548;

        m_wndBkgTile.Superclass(_T("Static"), _T("FCDraw"));
        m_wndBkgTile.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, nPosX, 124, 256, 256, FALSE);
        DrawBkgVram();
        m_wndBkgTileEdit.Superclass(_T("Static"), _T("FCDraw"));
        m_wndBkgTileEdit.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, nPosX, 388, 128, 128, FALSE);
        UpdateBkgVramEdit();

        m_wndSpTile.Superclass(_T("Static"), _T("FCDraw"));
        m_wndSpTile.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, nPosX + 256 + 8, 124, 256, 256, FALSE);
        DrawSpVram();

        m_wndSpTileEdit.Superclass(_T("Static"), _T("FCDraw"));
        m_wndSpTileEdit.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, nPosX + 256 + 8, 388, 128, 128, FALSE);
        UpdateSpVramEdit();

        m_wndCharMappingDest.Superclass(_T("Static"), _T("FCDraw"));
        m_wndCharMappingDest.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 32, 32, 1024, 16, FALSE);

        m_wndCharMappingSrc.Superclass(_T("Static"), _T("FCDraw"));
        m_wndCharMappingSrc.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 32, 48, 1024, 16, FALSE);

        m_wndDragTile.Superclass(_T("Static"), _T("FCDraw"));
        m_wndDragTile.Create(m_hWnd, _T(""), SW_HIDE, WS_POPUP, WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY, 0, 0, 18, 36, FALSE);

        DrawCharMappingTarget();
    }

    _tstring strCustom;

    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_CUSTOM_TEXT_EDIT, strCustom);

    SetItemString(IDC_EDIT_CUSTOM_TEXT, strCustom);

    m_hWndNameTable.Superclass(_T("Static"), _T("FCDraw"));
    m_hWndNameTable.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 0, 0, 512, 480, FALSE);

    m_hWndAttribute.Superclass(_T("Static"), _T("FCDraw"));
    m_hWndAttribute.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 0, 0, 128, 128, FALSE);

    m_hWndBkgPalette.Superclass(_T("Static"), _T("FCDraw"));
    m_hWndBkgPalette.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 0, 0, 256, 16, FALSE);

    m_hWndSpPalette.Superclass(_T("Static"), _T("FCDraw"));
    m_hWndSpPalette.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 0, 0, 256, 16, FALSE);

    m_hWndPalette.Superclass(_T("Static"), _T("FCDraw"));
    m_hWndPalette.Create(m_hWnd, _T(""), SW_SHOW, WS_CHILD, 0, 0, 0, 256, 64, FALSE);

    fSizeInit = false;
    SetWndPos();
    fSizeInit = true;

    ::ImmAssociateContext(GetDlgItem(m_hWnd, IDC_EDIT_CAPTION_TEXT), NULL);
    ::ImmAssociateContext(GetDlgItem(m_hWnd, IDC_EDIT_CUSTOM_TEXT), NULL);

    if (m_HistoryData.empty())
    {
        DWORD dwHistorySize = 0;
        if (CRegUtils::GetValueSize(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_HISTORY_DATA_LIST, dwHistorySize) && dwHistorySize > 0)
        {
            CByteBuffer cBuffer(dwHistorySize);

            CRegUtils::GetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_HISTORY_DATA_LIST, cBuffer.GetBuffer(), dwHistorySize);

            LPCBYTE lpBegin = cBuffer.GetBuffer();
            LPCBYTE lpData = lpBegin;
            while (lpData < lpBegin + dwHistorySize)
            {
                PCUSTOM_MENU_DATA pMenuData = (PCUSTOM_MENU_DATA)lpData;
                if (pMenuData->dwType != eMenuDataType::eMDTHistoryRoot)
                {
                    break;
                }

                CByteBuffer item(pMenuData->dwElementSize);
                memcpy_s(item.GetBuffer(), pMenuData->dwElementSize, pMenuData->data, pMenuData->dwElementSize);
                m_HistoryData.push_back(std::move(item));
                lpData += pMenuData->dwElementSize + sizeof(CUSTOM_MENU_DATA);
            }
        }
    }

    {
        DWORD dwLastWorkSize = 0;
        if (CRegUtils::GetValueSize(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_LAST_WORK_STATE, dwLastWorkSize) && dwLastWorkSize > 0)
        {
            CByteBuffer cBuffer(dwLastWorkSize);

            CRegUtils::GetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_LAST_WORK_STATE, cBuffer.GetBuffer(), dwLastWorkSize);

            SetLoadData(cBuffer);
        }
    }

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_HISTORY_DATA_INDEX, &m_nHistoryIndex, sizeof(m_nHistoryIndex));

    if (m_HistoryData.size() > 0 && m_nHistoryIndex >= m_HistoryData.size())
    {
        m_nHistoryIndex = m_HistoryData.size() - 1;
    }

    if (m_HistoryData.empty())
    {
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_UNDO), FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_REDO), FALSE);
    }

    if (!m_HistoryData.empty())
    {
        if (m_nHistoryIndex > 0)
        {
            ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_UNDO), TRUE);
        }

        if (m_nHistoryIndex < m_HistoryData.size() - 1)
        {
            ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_REDO), TRUE);
        }
    }

    CheckSpriteAttributeButton();

    ::CheckDlgButton(m_hWnd, IDC_CHECK_TILE_SELECT, m_bSelectMode ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_MARK_CURSOR, m_bMarkCursor ? BST_CHECKED : BST_UNCHECKED);

    UpdateLicense();

    ::SetTimer(m_hWnd, TIMER_LICENSE_CHECK, LICENSE_CHECK_TIME, nullptr);

    if (m_bShowBgTile)
    {
        PostMessage(WM_COMMAND, ID_SHOW_BG, 0);
    }

    if (m_bShowSpTile)
    {
        PostMessage(WM_COMMAND, ID_SHOW_SP, 0);
    }

    if (m_bShowList)
    {
        PostMessage(WM_COMMAND, ID_SHOW_LIST, 0);
    }

    if (m_bShowText)
    {
        PostMessage(WM_COMMAND, ID_SHOW_CUSTOM_TEXT, 0);
    }

    if (m_bShowCaption)
    {
        PostMessage(WM_COMMAND, ID_SHOW_CAPTION, 0);
    }

    if (m_bShowCursor)
    {
        PostMessage(WM_COMMAND, ID_SHOW_CURSOR, 0);
    }

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnClose(WPARAM wParam, LPARAM lParam)
{
    g_BkgTileList = m_BkgTileList;
    g_SpriteTileList = m_SpTileList;
    g_CustomTextList = m_CustomTextList;
    g_ArrowTileList = m_CursorTileList;
    g_tileArrow = m_tileCursor;

    m_bQuit = true;
    if (m_taskDraw.joinable())
    {
        m_cvDraw.notify_one();
        m_taskDraw.join();
    }
    m_bQuit = false;

    ::DestroyAcceleratorTable(m_hAccel);

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_BIG_CURSOR, &m_bBigCursor, sizeof(m_bBigCursor));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_BIG_SPRITE, &m_bBigSprite, sizeof(m_bBigSprite));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CHAR_MAP_DATA, g_CharMapData, sizeof(g_CharMapData));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_DESIGN_SHOW_BG_TILE, &m_bShowBgTile, sizeof(m_bShowBgTile));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_DESIGN_SHOW_SP_TILE, &m_bShowSpTile, sizeof(m_bShowSpTile));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_DESIGN_SHOW_LIST, &m_bShowList, sizeof(m_bShowList));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_DESIGN_SHOW_CUSTOM_TEXT, &m_bShowText, sizeof(m_bShowText));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_DESIGN_SHOW_CAPTION, &m_bShowCaption, sizeof(m_bShowCaption));
    }

    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_DESIGN_SHOW_CURSOR, &m_bShowCursor, sizeof(m_bShowCursor));
    }

    {
        CByteBuffer cBuffer = GetSaveData();
        {
            CRegUtils::SetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_LAST_WORK_STATE, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
    }

    if (!m_HistoryData.empty())
    {
        size_t dwSize = 0;

        for (auto item : m_HistoryData)
        {
            dwSize += sizeof(CUSTOM_MENU_DATA) + item.GetSize();
        }

        CByteBuffer cBuffer(dwSize);
        LPBYTE lpTempData = cBuffer.GetBuffer();
        for (auto item : m_HistoryData)
        {
            CUSTOM_MENU_DATA* pInfo = (CUSTOM_MENU_DATA*)lpTempData;
            pInfo->dwElementCount = 1;
            pInfo->dwType = eMenuDataType::eMDTHistoryRoot;
            pInfo->dwElementSize = (DWORD)item.GetSize();
            memcpy_s(pInfo->data, item.GetSize(), item.GetBuffer(), item.GetSize());
            lpTempData += sizeof(CUSTOM_MENU_DATA) + item.GetSize();
        }

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_HISTORY_DATA_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
    }

    CRegUtils::SetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_HISTORY_DATA_INDEX, &m_nHistoryIndex, sizeof(m_nHistoryIndex));

    if (CRegUtils::CreateKey(CONFIG_REG_KEY, CONFIG_MENU_DESIGN_PATH))
    {
        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_BKG_PALETTE_POS, &m_ptCurBkgPalette, sizeof(m_ptCurBkgPalette));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_SP_PALETTE_POS, &m_ptCurSpPalette, sizeof(m_ptCurSpPalette));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_ARROW_TILE, &g_tileArrow, sizeof(g_tileArrow));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_NAME_TABLE_ATTRIBUTE, g_AttributeData, sizeof(g_AttributeData));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_BKG_PALETTE, g_BkgPalette, sizeof(g_BkgPalette));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_SPRITE_PALETTE, g_SpritePalette, sizeof(g_SpritePalette));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CAPTION_POS, &m_ptCaption, sizeof(m_ptCaption));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_PAGE_NUMBER_POS, &m_ptPageNumber, sizeof(m_ptPageNumber));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_FILL_COLOR, &m_bFillColor, sizeof(m_bFillColor));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_DRAW_LINE, &m_bDrawLine, sizeof(m_bDrawLine));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_DRAG_TEXT, &m_bDragText, sizeof(m_bDragText));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_ITEM_POS, &m_ptItem, sizeof(m_ptItem));

        g_dwCaptionPpuAddr = 0x2000 + m_ptCaption.y * 0x20 + m_ptCaption.x;
        g_dwPageNumberPpuAddr = 0x2000 + m_ptPageNumber.y * 0x20 + m_ptPageNumber.x;
        g_dwItemPpuAddr = 0x2000 + m_ptItem.y * 0x20 + m_ptItem.x;

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CAPTION_PPU_ADDR, &g_dwCaptionPpuAddr, sizeof(g_dwCaptionPpuAddr));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_PAGE_NUMBER_PPU_ADDR, &g_dwPageNumberPpuAddr, sizeof(g_dwPageNumberPpuAddr));

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_ITEM_PPU_ADDR, &g_dwItemPpuAddr, sizeof(g_dwItemPpuAddr));

        g_strCaption = GetItemString(IDC_EDIT_CAPTION_TEXT);
        CRegUtils::SetString(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CAPTION_TEXT, g_strCaption);

        if (!m_BkgTileList.empty())
        {
            CByteBuffer cBuffer(m_BkgTileList.size() * sizeof(TILE_INFO));
            TILE_INFO* pInfo = (TILE_INFO*)cBuffer.GetBuffer();
            for (size_t i = 0; i < m_BkgTileList.size(); i++)
            {
                *pInfo = m_BkgTileList[i];
                pInfo->id = (DWORD)i;
                pInfo++;
            }

            CRegUtils::SetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_BKG_TILE_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
        else
        {
            CRegUtils::DelValue(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_BKG_TILE_LIST
            );
        }

        if (!m_SpTileList.empty())
        {
            CByteBuffer cBuffer(m_SpTileList.size() * sizeof(TILE_INFO));
            TILE_INFO* pInfo = (TILE_INFO*)cBuffer.GetBuffer();
            for (size_t i = 0; i < m_SpTileList.size(); i++)
            {
                *pInfo = m_SpTileList[i];
                pInfo->id = (DWORD)i;
                pInfo++;
            }

            CRegUtils::SetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_SPRITE_TILE_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
        else
        {
            CRegUtils::DelValue(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_SPRITE_TILE_LIST
            );
        }

        if (!m_CursorTileList.empty())
        {
            CByteBuffer cBuffer(m_CursorTileList.size() * sizeof(TILE_INFO));
            TILE_INFO* pInfo = (TILE_INFO*)cBuffer.GetBuffer();
            for (size_t i = 0; i < m_CursorTileList.size(); i++)
            {
                *pInfo = m_CursorTileList[i];
                pInfo->id = (DWORD)i;
                pInfo++;
            }

            CRegUtils::SetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_ARROW_TILE_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
        else
        {
            CRegUtils::DelValue(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_ARROW_TILE_LIST
            );
        }

        CRegUtils::SetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CHR_DATA, g_ChrRam, sizeof(g_ChrRam));

        CRegUtils::SetString(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CUSTOM_TEXT_EDIT, GetItemString(IDC_EDIT_CUSTOM_TEXT).c_str());

        {
            //计算大小
            size_t dwDataSize = 0;

            for (auto it = m_CustomTextList.begin(); it != m_CustomTextList.end();)
            {
                if (it->pt.y < 0 || it->pt.y > 29)
                {
                    it = m_CustomTextList.erase(it);
                    continue;
                }

                if (it->pt.x > 31)
                {
                    it = m_CustomTextList.erase(it);
                    continue;
                }

                if (it->pt.x < 0 && abs(it->pt.x) >= (LONG)it->strText.size())
                {
                    it = m_CustomTextList.erase(it);
                    continue;
                }

                it++;
            }

            for (const auto& item : m_CustomTextList)
            {
                dwDataSize += sizeof(CUSTOM_MENU_DATA) + sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt);
            }

            LPBYTE lpCustomData = (LPBYTE) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                LPBYTE lpTempData = lpCustomData;
                for (const auto& item : m_CustomTextList)
                {
                    PCUSTOM_MENU_DATA lpData = (PCUSTOM_MENU_DATA)lpTempData;

                    lpData->dwType = eMDTCustomText;
                    lpData->dwElementCount = 1;
                    lpData->dwElementSize = (DWORD)(sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt));

                    PCUSTOM_TEXT pData = (PCUSTOM_TEXT)lpData->data;
                    pData->pt = item.pt;
                    pData->dwSize = (DWORD)(item.strText.size() * sizeof(TCHAR));
                    memcpy(pData->data, item.strText.c_str(), item.strText.size() * sizeof(TCHAR));

                    lpTempData += lpData->dwElementSize + sizeof(CUSTOM_MENU_DATA);
                }

                CRegUtils::SetBinary(
                    CONFIG_REG_KEY,
                    CONFIG_MENU_DESIGN_PATH,
                    CONFIG_MENU_CUSTOM_TEXT, lpCustomData, (DWORD)dwDataSize);

                delete[] lpCustomData;
            }
        }
    }

    m_hWndNameTable.SendMessage(WM_CLOSE, 0, 0);
    m_hWndAttribute.SendMessage(WM_CLOSE, 0, 0);
    m_hWndBkgPalette.SendMessage(WM_CLOSE, 0, 0);
    m_hWndSpPalette.SendMessage(WM_CLOSE, 0, 0);
    m_hWndPalette.SendMessage(WM_CLOSE, 0, 0);
    m_wndTileDraw.SendMessage(WM_CLOSE, 0, 0);
    m_wndDragTile.SendMessage(WM_CLOSE, 0, 0);
    m_wndBkgTile.SendMessage(WM_CLOSE, 0, 0);
    m_wndSpTile.SendMessage(WM_CLOSE, 0, 0);
    m_wndBkgTileEdit.SendMessage(WM_CLOSE, 0, 0);
    m_wndSpTileEdit.SendMessage(WM_CLOSE, 0, 0);

    ::KillTimer(m_hWnd, TIMER_LICENSE_CHECK);
    EndDialog(IDCANCEL);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnTimer(WPARAM wParam, LPARAM lParam)
{
    if (TIMER_LICENSE_CHECK == wParam)
    {
        UpdateLicense();
    }

    return (LRESULT)TRUE;
}

void CDesignDialog::SetCaption(const _tstring& strCaption)
{
    m_strCaption = strCaption;

}

void CDesignDialog::SetPageShow(bool bShow)
{
    m_bShowPage = bShow;
}

void CDesignDialog::SetItemList(const std::vector<_tstring>& itemList, DWORD dwPageSize, bool bInsertRow)
{
    m_strItemListTotal = itemList;
    m_bNameInsertRow = bInsertRow;

    for (auto& item : m_strItemListTotal)
    {
        if (item.empty())
        {
            item = _T(" ");
        }

        std::transform(item.begin(), item.end(), item.begin(), [](TCHAR ch)->TCHAR {
            if (ch >= _T('a') && ch <= _T('z')) ch &= ~0x20;
            return ch;
            });

        for (auto& ch : item)
        {
            if (ch >= _T('\x80'))
            {
                ch = _T('?');
            }
        }
    }

    m_nPageSize = dwPageSize;


    m_strItemList.clear();

    DWORD nIndex = 0;
    for (const auto item : m_strItemListTotal)
    {
        if (nIndex < dwPageSize)
        {
            m_strItemList.push_back(item);

            if (m_bNameInsertRow)
            {
                m_strItemList.push_back(_T(""));
            }
        }
        else
        {
            break;
        }

        nIndex++;
    }

    m_nCurIndex = 0;
    m_nPageSize = dwPageSize;
    m_strPageNumber = GetPageText(m_nCurIndex, (int)m_strItemListTotal.size(), m_nPageSize);
}

_tstring CDesignDialog::GetPageText(int nIndex, int nTotal, int nPageSize)
{
    TCHAR szBuf[MAX_PATH] = { 0 };

    if (0 == nTotal)
    {
        return _T("1/1");
    }

    DWORD dwPageCount = (DWORD)(m_strItemListTotal.size() / nPageSize);
    if (0 != m_strItemListTotal.size() % nPageSize)
    {
        dwPageCount++;
    }

    _tstring strMax = std::to_wstring(dwPageCount);
    _tstring strFormat = _T("%0.") + std::to_wstring(strMax.size()) + _T("d/%d");
    ::StringCchPrintf(szBuf, _countof(szBuf), strFormat.c_str(), nIndex / nPageSize + 1, dwPageCount);

    return szBuf;
}

LRESULT CDesignDialog::ShowFirstPage(WPARAM wParam, LPARAM lParam)
{
    m_strItemList.clear();

    int nIndex = 0;
    m_nCurIndex = 0;
    for (size_t i = m_nCurIndex; i < m_strItemListTotal.size(); i++)
    {
        if (nIndex < m_nPageSize)
        {
            m_strItemList.push_back(m_strItemListTotal[i]);

            if (m_bNameInsertRow)
            {
                m_strItemList.push_back(_T(""));
            }
        }
        else
        {
            break;
        }

        nIndex++;
    }

    m_strPageNumber = GetPageText(m_nCurIndex, (int)m_strItemListTotal.size(), m_nPageSize);

    DrawNameTable();

    return TRUE;
}

LRESULT CDesignDialog::ShowLastPage(WPARAM wParam, LPARAM lParam)
{
    m_strItemList.clear();

    if (0 == m_nPageSize)
    {
        return TRUE;
    }

    int nIndex = 0;
    if (m_strItemListTotal.size() % m_nPageSize)
    {
        m_nCurIndex = (int)(m_strItemListTotal.size() / m_nPageSize * m_nPageSize);
    }
    else
    {
        m_nCurIndex = (int)(((m_strItemListTotal.size() / m_nPageSize) - 1) * m_nPageSize);
    }

    for (size_t i = m_nCurIndex; i < m_strItemListTotal.size(); i++)
    {
        if (nIndex < m_nPageSize)
        {
            m_strItemList.push_back(m_strItemListTotal[i]);

            if (m_bNameInsertRow)
            {
                m_strItemList.push_back(_T(""));
            }
        }
        else
        {
            break;
        }

        nIndex++;
    }

    m_strPageNumber = GetPageText(m_nCurIndex, (int)m_strItemListTotal.size(), m_nPageSize);

    DrawNameTable();

    return TRUE;
}

LRESULT CDesignDialog::ShowPrePage(WPARAM wParam, LPARAM lParam)
{
    if (m_nCurIndex - m_nPageSize < 0)
    {
        return FALSE;
    }

    m_strItemList.clear();

    DWORD nIndex = 0;
    m_nCurIndex -= m_nPageSize;
    for (size_t i = m_nCurIndex; i < m_strItemListTotal.size(); i++)
    {
        if (nIndex < (DWORD)m_nPageSize)
        {
            m_strItemList.push_back(m_strItemListTotal[i]);

            if (m_bNameInsertRow)
            {
                m_strItemList.push_back(_T(""));
            }
        }
        else
        {
            break;
        }

        nIndex++;
    }

    m_strPageNumber = GetPageText(m_nCurIndex, (int)m_strItemListTotal.size(), m_nPageSize);

    DrawNameTable();

    return TRUE;
}

LRESULT CDesignDialog::ShowNextPage(WPARAM wParam, LPARAM lParam)
{
    if (m_nCurIndex + m_nPageSize >= (int)m_strItemListTotal.size())
    {
        return FALSE;
    }

    m_strItemList.clear();

    int nIndex = 0;
    m_nCurIndex += m_nPageSize;


    for (size_t i = m_nCurIndex; i < m_strItemListTotal.size(); i++)
    {
        if (nIndex < m_nPageSize)
        {
            m_strItemList.push_back(m_strItemListTotal[i]);

            if (m_bNameInsertRow)
            {
                m_strItemList.push_back(_T(""));
            }
        }
        else
        {
            break;
        }

        nIndex++;
    }

    m_strPageNumber = GetPageText(m_nCurIndex, (int)m_strItemListTotal.size(), m_nPageSize);
    DrawNameTable();
    return TRUE;
}


LRESULT CDesignDialog::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
    //SetWndPos();

    return TRUE;
}

LRESULT CDesignDialog::OnCmdFillColor(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    m_bFillColor = ::IsDlgButtonChecked(m_hWnd, wID);

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdDrawLine(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    m_bDrawLine = ::IsDlgButtonChecked(m_hWnd, wID);

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdDragText(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    m_bDragText = ::IsDlgButtonChecked(m_hWnd, wID);

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdExportCharMapping(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
        {
            m_strPath += _T(".bin");
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        cFile.Write(g_CharMapData, sizeof(g_CharMapData), 0, nullptr, FILE_CURRENT);
        cFile.SetEnd(cFile.GetPointer());

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdResetCharMapping(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    ResetCharMap();
    DrawNameTable();

    UpdateHistoryData(true);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("fcm"), _T("*.fcm") },
        { _T("bin_2000"), _T("*.bin") },
        { _T("bin_2C00"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, (DWORD)_countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_CURRENT);

        //读取解析
        RecordHistoryData(wParam);

        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)cBuffer.GetBuffer();

        if (((2 == uIndex || 3 == uIndex) && 0x4000 == ullSize))
        {
            m_CustomTextList.clear();
            m_BkgTileList.clear();
            m_SpTileList.clear();
            m_CustomTextCount = 0;
            m_SpTileIdCount = 0;
            m_BkgTileIdCount = 0;
        }

        if (1 == uIndex)
        {
            m_CustomTextList.clear();
            m_BkgTileList.clear();
            m_SpTileList.clear();
            m_CursorTileList.clear();
            m_CustomTextCount = 0;
            m_SpTileIdCount = 0;
            m_BkgTileIdCount = 0;
        }

        if (2 == uIndex || 3 == uIndex)
        {
            DWORD dwBase = 0x2000;
            if (3 == uIndex)
            {
                dwBase = 0x2C00;
            }

            if (0x4000 == ullSize)
            {
                LPBYTE lpData = cBuffer.GetBuffer();

                memcpy_s(g_ChrRam + 0x0000, 0x1000, lpData + 0x0000, 0x1000);
                memcpy_s(g_ChrRam + 0x1000, 0x1000, lpData + 0x1000, 0x1000);
                memcpy_s(g_BkgPalette, sizeof(g_BkgPalette), lpData + 0x3F00, sizeof(g_BkgPalette));
                memcpy_s(g_SpritePalette, sizeof(g_SpritePalette), lpData + 0x3F10, sizeof(g_SpritePalette));
                memcpy_s(g_AttributeData, sizeof(g_AttributeData), lpData + dwBase + 0x03C0, sizeof(g_AttributeData));

                m_BkgTileList.clear();
                m_BkgTileIdCount = 0;

                for (int i = 0; i < 0x3C0; i++)
                {
                    if (0 == lpData[dwBase + i])
                    {
                        continue;
                    }

                    TILE_INFO info;
                    info.id = (DWORD)m_BkgTileIdCount++;
                    info.data = lpData[dwBase + i];
                    info.pt.x = i & 0x1F;
                    info.pt.y = i >> 5;
                    m_BkgTileList.push_back(info);
                }
            }
        }
        else
        {
            while ((LPBYTE)lpCustomData < cBuffer.GetBuffer() + ullSize)
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

                if (eMenuDataType::eMDTCharMap == dwType)
                {
                    if (dwElementSize <= sizeof(g_CharMapData))
                    {
                        memcpy_s(&g_CharMapData, sizeof(g_CharMapData), lpData, dwElementSize);
                    }
                }

                if (eMenuDataType::eMDTBigCursor == dwType)
                {
                    if (dwElementSize <= sizeof(m_bBigCursor))
                    {
                        memcpy_s(&m_bBigCursor, sizeof(m_bBigCursor), lpData, dwElementSize);
                    }
                }

                if (eMenuDataType::eMDTBigSprite == dwType)
                {
                    if (dwElementSize <= sizeof(m_bBigSprite))
                    {
                        memcpy_s(&m_bBigSprite, sizeof(m_bBigSprite), lpData, dwElementSize);
                    }
                }

                lpCustomData = (PCUSTOM_MENU_DATA)((LPBYTE)lpCustomData + dwElementSize + sizeof(CUSTOM_MENU_DATA));
            }
        }

        ::CheckDlgButton(m_hWnd, IDC_CHECK_BIG_CURSOR, m_bBigCursor ? BST_CHECKED : BST_UNCHECKED);
        ::CheckDlgButton(m_hWnd, IDC_CHECK_BIG_SPRITE, m_bBigSprite ? BST_CHECKED : BST_UNCHECKED);

        ResetBkgChecked();
        ResetSpChecked();

        UpdateAttribute();

        DrawTile();
        DrawBkgVram();
        DrawSpVram();
        DrawBkgPalette();
        DrawSpritePalette();
        DrawPalette();
        UpdateBkgVramEdit();
        UpdateSpVramEdit();
        DrawNameTable();
        DrawAttribute();

        UpdateHistoryData(wParam);
        CheckSpriteAttributeButton();
        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("fcm"), _T("*.fcm") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uIndex, filters, (int)_countof(filters)))
        {
            return;
        }

        if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("fcm")))
        {
            m_strPath += _T(".fcm");
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        cFile.Clear();

        //背景Tile
        {
            for (const auto& item : m_BkgTileList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
                PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
                if (lpCustomData)
                {
                    ZeroMemory(lpCustomData, dwDataSize);
                    lpCustomData->dwType = eMenuDataType::eMDTBkgTile;
                    lpCustomData->dwElementCount = 1;
                    lpCustomData->dwElementSize = (DWORD)sizeof(TILE_INFO);
                    memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));

                    cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                    delete[] lpCustomData;
                }
            }
        }

        //精灵Tile
        {
            for (const auto& item : m_SpTileList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
                PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
                if (lpCustomData)
                {
                    ZeroMemory(lpCustomData, dwDataSize);
                    lpCustomData->dwType = eMenuDataType::eMDTSpriteTile;
                    lpCustomData->dwElementCount = 1;
                    lpCustomData->dwElementSize = (DWORD)sizeof(TILE_INFO);
                    memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));

                    cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                    delete[] lpCustomData;
                }
            }
        }

        //字符映射
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_CharMapData);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTCharMap;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = sizeof(g_CharMapData);
                memcpy(lpCustomData->data, g_CharMapData, sizeof(g_CharMapData));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //属性表
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_AttributeData);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTAttributes;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(g_AttributeData);
                memcpy(lpCustomData->data, g_AttributeData, sizeof(g_AttributeData));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //背景调色板
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_BkgPalette);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTBkgPalette;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(g_BkgPalette);
                memcpy(lpCustomData->data, g_BkgPalette, sizeof(g_BkgPalette));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //精灵调色板
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_SpritePalette);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTSpritePalette;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(g_SpritePalette);
                memcpy(lpCustomData->data, g_SpritePalette, sizeof(g_SpritePalette));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //标题坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCaption);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTCaptionPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptCaption);
                memcpy(lpCustomData->data, &m_ptCaption, sizeof(m_ptCaption));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //标题文本
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + m_strCaption.size() * sizeof(TCHAR);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTCaptionText;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)(m_strCaption.size() * sizeof(TCHAR));
                memcpy(lpCustomData->data, m_strCaption.c_str(), m_strCaption.size() * sizeof(TCHAR));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //页号坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptPageNumber);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTPageNumberPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptPageNumber);
                memcpy(lpCustomData->data, &m_ptPageNumber, sizeof(m_ptPageNumber));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //节目坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptItem);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTItemPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptItem);
                memcpy(lpCustomData->data, &m_ptItem, sizeof(m_ptItem));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //箭头坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTCursorTile;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(TILE_INFO);
                memcpy(lpCustomData->data, &m_tileCursor, sizeof(m_tileCursor));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //箭头列表
        for (const auto& item : m_CursorTileList)
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTCursorTileList;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(TILE_INFO);
                memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //CHR
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_ChrRam);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTChrData;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(g_ChrRam);
                memcpy(lpCustomData->data, &g_ChrRam, sizeof(g_ChrRam));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //自定义文本
        {
            for (const auto& item : m_CustomTextList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt);

                PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
                if (lpCustomData)
                {
                    ZeroMemory(lpCustomData, dwDataSize);
                    lpCustomData->dwType = eMDTCustomText;
                    lpCustomData->dwElementCount = 1;
                    lpCustomData->dwElementSize = (DWORD)(sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt));

                    PCUSTOM_TEXT pData = (PCUSTOM_TEXT)lpCustomData->data;
                    pData->pt = item.pt;
                    pData->dwSize = (DWORD)(item.strText.size() * sizeof(TCHAR));
                    memcpy(pData->data, item.strText.c_str(), item.strText.size() * sizeof(TCHAR));

                    cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                    delete[] lpCustomData;
                }
            }
        }

        //光标模式
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBigCursor);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTBigCursor;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_bBigCursor);
                memcpy(lpCustomData->data, &m_bBigCursor, sizeof(m_bBigCursor));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //精灵模式
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_bBigSprite);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                ZeroMemory(lpCustomData, dwDataSize);
                lpCustomData->dwType = eMenuDataType::eMDTBigSprite;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_bBigSprite);
                memcpy(lpCustomData->data, &m_bBigSprite, sizeof(m_bBigSprite));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdReset(WPARAM wParam, LPARAM lParam)
{
    m_ptCurBkgPalette = { 1,0 };
    m_ptCurSpPalette = { 1,0 };

    bool bManual = wParam == IDC_BUTTON_RESET;
    RecordHistoryData(bManual);

    ResetBkgChecked();
    ResetSpChecked();

    ResetCharMap();
    OnCmdLayoutReset(wParam, 0);
    OnCmdBkgPaletteReset(wParam, 0);
    OnCmdSpPaletteReset(wParam, 0);
    OnCmdBkgListReset(wParam, 0);
    OnCmdBkgChrReset(wParam, 0);
    OnCmdSpChrReset(wParam, 0);
    OnCmdSpListReset(wParam, 0);

    UpdateHistoryData(bManual);
    CheckSpriteAttributeButton();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdAttrLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("attr"), _T("*.attr") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        RecordHistoryData(wParam);
        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_CURRENT);

        if ((DWORD)cBuffer.GetSize() <= sizeof(g_AttributeData))
        {
            memcpy_s(g_AttributeData, sizeof(g_AttributeData), cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
        else
        {
            memcpy_s(g_AttributeData, sizeof(g_AttributeData), cBuffer.GetBuffer(), sizeof(g_AttributeData));
        }

        UpdateHistoryData(wParam);

        UpdateAttribute();
        DrawNameTable();
        DrawAttribute();
        DrawBkgPalette();
        DrawPalette();
        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdAttrSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("attr"), _T("*.attr") },
        { _T("bin"), _T("*.bin") },
        { _T("attr(MMC5)"), _T("*.attr") },
        { _T("bin(MMC5)"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (1 == uTypeIndex || 3 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("attr")))
            {
                m_strPath += _T(".attr");
            }
        }

        if (2 == uTypeIndex || 4 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
            {
                m_strPath += _T(".bin");
            }
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        if (0 == uTypeIndex || 1 == uTypeIndex)
        {
            cFile.Write(g_AttributeData, sizeof(g_AttributeData), 0, nullptr, FILE_CURRENT);
        }

        if (3 == uTypeIndex || 4 == uTypeIndex)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    // 4 * 4 Tile 颜色分离
                    BYTE data = g_AttributeData[(i * 8 + j)];
                    DWORD dwLTIndex = (data & 0x03) >> 0;
                    DWORD dwRTIndex = (data & 0x0C) >> 2;
                    DWORD dwLBIndex = (data & 0x30) >> 4;
                    DWORD dwRBIndex = (data & 0xC0) >> 6;

                    // 普通PPU属性表 4 * 4 转 MMC5 扩展属性表32 * 32
                    g_AttributeDataMMC5[(i * 4 + 0) * 32 + j * 4 + 0] = (BYTE)(dwLTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 0) * 32 + j * 4 + 1] = (BYTE)(dwLTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 1) * 32 + j * 4 + 0] = (BYTE)(dwLTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 1) * 32 + j * 4 + 1] = (BYTE)(dwLTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 0) * 32 + j * 4 + 2] = (BYTE)(dwRTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 0) * 32 + j * 4 + 3] = (BYTE)(dwRTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 1) * 32 + j * 4 + 2] = (BYTE)(dwRTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 1) * 32 + j * 4 + 3] = (BYTE)(dwRTIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 2) * 32 + j * 4 + 0] = (BYTE)(dwLBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 2) * 32 + j * 4 + 1] = (BYTE)(dwLBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 3) * 32 + j * 4 + 0] = (BYTE)(dwLBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 3) * 32 + j * 4 + 1] = (BYTE)(dwLBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 2) * 32 + j * 4 + 2] = (BYTE)(dwRBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 2) * 32 + j * 4 + 3] = (BYTE)(dwRBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 3) * 32 + j * 4 + 2] = (BYTE)(dwRBIndex << 6);
                    g_AttributeDataMMC5[(i * 4 + 3) * 32 + j * 4 + 3] = (BYTE)(dwRBIndex << 6);
                }
            }

            cFile.Write(g_AttributeDataMMC5, sizeof(g_AttributeDataMMC5), 0, nullptr, FILE_CURRENT);
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdAttrReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_ATTR_RESET;
    RecordHistoryData(bManual);

    memcpy(g_AttributeData, g_AttributeDefaultData, 0x40);

    UpdateAttribute();

    DrawNameTable();
    DrawAttribute();
    DrawBkgPalette();
    DrawPalette();

    UpdateHistoryData(bManual);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdTextLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("fct"), _T("*.fct") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, (DWORD)_countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_CURRENT);

        //读取解析
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)cBuffer.GetBuffer();

        RecordHistoryData(wParam);

        m_CustomTextList.clear();
        m_CustomTextCount = 0;

        while ((LPBYTE)lpCustomData < cBuffer.GetBuffer() + ullSize)
        {
            if (eMenuDataType::eMDTCaptionText == lpCustomData->dwType)
            {
                TCHAR szBuf[MAX_PATH] = { 0 };
                memcpy_s(szBuf, sizeof(szBuf), lpCustomData->data, lpCustomData->dwElementSize);
                m_strCaption = szBuf;
            }

            if (eMenuDataType::eMDTCaptionPos == lpCustomData->dwType)
            {
                m_ptCaption = *(POINT*)&lpCustomData->data;
            }

            if (eMenuDataType::eMDTCustomText == lpCustomData->dwType)
            {
                PCUSTOM_TEXT lpCustomText = (PCUSTOM_TEXT)lpCustomData->data;

                TEXT_INFO info;
                info.id = (DWORD)m_CustomTextCount++;
                info.pt = lpCustomText->pt;
                TCHAR szBuf[MAX_PATH] = { 0 };
                memcpy_s(szBuf, sizeof(szBuf), lpCustomText->data, lpCustomText->dwSize);
                info.strText = szBuf;

                m_CustomTextList.push_back(info);
            }

            lpCustomData = (PCUSTOM_MENU_DATA)((LPBYTE)lpCustomData + lpCustomData->dwElementSize + sizeof(CUSTOM_MENU_DATA));
        }

        DrawNameTable();

        UpdateHistoryData(wParam);
        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdTextSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this](void) -> void {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("fct"), _T("*.fct") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uIndex, filters, (int)_countof(filters)))
        {
            return;
        }

        if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("fct")))
        {
            m_strPath += _T(".fct");
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        //标题坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCaption);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTCaptionPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptCaption);
                memcpy(lpCustomData->data, &m_ptCaption, sizeof(m_ptCaption));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (size_t)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //标题文本
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + m_strCaption.size() * sizeof(TCHAR);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTCaptionText;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)(m_strCaption.size() * sizeof(TCHAR));
                memcpy(lpCustomData->data, m_strCaption.c_str(), m_strCaption.size() * sizeof(TCHAR));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (size_t)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //自定义文本
        {
            for (const auto& item : m_CustomTextList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt);

                PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
                if (lpCustomData)
                {
                    lpCustomData->dwType = eMDTCustomText;
                    lpCustomData->dwElementCount = 1;
                    lpCustomData->dwElementSize = (DWORD)(sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt));

                    PCUSTOM_TEXT pData = (PCUSTOM_TEXT)lpCustomData->data;
                    pData->pt = item.pt;
                    pData->dwSize = (DWORD)(item.strText.size() * sizeof(TCHAR));
                    memcpy(pData->data, item.strText.c_str(), item.strText.size() * sizeof(TCHAR));

                    cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (size_t)FILE_CURRENT);
                    delete[] lpCustomData;
                }
            }
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdTextReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_TEXT_RESET;
    RecordHistoryData(bManual);

    m_CustomTextList.clear();
    m_CustomTextCount = 0;
    DrawNameTable();

    UpdateHistoryData(bManual);
    return (LRESULT)TRUE;
}


LRESULT CDesignDialog::OnCmdLayoutLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("fcc"), _T("*.fcc") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_CURRENT);

        RecordHistoryData(wParam);

        m_CustomTextList.clear();
        m_BkgTileList.clear();
        m_CustomTextCount = 0;
        m_BkgTileIdCount = 0;

        //读取解析
        PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)cBuffer.GetBuffer();

        while ((LPBYTE)lpCustomData < cBuffer.GetBuffer() + ullSize)
        {
            if (eMenuDataType::eMDTAttributes == lpCustomData->dwType)
            {
                memcpy_s(g_AttributeData, sizeof(g_AttributeData), lpCustomData->data, lpCustomData->dwElementSize);
            }

            if (eMenuDataType::eMDTBkgPalette == lpCustomData->dwType)
            {
                memcpy_s(g_BkgPalette, sizeof(g_BkgPalette), lpCustomData->data, lpCustomData->dwElementSize);
            }

            if (eMenuDataType::eMDTSpritePalette == lpCustomData->dwType)
            {
                memcpy_s(g_SpritePalette, sizeof(g_SpritePalette), lpCustomData->data, lpCustomData->dwElementSize);
            }

            if (eMenuDataType::eMDTCaptionText == lpCustomData->dwType)
            {
                TCHAR szBuf[MAX_PATH] = { 0 };
                memcpy_s(szBuf, sizeof(szBuf), lpCustomData->data, lpCustomData->dwElementSize);
                m_strCaption = szBuf;
            }

            if (eMenuDataType::eMDTCaptionPos == lpCustomData->dwType)
            {
                m_ptCaption = *(POINT*)&lpCustomData->data;
            }

            if (eMenuDataType::eMDTPageNumberPos == lpCustomData->dwType)
            {
                m_ptPageNumber = *(POINT*)&lpCustomData->data;
            }

            if (eMenuDataType::eMDTItemPos == lpCustomData->dwType)
            {
                m_ptItem = *(POINT*)&lpCustomData->data;
            }

            if (eMenuDataType::eMDTCursorTile == lpCustomData->dwType)
            {
                m_tileCursor = *(PTILE_INFO)&lpCustomData->data;
            }

            if (eMenuDataType::eMDTCustomText == lpCustomData->dwType)
            {
                PCUSTOM_TEXT lpCustomText = (PCUSTOM_TEXT)lpCustomData->data;

                TEXT_INFO info;
                info.id = (DWORD)m_CustomTextCount++;
                info.pt = lpCustomText->pt;
                TCHAR szBuf[MAX_PATH] = { 0 };
                memcpy_s(szBuf, sizeof(szBuf), lpCustomText->data, lpCustomText->dwSize);
                info.strText = szBuf;

                m_CustomTextList.push_back(info);
            }

            if (eMenuDataType::eMDTBkgTile == lpCustomData->dwType)
            {
                TILE_INFO info;
                memcpy_s(&info, sizeof(info), lpCustomData->data, lpCustomData->dwElementSize);
                info.id = (DWORD)m_BkgTileIdCount++;
                m_BkgTileList.push_back(info);
            }

            lpCustomData = (PCUSTOM_MENU_DATA)((LPBYTE)lpCustomData + lpCustomData->dwElementSize + sizeof(CUSTOM_MENU_DATA));


            UpdateAttribute();

            DrawTile();
            DrawNameTable();
            DrawBkgPalette();
            DrawSpritePalette();
            DrawAttribute();
            DrawPalette();
            UpdateBkgVramEdit();
            UpdateSpVramEdit();
        }

        UpdateHistoryData(wParam);

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdLayoutSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("fcc"), _T("*.fcc") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("fcc")))
        {
            m_strPath += _T(".fcc");
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        //背景Tile
        {
            for (const auto& item : m_BkgTileList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
                PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
                if (lpCustomData)
                {
                    lpCustomData->dwType = eMenuDataType::eMDTBkgTile;
                    lpCustomData->dwElementCount = 1;
                    lpCustomData->dwElementSize = (DWORD)sizeof(TILE_INFO);
                    memcpy(lpCustomData->data, &item, sizeof(TILE_INFO));

                    cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                    delete[] lpCustomData;
                }
            }
        }

        //属性表
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(g_AttributeData);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTAttributes;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(g_AttributeData);
                memcpy(lpCustomData->data, g_AttributeData, sizeof(g_AttributeData));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //标题坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptCaption);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTCaptionPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptCaption);
                memcpy(lpCustomData->data, &m_ptCaption, sizeof(m_ptCaption));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //标题文本
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + m_strCaption.size() * sizeof(TCHAR);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTCaptionText;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)(m_strCaption.size() * sizeof(TCHAR));
                memcpy(lpCustomData->data, m_strCaption.c_str(), m_strCaption.size() * sizeof(TCHAR));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //页号坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptPageNumber);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTPageNumberPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptPageNumber);
                memcpy(lpCustomData->data, &m_ptPageNumber, sizeof(m_ptPageNumber));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //节目坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(m_ptItem);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTItemPos;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(m_ptItem);
                memcpy(lpCustomData->data, &m_ptItem, sizeof(m_ptItem));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, (DWORD)FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //箭头坐标
        {
            size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(TILE_INFO);
            PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
            if (lpCustomData)
            {
                lpCustomData->dwType = eMenuDataType::eMDTCursorTile;
                lpCustomData->dwElementCount = 1;
                lpCustomData->dwElementSize = (DWORD)sizeof(TILE_INFO);
                memcpy(lpCustomData->data, &m_tileCursor, sizeof(m_tileCursor));

                cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, FILE_CURRENT);
                delete[] lpCustomData;
            }
        }

        //自定义文本
        {
            for (const auto& item : m_CustomTextList)
            {
                size_t dwDataSize = sizeof(CUSTOM_MENU_DATA) + sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt);

                PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA) new (std::nothrow)BYTE[dwDataSize];
                if (lpCustomData)
                {
                    lpCustomData->dwType = eMDTCustomText;
                    lpCustomData->dwElementCount = 1;
                    lpCustomData->dwElementSize = (DWORD)(sizeof(CUSTOM_TEXT) + item.strText.size() * sizeof(TCHAR) + sizeof(item.pt));

                    PCUSTOM_TEXT pData = (PCUSTOM_TEXT)lpCustomData->data;
                    pData->pt = item.pt;
                    pData->dwSize = (DWORD)(item.strText.size() * sizeof(TCHAR));
                    memcpy(pData->data, item.strText.c_str(), item.strText.size() * sizeof(TCHAR));

                    cFile.Write(lpCustomData, (DWORD)dwDataSize, 0, nullptr, FILE_CURRENT);
                    delete[] lpCustomData;
                }
            }
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdLayoutReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_LAYOUT_RESET;

    RecordHistoryData(bManual);

    TCHAR szBuf[MAX_PATH] = { 0 };
    ::StringCchPrintf(szBuf, _countof(szBuf), _T("%d IN 1"), m_strItemListTotal.size());
    m_strCaption = szBuf;
    SetItemString(IDC_EDIT_CAPTION_TEXT, m_strCaption);

    m_CustomTextList.clear();
    m_CustomTextCount = 0;

    m_ptCaption = { (32 - (int)m_strCaption.size()) / 2, 0x2 };

    m_ptPageNumber = { (32 - (int)m_strPageNumber.size()) / 2, 0x1b };

    int nMaxLength = 0;

    for (auto& item : m_strItemListTotal)
    {
        if ((int)item.size() > nMaxLength)
        {
            nMaxLength = (int)item.size();
        }
    }

    if (nMaxLength > 30) nMaxLength = 30;

    m_ptItem = { (32 - nMaxLength) / 2, 0x05 };

    m_tileCursor.pt.x = (m_ptItem.x - 1) * 8 - 2;
    m_tileCursor.pt.y = m_ptItem.y * 8;
    m_tileCursor.data = 0x1F;
    m_tileCursor.mode = 0;

    {
        int nIndex = (m_ptCurNameTable.y) * 8 + (m_ptCurNameTable.x);
        if (nIndex < _countof(g_AttributeData))
        {
            m_AttributeData = g_AttributeData[nIndex];
        }
    }

    OnCmdTextReset(wParam, 0);
    OnCmdBkgListReset(wParam, 0);
    OnCmdAttrReset(wParam, 0);

    ResetCursorTileList();
    DrawTile();
    DrawBkgPalette();
    DrawSpritePalette();
    DrawAttribute();
    DrawPalette();
    DrawNameTable();

    UpdateHistoryData(bManual);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgChrLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("chr"), _T("*.chr") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        RecordHistoryData(wParam);

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_CURRENT);

        if (m_bSelectMode)
        {
            int nBegin = m_ptCurBkgTile.y * 16 + m_ptCurBkgTile.x;
            if (nBegin < 0) nBegin = 0;

            DWORD dwLoadSize = (0x100 - nBegin) * 0x10;

            if ((DWORD)cBuffer.GetSize() <= dwLoadSize)
            {
                memcpy_s(&g_ChrRam[nBegin * 0x10], dwLoadSize, cBuffer.GetBuffer(), cBuffer.GetSize());
            }
            else
            {
                memcpy_s(&g_ChrRam[nBegin * 0x10], dwLoadSize, cBuffer.GetBuffer(), dwLoadSize);
            }
        }
        else
        {
            if ((DWORD)cBuffer.GetSize() <= 0x1000)
            {
                memcpy_s(&g_ChrRam[0], 0x1000, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
            }
            else
            {
                memcpy_s(&g_ChrRam[0], 0x1000, cBuffer.GetBuffer(), 0x1000);
            }
        }

        DrawTile();
        DrawBkgVram();
        DrawNameTable();
        DrawBkgPalette();
        DrawAttribute();
        DrawPalette();
        UpdateBkgVramEdit();

        UpdateHistoryData(wParam);

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgChrSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("chr"), _T("*.chr") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (1 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("chr")))
            {
                m_strPath += _T(".chr");
            }
        }

        if (2 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
            {
                m_strPath += _T(".bin");
            }
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        //CHR
        {
            bool fChecked = false;
            if (m_bSelectMode)
            {
                for (int i = 0; i < 0x100; i++)
                {
                    if (m_bBkgTileCheck[i])
                    {
                        fChecked = true;
                        break;
                    }
                }
            }

            if (fChecked)
            {
                for (int i = 0; i < 0x100; i++)
                {
                    if (m_bBkgTileCheck[i])
                    {
                        cFile.Write(&g_ChrRam[i * 0x10], 0x10, 0, nullptr, FILE_CURRENT);
                    }
                }
            }
            else
            {
                cFile.Write(&g_ChrRam[0], 0x1000, 0, nullptr, FILE_CURRENT);
            }

            cFile.SetEnd(cFile.GetPointer());
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::CDesignDialog::OnCmdBkgChrReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_BKG_CHR_RESET;
    RecordHistoryData(bManual);

    memcpy_s(&g_ChrRam[0], 0x1000, g_ChrRamDefault, 0x1000);

    DrawTile();
    DrawBkgVram();
    DrawNameTable();
    DrawBkgPalette();
    DrawAttribute();
    DrawPalette();
    UpdateBkgVramEdit();

    UpdateHistoryData(bManual);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSwapChr(WPARAM wParam, LPARAM lParam)
{
    BYTE chrTemp[0x1000] = { 0 };

    RecordHistoryData(true);

    memcpy_s(chrTemp, 0x1000, g_ChrRam, 0x1000);
    memcpy_s(&g_ChrRam[0x0000], 0x1000, &g_ChrRam[0x1000], 0x1000);
    memcpy_s(&g_ChrRam[0x1000], 0x1000, chrTemp, 0x1000);

    DrawTile();
    DrawBkgVram();
    DrawSpVram();
    DrawNameTable();
    DrawBkgPalette();
    DrawAttribute();
    DrawPalette();
    UpdateBkgVramEdit();
    UpdateSpVramEdit();

    UpdateHistoryData(true);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpChrLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("chr"), _T("*.chr") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_BEGIN);

        RecordHistoryData(wParam);

        if (m_bSelectMode)
        {
            int nBegin = m_ptCurSpTile.y * 16 + m_ptCurSpTile.x;
            if (nBegin < 0) nBegin = 0;

            DWORD dwLoadSize = (0x100 - nBegin) * 0x10;

            if ((DWORD)cBuffer.GetSize() <= dwLoadSize)
            {
                memcpy_s(&g_ChrRam[0x1000 + nBegin * 0x10], dwLoadSize, cBuffer.GetBuffer(), cBuffer.GetSize());
            }
            else
            {
                memcpy_s(&g_ChrRam[0x1000 + nBegin * 0x10], dwLoadSize, cBuffer.GetBuffer(), dwLoadSize);
            }
        }
        else
        {
            if ((DWORD)cBuffer.GetSize() <= 0x1000)
            {
                memcpy_s(&g_ChrRam[0x1000], 0x1000, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
            }
            else
            {
                memcpy_s(&g_ChrRam[0x1000], 0x1000, cBuffer.GetBuffer(), 0x1000);
            }
        }

        DrawTile();
        DrawSpVram();
        DrawNameTable();
        DrawSpritePalette();
        DrawAttribute();
        DrawPalette();
        UpdateSpVramEdit();

        UpdateHistoryData(wParam);

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpChrSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("chr"), _T("*.chr") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (1 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("chr")))
            {
                m_strPath += _T(".chr");
            }
        }

        if (2 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
            {
                m_strPath += _T(".bin");
            }
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        //CHR
        {
            bool fChecked = false;

            if (m_bSelectMode)
            {
                for (int i = 0; i < 0x100; i++)
                {
                    if (m_bSpTileCheck[i])
                    {
                        fChecked = true;
                        break;
                    }
                }
            }

            if (fChecked)
            {
                for (int i = 0; i < 0x100; i++)
                {
                    if (m_bSpTileCheck[i])
                    {
                        cFile.Write(&g_ChrRam[0x1000 + i * 0x10], 0x10, 0, nullptr, FILE_CURRENT);
                    }
                }
            }
            else
            {
                cFile.Write(&g_ChrRam[0x1000], 0x1000, 0, FILE_BEGIN);
            }

            cFile.SetEnd(cFile.GetPointer());
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::CDesignDialog::OnCmdSpChrReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_SP_CHR_RESET;
    RecordHistoryData(bManual);

    memcpy_s(&g_ChrRam[0x1000], 0x1000, g_ChrRamDefault + 0x1000, 0x1000);
    DrawTile();
    DrawSpVram();
    DrawNameTable();
    DrawSpritePalette();
    DrawAttribute();
    DrawPalette();
    UpdateSpVramEdit();

    UpdateHistoryData(bManual);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgSelAll(WPARAM wParam, LPARAM lParam)
{
    if (m_bShowBgTile)
    {
        RecordHistoryData(true);

        for (auto& item : m_BkgTileList)
        {
            item.flag |= TILE_FLAG_CHECKED;
        }

        DrawNameTable();
        UpdateHistoryData(true);
    }

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgSelInverse(WPARAM wParam, LPARAM lParam)
{
    if (m_bShowBgTile)
    {
        RecordHistoryData(true);
        for (auto& item : m_BkgTileList)
        {
            item.flag ^= 0x01;
        }

        DrawNameTable();
        UpdateHistoryData(true);
    }
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgListLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("nam"), _T("*.nam") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        BYTE bData[0x3C0] = { 0 };
        cFile.Read(bData, sizeof(bData), 0, nullptr, (DWORD)FILE_BEGIN);

        RecordHistoryData(true);

        if (!m_bSelectMode)
        {
            m_BkgTileList.clear();
            m_BkgTileIdCount = 0;
        }

        for (int i = 0; i < sizeof(bData); i++)
        {
            if (0 == bData[i])
            {
                continue;
            }

            TILE_INFO info;
            info.id = (DWORD)m_BkgTileIdCount++;
            info.data = bData[i];
            info.pt.x = i & 0x1F;
            info.pt.y = i >> 5;
            m_BkgTileList.push_back(info);
        }

        DrawTile();
        DrawNameTable();

        UpdateHistoryData(true);

        }
    ).detach();

    return (LRESULT)TRUE;
}

std::vector<uint8_t> GetNametableCompress(const uint8_t* bNtAttrData, uint16_t nNtAttrSize, bool fContainAttr = true)
{
    std::vector<uint8_t> vCompress;
    size_t nOutSize = nNtAttrSize;

    if (!fContainAttr)
    {
        nOutSize = 0x03C0;
    }

    typedef struct _tile_block
    {
        bool flag = false;
        uint8_t count = 0;
        uint8_t data_rep = 0;
        uint16_t addr = 0;

        _tile_block(uint16_t a, uint8_t c, uint8_t d, bool f)
            :
            addr(a), count(c), data_rep(d), flag(f)
        {

        }

        bool operator < (const _tile_block& r) const
        {
            return this->addr < r.addr;
        }

    }tile_block;

    std::deque<tile_block> listData;

    //开始编码
    uint16_t uDataIndex = 0;
    do
    {
        // 处理连续重复数据
        {
            uint16_t uIndex = uDataIndex;
            uint16_t uStart = uDataIndex;
            uint16_t uCount = 0;
            uint8_t uRepeatTile = bNtAttrData[uIndex];

            while (uRepeatTile == bNtAttrData[uIndex] && uIndex < nOutSize)
            {
                if (0x7F == uCount)
                {
                    // 记录背景数量
                    listData.push_back(_tile_block(uDataIndex, uCount, uRepeatTile, false));
                    uDataIndex = uStart + uCount;
                    uStart = uDataIndex;
                    uCount = 0;
                }

                uCount++;
                uIndex++;
            }

            // 记录数据
            if (uCount > 0)
            {
                listData.push_back(_tile_block(uDataIndex, uCount, uRepeatTile, false));
                uDataIndex = uStart + uCount;
                uStart = uDataIndex;
                uCount = 0;
            }
        }

        // 记录非连续重复数据
        {
            uint16_t uStart = uDataIndex;
            uint16_t uIndex = uDataIndex;
            uint16_t uCount = 0;
            uint8_t uLastTile = bNtAttrData[uIndex];

            for (uIndex = uDataIndex + 1; uIndex < nOutSize;)
            {
                if (uLastTile == bNtAttrData[uIndex])
                {
                    break;
                }

                uLastTile = bNtAttrData[uIndex];
                if (0x7F == uCount)
                {
                    listData.push_back(_tile_block(uDataIndex, uCount, 0, true));
                    uDataIndex = uStart + uCount;
                    uStart = uDataIndex;
                    uCount = 0;
                }

                uCount++;
                uIndex++;

            }

            // 记录数据
            if (uCount > 0)
            {
                listData.push_back(_tile_block(uDataIndex, uCount, 0, true));
                uDataIndex = uStart + uCount;
                uStart = uDataIndex;
                uCount = 0;
            }
        }
    } while (uDataIndex < nOutSize);

    for (auto& it : listData)
    {
        if (!it.flag && it.count < 3)
        {
            it.flag = true;
            it.data_rep = 0;
        }
    }

    // 整理重复数据块, 短重复数据拼接到前一个非重复数据上
    {
        auto itData = listData.begin();
        while (itData != listData.end())
        {
            if (itData != listData.begin() && itData->count <= 2 && false == itData->flag)
            {
                auto itTemp = itData;
                auto itFront = itTemp;
                itFront--;

                if (itFront != listData.end() && itFront->flag && itFront->data_rep == itData->data_rep)
                {
                    if (itFront->count + itData->count < 0x7E)
                    {
                        itFront->count += itData->count;
                        itData = listData.erase(itTemp);
                        continue;
                    }
                }
            }

            itData++;
        }
    }

    // 整理非重复数据块, 短数据块拼接
    {
        auto itData = listData.begin();
        while (itData != listData.end())
        {
            if (itData != listData.begin() && true == itData->flag)
            {
                auto itTemp = itData;
                auto itFront = itTemp;
                itFront--;

                if (itFront != listData.end() && itFront->flag == itData->flag)
                {
                    if (itFront->count + itData->count < 0x7E)
                    {
                        itFront->count += itData->count;
                        itData = listData.erase(itTemp);
                        continue;
                    }
                }
            }

            itData++;
        }
    }

    // PPU地址
    vCompress.push_back(0x00);
    vCompress.push_back(0x20);

    for (const auto& it : listData)
    {
        if (it.flag)
        {
            vCompress.push_back(it.count | 0x80);
            for (uint8_t i = 0; i < it.count; i++)
            {
                vCompress.push_back(bNtAttrData[it.addr + i]);
            }
        }
        else
        {
            vCompress.push_back(it.count);
            vCompress.push_back(it.data_rep);
        }
    }

    // 结束符
    vCompress.push_back(0xFF);

    return vCompress;
}

LRESULT CDesignDialog::OnCmdBkgListSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("nam"), _T("*.nam") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        _tstring strCompressNtAttr = CPathUtils::GetFileDir(m_strPath) + _T("\\") + CPathUtils::GetFileName(m_strPath);
        _tstring strCompressNtOnly = CPathUtils::GetFileDir(m_strPath) + _T("\\") + CPathUtils::GetFileName(m_strPath);
        _tstring strWithAttr = CPathUtils::GetFileDir(m_strPath) + _T("\\") + CPathUtils::GetFileName(m_strPath);
        strCompressNtAttr += _T("_cps_with_attr");
        strCompressNtOnly += _T("_cps");
        strWithAttr += _T("_with_attr");

        if (1 == uTypeIndex && 0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("nam")))
        {
            m_strPath += _T(".nam");
        }

        if (2 == uTypeIndex && 0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
        {
            m_strPath += _T(".bin");
        }

        _tstring strExt = CPathUtils::GetFileExt(m_strPath, true);
        strWithAttr += strExt;
        strCompressNtAttr += _T(".bin");
        strCompressNtOnly += _T(".bin");

        static BYTE bNtAttrData[0x400] = { 0 };
        static BYTE bNtData[0x3C0] = { 0 };
        bool fChecked = false;

        memset(bNtData, 0, sizeof(bNtData));
        memset(bNtAttrData, 0, sizeof(bNtAttrData));

        if (m_bSelectMode)
        {

            for (const auto& item : m_BkgTileList)
            {
                if (item.flag & TILE_FLAG_CHECKED)
                {
                    fChecked = true;
                    break;
                }
            }

        }

        if (fChecked)
        {
            for (const auto& item : m_BkgTileList)
            {
                if (item.pt.x < 0 || item.pt.x > 31)
                {
                    continue;
                }

                if (item.pt.y < 0 || item.pt.y > 29)
                {
                    continue;
                }

                if (item.flag & TILE_FLAG_CHECKED)
                {
                    DWORD dwPos = item.pt.y * 0x20 + item.pt.x;
                    bNtData[dwPos] = item.data;
                }
            }
        }
        else
        {
            for (const auto& item : m_BkgTileList)
            {
                if (item.pt.x < 0 || item.pt.x > 31)
                {
                    continue;
                }

                if (item.pt.y < 0 || item.pt.y > 29)
                {
                    continue;
                }

                DWORD dwPos = item.pt.y * 0x20 + item.pt.x;
                bNtData[dwPos] = item.data;
            }
        }

        if (m_bShowList)
        {
            for (int i = 0; i < (LONG)m_strItemList.size(); i++)
            {
                if (m_strItemList[i].empty())
                {
                    continue;
                }

                if ((m_ptItem.y + i) >= 30)
                {
                    break;
                }

                DWORD dwPos = m_ptItem.y * 32 + (32 * i) + m_ptItem.x;

                for (int j = 0; j < m_strItemList[i].size(); j++)
                {
                    if (((dwPos & 0x1F) + j) >= 0x20)
                    {
                        continue;
                    }

                    LPCTSTR lpData = m_strItemList[i].c_str();
                    bNtData[dwPos + j] = g_CharMapData[lpData[j]];
                }
            }
        }

        if (m_bShowText)
        {
            for (const auto& item : m_CustomTextList)
            {
                DWORD dwPos = item.pt.y * 0x20 + item.pt.x;
                _tstring strCustom = item.strText;
                if (item.pt.x < 0)
                {
                    strCustom = strCustom.substr(0 - item.pt.x);
                    dwPos -= item.pt.x;
                }

                if (item.pt.y >= 30)
                {
                    break;
                }

                for (int j = 0; j < strCustom.size(); j++)
                {
                    if (((dwPos & 0x1F) + j) >= 0x20)
                    {
                        continue;
                    }

                    LPCTSTR lpData = strCustom.c_str();
                    bNtData[dwPos + j] = g_CharMapData[lpData[j]];
                }
            }
        }

        if (m_bShowCaption)
        {
            DWORD dwPos = m_ptCaption.y * 0x20 + m_ptCaption.x;
            _tstring strCustom = m_strCaption;
            if (m_ptCaption.x < 0)
            {
                strCustom = strCustom.substr(0 - m_ptCaption.x);
                dwPos -= m_ptCaption.x;
            }

            for (int j = 0; j < strCustom.size(); j++)
            {
                if (((dwPos & 0x1F) + j) >= 0x20)
                {
                    continue;
                }

                LPCTSTR lpData = strCustom.c_str();
                bNtData[dwPos + j] = g_CharMapData[lpData[j]];
            }
        }

        memcpy_s(bNtAttrData, sizeof(bNtAttrData), bNtData, sizeof(bNtData));
        memcpy_s(&bNtAttrData[0x03C0], sizeof(bNtAttrData) - 0x3C0, g_AttributeData, sizeof(g_AttributeData));

        //输出压缩档(含属性表)
        {
            CFileHelper cFile(strCompressNtAttr);
            if (!cFile.IsOpen())
            {
                return;
            }

            auto vCompress = GetNametableCompress(bNtAttrData, sizeof(bNtAttrData), true);

            cFile.Write(vCompress.data(), vCompress.size(), 0, FILE_BEGIN);

            cFile.SetEnd(vCompress.size());
        }

        //输出压缩档(不含属性表)
        {
            CFileHelper cFile(strCompressNtOnly);
            if (!cFile.IsOpen())
            {
                return;
            }

            auto vCompress = GetNametableCompress(bNtData, sizeof(bNtData), false);

            cFile.Write(vCompress.data(), vCompress.size(), 0, FILE_BEGIN);

            cFile.SetEnd(vCompress.size());
        }

        //输出原始档(不含属性表)
        {
            CFileHelper cFile(m_strPath);
            if (!cFile.IsOpen())
            {
                return;
            }

            cFile.Write(bNtData, sizeof(bNtData), 0, FILE_BEGIN);

            cFile.SetEnd(sizeof(bNtData));
        }

        //输出原始档(含属性表)
        {
            CFileHelper cFile(strWithAttr);
            if (!cFile.IsOpen())
            {
                return;
            }

            cFile.Write(bNtAttrData, sizeof(bNtAttrData), 0, FILE_BEGIN);

            cFile.SetEnd(sizeof(bNtAttrData));
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgListReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_BKG_LIST_RESET;
    RecordHistoryData(bManual);

    m_BkgTileList.clear();
    m_BkgTileIdCount = 0;

    DrawNameTable();

    UpdateHistoryData(bManual);

    return (LRESULT)TRUE;
}

void CDesignDialog::TopCheckedBkgListTile()
{
    std::vector<TILE_INFO> vChecked;
    std::vector<TILE_INFO> vNoChecked;

    m_BkgTileIdCount = 0;
    for (auto& item : m_BkgTileList)
    {
        item.id = (DWORD)m_BkgTileIdCount;
        if (item.flag)
        {
            vChecked.push_back(item);
        }
        else
        {
            vNoChecked.push_back(item);
        }
        m_BkgTileIdCount++;
    }

    vNoChecked.insert(vNoChecked.end(), vChecked.begin(), vChecked.end());
    m_BkgTileList = vNoChecked;
}

LRESULT CDesignDialog::OnCmdBkgListLeft(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    TopCheckedBkgListTile();

    int nCheckSize = 0;
    for (auto& item : m_BkgTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_BkgTileList.size()))
    {
        for (auto& item : m_BkgTileList)
        {
            if (item.flag)
            {
                item.pt.x -= 1;
            }
        }
    }
    else
    {
        for (auto& item : m_BkgTileList)
        {
            item.pt.x -= 1;
        }
    }

    DrawNameTable();

    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgListUp(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    TopCheckedBkgListTile();

    int nCheckSize = 0;
    for (auto& item : m_BkgTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_BkgTileList.size()))
    {
        for (auto& item : m_BkgTileList)
        {
            if (item.flag)
            {
                item.pt.y -= 1;
            }
        }
    }
    else
    {
        for (auto& item : m_BkgTileList)
        {
            item.pt.y -= 1;
        }
    }

    DrawNameTable();

    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgListRight(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    TopCheckedBkgListTile();

    int nCheckSize = 0;
    for (auto& item : m_BkgTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_BkgTileList.size()))
    {
        for (auto& item : m_BkgTileList)
        {
            if (item.flag)
            {
                item.pt.x += 1;
            }
        }
    }
    else
    {
        for (auto& item : m_BkgTileList)
        {
            item.pt.x += 1;
        }
    }

    DrawNameTable();
    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgListDown(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    TopCheckedBkgListTile();

    int nCheckSize = 0;
    for (auto& item : m_BkgTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_BkgTileList.size()))
    {
        for (auto& item : m_BkgTileList)
        {
            if (item.flag)
            {
                item.pt.y += 1;
            }
        }
    }
    else
    {
        for (auto& item : m_BkgTileList)
        {
            item.pt.y += 1;
        }
    }

    DrawNameTable();
    UpdateHistoryData(wParam);
    return (LRESULT)TRUE;
}


LRESULT CDesignDialog::OnCmdSpSelAll(WPARAM wParam, LPARAM lParam)
{
    if (m_bShowSpTile)
    {
        RecordHistoryData(true);
        for (auto& item : m_SpTileList)
        {
            item.flag |= TILE_FLAG_CHECKED;
        }

        DrawNameTable();
        UpdateHistoryData(true);
        CheckSpriteAttributeButton();
    }
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpSelInverse(WPARAM wParam, LPARAM lParam)
{
    if (m_bShowSpTile)
    {
        RecordHistoryData(true);
        for (auto& item : m_SpTileList)
        {
            item.flag ^= TILE_FLAG_CHECKED;
        }

        DrawNameTable();
        UpdateHistoryData(true);
        CheckSpriteAttributeButton();
    }
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("spl"), _T("*.spl") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, (DWORD)_countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_BEGIN);

        RecordHistoryData(true);

        LPBYTE lpData = cBuffer.GetBuffer();
        DWORD dwSize = (DWORD)cBuffer.GetSize();
        m_SpTileList.clear();
        m_SpTileIdCount = 0;

        for (DWORD i = 0; i + 3 < dwSize; i += 4)
        {
            if (m_SpTileList.size() >= SPRITE_MAX_SIZE)
            {
                break;
            }

            TILE_INFO info;

            info.id = (DWORD)m_SpTileIdCount++;
            info.pt.y = lpData[i + 0];
            info.pt.x = lpData[i + 3];
            info.data = lpData[i + 1];

            info.color = lpData[i + 2] & 0x03;
            info.mode.bVerticalFlip = (lpData[i + 2] & 0x80) ? true : false;
            info.mode.bHorizontalFlip = (lpData[i + 2] & 0x40) ? true : false;
            info.mode.bBackgroundDisplay = (lpData[i + 2] & 0x20) ? true : false;

            if (info.pt.y > 239) continue;

            m_SpTileList.push_back(info);
        }

        DrawTile();
        DrawNameTable();

        UpdateHistoryData(true);
        CheckSpriteAttributeButton();
        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("spl"), _T("*.spl") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (1 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("spl")))
            {
                m_strPath += _T(".spl");
            }
        }

        if (2 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
            {
                m_strPath += _T(".bin");
            }
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        BYTE bSpritesBuf[0x100] = { 0 };
        memset(bSpritesBuf, 0xF8, sizeof(bSpritesBuf));
        int nIndex = 0;

        bool fChecked = false;

        if (m_bSelectMode)
        {
            for (auto& item : m_SpTileList)
            {
                if (item.flag & TILE_FLAG_CHECKED)
                {
                    fChecked = true;
                    break;
                }
            }
        }

        if (fChecked)
        {
            for (auto& item : m_SpTileList)
            {
                if (nIndex > 64) break;

                if ((item.data & TILE_FLAG_CHECKED) &&
                    (item.pt.y >= 0 && item.pt.y < 240) &&
                    (item.pt.x >= 0 && item.pt.x < 256)
                    )
                {
                    bSpritesBuf[nIndex * 4 + 0] = (BYTE)item.pt.y;
                    bSpritesBuf[nIndex * 4 + 1] = item.data;
                    bSpritesBuf[nIndex * 4 + 2] = 0;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.mode.bVerticalFlip << 0x07;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.mode.bHorizontalFlip << 0x06;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.mode.bBackgroundDisplay << 0x05;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.color;
                    bSpritesBuf[nIndex * 4 + 3] = (BYTE)item.pt.x;
                    nIndex++;
                }
            }
        }
        else
        {
            for (auto& item : m_SpTileList)
            {
                if (nIndex > 64) break;

                if ((item.pt.y >= 0 && item.pt.y < 240) &&
                    (item.pt.x >= 0 && item.pt.x < 256)
                    )
                {
                    bSpritesBuf[nIndex * 4 + 0] = (BYTE)item.pt.y;
                    bSpritesBuf[nIndex * 4 + 1] = item.data;
                    bSpritesBuf[nIndex * 4 + 2] = 0;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.mode.bVerticalFlip << 0x07;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.mode.bHorizontalFlip << 0x06;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.mode.bBackgroundDisplay << 0x05;
                    bSpritesBuf[nIndex * 4 + 2] |= (BYTE)item.color;
                    bSpritesBuf[nIndex * 4 + 3] = (BYTE)item.pt.x;
                    nIndex++;
                }
            }

        }

        cFile.Write(bSpritesBuf, sizeof(bSpritesBuf), 0, nullptr, FILE_CURRENT);
        cFile.SetEnd(cFile.GetPointer());

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_SP_LIST_RESET;
    RecordHistoryData(bManual);

    m_SpTileList.clear();
    m_SpTileIdCount = 0;

    DrawTile();
    DrawNameTable();

    UpdateHistoryData(bManual);
    CheckSpriteAttributeButton();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListLeft(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    int nCheckSize = 0;
    for (auto& item : m_SpTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_SpTileList.size()))
    {
        for (auto& item : m_SpTileList)
        {
            if (item.flag)
            {
                if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
                {
                    item.pt.x -= 8;
                }
                else
                {
                    item.pt.x -= 1;
                }
            }
        }
    }
    else
    {
        for (auto& item : m_SpTileList)
        {
            if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
            {
                item.pt.x -= 8;
            }
            else
            {
                item.pt.x -= 1;
            }
        }
    }

    DrawNameTable();
    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListUp(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    int nCheckSize = 0;
    for (auto& item : m_SpTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_SpTileList.size()))
    {
        for (auto& item : m_SpTileList)
        {
            if (item.flag)
            {
                if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
                {
                    item.pt.y -= 8;
                }
                else
                {
                    item.pt.y -= 1;
                }
            }
        }
    }
    else
    {
        for (auto& item : m_SpTileList)
        {
            if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
            {
                item.pt.y -= 8;
            }
            else
            {
                item.pt.y -= 1;
            }
        }
    }

    DrawNameTable();
    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListRight(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    int nCheckSize = 0;
    for (auto& item : m_SpTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_SpTileList.size()))
    {
        for (auto& item : m_SpTileList)
        {
            if (item.flag)
            {
                if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
                {
                    item.pt.x += 8;
                }
                else
                {
                    item.pt.x += 1;
                }
            }
        }
    }
    else
    {
        for (auto& item : m_SpTileList)
        {
            if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
            {
                item.pt.x += 8;
            }
            else
            {
                item.pt.x += 1;
            }
        }
    }

    DrawNameTable();
    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListDown(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(wParam);

    int nCheckSize = 0;
    for (auto& item : m_SpTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_SpTileList.size()))
    {
        for (auto& item : m_SpTileList)
        {
            if (item.flag)
            {
                if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
                {
                    item.pt.y += 8;
                }
                else
                {
                    item.pt.y += 1;
                }
            }
        }
    }
    else
    {
        for (auto& item : m_SpTileList)
        {
            if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000))
            {
                item.pt.y += 8;
            }
            else
            {
                item.pt.y += 1;
            }
        }
    }

    DrawNameTable();
    UpdateHistoryData(wParam);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpListSwap(WPARAM wParam, LPARAM lParam)
{
    if (!m_bBigSprite)
    {
        return (LRESULT)TRUE;
    }

    RecordHistoryData(true);

    int nCheckSize = 0;
    for (auto& item : m_SpTileList)
    {
        if (item.flag) nCheckSize++;
    }

    if (nCheckSize > 0 && (nCheckSize != (int)m_SpTileList.size()))
    {
        for (auto& item : m_SpTileList)
        {
            if (item.flag & TILE_FLAG_CHECKED)
            {
                item.data ^= 0x01;
            }
        }
    }
    else
    {
        for (auto& item : m_SpTileList)
        {
            item.data ^= 0x01;
        }
    }

    UpdateHistoryData(true);

    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgPaletteLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("pal"), _T("*.pal") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_BEGIN);

        RecordHistoryData(wParam);

        if ((DWORD)cBuffer.GetSize() <= 0x10)
        {
            memcpy_s(&g_BkgPalette[0], 0x10, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
        else
        {
            memcpy_s(&g_BkgPalette[0], 0x10, cBuffer.GetBuffer(), 0x10);
        }

        DrawTile();
        DrawBkgVram();
        DrawNameTable();
        DrawBkgPalette();
        DrawAttribute();
        DrawPalette();
        UpdateBkgVramEdit();

        UpdateHistoryData(wParam);

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgPaletteSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("pal"), _T("*.pal") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (1 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("pal")))
            {
                m_strPath += _T(".pal");
            }
        }

        if (2 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
            {
                m_strPath += _T(".bin");
            }
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        //CHR
        {
            cFile.Write(&g_BkgPalette[0], 0x10, 0, FILE_BEGIN);
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgPaletteReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_BKG_PALETTE_RESET;
    RecordHistoryData(bManual);

    memcpy_s(g_BkgPalette, sizeof(g_BkgPalette), g_BkgDefaultPalette, sizeof(g_BkgDefaultPalette));
    DrawTile();
    DrawBkgVram();
    DrawNameTable();
    DrawBkgPalette();
    DrawAttribute();
    DrawPalette();
    UpdateBkgVramEdit();

    UpdateHistoryData(bManual);

    return (LRESULT)TRUE;
}


LRESULT CDesignDialog::OnCmdSpPaletteLoad(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        static UINT uIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("pal"), _T("*.pal") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetOpenPath(m_hWnd, m_strPath, uIndex, filters, _countof(filters)))
        {
            return;
        }

        CFileHelper cFile(m_strPath, true);
        if (!cFile.IsOpen())
        {
            return;
        }

        unsigned long long ullSize = 0;
        if (!cFile.GetSize(ullSize) || 0 == ullSize)
        {
            return;
        }

        CByteBuffer cBuffer((DWORD)ullSize);
        cFile.Read(cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize(), 0, nullptr, FILE_BEGIN);

        RecordHistoryData(wParam);

        if ((DWORD)cBuffer.GetSize() <= 0x10)
        {
            memcpy_s(&g_SpritePalette[0], 0x10, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize());
        }
        else
        {
            memcpy_s(&g_SpritePalette[0], 0x10, cBuffer.GetBuffer(), 0x10);
        }

        DrawTile();
        DrawSpVram();
        DrawNameTable();
        DrawSpritePalette();
        DrawAttribute();
        DrawPalette();
        UpdateSpVramEdit();

        UpdateHistoryData(wParam);

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpPaletteSave(WPARAM wParam, LPARAM lParam)
{
    std::thread([this, wParam]() {
        UINT uTypeIndex = 0;
        COMDLG_FILTERSPEC filters[] = {
        { _T("pal"), _T("*.pal") },
        { _T("bin"), _T("*.bin") },
        };

        if (!CComFileDlg::GetSavePath(m_hWnd, m_strPath, uTypeIndex, filters, _countof(filters)))
        {
            return;
        }

        if (1 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("pal")))
            {
                m_strPath += _T(".pal");
            }
        }

        if (2 == uTypeIndex)
        {
            if (0 != _tcsicmp(CPathUtils::GetFileExt(m_strPath).c_str(), _T("bin")))
            {
                m_strPath += _T(".bin");
            }
        }

        CFileHelper cFile(m_strPath);
        if (!cFile.IsOpen())
        {
            return;
        }

        //CHR
        {
            cFile.Write(&g_SpritePalette[0], 0x10, 0, FILE_BEGIN);
        }

        }
    ).detach();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpPaletteReset(WPARAM wParam, LPARAM lParam)
{
    bool bManual = wParam == IDC_BUTTON_SP_PALETTE_RESET;
    RecordHistoryData(bManual);

    memcpy_s(g_SpritePalette, sizeof(g_SpritePalette), g_SpriteDefaultPalette, sizeof(g_SpriteDefaultPalette));
    DrawTile();
    DrawSpVram();
    DrawNameTable();
    DrawSpritePalette();
    DrawAttribute();
    DrawPalette();
    UpdateSpVramEdit();
    UpdateHistoryData(bManual);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdAddText(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);
    TEXT_INFO info;

    _tstring strText = GetItemString(IDC_EDIT_CUSTOM_TEXT);
    if (!strText.empty())
    {
        info.strText = strText;
        info.id = (DWORD)m_CustomTextCount++;
        m_CustomTextList.push_back(info);
        DrawNameTable();
    }

    UpdateHistoryData(true);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdVFlip(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);

    for (auto& item : m_SpTileList)
    {
        if (item.flag & TILE_FLAG_CHECKED)
        {
            item.mode.bVerticalFlip = bCheck;
        }
    }

    if (m_bBigCursor)
    {
        for (auto& item : m_CursorTileList)
        {
            if (item.flag & TILE_FLAG_CHECKED)
            {
                item.mode.bVerticalFlip = bCheck;
            }
        }
    }
    else
    {
        if (m_tileCursor.flag & TILE_FLAG_CHECKED)
        {
            m_tileCursor.mode.bVerticalFlip = bCheck;
        }
    }

    DrawNameTable();
    UpdateHistoryData(true);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdHFlip(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);

    for (auto& item : m_SpTileList)
    {
        if (item.flag & TILE_FLAG_CHECKED)
        {
            item.mode.bHorizontalFlip = bCheck;
        }
    }

    if (m_bBigCursor)
    {
        for (auto& item : m_CursorTileList)
        {
            if (item.flag & TILE_FLAG_CHECKED)
            {
                item.mode.bHorizontalFlip = bCheck;
            }
        }
    }
    else
    {
        if (m_tileCursor.flag & TILE_FLAG_CHECKED)
        {
            m_tileCursor.mode.bHorizontalFlip = bCheck;
        }
    }

    DrawNameTable();
    UpdateHistoryData(true);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBackgroundDisplay(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);

    for (auto& item : m_SpTileList)
    {
        if (item.flag & TILE_FLAG_CHECKED)
        {
            item.mode.bBackgroundDisplay = bCheck;
        }
    }

    if (m_bBigCursor)
    {
        for (auto& item : m_CursorTileList)
        {
            if (item.flag & TILE_FLAG_CHECKED)
            {
                item.mode.bBackgroundDisplay = bCheck;
            }
        }
    }
    else
    {
        if (m_tileCursor.flag & TILE_FLAG_CHECKED)
        {
            m_tileCursor.mode.bBackgroundDisplay = bCheck;
        }
    }

    DrawNameTable();
    UpdateHistoryData(true);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBigCursor(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);
    WORD wID = LOWORD(wParam);
    m_bBigCursor = IsDlgButtonChecked(m_hWnd, wID);

    CRegUtils::SetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_BIG_CURSOR, &m_bBigCursor, sizeof(m_bBigCursor));

    UpdateHistoryData(true);

    DrawNameTable();
    CheckSpriteAttributeButton();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBigSprite(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);
    WORD wID = LOWORD(wParam);
    m_bBigSprite = IsDlgButtonChecked(m_hWnd, wID);

    CRegUtils::SetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_BIG_SPRITE, &m_bBigSprite, sizeof(m_bBigSprite));

    UpdateHistoryData(true);
    UpdateCursorTileList();
    DrawNameTable();
    DrawSpVram();
    CheckSpriteAttributeButton();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdCharMappingQuick(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);
    m_bQuickCharMappingSrc = bCheck;
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgChrQuick(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);
    m_bQuickBkgChrDest = bCheck;
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpChrQuick(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);
    m_bQuickSpChrDest = bCheck;
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBkgListQuick(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    bool bCheck = IsDlgButtonChecked(m_hWnd, wID);
    m_bQuickBkgList = bCheck;
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdFileExit(WPARAM wParam, LPARAM lParam)
{
    PostMessage(WM_CLOSE, 0, 0);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdClearUnuseBkgTile(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    //检查tile列表
    DWORD tileCount[0x100] = { 0 };
    for (const auto& item : m_BkgTileList)
    {
        tileCount[(BYTE)item.data]++;
    }

    //检查列表名
    for (const auto& item : m_strItemListTotal)
    {
        for (const auto& ch : item)
        {
            if (ch >= 0x100)
            {
                continue;
            }

            tileCount[g_CharMapData[ch]]++;
        }
    }

    //检查自定义文本
    for (const auto& item : m_CustomTextList)
    {
        for (const auto& ch : item.strText)
        {
            if (ch >= 0x100)
            {
                continue;
            }

            tileCount[g_CharMapData[ch]]++;
        }
    }

    //标题名
    for (const auto& ch : m_strCaption)
    {
        if (ch >= 0x100)
        {
            continue;
        }

        tileCount[g_CharMapData[ch]]++;
    }

    //页号
    if (m_bShowPage)
    {
        for (const auto& ch : m_strPageNumber)
        {
            if (ch >= 0x100)
            {
                continue;
            }

            tileCount[g_CharMapData[ch]]++;
        }

        DWORD dwPageCount = (DWORD)(m_strItemListTotal.size() / m_nPageSize);
        if (0 != m_strItemListTotal.size() % m_nPageSize)
        {
            dwPageCount++;
        }

        if (dwPageCount > 9)
        {
            dwPageCount = 9;
        }

        dwPageCount += '0';
        for (DWORD ch = '0'; ch <= dwPageCount; ch++)
        {
            tileCount[g_CharMapData[ch]]++;
        }
    }

    if (m_bBigSprite)
    {
        for (const auto& item : m_SpTileList)
        {
            if (item.data & TILE_FLAG_CHECKED)
            {
                continue;
            }

            tileCount[(item.data & ~TILE_FLAG_CHECKED)]++;
            tileCount[(item.data | TILE_FLAG_CHECKED)]++;
        }
    }

    BYTE vramEmpty[0x10] = { 0 };
    LPBYTE lpBase = g_ChrRam;
    for (int i = 0; i < _countof(tileCount); i++)
    {
        if (0 == tileCount[i])
        {
            LPBYTE lpData = lpBase + (i * 0x10);
            memcpy_s(lpData, 0x10, vramEmpty, sizeof(vramEmpty));
        }
    }

    UpdateHistoryData(true);
    DrawTile(0, 4);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdClearUnuseSpTile(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    //检查tile列表
    DWORD tileCount[0x100] = { 0 };

    if (m_bBigSprite)
    {
        for (const auto& item : m_SpTileList)
        {
            if (!(item.data & TILE_FLAG_CHECKED))
            {
                continue;
            }

            tileCount[(item.data & ~TILE_FLAG_CHECKED)]++;
            tileCount[(item.data | TILE_FLAG_CHECKED)]++;
        }

        for (const auto& item : m_CursorTileList)
        {
            if (!(item.data & TILE_FLAG_CHECKED))
            {
                continue;
            }

            tileCount[(item.data & ~TILE_FLAG_CHECKED)]++;
            tileCount[(item.data | TILE_FLAG_CHECKED)]++;
        }

        tileCount[(m_tileCursor.data & ~TILE_FLAG_CHECKED)]++;
        tileCount[(m_tileCursor.data | TILE_FLAG_CHECKED)]++;
    }
    else
    {
        for (const auto& item : m_SpTileList)
        {
            tileCount[(BYTE)item.data]++;
        }

        for (const auto& item : m_CursorTileList)
        {
            tileCount[item.data]++;
        }

        tileCount[m_tileCursor.data]++;
    }

    BYTE vramEmpty[0x10] = { 0 };
    LPBYTE lpBase = &g_ChrRam[0x1000];
    for (int i = 0; i < _countof(tileCount); i++)
    {
        if (0 == tileCount[i])
        {
            LPBYTE lpData = lpBase + (i * 0x10);
            memcpy_s(lpData, 0x10, vramEmpty, sizeof(vramEmpty));
        }
    }

    UpdateHistoryData(true);

    //重绘界面
    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBgOrganize(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    BYTE vramEmpty[0x10] = { 0 };
    size_t tileSize = 0x10;
    LPBYTE lpBase = &g_ChrRam[0x0000];

    std::vector<TILE_INFO> vTempList;
    for (const auto& item : m_BkgTileList)
    {
        LPBYTE lpSrcData = lpBase + (item.data * tileSize);

        if (0 != memcmp(vramEmpty, lpSrcData, tileSize))
        {
            vTempList.push_back(item);
        }
    }

    m_BkgTileList = vTempList;
    for (int i = 0; i < m_BkgTileList.size(); i++)
    {
        m_BkgTileList[i].id = i;
    }
    m_BkgTileIdCount = m_BkgTileList.size();

    //替换
    for (int i = 1; i < 256; i++)
    {
        LPBYTE lpDest = lpBase + (i * 0x10);

        //检查是否为空Tile
        if (0 != memcmp(vramEmpty, lpDest, tileSize))
        {
            continue;
        }

        //查找后面第一个不为空的Tile
        for (int j = i + 1; j < 256; j++)
        {
            LPBYTE lpSrcData = lpBase + (j * tileSize);
            if (0 != memcmp(vramEmpty, lpSrcData, tileSize))
            {
                memcpy_s(lpDest, tileSize, lpSrcData, tileSize);
                memcpy_s(lpSrcData, tileSize, vramEmpty, tileSize);

                // 替换精灵组
                for (auto& item : m_BkgTileList)
                {
                    if (item.data == j)
                    {
                        item.data = (BYTE)i;
                    }
                }

                // 替换字符映射
                for (auto& item : g_CharMapData)
                {
                    if (item == j)
                    {
                        item = (BYTE)i;
                    }
                }

                break;
            }
        }
    }

    UpdateHistoryData(true);

    //重绘界面
    DrawTile(0, 4);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();


    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpOrganize(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    BYTE vramBigEmpty[0x20] = { 0 };
    BYTE vramEmpty[0x10] = { 0 };
    size_t tileSize = 0x10;
    LPBYTE lpBase = &g_ChrRam[0x1000];

    if (m_bBigSprite)
    {
        tileSize = 0x20;

        //替换
        for (int i = 0; i < 128; i++)
        {
            LPBYTE lpDest = lpBase + (i * tileSize);

            //检查是否为空Tile
            if (0 != memcmp(vramBigEmpty, lpDest, tileSize))
            {
                continue;
            }

            //查找后面第一个不为空的Tile
            for (int j = i + 2; j < 128; j++)
            {
                LPBYTE lpSrcData = lpBase + (j * tileSize);
                if (0 != memcmp(vramBigEmpty, lpSrcData, tileSize))
                {
                    memcpy_s(lpDest, tileSize, lpSrcData, tileSize);
                    memcpy_s(lpSrcData, tileSize, vramBigEmpty, tileSize);

                    // 精灵组
                    for (auto& item : m_SpTileList)
                    {
                        if (item.data == (j * 2 + 1))
                        {
                            item.data = (BYTE)(i * 2 + 1);
                        }
                    }

                    // 替换光标组
                    for (auto& item : m_CursorTileList)
                    {
                        if (item.data == (j * 2 + 1))
                        {
                            item.data = (BYTE)(i * 2 + 1);
                        }
                    }

                    // 替换光标
                    if (m_tileCursor.data == (j * 2 + 1))
                    {
                        m_tileCursor.data = (BYTE)(i * 2 + 1);
                    }

                    break;
                }
            }
        }
    }
    else
    {
        std::vector<TILE_INFO> vTempList;
        for (const auto& item : m_SpTileList)
        {
            LPBYTE lpSrcData = lpBase + (item.data * tileSize);

            if (0 != memcmp(vramEmpty, lpSrcData, tileSize))
            {
                vTempList.push_back(item);
            }
        }

        m_SpTileList = vTempList;
        for (int i = 0; i < m_SpTileList.size(); i++)
        {
            m_SpTileList[i].id = i;
        }
        m_SpTileIdCount = m_SpTileList.size();

        //替换
        for (int i = 1; i < 256; i++)
        {
            LPBYTE lpDest = lpBase + (i * tileSize);

            //检查是否为空Tile
            if (0 != memcmp(vramEmpty, lpDest, tileSize))
            {
                continue;
            }

            //查找后面第一个不为空的Tile
            for (int j = i + 1; j < 256; j++)
            {
                LPBYTE lpSrcData = lpBase + (j * tileSize);
                if (0 != memcmp(vramEmpty, lpSrcData, tileSize))
                {
                    memcpy_s(lpDest, tileSize, lpSrcData, tileSize);
                    memcpy_s(lpSrcData, tileSize, vramEmpty, tileSize);

                    // 精灵组
                    for (auto& item : m_SpTileList)
                    {
                        if (item.data == j)
                        {
                            item.data = (BYTE)i;
                        }
                    }

                    // 替换光标组
                    for (auto& item : m_CursorTileList)
                    {
                        if (item.data == j)
                        {
                            item.data = (BYTE)i;
                        }
                    }

                    // 替换光标
                    if (m_tileCursor.data == j)
                    {
                        m_tileCursor.data = (BYTE)i;
                    }

                    break;
                }
            }
        }
    }

    UpdateHistoryData(true);

    //重绘界面
    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSp16To8(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    std::vector<TILE_INFO> vTemp;

    m_SpTileIdCount = 0;
    for (auto& item : m_SpTileList)
    {
        TILE_INFO info = item;

        info.data &= ~TILE_FLAG_CHECKED;
        info.id = (DWORD)m_SpTileIdCount++;
        vTemp.push_back(info);

        info.pt.y += 8;
        info.data |= TILE_FLAG_CHECKED;
        info.id = (DWORD)m_SpTileIdCount++;
        vTemp.push_back(info);
    }

    //vTemp = m_SpTileList;
    m_SpTileList = vTemp;

    DrawNameTable();

    UpdateHistoryData(true);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpIdSwap(WPARAM wParam, LPARAM lParam)
{
    if (m_bBigSprite)
    {
        RecordHistoryData(true);

        for (auto& item : m_SpTileList)
        {
            item.data ^= TILE_FLAG_CHECKED;
        }

        DrawNameTable();

        UpdateHistoryData(true);
    }

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBgTileToSpTile(WPARAM wParam, LPARAM lParam)
{
    if (m_bBigSprite)
    {
        return (LRESULT)TRUE;
    }

    RecordHistoryData(true);

    std::vector<TILE_INFO> tmpTileList;

    for (auto& item : m_BkgTileList)
    {
        if (!(item.flag & TILE_FLAG_CHECKED))
        {
            tmpTileList.push_back(item);
            continue;
        }

        TILE_INFO info;
        info.data = 22;

        LPBYTE lpChrBgBase = &g_ChrRam[0];
        LPBYTE lpChrSpBase = &g_ChrRam[0x1000];

        // 查找可复用Tile
        int nSpIndex = -1;
        for (int i = 0; i < 0x100; i++)
        {
            LPBYTE lpSrcData = lpChrBgBase + item.data * 0x10;
            LPBYTE lpDestData = lpChrSpBase + i * 0x10;
            if (0 == memcmp(lpSrcData, lpDestData, 0x10))
            {
                nSpIndex = i;
                break;
            }
        }

        if (nSpIndex >= 0)
        {
            info.data = (BYTE)nSpIndex;
        }
        else
        {
            BYTE vramEmpty[0x10] = { 0 };
            for (int i = 0; i < 0x100; i++)
            {
                LPBYTE lpDestData = lpChrSpBase + i * 0x10;
                if (0 == memcmp(vramEmpty, lpDestData, 0x10))
                {
                    LPBYTE lpSrcData = lpChrBgBase + item.data * 0x10;

                    nSpIndex = i;
                    memcpy(lpDestData, lpSrcData, 0x10);

                    break;
                }
            }
        }

        if (-1 == nSpIndex)
        {
            break;
        }

        info.data = (BYTE)nSpIndex;
        info.pt.x = item.pt.x * 8;
        info.pt.y = item.pt.y * 8 - 1;

        info.color = (BYTE)GetNameTableColorIndex({ item.pt.x, item.pt.y });

        info.id = (DWORD)m_SpTileIdCount++;
        m_SpTileList.push_back(info);
    }

    m_BkgTileIdCount = 0;
    for (auto& item : tmpTileList)
    {
        item.id = (DWORD)m_BkgTileIdCount++;
    }
    m_BkgTileList = tmpTileList;

    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    DrawNameTable();

    UpdateHistoryData(true);
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdAutoCharMapping(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    std::set<char> charSets;

    int nBegin = m_ptCurBkgTile.y * 16 + m_ptCurBkgTile.x;
    if (nBegin < 0)
    {
        nBegin = 0;
    }

    //检查列表名
    for (const auto& item : m_strItemListTotal)
    {
        for (const auto& ch : item)
        {
            if (ch >= 0x100)
            {
                continue;
            }

            charSets.insert((char)ch);
        }
    }

    //标题名
    for (const auto& ch : m_strCaption)
    {
        if (ch >= 0x100)
        {
            continue;
        }

        charSets.insert((char)ch);
    }

    //页号
    if (m_bShowPage)
    {
        for (const auto& ch : m_strPageNumber)
        {
            if (ch >= 0x100)
            {
                continue;
            }

            charSets.insert((char)ch);
        }

        DWORD dwPageCount = (DWORD)(m_strItemListTotal.size() / m_nPageSize);
        if (0 != m_strItemListTotal.size() % m_nPageSize)
        {
            dwPageCount++;
        }

        if (dwPageCount > 9)
        {
            dwPageCount = 9;
        }

        dwPageCount += '0';
        for (DWORD ch = '0'; ch <= dwPageCount; ch++)
        {
            charSets.insert((char)ch);
        }
    }

    if (m_bBigSprite)
    {
        for (const auto& item : m_SpTileList)
        {
            if (item.data & TILE_FLAG_CHECKED)
            {
                continue;
            }

            charSets.insert((item.data & ~TILE_FLAG_CHECKED));
            charSets.insert((item.data | TILE_FLAG_CHECKED));
        }
    }

    memset(g_CharMapData, 0, sizeof(g_CharMapData));

    uint8_t emptyTile[0x10] = { 0 };
    LPBYTE lpBase = g_ChrRam;
    for (int i = 0; i < 0x40; i++)
    {
        if (nBegin >= 0x100)
        {
            break;
        }

        // 查找空位
        bool fFind = false;
        for (int j = nBegin; j < 0x100; j++)
        {
            if (0 == memcmp(emptyTile, lpBase + j * 0x10, 0x10))
            {
                fFind = true;
                nBegin = j;
                break;
            }
        }

        if (!fFind)
        {
            break;
        }

        auto itFind = charSets.find((char)(0x20 + i));
        if (itFind == charSets.end())
        {
            continue;
        }

        LPBYTE lpData = lpBase + (nBegin * 0x10);
        memcpy_s(lpData, 0x10, &g_ChrRamDefault[(0x20 + i) * 0x10], 0x10);
        g_CharMapData[0x20 + i] = (BYTE)nBegin;
        nBegin++;
    }

    UpdateHistoryData(true);

    DrawCharMappingSource();
    DrawCharMappingTarget();
    DrawTile(0, 4, 0, 0);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdAutoFullCharMapping(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    int nBegin = m_ptCurBkgTile.y * 16 + m_ptCurBkgTile.x;
    if (nBegin < 0)
    {
        nBegin = 0;
    }

    uint8_t emptyTile[0x10] = { 0 };
    LPBYTE lpBase = g_ChrRam;
    for (int i = 0; i < 0x40; i++)
    {
        if (nBegin >= 0x100)
        {
            break;
        }

        bool fFind = false;
        for (int j = nBegin; j < 0x100; j++)
        {
            if (0 == memcmp(emptyTile, lpBase + j * 0x10, 0x10))
            {
                fFind = true;
                nBegin = j;
                break;
            }
        }

        if (!fFind)
        {
            break;
        }

        LPBYTE lpData = lpBase + (nBegin * 0x10);
        memcpy_s(lpData, 0x10, &g_ChrRamDefault[(0x20 + i) * 0x10], 0x10);
        g_CharMapData[0x20 + i] = (BYTE)nBegin;
        nBegin++;
    }

    UpdateHistoryData(true);

    DrawTile(0, 4, 0, 0);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdCopyBgToSp(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    int nBegin = m_ptCurSpTile.y * 16 + m_ptCurSpTile.x;
    if (nBegin < 0) nBegin = 0;

    //查找首行
    int nStartRow = 0;
    int nStartCol = 0;
    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (nBegin > 0x100)
        {
            break;
        }

        if (m_bBkgTileCheck[i])
        {
            nStartRow = i / 16;
            nStartCol = i % 16;
            break;
        }
    }

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (nBegin > 0x100)
        {
            break;
        }

        if ((i / 16) < nStartRow)
        {
            continue;
        }

        int nRowOffset = (i / 16) - nStartRow;
        int nColOffset = (i % 16) - nStartCol;
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpSrcTile = 0;
            LPBYTE lpDestTile = 0;

            lpSrcTile = g_ChrRam + i * 0x10;
            lpDestTile = g_ChrRam + 0x1000 + (nBegin + nColOffset + nRowOffset * 0x10) * 0x10;
            memcpy_s(lpDestTile, 0x10, lpSrcTile, 0x10);

            if ((i + 0x10 < 0x100) && m_bBkgTileCheck[i + 0x10])
            {
                lpSrcTile = g_ChrRam + (i + 0x10) * 0x10;
                lpDestTile = g_ChrRam + 0x1000 + (nBegin + nColOffset + nRowOffset * 0x10 + 0x10) * 0x10;
                memcpy_s(lpDestTile, 0x10, lpSrcTile, 0x10);
            }
        }
    }

    UpdateHistoryData(true);

    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdCopySpToBg(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    int nBegin = m_ptCurBkgTile.y * 16 + m_ptCurBkgTile.x;
    if (nBegin < 0) nBegin = 0;

    //查找首行
    int nStartRow = 0;
    int nStartCol = 0;
    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (nBegin > 0x100)
        {
            break;
        }

        if (m_bSpTileCheck[i])
        {
            nStartRow = i / 16;
            nStartCol = i % 16;
            break;
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (nBegin > 0x100)
        {
            break;
        }

        if ((i / 16) < nStartRow)
        {
            continue;
        }

        int nRowOffset = (i / 16) - nStartRow;
        int nColOffset = (i % 16) - nStartCol;
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpSrcTile = 0;
            LPBYTE lpDestTile = 0;

            lpSrcTile = g_ChrRam + 0x1000 + i * 0x10;
            lpDestTile = g_ChrRam + (nBegin + nColOffset + nRowOffset * 0x10) * 0x10;
            memcpy_s(lpDestTile, 0x10, lpSrcTile, 0x10);

            if ((i + 0x10 < 0x100) && m_bBkgTileCheck[i + 0x10])
            {
                lpSrcTile = g_ChrRam + 0x1000 + (i + 0x10) * 0x10;
                lpDestTile = g_ChrRam + (nBegin + nColOffset + nRowOffset * 0x10 + 0x10) * 0x10;
                memcpy_s(lpDestTile, 0x10, lpSrcTile, 0x10);
            }
        }
    }

    UpdateHistoryData(true);

    DrawTile(0, 4, 0, 0);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdBgCharMapping(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    int nPos = m_ptCurBkgTile.y * 16 + m_ptCurBkgTile.x;
    if (nPos < 0)
    {
        nPos = 0;
    }

    LPBYTE lpBase = g_ChrRam;
    for (int i = 0; i < 0x40; i++)
    {
        if (nPos + i >= 0x100)
        {
            break;
        }

        LPBYTE lpData = lpBase + ((nPos + i) * 0x10);
        memcpy_s(lpData, 0x10, &g_ChrRamDefault[(0x20 + i) * 0x10], 0x10);
    }

    UpdateHistoryData(true);

    DrawTile(0, 4, 0, 0);
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSpCharMapping(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    int nPos = m_ptCurSpTile.y * 16 + m_ptCurSpTile.x;
    if (nPos < 0)
    {
        nPos = 0;
    }

    LPBYTE lpBase = g_ChrRam + 0x1000;
    for (int i = 0; i < 0x40; i++)
    {
        if (nPos + i >= 0x100)
        {
            break;
        }

        LPBYTE lpData = lpBase + ((nPos + i) * 0x10);
        memcpy_s(lpData, 0x10, &g_ChrRamDefault[(0x20 + i) * 0x10], 0x10);
    }

    UpdateHistoryData(true);

    DrawTile(0, 0, 0, 4);
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdDeleteColor0(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            DeleteTileColor(lpData, 0);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            DeleteTileColor(lpData, 0);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdDeleteColor1(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            DeleteTileColor(lpData, 1);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            DeleteTileColor(lpData, 1);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdDeleteColor2(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            DeleteTileColor(lpData, 2);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            DeleteTileColor(lpData, 2);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdDeleteColor3(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            DeleteTileColor(lpData, 3);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            DeleteTileColor(lpData, 3);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveColorLeft123(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTileColor123(lpData, false);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTileColor123(lpData, false);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveColorRight123(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTileColor123(lpData, true);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTileColor123(lpData, true);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveColorLeft0123(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTileColor0123(lpData, false);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTileColor0123(lpData, false);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveColorRight0123(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTileColor0123(lpData, true);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTileColor0123(lpData, true);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMovePixelLeft(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpLeft);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpLeft);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMovePixelRight(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpRight);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpRight);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMovePixelUp(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpUp);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpUp);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMovePixelDown(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpDown);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            MoveTilePixel(lpData, eMoveTilePixelType::eMtpDown);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

void CDesignDialog::MoveAttributes(eMoveAttributesType eType)
{
    BYTE attributeColors[16][16] = { 0 };
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            BYTE data = g_AttributeData[(i * 8 + j)];
            DWORD dwLTIndex = (data & 0x03) >> 0;
            DWORD dwRTIndex = (data & 0x0C) >> 2;
            DWORD dwLBIndex = (data & 0x30) >> 4;
            DWORD dwRBIndex = (data & 0xC0) >> 6;

            //左上
            attributeColors[i * 2 + 0][j * 2 + 0] = (BYTE)dwLTIndex;

            //右上
            attributeColors[i * 2 + 0][j * 2 + 1] = (BYTE)dwRTIndex;

            //左下
            attributeColors[i * 2 + 1][j * 2 + 0] = (BYTE)dwLBIndex;

            //右下
            attributeColors[i * 2 + 1][j * 2 + 1] = (BYTE)dwRBIndex;
        }
    }

    if (eType == eMoveAttributesType::eMatLeft)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                attributeColors[i][j] = attributeColors[i][j + 1];
            }
        }

        for (int i = 0; i < 16; i++)
        {
            attributeColors[i][15] = 0;
        }
    }

    if (eType == eMoveAttributesType::eMatRight)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 15; j > 0; j--)
            {
                attributeColors[i][j] = attributeColors[i][j - 1];
            }
        }

        for (int i = 0; i < 16; i++)
        {
            attributeColors[i][0] = 0;
        }
    }

    if (eType == eMoveAttributesType::eMatUp)
    {
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                attributeColors[i][j] = attributeColors[i + 1][j];
            }
        }

        for (int i = 0; i < 16; i++)
        {
            attributeColors[14][i] = 0;
        }
    }

    if (eType == eMoveAttributesType::eMatDown)
    {
        for (int i = 15; i > 0; i--)
        {
            for (int j = 0; j < 16; j++)
            {
                attributeColors[i][j] = attributeColors[i - 1][j];
            }
        }

        for (int i = 0; i < 16; i++)
        {
            attributeColors[0][i] = 0;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            //左上
            DWORD dwLTIndex = attributeColors[i * 2 + 0][j * 2 + 0];

            //右上
            DWORD dwRTIndex = attributeColors[i * 2 + 0][j * 2 + 1];

            //左下
            DWORD dwLBIndex = attributeColors[i * 2 + 1][j * 2 + 0];

            //右下
            DWORD dwRBIndex = attributeColors[i * 2 + 1][j * 2 + 1];

            BYTE data = (BYTE)(dwRBIndex << 6 | dwLBIndex << 4 | dwRTIndex << 2 | dwLTIndex);

            g_AttributeData[(i * 8 + j)] = data;
        }
    }

}

LRESULT CDesignDialog::OnCmdMoveAttrLeft(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    MoveAttributes(eMoveAttributesType::eMatLeft);
    DrawNameTable();

    UpdateHistoryData(true);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveAttrRight(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    MoveAttributes(eMoveAttributesType::eMatRight);
    DrawNameTable();

    UpdateHistoryData(true);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveAttrUp(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    MoveAttributes(eMoveAttributesType::eMatUp);
    DrawNameTable();

    UpdateHistoryData(true);

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMoveAttrDown(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    MoveAttributes(eMoveAttributesType::eMatDown);
    DrawNameTable();

    UpdateHistoryData(true);

    return (LRESULT)TRUE;
}


LRESULT CDesignDialog::OnCmdSwapColor12(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            SwapTileColor(lpData, 1, 2);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            SwapTileColor(lpData, 1, 2);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSwapColor13(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            SwapTileColor(lpData, 1, 3);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            SwapTileColor(lpData, 1, 3);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdSwapColor23(WPARAM wParam, LPARAM lParam)
{
    RecordHistoryData(true);

    for (int i = 0; i < _countof(m_bBkgTileCheck); i++)
    {
        if (m_bBkgTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[i * 0x10];
            SwapTileColor(lpData, 2, 3);
        }
    }

    for (int i = 0; i < _countof(m_bSpTileCheck); i++)
    {
        if (m_bSpTileCheck[i])
        {
            LPBYTE lpData = &g_ChrRam[0x1000 + i * 0x10];
            SwapTileColor(lpData, 2, 3);
        }
    }

    UpdateHistoryData(true);

    DrawTile();
    DrawBkgVram();
    UpdateBkgVramEdit();
    DrawSpVram();
    UpdateSpVramEdit();
    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdMarkCursor(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    m_bMarkCursor = IsDlgButtonChecked(m_hWnd, wID);

    DrawNameTable();
    DrawBkgVram();
    DrawSpVram();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdShowBgTile(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    DWORD dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), wID, MF_BYCOMMAND | (MF_CHECKED & dwState ? MF_UNCHECKED : MF_CHECKED));
    dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);

    m_bShowBgTile = dwState;

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdShowSpTile(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    DWORD dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), wID, MF_BYCOMMAND | (MF_CHECKED & dwState ? MF_UNCHECKED : MF_CHECKED));
    dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);

    m_bShowSpTile = dwState;

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdShowList(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    DWORD dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), wID, MF_BYCOMMAND | (MF_CHECKED & dwState ? MF_UNCHECKED : MF_CHECKED));
    dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);

    m_bShowList = dwState;

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdShowCustomText(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    DWORD dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), wID, MF_BYCOMMAND | (MF_CHECKED & dwState ? MF_UNCHECKED : MF_CHECKED));
    dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);

    m_bShowText = dwState;

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdShowCaption(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    DWORD dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), wID, MF_BYCOMMAND | (MF_CHECKED & dwState ? MF_UNCHECKED : MF_CHECKED));
    dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);

    m_bShowCaption = dwState;

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdShowCursor(WPARAM wParam, LPARAM lParam)
{
    WORD wID = LOWORD(wParam);
    DWORD dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), wID, MF_BYCOMMAND | (MF_CHECKED & dwState ? MF_UNCHECKED : MF_CHECKED));
    dwState = GetMenuState(GetMenu(m_hWnd), wID, MF_BYCOMMAND | MF_CHECKED);

    m_bShowCursor = dwState;

    DrawNameTable();

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnColor(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = (HDC)wParam;
    SetBkMode(hdc, TRANSPARENT);
    if (NULL == m_hBrush)
    {
        m_hBrush = CreateSolidBrush(RGB(190, 185, 255));
    }

    return (LRESULT)m_hBrush;
}

LRESULT CDesignDialog::OnLanguageChange(WPARAM wParam, LPARAM lParam)
{
    m_LangId = (LANGID)wParam;
    SetThreadUILanguage(m_LangId);

    ::SetWindowText(m_hWnd, LoadString(IDS_STRING_CUSTOM_MENU_TEXT).c_str());

    ::SetDlgItemText(m_hWnd, IDC_STATIC_CAPTION_TEXT, LoadString(IDS_STRING_CUSTOM_CAPTION_TEXT).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_FILL_COLOR, LoadString(IDS_STRING_CUSTOM_CHECK_FILL).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_DRAW_LINE, LoadString(IDS_STRING_CUSTOM_CHECK_LINE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_DRAG_TEXT, LoadString(IDS_STRING_CUSTOM_CHECK_DRAG).c_str());

    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_NAME_TABLE, LoadString(IDS_STRING_CUSTOM_ATTRIBUTES).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_COLOR_ATTRIBUTE, LoadString(IDS_STRING_CUSTOM_ATTRIBUTE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_BKG_PALETTE, LoadString(IDS_STRING_CUSTOM_BKG_PALETTE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_SPRITE_PALETTE, LoadString(IDS_STRING_CUSTOM_SPRITE_PALETTE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_PALETTE, LoadString(IDS_STRING_CUSTOM_PALETTE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_CHR, LoadString(IDS_STRING_CUSTOM_CHR).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_UNDO, LoadString(IDS_STRING_UNDO).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_REDO, LoadString(IDS_STRING_REDO).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_LAYOUT_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_LAYOUT_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_LAYOUT_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_TEXT_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_TEXT_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_TEXT_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_ATTR, LoadString(IDS_STRING_ATTRIBUTE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_ATTR_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_ATTR_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_ATTR_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_CHR_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_CHR_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_CHR_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SWAP_CHR, LoadString(IDS_STRING_SWAP).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_CHR_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_CHR_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_CHR_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_PALETTE_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_PALETTE_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_PALETTE_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_PALETTE_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_PALETTE_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_PALETTE_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_LIST_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_LIST_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_LIST_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_LIST_LOAD, LoadString(IDS_STRING_LOAD).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_LIST_SAVE_AS, LoadString(IDS_STRING_SAVE_AS).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_LIST_RESET, LoadString(IDS_STRING_RESET).c_str());

    ::SetDlgItemText(m_hWnd, IDC_STATIC_CUSTOM_TEXT, LoadString(IDS_STRING_CUSTOM_TEXT).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_TEXT_ADD, LoadString(IDS_STRING_ADD).c_str());

    ::SetDlgItemText(m_hWnd, IDC_CHECK_SPRITE_V, LoadString(IDS_STRING_SPRITE_FLIP_V).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_SPRITE_H, LoadString(IDS_STRING_SPRITE_FLIP_H).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_SPRITE_BACK, LoadString(IDS_STRING_SPRITE_BACKGROUND).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_SELECT_ALL, LoadString(IDS_STRING_SELECT_ALL).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_BKG_SELECT_INVERSE, LoadString(IDS_STRING_SELECT_INVERSE).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_SELECT_ALL, LoadString(IDS_STRING_SELECT_ALL).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SP_SELECT_INVERSE, LoadString(IDS_STRING_SELECT_INVERSE).c_str());

    ::SetDlgItemText(m_hWnd, IDC_BUTTON_FIRST_PAGE, LoadString(IDS_STRING_FIRST_PAGE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_LAST_PAGE, LoadString(IDS_STRING_LAST_PAGE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_PRE_PAGE, LoadString(IDS_STRING_PRE_PAGE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_NEXT_PAGE, LoadString(IDS_STRING_NEXT_PAGE).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_SP_LIST, LoadString(IDS_STRING_SP_LIST).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_BKG_LIST, LoadString(IDS_STRING_BKG_LIST).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_GROUP_TEXT_LIST, LoadString(IDS_STRING_TEXT_LIST).c_str());

    ::SetDlgItemText(m_hWnd, IDC_CHECK_BIG_CURSOR, LoadString(IDS_STRING_BIG_CURSOR).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_BIG_SPRITE, LoadString(IDS_STRING_8_16_MODE).c_str());

    ::SetDlgItemText(m_hWnd, IDC_STATIC_CHAR_DEST, LoadString(IDS_STRING_CHAR_DEST).c_str());
    ::SetDlgItemText(m_hWnd, IDC_STATIC_CHAR_SRC, LoadString(IDS_STRING_CHAR_SRC).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_SWAP_SP, LoadString(IDS_STRING_SWAP_SP).c_str());

    ::SetDlgItemText(m_hWnd, IDC_CHECK_CHAR_MAP_FAST, LoadString(IDS_STRING_QUICK_FROM_BKG_CHR).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_BKG_CHR_FAST, LoadString(IDS_STRING_QUICK_BKG_CHR).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_SP_CHR_FAST, LoadString(IDS_STRING_QUICK_SP_CHR).c_str());
    ::SetDlgItemText(m_hWnd, IDC_CHECK_BKG_LIST_FAST, LoadString(IDS_STRING_FAST).c_str());

    ::SetDlgItemText(m_hWnd, IDC_CHECK_MARK_CURSOR, LoadString(IDS_STRING_MARK_CURSOR).c_str());

    ::SetDlgItemText(m_hWnd, ID_EDIT_BG_TO_SP, LoadString(IDS_STRING_BG_TO_SP).c_str());
    ::SetDlgItemText(m_hWnd, ID_EDIT_SP_TO_BG, LoadString(IDS_STRING_SP_TO_BG).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_EXPORT_CHAR_MAPPING, LoadString(IDS_STRING_EXPORT).c_str());
    ::SetDlgItemText(m_hWnd, IDC_BUTTON_RESET_CHAR_MAPPING, LoadString(IDS_STRING_RESET).c_str());

    //文件
    //修改菜单文本
    {
        HMENU hMenu = GetMenu(m_hWnd);

        ModifyMenuText(hMenu, 0, IDS_STRING_FILE, TRUE);

        ModifyMenuText(hMenu, ID_FILE_EXIT, IDS_STRING_FILE_EXIT, FALSE);

        //编辑
        ModifyMenuText(hMenu, 1, IDS_STRING_EDIT, TRUE);

        ModifyMenuText(hMenu, ID_EDIT_CLEAR_UNUSE_BKG_TILE, IDS_STRING_CLEAR_UNUSE_BKG_TILE, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_CLEAR_UNUSE_SP_TILE, IDS_STRING_CLEAR_UNUSE_SP_TILE, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_AUTO_CHAR_MAPPING, IDS_STRING_AUTO_CHAR_MAPPING, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_AUTO_FULL_CHAR_MAPPING, IDS_STRING_FULL_CHAR_MAPPING, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_BG_CHAR_MAPPING, IDS_STRING_BG_CHAR_MAPPING, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_SP_CHAR_MAPPING, IDS_STRING_SP_CHAR_MAPPING, FALSE);

        ModifyMenuText(hMenu, ID_EDIT_BG_ORGANIZE, IDS_STRING_BG_ORGANIZE, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_SP_ORGANIZE, IDS_STRING_SP_ORGANIZE, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_BG_TO_SP, IDS_STRING_BG_TO_SP, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_SP_TO_BG, IDS_STRING_SP_TO_BG, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_SP_8_16_TO_8_8, IDS_STRING_SP_16_TO_8, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_SP_ID_SWAP, IDS_STRING_SP_ID_SWAP, FALSE);
        ModifyMenuText(hMenu, ID_EDIT_BG_TILE_TO_SP_TILE, IDS_STRING_BG_TILE_TO_SP_TILE, FALSE);

        //Tile 工具
        ModifyMenuText(hMenu, 2, IDS_STRING_TILE_TOOL, TRUE);
        ModifyMenuText(hMenu, ID_TILE_COLOR0123_MOVE_LEFT, IDS_STRING_TILE_MOVE_COLOR_0123_LEFT, FALSE);
        ModifyMenuText(hMenu, ID_TILE_COLOR0123_MOVE_RIGHT, IDS_STRING_TILE_MOVE_COLOR_0123_RIGHT, FALSE);
        ModifyMenuText(hMenu, ID_TILE_COLOR123_MOVE_LEFT, IDS_STRING_TILE_MOVE_COLOR_123_LEFT, FALSE);
        ModifyMenuText(hMenu, ID_TILE_COLOR123_MOVE_RIGHT, IDS_STRING_TILE_MOVE_COLOR_123_RIGHT, FALSE);

        ModifyMenuText(hMenu, ID_TILE_DELETE_COLOR_1, IDS_STRING_TILE_DELETE_COLOR_1, FALSE);;
        ModifyMenuText(hMenu, ID_TILE_DELETE_COLOR_2, IDS_STRING_TILE_DELETE_COLOR_2, FALSE);
        ModifyMenuText(hMenu, ID_TILE_DELETE_COLOR_3, IDS_STRING_TILE_DELETE_COLOR_3, FALSE);

        ModifyMenuText(hMenu, ID_TILE_MOVE_UP, IDS_STRING_TILE_MOVE_PIXEL_UP, FALSE);
        ModifyMenuText(hMenu, ID_TILE_MOVE_DOWN, IDS_STRING_TILE_MOVE_PIXEL_DOWN, FALSE);
        ModifyMenuText(hMenu, ID_TILE_MOVE_LEFT, IDS_STRING_TILE_MOVE_PIXEL_LEFT, FALSE);
        ModifyMenuText(hMenu, ID_TILE_MOVE_RIGHT, IDS_STRING_TILE_MOVE_PIXEL_RIGHT, FALSE);

        ModifyMenuText(hMenu, ID_TILE_SWAP_COLOR_1_2, IDS_STRING_TILE_SWAP_1_2_COLOR, FALSE);
        ModifyMenuText(hMenu, ID_TILE_SWAP_COLOR_1_3, IDS_STRING_TILE_SWAP_1_3_COLOR, FALSE);
        ModifyMenuText(hMenu, ID_TILE_SWAP_COLOR_2_3, IDS_STRING_TILE_SWAP_2_3_COLOR, FALSE);

        //显示
        ModifyMenuText(hMenu, 3, IDS_STRING_SHOW, TRUE);
        ModifyMenuText(hMenu, ID_SHOW_BG, IDS_STRING_SHOW_BG, FALSE);
        ModifyMenuText(hMenu, ID_SHOW_SP, IDS_STRING_SHOW_SP, FALSE);
        ModifyMenuText(hMenu, ID_SHOW_LIST, IDS_STRING_SHOW_LIST, FALSE);

        ModifyMenuText(hMenu, ID_SHOW_CUSTOM_TEXT, IDS_STRING_SHOW_CUSTOM_TEXT, FALSE);
        ModifyMenuText(hMenu, ID_SHOW_CAPTION, IDS_STRING_SHOW_CAPTION, FALSE);
        ModifyMenuText(hMenu, ID_SHOW_CURSOR, IDS_STRING_SHOW_CURSOR, FALSE);

        ::DrawMenuBar(m_hWnd);
    }

    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnPaint(WPARAM wParam, LPARAM lParam)
{
    ::DefWindowProc(m_hWnd, WM_PAINT, wParam, lParam);

    DrawNameTable();
    DrawBkgPalette();
    DrawSpritePalette();
    DrawAttribute();
    DrawPalette();

    return TRUE;
}

LRESULT CDesignDialog::OnCmdUndo(WPARAM wParam, LPARAM lParam)
{
    if (m_HistoryData.empty())
    {
        return (LRESULT)FALSE;
    }

    if (m_nHistoryIndex < m_HistoryData.size())
    {
        if (m_nHistoryIndex > 0)
        {
            m_nHistoryIndex--;
        }

        auto& item = m_HistoryData[m_nHistoryIndex];
        SetLoadData(item);
    }

    if (0 == m_nHistoryIndex)
    {
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_UNDO), FALSE);
    }

    if (m_nHistoryIndex < m_HistoryData.size())
    {
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_REDO), TRUE);
    }

    CheckSpriteAttributeButton();
    return (LRESULT)TRUE;
}

LRESULT CDesignDialog::OnCmdRedo(WPARAM wParam, LPARAM lParam)
{
    if (m_HistoryData.empty())
    {
        return (LRESULT)FALSE;
    }

    if ((m_nHistoryIndex + 1) < m_HistoryData.size())
    {
        m_nHistoryIndex++;
        auto& item = m_HistoryData[m_nHistoryIndex];
        SetLoadData(item);
    }

    if (m_nHistoryIndex > 0)
    {
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_UNDO), TRUE);
    }

    if (m_nHistoryIndex == m_HistoryData.size() - 1)
    {
        ::EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_REDO), FALSE);
    }

    CheckSpriteAttributeButton();
    return (LRESULT)TRUE;
}
