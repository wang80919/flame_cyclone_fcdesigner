#pragma once

#include <wtypesbase.h>
#include <vector>
#include <string>
#include <map>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

enum eFileAttributes
{
    eReadonly              = FILE_ATTRIBUTE_READONLY,
    eHidden                = FILE_ATTRIBUTE_HIDDEN,
    eSystem                = FILE_ATTRIBUTE_SYSTEM,
    eDirectory             = FILE_ATTRIBUTE_DIRECTORY,
    eArchive               = FILE_ATTRIBUTE_ARCHIVE,
    eDevice                = FILE_ATTRIBUTE_DEVICE,
    eNormal                = FILE_ATTRIBUTE_NORMAL,
    eTemporary             = FILE_ATTRIBUTE_TEMPORARY,
    eSparse_file           = FILE_ATTRIBUTE_SPARSE_FILE,
    eReparse_point         = FILE_ATTRIBUTE_REPARSE_POINT,
    eCompressed            = FILE_ATTRIBUTE_COMPRESSED,
    eOffline               = FILE_ATTRIBUTE_OFFLINE,
    eNot_content_indexed   = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
    eEncrypted             = FILE_ATTRIBUTE_ENCRYPTED,
    eIntegrity_stream      = FILE_ATTRIBUTE_INTEGRITY_STREAM,
    eVirtual               = FILE_ATTRIBUTE_VIRTUAL,
    eNo_scrub_data         = FILE_ATTRIBUTE_NO_SCRUB_DATA,
    eEa                    = FILE_ATTRIBUTE_EA,
    ePinned                = FILE_ATTRIBUTE_PINNED,
    eUnpinned              = FILE_ATTRIBUTE_UNPINNED,
    eRecall_on_open        = FILE_ATTRIBUTE_RECALL_ON_OPEN,
    eRecall_on_data_access = FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS,
};

typedef struct _FILEINFO
{
    _tstring m_fileDir;
    _tstring m_fileName;
    _tstring m_filePath;
    unsigned long long m_fileSize;
    DWORD m_fileAttributes;
    unsigned long long m_ftCreationTime;
    unsigned long long m_ftLastAccessTime;
    unsigned long long m_ftLastWriteTime;

    _FILEINFO() :
        m_fileSize(0),
        m_fileAttributes(0),
        m_ftCreationTime(0),
        m_ftLastAccessTime(0),
        m_ftLastWriteTime(0)
    {


    };
}FILEINFO;

class CFileUtils
{
public:
    //
    // @brief: 获取文件扩展名
    // @param: strfileName     文件名, 如: test.exe
    // @param: bIncludeDot     是否包含 . 号
    // @ret: 扩展名            如 test.exe bIncludeDot = false 时返回 exe, bIncludeDot = true 时返回 .exe, 
    static _tstring GetFileExtName(
        const _tstring& strfileName,
        bool bIncludeDot = false
    );

    //
    // @brief: 获取目录下文件路径
    // @param: fileList     文件路径字符串容器
    // @param: strDir          指定目录
    // @param: extList      扩展名过滤列表, 如: {("exe"), ("txt")}
    // @param: depth        目录深度 如: 0：指定目录为根目录, 遍历0层子目录 
    // @param: bIgnoreCase  是否忽略扩展名过滤大小写
    // @ret: void
    static void GetFileList(
        std::vector<_tstring>& fileList,
        const _tstring& strDir,
        const std::vector<_tstring>& extList = {},
        int32_t depth = MAX_PATH,
        bool bIgnoreCase = true
    );

    //
    // @brief: 获取目录下文件路径
    // @param: fileList     文件信息容器
    // @param: strDir          指定目录
    // @param: extList      扩展名过滤列表, 如: {("exe"), ("txt")}
    // @param: depth        目录深度 如: 0：指定目录为根目录, 遍历0层子目录 
    // @param: bIgnoreCase  是否忽略扩展名过滤大小写
    // @ret: void
    static void GetFileList(
        std::map<_tstring, FILEINFO>& fileList,
        const _tstring& strDir,
        const std::vector<_tstring>& extList = {},
        int32_t depth = MAX_PATH,
        bool bIgnoreCase = true
    );

    //
    // @brief: 获取目录下子目录
    // @param: fileList     文件信息容器
    // @param: strDir          指定目录
    // @param: depth        目录深度 如: 0：指定目录为根目录, 遍历0层子目录 
    // @ret: void
    static void GetDirList(
        std::vector<_tstring>& fileList,
        const _tstring& strDir,
        int32_t depth = MAX_PATH
    );

