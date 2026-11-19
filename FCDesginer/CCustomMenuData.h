#pragma once
#include <Windows.h>

#include <string>
#include <tchar.h>
#include <vector>
#include "Utils/CNesHeader.h"

#pragma warning(disable:4200)

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

typedef struct _TILE_MODE
{
    BYTE bVerticalFlip : 1;
    BYTE bHorizontalFlip : 1;
    BYTE bBackgroundDisplay : 1;
    BYTE bReserved : 5;
    _TILE_MODE()
    {
        bVerticalFlip = 0;
        bHorizontalFlip = 0;
        bBackgroundDisplay = 0;
        bReserved = 0;
    }

    _TILE_MODE(BYTE bData)
    {
        bVerticalFlip = (bData & 0x01) >> 0;
        bHorizontalFlip = (bData & 0x02) >> 1;
        bBackgroundDisplay = (bData & 0x03) >> 2;
        bReserved = (bData & 0x05) >> 3;
    }

}TILE_MODE;

typedef struct _TILE_INFO
{
    DWORD id = (DWORD)(-1);  //ID
    POINT pt = { 0 };        //坐标
    DWORD flag = 0;          //标记
    BYTE  data = 0;          //Tile数据
    BYTE  color = 0;         //颜色索引
    TILE_MODE  mode = 0;     //显示模式

    _TILE_INFO()
    {
        this->id = (DWORD)(-1);
        this->pt = { 0,0 };
        this->data = 0;
        this->mode = 0;
    }

    _TILE_INFO(UINT id, POINT pt = { 0, 0 }, BYTE data = 0, BYTE mode = 0, BYTE color = 0)
    {
        this->id = id;
        this->pt = pt;
        this->data = data;
        this->mode = mode;
        this->color = color;
    }

    bool operator == (const _TILE_INFO& r)
    {
        return this->id == r.id;
    }

}TILE_INFO, *PTILE_INFO;

typedef struct _TEXT_INFO
{
    DWORD id = (DWORD)(-1);
    POINT pt = { 0 };
    _tstring strText;

    _TEXT_INFO()
    {
        this->id = (DWORD)(-1);
        this->pt = { 0 };
    }

    _TEXT_INFO(UINT id, POINT pt, _tstring strText)
    {
        this->id = id;
        this->pt = pt;
        this->strText = strText;
    }

    bool operator == (const _TEXT_INFO& r)
    {
        return this->id == r.id;
    }

}TEXT_INFO, *PTEXT_INFO;

typedef struct _CUSTOM_TEXT
{
    POINT pt = { 0 };
    DWORD dwSize = 0;
    TCHAR data[0];
}CUSTOM_TEXT, *PCUSTOM_TEXT;

enum eMenuDataType
{
    eMDTNone,
    eMDTAttributes,
    eMDTBkgPalette,
    eMDTSpritePalette,
    eMDTChrData,
    eMDTNameTable,
    eMDTCustomText,
    eMDTBkgTileList,
    eMDTCaptionText,
    eMDTCaptionPos,
    eMDTPageNumberPos,
    eMDTItemPos,
    eMDTBkgTile,
    eMDTSpriteTile,
    eMDTCursorTile,
    eMDTCursorTileList,
    eMDTHistoryRoot,
    eMDTHistoryNtPos,
    eMDTHistoryNtExPos,
    eMDTHistoryAttrPos,
    eMDTHistoryBkgChrPos,
    eMDTHistoryBkgTilePos,
    eMDTHistoryBkgPalettePos,
    eMDTHistorySpChrPos,
    eMDTHistorySpTilePos,
    eMDTHistorySpPalettePos,
    eMDTHistoryPalettePos,
    eMDTCharMap,
    eMDTBigCursor,
    eMDTBigSprite,
    eMDTCharMapSrcPos,
    eMDTBkgChrQuickPos,
    eMDTNTTilePos,
    eMDTBkgTileChecked,
    eMDTSpTileChecked,
};

typedef struct _CUSTOM_MENU_DATA
{
    DWORD dwType = 0;               //数据类型
    DWORD dwVersion = 0;            //数据版本
    DWORD dwElementCount = 0;       //数据总大小
    DWORD dwElementSize = 0;        //单个元素大小
    BYTE  data[0];                  //数据元素

    _CUSTOM_MENU_DATA()
    {
        ZeroMemory(this, sizeof(_CUSTOM_MENU_DATA));
        dwVersion = MAKEWORD(1, 1);
    }

}CUSTOM_MENU_DATA, *PCUSTOM_MENU_DATA;

extern DWORD g_PaletteColor[0x40];
extern BYTE g_AttributeData[0x40];
extern BYTE g_AttributeDataMMC5[0x400];
extern BYTE g_AttributeDefaultData[0x40];
extern BYTE g_BkgPalette[0x10];
extern BYTE g_SpritePalette[0x10];
extern BYTE g_BkgDefaultPalette[0x10];
extern BYTE g_SpriteDefaultPalette[0x10];
extern BYTE g_ChrRam[0x2000];           // 图形表 0000-1FFF
extern BYTE g_ChrRamDefault[0x2000];    // 默认图形表 0000-1FFF
extern BYTE g_CharMapData[0x100];       // 字符映射表

extern DWORD g_dwCaptionPpuAddr;
extern DWORD g_dwPageNumberPpuAddr;
extern DWORD g_dwItemPpuAddr;
extern bool g_bBigSprite;

extern _tstring g_strCaption;

extern std::vector<TILE_INFO> g_BkgTileList;
extern std::vector<TILE_INFO> g_SpriteTileList;
extern std::vector<TEXT_INFO> g_CustomTextList;
extern std::vector<TILE_INFO> g_ArrowTileList;
extern TILE_INFO g_tileArrow;

bool LoadCustomMenu();
bool OutputCustomMenu(const std::vector<CNesHeader>& nesList, _tstring strBuildDir, bool bCharMapping);
void ResetCharMap();