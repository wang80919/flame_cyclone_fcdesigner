#include "CDesignDialog.h"
#include "resource.h"

BEGIN_DLG_MESSAGE_MAP(CDesignDialog, CDialogBase)

    
    ON_DLG_MESSAGE(WM_INITDIALOG, &CDesignDialog::OnInitDialog)
    ON_DLG_MESSAGE(WM_WINDOWPOSCHANGING, &CDesignDialog::OnWindowPosChanging)
    ON_DLG_MESSAGE(WM_CLOSE, &CDesignDialog::OnClose)
    ON_DLG_MESSAGE(WM_TIMER, &CDesignDialog::OnTimer)
    ON_DLG_MESSAGE(WM_CTLCOLORSTATIC, &CDesignDialog::OnColor)
    ON_DLG_MESSAGE(WM_CTLCOLORBTN, &CDesignDialog::OnColor)
    ON_DLG_MESSAGE(WM_CTLCOLORDLG, &CDesignDialog::OnColor)
    ON_DLG_MESSAGE(WM_CTLCOLORLISTBOX, &CDesignDialog::OnColor)
    ON_DLG_MESSAGE(WM_CTLCOLOREDIT, &CDesignDialog::OnColor)
    ON_DLG_MESSAGE(WM_CTLCOLORMSGBOX, &CDesignDialog::OnColor)
    ON_DLG_MESSAGE(WM_LANGUAGE_CHANGE, &CDesignDialog::OnLanguageChange)
    ON_DLG_MESSAGE(WM_PAINT, &CDesignDialog::OnPaint)
    ON_DLG_MESSAGE(WM_DISPLAYCHANGE, &CDesignDialog::OnDisplayChange)
    ON_DLG_MESSAGE(WM_LBUTTONDOWN, &CDesignDialog::OnLButtonDown)
    ON_DLG_MESSAGE(WM_LBUTTONDBLCLK, &CDesignDialog::OnLButtonDbClick)
    ON_DLG_MESSAGE(WM_LBUTTONUP, &CDesignDialog::OnLButtonUp)
    ON_DLG_MESSAGE(WM_MBUTTONDOWN, &CDesignDialog::OnMButtonDown)

    ON_DLG_MESSAGE(WM_MOUSEMOVE, &CDesignDialog::OnMouseMove)
    ON_DLG_COMMAND(IDC_CHECK_FILL_COLOR, &CDesignDialog::OnCmdFillColor)
    ON_DLG_COMMAND(IDC_CHECK_DRAW_LINE, &CDesignDialog::OnCmdDrawLine)
    ON_DLG_COMMAND(IDC_CHECK_DRAG_TEXT, &CDesignDialog::OnCmdDragText)

    ON_DLG_COMMAND(IDC_BUTTON_UNDO, &CDesignDialog::OnCmdUndo)
    ON_DLG_COMMAND(IDC_BUTTON_REDO, &CDesignDialog::OnCmdRedo)

    ON_DLG_COMMAND(IDC_BUTTON_EXPORT_CHAR_MAPPING, &CDesignDialog::OnCmdExportCharMapping)
    ON_DLG_COMMAND(IDC_BUTTON_RESET_CHAR_MAPPING, &CDesignDialog::OnCmdResetCharMapping)

    ON_DLG_COMMAND(IDC_BUTTON_LOAD, &CDesignDialog::OnCmdLoad)
    ON_DLG_COMMAND(IDC_BUTTON_SAVE_AS, &CDesignDialog::OnCmdSave)
    ON_DLG_COMMAND(IDC_BUTTON_RESET, &CDesignDialog::OnCmdReset)

    ON_DLG_COMMAND(IDC_BUTTON_TEXT_LOAD, &CDesignDialog::OnCmdTextLoad)
    ON_DLG_COMMAND(IDC_BUTTON_TEXT_SAVE_AS, &CDesignDialog::OnCmdTextSave)
    ON_DLG_COMMAND(IDC_BUTTON_TEXT_RESET, &CDesignDialog::OnCmdTextReset)

    ON_DLG_COMMAND(IDC_BUTTON_ATTR_LOAD, &CDesignDialog::OnCmdAttrLoad)
    ON_DLG_COMMAND(IDC_BUTTON_ATTR_SAVE_AS, &CDesignDialog::OnCmdAttrSave)
    ON_DLG_COMMAND(IDC_BUTTON_ATTR_RESET, &CDesignDialog::OnCmdAttrReset)

    ON_DLG_COMMAND(IDC_BUTTON_LAYOUT_LOAD, &CDesignDialog::OnCmdLayoutLoad)
    ON_DLG_COMMAND(IDC_BUTTON_LAYOUT_SAVE_AS, &CDesignDialog::OnCmdLayoutSave)
    ON_DLG_COMMAND(IDC_BUTTON_LAYOUT_RESET, &CDesignDialog::OnCmdLayoutReset)

    ON_DLG_COMMAND(IDC_BUTTON_BKG_CHR_LOAD, &CDesignDialog::OnCmdBkgChrLoad)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_CHR_SAVE_AS, &CDesignDialog::OnCmdBkgChrSave)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_CHR_RESET, &CDesignDialog::OnCmdBkgChrReset)

    ON_DLG_COMMAND(IDC_BUTTON_SWAP_CHR, &CDesignDialog::OnCmdSwapChr)

    ON_DLG_COMMAND(IDC_BUTTON_SP_CHR_LOAD, &CDesignDialog::OnCmdSpChrLoad)
    ON_DLG_COMMAND(IDC_BUTTON_SP_CHR_SAVE_AS, &CDesignDialog::OnCmdSpChrSave)
    ON_DLG_COMMAND(IDC_BUTTON_SP_CHR_RESET, &CDesignDialog::OnCmdSpChrReset)

    ON_DLG_COMMAND(IDC_BUTTON_BKG_SELECT_ALL, &CDesignDialog::OnCmdBkgSelAll)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_SELECT_INVERSE, &CDesignDialog::OnCmdBkgSelInverse)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_LIST_LOAD, &CDesignDialog::OnCmdBkgListLoad)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_LIST_SAVE_AS, &CDesignDialog::OnCmdBkgListSave)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_LIST_RESET, &CDesignDialog::OnCmdBkgListReset)

    ON_DLG_COMMAND(IDC_BUTTON_SP_SELECT_ALL, &CDesignDialog::OnCmdSpSelAll)
    ON_DLG_COMMAND(IDC_BUTTON_SP_SELECT_INVERSE, &CDesignDialog::OnCmdSpSelInverse)
    ON_DLG_COMMAND(IDC_BUTTON_SP_LIST_LOAD, &CDesignDialog::OnCmdSpListLoad)
    ON_DLG_COMMAND(IDC_BUTTON_SP_LIST_SAVE_AS, &CDesignDialog::OnCmdSpListSave)
    ON_DLG_COMMAND(IDC_BUTTON_SP_LIST_RESET, &CDesignDialog::OnCmdSpListReset)
    ON_DLG_COMMAND(IDC_BUTTON_SWAP_SP, &CDesignDialog::OnCmdSpListSwap)

    ON_DLG_COMMAND(IDC_BUTTON_BKG_LEFT, &CDesignDialog::OnCmdBkgListLeft)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_UP, &CDesignDialog::OnCmdBkgListUp)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_RIGHT, &CDesignDialog::OnCmdBkgListRight)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_DOWN, &CDesignDialog::OnCmdBkgListDown)

    ON_DLG_COMMAND(IDC_BUTTON_ATTR_LEFT, &CDesignDialog::OnCmdMoveAttrLeft)
    ON_DLG_COMMAND(IDC_BUTTON_ATTR_UP, &CDesignDialog::OnCmdMoveAttrUp)
    ON_DLG_COMMAND(IDC_BUTTON_ATTR_RIGHT, &CDesignDialog::OnCmdMoveAttrRight)
    ON_DLG_COMMAND(IDC_BUTTON_ATTR_DOWN, &CDesignDialog::OnCmdMoveAttrDown)

    ON_DLG_COMMAND(IDC_BUTTON_SP_LEFT, &CDesignDialog::OnCmdSpListLeft)
    ON_DLG_COMMAND(IDC_BUTTON_SP_UP, &CDesignDialog::OnCmdSpListUp)
    ON_DLG_COMMAND(IDC_BUTTON_SP_RIGHT, &CDesignDialog::OnCmdSpListRight)
    ON_DLG_COMMAND(IDC_BUTTON_SP_DOWN, &CDesignDialog::OnCmdSpListDown)

    ON_DLG_COMMAND(IDC_BUTTON_BKG_PALETTE_LOAD, &CDesignDialog::OnCmdBkgPaletteLoad)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_PALETTE_SAVE_AS, &CDesignDialog::OnCmdBkgPaletteSave)
    ON_DLG_COMMAND(IDC_BUTTON_BKG_PALETTE_RESET, &CDesignDialog::OnCmdBkgPaletteReset)

    ON_DLG_COMMAND(IDC_BUTTON_SP_PALETTE_LOAD, &CDesignDialog::OnCmdSpPaletteLoad)
    ON_DLG_COMMAND(IDC_BUTTON_SP_PALETTE_SAVE_AS, &CDesignDialog::OnCmdSpPaletteSave)
    ON_DLG_COMMAND(IDC_BUTTON_SP_PALETTE_RESET, &CDesignDialog::OnCmdSpPaletteReset)

    ON_DLG_COMMAND(IDC_BUTTON_FIRST_PAGE, &CDesignDialog::ShowFirstPage)
    ON_DLG_COMMAND(IDC_BUTTON_LAST_PAGE, &CDesignDialog::ShowLastPage)
    ON_DLG_COMMAND(IDC_BUTTON_PRE_PAGE, &CDesignDialog::ShowPrePage)
    ON_DLG_COMMAND(IDC_BUTTON_NEXT_PAGE, &CDesignDialog::ShowNextPage)

    ON_DLG_COMMAND(IDC_BUTTON_TEXT_ADD, &CDesignDialog::OnCmdAddText)
    ON_DLG_COMMAND(IDC_CHECK_SPRITE_V, &CDesignDialog::OnCmdVFlip)
    ON_DLG_COMMAND(IDC_CHECK_SPRITE_H, &CDesignDialog::OnCmdHFlip)
    ON_DLG_COMMAND(IDC_CHECK_SPRITE_BACK, &CDesignDialog::OnCmdBackgroundDisplay)
    ON_DLG_COMMAND(IDC_CHECK_BIG_CURSOR, &CDesignDialog::OnCmdBigCursor)
    ON_DLG_COMMAND(IDC_CHECK_BIG_SPRITE, &CDesignDialog::OnCmdBigSprite)
    ON_DLG_COMMAND(IDC_CHECK_CHAR_MAP_FAST, &CDesignDialog::OnCmdCharMappingQuick)
    ON_DLG_COMMAND(IDC_CHECK_BKG_CHR_FAST, &CDesignDialog::OnCmdBkgChrQuick)
    ON_DLG_COMMAND(IDC_CHECK_SP_CHR_FAST, &CDesignDialog::OnCmdSpChrQuick)
    ON_DLG_COMMAND(IDC_CHECK_BKG_LIST_FAST, &CDesignDialog::OnCmdBkgListQuick)

    ON_DLG_COMMAND(ID_FILE_EXIT, &CDesignDialog::OnCmdFileExit)
    ON_DLG_COMMAND(ID_EDIT_CLEAR_UNUSE_BKG_TILE, &CDesignDialog::OnCmdClearUnuseBkgTile)
    ON_DLG_COMMAND(ID_EDIT_CLEAR_UNUSE_SP_TILE, &CDesignDialog::OnCmdClearUnuseSpTile)
    ON_DLG_COMMAND(ID_EDIT_AUTO_CHAR_MAPPING, &CDesignDialog::OnCmdAutoCharMapping)
    ON_DLG_COMMAND(ID_EDIT_AUTO_FULL_CHAR_MAPPING, &CDesignDialog::OnCmdAutoFullCharMapping)
    ON_DLG_COMMAND(ID_EDIT_BG_TO_SP, &CDesignDialog::OnCmdCopyBgToSp)
    ON_DLG_COMMAND(ID_EDIT_SP_TO_BG, &CDesignDialog::OnCmdCopySpToBg)
    ON_DLG_COMMAND(ID_EDIT_BG_CHAR_MAPPING, &CDesignDialog::OnCmdBgCharMapping)
    ON_DLG_COMMAND(ID_EDIT_SP_CHAR_MAPPING, &CDesignDialog::OnCmdSpCharMapping)
    ON_DLG_COMMAND(ID_EDIT_BG_ORGANIZE, &CDesignDialog::OnCmdBgOrganize)
    ON_DLG_COMMAND(ID_EDIT_SP_ORGANIZE, &CDesignDialog::OnCmdSpOrganize)

    ON_DLG_COMMAND(ID_TILE_DELETE_COLOR_0, &CDesignDialog::OnCmdDeleteColor0)
    ON_DLG_COMMAND(ID_TILE_DELETE_COLOR_1, &CDesignDialog::OnCmdDeleteColor1)
    ON_DLG_COMMAND(ID_TILE_DELETE_COLOR_2, &CDesignDialog::OnCmdDeleteColor2)
    ON_DLG_COMMAND(ID_TILE_DELETE_COLOR_3, &CDesignDialog::OnCmdDeleteColor3)

    ON_DLG_COMMAND(ID_TILE_MOVE_UP, &CDesignDialog::OnCmdMovePixelUp)
    ON_DLG_COMMAND(ID_TILE_MOVE_DOWN, &CDesignDialog::OnCmdMovePixelDown)
    ON_DLG_COMMAND(ID_TILE_MOVE_LEFT, &CDesignDialog::OnCmdMovePixelLeft)
    ON_DLG_COMMAND(ID_TILE_MOVE_RIGHT, &CDesignDialog::OnCmdMovePixelRight)

    ON_DLG_COMMAND(ID_TILE_COLOR123_MOVE_LEFT, &CDesignDialog::OnCmdMoveColorLeft123)
    ON_DLG_COMMAND(ID_TILE_COLOR123_MOVE_RIGHT, &CDesignDialog::OnCmdMoveColorRight123)

    ON_DLG_COMMAND(ID_TILE_COLOR0123_MOVE_LEFT, &CDesignDialog::OnCmdMoveColorLeft0123)
    ON_DLG_COMMAND(ID_TILE_COLOR0123_MOVE_RIGHT, &CDesignDialog::OnCmdMoveColorRight0123)

    ON_DLG_COMMAND(ID_TILE_SWAP_COLOR_1_2, &CDesignDialog::OnCmdSwapColor12)
    ON_DLG_COMMAND(ID_TILE_SWAP_COLOR_1_3, &CDesignDialog::OnCmdSwapColor13)
    ON_DLG_COMMAND(ID_TILE_SWAP_COLOR_2_3, &CDesignDialog::OnCmdSwapColor23)

    ON_DLG_COMMAND(IDC_CHECK_MARK_CURSOR, &CDesignDialog::OnCmdMarkCursor)

    ON_DLG_COMMAND(ID_EDIT_SP_8_16_TO_8_8, &CDesignDialog::OnCmdSp16To8)
    ON_DLG_COMMAND(ID_EDIT_SP_ID_SWAP, &CDesignDialog::OnCmdSpIdSwap)
    ON_DLG_COMMAND(ID_EDIT_BG_TILE_TO_SP_TILE, &CDesignDialog::OnCmdBgTileToSpTile)

    ON_DLG_COMMAND(ID_SHOW_BG, &CDesignDialog::OnCmdShowBgTile)
    ON_DLG_COMMAND(ID_SHOW_SP, &CDesignDialog::OnCmdShowSpTile)
    ON_DLG_COMMAND(ID_SHOW_LIST, &CDesignDialog::OnCmdShowList)
    ON_DLG_COMMAND(ID_SHOW_CUSTOM_TEXT, &CDesignDialog::OnCmdShowCustomText)
    ON_DLG_COMMAND(ID_SHOW_CAPTION, &CDesignDialog::OnCmdShowCaption)
    ON_DLG_COMMAND(ID_SHOW_CURSOR, &CDesignDialog::OnCmdShowCursor)

END_DLG_MESSAGE_MAP()
