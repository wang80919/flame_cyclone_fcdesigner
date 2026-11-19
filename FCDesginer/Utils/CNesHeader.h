#pragma once

#include <wtypesbase.h>
#include <string>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

#define NES_HEAD_SIZE           (0x10)          // NES文件头大小
#define NES_TRAINER_SIZE        (0x200)
#define NES_PRG_BANK_SIZE       (0x4000)
#define NES_CHR_BANK_SIZE       (0x2000)

// .NES 文件格式 1.0
#pragma pack(1)
typedef struct _NES_HEADER
{
    // BYTE 0x00 常量 $4E $45 $53 $1A（ASCII“NES”后跟 MS-DOS 文件结尾）
    BYTE Constant[4] = { 'N', 'E', 'S', 0x1A };

    // BYTE 0x04 PRG ROM 的大小，以 16 KB 为单位
    BYTE PrgRomSize;

    // BYTE 0x05 CHR ROM 的大小，以 8 KB 为单位（值 0 表示开发板使用 CHR RAM）
    BYTE ChrRomSize;

    // BYTE 0x06 映射器、镜像、电池、训练器
    BYTE bMirroring : 1;        // 0：水平 1：垂直
    BYTE bBatteryBacked : 1;    // 1：包含电池供电的 PRG RAM（$6000-7FFF）或其他持久内存
    BYTE bTrainer : 1;          // 1：在 $7000-$71FF 有 512 字节训练器 （存储在 PRG 数据之前）
    BYTE bFourScreen : 1;       // 1：忽略镜像控制或以上镜像位； 而是提供四屏显存
    BYTE MapperLow : 4;         // 映射器编号的低 4 位

    // BYTE 0x07 映射器、VS/Playchoice、NES 2.0
    BYTE VsUnisystem : 1;       // VS 统一系统
    BYTE PlayChoice10 : 1;      // PlayChoice-10（在 CHR 数据之后存储的 8 KB 提示屏幕数据）
    BYTE fNesVer : 2;           // 如果等于 2，则标志 8-15 为 NES 2.0 格式
    BYTE MapperHigh : 4;        // 映射器编号的高 4 位

    // BYTE 0x08 PRG-RAM 大小（很少使用的扩展）
    // PRG RAM 的大小（以 8 KB 为单位（值 0 推断 8 KB 的兼容性; 请参阅 PRG RAM 电路)
    // 这是后来对iNES格式的扩展，并未广泛使用。建议使用 NES 2.0 来指定 PRG RAM 大小。
    BYTE PrgRamSize;            // PRG RAM 大小 以 8 KB 为单位 
                                // 8KB, 16KB. 32KB, 64KB, 128KB, 256KB, 512KB, 1024KB

    // BYTE 0x09 电视系统（很少使用的扩展）
    // 尽管在官方规范中，很少有仿真器支持这一点，因为流通中的 ROM 映像几乎没有使用它。
    BYTE bTvSystem : 1;         // 电视制式（0：NTSC；1：PAL）
    BYTE bReserved9 : 7;        // 保留，设置为零

    // BYTE 0x0A 电视系统，PRG-RAM 存在（非官方，很少使用的扩展）
    // 这个字节不是官方规范的一部分，相对较少的模拟器遵循它。
    BYTE TvSystemCompatible : 2;    // 电视制式（0：NTSC；2：PAL；1/3：双兼容）
    BYTE bReservedA1 : 2;           // 未使用
    BYTE bPrgRam : 1;               // PRG RAM ($6000-$7FFF)（0：存在；1：不存在）
    BYTE bBusConflicts : 1;         // 0：板卡没有总线冲突； 1：板卡有总线冲突
    BYTE bReservedA2 : 2;           // 未使用

    // BYTE 0x0B - 0x0F 未使用的填充（应该用零填充，但一些 rippers 将他们的名字放在字节 7-15 之间）
    BYTE ReservedPadding[5];

    _NES_HEADER()
    {
        ::ZeroMemory(&PrgRomSize, sizeof(_NES_HEADER) - sizeof(Constant));
    }

}NES_HEADER;
#pragma pack()

