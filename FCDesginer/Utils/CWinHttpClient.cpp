#include "CWinHttpClient.h"
#include <strsafe.h>
#include <thread>
#include <atomic>
#include <map>

std::map<int32_t, std::string> g_utf8Code;
static std::map<int32_t, std::string> GetUtf8Code();

typedef struct _WINHTTP_URL_INFO
{
    std::wstring strScheme;
    std::wstring strHostName;
    std::wstring strUserName;
    std::wstring strPassword;
    std::wstring urlPath;
    std::wstring strExtraInfo;

    URL_COMPONENTS uc = { 0 };

    _WINHTTP_URL_INFO()
    {
        memset(&uc, 0, sizeof(uc));

        try
        {
            strScheme.resize(32);
            strHostName.resize(128);
            strUserName.resize(128);
            strPassword.resize(128);
            urlPath.resize(2048);
            strExtraInfo.resize(512);

            this->uc.dwStructSize = sizeof(this->uc);
            this->uc.lpszScheme = &this->strScheme[0];
            this->uc.dwSchemeLength = (DWORD)strScheme.size();
            this->uc.lpszHostName = &this->strHostName[0];
            this->uc.dwHostNameLength = (DWORD)strHostName.size();
            this->uc.lpszUserName = &this->strUserName[0];
            this->uc.dwUserNameLength = (DWORD)strUserName.size();
            this->uc.lpszPassword = &this->strPassword[0];
            this->uc.dwPasswordLength = (DWORD)strPassword.size();
            this->uc.lpszUrlPath = &this->urlPath[0];
            this->uc.dwUrlPathLength = (DWORD)urlPath.size();
            this->uc.lpszExtraInfo = &this->strExtraInfo[0];
            this->uc.dwExtraInfoLength = (DWORD)strExtraInfo.size();
        }
        catch (...)
        {

        }
    }
}WINHTTP_URL_INFO, *PWINHTTP_URL_INFO;



CWinHttpValue::CWinHttpValue()
    :
    m_Data{ 0 },
    m_Type(_WIN_HTTP_VALUE_TPYE::TypeNull)
{
}

CWinHttpValue::CWinHttpValue(bool val) : CWinHttpValue()
{
    m_Data._Bool = val;
    m_Type = _WIN_HTTP_VALUE_TPYE::TypeBool;
}

CWinHttpValue::CWinHttpValue(int val) : CWinHttpValue()
{
    m_Data._Int = val;
    m_Type = _WIN_HTTP_VALUE_TPYE::TypeInt;
}

CWinHttpValue::CWinHttpValue(double val) : CWinHttpValue()
{
    m_Data._Float = val;
    m_Type = _WIN_HTTP_VALUE_TPYE::TypeFloat;
}

CWinHttpValue::CWinHttpValue(const LPCTSTR val) : CWinHttpValue()
{
    if (val)
    {
        m_Data._StringPtr = new (std::nothrow) _tstring(val);
    }

    m_Type = _WIN_HTTP_VALUE_TPYE::TpyeString;
}

CWinHttpValue::CWinHttpValue(const _tstring& val) : CWinHttpValue()
{
    m_Data._StringPtr = new (std::nothrow) _tstring(val);
    m_Type = _WIN_HTTP_VALUE_TPYE::TpyeString;
}

CWinHttpValue::CWinHttpValue(const CWinHttpObject& val) : CWinHttpValue()
{
    m_Data._ObjectPtr = new (std::nothrow) CWinHttpObject(val);
    m_Type = _WIN_HTTP_VALUE_TPYE::TpyeObject;
}

CWinHttpValue::CWinHttpValue(const CWinHttpArray& val) : CWinHttpValue()
{
    m_Data._ArrayPtr = new (std::nothrow) CWinHttpArray(val);
    m_Type = _WIN_HTTP_VALUE_TPYE::TpyeArray;
}

CWinHttpValue::CWinHttpValue(const CWinHttpValue& val) : CWinHttpValue()
{
    if (_WIN_HTTP_VALUE_TPYE::TpyeString == val.m_Type && val.m_Data._StringPtr)
    {
        m_Data._StringPtr = new (std::nothrow) _tstring(*val.m_Data._StringPtr);
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeObject == val.m_Type && val.m_Data._ObjectPtr)
    {
        m_Data._ObjectPtr = new (std::nothrow) CWinHttpObject(*val.m_Data._ObjectPtr);
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeArray == val.m_Type && val.m_Data._ArrayPtr)
    {
        m_Data._ArrayPtr = new (std::nothrow) CWinHttpArray(*val.m_Data._ArrayPtr);
    }


    m_Type = val.m_Type;
}

CWinHttpValue::CWinHttpValue(CWinHttpValue&& val) noexcept
{
    m_Data = val.m_Data;
    m_Type = val.m_Type;
    val.m_Data = { 0 };
    val.m_Type = _WIN_HTTP_VALUE_TPYE::TypeBool;
}

CWinHttpValue::~CWinHttpValue()
{
    if (_WIN_HTTP_VALUE_TPYE::TpyeString == m_Type)
    {
        delete m_Data._StringPtr;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeObject == m_Type)
    {
        delete m_Data._ObjectPtr;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeArray == m_Type)
    {
        delete m_Data._ArrayPtr;
    }

    m_Data = { 0 };
    m_Type = _WIN_HTTP_VALUE_TPYE::TypeBool;
}

bool CWinHttpValue::IsEmpty() const
{
    if (_WIN_HTTP_VALUE_TPYE::TpyeObject == m_Type && m_Data._ObjectPtr)
    {
        return m_Data._ObjectPtr->empty();
    }

    return false;
}

_tstring CWinHttpValue::AsGetString() const
{
    _tstring strResult;

    if (IsEmpty())
    {
        return strResult;
    }

    return _AsGetString();
}

_tstring CWinHttpValue::AsJsonString() const
{
    return _AsJsonString();
}

_tstring CWinHttpValue::AsHeaderString() const
{
    return _AsHeaderString(0);
}

bool CWinHttpValue::_GetUnicodeString(_tstring& append_str, const TCHAR* data_ptr, const TCHAR** end_ptr)
{
    TCHAR ch = *data_ptr;

#ifdef _UNICODE
    TCHAR text_buffer[32] = { 0 };
    _stprintf_s(text_buffer, _countof(text_buffer), _T(R"(\u%.4x)"), ch);
    append_str += text_buffer;
    data_ptr++;
#else
    if (ch >= 0xC0)
    {
        // The number of bytes used to obtain characters.
        size_t byte_count = 0;
        uint32_t cp32 = 0;

        if (ch >= 0xE0 && ch <= 0xEF)
        {
            byte_count = 3;
            cp32 = ch & 0x0F;
        }
        else if (ch >= 0xC0 && ch <= 0xDF)
        {
            byte_count = 2;
            cp32 = ch & 0x1F;
        }
        else if (ch >= 0xF0 && ch <= 0xF7)
        {
            byte_count = 4;
            cp32 = ch & 0x07;
        }
        else if (ch >= 0xF8 && ch <= 0xFB)
        {
            byte_count = 5;
            cp32 = ch & 0x03;
        }
        else if (ch >= 0xFC && ch <= 0xFD)
        {
            byte_count = 6;
            cp32 = ch & 0x01;
        }

        if (0 == byte_count)
        {
            return false;
        }

        for (size_t i = 1; i < byte_count; i++)
        {
            cp32 = cp32 << 6;
            cp32 |= data_ptr[i] & 0x3F;
        }

        char text_buffer[32] = { 0 };
        if (cp32 < 0x10000)
        {
            snprintf(text_buffer, sizeof(text_buffer), R"(\u%.4x)", cp32);
            append_str.append(text_buffer, 6);
        }
        else
        {
            uint32_t cp = (uint16_t)(cp32 - 0x10000);
            uint16_t cp32_high = (uint16_t)(cp >> 10) + 0xD800;
            uint16_t cp32_low = (uint16_t)(cp & 0x3FF) + 0xDC00;

            snprintf(text_buffer, sizeof(text_buffer), R"(\u%.4x\u%.4x)", cp32_high, cp32_low);
            append_str.append(text_buffer, 12);
        }

        data_ptr += byte_count;
    }
#endif

    if (end_ptr)
    {
        *end_ptr = data_ptr;
    }

    return false;
}

void CWinHttpValue::_DumpString(_tstring& append_str, const _tstring& text, bool flag_escape)
{
    const TCHAR* data_ptr = text.c_str();

    while (_T('\0') != *data_ptr)
    {
        TCHAR ch = *data_ptr;

        switch (ch)
        {
        case _T('\b'):
        {
            append_str += _T(R"(\b)");
        }
        break;
        case _T('\t'):
        {
            append_str += _T(R"(\t)");
        }
        break;
        case _T('\n'):
        {
            append_str += _T(R"(\n)");
        }
        break;
        case _T('\f'):
        {
            append_str += _T(R"(\f)");
        }
        break;
        case _T('\r'):
        {
            append_str += _T(R"(\r)");
        }
        break;
        case _T('\"'):
        {
            append_str += _T(R"(\")");
        }
        break;
        case _T('/'):
        {
            append_str += _T(R"(/)");
        }
        case _T('\\'):
        {
            append_str += _T(R"(\\)");
        }
        break;
        default:
        {
            if (ch < 0x80 || !flag_escape)
            {
                append_str.push_back(ch);
            }
            else
            {
                _GetUnicodeString(append_str, data_ptr, &data_ptr);
                continue;
            }
        }
        break;
        }

        data_ptr++;
    }
}

