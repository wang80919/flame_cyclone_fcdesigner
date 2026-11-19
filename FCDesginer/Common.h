#pragma once

#include <Windows.h>

#include <wtypesbase.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <map>
#include <set>

#define DESIGN_MODE

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

#define WM_TRAYICON                                         (WM_USER + 1)
#define WM_LANGUAGE_CHANGE                                  (WM_USER + 2)
#define WM_NOTIFY_UPDATE                                    (WM_USER + 3)
#define WM_INSTALL_UPDATE                                   (WM_USER + 4)
#define WM_INFO_REPLACE_ALL                                 (WM_USER + 5)
#define WM_INFO_FIND                                        (WM_USER + 6)
#define WM_INFO_REPLACE                                     (WM_USER + 7)
#define WM_LIST_LABEL_EDIT                                  (WM_USER + 8)
#define WM_NOTIFY_MESSAGE                                   (WM_USER + 9)

#ifdef LITE_VERSION
#define UPDATE_URL                                          _T("https://gitee.com/flame_cyclone/fcassistant-lite-binary/raw/master/Release/update.json")
#define NOTIFY_URL                                          _T("https://gitee.com/flame_cyclone/fcassistant-lite-binary/raw/master/Release/notify.json")
#else
#define UPDATE_URL                                          _T("https://gitee.com/flame_cyclone/fcassistant-binary/raw/master/Release/update.json")
#define NOTIFY_URL                                          _T("https://gitee.com/flame_cyclone/fcassistant-binary/raw/master/Release/notify.json")
#endif

//#define MENU_PRG_SIZE                                       (8)
#define MENU_CHR_SIZE                                       (0)

#define LIST_MAX_SIZE                                       (20000)
#define MOVE_MAX_SIZE                                       (100)

#define SPRITE_MAX_SIZE                                     (255)

#ifdef _DEBUG
#define GAME_MAX_SIZE                                       (5100)
#else
#define GAME_MAX_SIZE                                       (3072)
#endif

#define ERROR_MAX_COUNT                                     (20)

#define CLOSE_TYPE_UPDATE                                   (5)
#define CHECK_TIMER_ID                                      (0xA9)
#define CHECK_TIMER_TIME                                    (1000 * 60 * 1)
#define NOTIFY_TIMER_ID                                     (0xAD)
#define NOTIFY_TIMER_TIME                                   (1000 * 30 * 1)

#define CONFIG_REG_KEY                                      HKCU

#ifdef DESIGN_MODE

#ifdef LITE_VERSION
#define CONFIG_ROOT_PATH                                    _T("Software\\FCDesign")
#define CONFIG_HISTORY_PATH                                 _T("Software\\FCDesign\\History")
#define CONFIG_COOLBOY_PATH                                 _T("Software\\FCDesign\\Coolboy")
#define CONFIG_MENU_DESIGN_PATH                             _T("Software\\FCDesign\\MenuV2")
#define CONFIG_REGISTER_PATH                                _T("Software\\FCDesign\\Register")
#define CONFIG_DESIGN_REGISTER_PATH                         _T("Software\\FCDesign\\DesignRegister")
#else
#define CONFIG_ROOT_PATH                                    _T("Software\\FCDesign")
#define CONFIG_HISTORY_PATH                                 _T("Software\\FCDesign\\History")
#define CONFIG_MENU_DESIGN_PATH                             _T("Software\\FCDesign\\MenuV2")
#define CONFIG_REGISTER_PATH                                _T("Software\\FCDesign\\Register")
#define CONFIG_DESIGN_REGISTER_PATH                         _T("Software\\FCDesign\\DesignRegister")
#endif

#else

#ifdef LITE_VERSION
#define CONFIG_ROOT_PATH                                    _T("Software\\FCAssistantLite")
#define CONFIG_HISTORY_PATH                                 _T("Software\\FCAssistantLite\\History")
#define CONFIG_COOLBOY_PATH                                 _T("Software\\FCAssistantLite\\Coolboy")
#define CONFIG_MENU_DESIGN_PATH                             _T("Software\\FCAssistantLite\\MenuV2")
#define CONFIG_REGISTER_PATH                                _T("Software\\FCAssistantLite\\Register")
#define CONFIG_DESIGN_REGISTER_PATH                         _T("Software\\FCAssistantLite\\DesignRegister")
#else
#define CONFIG_ROOT_PATH                                    _T("Software\\FCAssistant")
#define CONFIG_HISTORY_PATH                                 _T("Software\\FCAssistant\\History")
#define CONFIG_MENU_DESIGN_PATH                             _T("Software\\FCAssistant\\MenuV2")
#define CONFIG_REGISTER_PATH                                _T("Software\\FCAssistant\\Register")
#define CONFIG_DESIGN_REGISTER_PATH                         _T("Software\\FCAssistant\\DesignRegister")
#endif

