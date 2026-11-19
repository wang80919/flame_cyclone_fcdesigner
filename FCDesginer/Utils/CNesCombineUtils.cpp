#include "CNesCombineUtils.h"
#include "Win32Utils/CFileUtils.h"
#include "Win32Utils/CPathUtils.h"
#include <algorithm>
#include <strsafe.h>
#include <iostream>
#include <regex>
#include <map>
#include "Win32Utils/CStrUtils.h"
#include "CUnifHelper.h"
#include "../Common.h"

typedef struct _RESERVER_DATA {
    DWORD nBegin = 0;
    DWORD nSize = 0;
}RESERVER_DATA;

std::map<DWORD, COOLGIRL_CFG> g_CoolgirlCfg =
{
    std::make_pair(0,   COOLGIRL_CFG(  0,0,0x00,0x00,0x00,0x00)),
    std::make_pair(1,   COOLGIRL_CFG(  1,0,0x10,0x00,0x00,0x00)),
    std::make_pair(2,   COOLGIRL_CFG(  2,0,0x01,0x00,0x00,0x00)),
    std::make_pair(3,   COOLGIRL_CFG(  3,0,0x02,0x00,0x00,0x00)),
    std::make_pair(4,   COOLGIRL_CFG(  4,0,0x14,0x00,0x04,0x02)),
    std::make_pair(5,   COOLGIRL_CFG(  5,0,0x0F,0x00,0x04,0x07)),
    std::make_pair(7,   COOLGIRL_CFG(  7,0,0x08,0x00,0x07,0x00)),
    std::make_pair(9,   COOLGIRL_CFG(  9,0,0x11,0x00,0x04,0x05)),
    std::make_pair(10,  COOLGIRL_CFG( 10,0,0x11,0x01,0x00,0x05)),
    std::make_pair(11,  COOLGIRL_CFG( 11,0,0x0A,0x00,0x07,0x00)),
    std::make_pair(18,  COOLGIRL_CFG( 18,0,0x07,0x00,0x04,0x07)),
    std::make_pair(21,  COOLGIRL_CFG( 21,0,0x18,0x00,0x04,0x07)),
    std::make_pair(22,  COOLGIRL_CFG( 22,0,0x18,0x03,0x04,0x07)),
    std::make_pair(23,  COOLGIRL_CFG( 23,0,0x18,0x04,0x04,0x07)),
    std::make_pair(25,  COOLGIRL_CFG( 25,0,0x18,0x05,0x04,0x07)),
    std::make_pair(30,  COOLGIRL_CFG( 30,0,0x01,0x00,0x00,0x00)),
    std::make_pair(32,  COOLGIRL_CFG( 32,0,0x1A,0x00,0x04,0x07)),
    std::make_pair(33,  COOLGIRL_CFG( 33,0,0x16,0x00,0x04,0x02)),
    std::make_pair(34,  COOLGIRL_CFG( 34,0,0x08,0x01,0x07,0x00)),
    std::make_pair(36,  COOLGIRL_CFG( 36,0,0x1D,0x00,0x07,0x00)),
    std::make_pair(38,  COOLGIRL_CFG( 38,0,0x20,0x00,0x07,0x00)),
    std::make_pair(42,  COOLGIRL_CFG( 42,0,0x17,0x00,0x07,0x00)),
    std::make_pair(48,  COOLGIRL_CFG( 48,0,0x16,0x01,0x04,0x02)),
    std::make_pair(65,  COOLGIRL_CFG( 65,0,0x0E,0x00,0x04,0x07)),
    std::make_pair(66,  COOLGIRL_CFG( 66,0,0x0B,0x00,0x07,0x00)),
    std::make_pair(67,  COOLGIRL_CFG( 67,0,0x24,0x00,0x00,0x06)),
    std::make_pair(69,  COOLGIRL_CFG( 69,0,0x19,0x00,0x04,0x07)),
    std::make_pair(70,  COOLGIRL_CFG( 70,0,0x12,0x00,0x00,0x00)),
    std::make_pair(71,  COOLGIRL_CFG( 71,0,0x01,0x01,0x00,0x00)),
    std::make_pair(73,  COOLGIRL_CFG( 73,0,0x13,0x00,0x00,0x00)),
    std::make_pair(75,  COOLGIRL_CFG( 75,0,0x22,0x00,0x04,0x04)),
    std::make_pair(78,  COOLGIRL_CFG( 78,0,0x03,0x00,0x00,0x00)),
    std::make_pair(79,  COOLGIRL_CFG( 79,0,0x1B,0x00,0x07,0x00)),
    std::make_pair(83,  COOLGIRL_CFG( 83,0,0x23,0x00,0x04,0x07)),
    std::make_pair(87,  COOLGIRL_CFG( 87,0,0x0C,0x00,0x00,0x00)),
    std::make_pair(89,  COOLGIRL_CFG( 89,0,0x25,0x00,0x00,0x00)),
    std::make_pair(90,  COOLGIRL_CFG( 90,0,0x0D,0x00,0x04,0x07)),
    std::make_pair(93,  COOLGIRL_CFG( 93,0,0x05,0x00,0x00,0x00)),
    std::make_pair(97,  COOLGIRL_CFG( 97,0,0x04,0x00,0x01,0x00)),
    std::make_pair(112, COOLGIRL_CFG(112,0,0x15,0x00,0x04,0x02)),
    std::make_pair(118, COOLGIRL_CFG(118,0,0x14,0x01,0x04,0x02)),
    std::make_pair(133, COOLGIRL_CFG(133,0,0x1C,0x00,0x07,0x00)),
    std::make_pair(146, COOLGIRL_CFG(146,0,0x1B,0x00,0x07,0x00)),
    std::make_pair(152, COOLGIRL_CFG(152,0,0x12,0x00,0x00,0x00)),
    std::make_pair(163, COOLGIRL_CFG(163,0,0x06,0x00,0x07,0x00)),
    std::make_pair(184, COOLGIRL_CFG(184,0,0x1F,0x00,0x00,0x04)),
    std::make_pair(189, COOLGIRL_CFG(189,0,0x14,0x02,0x07,0x02)),
    std::make_pair(206, COOLGIRL_CFG(206,0,0x14,0x04,0x04,0x02)),
    std::make_pair(228, COOLGIRL_CFG(228,0,0x09,0x00,0x07,0x00)),
    std::make_pair(241, COOLGIRL_CFG(241,0,0x08,0x01,0x07,0x00)),
};

void SetPrgPosByPrg32(std::vector<CNesFileInfo>& infos, DWORD dwCrc32, DWORD dwPos)
{
    for (auto& item : infos)
    {
        if (dwCrc32 == item.m_nPrgCrc32)
        {
            item.m_nPrgPos = dwPos;
            item.m_bPrgSet = true;
        }
    }
}

void SetChrPosByCrc32(std::vector<CNesFileInfo>& infos, DWORD dwCrc32, DWORD dwPos)
{
    for (auto& item : infos)
    {
        if (dwCrc32 == item.m_nChrCrc32)
        {
            item.m_nChrPos = dwPos;
            item.m_bChrSet = true;
        }
    }
}

