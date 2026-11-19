#include "CFileVersion.h"
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "Version.lib")

CFileVersion::CFileVersion()
    :
    m_lpVerData(NULL),
    m_dwVerSize(0),
    m_lpTranslate(NULL)
{
}

CFileVersion::~CFileVersion()
{
    this->Close();
}

_tstring CFileVersion::GetCurrentExeVersion()
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szBuf, _countof(szBuf));

    CFileVersion obj;
    obj.LoadFile(szBuf);
    _tstring strVer;
    obj.GetFileVersion(strVer);
    return strVer;
}

BOOL CFileVersion::LoadFile(const _tstring& strFile)
{
    BOOL isDisableWow64Fs = FALSE;
    PVOID OldValue = NULL;
    BOOL isSuccess = FALSE;
    UINT cbTranslate = 0;

    if (strFile.empty())
    {
        goto L_Cleanup;
    }

    this->Close();

    isDisableWow64Fs = Wow64DisableWow64FsRedirection(&OldValue);

    m_dwVerSize = GetFileVersionInfoSize(strFile.c_str(), 0);
    if (0 == m_dwVerSize)
    {
        goto L_Cleanup;
    }

    m_lpVerData = malloc(m_dwVerSize);
    if (!m_lpVerData)
    {
        goto L_Cleanup;
    }

    if (!GetFileVersionInfo(strFile.c_str(), 0, m_dwVerSize, m_lpVerData))
    {
        goto L_Cleanup;
    }

    if (!VerQueryValue(m_lpVerData, _T("\\VarFileInfo\\Translation"), (LPVOID*)&m_lpTranslate, &cbTranslate))
    {
        goto L_Cleanup;
    }
    
    isSuccess = TRUE;

L_Cleanup:

    if (!isSuccess)
    {
        if (!m_lpVerData)
        {
            free(m_lpVerData);
            m_lpVerData = NULL;
        }
    }

    if (isDisableWow64Fs)
    {
        Wow64RevertWow64FsRedirection(OldValue);
    }

    return isSuccess;
}

VOID CFileVersion::Close()
{
    if (m_lpVerData)
    {
        free(m_lpVerData);
        m_lpVerData = NULL;
        m_dwVerSize = 0;
    }
}

BOOL CFileVersion::QueryInfo(const _tstring& strName, _tstring& strRes) const
{
    TCHAR strQuery[MAX_PATH] = { 0 };
    HRESULT hr = S_OK;
    BOOL isSuccess = FALSE;
    LPCTSTR lpQueryRes = NULL;
    UINT uQueryCchSize;

    strRes.clear();

    if (!m_lpVerData || !m_lpTranslate)
    {
        goto L_Cleanup;
    }

    hr = StringCchPrintf(strQuery, _countof(strQuery), 
        _T("\\StringFileInfo\\%04x%04x\\%s"), 
        m_lpTranslate[0].wLanguage, 
        m_lpTranslate[0].wCodePage, 
        strName.c_str());
    if (FAILED(hr))
    {
        goto L_Cleanup;
    }

    if (!VerQueryValue(m_lpVerData, strQuery, (LPVOID*)&lpQueryRes, &uQueryCchSize))
    {
        goto L_Cleanup;

    }

    strRes = lpQueryRes;
    isSuccess = TRUE;

L_Cleanup:

    return isSuccess;
}

BOOL CFileVersion::GetComments(_tstring& strRes) const
{
    return QueryInfo(_T("Comments"), strRes);
}

BOOL CFileVersion::GetInternalName(_tstring& strRes) const
{
    return QueryInfo(_T("InternalName"), strRes);
}

BOOL CFileVersion::GetProductName(_tstring& strRes) const
{
    return QueryInfo(_T("ProductName"), strRes);
}

BOOL CFileVersion::GetCompanyName(_tstring& strRes) const
{
    return QueryInfo(_T("CompanyName"), strRes);
}

BOOL CFileVersion::GetLegalCopyright(_tstring& strRes) const
{
    return QueryInfo(_T("LegalCopyright"), strRes);
}

BOOL CFileVersion::GetProductVersion(_tstring& strRes) const
{
    return QueryInfo(_T("ProductVersion"), strRes);
}

BOOL CFileVersion::GetFileDescription(_tstring& strRes) const
    {
        return QueryInfo(_T("FileDescription"), strRes);
    }

BOOL CFileVersion::GetLegalTrademarks(_tstring& strRes) const
{
    return QueryInfo(_T("LegalTrademarks"), strRes);
}

BOOL CFileVersion::GetPrivateBuild(_tstring& strRes) const
{
    return QueryInfo(_T("PrivateBuild"), strRes);
}

BOOL CFileVersion::GetFileVersion(_tstring& strRes) const
{
    return QueryInfo(_T("FileVersion"), strRes);
}

BOOL CFileVersion::GetOriginalFilename(_tstring& strRes) const
{
    return QueryInfo(_T("OriginalFilename"), strRes);
}

BOOL CFileVersion::GetSpecialBuild(_tstring& strRes) const
{
    return QueryInfo(_T("SpecialBuild"), strRes);
}