_tstring CWinHttpValue::_AsGetString() const
{
    _tstring strResult;
    if (_WIN_HTTP_VALUE_TPYE::TypeBool == m_Type)
    {
        strResult = m_Data._Bool ? _T("true") : _T("false");
    }
    else if (_WIN_HTTP_VALUE_TPYE::TypeInt == m_Type)
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("%d"), m_Data._Int);
        strResult = szBuf;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TypeFloat == m_Type)
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("%.16g"), m_Data._Float);
        strResult = szBuf;

        TCHAR* chPtr = szBuf;
        bool flag_dot = false;
        bool flag_exponent = false;

        while (_T('\0') != *chPtr)
        {
            TCHAR ch = *chPtr;
            if (_T('.') == ch)
            {
                flag_dot = true;
            }
            else if (_T('e') == ch)
            {
                flag_exponent = true;
            }
            chPtr++;
        }

        if (!flag_dot && 0 == !flag_exponent)
        {
            strResult += _T(".0");
        }
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeString == m_Type)
    {
        strResult = *m_Data._StringPtr;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeObject == m_Type)
    {
        const CWinHttpObject& object = *m_Data._ObjectPtr;
        size_t size = object.size();
        if (!object.empty())
        {
            strResult += _T("?");
            for (const auto& item : object)
            {
                strResult += item.first;
                strResult += _T("=");
                strResult += item.second._AsGetString();
                size--;
                if (size > 0)
                {
                    strResult += _T("&");
                }
            }
        }
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeArray == m_Type)
    {
        const CWinHttpArray& object = *m_Data._ArrayPtr;
        size_t size = object.size();
        for (const auto& item : object)
        {
            strResult += item._AsGetString();
            size--;
            if (size > 0)
            {
                strResult += _T(",");
            }
        }
    }

    return strResult;
}

_tstring CWinHttpValue::_AsJsonString() const
{
    _tstring strResult;
    if (_WIN_HTTP_VALUE_TPYE::TypeBool == m_Type)
    {
        strResult = m_Data._Bool ? _T("true") : _T("false");
    }
    else if (_WIN_HTTP_VALUE_TPYE::TypeInt == m_Type)
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("%d"), m_Data._Int);
        strResult = szBuf;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TypeFloat == m_Type)
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("%.16g"), m_Data._Float);
        strResult = szBuf;

        TCHAR* chPtr = szBuf;
        bool flag_dot = false;
        bool flag_exponent = false;

        while (_T('\0') != *chPtr)
        {
            TCHAR ch = *chPtr;
            if (_T('.') == ch)
            {
                flag_dot = true;
            }
            else if (_T('e') == ch)
            {
                flag_exponent = true;
            }
            chPtr++;
        }

        if (!flag_dot && 0 == !flag_exponent)
        {
            strResult += _T(".0");
        }
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeString == m_Type)
    {
        strResult += _T("\"");
        _DumpString(strResult, *m_Data._StringPtr, false);
        strResult += _T("\"");
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeObject == m_Type)
    {
        const CWinHttpObject& object = *m_Data._ObjectPtr;
        size_t size = object.size();
        if (!object.empty())
        {
            strResult += _T("{");
            for (const auto& item : object)
            {
                strResult += _T("\"");
                _DumpString(strResult, item.first, false);
                strResult += _T("\"");
                strResult += _T(":");
                strResult += item.second._AsJsonString();
                size--;
                if (size > 0)
                {
                    strResult += _T(",");
                }
            }
            strResult += _T("}");
        }
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeArray == m_Type)
    {
        const CWinHttpArray& object = *m_Data._ArrayPtr;
        size_t size = object.size();
        strResult += _T("[");
        for (const auto& item : object)
        {
            strResult += item._AsJsonString();
            size--;
            if (size > 0)
            {
                strResult += _T(",");
            }
        }
        strResult += _T("]");
    }

    return strResult;
}

_tstring CWinHttpValue::_AsHeaderString(int depth) const
{
    _tstring strResult;
    if (_WIN_HTTP_VALUE_TPYE::TypeBool == m_Type)
    {
        strResult = m_Data._Bool ? _T("true") : _T("false");
    }
    else if (_WIN_HTTP_VALUE_TPYE::TypeInt == m_Type)
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("%d"), m_Data._Int);
        strResult = szBuf;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TypeFloat == m_Type)
    {
        TCHAR szBuf[MAX_PATH] = { 0 };
        ::StringCchPrintf(szBuf, _countof(szBuf), _T("%.16g"), m_Data._Float);
        strResult = szBuf;

        TCHAR* chPtr = szBuf;
        bool flag_dot = false;
        bool flag_exponent = false;

        while (_T('\0') != *chPtr)
        {
            TCHAR ch = *chPtr;
            if (_T('.') == ch)
            {
                flag_dot = true;
            }
            else if (_T('e') == ch)
            {
                flag_exponent = true;
            }
            chPtr++;
        }

        if (!flag_dot && 0 == !flag_exponent)
        {
            strResult += _T(".0");
        }
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeString == m_Type)
    {
        strResult += *m_Data._StringPtr;
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeObject == m_Type)
    {
        const CWinHttpObject& object = *m_Data._ObjectPtr;
        size_t size = object.size();
        if (!object.empty())
        {
            for (const auto& item : object)
            {
                strResult += item.first;

                strResult += _T(": ");
                strResult += item.second._AsHeaderString(depth + 1);
                size--;
                if (size > 0)
                {
                    strResult += _T("\r\n");
                }
            }
        }
    }
    else if (_WIN_HTTP_VALUE_TPYE::TpyeArray == m_Type)
    {
        const CWinHttpArray& object = *m_Data._ArrayPtr;
        size_t size = object.size();
        for (const auto& item : object)
        {
            strResult += item._AsHeaderString(depth + 1);
            size--;
            if (size > 0)
            {
                strResult += _T(", ");
            }
        }
    }

    return strResult;
}

CWinHttpClient::CWinHttpClient()
    :
    m_hSession(NULL),
    m_hConnect(NULL),
    m_hRequest(NULL),
    m_fAbort(false),
    m_fPrint(false),
    m_ullStart(0),
    m_ullLength(-1),
    m_ullDownload(0),
    m_AsyncData{ 0 }
{
    m_AsyncData.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    return;
}

CWinHttpClient::~CWinHttpClient()
{
    Close();

    if (NULL != m_AsyncData.hEvent)
    {
        ::CloseHandle(m_AsyncData.hEvent);
    }
}

void CWinHttpClient::Close()
{
    m_fAbort = true;

    if (NULL != m_AsyncData.hEvent)
    {
        ::SetEvent(m_AsyncData.hEvent);
    }

    if (m_hRequest)
    {
        ::WinHttpCloseHandle(m_hRequest);
        m_hRequest = NULL;
    }

    if (m_hConnect)
    {
        ::WinHttpCloseHandle(m_hConnect);
        m_hConnect = NULL;
    }

    if (m_hSession)
    {
        ::WinHttpCloseHandle(m_hSession);
        m_hSession = NULL;
    }

    m_fAbort = false;
}

std::string FormatA(LPCSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::string strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //成功则赋值字符串并终止循环
        int nSize = _vsnprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            strResult.resize(nSize);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= INT32_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);

    return strResult;
}

std::map<int32_t, std::string> GetUtf8Code()
{
    std::map<int32_t, std::string> mapCode;

    uint8_t szBuf[MAX_PATH] = { 0 };
    for (uint32_t i = 0x4E00; i <= 0x9FFF; i++)
    {
        // 1字节
        // 0xxxxxxx
        if (i >= 0x00000000 && i <= 0x0000007F)
        {
            szBuf[0] = i;
            szBuf[1] = 0;
        }

        // 2字节
        // 110xxxxx 10xxxxxx
        if (i >= 0x00000080 && i <= 0x000007FF)
        {
            szBuf[0] = ((i >> 6) & 0x1F) | 0xC0;
            szBuf[1] = ( i & 0x3F) | 0x80;
            szBuf[2] = 0;
        }

        // 3字节
        // 1110xxxx 10xxxxxx 10xxxxxx
        if (i >= 0x00000800 && i <= 0x0000FFFF)
        {
            szBuf[0] = ((i >> 12) & 0x0F) | 0xE0;
            szBuf[1] = ((i >> 6) & 0x3F) | 0x80;
            szBuf[2] = (i & 0x3F) | 0x80;
            szBuf[3] = 0;
        }

        mapCode.insert(std::make_pair(i, (char*)szBuf));
    }

    return mapCode;
}

bool IsUtf8String(const std::string& strContent)
{
    bool fResult = false;
    size_t nLength = strContent.size();
    const char* pStr = strContent.c_str();
    bool fAscii = true;
    int nBytes = 0;

    for (int i = 0; i < nLength; i++)
    {
        char ch = pStr[i];

        if (0 != (ch & 0x80))
        {
            fAscii = false;
        }

        if (ch >= 0x80)
        {
            // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            if (ch >= 0xFC && ch <= 0xFD)
            {
                nBytes = 6;
            }
            // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            else if (ch >= 0xF8 && ch <= 0xFB)
            {
                nBytes = 5;
            }
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            else if (ch >= 0xF0 && ch <= 0xF7)
            {
                nBytes = 4;
            }
            // 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            else if (ch >= 0xE0 && ch <= 0xEF)
            {
                nBytes = 3;
            }
            // 110xxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            else if (ch >= 0xC0 && ch <= 0xDF)
            {
                nBytes = 2;
            }
            else
            {
                return FALSE;
            }

            nBytes--;
        }
    }

    return fResult;
}
std::string CWinHttpClient::EncoderFromUtf8(const std::string& strContent)
{
    std::string strResult;



    return strResult;
}

