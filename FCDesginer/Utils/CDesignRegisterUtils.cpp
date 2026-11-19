#include "CDesignRegisterUtils.h"
#include "Win32Utils/CStrUtils.h"
#include "Win32Utils/CRegUtils.h"
#include "Utils/CCrc32Utils.h"
#include "Win32Utils/CWmiUtils.h"
#include <strsafe.h>
CDesignRegisterUtils CDesignRegisterUtils::m_Instance;

CDesignRegisterUtils::CDesignRegisterUtils()
{
    if (!m_bChecked)
    {
        m_bChecked = true;
        _tstring strProductKey = GetProductKey();
        _tstring strSerialNumber = GetSerialNumber();
        _tstring strUserName = GetUser();

        Fcad_Type Types[] = {
            Fcad_Type::FDT_Full,
            Fcad_Type::FDT_Pro,
            Fcad_Type::FDT_Advanced,
            Fcad_Type::FDT_Standard,
            Fcad_Type::FDT_Basic
        };

        DWORD dwUnregister = 0;
        CRegUtils::GetDword(
            CONFIG_REG_KEY,
            CONFIG_DESIGN_REGISTER_PATH,
            CONFIG_DESIGN_REGISTER_UNREGISTER,
            dwUnregister);

        if (!dwUnregister)
        {
            for (auto item : Types)
            {
                _tstring strCheckKey = GenerateProductKey(m_strSerialNumber, strUserName, item);
                if (strCheckKey == strProductKey)
                {
                    m_dwType = item;
                    m_Registered = true;
                }
            }
        }

        m_bEnable = GetEnable();
    }
}

CDesignRegisterUtils::~CDesignRegisterUtils()
{

}

void CDesignRegisterUtils::Enable(bool bEnable/* = false*/)
{
    m_bEnable = bEnable;
    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_ENABLE,
        m_bEnable);
}

void CDesignRegisterUtils::Free(bool bFree/* = false*/)
{
    m_bFree = bFree;
}

bool CDesignRegisterUtils::IsEnable() const
{
#ifdef DESIGN_MODE
    return true;
#endif

    if (m_bFree)
    {
        return true;
    }

    return m_bEnable;
}

bool CDesignRegisterUtils::IsFree() const
{
    return m_bFree;
}

bool CDesignRegisterUtils::IsRegistered()
{
#ifdef DESIGN_MODE
    return true;
#endif

    return m_Registered;
}

Fcad_Type CDesignRegisterUtils::GetType()
{
#ifdef DESIGN_MODE
    return Fcad_Type::FDT_Full;
#endif

    if (m_bFree)
    {
        return Fcad_Type::FDT_Full;
    }

    return m_dwType;
}

bool CDesignRegisterUtils::Register(_tstring strUserName, _tstring strProductKey)
{
    _tstring strCrc32strSerialNumber = CCrc32Utils::GetCrc32Str((LPBYTE)m_strSerialNumber.c_str(), m_strSerialNumber.size(), TRUE);
    _tstring strCrc32UserName = CCrc32Utils::GetCrc32Str((LPBYTE)strUserName.c_str(), strUserName.size(), TRUE);
    bool bRegSuccess = false;

    Fcad_Type Types[] = {
            Fcad_Type::FDT_Full,
            Fcad_Type::FDT_Pro,
            Fcad_Type::FDT_Advanced,
            Fcad_Type::FDT_Standard,
            Fcad_Type::FDT_Basic
    };

    for (auto item : Types)
    {
        _tstring strCheckKey = GenerateProductKey(m_strSerialNumber, strUserName, item);
        if (strCheckKey == strProductKey)
        {
            m_dwType = item;
            if (!CRegUtils::SetString(
                CONFIG_REG_KEY,
                CONFIG_DESIGN_REGISTER_PATH,
                CONFIG_DESIGN_REGISTER_USER_NAME,
                strUserName))
            {
                break;
            }

            if (!CRegUtils::SetString(
                CONFIG_REG_KEY,
                CONFIG_DESIGN_REGISTER_PATH,
                CONFIG_DESIGN_REGISTER_PRODUCT_ID,
                strProductKey))
            {
                break;
            }

            m_Registered = true;
            bRegSuccess = true;
            break;
        }
    }

    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_UNREGISTER,
        !bRegSuccess);

    return bRegSuccess;
}

bool CDesignRegisterUtils::Unregister()
{
    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_UNREGISTER,
       true);

    m_Registered = false;
    m_dwType = Fcad_Type::FDT_None;
    return true;
}

