#include "CFileUtils.h"
#include <io.h>

_tstring CFileUtils::GetFileExtName(const _tstring& strfileName, bool bIncludeDot/* = false*/)
{
    _tstring strExtName = strfileName;
    size_t nExtPos = strExtName.find_last_of(_T('.'));
    if (_tstring::npos == nExtPos)
    {
        return _T("");
    }

    if (bIncludeDot)
    {
        strExtName.erase(0, nExtPos);
    }
    else
    {
        strExtName.erase(0, nExtPos + 1);
    }

    return strExtName;
}

void CFileUtils::GetFileList(
    std::vector<_tstring>& fileList, 
    const _tstring& strDir, 
    const std::vector<_tstring>& extList /* = {} */, 
    int32_t depth /*  = MAX_PATH */,
    bool bIgnoreCase/* = true*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((strDir + _T("\\*.*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;

        //非目录
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (extList.empty())
            {
                fileList.emplace_back(strDir + _T("\\") + strName);
            }
            else
            {
                //获取扩展名
                _tstring strExtName = GetFileExtName(strName, false);

                //大小写比较区分
                if (bIgnoreCase)
                {
                    //进行扩展名过滤
                    for (auto& item : extList)
                    {
                        if (0 == _tcsicmp(item.c_str(), strExtName.c_str()))
                        {
                            fileList.emplace_back(strDir + _T("\\") + strName);
                        }
                    }
                }
                else
                {
                    //进行扩展名过滤
                    for (auto& item : extList)
                    {
                        if (item == strExtName)
                        {
                            fileList.emplace_back(strDir + _T("\\") + strName);
                        }
                    }
                }
            }
            continue;
        }

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        (void)GetFileList(fileList, strDir + _T("\\") + strName, extList, depth - 1);

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

void CFileUtils::GetFileList(
    std::map<_tstring, FILEINFO>& fileList,
    const _tstring& dir,
    const std::vector<_tstring>& extList /* = {} */,
    int32_t depth /*  = MAX_PATH */,
    bool bIgnoreCase/* = true*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((dir + _T("\\*.*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;

        //非目录
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            FILEINFO info;
            info.m_fileName = findData.cFileName;
            info.m_fileSize = (unsigned long long)findData.nFileSizeHigh << 32 | findData.nFileSizeLow;
            info.m_fileDir = dir;
            info.m_filePath = dir + _T("\\") + strName;
            info.m_ftCreationTime = *(size_t*)&findData.ftCreationTime;
            info.m_ftLastAccessTime = *(size_t*)&findData.ftLastAccessTime;
            info.m_ftLastWriteTime = *(size_t*)&findData.ftLastWriteTime;
            info.m_fileAttributes = findData.dwFileAttributes;

            info.m_ftCreationTime = (info.m_ftCreationTime - 116444736000000000) / 10000000;
            info.m_ftLastAccessTime = (info.m_ftLastAccessTime - 116444736000000000) / 10000000;
            info.m_ftLastWriteTime = (info.m_ftLastWriteTime - 116444736000000000) / 10000000;

            if (extList.empty())
            {
                fileList.insert(std::make_pair(dir + _T("\\") + strName, info));
            }
            else
            {
                //获取扩展名
                _tstring strExtName = GetFileExtName(strName, false);

                //大小写比较区分
                if (bIgnoreCase)
                {
                    //进行扩展名过滤
                    for (const auto& item : extList)
                    {
                        if (0 == _tcsicmp(item.c_str(), strExtName.c_str()))
                        {
                            fileList.insert(std::make_pair(dir + _T("\\") + strName, info));
                        }
                    }
                }
                else
                {
                    //进行扩展名过滤
                    for (const auto& item : extList)
                    {
                        if (item == strExtName)
                        {
                            fileList.insert(std::make_pair(dir + _T("\\") + strName, info));
                        }
                    }
                }
            }
            continue;
        }

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        (void)GetFileList(fileList, dir + _T("\\") + strName, extList, depth - 1);

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

void CFileUtils::GetDirList(
    std::vector<_tstring>& fileList,
    const _tstring& strDir,
    int32_t depth/* = MAX_PATH*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((strDir + _T("\\*.*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        //非目录
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            continue;
        }

        fileList.emplace_back(strDir + _T("\\") + strName);
        (void)GetDirList(fileList, strDir + _T("\\") + strName, depth - 1);

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

void CFileUtils::GetDirList(
    std::map<_tstring, FILEINFO>& dirList,
    const _tstring& strDir,
    int32_t depth/* = MAX_PATH*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((strDir + _T("\\*.*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        //非目录
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            continue;
        }

        FILEINFO info;
        info.m_fileName = findData.cFileName;
        info.m_fileSize = (unsigned long long)findData.nFileSizeHigh << 32 | findData.nFileSizeLow;
        info.m_fileDir = strDir;
        info.m_filePath = strDir + _T("\\") + strName;
        info.m_ftCreationTime = *(size_t*)&findData.ftCreationTime;
        info.m_ftLastAccessTime = *(size_t*)&findData.ftLastAccessTime;
        info.m_ftLastWriteTime = *(size_t*)&findData.ftLastWriteTime;
        info.m_fileAttributes = findData.dwFileAttributes;

        info.m_ftCreationTime = (info.m_ftCreationTime - 116444736000000000) / 10000000;
        info.m_ftLastAccessTime = (info.m_ftLastAccessTime - 116444736000000000) / 10000000;
        info.m_ftLastWriteTime = (info.m_ftLastWriteTime - 116444736000000000) / 10000000;

        dirList.insert(std::make_pair(strDir + _T("\\") + strName, info));
        (void)GetDirList(dirList, strDir + _T("\\") + strName, depth - 1);

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

bool CFileUtils::IsExist(const _tstring& strPath)
{
    return 0 == _taccess_s(strPath.c_str(), 00);
}

bool CFileUtils::IsHidden(const _tstring& strPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    return attr.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;
}

bool CFileUtils::IsArchive(const _tstring& strPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    return attr.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE;
}

bool CFileUtils::IsReadOnly(const _tstring& strPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    return attr.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
}

bool CFileUtils::IsDirectory(const _tstring& strPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    return attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

bool CFileUtils::GetAttributes(const _tstring& strPath, DWORD& dwAttr)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    dwAttr = attr.dwFileAttributes;
    return true;
}

bool CFileUtils::SetAttributes(const _tstring& strPath, const DWORD& dwAttr)
{
    return ::SetFileAttributes(strPath.c_str(), dwAttr);
}

unsigned long long CFileUtils::GetFileSize(const _tstring& strPath)
{
    unsigned long long ullSize = 0;
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return 0;
    }

    ullSize = (unsigned long long)attr.nFileSizeHigh << 32 | attr.nFileSizeLow;

    return ullSize;
}

bool CFileUtils::DeleteArchive(const _tstring& strPath)
{
    bool bSuccess = false;

    if (strPath.empty())
    {
        return false;
    }

    bSuccess = ::DeleteFile(strPath.c_str());

    if (!bSuccess && ERROR_FILE_NOT_FOUND == ::GetLastError())
    {
        bSuccess = true;
    }

    return bSuccess;
}

bool CFileUtils::DeleteDir(const _tstring& strDir)
{
    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;

    if (strDir.empty())
    {
        return false;
    }

    hFindHandle = FindFirstFile((strDir + _T("\\*.*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return false;
    }

    do
    {
        _tstring strFilePath = strDir + _T("\\") + findData.cFileName;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //删除目录
        {
            DeleteDir(strFilePath);
            continue;
        }
        else    //删除文件
        {
            ::DeleteFile(strFilePath.c_str());
        }

    } while (::FindNextFile(hFindHandle, &findData));
    ::FindClose(hFindHandle);

    return ::RemoveDirectory(strDir.c_str());
}

bool CFileUtils::CreateDir(const _tstring& strPath)
{
    _tstring strDriver;              //驱动器号, 如 D:
    _tstring strSubPath = strPath;   //路径, 如 Test\1\2\3

    if (strPath.empty())
    {
        return false;
    }

    //获取盘符
    do
    {
        size_t nFindIndex = strPath.find_first_of(':');  //检查是否有驱动器号
        if (nFindIndex == _tstring::npos)
        {
            break;
        }

        strDriver = strPath.substr(0, nFindIndex + 1); //得到驱动器号, 如 D:
        nFindIndex = strPath.find(_T("\\"), nFindIndex);
        if (nFindIndex == _tstring::npos)
        {
            break;
        }

        strSubPath = strPath.substr(nFindIndex + 1); //得到路径, 如 Test\1\2\3
    } while (false);

    _tstring strDestDir;
    size_t nFindBegin = 0;
    size_t nFindIndex = 0;
    do
    {
        nFindIndex = strSubPath.find(_T("\\"), nFindBegin);
        if (nFindIndex != _tstring::npos)
        {
            strDestDir = strSubPath.substr(0, nFindIndex);
            nFindBegin = nFindIndex + 1;
        }
        else
        {
            strDestDir = strSubPath;
        }

        if (!strDriver.empty())
        {
            strDestDir = strDriver + _T("\\") + strDestDir;
        }

        if (!::CreateDirectory(strDestDir.c_str(), NULL) && ERROR_ALREADY_EXISTS != ::GetLastError())
        {
            return false;
        }

    }while (nFindIndex != _tstring::npos);

    return true;
}

CByteBuffer::CByteBuffer()
{

}

CByteBuffer::CByteBuffer(size_t nSize, BYTE nValue/* = 0xFF*/)
    :
    m_lpBuf(new (std::nothrow)BYTE[nSize])
{
    if (nullptr != m_lpBuf)
    {
        ::FillMemory(m_lpBuf, nSize, nValue);
        m_Size = nSize;
        m_Capacity = nSize;
    }
}