NES_COMBINE_INFO 
CNesCombineUtils::AdjustRomData(
    std::vector<CNesFileInfo> infoList,
    CNesFileInfo infoMenu,
    DWORD dwType/* = 0*/
)
{
    NES_COMBINE_INFO nesCombine;
    std::vector< CNesFileInfo> infoTemp;

    infoMenu.m_bMenu = true;

    // 从大到小排序
    std::sort(infoList.begin(), infoList.end(), [](const CNesFileInfo& a, const CNesFileInfo& b) {
        return a.m_nDestPrgCount > b.m_nDestPrgCount;
        });

    if (infoMenu.m_nDestPrgCount <= 0)
    {
        return nesCombine;
    }

    //菜单必须放在 0x80000 前
    //菜单如果是512KB, 则位置存放 必须放在 0x80000 前
    std::vector<RESERVER_DATA> ReservedList;

    DWORD dwMenuEndPos = 32;
    if (MULTICATR_TYPE_COOLGIRL == dwType)
    {
        dwMenuEndPos = infoMenu.m_nDestPrgCount;
    }

    if (infoMenu.m_nDestPrgCount > 32)
    {
        dwMenuEndPos = infoMenu.m_nDestPrgCount;
    }

    DWORD nMenuBegin = 0;
    infoMenu.m_nPrgPos = (dwMenuEndPos - infoMenu.m_nDestPrgCount) * NES_PRG_BANK_SIZE;
    infoMenu.m_bPrgSet = true;

    infoTemp = infoList;

    if (infoMenu.m_nDestPrgCount >= 32)
    {
        infoTemp.push_back(infoMenu);
        nMenuBegin = 0;
    }
    else
    {
        RESERVER_DATA data;
        data.nBegin = 0;
        data.nSize = infoMenu.m_nPrgPos;
        ReservedList.push_back(data);
        infoTemp.push_back(infoMenu);
    }

    // 使用PRG填充菜单前的空间
    for (auto& item : ReservedList)
    {
        if (item.nSize <= 0)
        {
            continue;
        }

        for (auto& prg : infoTemp)
        {
            if (prg.m_bPrgSet)
            {
                continue;
            }

            if (0 == prg.m_nDestPrgSize)
            {
                prg.m_bPrgSet = true;
                continue;
            }

            if (prg.m_nDestPrgSize > item.nSize)
            {
                continue;
            }

            if (0 == item.nBegin % prg.m_nDestPrgSize)
            {
                prg.m_nPrgPos = item.nBegin;
                prg.m_bPrgSet = true;
                SetPrgPosByPrg32(infoTemp, prg.m_nPrgCrc32, prg.m_nPrgPos);
                item.nBegin += prg.m_nDestPrgSize;
                item.nSize -= prg.m_nDestPrgSize;
            }
        }
    }

    // 使用CHR填充菜单前的空间
    for (auto& item : ReservedList)
    {
        if (item.nSize <= 0)
        {
            continue;
        }

        for (auto& chr : infoTemp)
        {
            if (chr.m_bChrSet)
            {
                continue;
            }

            if (0 == chr.m_nDestChrSize)
            {
                chr.m_bChrSet = true;
                continue;
            }

            if (chr.m_nDestChrSize > item.nSize)
            {
                continue;
            }

            if (0 == item.nBegin % chr.m_nDestChrSize)
            {
                chr.m_nChrPos = item.nBegin;
                chr.m_bChrSet = true;
                SetChrPosByCrc32(infoTemp, chr.m_nChrCrc32, chr.m_nChrPos);
                item.nBegin += chr.m_nDestChrSize;
                item.nSize -= chr.m_nDestChrSize;
            }
        }
    }

    ReservedList.clear();

    // PRG从大到小排序
    std::sort(infoTemp.begin(), infoTemp.end(), [](const CNesFileInfo& a, const CNesFileInfo& b) {
        return a.m_nDestPrgCount > b.m_nDestPrgCount;
        });

    //对齐
    int nPrgOffset = infoMenu.m_nPrgPos + infoMenu.m_nDestPrgSize;
    for (size_t i = 0; i < infoTemp.size(); i++)
    {
        // 菜单前填充
        if (infoTemp[i].m_bPrgSet)
        {
            continue;
        }

        if (0 != nPrgOffset % infoTemp[i].m_nDestPrgSize)
        {
            //将PRG放到当前位置对齐保证
            bool bAdjusted = false;
            for (size_t j = i + 1; j < infoTemp.size(); j++)
            {
                std::swap(infoTemp[i], infoTemp[j]);

                if (infoTemp[i].m_bPrgSet || 0 == infoTemp[i].m_nDestPrgSize)
                {
                    continue;
                }

                if (0 == nPrgOffset % infoTemp[i].m_nDestPrgSize)
                {
                    bAdjusted = true;
                    break;
                }
            }

            // 实在找不到对齐项则填充一下, 记录对齐时未使用的空间
            if (!bAdjusted && 0 != infoTemp[i].m_nDestPrgSize)
            {
                RESERVER_DATA data;
                data.nBegin = nPrgOffset;
                while (0 != nPrgOffset % infoTemp[i].m_nDestPrgSize)
                {
                    nPrgOffset += NES_PRG_BANK_SIZE;
                }

                data.nSize = nPrgOffset - data.nBegin;
                if (data.nSize > 0)
                {
                    ReservedList.push_back(data);
                }
            }
        }

        if (infoTemp[i].m_bPrgSet)
        {
            continue;
        }

        SetPrgPosByPrg32(infoTemp, infoTemp[i].m_nPrgCrc32, nPrgOffset);
        infoTemp[i].m_bPrgSet = true;
        infoTemp[i].m_nPrgPos = nPrgOffset;
        nPrgOffset += infoTemp[i].m_nDestPrgSize;
    }

    // CHR从大到小排序
    std::sort(infoTemp.begin(), infoTemp.end(), [](const CNesFileInfo& a, const CNesFileInfo& b) {
        return a.m_nDestChrCount > b.m_nDestChrCount;
        });

    // 使用CHR填充PRG对齐时未使用的空间
    for (auto& item : ReservedList)
    {
        if (item.nSize <= 0)
        {
            continue;
        }

        for (auto& chr : infoTemp)
        {
            if (chr.m_bChrSet)
            {
                continue;
            }

            if (0 == chr.m_nDestChrSize)
            {
                chr.m_bChrSet = true;
                continue;
            }

            if (chr.m_nDestChrSize > item.nSize)
            {
                continue;
            }

            if (0 == item.nBegin % chr.m_nDestChrSize)
            {
                chr.m_nChrPos = item.nBegin;
                chr.m_bChrSet = true;
                SetChrPosByCrc32(infoTemp, chr.m_nChrCrc32, chr.m_nChrPos);
                item.nBegin += chr.m_nDestChrSize;
                item.nSize -= chr.m_nDestChrSize;
            }
        }
    }

    int nChrOffset = nPrgOffset;

    for (size_t i = 0; i < infoTemp.size(); i++)
    {
        if (infoTemp[i].m_bChrSet || 0 == infoTemp[i].m_nDestChrSize)
        {
            infoTemp[i].m_bChrSet = true;
            continue;
        }

        if (0 != (nChrOffset % infoTemp[i].m_nDestChrSize))
        {
            bool bAdjusted = false;
            for (size_t j = i + 1; j < infoTemp.size(); j++)
            {
                std::swap(infoTemp[i], infoTemp[j]);

                if (infoTemp[i].m_bChrSet || 0 == infoTemp[i].m_nDestChrSize)
                {
                    infoTemp[i].m_bChrSet = true;
                    continue;
                }

                if (0 == nChrOffset % infoTemp[i].m_nDestChrSize)
                {
                    bAdjusted = true;
                    break;
                }
            }

            //实在找不到对齐项则填充一下直到对齐
            if (!bAdjusted && 0 != infoTemp[i].m_nDestChrSize)
            {
                while (0 != nChrOffset % infoTemp[i].m_nDestChrSize)
                {
                    nChrOffset += NES_CHR_BANK_SIZE;
                }
            }
        }

        if (infoTemp[i].m_bChrSet)
        {
            continue;
        }

        SetChrPosByCrc32(infoTemp, infoTemp[i].m_nChrCrc32, nChrOffset);
        infoTemp[i].m_bChrSet = true;
        infoTemp[i].m_nChrPos = nChrOffset;
        nChrOffset += infoTemp[i].m_nDestChrSize;
    }

    std::swap(nesCombine.m_FileInfo, infoTemp);

    std::map<DWORD, DWORD> mapPrgSize;
    for (const auto& item : nesCombine.m_FileInfo)
    {
        mapPrgSize.insert(std::make_pair(item.m_nPrgCrc32, item.m_nDestPrgSize));
    }

    std::map<DWORD, DWORD> mapChrSize;
    for (const auto& item : nesCombine.m_FileInfo)
    {
        mapChrSize.insert(std::make_pair(item.m_nChrCrc32, item.m_nDestChrSize));
    }

    for (const auto& item : mapPrgSize)
    {
        nesCombine.m_dwPrgTotalSize += item.second;
    }

    for (const auto& item : mapChrSize)
    {
        nesCombine.m_dwChrTotalSize += item.second;
    }

    for (const auto& item : nesCombine.m_FileInfo)
    {
        if (item.m_nPrgPos >= nesCombine.m_dwBinFileSize)
        {
            nesCombine.m_dwBinFileSize = item.m_nPrgPos + item.m_nDestPrgSize;
        }
        if (item.m_nChrPos >= nesCombine.m_dwBinFileSize)
        {
            nesCombine.m_dwBinFileSize = item.m_nChrPos + item.m_nDestChrSize;
        }
    }

    nesCombine.m_dwRomTotalSize = nesCombine.m_dwPrgTotalSize + nesCombine.m_dwChrTotalSize;

    while (0 != nesCombine.m_dwBinFileSize % NES_PRG_BANK_SIZE)
    {
        nesCombine.m_dwBinFileSize += NES_CHR_BANK_SIZE;
    }

    // 按指定序号排序
    std::sort(nesCombine.m_FileInfo.begin(), nesCombine.m_FileInfo.end(), [](const CNesFileInfo& a, const CNesFileInfo& b) {
        return a.m_nRomIndex < b.m_nRomIndex;
        });

    return nesCombine;
}