#endif


#define CONFIG_COOLBOY_TYPE                                 _T("CoolboyType")

#define CONFIG_WINDOW_POS_X                                 _T("PosX")
#define CONFIG_WINDOW_POS_Y                                 _T("PosY")
#define CONFIG_WINDOW_SIZE_X                                _T("SizeX")
#define CONFIG_WINDOW_SIZE_Y                                _T("SizeY")
#define CONFIG_WINDOW_TOPMOST                               _T("Topmost")
#define CONFIG_WINDOW_DELETE                                _T("Delete")
#define CONFIG_WINDOW_LANGUAGE                              _T("Language")
#define CONFIG_WINDOW_CLOSE_STATE                           _T("CloseState")

#define CONFIG_MENU_SHOW_NUMBER_ORDER                       _T("ShowNumberOrder")
#define CONFIG_MENU_SHOW_NUMBER_DOT                         _T("ShowNumberDot")
#define CONFIG_MENU_SHOW_NUMBER_FILL                        _T("ShowNumberFill")

#define CONFIG_MENU_SHOW_STYLE                              _T("ShowStyle")
#define CONFIG_MENU_SHOW_STYLE_DEFAULT                      (0)
#define CONFIG_MENU_SHOW_STYLE_WHITE                        (1)
#define CONFIG_MENU_PAGE_SIZE                               _T("PageSize")
#define CONFIG_MENU_PAGE_NUMBER                             _T("PageNumber")
#define CONFIG_MENU_SIZE                                    _T("MenuSize")
#define CONFIG_MENU_SIZE_128                                8
#define CONFIG_MENU_SIZE_256                                16
#define CONFIG_MENU_SIZE_512                                32
#define CONFIG_MENU_SIZE_1024                               64
#define CONFIG_MENU_SIZE_2048                               128
#define CONFIG_MENU_MUSIC                                   _T("Music")
#define CONFIG_MENU_MUSIC_DISABLE                           (0)
#define CONFIG_MENU_MUSIC_DEFAULT                           (1)
#define CONFIG_MENU_MUSIC_NINJA_GAIDEN                      (2)
#define CONFIG_MENU_MUSIC_FINAL_MISSOIN                     (3)
#define CONFIG_MENU_MUSIC_CONTRA                            (4)
#define CONFIG_MENU_MUSIC_SUPER_CONTRA                      (5)
#define CONFIG_MENU_PAGE_TURNING_SPEED                      _T("PageTurningSpeed")
#define CONFIG_MENU_PAGE_SELECTION_CENTER                   _T("InitSelectionCenter")

#define CONFIG_RENAME_SEARCH                                _T("Search")
#define CONFIG_RENAME_REPLACE                               _T("Replace")
#define CONFIG_RENAME_PATTERN                               _T("Pattern")
#define CONFIG_RENAME_ONLY_CHECKED                          _T("OnlyChecked")

#define CONFIG_LAST_OPEN_DIR                                _T("LastOpenDir")
#define CONFIG_LAST_BUILD_DIR                               _T("LastBuildDir")
#define CONFIG_LAST_EXPORT_DIR                              _T("LastExportDir")
#define CONFIG_LAST_LOAD_DIR                                _T("LastLoadDir")

#define CONFIG_WINDOW_CLOSE_TRAY                            (0)
#define CONFIG_WINDOW_CLOSE_EXIT                            (1)

#define CONFIG_WINDOW_REMEMBER_STATE                        _T("Remember")
#define CONFIG_WINDOW_REMEMBER_FALSE                        (0)
#define CONFIG_WINDOW_REMEMBER_TRUE                         (1)

#define CONFIG_HISTORY_VALUE                                _T("Data")