CByteBuffer::CByteBuffer(const CByteBuffer& r) noexcept
{
    if (ResetSize(r.m_Capacity) && nullptr != r.m_lpBuf)
    {
        memcpy_s(m_lpBuf, m_Size, r.m_lpBuf, r.m_Size);
    }
}

CByteBuffer::CByteBuffer(CByteBuffer&& r) noexcept
{
    if (nullptr != m_lpBuf)
    {
        delete[] this->m_lpBuf;
        this->m_lpBuf = nullptr;
        this->m_Size = 0;
        this->m_Capacity = 0;
    }

    if (r.m_lpBuf)
    {
        this->m_lpBuf = r.m_lpBuf;
        this->m_Size = r.m_Size;
        this->m_Capacity = r.m_Capacity;
        
        r.m_lpBuf = nullptr;
        r.m_Capacity = 0;
        r.m_Size = 0;
    }
}

CByteBuffer::~CByteBuffer()
{
    if (nullptr != m_lpBuf)
    {
        delete[] m_lpBuf;
        m_lpBuf = nullptr;
    }
}

CByteBuffer& CByteBuffer::operator = (CByteBuffer&& r) noexcept
{
    if (this == &r)
    {
        return *this;
    }

    if (nullptr != m_lpBuf)
    {
        delete[] this->m_lpBuf;
        this->m_lpBuf = nullptr;
        this->m_Size = 0;
        this->m_Capacity = 0;
    }

    if (r.m_lpBuf)
    {
        this->m_lpBuf = r.m_lpBuf;
        this->m_Size = r.m_Size;
        this->m_Capacity = r.m_Capacity;

        r.m_lpBuf = nullptr;
        r.m_Capacity = 0;
        r.m_Size = 0;
    }

    return *this;
}