bool CNesCombineUtils::CombineRomData(
    const NES_COMBINE_INFO& info,
    const _tstring& strPath,
    bool bUnifHeader/* = true*/,
    const std::string strBoardName/* = "COOLBOY"*/
)
{
    LONGLONG llEndPod = 0;
    LPBYTE lpData = nullptr;
    DWORD dwSize = 0;

    CFileHelper cFileOut;
    if (!cFileOut.Open(strPath, true))
    {
        return false;
    }

    cFileOut.Clear();

    if (!Dump(info, &lpData, dwSize))
    {
        return false;
    }

    if (bUnifHeader)
    {
        CUnifHelper cUnfHelper;
        cUnfHelper.SetBoardName(strBoardName);
        cUnfHelper.SetMirroring(5);
        cUnfHelper.SetPRG(0, lpData, dwSize);

        LPCBYTE lpUnifData = nullptr;
        DWORD dwDumpSize = 0;
        if (cUnfHelper.Dump(&lpUnifData, dwDumpSize))
        {
            cFileOut.Write(lpUnifData, dwDumpSize, 0, nullptr, FILE_BEGIN);
            delete[] lpUnifData;
            llEndPod = dwDumpSize;
        }
    }
    else
    {
        cFileOut.Write(lpData, dwSize, 0, nullptr, FILE_BEGIN);
        llEndPod = info.m_dwBinFileSize;
    }

    cFileOut.SetEnd(llEndPod);
    cFileOut.Close();

    delete[] lpData;

    return true;
}

bool CNesCombineUtils::CombineNesData(
    const NES_COMBINE_INFO& info,
    const _tstring& strPath,
    DWORD dwMapper/* = 268*/,
    DWORD dwSubmapper/* = 0*/
)
{
    NES_HEADER_2 nes2Header;

    nes2Header.Nes2Identifier = 2;
    nes2Header.MapperH = dwMapper >> 8 & 0x0F;
    nes2Header.MapperM = dwMapper >> 4 & 0x0F;
    nes2Header.MapperL = dwMapper & 0x0F;
    nes2Header.ChrRomSizeM = 0x00;
    nes2Header.ChrRomSizeL = 0x00;
    nes2Header.ChrRamSize = 0x0C;
    DWORD dwRomSize = 0;

    if (info.m_dwBinFileSize <= 62898176)
    {
        // ++++---------- - Header byte 9 D0..D3
        //     |||| ++++ - ++++ - Header byte 4
        //     D~BA98 7654 3210
        //     --------------
        //     BBBB BBBB BBBB
        //     ++++ - ++++ - ++++ - PRG - ROM size in 16 KiB units,
        //     values $000 - $EFF for 0..62, 898, 176 bytes

        DWORD dwBinSize = info.m_dwBinFileSize >> 14;
        nes2Header.PrgRomSizeM = dwBinSize >> 8 & 0x0F;
        nes2Header.PrgRomSizeL = dwBinSize & 0xFF;
        dwRomSize = info.m_dwBinFileSize;
    }
    else
    {
        // ++++---------- - Header byte 9 D0..D3
        //     |||| ++++ - ++++ - Header byte 4
        //     D~BA98 7654 3210
        //     --------------
        //     1111 EEEE EEMM
        //     |||| ||++ - Multiplier, actual value is MM * 2 + 1 (1, 3, 5, 7)
        //     ++++ - ++-- - Exponent(2 ^ E), 0 - 63
        //
        //     The actual PRG - ROM size is 2 ^ E * (MM * 2 + 1) bytes.
        DWORD dwExponent = 1;

        for (int i = 0; i < 64; i++)
        {
            dwRomSize = 1 << i;

            if (dwRomSize >= info.m_dwBinFileSize)
            {
                dwExponent = i;
                break;
            }
        }
        nes2Header.PrgRomSizeM = 0x0F;
        nes2Header.PrgRomSizeL = (BYTE)(dwExponent << 2);
    }

    if (268 == dwMapper)
    {
        nes2Header.PrgRamCount = 0x00;
        nes2Header.PrgNvRamCount = 0x07;
    }

    if (342 == dwMapper)
    {
        nes2Header.PrgRamCount = 0x00;
        nes2Header.PrgNvRamCount = 0x09;
        nes2Header.bBatteryBacked = true;
    }

    nes2Header.Submapper = dwSubmapper;

    LONGLONG llEndPod = 0;
    LPBYTE lpData = nullptr;
    DWORD dwSize = 0;

    CFileHelper cFileOut;
    if (!cFileOut.Open(strPath, true))
    {
        return false;
    }

    cFileOut.Clear();

    if (!Dump(info, &lpData, dwSize))
    {
        return false;
    }

    cFileOut.Write(&nes2Header, sizeof(NES_HEADER_2), 0, nullptr, FILE_BEGIN);

    {
        cFileOut.Write(lpData, dwSize, 0, nullptr, FILE_CURRENT);
        llEndPod = NES_HEAD_SIZE + dwRomSize;
    }

    cFileOut.SetEnd(llEndPod);
    cFileOut.Close();

    delete[] lpData;

    return true;
}

