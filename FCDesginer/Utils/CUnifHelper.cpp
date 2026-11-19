#include "CUnifHelper.h"
#include <strsafe.h>

CUnifHelper::CUnifHelper()
{
}

CUnifHelper::~CUnifHelper()
{
    for (auto item : m_MetaList)
    {
        delete[] item.second;
    }

    m_MetaList.clear();
}

bool CUnifHelper::SetPRG(BYTE nIndex, LPBYTE lpData, DWORD dwSize)
{
    if (nIndex > 0x0F || nullptr == lpData || 0 == dwSize)
    {
        return false;
    }

    CHAR szBuf[MAX_PATH] = { 0 };
    ::StringCchPrintfA(szBuf, sizeof(szBuf), "PRG%X", nIndex);

    return SetLargeData(szBuf, lpData, dwSize);
}

bool CUnifHelper::SetCHR(BYTE nIndex, LPBYTE lpData, DWORD dwSize)
{
    if (nIndex > 0x0F || nullptr == lpData || 0 == dwSize)
    {
        return false;
    }

    CHAR szBuf[MAX_PATH] = { 0 };
    ::StringCchPrintfA(szBuf, sizeof(szBuf), "CHR%X", nIndex);

    return SetLargeData(szBuf, lpData, dwSize);
}

bool CUnifHelper::SetMirroring(BYTE bValue)
{
    return SetByteData("MIRR", bValue);
}

bool CUnifHelper::SetBatteryBacked(BYTE bValue)
{
    return SetByteData("BATR", bValue);
}

bool CUnifHelper::SetTVStandard(BYTE bValue)
{
    return SetByteData("TVCI", bValue);
}

bool CUnifHelper::SetBoardName(const std::string& strName)
{
    PUNIF_META pUnifMeta = nullptr;
    size_t dwMetaSize = strName.size() + 1;
    size_t dwDataSize = sizeof(UNIF_META) + dwMetaSize;
    std::string strIdName = "MAPR";

    RemoveMeta(strIdName);

    pUnifMeta = (PUNIF_META)new (std::nothrow) BYTE[dwDataSize];
    if (nullptr == pUnifMeta)
    {
        return false;
    }

    ::ZeroMemory(pUnifMeta, dwDataSize);
    pUnifMeta->m_Length = (DWORD)dwMetaSize;
    ::memcpy_s(pUnifMeta->m_Type, sizeof(UNIF_META::m_Type), strIdName.c_str(), 4);
    ::memcpy_s(pUnifMeta->m_Data, dwMetaSize, strName.c_str(), strName.size());

    m_MetaList.insert(std::make_pair(strIdName.c_str(), pUnifMeta));
    return true;
}

bool CUnifHelper::SetDumpingInfo(const std::string& strDumper, BYTE bDay, BYTE bMonth, BYTE bYear, const std::string& strSoftware)
{
    PUNIF_META pUnifMeta = nullptr;
    DWORD dwMetaSize = 204;
    DWORD dwDataSize = sizeof(UNIF_META) + dwMetaSize;
    std::string strIdName = "DINF";

    RemoveMeta(strIdName);

    pUnifMeta = (PUNIF_META)new (std::nothrow) BYTE[dwDataSize];
    if (nullptr == pUnifMeta)
    {
        return false;
    }

    ::ZeroMemory(pUnifMeta, dwDataSize);
    pUnifMeta->m_Length = dwMetaSize;
    ::memcpy_s(pUnifMeta->m_Type, sizeof(UNIF_META::m_Type), strIdName.c_str(), 4);
    ::memcpy_s(&pUnifMeta->m_Data[0], 100, strDumper.c_str(), strDumper.size());
    ::memcpy_s(&pUnifMeta->m_Data[100], 1, &bDay, 1);
    ::memcpy_s(&pUnifMeta->m_Data[101], 1, &bMonth, 1);
    ::memcpy_s(&pUnifMeta->m_Data[102], 2, &bYear, 2);
    ::memcpy_s(&pUnifMeta->m_Data[104], 100, strSoftware.c_str(), strSoftware.size());

    m_MetaList.insert(std::make_pair(strIdName.c_str(), pUnifMeta));
    return true;
}