CByteBuffer& CByteBuffer::operator = (const CByteBuffer& r) noexcept
{
    if (&r == this)
    {
        return *this;
    }

    if (ResetSize(r.m_Capacity) && nullptr != r.m_lpBuf)
    {
        memcpy_s(m_lpBuf, m_Size, r.m_lpBuf, r.m_Size);
    }

    return *this;
}

LPBYTE CByteBuffer::GetBuffer() const
{
    return m_lpBuf;
}

bool CByteBuffer::ResetSize(size_t nSize)
{
    if (0 == nSize)
    {
        return false;
    }

    if (nSize <= m_Capacity)
    {
        m_Size = nSize;
        return true;
    }

    if (m_lpBuf)
    {
        delete[] m_lpBuf;
        m_lpBuf = nullptr;
    }

    m_lpBuf = new (std::nothrow)BYTE[nSize];
    if (nullptr == m_lpBuf)
    {
        return false;
    }

    RtlFillMemory(m_lpBuf, nSize, 0);
    m_Size = nSize;
    m_Capacity = nSize;

    return true;
}

size_t CByteBuffer::GetSize() const
{
    return m_Size;
}

CFileHelper::CFileHelper()
    :
    m_hFile(INVALID_HANDLE_VALUE)
{

}

CFileHelper::CFileHelper(const _tstring& strPath, bool bReadonly/* = false*/)
{
    Open(strPath, true, bReadonly);
}

CFileHelper::~CFileHelper()
{
    Close();
}

bool CFileHelper::IsOpen() const
{
    return INVALID_HANDLE_VALUE != m_hFile;
}

bool CFileHelper::GetSize(unsigned long long& ullSize) const
{
    if (!IsOpen())
    {
        return false;
    }

    LARGE_INTEGER ullFileSize = { 0 };
    if (!::GetFileSizeEx(m_hFile, &ullFileSize))
    {
        return false;
    }

    ullSize = ullFileSize.QuadPart;
    return true;
}

bool CFileHelper::GetAttributes(const _tstring& strPath, DWORD& dwAttr)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    dwAttr = attr.dwFileAttributes;
    return true;
}

