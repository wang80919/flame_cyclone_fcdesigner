#include "CCustomMenuData.h"
#include <Windows.h>
#include <strsafe.h>
#include "Win32Utils/CFileUtils.h"
#include "Win32Utils/CRegUtils.h"
#include "Win32Utils/CStrUtils.h"
#include "Utils/CDesignRegisterUtils.h"
#include "Common.h"
#include "resource.h"

_tstring g_strCaption;

DWORD g_dwCaptionPpuAddr = 0x204C;
DWORD g_dwPageNumberPpuAddr = 0x236D;
DWORD g_dwItemPpuAddr = 0x20A9;
BYTE g_ChrRam[0x2000] = { 0 };
BYTE g_ChrRamDefault[0x2000] = { 0 };
BYTE g_CharMapData[0x100] = { 0 };
bool g_bBigSprite = false;

std::vector<TILE_INFO> g_BkgTileList;
std::vector<TILE_INFO> g_SpriteTileList;
std::vector<TEXT_INFO> g_CustomTextList;
std::vector<TILE_INFO> g_ArrowTileList;
TILE_INFO g_tileArrow = { 0 };

//调色板颜色, FCEUX
DWORD g_PaletteColor[0x40] =
{
    RGB(0x75, 0x75, 0x75), //00
    RGB(0x24, 0x18, 0x8E), //01
    RGB(0x00, 0x00, 0xAA), //02
    RGB(0x45, 0x00, 0x9E), //03
    RGB(0x8E, 0x00, 0x75), //04
    RGB(0xAA, 0x00, 0x10), //05
    RGB(0xA6, 0x00, 0x00), //06
    RGB(0x7D, 0x08, 0x00), //07
    RGB(0x41, 0x2C, 0x00), //08
    RGB(0x00, 0x45, 0x00), //09
    RGB(0x00, 0x51, 0x00), //0A
    RGB(0x00, 0x3C, 0x14), //0B
    RGB(0x1B, 0x3C, 0x5D), //0C
    RGB(0x00, 0x00, 0x00), //0D
    RGB(0x00, 0x00, 0x00), //0E
    RGB(0x00, 0x00, 0x00), //0F
    RGB(0xBE, 0xBE, 0xBE), //10
    RGB(0x00, 0x71, 0xEF), //11
    RGB(0x20, 0x38, 0xEF), //12
    RGB(0x82, 0x00, 0xF3), //13
    RGB(0xBE, 0x00, 0xBE), //14
    RGB(0xE7, 0x00, 0x59), //15
    RGB(0xDB, 0x28, 0x00), //16
    RGB(0xCB, 0x4D, 0x0C), //17
    RGB(0x8A, 0x71, 0x00), //18
    RGB(0x00, 0x96, 0x00), //19
    RGB(0x00, 0xAA, 0x00), //1A
    RGB(0x00, 0x92, 0x38), //1B
    RGB(0x00, 0x82, 0x8A), //1C
    RGB(0x00, 0x00, 0x00), //1D
    RGB(0x00, 0x00, 0x00), //1E
    RGB(0x00, 0x00, 0x00), //1F
    RGB(0xFF, 0xFF, 0xFF), //20
    RGB(0x3C, 0xBE, 0xFF), //21
    RGB(0x5D, 0x96, 0xFF), //22
    RGB(0xCF, 0x8A, 0xFF), //23
    RGB(0xF7, 0x79, 0xFF), //24
    RGB(0xFF, 0x75, 0xB6), //25
    RGB(0xFF, 0x75, 0x61), //26
    RGB(0xFF, 0x9A, 0x38), //27
    RGB(0xF3, 0xBE, 0x3C), //28
    RGB(0x82, 0xD3, 0x10), //29
    RGB(0x4D, 0xDF, 0x49), //2A
    RGB(0x59, 0xFB, 0x9A), //2B
    RGB(0x00, 0xEB, 0xDB), //2C
    RGB(0x79, 0x79, 0x79), //2D
    RGB(0x00, 0x00, 0x00), //2E
    RGB(0x00, 0x00, 0x00), //2F
    RGB(0xFF, 0xFF, 0xFF), //30
    RGB(0xAA, 0xE7, 0xFF), //31
    RGB(0xC7, 0xD7, 0xFF), //32
    RGB(0xD7, 0xCB, 0xFF), //33
    RGB(0xFF, 0xC7, 0xFF), //34
    RGB(0xFF, 0xC7, 0xDB), //35
    RGB(0xFF, 0xBE, 0xB2), //36
    RGB(0xFF, 0xDB, 0xAA), //37
    RGB(0xFF, 0xE7, 0xA2), //38
    RGB(0xE3, 0xFF, 0xA2), //39
    RGB(0xAA, 0xF3, 0xBE), //3A
    RGB(0xB2, 0xFF, 0xCF), //3B
    RGB(0x9F, 0xFF, 0xF3), //3C
    RGB(0xC7, 0xC7, 0xC7), //3D
    RGB(0x00, 0x00, 0x00), //3E
    RGB(0x00, 0x00, 0x00)  //3F
};