std::string CWinHttpClient::DecoderFromUtf8(const std::string& strContent)
{
    std::string strResult;
    strResult.reserve(strContent.capacity());

    size_t nLength = strContent.size();
    const char* pStr = strContent.c_str();

    for (int i = 0; i < nLength; i++)
    {
        // 转义解码
        if (('\\' == pStr[i]))
        {
            // Unicode 字符
            if ((i + 5) < nLength && 'u' == pStr[i + 1])
            {
                char szCode[8] = { 0 };
                szCode[0] = pStr[i + 2];
                szCode[1] = pStr[i + 3];
                szCode[2] = pStr[i + 4];
                szCode[3] = pStr[i + 5];
                uint32_t uCode = _strtoui64(szCode, nullptr, 16);

                // 1字节
                // 0xxxxxxx
                if (uCode >= 0x00000000 && uCode <= 0x0000007F)
                {
                    szCode[0] = uCode;
                    szCode[1] = 0;
                }

                // 2字节
                // 110xxxxx 10xxxxxx
                if (uCode >= 0x00000080 && uCode <= 0x000007FF)
                {
                    szCode[0] = ((uCode >> 6) & 0x1F) | 0xC0;
                    szCode[1] = ( uCode & 0x3F) | 0x80;
                    szCode[2] = 0;
                }

                // 3字节
                // 1110xxxx 10xxxxxx 10xxxxxx
                if (uCode >= 0x00000800 && uCode <= 0x0000FFFF)
                {
                    szCode[0] = ((uCode >> 12) & 0x0F) | 0xE0;
                    szCode[1] = ((uCode >> 6) & 0x3F) | 0x80;
                    szCode[2] = (uCode & 0x3F) | 0x80;
                    szCode[3] = 0;
                }

                // 4字节
                // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                if (uCode >= 0x00010000 && uCode <= 0x001FFFFF)
                {
                    szCode[0] = ((uCode >> 18) & 0x07) | 0xF0;
                    szCode[1] = ((uCode >> 12) & 0x3F) | 0x80;
                    szCode[2] = ((uCode >> 6) & 0x3F) | 0x80;
                    szCode[3] = (uCode & 0x3F) | 0x80;
                    szCode[4] = 0;
                }

                // 5字节
                // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                if (uCode >= 0x00200000 && uCode <= 0x03FFFFFF)
                {
                    szCode[0] = ((uCode >> 24) & 0x03) | 0xF8;
                    szCode[1] = ((uCode >> 18) & 0x3F) | 0x80;
                    szCode[2] = ((uCode >> 12) & 0x3F) | 0x80;
                    szCode[3] = ((uCode >> 6) & 0x3F) | 0x80;
                    szCode[4] = (uCode & 0x3F) | 0x80;
                    szCode[5] = 0;
                }

                // 6字节
                // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                if (uCode >= 0x04000000 && uCode <= 0x7FFFFFFF)
                {
                    szCode[0] = ((uCode >> 30) & 0x01) | 0xFC;
                    szCode[1] = ((uCode >> 24) & 0x3F) | 0x80;
                    szCode[2] = ((uCode >> 18) & 0x3F) | 0x80;
                    szCode[3] = ((uCode >> 12) & 0x3F) | 0x80;
                    szCode[4] = ((uCode >> 6) & 0x3F) | 0x80;
                    szCode[5] = (uCode & 0x3F) | 0x80;
                    szCode[6] = 0;
                }

                strResult += szCode;

                i += 5;
            }

            // 转义字符
            switch (pStr[i + 1])
            {
            case '\"':
            {
                strResult.push_back('\"');
                i += 1;
            }
            break;
            case '/':
            {
                strResult.push_back('\"');
                i += 1;
            }
            break;
            case 'b':
            {
                strResult.push_back('\b');
                i += 1;
            }
            break;
            case 'f':
            {
                strResult.push_back('\f');
                i += 1;
            }
            break;
            case 't':
            {
                strResult.push_back('\t');
                i += 1;
            }
            break;
            case 'r':
            {
                strResult.push_back('\r');
                i += 1;
            }
            break;
            case 'n':
            {
                strResult.push_back('\n');
                i += 1;
            }
            break;
            case '<':
            {
                strResult.push_back('<');
                i += 1;
            }
            break;
            case '>':
            {
                strResult.push_back('>');
                i += 1;
            }
            break;
            case '&':
            {
                strResult.push_back('&');
                i += 1;
            }
            break;
            }
        }
        else
        {
            strResult.push_back(pStr[i]);
        }
    }

    strResult.push_back('\0');

    return strResult;
}

static bool _CrackUrl(const _tstring& url, PWINHTTP_URL_INFO lpUrlInfo)
{
    // 将 URL 分解到其组件部件中
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpcrackurl
    // 即使在异步模式下使用 WinHTTP (即在 WinHttpOpen) 中设置了WINHTTP_FLAG_ASYNC时，此函数也会同步运行。
    if (!::WinHttpCrackUrl(CWinHttpClient::TStrToWStr(url).c_str(), 0, 0, &lpUrlInfo->uc))
    {
        return false;
    }

    lpUrlInfo->strScheme.resize(wcslen(lpUrlInfo->strScheme.c_str()));
    lpUrlInfo->strExtraInfo.resize(wcslen(lpUrlInfo->strExtraInfo.c_str()));
    lpUrlInfo->strHostName.resize(wcslen(lpUrlInfo->strHostName.c_str()));
    lpUrlInfo->strUserName.resize(wcslen(lpUrlInfo->strUserName.c_str()));
    lpUrlInfo->strPassword.resize(wcslen(lpUrlInfo->strPassword.c_str()));
    lpUrlInfo->urlPath.resize(wcslen(lpUrlInfo->urlPath.c_str()));

    if (!lpUrlInfo->strExtraInfo.empty())
    {
        lpUrlInfo->urlPath += lpUrlInfo->strExtraInfo;
    }

    // 协议检查
    if (!(INTERNET_SCHEME_HTTPS == lpUrlInfo->uc.nScheme || INTERNET_SCHEME_HTTP == lpUrlInfo->uc.nScheme))
    {
        return false;
    }

    return true;
}

CWinHttpResult CWinHttpClient::Get(const _tstring& url, const CWinHttpValue& param/* = {}*/, const CWinHttpValue& header/* = {}*/, WinHttpCallback cb/* = nullptr*/)
{
    m_strFilePath.clear();
    m_cbProgress = cb;

    m_strUrl = url;
    m_strParam = param.AsGetString();
    m_strHeader = header.AsHeaderString();

    return _Get(url, cb);
}

CWinHttpResult CWinHttpClient::Post(const _tstring& url, const CWinHttpValue& param/* = {}*/, const CWinHttpValue& header/* = {}*/, WinHttpCallback cb/* = nullptr*/)
{
    m_strFilePath.clear();
    m_cbProgress = cb;

    m_strUrl = url;
    m_strParam = param.AsJsonString();
    m_strHeader = header.AsHeaderString();

    return _Post(url, cb);
}

CWinHttpResult CWinHttpClient::GetEx(const _tstring& url, const CWinHttpValue& param/* = {}*/, const CWinHttpValue& header/* = {}*/, WinHttpCallback cb/* = nullptr*/, DWORD dwThreadCount/* = 1*/)
{
    m_strFilePath.clear();
    m_cbProgress = cb;

    m_strUrl = url;
    m_strParam = param.AsJsonString();
    m_strHeader = header.AsHeaderString();

    return _GetEx(url, cb, dwThreadCount);
}

CWinHttpResult CWinHttpClient::DownloadFile(const _tstring& url, const _tstring& strFile, const CWinHttpValue& header/* = {}*/, WinHttpCallback cb/* = nullptr*/, DWORD dwThreadCount/* = 4*/)
{
    m_strFilePath = strFile;

    m_strUrl = url;
    m_strParam.clear();
    m_strHeader = header.AsHeaderString();

    if (m_strFilePath.empty())
    {
        size_t nPos = url.find_last_of(_T("/"));
        if (_tstring::npos != nPos)
        {
            m_strFilePath = url.substr(nPos + 1);
        }
    }

    // 清空文件
    HANDLE hFile = INVALID_HANDLE_VALUE;
    hFile = ::CreateFile(strFile.c_str(),
        GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,
        NULL,
        TRUNCATE_EXISTING,
        FILE_ATTRIBUTE_ARCHIVE,
        NULL);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }

    CWinHttpResult httpResult = _GetEx(url, cb, dwThreadCount);

    return httpResult;
}

CWinHttpResult CWinHttpClient::_Get(const _tstring& url, WinHttpCallback cb/* = nullptr*/)
{
    _tstring urlAddr = url;

    if (!m_strParam.empty())
    {
        urlAddr += m_strParam;
    }

    return _DoRequest(urlAddr, _T("GET"), "");
}

