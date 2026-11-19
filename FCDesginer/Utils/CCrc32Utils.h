#pragma once

#include <wtypesbase.h>
#include <string>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CCrc32Utils
{
public:

    static _tstring GetCrc32Str(LPVOID lpData, size_t nSize, bool bUpper = false);
    static DWORD GetCrc32Value(LPVOID lpData, size_t nSize);

private:

    static void InitCrc32Table(void);
    static DWORD GetCrc32(LPVOID lpData, size_t byteSize);
    static DWORD GetCrc32Data(DWORD crcInit, LPBYTE lpData, size_t byteSize);
};