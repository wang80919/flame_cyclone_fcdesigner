#pragma once

#include <wtypesbase.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "Common.h"

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CRegisterUtils
{
public:

    bool IsRegistered();
    bool Register(_tstring strUserName, _tstring strProductKey);
    bool Unregister();
    Fca_Type GetType();

    _tstring GenerateProductKey(
        _tstring& SerialNumber, 
        _tstring& strUserName,
        Fca_Type dwType = Fca_Type::FT_None
    );

    _tstring GetProductKey();
    _tstring GetProductName();
    _tstring GetUser();

    static CRegisterUtils& Instance()
    {
        return m_Instance;
    }

    _tstring GetSerialNumber();
    bool GetEnable() const;

    void Enable(bool bEnable = false);
    void Free(bool bFree = false);
    bool IsEnable() const;
    bool IsFree() const;

private:
    CRegisterUtils();
    ~CRegisterUtils();
    _tstring LoadString(UINT uID);

private:
    static CRegisterUtils m_Instance;
    _tstring m_strSerialNumber;
    bool m_Registered = false;
    bool m_bChecked = false;
    bool m_bEnable = true;
    bool m_bFree = false;
    Fca_Type m_dwType = FT_None;     //1: 基础版, 2: 标准版, 3: 高级版 4:完全版
};