#define CONFIG_MENU_NAME_TABLE_ATTRIBUTE                    _T("Attributes")
#define CONFIG_MENU_BKG_PALETTE                             _T("BkgPalette")
#define CONFIG_MENU_SPRITE_PALETTE                          _T("SpritePalette")
#define CONFIG_MENU_BKG_TILE_LIST                           _T("BkgTileList")
#define CONFIG_MENU_SPRITE_TILE_LIST                        _T("SpriteTileList")
#define CONFIG_MENU_HISTORY_DATA_LIST                       _T("HistoryDataList")
#define CONFIG_MENU_HISTORY_DATA_INDEX                      _T("HistoryDataIndex")
#define CONFIG_MENU_LAST_WORK_STATE                         _T("LastWorkState")
#define CONFIG_MENU_CHAR_MAP_DATA                           _T("CharMapData")

#define CONFIG_MENU_ARROW_TILE_LIST                         _T("ArrowTileList")
#define CONFIG_MENU_CHR_DATA                                _T("ChrData")
#define CONFIG_MENU_CUSTOM_TEXT_EDIT                        _T("CustomTextEdit")
#define CONFIG_MENU_CUSTOM_TEXT                             _T("CustomText")
#define CONFIG_MENU_BKG_PALETTE_POS                         _T("BkgPalettePos")
#define CONFIG_MENU_SP_PALETTE_POS                          _T("SpPalettePos")

#define CONFIG_MENU_CAPTION_POS                             _T("CaptionPos")
#define CONFIG_MENU_PAGE_NUMBER_POS                         _T("PageNumberPos")
#define CONFIG_MENU_ITEM_POS                                _T("ItemPos")
#define CONFIG_MENU_FILL_COLOR                              _T("FillColor")
#define CONFIG_MENU_DRAW_LINE                               _T("DrawLine")
#define CONFIG_MENU_DRAG_TEXT                               _T("DragText")
#define CONFIG_MENU_ARROW_TILE                              _T("ArrowTile")
#define CONFIG_MENU_BIG_CURSOR                              _T("BigCursor")
#define CONFIG_MENU_BIG_SPRITE                              _T("BigSprite")

#define CONFIG_MENU_CAPTION_PPU_ADDR                        _T("CaptionPpuAddr")
#define CONFIG_MENU_PAGE_NUMBER_PPU_ADDR                    _T("PageNumberPpuAddr")
#define CONFIG_MENU_ITEM_PPU_ADDR                           _T("ItemPpuAddr")
#define CONFIG_MENU_APPLY_ENABLE                            _T("CustomEnable")
#define CONFIG_MENU_CAPTION_TEXT                            _T("CaptionText")

#define CONFIG_REGISTER_PRODUCT_ID                          _T("ProductID")
#define CONFIG_REGISTER_PRODUCT_TYPE                        _T("ProductType")
#define CONFIG_REGISTER_USER_NAME                           _T("UserName")
#define CONFIG_REGISTER_ENABLE                              _T("Enable")
#define CONFIG_REGISTER_UNREGISTER                          _T("Unregister")

#define CONFIG_DESIGN_REGISTER_PRODUCT_ID                   _T("ProductID")
#define CONFIG_DESIGN_REGISTER_PRODUCT_TYPE                 _T("ProductType")
#define CONFIG_DESIGN_REGISTER_USER_NAME                    _T("UserName")
#define CONFIG_DESIGN_REGISTER_ENABLE                       _T("Enable")
#define CONFIG_DESIGN_REGISTER_UNREGISTER                   _T("Unregister")

#define CONFIG_DESIGN_SHOW_BG_TILE                          _T("Show_Bg_Tile")
#define CONFIG_DESIGN_SHOW_SP_TILE                          _T("Show_Sp_Tile")
#define CONFIG_DESIGN_SHOW_LIST                             _T("Show_List")
#define CONFIG_DESIGN_SHOW_CUSTOM_TEXT                      _T("Show_Custom_Text")
#define CONFIG_DESIGN_SHOW_CAPTION                          _T("Show_Caption")
#define CONFIG_DESIGN_SHOW_CURSOR                           _T("Show_Cursor")

#define UNREGISTER_LIMIT_SIZE                               (1024 * 1024 * 2)
#define UNREGISTER_LIMIT_COUNT                              (8)