bool CNesCombineUtils::Dump(const NES_COMBINE_INFO& info, LPBYTE* lpDataOut, DWORD& dwSize)
{
    LPBYTE lpData = nullptr;
    LPBYTE lpDataPos = nullptr;

    if (nullptr == lpDataOut)
    {
        return false;
    }

    lpData = new (std::nothrow) BYTE[info.m_dwBinFileSize];
    if (nullptr == lpData)
    {
        return false;
    }

    // 合并PRG
    for (const auto& item : info.m_FileInfo)
    {
        CByteBuffer ByteBuf(item.m_nSrcPrgSize);
        CFileHelper cFileRead(item.m_strFilePath, true);
        if (!cFileRead.IsOpen())
        {
            return false;
        }

        LONGLONG llReadOffset = item.m_nHeadSize + item.m_nTrainerSize;
        LONGLONG llWritePos = item.m_nPrgPos;
        lpDataPos = lpData + llWritePos;
        cFileRead.Read(ByteBuf.GetBuffer(), (DWORD)ByteBuf.GetSize(), llReadOffset, nullptr, (DWORD)FILE_BEGIN);

        if (0 != item.m_nSrcPrgCount && 0 != item.m_nDestPrgCount)
        {
            int nCount = item.m_nDestPrgCount / item.m_nSrcPrgCount;
            for (int i = 0; i < nCount; i++)
            {
                memcpy_s(lpDataPos, ByteBuf.GetSize(), ByteBuf.GetBuffer(), ByteBuf.GetSize());
                lpDataPos += ByteBuf.GetSize();
            }
        }
    }

    // 合并CHR
    for (const auto& item : info.m_FileInfo)
    {
        CByteBuffer ByteBuf(item.m_nDestChrSize);
        CFileHelper cFileRead(item.m_strFilePath, true);
        if (!cFileRead.IsOpen())
        {
            return false;
        }

        LONGLONG llReadOffset = item.m_nHeadSize + item.m_nTrainerSize + item.m_nSrcPrgSize;
        LONGLONG llWritePos = item.m_nChrPos;
        lpDataPos = lpData + llWritePos;
        cFileRead.Read(ByteBuf.GetBuffer(), item.m_nSrcChrSize, llReadOffset, nullptr, FILE_BEGIN);
        memcpy_s(lpDataPos, ByteBuf.GetSize(), ByteBuf.GetBuffer(), item.m_nSrcChrSize);
        lpDataPos += ByteBuf.GetSize();
    }

    *lpDataOut = lpData;
    dwSize = info.m_dwBinFileSize;

    return true;
}

bool CNesCombineUtils::OutGameInfo(
    const NES_COMBINE_INFO& combineInfo,
    const _tstring& strOutDir,
    int& nOutBankCount,
    bool bOut/* = true*/,
    bool bFixCount/* = true*/
)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    std::vector<CNesFileInfo> infoList;

    DWORD dwTextSize = 0;
    DWORD dwBank = 0;
    DWORD dwIndex = 0;
    std::vector<DWORD> vDataBank;

    _tstring strBankSize = _T("game_data_bank_index\r\n");

    std::vector<CNesFileInfo> vInfo;

    for (const auto& item : combineInfo.m_FileInfo)
    {
        if (!item.m_bMenu)
        {
            vInfo.push_back(item);
        }
    }

    nOutBankCount = 0;

    const DWORD beginSize = 2;
    const DWORD indexSize = 2;
    const DWORD metaSize = 8;
    const DWORD itemSize = indexSize + metaSize;

    dwTextSize = beginSize;
    for (auto& item : vInfo)
    {
        dwTextSize += itemSize;
        if (dwTextSize <= 8192)
        {
            infoList.push_back(item);
            vDataBank.push_back(dwBank);
        }
        else
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_data_%X.asm"), dwBank);
            _tstring strOutFile = strOutDir + _T("\\") + szBuf;

            if (bOut)
            {
                OutGameDataInfo(infoList, dwIndex - infoList.size(), strOutFile);
            }

            infoList.clear();
            dwBank++;
            nOutBankCount++;
            dwTextSize = beginSize;
            dwTextSize += itemSize;

            infoList.push_back(item);
            vDataBank.push_back(dwBank);
        }
        dwIndex++;
    }

    if (!infoList.empty())
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_data_%X.asm"), dwBank);
        _tstring strOutFile = strOutDir + _T("\\") + szBuf;

        if (bOut)
        {
            OutGameDataInfo(infoList, dwIndex - infoList.size(), strOutFile);
        }

        infoList.clear();
        dwBank++;
        nOutBankCount++;
    }

    for (int i = dwBank; i < GAME_DATA_BANK_MAX; i++)
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_data_%X.asm"), dwBank);
        _tstring strOutFile = strOutDir + _T("\\") + szBuf;

        if (bFixCount)
        {
            if (bOut)
            {
                OutGameDataInfo(infoList, dwIndex - infoList.size(), strOutFile);
            }
        }
        else
        {
            CPathUtils::DeleteArchive(strOutFile);
        }

        infoList.clear();
        dwBank++;
    }

    {
        for (size_t i = 0; i < vDataBank.size(); i++)
        {
            if (0 == (i % 10))
            {
                strBankSize += _T(" .DB ");
            }

            ::StringCchPrintf(szBuf,
                _countof(szBuf),
                _T("$%02X"), vDataBank[i]);

            strBankSize += szBuf;

            if (0 != (i + 1) % 10 && (i != vDataBank.size() - 1))
            {
                strBankSize += _T(",");
            }
            else
            {
                strBankSize += _T("\r\n");
            }
        }

        CFileHelper cFile;
        _tstring strOutFile = _T("game_data_bank.asm");
        if (!strOutDir.empty())
        {
            strOutFile = strOutDir + _T("\\game_data_bank.asm");
        }

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        std::string strOut = CStrUtils::TStrToU8Str(strBankSize);
        cFile.Write(strOut.c_str(), (DWORD)strOut.size());
        cFile.Close();
    }

    //输出物理顺序
    {
        _tstring strOut;

        // 从小到大排序
        std::sort(vInfo.begin(), vInfo.end(), [](const CNesFileInfo& a, const CNesFileInfo& b) {
            return a.m_nPrgPos < b.m_nPrgPos;
            });

        strOut += _T("rom prg offset:\r\n");
        for (size_t i = 0; i < vInfo.size(); i++)
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T("%0.4d %0.8X %0.6X %s\r\n"), 
                vInfo[i].m_nRomIndex,
                vInfo[i].m_nPrgPos,
                vInfo[i].m_nDestPrgSize,
                vInfo[i].m_strFileName.c_str()
                );

            strOut += szBuf;
        }

        // 从小到大排序
        std::sort(vInfo.begin(), vInfo.end(), [](const CNesFileInfo& a, const CNesFileInfo& b) {
            return a.m_nChrPos < b.m_nChrPos;
            });

        strOut += _T("\r\n");

        strOut += _T("rom chr offset:\r\n");
        for (size_t i = 0; i < vInfo.size(); i++)
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T("%0.4d %0.8X %0.6X %s\r\n"),
                vInfo[i].m_nRomIndex,
                vInfo[i].m_nChrPos,
                vInfo[i].m_nDestChrSize,
                vInfo[i].m_strFileName.c_str()
            );

            strOut += szBuf;
        }

        CFileHelper cFile;
        _tstring strOutFile = _T("game_prg_chr_offset.txt");
        if (!strOutDir.empty())
        {
            strOutFile = strOutDir + _T("\\game_prg_chr_offset.txt");
        }

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        std::string strAnsi = CStrUtils::TStrToAStr(strOut);
        cFile.Clear();
        cFile.Write(strAnsi.c_str(), (DWORD)strAnsi.size());
        cFile.Close();
    }

    return true;
}

