#pragma once

#include <windows.h>
#include <string>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

typedef struct _FILE_VERSION_INFO
{
    DWORD m_nVer0;
    DWORD m_nVer1;
    DWORD m_nVer2;
    DWORD m_nVer3;

    _FILE_VERSION_INFO() :m_nVer0(0), m_nVer1(0), m_nVer2(0), m_nVer3(0) {};

    _FILE_VERSION_INFO(LPCTSTR lpStrVer) :m_nVer0(0), m_nVer1(0), m_nVer2(0), m_nVer3(0)
    {
        _stscanf_s(lpStrVer, _T("%d.%d.%d.%d"),
            &m_nVer0,
            &m_nVer1,
            &m_nVer2,
            &m_nVer3
        );
    }

    _FILE_VERSION_INFO& operator = (LPCTSTR lpStrVer)
    {
        _stscanf_s(lpStrVer, _T("%d.%d.%d.%d"),
            &m_nVer0,
            &m_nVer1,
            &m_nVer2,
            &m_nVer3
        );

        return *this;
    }

    int CompareVersion(const _FILE_VERSION_INFO& ref)
    {
        if (m_nVer0 != ref.m_nVer0) return (m_nVer0 > ref.m_nVer0 ? 1 : -1);
        if (m_nVer1 != ref.m_nVer1) return (m_nVer1 > ref.m_nVer1 ? 1 : -1);
        if (m_nVer2 != ref.m_nVer2) return (m_nVer2 > ref.m_nVer2 ? 1 : -1);
        if (m_nVer3 != ref.m_nVer3) return (m_nVer3 > ref.m_nVer3 ? 1 : -1);

        return 0;
    }

    bool operator < (const _FILE_VERSION_INFO& ref)
    {
        return CompareVersion(ref) < 0;
    }

    bool operator == (const _FILE_VERSION_INFO& ref)
    {
        return CompareVersion(ref) == 0;
    }

    bool operator > (const _FILE_VERSION_INFO& ref)
    {
        return CompareVersion(ref) > 0;
    }

}FILE_VERSION_INFO;

class CFileVersion
{
    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    };

public:
    CFileVersion();
    ~CFileVersion();

    CFileVersion operator = (const CFileVersion& r) = delete;

    static _tstring GetCurrentExeVersion();

public:
    BOOL LoadFile(const _tstring& strFile);             //加载文件信息
    VOID Close();

    BOOL GetComments(_tstring& strRes) const;            //文件注释
    BOOL GetInternalName(_tstring& strRes) const;        //内部名称
    BOOL GetProductName(_tstring& strRes) const;         //产品名称
    BOOL GetCompanyName(_tstring& strRes) const;         //公司名称
    BOOL GetLegalCopyright(_tstring& strRes) const;      //法律版权
    BOOL GetProductVersion(_tstring& strRes) const;      //产品版本
    BOOL GetFileDescription(_tstring& strRes) const;     //文件描述
    BOOL GetLegalTrademarks(_tstring& strRes) const;     //合法商标
    BOOL GetPrivateBuild(_tstring& strRes) const;        //私有版本
    BOOL GetFileVersion(_tstring& strRes) const;         //文件版本
    BOOL GetOriginalFilename(_tstring& strRes) const;    //原始文件名
    BOOL GetSpecialBuild(_tstring& strRes) const;        //特别版本

private:
    BOOL QueryInfo(const _tstring& strName, _tstring& strRes) const;   //查询文件信息

    LPVOID m_lpVerData = NULL;                          //文件信息数据指针
    DWORD m_dwVerSize = 0;                              //文件信息数据大小
    LANGANDCODEPAGE *m_lpTranslate = NULL;              //语言代码页
};