_tstring CDesignRegisterUtils::GenerateProductKey(
    _tstring& SerialNumber,
    _tstring& strUserName,
    Fcad_Type dwType/* = 0*/
)
{
    DWORD dwCrcSerialNumber = CCrc32Utils::GetCrc32Value((LPBYTE)SerialNumber.c_str(), SerialNumber.size());
    DWORD dwCrcUserName = CCrc32Utils::GetCrc32Value((LPBYTE)strUserName.c_str(), strUserName.size());
    dwCrcSerialNumber |= 0x80000000;
    dwCrcUserName |= 0x04000000;

    dwCrcSerialNumber = CCrc32Utils::GetCrc32Value((LPBYTE)&dwCrcSerialNumber, sizeof(dwCrcSerialNumber));
    dwCrcUserName = CCrc32Utils::GetCrc32Value((LPBYTE)&dwCrcUserName, sizeof(dwCrcUserName));

    DWORD dwTemp = dwCrcSerialNumber * dwCrcUserName;

    DWORD dwKey0 = dwTemp * dwType;

    dwKey0 = CCrc32Utils::GetCrc32Value((LPBYTE)&dwKey0, sizeof(dwKey0));
    DWORD dwKey1 = CCrc32Utils::GetCrc32Value((LPBYTE)&dwKey0, sizeof(dwKey0)) * dwType;

    TCHAR szBuf[MAX_PATH] = { 0 };
    ::StringCchPrintf(szBuf, _countof(szBuf),  _T("%0.8X%0.8X"), dwKey0, dwKey1);

    return szBuf;
}

_tstring CDesignRegisterUtils::GetSerialNumber()
{
    _tstring strIDNumber;
    _tstring strUUID;
    _tstring strName;
    _tstring strVendor;
    _tstring strVersion;

    if (!m_strSerialNumber.empty())
    {
        return m_strSerialNumber;
    }

    CWmiUtils obj;
    obj.Initialize();

    obj.Query(_T("SELECT * FROM Win32_ComputerSystemProduct"),
        [&](IWbemClassObject* pclsObj) ->void {
            strIDNumber = CWmiUtils::GetVariantString(pclsObj, L"IdentifyingNumber");
            strName = CWmiUtils::GetVariantString(pclsObj, L"Name");
            strUUID = CWmiUtils::GetVariantString(pclsObj, L"UUID");
            strVendor = CWmiUtils::GetVariantString(pclsObj, L"Vendor");
            strVersion = CWmiUtils::GetVariantString(pclsObj, L"Version");
        });

    obj.Uninitialize();

    DWORD strCrc32ID = CCrc32Utils::GetCrc32Value((LPBYTE)strIDNumber.c_str(), strIDNumber.size());
    DWORD strCrc32UUID = CCrc32Utils::GetCrc32Value((LPBYTE)strUUID.c_str(), strUUID.size());
    DWORD strCrc32Version = CCrc32Utils::GetCrc32Value((LPBYTE)strVersion.c_str(), strVersion.size());

    strCrc32UUID = strCrc32ID * strCrc32UUID;
    strCrc32Version = strCrc32UUID * strCrc32Version;

    TCHAR szBuf[MAX_PATH] = { 0 };
    ::StringCchPrintf(szBuf, _countof(szBuf), _T("%0.8X%0.8X%0.8X"), strCrc32ID, strCrc32UUID, strCrc32Version);

    m_strSerialNumber = szBuf;

    return m_strSerialNumber;
}

_tstring CDesignRegisterUtils::GetProductKey()
{
    _tstring strKey;
    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_PRODUCT_ID,
        strKey);

    return strKey;
}

_tstring CDesignRegisterUtils::LoadString(UINT uID)
{
    static TCHAR szBuf[1024] = { 0 };
    ::LoadString(GetModuleHandle(NULL), uID, szBuf, _countof(szBuf));
    return szBuf;
}

_tstring CDesignRegisterUtils::GetProductName()
{
    for (const auto& item : g_FcaLimitForCoolBoy)
    {
        if (m_dwType != item.dwType)
        {
            continue;
        }

        return LoadString(item.dwData);
    }

    return _T("");
}

bool CDesignRegisterUtils::GetEnable() const
{
    DWORD dwEnable = TRUE;
    if (CRegUtils::GetDword(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_ENABLE,
        dwEnable))
    {
        return dwEnable;
    }

    dwEnable = TRUE;
    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_ENABLE,
        dwEnable);

    return dwEnable;
}

_tstring CDesignRegisterUtils::GetUser()
{
    _tstring strKey;
    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_DESIGN_REGISTER_PATH,
        CONFIG_DESIGN_REGISTER_USER_NAME,
        strKey);

    return strKey;
}