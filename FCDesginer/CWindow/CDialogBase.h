#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>

#pragma warning(disable:4100)

#define   STRING_MAX_SIZE           (1024 * 64)

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CDialogBase;

typedef LRESULT(CDialogBase::* pDlgMapFn)(WPARAM wParam, LPARAM lParam);

struct WND_DLG_MSGMAP_ENTRY
{
    pDlgMapFn m_pfn;
    UINT m_nMessage;
    UINT m_nID;
    WND_DLG_MSGMAP_ENTRY(UINT uMsg, pDlgMapFn pfn) : m_nMessage(uMsg), m_pfn(pfn), m_nID(0) {}
    WND_DLG_MSGMAP_ENTRY(UINT uMsg, UINT nID, pDlgMapFn pfn) : m_nMessage(uMsg), m_nID(nID), m_pfn(pfn) {}
};

struct WND_DLG_MSGMAP
{
    const WND_DLG_MSGMAP* (*pfnGetBaseMap)();
    const WND_DLG_MSGMAP_ENTRY* lpEntries;
};

#ifndef dlg_msg
#define dlg_msg
#endif

#define DECLARE_DLG_MESSAGE_MAP()                                   \
protected:                                                      \
static const WND_DLG_MSGMAP* GetThisMessageMap();                   \
virtual const WND_DLG_MSGMAP* GetMessageMap() const;                \
                                                                
#define BEGIN_DLG_MESSAGE_MAP(theClass, baseClass)                  \
const WND_DLG_MSGMAP* theClass::GetMessageMap() const               \
{                                                               \
    return GetThisMessageMap();                                 \
}                                                               \
const WND_DLG_MSGMAP* theClass::GetThisMessageMap()                 \
{                                                               \
    typedef baseClass TheBaseClass;					            \
static const WND_DLG_MSGMAP_ENTRY _messageEntries[] =               \
{                                                               \
                                                                
#define ON_DLG_MESSAGE(message, memberFxn)                          \
WND_DLG_MSGMAP_ENTRY(message, static_cast< LRESULT (CDialogBase::*)(WPARAM, LPARAM) >(memberFxn)),

#define ON_DLG_COMMAND(_id, memberFxn)                          \
WND_DLG_MSGMAP_ENTRY(WM_COMMAND, _id, static_cast< LRESULT (CDialogBase::*)(WPARAM, LPARAM) >(memberFxn)),

#define END_DLG_MESSAGE_MAP()  WND_DLG_MSGMAP_ENTRY(0, NULL) };         \
static const WND_DLG_MSGMAP messageMap =                            \
{&TheBaseClass::GetThisMessageMap , &_messageEntries[0] };      \
return &messageMap;                                             \
}                                                               \

#define DECLARE_DLG_BASE_MESSAGE_MAP()                              \
protected:                                                      \
static const WND_DLG_MSGMAP* GetThisMessageMap();                   \
virtual const WND_DLG_MSGMAP* GetMessageMap() const;                \
static const WND_DLG_MSGMAP* GetBaseMessageMap();                   \

#define BEGIN_DLG_BASE_MESSAGE_MAP(theClass)                        \
typedef theClass TheThisClass;					                \
const WND_DLG_MSGMAP* TheThisClass::GetMessageMap() const           \
{                                                               \
    return GetThisMessageMap();                                 \
}                                                               \
const WND_DLG_MSGMAP* TheThisClass::GetBaseMessageMap()             \
{                                                               \
    return NULL;                                                \
}                                                               \
const WND_DLG_MSGMAP* TheThisClass::GetThisMessageMap()             \
{                                                               \
static const WND_DLG_MSGMAP_ENTRY _messageEntries[] =               \
{                                                               \

#define END_DLG_BASE_MESSAGE_MAP()  WND_DLG_MSGMAP_ENTRY(0, NULL) };    \
static const WND_DLG_MSGMAP messageMap =                            \
{&TheThisClass::GetBaseMessageMap, &_messageEntries[0] };       \
return &messageMap;                                             \
}                                                               \

class CDialogBase
{
public:
    CDialogBase();
    ~CDialogBase();

    dlg_msg INT_PTR DoModal(HWND hWndParent, bool bCenter = true);
    dlg_msg INT_PTR DoModal(UINT resourceID, HWND hWndParent, bool bCenter = true);
    dlg_msg INT_PTR DoModalEx(UINT resourceID, HWND hWndParent, BOOL isShow = TRUE, bool bCenter = true);
    dlg_msg HWND DoDialog(UINT resourceID, HWND hWndParent, BOOL isShow = TRUE, bool bCenter = true);
    dlg_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnLanguageChange(WPARAM wParam, LPARAM lParam);

    VOID SetResourceID(UINT resourceID);
    VOID SetLanguageID(LANGID langID);
    VOID ShowWindow(int cmdShow = SW_SHOW);
    BOOL IsVisiable() const;
    VOID MoveToCenter(HWND hParent);
    _tstring LoadString(UINT resourceID);
    _tstring GetItemString(DWORD dwID);
    void SetItemString(DWORD dwID, const _tstring strText);
    VOID ModifyMenuText(HMENU hMnu, UINT uPosition, LPCTSTR lpNewItem, BOOL fByPos = TRUE);
    VOID ModifyMenuText(HMENU hMnu, UINT uPosition, UINT resourceID, BOOL fByPos = TRUE);
    VOID ModifyMenuState(HMENU hMnu, UINT uPosition, UINT fState = MFS_ENABLED, BOOL fByPos = TRUE);
    HWND GetWndHandle() const;
    bool EndDialog(INT_PTR nResult);
    BOOL PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CheckButton(DWORD dwID, bool fCheck = true);
    bool IsChecked(DWORD dwID);

    operator HWND() const
    {
        return m_hWnd;
    }

protected:

    //¼ÙÄ£Ì¬¶Ô»°¿ò
    dlg_msg LRESULT DoFakeModal(UINT resourceID, HWND hWndParent, BOOL isShow = TRUE);
    static INT_PTR WINAPI DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

    virtual BOOL PreTranslateMessage(LPMSG pMsg);

protected:
    HWND m_hWnd;
    UINT m_resID;
    LANGID m_LangId;
    BOOL m_isModel;
    BOOL m_isFakeModel;
    BOOL m_isCenter;
    DECLARE_DLG_BASE_MESSAGE_MAP()
};