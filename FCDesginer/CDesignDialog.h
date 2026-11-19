#pragma once

#include "CWindow/CDialogBase.h"
#include "Win32Utils/CFileUtils.h"
#include "CWindowDraw.h"
#include "Common.h"
#include "CCustomMenuData.h"
#include <condition_variable>
#include "Utils/CBitmapDraw.h"

#define FOCUS_COLOR                 (RGB(255, 0, 0))
#define FOCUS_CURSOR_COLOR          (RGB(128, 255, 0))
#define FOCUS_PALETTE_COLOR         (RGB(255, 255, 0))
#define FOCUS_BKG_PALETTE_COLOR     (RGB(255, 255, 0))
#define FOCUS_TILE_COLOR            (RGB(255, 0, 0))
#define FOCUS_SP_COLOR              (RGB(0, 255, 255))
#define CHECK_TILE_COLOR            (RGB(255, 255, 0))
#define SELECTING_LINE_COLOR        (RGB(255, 0, 255))
#define MARK_TILE_COLOR             (RGB(255, 128, 128))

#define GRID_LINE_COLOR             (RGB(128, 128, 128))
#define GRID_CENTER_LINE_COLOR      (RGB(0, 255, 0))
#define GRID_PART_LINE_COLOR        (RGB(255, 255, 255))

#define TIMER_LICENSE_CHECK         (0x200)
#define LICENSE_CHECK_TIME          (5000)

#define TILE_FLAG_CHECKED           (0x01)

enum eClickType
{
    eNone,
    eCaption,
    ePageNumber,
    eItemText,
    eCursor,
    eCursorList,
    eNameTable,
    eAttribute,
    eBkgPalette,
    eSpritePalette,
    ePalette,
    eBkgTile,
    eSpriteTile,
    eCustomText,
    eBkgTileEdit,
    eSpriteTileEdit,
    eCharMapSrc,
    eCharMapDest,
    eBkgListQuick
};

enum eClickFromType
{
    eFromNone,
    eFromNameTable,
    eFromBgkTile,
    eFromSpriteTile
};

enum eMoveTilePixelType
{
    eMtpLeft,
    eMtpRight,
    eMtpUp,
    eMtpDown
};

enum eMoveAttributesType
{
    eMatLeft,
    eMatRight,
    eMatUp,
    eMatDown
};

bool operator == (const POINT& ptA, const POINT& ptB);

bool operator != (const POINT& ptA, const POINT& ptB);

