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

class CDesignRegisterUtils
{
public:

    bool IsRegistered();
    bool Register(_tstring strUserName, _tstring strProductKey);
    bool Unregister();
    Fcad_Type GetType();

    _tstring GenerateProductKey(
        _tstring& SerialNumber, 
        _tstring& strUserName,
        Fcad_Type dwType = Fcad_Type::FDT_None
    );

    _tstring GetProductKey();
    _tstring GetProductName();
    _tstring GetUser();

    static CDesignRegisterUtils& Instance()
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
    CDesignRegisterUtils();
    ~CDesignRegisterUtils();
    _tstring LoadString(UINT uID);

private:
    static CDesignRegisterUtils m_Instance;
    _tstring m_strSerialNumber;
    bool m_Registered = false;
    bool m_bChecked = false;
    bool m_bEnable = true;
    bool m_bFree = false;
    Fcad_Type m_dwType = FDT_None;     //1: 基础版, 2: 标准版, 3: 高级版 4:完全版
};