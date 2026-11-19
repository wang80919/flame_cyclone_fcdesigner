#pragma once
#include <vector>
#include <map>
#include "CNesHeader.h"

#define GAME_NAME_BANK_MAX          (16)
#define GAME_DATA_BANK_MAX          (8)
#define GAME_FINAL_DATA_BANK_MAX    (8)

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

typedef struct _COOLGIRL_CFG
{
    BYTE bMapper = 0;
    BYTE bSubmapper = 0;
    BYTE bMapperCode = 0;
    BYTE bFlag = 0;
    BYTE bPrgBankingMode = 0;
    BYTE bChrBankingMode = 0;

    _COOLGIRL_CFG()
    {
        ZeroMemory(this, sizeof(_COOLGIRL_CFG));
    }

    _COOLGIRL_CFG(BYTE bMapper, BYTE bSubmapper, BYTE bMapperCode, BYTE bFlag, BYTE bPrgBankingMode, BYTE bChrBankingMode)
    {
        this->bMapper = bMapper;
        this->bSubmapper = bSubmapper;
        this->bMapperCode = bMapperCode;
        this->bFlag = bFlag;
        this->bPrgBankingMode = bPrgBankingMode;
        this->bChrBankingMode = bChrBankingMode;
    }

}COOLGIRL_CFG;

extern std::map<DWORD, COOLGIRL_CFG> g_CoolgirlCfg;

typedef struct _ROM_DATA_INFO
{
    _tstring strPath;

    BYTE ChrReg0 = 0;
    BYTE ChrReg1 = 0;
    BYTE ChrReg2 = 0;
    BYTE ChrReg3 = 0;
    BYTE ChrReg4 = 0;
    BYTE ChrReg5 = 0;
    BYTE ChrReg6 = 0;
    BYTE ChrReg7 = 0;

    BYTE PrgReg0 = 0;
    BYTE PrgReg1 = 0;
    BYTE PrgReg2 = 0;
    BYTE PrgReg3 = 0;
    BYTE PrgReg4 = 0;
    BYTE PrgReg5 = 0;
    BYTE PrgReg6 = 0;
    BYTE PrgReg7 = 0;

    WORD ChrOffset = 0;
    BYTE ChrSize = 0;
    BYTE Flags = 0;

}ROM_DATA_INFO;

typedef std::vector<ROM_DATA_INFO> ROM_DATA_INFO_LIST;

typedef struct _NES_COMBINE_INFO
{
    std::vector< CNesFileInfo> m_FileInfo;
    DWORD m_dwPrgTotalSize = 0;
    DWORD m_dwChrTotalSize = 0;
    DWORD m_dwRomTotalSize = 0;
    DWORD m_dwHeaderSize = 0;
    DWORD m_dwBinFileSize = 0;

    _NES_COMBINE_INFO() {};

    _NES_COMBINE_INFO(const _NES_COMBINE_INFO& r) noexcept
    {
        m_FileInfo = r.m_FileInfo;
        m_dwHeaderSize = r.m_dwHeaderSize;

        m_dwPrgTotalSize = r.m_dwPrgTotalSize;
        m_dwChrTotalSize = r.m_dwChrTotalSize;
        m_dwRomTotalSize = r.m_dwRomTotalSize;
        m_dwBinFileSize = r.m_dwBinFileSize;
    }

    _NES_COMBINE_INFO(_NES_COMBINE_INFO&& r) noexcept
    {
        m_FileInfo = r.m_FileInfo;
        m_dwHeaderSize = r.m_dwHeaderSize;

        r.m_dwHeaderSize = 0;

        m_dwPrgTotalSize = r.m_dwPrgTotalSize;
        m_dwChrTotalSize = r.m_dwChrTotalSize;
        m_dwRomTotalSize = r.m_dwRomTotalSize;
        m_dwBinFileSize = r.m_dwBinFileSize;
    }

    _NES_COMBINE_INFO& operator = (const _NES_COMBINE_INFO& r)
    {
        m_FileInfo = r.m_FileInfo;
        m_dwHeaderSize = r.m_dwHeaderSize;

        m_dwPrgTotalSize = r.m_dwPrgTotalSize;
        m_dwChrTotalSize = r.m_dwChrTotalSize;
        m_dwRomTotalSize = r.m_dwRomTotalSize;
        m_dwBinFileSize = r.m_dwBinFileSize;

        return *this;
    }

    ~_NES_COMBINE_INFO()
    {
    }

}NES_COMBINE_INFO;

class CNesCombineUtils
{
public:

    static NES_COMBINE_INFO AdjustRomData(
        std::vector<CNesFileInfo> infoList, 
        CNesFileInfo infoMenu,
        DWORD dwType = 0
    );

    static bool CombineRomData(
        const NES_COMBINE_INFO& info,
        const _tstring& strPath,
        bool bUnifHeader = true,
        const std::string strBoardName = "COOLBOY"
    );

    static bool CombineNesData(
        const NES_COMBINE_INFO& info,
        const _tstring& strPath,
        DWORD dwMapper = 268,
        DWORD dwSubmapper = 0
    );

    static bool Dump(
        const NES_COMBINE_INFO& info, 
        LPBYTE* lpDataOut, 
        DWORD& dwSize
    );

    static bool OutGameInfo(
        const NES_COMBINE_INFO& infoCombine,
        const _tstring& strAsmPath,
        int& nOutBankCount,
        bool bOut = true,
        bool bFixCount = true
    );

    static bool OutListFile(
        const _tstring& strScanDir,
        const _tstring& strOutDir,
        const _tstring& strReg
    );

    static bool OutListName(
        const std::vector<_tstring>& strList,
        const _tstring& strOutDir,
        const _tstring& strReg,
        int& nOutBankCount,
        int nItemX,
        bool bOut = true,
        bool bFixCount = true
    );

    static bool OutList(
        const std::vector<_tstring>& pathList, 
        size_t nBeginIndex,
        const _tstring& strOutFile
    );

    static bool OutGameDataInfo(
        const std::vector<CNesFileInfo>& gemsInfos,
        size_t nBeginIndex,
        const _tstring& strOutFile
    );

    static ROM_DATA_INFO_LIST GetFinalDataForMulticart(
        const std::vector< CNesFileInfo> m_FileInfo,
        DWORD dwRomType
    );

    static bool OutFinalDataForMulticart(
        const ROM_DATA_INFO_LIST infoList,
        const _tstring& strOutDir,
        int nRomType,
        int& nOutBankCount,
        bool bOut = true,
        bool bFixCount = true
    );

    static bool OutFinalDataForCoolgirlBank(
        const ROM_DATA_INFO_LIST infoList,
        size_t nBeginIndex,
        const _tstring& strOutFile
    );

    static bool OutFinalDataForCoolboyBank(
        const ROM_DATA_INFO_LIST infoList,
        size_t nBeginIndex,
        const _tstring& strOutFile
    );
};