bool CFileHelper::SetAttributes(const _tstring& strPath, const DWORD& dwAttr)
{
    return ::SetFileAttributes(strPath.c_str(), dwAttr);
}

bool CFileHelper::GetAttributes(DWORD& dwAttr) const
{
    if (!IsOpen())
    {
        return false;
    }

    return GetAttributes(m_strFile, dwAttr);
}

bool CFileHelper::SetAttributes(const DWORD& dwAttr) const
{
    if (!IsOpen())
    {
        return false;
    }

    return SetAttributes(m_strFile, dwAttr);
}

bool CFileHelper::Open(const _tstring& strPath, bool bCreateIfNoExist/* = false*/, bool bReadonly/* = false*/)
{
    if (strPath.empty())
    {
        return false;
    }

    //不重复打开相同文件
    if (m_strFile == strPath && IsOpen())
    {
        return true;
    }

    //关闭文件, 如果已经打开
    Close();

    m_strFile = strPath;
    DWORD dwOpenFlag = GENERIC_READ;
    if (!bReadonly)
    {
        dwOpenFlag |= GENERIC_WRITE;
    }

    m_hFile = CreateFile(m_strFile.c_str(),
        dwOpenFlag,
        FILE_SHARE_READ,
        NULL,
        bCreateIfNoExist ? OPEN_ALWAYS : OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        return false;
    }

    return true;
}

void CFileHelper::Close()
{
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

LONGLONG CFileHelper::GetPointer() const
{
    LARGE_INTEGER  liDistanceToMove = { 0 };
    LARGE_INTEGER  liNewFilePointer = { 0 };
    ::SetFilePointerEx(m_hFile, liDistanceToMove, &liNewFilePointer, FILE_CURRENT);

    return liNewFilePointer.QuadPart;
}

bool CFileHelper::SetPointer(LONGLONG llPos/* = 0*/, DWORD dwFlag/* = FILE_CURRENT*/)
{
    LARGE_INTEGER  liDistanceToMove = { 0 };

    if (!IsOpen())
    {
        return false;
    }

    liDistanceToMove.QuadPart = llPos;
    return ::SetFilePointerEx(m_hFile, liDistanceToMove, NULL, dwFlag);
}

bool CFileHelper::Read(LPVOID lpBuffer, DWORD dwSize, LONGLONG llPos/* = 0*/, LPDWORD lpBytesRead/* = nullptr*/, DWORD dwFlag/* = FILE_CURRENT*/)
{
    if (!IsOpen() || nullptr == lpBuffer)
    {
        return false;
    }

    LARGE_INTEGER  liDistanceToMove = { 0 };
    BOOL bResult = FALSE;
    liDistanceToMove.QuadPart = llPos;
    ::SetFilePointerEx(m_hFile, liDistanceToMove, NULL, dwFlag);
    DWORD dwBytesRead = 0;

    bResult = ::ReadFile(m_hFile, lpBuffer, dwSize, &dwBytesRead, NULL);
    if (nullptr != lpBytesRead)
    {
        *lpBytesRead = dwBytesRead;
    }

    return bResult;
}

bool CFileHelper::Write(LPCVOID lpBuffer, DWORD dwSize, LONGLONG llPos/* = 0*/, LPDWORD lpBytesWritten/* = nullptr*/, DWORD dwFlag/* = FILE_CURRENT*/)
{
    if (!IsOpen() || nullptr == lpBuffer)
    {
        return false;
    }

    LARGE_INTEGER  liDistanceToMove = { 0 };
    BOOL bResult = FALSE;
    liDistanceToMove.QuadPart = llPos;
    ::SetFilePointerEx(m_hFile, liDistanceToMove, NULL, dwFlag);

    DWORD dwBytesWritten = 0;
    bResult = ::WriteFile(m_hFile, lpBuffer, dwSize, &dwBytesWritten, NULL);

    if (nullptr != lpBytesWritten)
    {
        *lpBytesWritten = dwBytesWritten;
    }

    return bResult;
}

bool CFileHelper::Clear()
{
    if (!IsOpen())
    {
        return false;
    }

    LARGE_INTEGER  liDistanceToMove = { 0 };
    ::SetFilePointerEx(m_hFile, liDistanceToMove, NULL, FILE_BEGIN);
    return ::SetEndOfFile(m_hFile);
}

bool CFileHelper::SetEnd(LONGLONG llPos)
{
    if (!IsOpen())
    {
        return false;
    }

    LARGE_INTEGER  liDistanceToMove = { 0 };
    liDistanceToMove.QuadPart = llPos;
    ::SetFilePointerEx(m_hFile, liDistanceToMove, NULL, FILE_BEGIN);
    return ::SetEndOfFile(m_hFile);
}