CWinHttpResult CWinHttpClient::_Post(const _tstring& url, WinHttpCallback cb/* = nullptr*/)
{
    std::string strParam = TStrToU8Str(m_strParam);
    return _DoRequest(url, _T("POST"), strParam);
}

CWinHttpResult CWinHttpClient::_DoRequest(const _tstring& url, const _tstring& strMethod, const std::string& strParam)
{
    WINHTTP_URL_INFO urlInfo;
    CWinHttpResult httpResult;

    if (!_CrackUrl(url, &urlInfo))
    {
        _PrintError(_T("_CrackUrl"));
        return httpResult;
    }

    do
    {
        // 初始化 HTTP 会话
        std::wstring wstrAgent = TStrToWStr(m_strAgent);
        DWORD dwAccessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
        LPCWSTR pszProxy = WINHTTP_NO_PROXY_NAME;
        LPCWSTR pszProxyBypass = WINHTTP_NO_PROXY_BYPASS;
        DWORD dwSessionFlags = WINHTTP_FLAG_ASYNC;    //WINHTTP_FLAG_ASYNC / WINHTTP_FLAG_SECURE_DEFAULTS
        if (!_WinHttpOpen(wstrAgent.c_str(), dwAccessType, pszProxy, pszProxyBypass, dwSessionFlags))
        {
            _PrintError(_T("_WinHttpOpen"));
            break;
        }

        // 设置回调函数
        if (!_WinHttpSetStatusCallback(WinHttpStatusCallback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS))
        {
            _PrintError(_T("WinHttpSetStatusCallback"));
            break;
        }

        // 设置状态回调上下文
        DWORD_PTR dwContext = (DWORD_PTR)this;
        if (!_WinHttpSetSessionOption(WINHTTP_OPTION_CONTEXT_VALUE, &dwContext, sizeof(dwContext)))
        {
            _PrintError(_T("WinHttpSetOption"));
            break;
        }

        // 连接 HTTP 会话
        if (!_WinHttpConnect(urlInfo.strHostName.c_str(), urlInfo.uc.nPort))
        {
            _PrintError(_T("_WinHttpConnect"));
            break;
        }

        // 创建 HTTP 请求句柄
        LPCWSTR pwszReferrer = WINHTTP_NO_REFERER;
        LPCWSTR* pwszAcceptTypes = WINHTTP_DEFAULT_ACCEPT_TYPES;
        DWORD dwRequestFlags = (INTERNET_SCHEME_HTTPS == urlInfo.uc.nScheme) ? WINHTTP_FLAG_SECURE : 0;
        if (!_WinHttpOpenRequest(strMethod, urlInfo.urlPath.c_str(), NULL, pwszReferrer, pwszAcceptTypes, dwRequestFlags))
        {
            _PrintError(_T("_WinHttpOpenRequest"));
            break;
        }

        // 设置请求超时
        if (!_WinHttpSetRequestTimeouts(500, 500, 1000, 1000))
        {
            _PrintError(_T("WinHttpSetTimeouts"));
            break;
        }

        // 设置请求范围
        if (!_SetRequestDataRange(m_ullStart, m_ullLength))
        {
            _PrintError(_T("_SetRequestDataRange"));
            break;
        }

        // 发送请求
        if (!_WinHttpSendRequest(m_strHeader, (LPVOID)strParam.data(), (DWORD)strParam.size(), (DWORD_PTR)this))
        {
            _PrintError(_T("_WinHttpSendRequest"));
            break;
        }

        // 等待接收请求响应
        if (!_WinHttpReceiveResponse(m_hRequest))
        {
            _PrintError(_T("_WinHttpReceiveResponse"));
            break;
        }

        // 查询文件大小
        ULONGLONG UllContentLength = 0;
        if (!_QueryContentLength(&UllContentLength))
        {
            _PrintError(_T("_QueryContentLength"));
            break;
        }

        // 获取响应码
        httpResult.code = _GetStatusCode(m_hRequest);
        if (httpResult.code < 200 || httpResult.code >= 300)
        {
            _PrintError(_T("_GetStatusCode"));
            break;
        }

        // 接收请求数据
        if (!_InternetReadData(httpResult.result))
        {
            _PrintError(_T("_InternetReadData"));
            break;
        }

    } while (false);

    // 关闭相关句柄
    Close();

    return httpResult;
}

CWinHttpResult CWinHttpClient::_GetEx(const _tstring& url, WinHttpCallback cb/* = nullptr*/, DWORD dwThreadCount/* = 6*/)
{
    WINHTTP_URL_INFO urlInfo;
    CWinHttpResult httpResult;

    if (!_CrackUrl(url, &urlInfo))
    {
        _PrintError(_T("_CrackUrl"));
        return httpResult;
    }

    m_cbProgress = cb;

    do
    {
        // 初始化 HTTP 会话
        std::wstring wstrAgent = TStrToWStr(m_strAgent);
        DWORD dwAccessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
        LPCWSTR pszProxy = WINHTTP_NO_PROXY_NAME;
        LPCWSTR pszProxyBypass = WINHTTP_NO_PROXY_BYPASS;
        DWORD dwSessionFlags = WINHTTP_FLAG_ASYNC;    //WINHTTP_FLAG_ASYNC / WINHTTP_FLAG_SECURE_DEFAULTS
        if (!_WinHttpOpen(wstrAgent.c_str(), dwAccessType, pszProxy, pszProxyBypass, dwSessionFlags))
        {
            _PrintError(_T("_WinHttpOpen"));
            break;
        }

        // 设置回调函数
        if (!_WinHttpSetStatusCallback(WinHttpStatusCallback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS))
        {
            _PrintError(_T("WinHttpSetStatusCallback"));
            break;
        }

        // 设置状态回调上下文
        DWORD_PTR dwContext = (DWORD_PTR)this;
        if (!_WinHttpSetSessionOption(WINHTTP_OPTION_CONTEXT_VALUE, &dwContext, sizeof(dwContext)))
        {
            _PrintError(_T("WinHttpSetOption"));
            break;
        }

        // 连接 HTTP 会话
        if (!_WinHttpConnect(urlInfo.strHostName.c_str(), urlInfo.uc.nPort))
        {
            _PrintError(_T("_WinHttpConnect"));
            break;
        }

        // 创建 HTTP 请求句柄
        LPCWSTR pwszReferrer = WINHTTP_NO_REFERER;
        LPCWSTR* pwszAcceptTypes = WINHTTP_DEFAULT_ACCEPT_TYPES;
        DWORD dwRequestFlags = (INTERNET_SCHEME_HTTPS == urlInfo.uc.nScheme) ? WINHTTP_FLAG_SECURE : 0;
        if (!_WinHttpOpenRequest(_T("GET"), urlInfo.urlPath.c_str(), NULL, pwszReferrer, pwszAcceptTypes, dwRequestFlags))
        {
            _PrintError(_T("_WinHttpOpenRequest"));
            break;
        }

        // 设置请求超时
        if (!_WinHttpSetRequestTimeouts(1000, 1000, 3000, 5000))
        {
            _PrintError(_T("WinHttpSetTimeouts"));
            break;
        }

        // 发送请求
        if (!_WinHttpSendRequest(m_strHeader, NULL, 0, (DWORD_PTR)this))
        {
            _PrintError(_T("_WinHttpSendRequest"));
            break;
        }

        // 等待接收请求响应
        if (!_WinHttpReceiveResponse(m_hRequest))
        {
            _PrintError(_T("_WinHttpReceiveResponse"));
            break;
        }

        // 获取响应码
        httpResult.code = _GetStatusCode(m_hRequest);
        if (httpResult.code < 200 || httpResult.code >= 300)
        {
            _PrintError(_T("_GetStatusCode"));
            break;
        }

        // 查询文件大小
        ULONGLONG ullContentLength = 0;
        if (!_QueryContentLength(&ullContentLength))
        {
            _PrintError(_T("QueryContentLength"));
            break;
        }

        // 是否支持接收范围请求
        bool fAcceptRange =  _IsSupportAcceptRanges();

        // 多线程接收请求数据
        if (ullContentLength > 0 && fAcceptRange && dwThreadCount > 1)
        {
            httpResult = _MultiThreadRequest(url, ullContentLength, dwThreadCount);
        }
        else
        {
            // 单线程接收请求数据
            if (!_InternetReadData(httpResult.result))
            {
                _PrintError(_T("_InternetReadData"));
                break;
            }
        }

    } while (false);

    // 关闭相关句柄
    Close();

    return httpResult;
}

