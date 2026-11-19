#pragma once

#include <wtypesbase.h>
#include <string>
#include <tchar.h>
#include <map>
#include <windows.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

#define UNIF_META_TYPE_ID_LEN           =  4

#pragma warning(disable:4200)

typedef struct _UNIF_HEADER
{
    BYTE m_Flag[4] = { 'U', 'N', 'I', 'F' };     // "UNIF" 常量字符串标识
    DWORD m_dwVer = 5;                          // LE32，解析文件中的所有区块所需的最低版本号
    BYTE m_EmptyData[24] = { 0 };               // 空值
    BYTE m_Data[0];                             // 任意数量的数据块
}UNIF_HEADER, * PUNIF_HEADER;

typedef struct _UNIF_META
{
    BYTE m_Type[4] = {0};       //类型，变化，  定义如下
                                //MAPR 变长     唯一的人类可读标识符，指定所使用的确切硬件;
                                                //不是 iNES 映射器编号，也不是映射器的全文描述;必填
                                //PRGn 变长     第 n 个 PRG ROM 的内容;至少需要 PRG0;n 是十六进制
                                //CHRn 变长     第n个CHR ROM的内容
                                //PCKn 4字节    第 n 个 PRG ROM 的 CRC-32
                                //CCKn 4字节    第 n 个 CHR ROM 的 CRC-32
                                //NAME 变长     游戏名称
                                //WRTR 变长     非官方，无效。转储软件的名称。应该在 DINF 块中
                                //READ 变长     关于游戏的评论，尤其是自制软件的许可信息
                                //DINF 204字节  Dumping信息
                                                //偏移长度（字节）值
                                                //0 100 个以空结尾的 UTF - 8 转储程序名称
                                                //100 每月 1 天的转储
                                                //101 一年中的 1 个月转储
                                                //102 2 LE16，转储年份
                                                //104 100 以空值结尾的 UTF - 8 转储软件或机制的名称
                                //TVCI 1字节    电视标准兼容性信息 0:NTSC 1:PAL 2:两者都兼容
                                //CTRL 1字节    此游戏可用的控制器（位掩码）
                                                //1.标准控制器
                                                //2.灭音器
                                                //4.中华民国
                                                //8.类阿卡控制器
                                                //16.电源板
                                                //32.四分
                                                //64.扩展（已清除）
                                                //128.扩展（已清除）
                                //BATR 1字节    指定 RAM 是否由电池供电的布尔值。
                                //VROR 1字节    “如果这个块存在，那么即使存在ROM，CHR-ROM区域也将被视为RAM。
                                //MIRR 1字节    CIRAM A10 连接到什么：
                                                //0.PPU A11（水平镜像）
                                                //1.PPU A10（垂直镜像）
                                                //2.接地（单屏 A）
                                                //3.虚拟电话（单屏 B）
                                                //4.增加了额外的内存（四屏）
                                                //5.映射器控制




    DWORD m_Length = 0;                         // LE32，长度
    BYTE m_Data[0];                             // 内容编码因类型而异
}UNIF_META, *PUNIF_META;

class CUnifHelper
{
public:
    CUnifHelper();
    ~CUnifHelper();

    bool SetPRG(BYTE nIndex, LPBYTE lpData, DWORD dwSize);
    bool SetCHR(BYTE nIndex, LPBYTE lpData, DWORD dwSize);
    bool SetMirroring(BYTE bValue = 5);
    bool SetBatteryBacked(BYTE bValue = 1);
    bool SetTVStandard(BYTE bValue = 0);
    bool SetBoardName(const std::string& strName = "COOLBOY");
    bool SetDumpingInfo(const std::string& strDumper, BYTE bDay, BYTE bMonth, BYTE bYear, const std::string& strSoftware);
    bool Dump(LPCBYTE *lpDataBuf, DWORD &dwSize);

    bool RemoveMeta(const std::string strMeta);

private:

    bool SetByteData(const std::string& strId, BYTE bValue);
    bool SetLargeData(const std::string& strId, LPBYTE lpData, DWORD dwSize);

private:
    std::map<std::string, PUNIF_META> m_MetaList;
};