BYTE g_AttributeData[0x40] = 
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,
    0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
};

BYTE g_AttributeDataMMC5[0x400] =
{
    0
};

BYTE g_AttributeDefaultData[0x40] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,
    0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
};

BYTE g_BkgPalette[0x10] =
{
    0x0F,0x24,0x23,0x23,0x0F,0x21,0x21,0x21,0x0F,0x27,0x22,0x22,0x0F,0x22,0x22,0x22
};

BYTE g_BkgDefaultPalette[0x10] =
{
    0x0F,0x24,0x23,0x23,0x0F,0x21,0x21,0x21,0x0F,0x27,0x22,0x22,0x0F,0x22,0x22,0x22
};

BYTE g_SpritePalette[0x10] =
{
    0x0F,0x20,0x27,0x22,0x0F,0x24,0x24,0x24,0x0F,0x21,0x21,0x21,0x0F,0x22,0x22,0x22
};

BYTE g_SpriteDefaultPalette[0x10] =
{
    0x0F,0x20,0x27,0x22,0x0F,0x24,0x24,0x24,0x0F,0x21,0x21,0x21,0x0F,0x22,0x22,0x22
};

void ResetCharMap()
{
    for (int i = 0; i < sizeof(g_CharMapData); i++)
    {
        if (i > 0x20 && i < 0x80)
        {
            g_CharMapData[i] = (BYTE)i;
        }
        else
        {
            g_CharMapData[i] = (BYTE)0;
        }
    }
}