bool CNesCombineUtils::OutGameDataInfo(
    const std::vector<CNesFileInfo>& gemsInfos,
    size_t nBeginIndex,
    const _tstring& strOutFile
)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _tstring strGameInfoOut;

    if (!gemsInfos.empty())
    {
        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T("game_data_begin_%0.3d\r\n .WORD %d\r\ngame_data_list_%0.3d\r\n"),
            nBeginIndex, nBeginIndex, nBeginIndex
        );
        strGameInfoOut += szBuf;
    }

    for (size_t i = 0; i < gemsInfos.size(); i++)
    {
        if (0 == (i % 10))
        {
            strGameInfoOut += _T(" .WORD ");
        }

        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T("game_data_%0.3d"),
            nBeginIndex + i);

        strGameInfoOut += szBuf;
        if (0 != (i + 1) % 10 && (i != gemsInfos.size() - 1))
        {
            strGameInfoOut += _T(",");
        }
        else
        {
            strGameInfoOut += _T("\r\n");
        }
    }
    if (!strGameInfoOut.empty())
    {
        strGameInfoOut += _T("\r\n");
    }

    int nIndex = 0;
    TCHAR szBinary[MAX_PATH] = { 0 };
    for (const auto& item : gemsInfos)
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_data_%0.3d;%s\r\n"), 
            nBeginIndex + nIndex,
            item.m_strFileName.c_str());
        strGameInfoOut += szBuf;

        // PRG偏移
        {
            _ultot_s(item.m_nPrgPos >> 13, szBinary, _countof(szBinary), 2);
            ::StringCchPrintf(szBuf, _countof(szBuf),
                _T(" .DW $%04X ;%012.12s %08X CRC32: %08X Size: %08X\r\n"),
                item.m_nPrgPos >> 13,
                szBinary,
                item.m_nPrgPos,
                item.m_nPrgCrc32,
                item.m_nDestPrgSize);
            strGameInfoOut += szBuf;
        }

        // PRG大小
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;PRG Size\r\n"), item.m_nDestPrgCount);
            strGameInfoOut += szBuf;
        }

        // CHR偏移
        {
            _ultot_s(item.m_nChrPos >> 13, szBinary, _countof(szBinary), 2);
            ::StringCchPrintf(szBuf, _countof(szBuf),
                _T(" .DW $%04X ;%012.12s %08X CRC32: %08X Size: %08X\r\n"),
                item.m_nChrPos >> 13,
                szBinary,
                item.m_nChrPos,
                item.m_nChrCrc32,
                item.m_nDestChrSize);

            strGameInfoOut += szBuf;
        }

        // CHR大小
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;CHR Size\r\n"), item.m_nDestChrCount);
            strGameInfoOut += szBuf;
        }

        // FLAG
        {
            DWORD dwFlag = 0;
            if (item.m_bVertical)
            {
                dwFlag |= 1;
            }

            if (item.m_bBatteryBacked)
            {
                dwFlag |= 1 << 1;
            }

            if (item.m_bFourScreen)
            {
                dwFlag |= 1 << 2;
            }

            _ultot_s(dwFlag, szBinary, _countof(szBinary), 2);
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB %%%08.8s ;Mirroring D0(0: H 1: V) Battery: D1(0: N 1: Y) Four-Screen: D2(0: N 1: Y)\r\n"), szBinary);
            strGameInfoOut += szBuf;
        }

        // Mapper
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB %03d ;Mapper\r\n"),
                item.m_nMapper);

            strGameInfoOut += szBuf;
        }

        nIndex++;
    }

    if (!strGameInfoOut.empty())
    {
        strGameInfoOut += _T("\r\n");
    }


    {
        CFileHelper cFile;
        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        std::string strOut = CStrUtils::TStrToU8Str(strGameInfoOut);
        cFile.Write(strOut.c_str(), (DWORD)strOut.size());
        cFile.Close();
    }

    return true;
}

bool CNesCombineUtils::OutList(
    const std::vector<_tstring>& pathList, 
    size_t nBeginIndex,
    const _tstring& strOutFile
)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _tstring strNameListOut;

    if (!pathList.empty())
    {
        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T("game_name_begin_%0.3d\r\n .WORD %d\r\ngame_name_list_%0.3d\r\n"),
            nBeginIndex, nBeginIndex, nBeginIndex
        );
        strNameListOut += szBuf;
    }

    for (size_t i = 0; i < pathList.size(); i++)
    {
        if (0 == (i % 10))
        {
            strNameListOut += _T(" .WORD ");
        }

        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T("game_name_%0.3d"),
            nBeginIndex + i);

        strNameListOut += szBuf;
        if (0 != (i + 1) % 10 && (i != pathList.size() - 1))
        {
            strNameListOut += _T(",");
        }
        else
        {
            strNameListOut += _T("\r\n");
        }
    }
    if (!strNameListOut.empty())
    {
        strNameListOut += _T("\r\n");
    }

    int nIndex = 0;
    for (const auto& item : pathList)
    {
        if (item.empty())
        {
            ::StringCchPrintf(szBuf,
                _countof(szBuf),
                _T("game_name_%0.3d .DB $00\r\n"),
                nBeginIndex + nIndex
            );
        }
        else
        {
            ::StringCchPrintf(szBuf,
                _countof(szBuf),
                _T("game_name_%0.3d .STR \"%s\"\r\n"),
                nBeginIndex + nIndex,
                item.c_str()
            );
        }
        strNameListOut += szBuf;
        nIndex++;
    }

    if (!strNameListOut.empty())
    {
        strNameListOut += _T("\r\n");
    }


    {
        CFileHelper cFile;
        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        std::string strOut = CStrUtils::TStrToU8Str(strNameListOut);
        cFile.Write(strOut.c_str(), (DWORD)strOut.size());
        cFile.Close();
    }

    return true;
}

bool
CNesCombineUtils::OutListFile(
    const _tstring& strScanDir,
    const _tstring& strOutDir,
    const _tstring& strReg
)
{
    _tstring strAsmOut;

    std::vector<_tstring> fileList;
    CFileUtils::GetFileList(fileList, strScanDir, { _T("nes") }, 0);

    if (fileList.empty())
    {
        printf("file list is empty!\n");
        return false;
    }
    std::vector<_tstring> nameList;
    for (const auto& item : fileList)
    {
        nameList.push_back(CPathUtils::GetFileName(item));
    }

    int nBankCount = 0;
    OutListName(nameList, strOutDir, strReg, nBankCount, 0);

    return true;
}

