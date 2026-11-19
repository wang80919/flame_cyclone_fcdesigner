#include "CNesHeader.h"
#include "CCrc32Utils.h"
#include "Win32Utils/CFileUtils.h"
#include "Common.h"

const BYTE NesHeadFlag[] = { 0x4E, 0x45, 0x53, 0x1A };   // NES文件标识

CNesHeader::CNesHeader(const _tstring& strPath)
{
    Load(strPath);
}

CNesHeader::~CNesHeader()
{

}

bool CNesHeader::Load(const _tstring& strPath)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD lpNumberOfBytesRead = 0;
    bool bSuccess = false;
    m_FilePath = strPath;
    m_FileName = m_FilePath.substr(m_FilePath.find_last_of(_T('\\')) + 1);
    m_FileName = m_FileName.substr(0, m_FileName.find_last_of(_T('.')));

    hFile = CreateFile(strPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        return false;
    }

    do
    {
        if (!::ReadFile(hFile, &m_iNesHead, sizeof(NES_HEADER), &lpNumberOfBytesRead, nullptr))
        {
            break;
        }

    } while (FALSE);

    ::CloseHandle(hFile);

    if (sizeof(NES_HEADER) == lpNumberOfBytesRead)
    {
        bSuccess = true;
    }

    m_bLoaded = bSuccess;
    m_bCoolBoy = _IsSupportCoolBoy();
    m_bCoolGirl = _IsSupportCoolGirl();

    return bSuccess;
}

bool CNesHeader::_IsSupportCoolBoy()
{
    if (!IsNesFile())
    {
        return false;
    }

    unsigned int nMapper = GetMapper();
    unsigned int nPrgCount = GetPrgCount();
    unsigned int nChrCount = GetChrCount();

    if (0 == nMapper )
    {
        if (nPrgCount > 2)
        {
            return false;
        }

        if (nChrCount > 1)
        {
            return false;
        }

        return true;
    }

    if (4 == nMapper || 74 == nMapper)
    {
        //检查PRG大小
        bool bCheckPrg = false;

        //检查16KB prg
        for (auto prgSize : { 1, 2, 4, 8, 16, 32, 64, 128 })
        {
            if (prgSize == (int)nPrgCount)
            {
                bCheckPrg = true;
                break;
            }
        }

        if (!bCheckPrg)
        {
            return false;
        }

        return true;
    }

    return false;
}

bool CNesHeader::_IsSupportCoolGirl()
{
    if (!IsNesFile())
    {
        return false;
    }

    unsigned int nMapper = GetMapper();
    unsigned int nPrgCount = GetPrgCount();
    unsigned int nChrCount = GetChrCount();

    auto mapperStandard = g_CoolGirlMapperTable.find(nMapper);
    if (g_CoolGirlMapperTable.end() == mapperStandard)
    {
        return false;
    }

    //检查PRG大小
    bool bCheckPrg = false;

    //检查16KB prg
    for (auto prgSize : mapperStandard->second.m_PrgSet)
    {
        if (prgSize == (int)nPrgCount)
        {
            bCheckPrg = true;
            break;
        }
    }

    if (!bCheckPrg)
    {
        return false;
    }

    return true;
}

bool CNesHeader::IsLoad() const
{
    return m_bLoaded;
}

bool CNesHeader::IsNesFile() const
{
    if (0 == m_iNesHead.PrgRomSize)
    {
        return false;
    }

    return 0 == memcmp(&m_iNesHead, NesHeadFlag, sizeof(NesHeadFlag));
}

unsigned int CNesHeader::GetPrgCount() const
{
    unsigned int uPrgCount = m_iNesHead.PrgRomSize;
    if (IsNes2())
    {
        uPrgCount |= ((unsigned int)(((LPBYTE)(&m_iNesHead))[9] & 0x0F)) << 8;
    }

    return uPrgCount;
}

unsigned int CNesHeader::GetChrCount() const
{
    unsigned int uChrCount = m_iNesHead.ChrRomSize;
    if (IsNes2())
    {
        uChrCount |= ((unsigned int)(((LPBYTE)(&m_iNesHead))[9] & 0xF0) >> 4) << 8;
    }

    return uChrCount;
}

unsigned int CNesHeader::GetPrgSize() const
{
    unsigned int uPrgCount = m_iNesHead.PrgRomSize;
    if (IsNes2())
    {
        uPrgCount |= ((unsigned int)(((LPBYTE)(&m_iNesHead))[9] & 0x0F)) << 8;
    }

    return uPrgCount * NES_PRG_BANK_SIZE;
}

unsigned int CNesHeader::GetChrSize() const
{
    unsigned int uChrCount = m_iNesHead.ChrRomSize;
    if (IsNes2())
    {
        uChrCount |= ((unsigned int)(((LPBYTE)(&m_iNesHead))[9] & 0xF0) >> 4) << 8;
    }

    return uChrCount * NES_CHR_BANK_SIZE;
}

bool CNesHeader::IsVertical() const
{
    return m_iNesHead.bMirroring;
}

bool CNesHeader::IsHorizontal() const
{
    return 0 == m_iNesHead.bMirroring;
}

bool CNesHeader::IsBatteryBacked() const
{
    return m_iNesHead.bBatteryBacked;
}

bool CNesHeader::IsTrainer() const
{
    return m_iNesHead.bTrainer;
}

bool CNesHeader::IsFourScreen() const
{
    return m_iNesHead.bFourScreen;
}

bool CNesHeader::IsCoolboy() const
{
    return m_bCoolBoy;
}

bool CNesHeader::IsCoolgirl() const
{
    return m_bCoolGirl;
}