// .NES 文件格式 2.0
#pragma pack(1)
typedef struct _NES_HEADER_2
{
    // BYTE 0x00 - 0x03
    BYTE Constant[4] = {'N', 'E', 'S', 0x1A};               // 标识字符串。 必须是"NES<EOF>".

    // BYTE 0x04
    BYTE PrgRomSizeL;               // PRG-ROM 大小 最低8位有效位

    // BYTE 0x05
    BYTE ChrRomSizeL;               // CHR-ROM 大小 最低8位有效位

    // BYTE 0x06
    BYTE bMirroring : 1;            // 硬连线命名表镜像类型 (0: 水平  1: 垂直)
    BYTE bBatteryBacked : 1;        // 电池存储 (0: 不存在 1: 存在)
    BYTE bTrainer : 1;              // 512 字节 Trainer (0: 不存在 1: 存在)
    BYTE bFourScreen : 1;           // 硬连线四屏模式 (0: 不存在 1: 存在)
    BYTE MapperL : 4;               // 映射器编号低4位 D0..D3

    // BYTE 0x07
    BYTE ConsoleType : 2;           // 控制台类型
    BYTE Nes2Identifier : 2;        // 控制台类型, 如果等于 2，则标志 8-15 为 NES 2.0 格式
    BYTE MapperM : 4;               // 映射器编号中4位 D0..D3

    // BYTE 0x08 映射器 MSB/子映射器
    BYTE MapperH : 4;               // 映射器编号高4位 D8..D11
    BYTE Submapper : 4;             // 子映射器编号

    // BYTE 0x09 PRG-ROM/CHR-ROM 大小 最高有效位
    BYTE PrgRomSizeM : 4;           // PRG-ROM 大小 最高4位有效位
    BYTE ChrRomSizeM : 4;           // CHR-ROM 大小 最高4位有效位

    // BYTE 0x0A PRG-RAM/EEPROM 大小
    BYTE PrgRamCount : 4;           // PRG-RAM（易失性）移位计数
    BYTE PrgNvRamCount : 4;         // PRG-NVRAM/EEPROM（非易失性）移位计数

    // BYTE 0x0B CHR-RAM 大小
    BYTE ChrRamSize : 4;            // CHR-RAM 大小（易失性）移位计数
    BYTE ChrNvRamSize : 4;          // CHR-NVRAM 大小（非易失性）移位计数

    // BYTE 0x0C CPU/PPU 时序
    BYTE CpuPpuTimingDmoe : 2;      // CPU/PPU时序模式
                                    // 0：RP2C02 ("NTSC NES")
                                    // 1：RP2C07 ( "许可 PAL NES")
                                    // 2：多区域
                                    // 3：UA6538 ("Dendy")
    BYTE ReservedC:6;

    // BYTE 0x0D 当字节 7 和 3 =1 时：Vs.系统类型
    BYTE VsPpuType:4;               // Vs.PPU类型
    BYTE VsHardwareType : 4;        // Vs.硬件类型

    // BYTE 0x0E 杂项 ROM
    BYTE MiscellaneousROMs : 2;     // 存在的杂项 ROM 数量
    BYTE ReservedE : 6;             // 未使用

    // BYTE 0x0F 默认扩展设备
    BYTE DefaultExpansionDevice : 6;    // 默认扩展设备
    BYTE ReservedF : 2;                 // 未使用

    _NES_HEADER_2()
    {
        ::ZeroMemory(&PrgRomSizeL, sizeof(_NES_HEADER_2) - sizeof(Constant));
    }

}NES_HEADER_2;
#pragma pack()

class CNesHeader
{
public:
    CNesHeader(const _tstring& strPath);

    ~CNesHeader();

    bool IsLoad() const;
    bool Load(const _tstring& strPath);
    bool IsNesFile() const;
    unsigned int GetPrgCount() const;
    unsigned int GetChrCount() const;
    unsigned int GetPrgSize() const;
    unsigned int GetChrSize() const;
    bool IsVertical() const;
    bool IsHorizontal() const;
    bool IsBatteryBacked() const;
    bool IsTrainer() const;
    bool IsFourScreen() const;
    bool IsCoolboy() const;
    bool IsCoolgirl() const;
    unsigned int GetMapper() const;

    bool IsNes2() const;
    _tstring GetFileDir() const;
    _tstring GetFilePath() const;
    _tstring GetFileName() const;
    _tstring GetRemark() const;
    _tstring SetRemark(const _tstring strRemark);
    _tstring SetFileName(const _tstring strName);

    bool IsChecked() const
    {
        return m_bChecked;
    }

    void SetChecked(bool bChecked)
    {
        m_bChecked = bChecked;
    }

    bool operator == (const CNesHeader& r) const
    {
        return m_FilePath == r.m_FilePath;
    }

private:

    bool _IsSupportCoolBoy();
    bool _IsSupportCoolGirl();

private:

    _tstring m_FilePath;
    _tstring m_FileName;
    _tstring m_FileRemark;
    NES_HEADER m_iNesHead;
    bool m_bLoaded = false;
    bool m_bChecked = false;
    bool m_bCoolBoy = false;
    bool m_bCoolGirl = false;
};

class CNesFileInfo
{
public:

    CNesFileInfo();
    CNesFileInfo(const _tstring& strPath);
    ~CNesFileInfo();

    bool m_bPrgSet = false;
    bool m_bChrSet = false;
    DWORD m_nPrgPos = 0;
    DWORD m_nChrPos = 0;
    DWORD m_nPrgCrc32 = 0;
    DWORD m_nChrCrc32 = 0;
    DWORD m_nDestPrgSize = 0;
    DWORD m_nDestChrSize = 0;
    DWORD m_nSrcPrgCount = 0;
    DWORD m_nSrcChrCount = 0;
    DWORD m_nSrcPrgSize = 0;
    DWORD m_nSrcChrSize = 0;
    DWORD m_nRomIndex = 0;
    DWORD m_nMapper = 0;
    DWORD m_nHeadSize = 0;

    DWORD m_nDestPrgCount = 0;
    DWORD m_nDestChrCount = 0;
    DWORD m_nTrainerSize = 0;

    bool m_bVertical = false;
    bool m_bTrainer = false;
    bool m_bBatteryBacked = false;
    bool m_bFourScreen = false;
    bool m_bMenu = false;
    bool m_bIsNes = false;
    bool m_bChecked = false;
    _tstring m_strFilePath;
    _tstring m_strFileName;

    void SetSrcPrgCount(DWORD dwCount);
    void SetSrcChrCount(DWORD dwCount);
    void SetDestPrgCount(DWORD dwCount);
    void SetDestChrCount(DWORD dwCount);

    bool Load(const _tstring& strPath);
    bool operator == (const CNesFileInfo& r) const;
};