bool CNesCombineUtils::OutListName(
    const std::vector<_tstring>& pathList,
    const _tstring& strOutDir,
    const _tstring& strReg,
    int& nOutBankCount,
    int nItemX/* = 0*/,
    bool bOut/* = true*/,
    bool bFixCount/* = true*/
)
{
    _tstring strAsmOut;

    if (pathList.empty())
    {
        printf("file list is empty!\n");
        return false;
    }

    std::vector<_tstring> nameList = pathList;
    strAsmOut += _T("game_name_list:\r\n");

    // 先替换名字
    for (auto& item : nameList)
    {
        for (auto& ch : item)
        {
            if (ch > _T('\x80'))
            {
                ch = _T('?');
            }
        }
        std::transform(item.begin(), item.end(), item.begin(), [](TCHAR ch)->TCHAR {
            if (ch >= _T('a') && ch <= _T('z')) ch &= ~0x20;
            return ch;
            });
    }

    if (!strReg.empty())
    {
        for (auto& item : nameList)
        {
            _tstring strName = item;
            std::wsmatch result;
            std::wregex regClear(strReg);
            item = std::regex_replace(strName, regClear, _T(""));
        }
    }

    nOutBankCount = 0;

    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        std::vector<_tstring> tempList;

        size_t dwTextSize = 0;
        DWORD dwBank = 0;
        size_t dwIndex = 0;
        std::vector<DWORD> dwNameBank;

        _tstring strBankSize = _T("game_list_bank_index\r\n");


        dwTextSize = 2;
        for (auto& item : nameList)
        {
            //nItemX
            int nItemLeng = (int)item.length();
            if (nItemLeng + nItemX > 32)
            {
                nItemLeng = 32 - nItemX;
            }

            dwTextSize += nItemLeng + 3;
            if (dwTextSize <= 8192 )
            {
                tempList.push_back(item.substr(0, nItemLeng));
                dwNameBank.push_back(dwBank);
            }
            else
            {
                ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_list_%X.asm"), dwBank);
                _tstring strOutFile = strOutDir + _T("\\") + szBuf;

                if (bOut)
                {
                    OutList(tempList, dwIndex - tempList.size(), strOutFile);
                }

                tempList.clear();
                dwBank++;
                nOutBankCount++;
                dwTextSize = 2;
                dwTextSize += item.size() + 3;

                tempList.push_back(item);
                dwNameBank.push_back(dwBank);
            }
            dwIndex++;
        }

        if (!tempList.empty())
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_list_%X.asm"), dwBank);
            _tstring strOutFile = strOutDir + _T("\\") + szBuf;

            if (bOut)
            {
                OutList(tempList, dwIndex - tempList.size(), strOutFile);
            }

            tempList.clear();
            dwBank++;
            nOutBankCount++;
        }

        for (int i = dwBank; i < GAME_NAME_BANK_MAX; i++)
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_list_%X.asm"), dwBank);
            _tstring strOutFile = strOutDir + _T("\\") + szBuf;

            if (bFixCount)
            {
                if (bOut)
                {
                    OutList(tempList, dwIndex - tempList.size(), strOutFile);
                }
            }
            else
            {
                CPathUtils::DeleteArchive(strOutFile);
            }

            tempList.clear();
            dwBank++;
        }

        {
            for (size_t i = 0; i < dwNameBank.size(); i++)
            {
                if (0 == (i % 10))
                {
                    strBankSize += _T(" .DB ");
                }

                ::StringCchPrintf(szBuf,
                    _countof(szBuf),
                    _T("$%02X"), dwNameBank[i]);

                strBankSize += szBuf;

                if (0 != (i + 1) % 10 && (i != dwNameBank.size() - 1))
                {
                    strBankSize += _T(",");
                }
                else
                {
                    strBankSize += _T("\r\n");
                }
            }

            CFileHelper cFile;
            _tstring strOutFile = _T("game_list_bank.asm");
            if (!strOutDir.empty())
            {
                strOutFile = strOutDir + _T("\\game_list_bank.asm");
            }

            if (!cFile.Open(strOutFile, true))
            {
                printf("open file failed!\n");
                return false;
            }

            cFile.Clear();
            std::string strOut = CStrUtils::TStrToU8Str(strBankSize);
            cFile.Write(strOut.c_str(), (DWORD)strOut.size());
            cFile.Close();
        }
    }

    {

        std::string strTitle = std::to_string(nameList.size()) + " IN 1";
        CHAR szBufA[MAX_PATH] = { 0 };
        ::StringCchPrintfA(szBufA,
            _countof(szBufA),
            " .STR \"%s\"",
            strTitle.c_str()
        );

        CFileHelper cFile;
        _tstring strOutFile = _T("game_title.asm");
        if (!strOutDir.empty())
        {
            strOutFile = strOutDir + _T("\\game_title.asm");
        }

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        strTitle = szBufA;
        cFile.Write(strTitle.c_str(), (DWORD)strTitle.size());
        cFile.Close();
    }

    return true;
}