CWinHttpResult CWinHttpClient::_MultiThreadRequest(const _tstring& url, ULONGLONG ullContentLength, DWORD dwThreadCount/* = 1*/)
{
    CWinHttpResult httpResult;

    std::atomic<int> nTaskCount = dwThreadCount;
    bool fQuit = false;
    DWORD ullSinglePackageSize = ullContentLength / dwThreadCount;

    std::vector<CWinHttpClient> vTaskClients;
    vTaskClients.resize(dwThreadCount);

    std::vector<CWinHttpResult> vTaskResults;
    vTaskResults.resize(dwThreadCount);

    std::vector<std::thread> vTaskThreads;

    for (DWORD i = 0; i < dwThreadCount; i++)
    {
        ULONGLONG ullBeginPos = ullSinglePackageSize * i;
        ULONGLONG ullPackageSize = (i != dwThreadCount - 1) ? ullSinglePackageSize : ullContentLength - ullBeginPos;
        vTaskClients[i].SetReQuestDataRange(ullBeginPos, ullPackageSize);

        vTaskClients[i].m_strAgent = m_strAgent;
        vTaskClients[i].m_strHeader = m_strHeader;
        vTaskClients[i].m_strParam = m_strParam;
        vTaskClients[i].m_strUrl = m_strUrl;
        vTaskClients[i].m_strFilePath = m_strFilePath;
        vTaskClients[i].m_fPrint = m_fPrint;

        vTaskThreads.emplace_back(std::thread([&vTaskClients, &vTaskResults, i, url, &nTaskCount, &fQuit, this]() {

            while (!fQuit)
            {
                vTaskClients[i].m_AsyncData.AsyncResult.dwResult = 0;
                vTaskClients[i].m_AsyncData.AsyncResult.dwError = 0;

                // 修改请求起始位置
                vTaskClients[i].m_ullStart += vTaskClients[i].m_ullDownload;
                vTaskResults[i] = vTaskClients[i]._Get(url, [](const WINHTTP_PROGRESS_INFO& progress) {
                    return true;
                    }
                );

                if (vTaskResults[i].code >= 200 && vTaskResults[i].code < 300)
                {
                    break;
                }
            }

            nTaskCount--;

            }
        )
        );
    }

    // 进度统计用变量
    clock_t refreshInterval = 1000;
    clock_t curTime = ::clock();
    clock_t startTime = curTime;
    clock_t lastTime = curTime;
    double lfRemainTime = 0.0f;
    double lfSpeed = 0.0f;
    ULONGLONG ullLastDownload = 0;
    ULONGLONG ullTotalLength = ullContentLength;

    while (!fQuit)
    {
        ULONGLONG ullDownloaded = 0;

        // 进度计算
        if (m_cbProgress)
        {
            // 速度计算
            clock_t curTime = ::clock();
            if (curTime - lastTime >= refreshInterval  || 0 == nTaskCount)
            {
                for (auto& item : vTaskClients)
                {
                    ullDownloaded += item.m_ullDownload;
                }

                lfSpeed = (double)(ullDownloaded - ullLastDownload) / ((double)refreshInterval / 1000.0f);
                if (isinf(lfSpeed) || isnan(lfSpeed))
                {
                    lfSpeed = 0.0f;
                }

                // 进度报告
                {
                    WINHTTP_PROGRESS_INFO progress = { 0 };
                    progress.lfProgress = (double)ullDownloaded / (double)ullTotalLength;
                    progress.ullCur = ullDownloaded;
                    progress.ullTotal = ullTotalLength;
                    progress.lfSpeed = lfSpeed;
                    progress.costTime = curTime - startTime;
                    progress.lfRemainTime = ((double)(ullTotalLength - ullDownloaded)) / lfSpeed;
                    progress.nActiveThread = nTaskCount;
                    progress.nTotalThread = dwThreadCount;

                    if (!m_cbProgress(progress))
                    {
                        fQuit = true;
                        break;
                    }
                }

                lastTime = curTime;
                ullLastDownload = ullDownloaded;
            }
        }

        if (0 == nTaskCount)
        {
            break;
        }

        ::Sleep(10);
    }

    // 主动退出
    if (fQuit)
    {
        for (auto& item : vTaskClients)
        {
            item.Close();
        }
    }

    // 等待线程结束
    for (auto& item : vTaskThreads)
    {
        if (item.joinable())
        {
            item.join();
        }
    }

    // 结果拼接
    if(m_strFilePath.empty())
    {
        for (auto& item : vTaskResults)
        {
            httpResult.result += item.result;

            if (item.code >= 200 && item.code < 300)
            {
                httpResult.code = item.code;
            }
            else
            {
                httpResult.code = 0;
            }
        }
    }

    return httpResult;
}

std::string CWinHttpClient::_WStrToMultiStr(UINT CodePage, const std::wstring& str)
{
    int cbMultiByte = ::WideCharToMultiByte(CodePage, 0, str.c_str(), -1, NULL, 0, NULL, 0);
    std::string strResult(cbMultiByte, 0);
    size_t nConverted = ::WideCharToMultiByte(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size(), NULL, NULL);
    strResult.resize(nConverted);
    return strResult;
}

std::wstring CWinHttpClient::_MultiStrToWStr(UINT CodePage, const std::string& str)
{
    int cchWideChar = ::MultiByteToWideChar(CodePage, 0, str.c_str(), -1, NULL, NULL);
    std::wstring strResult(cchWideChar, 0);
    size_t nConverted = ::MultiByteToWideChar(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size());
    strResult.resize(nConverted);
    return strResult;
}

std::string CWinHttpClient::TStrToU8Str(const _tstring& str)
{
#ifdef _UNICODE
    return _WStrToMultiStr(CP_UTF8, str);
#else
    return _WStrToMultiStr(CP_UTF8, _MultiStrToWStr(CP_ACP, str));
#endif
}

std::wstring CWinHttpClient::TStrToWStr(const _tstring& str)
{
#ifdef _UNICODE
    return str;
#else
    return _MultiStrToWStr(CP_ACP, str);
#endif
}

_tstring CWinHttpClient::WStrToTStr(const std::wstring& str)
{
#ifdef _UNICODE
    return str;
#else
    return _WStrToMultiStr(CP_ACP, str);
#endif
}

void CWinHttpClient::SetAgent(const _tstring strAgent)
{
    m_strAgent = strAgent;
}

void CWinHttpClient::SetReQuestDataRange(ULONGLONG ullStart, ULONGLONG ullLength)
{
    m_ullStart = ullStart;
    m_ullLength = ullLength;
}

void CWinHttpClient::SetPrintStatus(bool fEnable/* = true*/)
{
    m_fPrint = fEnable;
}

bool CWinHttpClient::_QueryContentLength(PULONGLONG lpUllContentLength)
{
    DWORD dwInfoLevel = WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER64;
    ULONGLONG ullContentLength = 0;
    DWORD dwBufferLength = sizeof(ullContentLength);
    DWORD dwIndex = 0;
    if (!_WinHttpQueryHeaders(dwInfoLevel, WINHTTP_HEADER_NAME_BY_INDEX, &ullContentLength, &dwBufferLength, &dwIndex))
    {
        // 无法找到请求的标头
        if (ERROR_WINHTTP_HEADER_NOT_FOUND == ::GetLastError())
        {
            *lpUllContentLength = -1;
            return true;
        }

        _PrintError(_T("WinHttpQueryHeaders"));
        *lpUllContentLength = 0;
        return false;
    }

    *lpUllContentLength = ullContentLength;

    return true;
}

bool CWinHttpClient::_IsSupportAcceptRanges()
{
    WCHAR szBuf[MAX_PATH] = { 0 };
    DWORD dwBufferLength = sizeof(szBuf);
    DWORD dwIndex = 0;

    if (!_WinHttpQueryHeaders(WINHTTP_QUERY_ACCEPT_RANGES, WINHTTP_HEADER_NAME_BY_INDEX, &szBuf, &dwBufferLength, &dwIndex))
    {
        return false;
    }

    if (0 == _wcsicmp(szBuf, L"bytes"))
    {
        return true;
    }

    return false;
}

bool CWinHttpClient::_SetRequestDataRange(LONGLONG nBegin, LONGLONG nLength)
{
    WCHAR szBuf[MAX_PATH] = { 0 };

    if (0 == nLength)
    {
        (void)::StringCchPrintfW(szBuf, _countof(szBuf), L"Range:bytes=%lld-%lld", nBegin, nBegin);
    }
    else if (nLength > 0)
    {
        (void)::StringCchPrintfW(szBuf, _countof(szBuf), L"Range:bytes=%lld-%lld", nBegin, nBegin + nLength - 1);
    }
    else
    {
        (void)::StringCchPrintfW(szBuf, _countof(szBuf), L"Range:bytes=%lld-", nBegin);
    }

    return ::WinHttpAddRequestHeaders(m_hRequest, szBuf, (DWORD)wcslen(szBuf), 0);
}

DWORD CWinHttpClient::_GetStatusCode(HINTERNET hRequest)
{
    DWORD dwInfoLevel = WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER;
    DWORD dwRespCode = 0;
    DWORD dwBufferLength = sizeof(dwRespCode);
    DWORD dwIndex = 0;
    if (!_WinHttpQueryHeaders(dwInfoLevel, WINHTTP_HEADER_NAME_BY_INDEX, &dwRespCode, &dwBufferLength, &dwIndex))
    {
        return dwRespCode;
    }

    return dwRespCode;
}