#define UNREGISTER_LIMIT_BASIC_SIZE                         (1024 * 1024 * 12)
#define UNREGISTER_LIMIT_BASIC_COUNT                        (32)

#define UNREGISTER_LIMIT_STANDARD_SIZE                      (1024 * 1024 * 16)
#define UNREGISTER_LIMIT_STANDARD_COUNT                     (64)

#define MULTICATR_TYPE_COOLBOY                              (0)
#define MULTICATR_TYPE_MINDKIDS                             (1)
#define MULTICATR_TYPE_COOLGIRL                             (2)

#define EXPORT_INFO_SETTING_TYPE                            _T("ExportRomType")
#define EXPORT_INFO_SETTING_SIZE                            _T("ExportPrgSize")

#define EXPORT_INFO_SIZE_128                                (0)
#define EXPORT_INFO_SIZE_256                                (1)
#define EXPORT_INFO_SIZE_512                                (2)

#define EXTERN_MENU_ENABLE                                  _T("ExternMenuEnable")
#define EXTERN_MENU_PATH                                    _T("ExternMenuPath")
#define SINGLE_GAME_NO_MENU                                 _T("SingleGameNoMenu")
#define FINAL_DATA_ENABLE                                   (1)

#define LAST_NOTIFY_ID                                     _T("LastNotifyId")

#define NAME_INSERT_ROW_ENABLE                             _T("NameInsertRow")

enum Fca_Type
{
    FT_None,
    FT_Basic,
    FT_Standard,
    FT_Advanced,
    FT_Pro,
    FT_Full,
    FT_Max,
};

enum Fcad_Type
{
    FDT_None,
    FDT_Basic,
    FDT_Standard,
    FDT_Advanced,
    FDT_Pro,
    FDT_Full,
    FDT_Max,
};

typedef struct _FCA_TYPE_LIMIT
{
    Fca_Type dwType = Fca_Type::FT_None;
    DWORD dwRomSize = 0;
    DWORD dwRomCount = 0;
    DWORD dwData = 0;

    _FCA_TYPE_LIMIT(Fca_Type type, DWORD nSize, DWORD nCount, DWORD dwID)
    {
        dwType = type;
        dwRomSize = nSize;
        dwRomCount = nCount;
        dwData = dwID;
    }
}FCA_TYPE_LIMIT;

typedef struct _UPDATE_INFO
{
    std::map<_tstring, _tstring> mapNotifyMsg;
    std::map<_tstring, _tstring> mapUpdateDesc;
    std::vector<_tstring> vBlackList;
    std::vector<_tstring> vWhiteList;
    std::vector<_tstring> vDesignBlackList;
    std::vector<_tstring> vDesignWhiteList;
    _tstring strVersion;
    _tstring strFileUrl;
    _tstring strFileUrlX86;
    _tstring strSavePath;
    _tstring strNotifyID;
    bool bForceUpdate = false;
    bool bBlackList = false;
    bool bWhiteList = false;
    bool bDesignBlackList = false;
    bool bDesignWhiteList = false;
    bool bFreeFunction = false;
    bool bFreeDesign = false;
}UPDATE_INFO;

typedef struct _NOTIFY_INFO
{
    int nId = 0;
    bool bRestartShow = false;
    int nForUserType = 0;
    std::map<_tstring, _tstring> mapNotifyMsg;
}NOTIFY_INFO;

typedef struct _RENAME_INFO
{
    _tstring strSearch;
    _tstring strReplace;
    bool bRegex = false;
    bool bChecked = false;

}RENAME_INFO, *LPRENAME_INFO;

typedef struct _MAPPER_STANDARD
{
    int m_Mapper;
    std::set<int> m_PrgSet;
    std::set<int> m_ChrSet;

    _MAPPER_STANDARD(int nMapper, std::set<int> PrgSet, std::set<int> ChrSet)
    {
        m_Mapper = nMapper;
        m_PrgSet = PrgSet;
        m_ChrSet = ChrSet;
    }

}MAPPER_STANDARD;

extern FCA_TYPE_LIMIT g_FcaLimitForDesign[FT_Max];
extern FCA_TYPE_LIMIT g_FcaLimitForCoolBoy[FT_Max];
extern FCA_TYPE_LIMIT g_FcaLimitForCoolGirl[FT_Max];
extern std::map<int, MAPPER_STANDARD> g_CoolGirlMapperTable;