    //
    // @brief: 获取目录下文件路径
    // @param: fileList     文件信息容器
    // @param: strDir          指定目录
    // @param: depth        目录深度 如: 0：指定目录为根目录, 遍历0层子目录 
    // @ret: void
    static void GetDirList(
        std::map<_tstring, FILEINFO>& dirList,
        const _tstring& strDir,
        int32_t depth = MAX_PATH
    );

    //
    // @brief: 检查路径是否存在
    // @param: strPath      路径
    // @ret: 存在返回true
    static bool IsExist(const _tstring& strPath);

    //
    // @brief: 检查路径是否隐藏
    // @param: strPath      路径
    // @ret: 隐藏返回true
    static bool IsHidden(const _tstring& strPath);

    //
    // @brief: 检查路径是否是存档
    // @param: strPath      路径
    // @ret: 存档返回true
    static bool IsArchive(const _tstring& strPath);

    //
    // @brief: 检查路径是否是只读
    // @param: strPath      路径
    // @ret: 只读返回true
    static bool IsReadOnly(const _tstring& strPath);

    //
    // @brief: 检查路径是否是目录
    // @param: strPath      路径
    // @ret: 目录返回true
    static bool IsDirectory(const _tstring& strPath);

    //
    // @brief: 获取路径属性
    // @param: strPath      路径
    // @ret: 成功返回true
    static bool GetAttributes(const _tstring& strPath, DWORD& dwAttr);

    //
    // @brief: 设置路径属性
    // @param: strPath      路径
    // @ret: 成功返回true
    static bool SetAttributes(const _tstring& strPath, const DWORD& dwAttr);

    //
    // @brief: 获取文件大小
    // @param: strPath      路径
    // @ret: 文件大小
    static unsigned long long GetFileSize(const _tstring& strPath);

    //
    // @brief: 删除文件
    // @param: strPath      路径
    // @ret: 成功返回true
    static bool DeleteArchive(const _tstring& strPath);

    //
    // @brief: 删除目录(递归删除子目录及文件)
    // @param: strPath      路径
    // @ret: 成功返回true
    static bool DeleteDir(const _tstring& strPath);

    //
    // @brief: 创建目录(递归)
    // @param: strPath      路径
    // @ret: 成功返回true
    static bool CreateDir(const _tstring& strPath);
};

//简单的字节数据对象, 维护字节数组
class CByteBuffer
{
public:
    CByteBuffer();
    CByteBuffer(size_t nSize, BYTE nValue = 0xFF);
    CByteBuffer(const CByteBuffer&) noexcept;
    CByteBuffer(CByteBuffer&& r) noexcept;

    ~CByteBuffer();

    CByteBuffer& operator = (CByteBuffer&& r) noexcept;
    CByteBuffer& operator = (const CByteBuffer& r) noexcept;

    LPBYTE GetBuffer()  const;
    bool ResetSize(size_t nSize);
    size_t GetSize() const;

private:

    LPBYTE m_lpBuf = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
};

class CFileHelper
{
public:
    static bool GetAttributes(const _tstring& strPath, DWORD& dwAttr);
    static bool SetAttributes(const _tstring& strPath, const DWORD& dwAttr);

public:
    CFileHelper();
    CFileHelper(const CFileHelper&) = delete;
    CFileHelper(const _tstring& strPath, bool bReadonly = false);
    ~CFileHelper();

    bool Open(const _tstring& strPath, bool bCreateIfNoExist = true, bool bReadonly = false);
    void Close();
    bool IsOpen() const;
    bool GetSize(unsigned long long& ullSize) const;
    bool GetAttributes(DWORD& dwAttr) const;
    bool SetAttributes(const DWORD& dwAttr) const;
    LONGLONG GetPointer() const;
    bool SetPointer(LONGLONG llPos = 0, DWORD dwFlag = FILE_CURRENT);
    bool Read(LPVOID lpBuffer, DWORD dwSize, LONGLONG llPos = 0, LPDWORD lpBytesRead = nullptr, DWORD dwFlag = FILE_CURRENT);
    bool Write(LPCVOID lpBuffer, DWORD dwSize, LONGLONG llPos = 0, LPDWORD lpBytesWritten = nullptr, DWORD dwFlag = FILE_CURRENT);
    bool Clear();
    bool SetEnd(LONGLONG llPos);

private:
    HANDLE m_hFile;
    _tstring m_strFile;
};
