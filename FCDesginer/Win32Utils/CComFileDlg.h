#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <commctrl.h>
#include <vector>
#include <shobjidl_core.h>

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CComFileDlg
{
public:

    static bool GetSavePath(HWND hOwner, _tstring& strPath, UINT& nTypeIndex, const COMDLG_FILTERSPEC* pFilters, int filterCnt);
    static bool GetSaveDir(HWND hOwner, _tstring& strDir);
    static bool GetOpenPath(HWND hOwner, _tstring& strPath, UINT& nTypeIndex, const COMDLG_FILTERSPEC* pFilters, int filterCnt);
    static std::vector<_tstring> GetFileList(HWND hOwner, _tstring& strDir, const COMDLG_FILTERSPEC* pFilters, int filterCnt);
    static _tstring GetCurrentModulePath();
    static _tstring GetCurrentModuleDir();
    static _tstring GetCurrentModuleName(bool bHasExt = false);
    static _tstring GetFileName(const _tstring& strPath, bool bHasExt/* = true*/);
    static bool IsExist(const _tstring& strPath);
    static bool IsDirectory(const _tstring& strPath);
    static bool IsArchive(const _tstring& strPath);
    static _tstring GetFileDir(const _tstring& strPath);
};