bool CWinHttpClient::_InternetReadData(std::string& strData)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    bool fResult = false;

    // 查询文件大小
    ULONGLONG ullContentLength = 0;
    if (!_QueryContentLength(&ullContentLength))
    {
        _PrintError(_T("QueryContentLength"));
        return false;
    }

    // 分配数据缓冲
    try
    {
        if (-1 != ullContentLength)
        {
            strData.resize(ullContentLength, 0);
        }
    }
    catch (...)
    {
        _PrintError(_T("std::string resize"));
        return false;
    }

    // 接收数据
    LPBYTE lpBufPos = (LPBYTE)strData.data();
    std::string strBuf;
    ULONGLONG ullDownloaded = 0;
    ULONGLONG ullTotalLength = ullContentLength;
    ULONGLONG ullLastDownload = 0;

    // 进度统计用变量
    clock_t refreshInterval = 1000;
    clock_t curTime = ::clock();
    clock_t startTime = curTime;
    clock_t lastTime = curTime;
    double lfRemainTime = 0.0f;
    double lfSpeed = 0.0f;

    if (!m_strFilePath.empty())
    {
        LARGE_INTEGER liDistanceToMove = { 0 };

        // 共享读写 创建/打开 文件, 多线程读写
        hFile = ::CreateFile(m_strFilePath.c_str(),
            GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_ARCHIVE,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            _PrintError(_T("CreateFile"));
            return false;
        }

        // 设置数据写入位置
        liDistanceToMove.QuadPart = m_ullStart;
        ::SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_BEGIN);
    }

    do
    {
        // 检查可用数据
        DWORD dwNumberOfBytesAvailable = 0;
        DWORD dwNumberOfBytesRead = 0;
        if (!_WinHttpQueryDataAvailable(&dwNumberOfBytesAvailable))
        {
            _PrintError(_T("_WinHttpQueryDataAvailable"));
            break;
        }

        if (dwNumberOfBytesAvailable > 0)
        {
            try
            {
                strBuf.resize(dwNumberOfBytesAvailable);
            }
            catch (...)
            {
                _PrintError(_T("std::vector resize"));
                break;
            }

            if (!_WinHttpReadData(&strBuf[0], dwNumberOfBytesAvailable, &dwNumberOfBytesRead))
            {
                _PrintError(_T("_WinHttpReadData"));
                break;
            }

            // 写入到缓冲
            if (-1 == ullContentLength)
            {
                strData += strBuf;
                ullDownloaded += strBuf.size();
            }
            else
            {
                memcpy(lpBufPos, &strBuf[0], dwNumberOfBytesRead);
                lpBufPos += dwNumberOfBytesRead;
                ullDownloaded += dwNumberOfBytesRead;
            }

            if (INVALID_HANDLE_VALUE != hFile)
            {
                DWORD dwWritten = 0;
                if (!::WriteFile(hFile, strBuf.data(), dwNumberOfBytesRead, &dwWritten, NULL))
                {
                    _PrintError(_T("WriteFile"));
                    break;
                }
            }

            m_ullDownload += dwNumberOfBytesRead;
        }

        // 进度计算
        if (m_cbProgress)
        {
            // 速度计算
            clock_t curTime = ::clock();
            if (curTime - lastTime >= refreshInterval  || 0 == dwNumberOfBytesAvailable)
            {
                lfSpeed = (double)(ullDownloaded - ullLastDownload) / ((double)refreshInterval / 1000.0f);
                if (isinf(lfSpeed) || isnan(lfSpeed))
                {
                    lfSpeed = 0.0f;
                }

                if (0 == dwNumberOfBytesAvailable)
                {
                    ullTotalLength = ullDownloaded;
                }

                // 进度报告
                {
                    WINHTTP_PROGRESS_INFO progress = { 0 };
                    progress.lfProgress = (double)ullDownloaded / (double)ullTotalLength;
                    progress.ullCur = ullDownloaded;
                    progress.ullTotal = ullTotalLength;
                    progress.lfSpeed = lfSpeed;
                    progress.costTime = curTime - startTime;
                    progress.lfRemainTime = ((double)(ullTotalLength - ullDownloaded)) / lfSpeed;
                    progress.nActiveThread = 1;
                    progress.nTotalThread = 1;

                    if (!m_cbProgress(progress))
                    {
                        break;
                    }
                }

                lastTime = curTime;
                ullLastDownload = ullDownloaded;
            }
        }

        // 检查读取是否结束
        if (!dwNumberOfBytesAvailable)
        {
            fResult = true;
            break;
        }

    } while (true);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }

    return fResult;
}

void CWinHttpClient::_PrintError(LPCTSTR lpszError) const
{
    return;
    ConsoleOutput(_T("[Error][LastError: %d Error: %d Result: %d][%s]\r\n"),
        ::GetLastError(), 
        m_AsyncData.AsyncResult.dwError,
        m_AsyncData.AsyncResult.dwResult,
        lpszError
    );
}

void CWinHttpClient::ConsoleOutput(LPCTSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    _tstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsntprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
            ::WriteConsole(console, strResult.c_str(), nSize, NULL, NULL);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= INT32_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

#define WINHTTP_STATUS_TEXT(_code)  std::make_pair(_code, _T(#_code)),

void CWinHttpClient::_PrintStatus(DWORD dwCode, LPVOID lpvStatusInfo, DWORD dwStatusInfoLength)
{
    std::map<DWORD, _tstring> mapWinHttpCode = {
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_RESOLVING_NAME             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_NAME_RESOLVED              )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER       )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER        )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_SENDING_REQUEST            )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_REQUEST_SENT               )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE         )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED          )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION         )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED          )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_HANDLE_CREATED             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_DETECTING_PROXY            )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_REDIRECT                   )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE      )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_SECURE_FAILURE             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE          )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_READ_COMPLETE              )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_REQUEST_ERROR              )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE       )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE    )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_CLOSE_COMPLETE             )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_SHUTDOWN_COMPLETE          )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_GETPROXYSETTINGS_COMPLETE  )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_SETTINGS_WRITE_COMPLETE    )
        WINHTTP_STATUS_TEXT(WINHTTP_CALLBACK_STATUS_SETTINGS_READ_COMPLETE     )
    };

    auto itFind = mapWinHttpCode.find(dwCode);

    if (mapWinHttpCode.end() != itFind)
    {
        ConsoleOutput(_T("%08X %-50s "), itFind->first, itFind->second.c_str());
    }

    if (dwCode == WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER ||
        dwCode == WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER ||
        dwCode == WINHTTP_CALLBACK_STATUS_NAME_RESOLVED ||
        dwCode == WINHTTP_CALLBACK_STATUS_REDIRECT
        )
    {
        if (NULL != lpvStatusInfo)
        {
            LPWSTR lpData = (LPWSTR)lpvStatusInfo;
            ConsoleOutput(_T("Data %s"), WStrToTStr(lpData).c_str());
        }
    }

    if (dwCode == WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE ||
        dwCode == WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE ||
        dwCode == WINHTTP_CALLBACK_STATUS_REQUEST_SENT ||
        dwCode == WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED ||
        dwCode == WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE ||
        dwCode == WINHTTP_CALLBACK_STATUS_SECURE_FAILURE
        )
    {
        if (NULL != lpvStatusInfo)
        {
            LPDWORD lpData = (LPDWORD)lpvStatusInfo;
            ConsoleOutput(_T("Data %d(%08X)"), *lpData, *lpData);
        }
    }

    if (dwCode == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR
        )
    {
        if (NULL != lpvStatusInfo)
        {
            LPWINHTTP_ASYNC_RESULT lpData = (LPWINHTTP_ASYNC_RESULT)lpvStatusInfo;
            ConsoleOutput(_T("Error %d(%08X) Result: %d(%08X)"), 
                lpData->dwError, lpData->dwError,
                lpData->dwResult, lpData->dwResult
            );
        }
    }

    ConsoleOutput(_T("\r\n"));
}

VOID CALLBACK CWinHttpClient::WinHttpStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwStatus, LPVOID lpvInfo, DWORD dwInfoLength)
{
    CWinHttpClient* pThis = (CWinHttpClient*)dwContext;
    if (nullptr != pThis)
    {
        pThis->_WinHttpStatusCallback(hInternet, dwStatus, lpvInfo, dwInfoLength);
    }
}