bool LoadCustomMenu()
{
    DWORD dwSize = 0;

    g_tileArrow.data = 0x1F;
    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_ARROW_TILE, &g_tileArrow, sizeof(g_tileArrow));

    if (g_CustomTextList.empty())
    {
        dwSize = 0;
        if (CRegUtils::GetValueSize(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CUSTOM_TEXT,
            dwSize
        ))
        {
            CByteBuffer cBuffer(dwSize);
            if (CRegUtils::GetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_CUSTOM_TEXT, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize()))
            {
                LPBYTE lpData = cBuffer.GetBuffer();
                int nIndex = 0;
                while (lpData < cBuffer.GetBuffer() + dwSize)
                {
                    PCUSTOM_MENU_DATA lpCustomData = (PCUSTOM_MENU_DATA)lpData;
                    PCUSTOM_TEXT lpCustomText = (PCUSTOM_TEXT)lpCustomData->data;

                    TEXT_INFO info;
                    info.pt = lpCustomText->pt;
                    info.id = nIndex++;
                    TCHAR szBuf[MAX_PATH] = { 0 };
                    memcpy_s(szBuf, sizeof(szBuf), lpCustomText->data, lpCustomText->dwSize);
                    info.strText = szBuf;

                    g_CustomTextList.push_back(info);

                    lpData += lpCustomData->dwElementSize + sizeof(CUSTOM_MENU_DATA);
                }
            }
        }
    }

    if (g_BkgTileList.empty())
    {
        if (CRegUtils::GetValueSize(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_BKG_TILE_LIST,
            dwSize
        ))
        {
            CByteBuffer cBuffer(dwSize);
            if (CRegUtils::GetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_BKG_TILE_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize()))
            {
                TILE_INFO* pInfo = (TILE_INFO*)cBuffer.GetBuffer();
                while ((LPBYTE)pInfo < cBuffer.GetBuffer() + dwSize)
                {
                    g_BkgTileList.push_back(*pInfo);
                    pInfo++;
                }
            }
        }
    }

    if (g_SpriteTileList.empty())
    {
        if (CRegUtils::GetValueSize(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_SPRITE_TILE_LIST,
            dwSize
        ))
        {
            CByteBuffer cBuffer(dwSize);
            if (CRegUtils::GetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_SPRITE_TILE_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize()))
            {
                LPBYTE lpData = cBuffer.GetBuffer();

                while (lpData < cBuffer.GetBuffer() + dwSize)
                {
                    PTILE_INFO lpCustomText = (PTILE_INFO)lpData;

                    TILE_INFO info = *lpCustomText;

                    g_SpriteTileList.push_back(info);

                    lpData += sizeof(TILE_INFO);
                }
            }
        }
    }

    if (g_ArrowTileList.size() < 4)
    {
        g_ArrowTileList.clear();

        if (CRegUtils::GetValueSize(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_ARROW_TILE_LIST,
            dwSize
        ))
        {
            CByteBuffer cBuffer(dwSize);
            if (CRegUtils::GetBinary(
                CONFIG_REG_KEY,
                CONFIG_MENU_DESIGN_PATH,
                CONFIG_MENU_ARROW_TILE_LIST, cBuffer.GetBuffer(), (DWORD)cBuffer.GetSize()))
            {
                LPBYTE lpData = cBuffer.GetBuffer();

                while (lpData < cBuffer.GetBuffer() + dwSize)
                {
                    PTILE_INFO lpCustomText = (PTILE_INFO)lpData;

                    TILE_INFO info = *lpCustomText;
                    if (info.pt.x < 0 || info.pt.x > 255) info.pt.x = 0;
                    if (info.pt.y < 1 || info.pt.y > 239) info.pt.y = 1;

                    g_ArrowTileList.push_back(info);

                    lpData += sizeof(TILE_INFO);
                }
            }
        }

        if (g_ArrowTileList.size() < 4)
        {
            g_ArrowTileList.clear();

            TILE_INFO info;
            info.id = 0;
            info.data = 0x02;
            info.pt.x = (g_tileArrow.pt.x - 2) * 8 - 2;
            info.pt.y = g_tileArrow.pt.y * 8 - 4;

            if (info.pt.x < 0 || info.pt.x > 255) info.pt.x = 0;
            if (info.pt.y < 1 || info.pt.y > 239) info.pt.y = 1;

            g_ArrowTileList.push_back(info);
            info.id++;
            info.data = 0x03;
            g_ArrowTileList.push_back(info);
            info.id++;
            info.data = 0x12;
            g_ArrowTileList.push_back(info);
            info.id++;
            info.data = 0x13;
            g_ArrowTileList.push_back(info);
        }
    }

    static bool bLoadDefault = false;
    if (!bLoadDefault)
    {
        HRSRC hResource = ::FindResource(NULL, MAKEINTRESOURCE(IDR_MENU_CHR), _T("MENU_CHR"));
        if (NULL == hResource)
        {
            return false;
        }

        dwSize = ::SizeofResource(nullptr, hResource);

        //无需释放使用 LoadResource 加载的资源
        HGLOBAL hGlobal = ::LoadResource(nullptr, hResource);
        if (NULL == hGlobal)
        {
            return false;
        }

        //不需要解锁资源
        LPBYTE lpData = (LPBYTE)::LockResource(hGlobal);
        if (NULL == lpData)
        {
            return false;
        }
        if (dwSize <= sizeof(g_ChrRamDefault))
        {
            memcpy_s(g_ChrRamDefault, sizeof(g_ChrRamDefault), lpData, dwSize);
        }
        else
        {
            memcpy_s(g_ChrRamDefault, sizeof(g_ChrRamDefault), lpData, sizeof(g_ChrRamDefault));
        }

        bLoadDefault = true;
    }

    dwSize = 0;
    if (CRegUtils::GetValueSize(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_CHR_DATA,
        dwSize
    ))
    {
        CRegUtils::GetBinary(
            CONFIG_REG_KEY,
            CONFIG_MENU_DESIGN_PATH,
            CONFIG_MENU_CHR_DATA, g_ChrRam, sizeof(g_ChrRam));
    }
    else
    {
        memcpy_s(g_ChrRam, sizeof(g_ChrRam), g_ChrRamDefault, sizeof(g_ChrRamDefault));
    }

    POINT ptItem = { 0x09, 0x05 };
    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_ITEM_POS, &ptItem, sizeof(ptItem));
    
    if (g_tileArrow.pt.x < 0) g_tileArrow.pt.x = 0;
    if (g_tileArrow.pt.y < 1) g_tileArrow.pt.y = 1;
    if (g_tileArrow.pt.x > 255) g_tileArrow.pt.x = 255;
    if (g_tileArrow.pt.y > 239) g_tileArrow.pt.y = 239;

    ResetCharMap();

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_CHAR_MAP_DATA, g_CharMapData, sizeof(g_CharMapData));

    CRegUtils::GetBinary(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_BIG_SPRITE, &g_bBigSprite, sizeof(g_bBigSprite));

    return true;
}