ROM_DATA_INFO_LIST CNesCombineUtils::GetFinalDataForMulticart(
    const std::vector< CNesFileInfo> m_FileInfo,
    DWORD dwRomType
)
{
    ROM_DATA_INFO_LIST infoList;
    if (!(MULTICATR_TYPE_COOLBOY == dwRomType || 
        MULTICATR_TYPE_MINDKIDS == dwRomType ||
        MULTICATR_TYPE_COOLGIRL == dwRomType))
    {
        return infoList;
    }

    for (const auto& item : m_FileInfo)
    {
        auto itFind = g_CoolgirlCfg.find(item.m_nMapper);
        if (itFind == g_CoolgirlCfg.end())
        {
            itFind = g_CoolgirlCfg.find(4);
        }

        if (itFind == g_CoolgirlCfg.end())
        {
            continue;
        }

        if (item.m_bMenu)
        {
            continue;
        }

        COOLGIRL_CFG CoolgirlCfg = itFind->second;
        ROM_DATA_INFO info;
        info.strPath = item.m_strFileName;

        //CHR
        {
            WORD dwChrOffset = (WORD)(item.m_nChrPos >> 13);

            // CHR 偏移基址 8KB 对齐;
            info.ChrOffset = dwChrOffset;

            // CHR 大小
            info.ChrSize = (BYTE)(item.m_nSrcChrCount);
        }

        //PRG
        if (MULTICATR_TYPE_COOLGIRL == dwRomType)
        {
            WORD dwPrgOffset = (WORD)(item.m_nPrgPos >> 14);

            // PRG 偏移基址 16KB 对齐;
            info.PrgReg0 = (dwPrgOffset >> 8) & 0xFF;
            info.PrgReg1 = (dwPrgOffset >> 0) & 0xFF;

            // PRG Size Bit 6 - 0
            info.PrgReg2 |= (0x80 - item.m_nDestPrgCount) & 0x7F;

            // CHR Size Bit 5 - 0
            DWORD dwChrSizeMask = (0x40 - item.m_nDestChrCount) & 0x3F;
            if (0 == info.ChrSize)
            {
                dwChrSizeMask = 0x3F;
            }

            info.PrgReg2 |= (dwChrSizeMask & 0x20) << 2;
            info.PrgReg4 |= dwChrSizeMask & 0x1F;

            // Mapper code 6位
            info.PrgReg6 |= CoolgirlCfg.bMapperCode & 0x1F;
            info.PrgReg7 |= ((CoolgirlCfg.bMapperCode & 0x20) << 1) & 0x40;

            // Mapper Flag
            info.PrgReg6 |= (CoolgirlCfg.bFlag << 5) & 0xE0;

            // PRG bnking Mode
            info.PrgReg3 |= (CoolgirlCfg.bPrgBankingMode << 5) & 0xE0;

            // CHR bnking Mode
            info.PrgReg4 |= (CoolgirlCfg.bChrBankingMode << 5) & 0xE0;

            // Mirroring
            if (!item.m_bVertical) info.PrgReg7 |= 0x08;

            // Enable four-screen mode
            if (item.m_bFourScreen) info.PrgReg7 |= 0x20;

            // Enable WRAM (read and write) at $6000-$7FFF
            info.PrgReg7 |= 0x01;

            // Lockout bit
            info.PrgReg7 |= 0x80;

            if (item.m_nDestChrCount == 0 || 74 == item.m_nMapper)
            {
                // Allow writes to CHR RAM
                info.PrgReg7 |= 0x02;
            }

            // Set PRG bank A for Mapper42 FDS convert
            if (42 == item.m_nMapper)
            {
                info.PrgReg5 |= 0x7C;
            }
        }
        else if (MULTICATR_TYPE_COOLBOY == dwRomType || MULTICATR_TYPE_MINDKIDS== dwRomType)
        {
            DWORD dwPrgOffset = item.m_nPrgPos;

            // PRG
            {
                // PRG A24-A23
                info.PrgReg0 |= ((dwPrgOffset >> 23) << 4) & 0x30;

                // PRG A22
                info.PrgReg1 |= ((dwPrgOffset >> 22) << 3) & 0x08;
                // PRG A21
                info.PrgReg1 |= ((dwPrgOffset >> 21) << 2) & 0x04;
                // PRG A20
                info.PrgReg1 |= ((dwPrgOffset >> 20) << 4) & 0x10;

                // PRG A19-A17
                info.PrgReg0 |= ((dwPrgOffset >> 17) << 0) & 0x07;

                // PRG A16-A14
                info.PrgReg3 |= ((dwPrgOffset >> 14) << 1) & 0x0E;

                // PRG 128KB Size 
                BYTE bPrgSize = (BYTE)(item.m_nDestPrgCount >> 3);
                BYTE bPrgMask = (0x10 - bPrgSize);

                // PRG Size IHGB
                // I A20
                info.PrgReg1 |= ((bPrgMask & 0x08) ^ 0x08) << 2;
                // H A19
                info.PrgReg1 |= ((bPrgMask & 0x04) ^ 0x04) << 4;
                // G A18
                info.PrgReg1 |= ((bPrgMask & 0x02)) << 6;
                // B A17
                info.PrgReg0 |= ((bPrgMask & 0x01)) << 6;

                // CHR mask
                info.PrgReg0 |= (((item.m_nDestChrCount & 0x20) << 2) ^0x80) & 0x80;

                // Lockout
                info.PrgReg3 |= 0x80;
            }

            //CHR
            {
                DWORD dwChrOffset = item.m_nChrPos;

                // CHR A24-A23
                info.ChrReg0 |= ((dwChrOffset >> 23) << 4) & 0x30;

                // CHR A22
                info.ChrReg1 |= ((dwChrOffset >> 22) << 3) & 0x08;
                // CHR A21
                info.ChrReg1 |= ((dwChrOffset >> 21) << 2) & 0x04;
                // CHR A20
                info.ChrReg1 |= ((dwChrOffset >> 20) << 4) & 0x10;

                // CHR A19-A17
                info.ChrReg0 |= ((dwChrOffset >> 17) << 0) & 0x07;

                // CHR A16-A13
                info.ChrReg3 |= ((dwChrOffset >> 13)) & 0x0F;

                // CHR 128KB Size 
                BYTE bChrSize = (BYTE)(item.m_nDestChrCount >> 4);
                if (bChrSize < 1)
                {
                    bChrSize = 1;
                }

                BYTE bChrMask = (0x10 - bChrSize);

                // CHR Size IHGB
                // I PRG A20
                info.ChrReg1 |= ((bChrMask & 0x08) ^ 0x08) << 2;
                // H PRG A19
                info.ChrReg1 |= ((bChrMask & 0x04) ^ 0x04) << 4;
                // G PRG A18
                info.ChrReg1 |= ((bChrMask & 0x02)) << 6;
                // B PRG A17
                info.ChrReg0 |= ((bChrMask & 0x01)) << 6;

                // Banking mode = $00 -> Normal” oversize MMC3 mode.
                info.ChrReg3 &= 0xAF;
            }

            // GNROM
            if (0 == item.m_nMapper)
            {
                info.PrgReg1 |= (item.m_nDestPrgCount >> 1) << 1;

                info.PrgReg0 |= 0xC0;

                info.PrgReg1 &= 0x1F;
                info.PrgReg1 |= 0x80;

                info.PrgReg3 |= 0x10;

                info.ChrReg0 |= 0xC0;

                info.ChrReg1 &= 0x1F;
                info.ChrReg1 |= 0x80;

                // Banking mode = $10 -> GNROM mode
                info.ChrReg3 |= 0x10;
            }

        }

        // Flag
        {
            if (item.m_bVertical) info.Flags |= 0x01;
            if (item.m_bBatteryBacked) info.Flags |= 0x02;
            if (item.m_bFourScreen) info.Flags |= 0x04;
        }

        infoList.push_back(info);
    }

    return infoList;
}

bool CNesCombineUtils::OutFinalDataForMulticart(
    const ROM_DATA_INFO_LIST infoList,
    const _tstring& strOutDir,
    int nRomType,
    int& nOutBankCount,
    bool bOut/* = true*/,
    bool bFixCount/* = true*/
)
{
    TCHAR szBuf[MAX_PATH] = { 0 };

    DWORD dwTextSize = 0;
    DWORD dwBank = 0;
    size_t dwIndex = 0;
    std::vector<DWORD> vDataBank;

    _tstring strBankSize = _T("game_final_data_bank_index\r\n");
    nOutBankCount = 0;

    ROM_DATA_INFO_LIST infoTmp;

    const DWORD beginSize = 2;
    const DWORD indexSize = 2;
    DWORD metaSize = 10;

    if (MULTICATR_TYPE_COOLGIRL == nRomType)
    {
        metaSize = 12;
    }
    else
    {
        metaSize = 10;
    }

    DWORD itemSize = indexSize + metaSize;
    dwTextSize = beginSize;
    for (auto& item : infoList)
    {
        dwTextSize += itemSize;
        if (dwTextSize <= 8192)
        {
            infoTmp.push_back(item);
            vDataBank.push_back(dwBank);
        }
        else
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_final_data_%X.asm"), dwBank);
            _tstring strOutFile = strOutDir + _T("\\") + szBuf;

            if (bOut)
            {
                if (MULTICATR_TYPE_COOLGIRL == nRomType)
                {
                    OutFinalDataForCoolgirlBank(infoTmp, dwIndex - infoTmp.size(), strOutFile);
                }
                else
                {
                    OutFinalDataForCoolboyBank(infoTmp, dwIndex - infoTmp.size(), strOutFile);
                }
            }

            infoTmp.clear();
            dwBank++;
            nOutBankCount++;
            dwTextSize = beginSize;
            dwTextSize += itemSize;

            infoTmp.push_back(item);
            vDataBank.push_back(dwBank);
        }
        dwIndex++;
    }

    if (!infoTmp.empty())
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_final_data_%X.asm"), dwBank);
        _tstring strOutFile = strOutDir + _T("\\") + szBuf;

        if (bOut)
        {
            if (MULTICATR_TYPE_COOLGIRL == nRomType)
            {
                OutFinalDataForCoolgirlBank(infoTmp, dwIndex - infoTmp.size(), strOutFile);
            }
            else
            {
                OutFinalDataForCoolboyBank(infoTmp, dwIndex - infoTmp.size(), strOutFile);
            }
        }
        infoTmp.clear();
        dwBank++;
        nOutBankCount++;
    }

    for (int i = dwBank; i < GAME_FINAL_DATA_BANK_MAX; i++)
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_final_data_%X.asm"), dwBank);
        _tstring strOutFile = strOutDir + _T("\\") + szBuf;

        if (bFixCount)
        {
            if (bOut)
            {
                if (MULTICATR_TYPE_COOLGIRL == nRomType)
                {
                    OutFinalDataForCoolgirlBank(infoTmp, dwIndex - infoTmp.size(), strOutFile);
                }
                else
                {
                    OutFinalDataForCoolboyBank(infoTmp, dwIndex - infoTmp.size(), strOutFile);
                }
            }
        }
        else
        {
            CPathUtils::DeleteArchive(strOutFile);
        }

        infoTmp.clear();
        dwBank++;
    }

    {
        for (size_t i = 0; i < vDataBank.size(); i++)
        {
            if (0 == (i % 10))
            {
                strBankSize += _T(" .DB ");
            }

            ::StringCchPrintf(szBuf,
                _countof(szBuf),
                _T("$%02X"), vDataBank[i]);

            strBankSize += szBuf;

            if (0 != (i + 1) % 10 && (i != vDataBank.size() - 1))
            {
                strBankSize += _T(",");
            }
            else
            {
                strBankSize += _T("\r\n");
            }
        }

        CFileHelper cFile;
        _tstring strOutFile = _T("game_final_data_bank.asm");
        if (!strOutDir.empty())
        {
            strOutFile = strOutDir + _T("\\game_final_data_bank.asm");
        }

        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        std::string strOut = CStrUtils::TStrToU8Str(strBankSize);
        cFile.Write(strOut.c_str(), (DWORD)strOut.size());
        cFile.Close();
    }

    return true;
}