bool CUnifHelper::SetByteData(const std::string& strId, BYTE bValue)
{
    PUNIF_META pUnifMeta = nullptr;
    DWORD dwMetaSize = 1;
    DWORD dwDataSize = sizeof(UNIF_META) + dwMetaSize;

    RemoveMeta(strId);

    pUnifMeta = (PUNIF_META)new (std::nothrow) BYTE[dwDataSize];
    if (nullptr == pUnifMeta)
    {
        return false;
    }

    ::ZeroMemory(pUnifMeta, dwDataSize);
    pUnifMeta->m_Length = dwMetaSize;
    ::memcpy_s(pUnifMeta->m_Type, sizeof(UNIF_META::m_Type), strId.c_str(), 4);
    ::memcpy_s(pUnifMeta->m_Data, dwMetaSize, &bValue, sizeof(bValue));

    m_MetaList.insert(std::make_pair(strId.c_str(), pUnifMeta));
    return true;
}

bool CUnifHelper::SetLargeData(const std::string& strId, LPBYTE lpData, DWORD dwSize)
{
    PUNIF_META pUnifMeta = nullptr;
    DWORD dwMetaSize = dwSize;
    DWORD dwDataSize = sizeof(UNIF_META) + dwMetaSize;

    RemoveMeta(strId);

    pUnifMeta = (PUNIF_META)new (std::nothrow) BYTE[dwDataSize];
    if (nullptr == pUnifMeta)
    {
        return false;
    }

    ::ZeroMemory(pUnifMeta, dwDataSize);
    pUnifMeta->m_Length = dwMetaSize;
    ::memcpy_s(pUnifMeta->m_Type, sizeof(UNIF_META::m_Type), strId.c_str(), 4);
    ::memcpy_s(pUnifMeta->m_Data, dwMetaSize, lpData, dwSize);

    m_MetaList.insert(std::make_pair(strId.c_str(), pUnifMeta));
    return true;
}

bool CUnifHelper::RemoveMeta(const std::string strMeta)
{
    auto ifFind = m_MetaList.find(strMeta);
    if (ifFind == m_MetaList.end())
    {
        return false;
    }

    if (ifFind->second)
    {
        delete[] ifFind->second;
        ifFind->second = nullptr;
    }

    m_MetaList.erase(ifFind);

    return true;
}

bool CUnifHelper::Dump(LPCBYTE *lpDataBuf, DWORD& dwSize)
{
    if (nullptr == lpDataBuf)
    {
        return false;
    }

    //文件头标识大小
    DWORD dwDataSize = sizeof(UNIF_HEADER);

    //累加元素大小
    for (const auto& item : m_MetaList)
    {
        dwDataSize += sizeof(UNIF_META) + item.second->m_Length;
    }

    LPBYTE lpData = new (std::nothrow) BYTE[dwDataSize];
    if (nullptr == lpData)
    {
        return false;
    }

    ::ZeroMemory(lpData, dwDataSize);
    LPBYTE lpDataPos = lpData;

    {
        const char szIdName[] = "UNIF";
        PUNIF_HEADER pUnifHeader = (PUNIF_HEADER)lpDataPos;
        ::memcpy_s(pUnifHeader->m_Flag, sizeof(UNIF_META::m_Type), szIdName, 4);
        pUnifHeader->m_dwVer = 5;
        lpDataPos += sizeof(UNIF_HEADER);
    }

    for (const auto& item : m_MetaList)
    {
        PUNIF_META pMetaData = (PUNIF_META)lpDataPos;
        DWORD dwMetaSize = sizeof(UNIF_META) + item.second->m_Length;
        ::memcpy_s(pMetaData, dwMetaSize, item.second, dwMetaSize);
        lpDataPos += sizeof(UNIF_META) + item.second->m_Length;
    }

    *lpDataBuf = lpData;
    dwSize = dwDataSize;

    return true;
}