bool OutputCustomMenu(const std::vector<CNesHeader>& nesList, _tstring strBuildDir, bool bCharMapping)
{
    CHAR szBufA[MAX_PATH] = { 0 };
    std::string strContent;

    DWORD dwIndexTotal = 0;
    for (auto& item : nesList)
    {
        if (item.IsChecked())
        {
            dwIndexTotal++;
        }
    }

    g_strCaption = std::to_wstring(dwIndexTotal) + _T(" IN 1");
    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_MENU_DESIGN_PATH,
        CONFIG_MENU_CAPTION_TEXT, g_strCaption);

    //标题位置
    {
        ::StringCchPrintfA(szBufA,
            _countof(szBufA),
            "TITLE_SHOW_POS = $%04X",
            g_dwCaptionPpuAddr
        );
        strContent = szBufA;

        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_caption_addr.asm");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(strContent.c_str(), (DWORD)strContent.size());
        cFile.Close();
    }

    //标题
    {
        std::string strCaption = CStrUtils::TStrToAStr(g_strCaption);
        if (!strCaption.empty())
        {
            if ((g_dwCaptionPpuAddr & 0x1F) + (LONG)strCaption.size() > 32)
            {
                strCaption = strCaption.substr(0, 32 - (g_dwCaptionPpuAddr & 0x1F));
            }

            strContent.clear();
            ::StringCchPrintfA(szBufA,
                _countof(szBufA),
                " .STR \"%s\"",
                strCaption.c_str()
            );
            strContent = szBufA;

            CFileHelper cFile;
            _tstring strOutFile;
            strOutFile = strBuildDir + _T("\\game_caption_data.asm");

            if (!cFile.Open(strOutFile, true))
            {
                printf("open file failed!\n");
                return false;
            }

            cFile.Clear();
            cFile.Write(strContent.c_str(), (DWORD)strContent.size());
            cFile.Close();
        }
    }

    //页号位置
    {
        ::StringCchPrintfA(szBufA,
            _countof(szBufA),
            "PAGE_INDER_SHOW_POS = $%04X",
            g_dwPageNumberPpuAddr
        );
        strContent = szBufA;

        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_page_number_addr.asm");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(strContent.c_str(), (DWORD)strContent.size());
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //节目位置
    {
        ::StringCchPrintfA(szBufA,
            _countof(szBufA),
            "ITEM_SHOW_POS = $%04X",
            g_dwItemPpuAddr
        );
        strContent = szBufA;

        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_item_addr.asm");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(strContent.c_str(), (DWORD)strContent.size());
        cFile.Close();
    }

    //大光标
    {
        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_big_arrow_data.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();

        for (auto& item : g_ArrowTileList)
        {
            BYTE bDataBuf[4] = { 0 };
            bDataBuf[0] = (BYTE)item.pt.y;
            bDataBuf[1] = item.data;
            BYTE bMode = 0;
            bMode |= (BYTE)item.mode.bVerticalFlip << 0x07;
            bMode |= (BYTE)item.mode.bHorizontalFlip << 0x06;
            bMode |= (BYTE)item.mode.bBackgroundDisplay << 0x05;
            bMode |= (BYTE)item.color;
            bDataBuf[2] = bMode;
            bDataBuf[3] = (BYTE)item.pt.x;

            cFile.Write(bDataBuf, sizeof(bDataBuf));
        }

        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //属性表
    {
        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_attributes_data.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(g_AttributeData, sizeof(g_AttributeData));
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //背景调色板
    {
        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_bkg_palette_data.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(g_BkgPalette, sizeof(g_BkgPalette));
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //精灵调色板
    {
        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_sprite_palette_data.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(g_SpritePalette, sizeof(g_SpritePalette));
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //自定义精灵
    {
        strContent.clear();

        for (size_t i = 0; i < g_SpriteTileList.size(); i++)
        {
            BYTE bMode = 0;
            bMode |= (BYTE)g_SpriteTileList[i].mode.bVerticalFlip << 0x07;
            bMode |= (BYTE)g_SpriteTileList[i].mode.bHorizontalFlip << 0x06;
            bMode |= (BYTE)g_SpriteTileList[i].mode.bBackgroundDisplay << 0x05;
            bMode |= (BYTE)g_SpriteTileList[i].color;

            if ((g_SpriteTileList[i].pt.y <= 0 || g_SpriteTileList[i].pt.y > 239) ||
                (g_SpriteTileList[i].pt.x < 0 || g_SpriteTileList[i].pt.x > 255)
                
                )
            {
                continue;
            }

            ::StringCchPrintfA(szBufA,
                _countof(szBufA),
                " .DB $%0.2X,$%0.2X,$%0.2X,$%0.2X\r\n",
                (BYTE)g_SpriteTileList[i].pt.y,
                g_SpriteTileList[i].data,
                bMode,
                g_SpriteTileList[i].pt.x
            );

            strContent += szBufA;
        }

        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_sprite_list.asm");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(strContent.c_str(), (DWORD)strContent.size());
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //自定义文本 & 自定义图块
    {
        strContent.clear();

        //自定义图块
        BYTE bData[0x3C0] = { 0 };
        for (const auto& item : g_BkgTileList)
        {
            if ((item.pt.y >= 0 && item.pt.y < 30) &&
                (item.pt.x >= 0 && item.pt.x < 32)
                )
            {
                DWORD dwPos = item.pt.y * 0x20 + item.pt.x;
                bData[dwPos] = item.data;
            }
        }

        BYTE dataBuf[MAX_PATH] = { 0 };

        //自定义文本
        for (const auto& item : g_CustomTextList)
        {
            POINT pt = item.pt;
            _tstring strText = item.strText;

            if (pt.x > 31 || pt.y < 0 || pt.y > 29)
            {
                continue;
            }

            if (pt.x < 0)
            {
                if ((size_t)abs(pt.x) >= item.strText.size())
                {
                    continue;
                }
                else
                {
                    strText = item.strText.substr(abs(pt.x));
                    pt.x = 0;
                }
            }

            if (pt.x + (LONG)strText.size() > 32)
            {
                strText = strText.substr(0, 32 - pt.x);
            }

            DWORD dwPos = pt.y * 0x20 + pt.x;

            std::string strAnsi = CStrUtils::TStrToAStr(strText);

            memcpy_s(dataBuf, sizeof(dataBuf), strAnsi.c_str(), strAnsi.size());
            for (size_t i = 0; i < strAnsi.size(); i++)
            {
                if (' ' == strAnsi[i])
                {
                    continue;
                }
                dataBuf[i] = g_CharMapData[dataBuf[i]];
                bData[dwPos + i] = dataBuf[i];
            }
        }

        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\game_name_table.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(bData, sizeof(bData));
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //自定义CHR
    {
        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\custom_chr_data.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        cFile.Write(g_ChrRam, sizeof(g_ChrRam));
        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    //字符映射表
    {
        CFileHelper cFile;
        _tstring strOutFile;
        strOutFile = strBuildDir + _T("\\char_mapping.bin");

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        if (!bCharMapping)
        {
            BYTE CharMapDefault[0x100] = { 0 };
            for (int i = 0; i < sizeof(CharMapDefault); i++)
            {
                CharMapDefault[i] = (BYTE)i;
            }

            cFile.Write(CharMapDefault, sizeof(CharMapDefault));
        }
        else
        {
            cFile.Write(g_CharMapData, sizeof(g_CharMapData));
        }

        cFile.SetEnd(cFile.GetPointer());
        cFile.Close();
    }

    return true;
}