bool CNesCombineUtils::OutFinalDataForCoolgirlBank(
    const ROM_DATA_INFO_LIST infoList,
    size_t nBeginIndex,
    const _tstring& strOutFile
)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _tstring strGameInfoOut;

    if (!infoList.empty())
    {
        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T(";COOLGIRL\r\ngame_final_data_begin_%0.3d\r\n .WORD %d\r\ngame_final_data_list_%0.3d\r\n"),
            nBeginIndex, nBeginIndex, nBeginIndex
        );
        strGameInfoOut += szBuf;
    }

    for (size_t i = 0; i < infoList.size(); i++)
    {
        if (0 == (i % 10))
        {
            strGameInfoOut += _T(" .WORD ");
        }

        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T("game_final_data_%0.3d"),
            nBeginIndex + i);

        strGameInfoOut += szBuf;
        if (0 != (i + 1) % 10 && (i != infoList.size() - 1))
        {
            strGameInfoOut += _T(",");
        }
        else
        {
            strGameInfoOut += _T("\r\n");
        }
    }
    if (!strGameInfoOut.empty())
    {
        strGameInfoOut += _T("\r\n");
    }

    int nIndex = 0;
    for (const auto& item : infoList)
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_final_data_%0.3d;%s\r\n"), 
            nBeginIndex + nIndex, 
            item.strPath.c_str());
        strGameInfoOut += szBuf;

        // CHR
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DW $%04X ;CHR 8KB offset\r\n"), item.ChrOffset);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;CHR 8KB count\r\n"), item.ChrSize);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Mirroring D0(0: H 1: V) Battery-backed: D1(0: N 1: Y) Four-screen: D2(0: N 1: Y)\r\n"), item.Flags);
            strGameInfoOut += szBuf;
        }

        // PRG
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg0 for PRG\r\n"), item.PrgReg0);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg1 for PRG\r\n"), item.PrgReg1);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg2 for PRG\r\n"), item.PrgReg2);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg3 for PRG\r\n"), item.PrgReg3);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg4 for PRG\r\n"), item.PrgReg4);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg5 for PRG\r\n"), item.PrgReg5);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg6 for PRG\r\n"), item.PrgReg6);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg7 for PRG\r\n"), item.PrgReg7);
            strGameInfoOut += szBuf;
        }

        nIndex++;
    }

    if (!strGameInfoOut.empty())
    {
        strGameInfoOut += _T("\r\n");
    }


    {
        CFileHelper cFile;
        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        std::string strOut = CStrUtils::TStrToU8Str(strGameInfoOut);
        cFile.Write(strOut.c_str(), (DWORD)strOut.size());
        cFile.Close();
    }

    return true;
}

bool CNesCombineUtils::OutFinalDataForCoolboyBank(
    const ROM_DATA_INFO_LIST infoList,
    size_t nBeginIndex,
    const _tstring& strOutFile
)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _tstring strGameInfoOut;

    if (!infoList.empty())
    {
        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T(";COOLBOY\r\ngame_final_data_begin_%0.3d\r\n .WORD %d\r\ngame_final_data_list_%0.3d\r\n"),
            nBeginIndex, nBeginIndex, nBeginIndex
        );
        strGameInfoOut += szBuf;
    }

    for (size_t i = 0; i < infoList.size(); i++)
    {
        if (0 == (i % 10))
        {
            strGameInfoOut += _T(" .WORD ");
        }

        ::StringCchPrintf(szBuf,
            _countof(szBuf),
            _T("game_final_data_%0.3d"),
            nBeginIndex + i);

        strGameInfoOut += szBuf;
        if (0 != (i + 1) % 10 && (i != infoList.size() - 1))
        {
            strGameInfoOut += _T(",");
        }
        else
        {
            strGameInfoOut += _T("\r\n");
        }
    }
    if (!strGameInfoOut.empty())
    {
        strGameInfoOut += _T("\r\n");
    }

    int nIndex = 0;
    for (const auto& item : infoList)
    {
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("game_final_data_%0.3d;%s\r\n"),
            nBeginIndex + nIndex,
            item.strPath.c_str());
        strGameInfoOut += szBuf;

        // CHR
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Mirroring D0(0: H 1: V) Battery-backed: D1(0: N 1: Y) Four-screen: D2(0: N 1: Y)\r\n"), item.Flags);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;CHR 8KB count\r\n"), item.ChrSize);
            strGameInfoOut += szBuf;
        }

        // PRG
        {
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg0 for CHR\r\n"), item.ChrReg0);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg1 for CHR\r\n"), item.ChrReg1);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg2 for CHR\r\n"), item.ChrReg2);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg3 for CHR\r\n"), item.ChrReg3);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg0 for PRG\r\n"), item.PrgReg0);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg1 for PRG\r\n"), item.PrgReg1);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg2 for PRG\r\n"), item.PrgReg2);
            strGameInfoOut += szBuf;
            ::StringCchPrintf(szBuf, _countof(szBuf), _T(" .DB $%02X ;Reg3 for PRG\r\n"), item.PrgReg3);
            strGameInfoOut += szBuf;
        }

        nIndex++;
    }

    if (!strGameInfoOut.empty())
    {
        strGameInfoOut += _T("\r\n");
    }


    {
        CFileHelper cFile;
        if (!cFile.Open(strOutFile, true))
        {
            printf("open file failed!\n");
            return false;
        }

        cFile.Clear();
        std::string strOut = CStrUtils::TStrToU8Str(strGameInfoOut);
        cFile.Write(strOut.c_str(), (DWORD)strOut.size());
        cFile.Close();
    }

    return true;
}