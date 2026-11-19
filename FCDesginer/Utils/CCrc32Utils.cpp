#include "CCrc32Utils.h"
#include <strsafe.h>

using namespace std;

// CRC分块
#define CRC32_BLOCK_SIZE    (1024 * 1024 * 64)

// CRC32标准
#define CRC32_POLY          (0xEDB88320L) 

// CRC查询表大小
#define CRC32_TABLE_SIZE    (256)

// CRC查询表
static UINT g_Crc32Table[CRC32_TABLE_SIZE] = { 0 };

// CRC查询表初始化标记
static bool g_bInitFlag = false;

_tstring CCrc32Utils::GetCrc32Str(LPVOID lpData, size_t nSize, bool bUpper/* = false*/)
{
    TCHAR szBuf[MAX_PATH] = { 0 };

    if (!g_bInitFlag)
    {
        InitCrc32Table();
        g_bInitFlag = true;
    }

    DWORD dwCrc32 = GetCrc32((LPBYTE)lpData, nSize);
    ::StringCchPrintf(szBuf, _countof(szBuf), bUpper ? _T("%0.8X") : _T("%0.8x"), dwCrc32);

    return szBuf;
}

DWORD CCrc32Utils::GetCrc32Value(LPVOID lpData, size_t nSize)
{
    if (!g_bInitFlag)
    {
        InitCrc32Table();
        g_bInitFlag = true;
    }

    return GetCrc32((LPBYTE)lpData, nSize);
}

void CCrc32Utils::InitCrc32Table(void)
{
    for (UINT i = 0; i < _countof(g_Crc32Table); i++)
    {
        UINT crc = i;
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x00000001L)
                crc = (crc >> 1) ^ CRC32_POLY;
            else
                crc = crc >> 1;
        }
        g_Crc32Table[i] = crc;
    }
}

DWORD CCrc32Utils::GetCrc32(LPVOID lpData, size_t byteSize)
{
    DWORD dwCrc32 = 0;
    size_t nBlockSize = CRC32_BLOCK_SIZE;
    LPBYTE lpBase = (LPBYTE)lpData;

    nBlockSize = byteSize > CRC32_BLOCK_SIZE ? CRC32_BLOCK_SIZE : byteSize;

    while (byteSize > 0)
    {
        dwCrc32 = GetCrc32Data(dwCrc32, lpBase, nBlockSize);
        lpBase += nBlockSize;
        byteSize -= nBlockSize;

        nBlockSize = byteSize > CRC32_BLOCK_SIZE ? CRC32_BLOCK_SIZE : byteSize;
    }

    return dwCrc32;
}

DWORD CCrc32Utils::GetCrc32Data(DWORD crcInit, LPBYTE lpData, size_t byteSize)
{
    UINT crc = crcInit ^ 0xffffffff;

    while (byteSize--)
    {
        crc = (crc >> 8) ^ g_Crc32Table[(crc & 0xff) ^ *lpData++];
    }

    return crc ^ 0xffffffff;
}