unsigned int CNesHeader::GetMapper() const
{
    unsigned int nMapper = ((unsigned int)m_iNesHead.MapperHigh << 4) | (m_iNesHead.MapperLow);
    if (IsNes2())
    {
        nMapper |= ((unsigned int)(((LPBYTE)(&m_iNesHead))[8] & 0x0F)) << 8;
    }

    return nMapper;
}

bool CNesHeader::IsNes2() const
{
    return m_iNesHead.fNesVer == 0x02;
}

_tstring CNesHeader::GetFileDir() const
{
    _tstring strFileName = m_FilePath.substr(0, m_FilePath.find_last_of(_T('\\')));

    return strFileName;
}

_tstring CNesHeader::GetFilePath() const
{
    return m_FilePath;
}

_tstring CNesHeader::GetFileName() const
{
    return m_FileName;
}

_tstring CNesHeader::SetFileName(const _tstring strName)
{
    m_FileName = strName;
    return m_FileName;
}

_tstring CNesHeader::GetRemark() const
{
    return m_FileRemark;
}

_tstring CNesHeader::SetRemark(const _tstring strRemark)
{
    m_FileRemark = strRemark;
    return m_FileRemark;
}

CNesFileInfo::CNesFileInfo()
{

}

CNesFileInfo::CNesFileInfo(const _tstring& strPath)
{
    Load(strPath);
}

CNesFileInfo::~CNesFileInfo()
{

}

void CNesFileInfo::SetSrcPrgCount(DWORD dwCount)
{
    m_nSrcPrgCount = dwCount;
    m_nSrcPrgSize = dwCount * NES_PRG_BANK_SIZE;
}

void CNesFileInfo::SetSrcChrCount(DWORD dwCount)
{
    m_nSrcChrCount = dwCount;
    m_nSrcChrSize = dwCount * NES_CHR_BANK_SIZE;
}

void CNesFileInfo::SetDestPrgCount(DWORD dwCount)
{
    m_nDestPrgCount = dwCount;
    m_nDestPrgSize = dwCount * NES_PRG_BANK_SIZE;
}

void CNesFileInfo::SetDestChrCount(DWORD dwCount)
{
    m_nDestChrCount = dwCount;
    m_nDestChrSize = dwCount * NES_CHR_BANK_SIZE;
}

bool CNesFileInfo::Load(const _tstring& strPath)
{
    NES_HEADER m_iNesHead;

    m_strFilePath = strPath;

    CFileHelper cFile(strPath, true);
    if (!cFile.IsOpen())
    {
        return false;
    }

    if (!cFile.Read(&m_iNesHead, sizeof(NES_HEADER)))
    {
        return false;
    }

    if (0 != memcmp(&m_iNesHead, NesHeadFlag, sizeof(NesHeadFlag)))
    {
        return false;
    }

    m_strFileName = m_strFilePath.substr(m_strFilePath.find_last_of(_T('\\')) + 1);
    m_strFileName = m_strFileName.substr(0, m_strFileName.find_last_of(_T('.')));

    m_nHeadSize = sizeof(NES_HEADER);
    
    m_nTrainerSize = m_iNesHead.bTrainer ? NES_TRAINER_SIZE : 0;

    m_nMapper = ((unsigned int)m_iNesHead.MapperHigh << 4) | (m_iNesHead.MapperLow);
    if (2 == m_iNesHead.fNesVer)
    {
        m_nMapper |= ((unsigned int)(((LPBYTE)(&m_iNesHead))[8] & 0x0F)) << 8;
    }

    m_nSrcPrgCount = m_iNesHead.PrgRomSize;
    m_nSrcChrCount = m_iNesHead.ChrRomSize;
    m_nSrcPrgSize = m_iNesHead.PrgRomSize * NES_PRG_BANK_SIZE;
    m_nSrcChrSize = m_iNesHead.ChrRomSize * NES_CHR_BANK_SIZE;

    m_nDestPrgCount = m_iNesHead.PrgRomSize;
    m_nDestChrCount = m_iNesHead.ChrRomSize;
    m_nDestPrgSize = m_iNesHead.PrgRomSize * NES_PRG_BANK_SIZE;
    m_nDestChrSize = m_iNesHead.ChrRomSize * NES_CHR_BANK_SIZE;

    m_bVertical = m_iNesHead.bMirroring;;
    m_bTrainer = m_iNesHead.bTrainer;
    m_bFourScreen = m_iNesHead.bFourScreen;
    m_bBatteryBacked = m_iNesHead.bBatteryBacked;
    m_bIsNes = true;

    CByteBuffer bufPrgData(m_nSrcPrgSize);
    CByteBuffer bufChrData(m_nSrcChrSize);

    DWORD dwOffset = sizeof(NES_HEADER) + m_nTrainerSize;
    cFile.Read(bufPrgData.GetBuffer(), (DWORD)bufPrgData.GetSize(), dwOffset, nullptr, FILE_BEGIN);
    m_nPrgCrc32 = CCrc32Utils::GetCrc32Value(bufPrgData.GetBuffer(), bufPrgData.GetSize());

    dwOffset += m_nSrcPrgSize;
    cFile.Read(bufChrData.GetBuffer(), (DWORD)bufChrData.GetSize(), dwOffset, nullptr, FILE_BEGIN);
    m_nChrCrc32 = CCrc32Utils::GetCrc32Value(bufChrData.GetBuffer(), bufChrData.GetSize());

    return true;
}

bool CNesFileInfo::operator == (const CNesFileInfo& r) const
{
    return m_strFilePath == r.m_strFilePath;
}