// 状态回调
void CWinHttpClient::_WinHttpStatusCallback(HINTERNET hInternet, DWORD dwStatus, LPVOID lpvInfo, DWORD dwInfoLength)
{
    if (m_fPrint)
    {
        _PrintStatus(dwStatus, lpvInfo, dwInfoLength);
    }

    switch (dwStatus)
    {

        // 关闭与服务器的连接。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION:
    {
        break;
    }

    // 已成功连接到服务器。 lpvStatusInformation 参数包含指向 LPWSTR 的指针，该指针以点表示法指示服务器的 IP 地址。
    case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER:
    {
        LPWSTR lpData = (LPWSTR)lpvInfo;
        break;
    }

    // 连接到服务器。 lpvStatusInformation 参数包含指向 LPWSTR 的指针，该指针以点表示法指示服务器的 IP 地址。
    case WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER:
    {
        LPWSTR lpData = (LPWSTR)lpvInfo;
        break;
    }

    // 已成功关闭与服务器的连接。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED:
    {
        break;
    }

    // 可以使用 WinHttpReadData 检索数据。 lpvStatusInformation 参数指向包含可用数据字节数的 DWORD。 
    // dwStatusInformationLength 参数本身是 4 (DWORD) 的大小。
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:// WinHttpQueryDataAvailable 完成
    {
        DWORD lpSize = *(LPDWORD)lpvInfo;
        m_AsyncData.dwSize = lpSize;
        m_AsyncData.AsyncResult.dwResult = 0;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 已创建 HINTERNET 句柄。 lpvStatusInformation 参数包含指向 HINTERNET 句柄的指针。
    case WINHTTP_CALLBACK_STATUS_HANDLE_CREATED:
    {
        LPHINTERNET pHandle = (LPHINTERNET)lpvInfo;
        m_AsyncData.dwContext = (DWORD_PTR)*pHandle;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 此句柄值已终止。 lpvStatusInformation 参数包含指向 HINTERNET 句柄的指针。 此句柄不再有回调。
    case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
    {
        LPHINTERNET pHandle = (LPHINTERNET)lpvInfo;
        break;
    }

    // 响应标头已收到，可用于 WinHttpQueryHeaders。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE: // WinHttpReceiveResponse 完成
    {
        m_AsyncData.AsyncResult.dwResult = 0;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 从服务器收到中间 (100 级别) 状态代码消息。 lpvStatusInformation 参数包含指向指示状态代码的 DWORD 的指针。
    case WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE:
    {
        LPDWORD pStatusCode = (LPDWORD)lpvInfo;
        break;
    }

    // 已成功找到服务器的 IP 地址。 lpvStatusInformation 参数包含指向 LPWSTR 的指针，该指针指示已解析的名称。
    case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED:
    {
        LPWSTR lpResolved = (LPWSTR)lpvInfo;
        break;
    }

    // 已成功从服务器读取数据。 
    // lpvStatusInformation 参数包含指向调用 WinHttpReadData 中指定的缓冲区的指针。 
    // dwStatusInformationLength 参数包含读取的字节数。
    // WinHttpWebSocketReceive 使用时，lpvStatusInformation 参数包含指向WINHTTP_WEB_SOCKET_STATUS结构的指针，
    // dwStatusInformationLength 参数指示 lpvStatusInformation 的大小。
    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE: // WinHttpReadData 完成
    {
        LPBYTE* ppBuf = (LPBYTE*)lpvInfo;
        DWORD dwRead = dwInfoLength;
        m_AsyncData.AsyncResult.dwResult = 0;
        m_AsyncData.dwSize = dwRead;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 等待服务器响应请求。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
    {
        break;
    }

    // HTTP 请求将自动重定向请求。 lpvStatusInformation 参数包含指向指示新 URL 的 LPWSTR 的指针。 
    // 此时，应用程序可以使用重定向响应读取服务器返回的任何数据，并且可以查询响应标头。 它还可以通过关闭句柄来取消操作。
    case WINHTTP_CALLBACK_STATUS_REDIRECT:
    {
        LPWSTR lpData = (LPWSTR)lpvInfo;
        break;
    }

    // 发送 HTTP 请求时出错。 lpvStatusInformation 参数包含指向WINHTTP_ASYNC_RESULT结构的指针。 
    // 其 dwResult 成员指示被调用函数的 ID，dwError 指示返回值。
    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    {
        DWORD dwError = ERROR_WINHTTP_INCORRECT_HANDLE_STATE;
        LPWINHTTP_ASYNC_RESULT pAsyncResult = (LPWINHTTP_ASYNC_RESULT)lpvInfo;
        m_AsyncData.AsyncResult = *pAsyncResult;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 已成功将信息请求发送到服务器。 lpvStatusInformation 参数包含指向 DWORD 的指针，该指针指示发送的字节数。
    case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
    {
        LPDWORD pSentBytes = (LPDWORD)lpvInfo;
        break;
    }

    // 查找服务器名称的 IP 地址。 lpvStatusInformation 参数包含指向要解析的服务器名称的指针。
    case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME:
    {
        LPWSTR lpName = (LPWSTR)lpvInfo;
        break;
    }

    // 已成功从服务器收到响应。 lpvStatusInformation 参数包含指向指示接收字节数的 DWORD 的指针。
    case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
    {
        LPDWORD pRecv = (LPDWORD)lpvInfo;
        break;
    }

    // 在与服务器建立安全 (HTTPS) 连接时遇到一个或多个错误。 
    // lpvStatusInformation 参数包含指向 DWORD 的指针，该指针是错误值的按位 OR 组合。 
    // 有关详细信息，请参阅 lpvStatusInformation 的说明。
    case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
    {
        DWORD dwCode = *(LPDWORD)lpvInfo;
        switch (dwCode)
        {
            // 证书吊销检查已启用，但吊销检查未能验证证书是否已吊销。 用于检查吊销的服务器可能无法访问
        case WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED:
        {
            break;
        }
        // SSL 证书无效
        case WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CERT:
        {
            break;
        }
        // SSL 证书已吊销
        case WINHTTP_CALLBACK_STATUS_FLAG_CERT_REVOKED:
        {
            break;
        }
        // 函数不熟悉生成服务器证书的证书颁发机构
        case WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CA:
        {
            break;
        }
        // SSL 证书公用名 (主机名字段) 不正确，例如，如果输入 www.microsoft.com 且证书上的公用名显示 www.msn.com
        case WINHTTP_CALLBACK_STATUS_FLAG_CERT_CN_INVALID:
        {
            break;
        }
        // 从服务器收到的 SSL 证书日期不正确。 证书已过期。
        case WINHTTP_CALLBACK_STATUS_FLAG_CERT_DATE_INVALID:
        {
            break;
        }
        // 应用程序在加载 SSL 库时遇到内部错误
        case WINHTTP_CALLBACK_STATUS_FLAG_SECURITY_CHANNEL_ERROR:
        {
            break;
        }
        }

        break;
    }

    // 将信息请求发送到服务器。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
    {
        break;
    }

    // 请求已成功完成。 
    // lpvStatusInformation 参数是传递给 WinHttpSendRequest (初始请求正文) 的 lpOptional 值，
    // dwStatusInformationLength 参数指示 (传递到 WinHttpSendRequest) 传递到 winHttpSendRequest 的 dwOptionalLength 值成功写入此类初始正文字节的数目。
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:  // WinHttpSendRequest 完成
    {
        m_AsyncData.AsyncResult.dwResult = 0;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 数据已成功写入服务器。 lpvStatusInformation 参数包含指向 DWORD 的指针，该指针指示写入的字节数。
    // 当 WinHttpWebSocketSend 使用时， lpvStatusInformation 参数包含指向WINHTTP_WEB_SOCKET_STATUS结构的指针，
    // dwStatusInformationLength 参数指示 lpvStatusInformation 的大小。
    case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:    // WinHttpWriteData 完成
    {
        m_AsyncData.AsyncResult.dwResult = 0;
        ::SetEvent(m_AsyncData.hEvent);
        break;
    }

    // 通过调用 WinHttpGetProxyForUrlEx 启动的操作已完成。 可以使用 WinHttpReadData 检索数据。
    case WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE:
    {
        break;
    }
    // 通过调用 WinHttpWebSocketClose 成功关闭了连接。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_CLOSE_COMPLETE:
    {
        break;
    }
    // 通过调用 WinHttpWebSocketShutdown 成功关闭连接。 lpvStatusInformation 参数为 NULL。
    case WINHTTP_CALLBACK_STATUS_SHUTDOWN_COMPLETE:
    {
        break;
    }
    }
}

bool CWinHttpClient::_WinHttpOpen(LPCWSTR pszAgentW, DWORD dwAccessType, LPCWSTR pszProxyW, LPCWSTR pszProxyBypassW, DWORD dwFlags)
{
    // 初始化 WinHTTP 函数的使用并返回 WinHTTP 会话句柄。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpopen
    m_hSession = ::WinHttpOpen(
        pszAgentW,                      // 指向字符串变量的指针，此名称用作 HTTP 协议中的 用户代理
        dwAccessType,                   // 所需的访问类型
        pszProxyW,                      // 代理访问时要使用的代理服务器的名称
        pszProxyBypassW,                // 代理访问时要使用的代理服务器的密码
        dwFlags                         // 指示影响此函数行为的各种选项的标志
    );

    return NULL != m_hSession;
}

bool CWinHttpClient::_WinHttpConnect(LPCWSTR pswzServerName, INTERNET_PORT nServerPort)
{
    // 指定 HTTP 请求的初始目标服务器，并将 HINTERNET 连接句柄返回到该初始目标的 HTTP 会话
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpconnect
    // 即使 WinHTTP 在异步模式下使用（即在 WinHttpOpen中设置 WINHTTP_FLAG_ASYNC），此函数也会同步运行
    m_hConnect = ::WinHttpConnect(
        m_hSession,                   // 由先前调用 WinHttpOpen 返回的有效 HINTERNETWinHTTP 会话句柄
        pswzServerName,             // HTTP 服务器的主机名
        nServerPort,                // 建立连接的服务器上的 TCP/IP 端口
        0                           // 保留参数, 必须为0
    );

    return NULL != m_hConnect;
}

bool CWinHttpClient::_WinHttpOpenRequest(const _tstring& strVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags)
{
    // 创建 HTTP 请求句柄
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpopenrequest
    HINTERNET hRequest = ::WinHttpOpenRequest(
        m_hConnect,                         // WinHttpConnect 返回的 HTTP 会话的 HINTERNET 连接句柄
        TStrToWStr(strVerb).c_str(),        // 请求的 HTTP 谓词
        pwszObjectName,                     // 指定 HTTP 谓词的目标资源名称
        pwszVersion,                        // HTTP 版本的字符串的指针
        pwszReferrer,                       // 指定从中获取 请求 pwszObjectName 中的 URL 的文档的 URL
        ppwszAcceptTypes,                   // 指定客户端接受的媒体类型
        dwFlags                             // Internet 标志值
    );

    // 等待异步请求完成
    if (!_WaitForAsyncEvent())
    {
        return NULL;
    }

    m_hRequest = (HINTERNET)m_AsyncData.dwContext;

    return NULL != m_hRequest;
}

bool CWinHttpClient::_WinHttpSetSessionTimeouts(int nResolveTimeout, int nConnectTimeout,int nSendTimeout,int nReceiveTimeout)
{
    // 设置与 HTTP 事务相关的超时。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsettimeouts
    // 即使在异步模式下使用 WinHTTP (即在 WinHttpOpen) 中设置了WINHTTP_FLAG_ASYNC时，此函数也会同步运行。
    return ::WinHttpSetTimeouts(
        m_hSession,             // WinHttpOpen 或 WinHttpOpenRequest 返回的 HINTERNET 句柄。
        nResolveTimeout,        // 名称解析的超时值（以毫秒为单位）
        nConnectTimeout,        // 服务器连接请求的超时值（以毫秒为单位）
        nSendTimeout,           // 发送请求的超时值（以毫秒为单位）
        nReceiveTimeout         // 接收对请求的响应超超时值（以毫秒为单位）
    );
}

bool CWinHttpClient::_WinHttpSetRequestTimeouts(int nResolveTimeout, int nConnectTimeout,int nSendTimeout,int nReceiveTimeout)
{
    // 设置与 HTTP 事务相关的超时。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsettimeouts
    // 即使在异步模式下使用 WinHTTP (即在 WinHttpOpen) 中设置了WINHTTP_FLAG_ASYNC时，此函数也会同步运行。
    return ::WinHttpSetTimeouts(
        m_hRequest,             // WinHttpOpen 或 WinHttpOpenRequest 返回的 HINTERNET 句柄。
        nResolveTimeout,        // 名称解析的超时值（以毫秒为单位）
        nConnectTimeout,        // 服务器连接请求的超时值（以毫秒为单位）
        nSendTimeout,           // 发送请求的超时值（以毫秒为单位）
        nReceiveTimeout         // 接收对请求的响应超超时值（以毫秒为单位）
    );
}

bool CWinHttpClient::_WinHttpSetStatusCallback(WINHTTP_STATUS_CALLBACK lpfnInternetCallback, DWORD dwNotificationFlags
)
{
    // 设置回调函数
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsetstatuscallback
    WINHTTP_STATUS_CALLBACK statusCallback = ::WinHttpSetStatusCallback(
        m_hSession,                 // 要为其设置回调的 HINTERNET 句柄
        lpfnInternetCallback,       // 指向进度时要调用的回调函数的指针
        dwNotificationFlags,        // 无符号长整数值，该值指定标志以指示哪些事件激活回调函数
        0
    );

    if (WINHTTP_INVALID_STATUS_CALLBACK == statusCallback)
    {
        return false;
    }

    return true;
}

bool CWinHttpClient::_WaitForAsyncEvent(DWORD dwMilliseconds/* = INFINITE*/)
{
    // 等待异步事件响应
    m_AsyncData.dwWait = ::WaitForSingleObject(m_AsyncData.hEvent, dwMilliseconds);
    if (m_fAbort)
    {
        return false;
    }

    if (WAIT_OBJECT_0 == m_AsyncData.dwWait && 0 == m_AsyncData.AsyncResult.dwResult)
    {
        return true;
    }

    return false;
}

bool CWinHttpClient::_WinHttpSetSessionOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength)
{
    // 设置 Internet 选项。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsetoption
    return ::WinHttpSetOption(m_hSession, dwOption, lpBuffer, dwBufferLength);
}

bool CWinHttpClient::_WinHttpSetRequestOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength)
{
    // 设置 Internet 选项。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsetoption
    return ::WinHttpSetOption(m_hRequest, dwOption, lpBuffer, dwBufferLength);
}

bool CWinHttpClient::_WinHttpSendRequest(_tstring strHeader, LPVOID lpData, DWORD dwSize, DWORD_PTR dwContext)
{
    std::wstring wstrHeader = TStrToWStr(strHeader);
    LPCWSTR lpHeader = (LPCWSTR)wstrHeader.data();
    DWORD dwHeaderSize = (DWORD)wstrHeader.size();

    if (wstrHeader.empty())
    {
        lpHeader = WINHTTP_NO_ADDITIONAL_HEADERS;
        dwHeaderSize = 0;
    }

    bool fResult = false;

    do
    {
        // 发送请求
        // 将指定的请求发送到 HTTP 服务器。
        // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsendrequest
        if (::WinHttpSendRequest(
            m_hRequest,             //WinHttpOpenRequest 返回的 HINTERNET 句柄
            lpHeader,               //要追加到请求的其他标头
            dwHeaderSize,           //附加标头的长度（以字符为单位）
            lpData,                 //请求标头之后发送的任何可选数据
            dwSize,                 //可选数据的长度（以字节为单位）
            dwSize,                 //发送的总数据的长度
            dwContext               //上下文
        ))
        {
            fResult = true;
            break;
        }

        // 安全 HTTP 服务器需要客户端证书
        if (ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED != ::GetLastError())
        {
            break;
        }

        // 设置 Internet 选项
        // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsetoption
        if (!_WinHttpSetRequestOption(
            WINHTTP_OPTION_CLIENT_CERT_CONTEXT,
            WINHTTP_NO_CLIENT_CERT_CONTEXT,
            0
        ))
        {
            break;
        }

        // 再次发送请求
        // 将指定的请求发送到 HTTP 服务器。
        // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsendrequest
        if (!::WinHttpSendRequest(
            m_hRequest,             // WinHttpOpenRequest 返回的 HINTERNET 句柄
            lpHeader,               // 要追加到请求的其他标头
            dwHeaderSize,           // 附加标头的长度（以字符为单位）
            lpData,                 // 请求标头之后发送的任何可选数据
            dwSize,                 // 可选数据的长度（以字节为单位）
            dwSize,                 // 发送的总数据的长度
            NULL
        ))
        {
            break;
        }

        fResult = true;

    } while (false);

    if (!fResult)
    {
        return false;
    }

    // 等待异步请求完成
    return _WaitForAsyncEvent();
}

bool CWinHttpClient::_WinHttpReceiveResponse(HINTERNET hRequest)
{
    // 等待接收 WinHttpSendRequest 发起的 HTTP 请求的响应。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpreceiveresponse
    if (!::WinHttpReceiveResponse(
        hRequest,       // WINHttpOpenRequest 返回并由 WinHttpSendRequest 发送的 HINTERNET 句柄。
        NULL            // 此参数是保留的，必须为 NULL。
    ))
    {
        return false;
    }

    // 等待异步请求完成
    return _WaitForAsyncEvent();
}

bool CWinHttpClient::_WinHttpQueryHeaders(DWORD dwInfoLevel, LPCWSTR pwszName, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex)
{
    return ::WinHttpQueryHeaders(
        m_hRequest,                     //WinHttpOpenRequest 返回的 HINTERNET 请求句柄
        dwInfoLevel,                    //指定“查询信息标志”页上列出的属性标志和修饰符标志的组合
        pwszName,                       //标头名称
        lpBuffer,                       //接收信息的缓冲区
        lpdwBufferLength,               //数据缓冲区的长度
        lpdwIndex                       //从零开始的标头索引的指针，用于枚举具有相同名称的多个标头
    );
}

bool CWinHttpClient::_WinHttpQueryDataAvailable(LPDWORD lpdwNumberOfBytesAvailable)
{
    m_AsyncData.dwSize = 0;

    // 返回可使用 WinHttpReadData 读取的数据量（以字节为单位）。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpquerydataavailable
    if (!::WinHttpQueryDataAvailable(
        m_hRequest,                     // WinHttpOpenRequest 返回的有效 HINTERNET 句柄。 
                                        // WinHttpReceiveResponse 必须已为此句柄调用，并在调用 WinHttpQueryDataAvailable 之前完成。
        NULL                            // 指向接收可用字节数的无符号长整数变量的指针。 
                                        // 在异步模式下使用 WinHTTP 时，始终将此参数设置为 NULL ，并在回调函数中检索数据;
                                        // 不这样做可能会导致内存故障。
    ))
    {
        return false;
    }

    // 等待异步请求完成
    if (!_WaitForAsyncEvent())
    {
        return false;
    }

    *lpdwNumberOfBytesAvailable = m_AsyncData.dwSize;
    return true;
}

bool CWinHttpClient::_WinHttpReadData(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead)
{
    m_AsyncData.dwSize = 0;

    // 从 WinHttpOpenRequest 函数打开的句柄读取数据。
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpreaddata
    if (!::WinHttpReadData(
        m_hRequest,             // 从上一次调用 WinHttpOpenRequest 返回的有效 HINTERNET 句柄。
        lpBuffer,               // 指向接收读取数据的缓冲区的指针。 确保此缓冲区在 WinHttpReadData 完成之前保持有效。
        dwNumberOfBytesToRead,  // 要读取的字节数的无符号长整数值。
        NULL                    // 指向接收读取字节数的无符号长整数变量的指针。 
                                // WinHttpReadData 在执行任何工作或错误检查之前将此值设置为零。 
                                // 异步使用 WinHTTP 时，始终将此参数设置为 NULL ，并在回调函数中检索信息;
                                // 不这样做可能会导致内存故障。
    ))
    {
        return false;
    }

    // 等待异步请求完成
    if (!_WaitForAsyncEvent())
    {
        return false;
    }

    *lpdwNumberOfBytesRead = m_AsyncData.dwSize;
    return true;
}