class CDesignDialog :
    public CDialogBase
{
public:
    CDesignDialog();
    ~CDesignDialog();

    dlg_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnWindowPosChanging(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnColor(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnLanguageChange(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnMButtonDown(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnLButtonDbClick(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdFillColor(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdDrawLine(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdDragText(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdUndo(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdRedo(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdExportCharMapping(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdResetCharMapping(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdAttrLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdAttrSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdAttrReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdTextLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdTextSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdTextReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdLayoutLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdLayoutSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdLayoutReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdBkgChrLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgChrSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgChrReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdSwapChr(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdBkgSelAll(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgSelInverse(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdBkgListLeft(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListUp(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListRight(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListDown(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdSpChrLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpChrSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpChrReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdSpSelAll(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpSelInverse(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdSpListLeft(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListUp(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListRight(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListDown(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpListSwap(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdBkgPaletteLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgPaletteSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgPaletteReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdSpPaletteLoad(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpPaletteSave(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpPaletteReset(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdAddText(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT ShowFirstPage(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT ShowLastPage(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT ShowPrePage(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT ShowNextPage(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdVFlip(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdHFlip(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBackgroundDisplay(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBigCursor(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBigSprite(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdCharMappingQuick(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgChrQuick(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpChrQuick(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBkgListQuick(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdFileExit(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdClearUnuseBkgTile(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdClearUnuseSpTile(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdAutoCharMapping(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdAutoFullCharMapping(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdCopyBgToSp(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdCopySpToBg(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBgCharMapping(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpCharMapping(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBgOrganize(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpOrganize(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSp16To8(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSpIdSwap(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdBgTileToSpTile(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdDeleteColor0(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdDeleteColor1(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdDeleteColor2(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdDeleteColor3(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdMoveColorLeft123(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMoveColorRight123(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdMoveColorLeft0123(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMoveColorRight0123(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdMovePixelLeft(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMovePixelRight(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMovePixelUp(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMovePixelDown(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdMoveAttrLeft(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMoveAttrRight(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMoveAttrUp(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdMoveAttrDown(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdSwapColor12(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSwapColor13(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdSwapColor23(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdMarkCursor(WPARAM wParam, LPARAM lParam);

    dlg_msg LRESULT OnCmdShowBgTile(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdShowSpTile(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdShowList(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdShowCustomText(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdShowCaption(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCmdShowCursor(WPARAM wParam, LPARAM lParam);

    virtual BOOL PreTranslateMessage(LPMSG pMsg);

    void SetCaption(const _tstring& strCaption);
    void SetPageShow(bool bShow);
    void SetItemList(const std::vector<_tstring>& itemList, DWORD dwPageSize, bool bInsertRow);
    void SetWndPos();

private:

    void UpdateLicense();
    void ResetBkgChecked();
    void ResetSpChecked();

    void MoveAttributes(eMoveAttributesType eType);
    void SwapTileColor(LPBYTE lpData, BYTE c1, BYTE c2);
    void MoveTileColor123(LPBYTE lpData, bool bRight = false);
    void MoveTileColor0123(LPBYTE lpData, bool bRight = false);
    void MoveTilePixel(LPBYTE lpData, eMoveTilePixelType nType = eMoveTilePixelType::eMtpLeft);
    void DeleteTileColor(LPBYTE lpData, BYTE dwColorIndex);
    void ClickByFastBkgList(POINT ptMouse);
    void ClickByFastSpList(POINT ptMouse);
    void ClickByFastBkgChr(POINT ptMouse);
    void ClickByFastSpChr(POINT ptMouse);
    void ClickByFastCharMapping(POINT ptMouse);
    void DeleteCheckedBkgTileItem();
    void DeleteCheckedBkgVramTile();
    void DeleteCheckedSpVramTile();
    void DeleteCheckedTile();
    void CheckSelectedNameTableTile(POINT ptLT, POINT ptRB);
    void CheckSelectedBkgTile(POINT ptLT, POINT ptRB);
    void CheckSelectedSpTile(POINT ptLT, POINT ptRB);
    void CheckSpriteAttributeButton();
    void EnableSpriteAttributeButton(bool bEnable);
    void UpdateHistoryData(bool bManual);
    void RecordHistoryData(bool bManual);
    bool IsSameData(CByteBuffer& data1, CByteBuffer& data2);
    void SetLoadData(CByteBuffer& data);
    CByteBuffer GetSaveData();

    void UpdateAttribute();
    void UpdateCursorTileList();
    void ResetCursorTileList();
    void ClearBkgTileListCheck();
    void ClearSpriteTileListCheck();
    void ClearCursorTileListCheck();
    bool ClickNameTableAttributes(POINT pt);
    bool ClickAttribute(POINT pt);
    bool ClickBkgPalette(POINT pt);
    bool ClickSpritePalette(POINT pt);
    bool ClickPalette(POINT pt);
    bool ClickBkgChrEdit(POINT pt);
    bool ClickSpriteChrEdit(POINT pt);

    DWORD GetBkgColor();
    DWORD GetTileBkgColor(const TILE_INFO& tile);
    void DrawNameTableElements();
    void DrawNameTableSync ();
    void DrawNameTable();
    void DrawItemList();
    void DrawTextList();
    void DrawBkgTileList(bool bFocus = false);
    void DrawSpriteTileList(bool bFocus = false, bool bBack = false);
    void DrawCursorTile();
    void DrawCursorTileList(bool bFocus = false);
    void DrawBkgPalette();
    void DrawSpritePalette();
    void DrawPalette();
    void DrawAttribute();
    void DrawTile(int nBkgBegin = 0, int nBkgCount = 4, int nSpriteBegin = 0, int nSpriteCount = 4);
    void DrawDefaultTile();
    bool GetGridClick(HWND hParent, HWND hWnd, float nWidth, float nHeight, float nRow, float nCol, POINT& ptCheck, POINT& ptOffset);
    void DrawBkgVram();
    void DrawSpVram();
    void DrawCharMappingSource();
    void DrawCharMappingTarget();
    void UpdateBkgVramEdit();
    void UpdateSpVramEdit();
    void UpdateNameTableColor(int nAttributeIndex = 0);
    void TopCheckedBkgListTile();

    POINT GetClientOffset(HWND hFirst, HWND hSecond);

    DWORD GetNameTableColor(POINT pt);
    DWORD GetNameTableColorIndex(POINT pt);

    bool SetBitmap32Data(LPDWORD lpData, DWORD dwSize, DWORD dwWidth, DWORD dwHeight, DWORD dwX, DWORD dwY, DWORD dwColor);

    void CalcItemRect();

    _tstring GetPageText(int nIndex, int nTotal, int nPageSize);


    void FillGridText(
        HWND hWnd,
        HDC hdc,
        float nWidth,
        float nHeight,
        float nRow,
        float nCol,
        POINT pt,
        _tstring strText = _T("")
    );

    void DrawBkgTile(
        HWND hWnd,
        HDC hdc,
        float nWidth,
        float nHeight,
        float nRow,
        float nCol,
        TILE_INFO tile,
        bool isTransparent = true
    );

    void DrawSpriteTile(
        HWND hWnd,
        HDC hdc,
        TILE_INFO tile,
        bool isTransparent = true
    );

    void DrawTask();

    DECLARE_DLG_MESSAGE_MAP()

private:

    BYTE m_AttributeData = 0;

    CWindowDraw m_wndTileDraw;
    CWindowDraw m_hWndNameTable;
    CWindowDraw m_hWndAttribute;
    CWindowDraw m_hWndBkgPalette;
    CWindowDraw m_hWndSpPalette;
    CWindowDraw m_hWndPalette;
    CWindowDraw m_wndDragTile;
    CWindowDraw m_wndBkgTile;
    CWindowDraw m_wndSpTile;
    CWindowDraw m_wndBkgTileEdit;
    CWindowDraw m_wndSpTileEdit;
    CWindowDraw m_wndCharMappingDest;
    CWindowDraw m_wndCharMappingSrc;
    CWindowDraw m_wndDefaultTile;

    POINT m_ptCurNameTable = { 0 , 0 };
    POINT m_ptCurNameTableAttr = { 0 , 0 };
    POINT m_ptCurAttribute = { 1 , 0 };
    POINT m_ptCurBkgPalette = { 1 , 0 };
    POINT m_ptCurSpPalette = { 0 , 0 };
    POINT m_ptCurPalette = { 0 , 0 };

    _tstring m_strCaption;
    _tstring m_strPageNumber;

    POINT m_ptCaption = { 0x0c, 0x2 };
    POINT m_ptPageNumber = { 0x0d, 0x1b };
    POINT m_ptItem = { 0x09, 0x05 };

    std::vector<_tstring> m_strItemList;
    std::vector<_tstring> m_strItemListTotal;

    RECT m_rtItems[20] = {0};
    POINT m_ptItemMax = { 0 };

    HBRUSH m_hBrush = NULL;

    eClickType m_Click = eClickType::eNone;
    eClickFromType m_ClickFrom = eClickFromType::eFromNone;

    std::vector<TILE_INFO> m_BkgTileList;
    size_t m_BkgTileIdCount = 0;

    std::vector<TILE_INFO> m_SpTileList;
    size_t m_SpTileIdCount = 0;

    std::vector<TEXT_INFO> m_CustomTextList;
    size_t m_CustomTextCount = 0;

    TILE_INFO m_dragTile;
    TEXT_INFO m_CustomText;

    POINT m_ptDragOffset = { 0 };
    POINT m_ptClickOffset = { 0 };

    POINT m_ptCurBkgTile = { 0 };
    POINT m_ptCurSpTile = { 0 };

    POINT m_ptCurBkgTileEdit = { 0 };
    POINT m_ptCurSpTileEdit = { 0 };

    POINT m_ptCharMapSrc = { 0 };
    POINT m_ptCharMapDest = { 0 };
    POINT m_ptNameTableTile = { 0 , 0 };

    POINT m_ptNtSelectBegin = { 0 };
    POINT m_ptNtSelectEnd = { 0 };
    POINT m_ptBkgTileSelectBegin = { 0 };
    POINT m_ptBkgTileSelectEnd = { 0 };
    POINT m_ptSpTileSelectBegin = { 0 };
    POINT m_ptSpTileSelectEnd = { 0 };

    TILE_INFO m_tileCursor = { 0 };

    std::vector<TILE_INFO> m_CursorTileList;
    std::vector<CByteBuffer> m_HistoryData;

    CByteBuffer m_HistoryBuf;
    size_t m_nHistoryIndex = 0;

    bool m_bFillColor = false;
    bool m_bDrawLine = false;
    bool m_bDragText = false;
    bool m_bShowPage = false;
    bool m_bNameInsertRow = false;
    bool m_SelSpPalette = false;

    int m_nCurIndex = 0;
    int m_nPageSize = 0;
    bool m_bSelectMode = false;
    bool m_bSelecting = false;
    bool m_bBigCursor = false;
    bool m_bBigSprite = false;
    bool m_bQuickCharMappingSrc = false;
    bool m_bQuickBkgChrDest = false;
    bool m_bQuickSpChrDest = false;
    bool m_bQuickBkgList = false;
    bool m_bMarkCursor = false;

    bool m_bShowBgTile = true;
    bool m_bShowSpTile = true;
    bool m_bShowList = true;
    bool m_bShowText = true;
    bool m_bShowCaption = true;
    bool m_bShowCursor = true;

    POINT m_ptLastBkgPal = { -1,-1 };
    POINT m_ptLastSpPal = { -1,-1 };

    bool static m_bBkgTileCheck[16 * 16];
    bool static m_bSpTileCheck[16 * 16];

    HACCEL m_hAccel = NULL;
    Fcad_Type m_ftType = Fcad_Type::FDT_None;
    bool m_designEnable = true;
    _tstring m_strPath;
    double m_nWndScale = 1.0f;

    std::thread m_taskDraw;
    std::condition_variable m_cvDraw;
    CBitmapDraw m_bitmapDraw;
    bool m_bQuit = false;
    std::mutex m_tileMutex;
    bool fSizeInit = false;
};