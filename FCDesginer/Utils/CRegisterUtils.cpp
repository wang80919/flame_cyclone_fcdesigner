#include "CRegisterUtils.h"
#include "Win32Utils/CStrUtils.h"
#include "Win32Utils/CRegUtils.h"
#include "Utils/CCrc32Utils.h"
#include "Win32Utils/CWmiUtils.h"

CRegisterUtils CRegisterUtils::m_Instance;

CRegisterUtils::CRegisterUtils()
{
    if (!m_bChecked)
    {
        m_bChecked = true;
        _tstring strProductKey = GetProductKey();
        _tstring strSerialNumber = GetSerialNumber();
        _tstring strUserName = GetUser();

        Fca_Type Types[] = {
            Fca_Type::FT_Full,
            Fca_Type::FT_Pro,
            Fca_Type::FT_Advanced,
            Fca_Type::FT_Standard,
            Fca_Type::FT_Basic
        };

        DWORD dwUnregister = 0;
        CRegUtils::GetDword(
            CONFIG_REG_KEY,
            CONFIG_REGISTER_PATH,
            CONFIG_REGISTER_UNREGISTER,
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

CRegisterUtils::~CRegisterUtils()
{

}

void CRegisterUtils::Enable(bool bEnable/* = false*/)
{
    m_bEnable = bEnable;
    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_ENABLE,
        m_bEnable);
}

void CRegisterUtils::Free(bool bFree/* = false*/)
{
    m_bFree = bFree;
}

bool CRegisterUtils::IsEnable() const
{
    if (m_bFree)
    {
        return true;
    }

    return m_bEnable;
}

bool CRegisterUtils::IsFree() const
{
    return m_bFree;
}

bool CRegisterUtils::IsRegistered()
{
    return m_Registered;
}

Fca_Type CRegisterUtils::GetType()
{
    if (m_bFree)
    {
        return Fca_Type::FT_Full;
    }

    return m_dwType;
}

bool CRegisterUtils::Register(_tstring strUserName, _tstring strProductKey)
{
    _tstring strCrc32strSerialNumber = CCrc32Utils::GetCrc32Str((LPBYTE)m_strSerialNumber.c_str(), m_strSerialNumber.size(), TRUE);
    _tstring strCrc32UserName = CCrc32Utils::GetCrc32Str((LPBYTE)strUserName.c_str(), strUserName.size(), TRUE);
    bool bRegSuccess = false;

    Fca_Type Types[] = {
            Fca_Type::FT_Full,
            Fca_Type::FT_Pro,
            Fca_Type::FT_Advanced,
            Fca_Type::FT_Standard,
            Fca_Type::FT_Basic
    };

    for (auto item : Types)
    {
        _tstring strCheckKey = GenerateProductKey(m_strSerialNumber, strUserName, item);
        if (strCheckKey == strProductKey)
        {
            m_dwType = item;
            if (!CRegUtils::SetString(
                CONFIG_REG_KEY,
                CONFIG_REGISTER_PATH,
                CONFIG_REGISTER_USER_NAME,
                strUserName))
            {
                break;
            }

            if (!CRegUtils::SetString(
                CONFIG_REG_KEY,
                CONFIG_REGISTER_PATH,
                CONFIG_REGISTER_PRODUCT_ID,
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
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_UNREGISTER,
        !bRegSuccess);

    return bRegSuccess;
}

bool CRegisterUtils::Unregister()
{
    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_UNREGISTER,
       true);

    m_Registered = false;
    m_dwType = Fca_Type::FT_None;
    return true;
}

_tstring CRegisterUtils::GenerateProductKey(
    _tstring& SerialNumber,
    _tstring& strUserName,
    Fca_Type dwType/* = 0*/
)
{
    _tstring strCrc32strSerialNumber = strUserName + CCrc32Utils::GetCrc32Str((LPBYTE)SerialNumber.c_str(), SerialNumber.size(), TRUE);
    _tstring strCrc32UserName = SerialNumber + CCrc32Utils::GetCrc32Str((LPBYTE)strUserName.c_str(), strUserName.size(), TRUE);

    if (Fca_Type::FT_Full != dwType)
    {
        for (size_t i = 0; i < strCrc32strSerialNumber.size(); i++)
        {
            strCrc32strSerialNumber[i] ^= dwType;
        }
        for (size_t i = 0; i < strCrc32UserName.size(); i++)
        {
            strCrc32UserName[i] ^= dwType;
        }
    }

    _tstring strProductKey1 =
        CCrc32Utils::GetCrc32Str(
            (LPVOID)strCrc32strSerialNumber.c_str(),
            strCrc32strSerialNumber.size(),
            TRUE);

    _tstring strProductKey2 =
        CCrc32Utils::GetCrc32Str(
            (LPVOID)strCrc32UserName.c_str(),
            strCrc32UserName.size(),
            TRUE);
    for (size_t i = 0; i < strProductKey1.size(); i++)
    {
        strProductKey1[i] += strProductKey2[i];
    }

    for (size_t i = 0; i < strProductKey2.size(); i++)
    {
        strProductKey2[i] -= strProductKey1[i];
    }

    _tstring strProductKey = CCrc32Utils::GetCrc32Str(
        (LPVOID)strProductKey1.c_str(),
        strProductKey1.size(),
        TRUE) + CCrc32Utils::GetCrc32Str(
            (LPVOID)strProductKey2.c_str(),
            strProductKey2.size(),
            TRUE);


    return strProductKey;
}

_tstring CRegisterUtils::GetSerialNumber()
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

    _tstring strCrc32ID = CCrc32Utils::GetCrc32Str((LPBYTE)strIDNumber.c_str(), strIDNumber.size(), TRUE);
    _tstring strCrc32UUID = CCrc32Utils::GetCrc32Str((LPBYTE)strUUID.c_str(), strUUID.size(), TRUE);
    _tstring strCrc32Version = CCrc32Utils::GetCrc32Str((LPBYTE)strVersion.c_str(), strVersion.size(), TRUE);

    m_strSerialNumber =
        CCrc32Utils::GetCrc32Str((LPBYTE)strCrc32ID.c_str(), strCrc32ID.size(), TRUE) +
        CCrc32Utils::GetCrc32Str((LPBYTE)strCrc32UUID.c_str(), strCrc32UUID.size(), TRUE) +
        CCrc32Utils::GetCrc32Str((LPBYTE)strCrc32Version.c_str(), strCrc32Version.size(), TRUE);

    return m_strSerialNumber;
}

_tstring CRegisterUtils::GetProductKey()
{
    _tstring strKey;
    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_PRODUCT_ID,
        strKey);

    return strKey;
}

_tstring CRegisterUtils::LoadString(UINT uID)
{
    static TCHAR szBuf[1024] = { 0 };
    ::LoadString(GetModuleHandle(NULL), uID, szBuf, _countof(szBuf));
    return szBuf;
}

_tstring CRegisterUtils::GetProductName()
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

bool CRegisterUtils::GetEnable() const
{
    DWORD dwEnable = TRUE;
    if (CRegUtils::GetDword(
        CONFIG_REG_KEY,
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_ENABLE,
        dwEnable))
    {
        return dwEnable;
    }

    dwEnable = TRUE;
    CRegUtils::SetDword(
        CONFIG_REG_KEY,
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_ENABLE,
        dwEnable);

    return dwEnable;
}

_tstring CRegisterUtils::GetUser()
{
    _tstring strKey;
    CRegUtils::GetString(
        CONFIG_REG_KEY,
        CONFIG_REGISTER_PATH,
        CONFIG_REGISTER_USER_NAME,
        strKey);

    return strKey;
}