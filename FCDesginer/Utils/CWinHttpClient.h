#pragma once

#include <Windows.h>
#include <WinHttp.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <set>
#include <thread>
#include <time.h>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

#pragma comment(lib, "winhttp.lib")
#define WINHTTP_AGENT_HTTPS "(Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36 Edg/122.0.0.0)"

// 响应结果
class CWinHttpResult
{
public:
    CWinHttpResult() :code(0) {}
    std::string result;             //响应结果
    uint32_t code;                  //响应状态码

    bool IsSuccess()
    {
        return (code >= 200) && (code < 300);
    }
};

// 进度信息
typedef struct _WINHTTP_PROGRESS_INFO
{
    ULONGLONG ullCur;           // 当前下载量(字节)
    ULONGLONG ullTotal;         // 总数据量(字节)
    double lfProgress;          // 当前进度(0.0f - 1.0f)
    double lfSpeed;             // 当前速度(字节/秒)
    double lfRemainTime;        // 剩余时间(毫秒)
    clock_t costTime;           // 消耗时间(毫秒)
    uint32_t nActiveThread;     // 活动任务数
    uint32_t nTotalThread;      // 总任务线程数
}WINHTTP_PROGRESS_INFO, *LPWINHTTP_PROGRESS_INFO;

// 异步数据
typedef struct _WHTTP_ASYNC_DATA
{
    DWORD_PTR dwContext;                // 上下文
    HANDLE hEvent;                      // 事件
    DWORD dwWait;                       // 事件等待结果
    DWORD dwSize;                       // 传输数据大小
    WINHTTP_ASYNC_RESULT AsyncResult;   // 异步结果
}WHTTP_ASYNC_DATA, *LPWHTTP_ASYNC_DATA;

// 进度回调
using WinHttpCallback = std::function<bool(const WINHTTP_PROGRESS_INFO& progress)>;

class CWinHttpValue;
using CWinHttpObject = std::map<_tstring, CWinHttpValue>;
using CWinHttpArray = std::vector<CWinHttpValue>;

// WinHttp客户端参数类
class CWinHttpValue
{
public:

    CWinHttpValue();
    CWinHttpValue(bool val);
    CWinHttpValue(int val);
    CWinHttpValue(double val);
    CWinHttpValue(const LPCTSTR val);
    CWinHttpValue(const _tstring& val);
    CWinHttpValue(const CWinHttpObject& val);
    CWinHttpValue(const CWinHttpValue& val);
    CWinHttpValue(const CWinHttpArray& val);
    CWinHttpValue(CWinHttpValue&& val) noexcept;
    ~CWinHttpValue();

    bool IsEmpty() const;
    _tstring AsGetString() const;
    _tstring AsJsonString() const;
    _tstring AsHeaderString() const;

private:
    _tstring _AsGetString() const;
    _tstring _AsJsonString() const;
    _tstring _AsHeaderString(int depth) const;
    static void _DumpString(_tstring& append_str, const _tstring& text, bool flag_escape);
    static bool _GetUnicodeString(_tstring& append_str, const TCHAR* data_ptr, const TCHAR** end_ptr);

private:

    union _WIN_HTTP_VALUE_DATA
    {
        bool                _Bool;
        int                 _Int;
        double              _Float;
        _tstring*           _StringPtr;
        CWinHttpObject*     _ObjectPtr;
        CWinHttpArray*      _ArrayPtr;
    }m_Data;

    enum _WIN_HTTP_VALUE_TPYE
    {
        TypeNull,
        TypeBool,
        TypeInt,
        TypeFloat,
        TpyeString,
        TpyeObject,
        TpyeArray
    }m_Type;
};

// WinHttp客户端辅助类
class CWinHttpClient
{
public:

    CWinHttpClient();

    ~CWinHttpClient();

    // 关闭 
    void Close();

    // 发送 GET 请求
    CWinHttpResult Get(const _tstring& url, const CWinHttpValue& param = {}, const CWinHttpValue& header = {}, WinHttpCallback cb = nullptr);

    // 发送 POST 请求
    CWinHttpResult Post(const _tstring& url, const CWinHttpValue& param = {}, const CWinHttpValue& header = {}, WinHttpCallback cb = nullptr);

    // 发送 GET 请求(多线程)
    CWinHttpResult GetEx(const _tstring& url, const CWinHttpValue& param = {}, const CWinHttpValue& header = {}, WinHttpCallback cb = nullptr, DWORD dwThreadCount = 1);

    // 多线程下载文件
    CWinHttpResult DownloadFile(const _tstring& url, const _tstring& path = _T(""), const CWinHttpValue& header = {}, WinHttpCallback cb = nullptr, DWORD dwThreadCount = 4);

    // 设置用户代理字符串
    void SetAgent(const _tstring strAgent = _T(WINHTTP_AGENT_HTTPS));

    // 设置请求数据范围
    void SetReQuestDataRange(ULONGLONG ullStart = 0, ULONGLONG ullLength = -1);

    // 设置异步状态打印
    void SetPrintStatus(bool fEnable = true);

public:

    // 字符串转UTF-8编码字符串
    static std::string TStrToU8Str(const _tstring& str);

    // 字符串转宽字节字符串
    static std::wstring TStrToWStr(const _tstring& str);

    // 宽字节字符串转字符串
    static _tstring WStrToTStr(const std::wstring& str);

    // 控制台打印
    static void ConsoleOutput(LPCTSTR pFormat, ...);

    // 编码Utf8字符串
    static std::string EncoderFromUtf8(const std::string& strContent);

    // 解码Utf8字符串
    static std::string DecoderFromUtf8(const std::string& strContent);

private:

    // 发送 GET 请求
    CWinHttpResult _Get(const _tstring& url, WinHttpCallback cb = nullptr);

    // 发送 POST 请求
    CWinHttpResult _Post(const _tstring& url, WinHttpCallback cb = nullptr);

    // 执行请求
    CWinHttpResult _DoRequest(const _tstring& url, const _tstring& strMethod, const std::string& strParam);

    // 执行请求(多线程)
    CWinHttpResult _GetEx(const _tstring& url, WinHttpCallback cb = nullptr, DWORD dwThreadCount = 1);

    CWinHttpResult _MultiThreadRequest(const _tstring& url, ULONGLONG ullContentLength, DWORD dwThreadCount = 1);

    // 读取网络流
    bool _InternetReadData(std::string& strData);

    // 查询资源大小
    bool _QueryContentLength(PULONGLONG lpUllContentLength);

    // 查询是否支持接收范围
    bool _IsSupportAcceptRanges();

    // 设置请求数据范围
    bool _SetRequestDataRange(LONGLONG nBegin, LONGLONG nLength);

    // 获取状态码
    DWORD _GetStatusCode(HINTERNET hRequest);

    // 错误输出
    void _PrintError(LPCTSTR lpszError) const;

    // 状态码打印
    static void _PrintStatus(DWORD dwCode, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

    // 状态回调
    static VOID CALLBACK WinHttpStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwStatus, LPVOID lpvInfo, DWORD dwInfoLength);

    // 状态回调
    void _WinHttpStatusCallback(HINTERNET hInternet, DWORD dwStatus, LPVOID lpvInfo, DWORD dwInfoLength);

    // 等待异步事件
    bool _WaitForAsyncEvent(DWORD dwMilliseconds = INFINITE);

    // 打开会话
    bool _WinHttpOpen(LPCWSTR pszAgentW, DWORD dwAccessType, LPCWSTR pszProxyW, LPCWSTR pszProxyBypassW, DWORD dwFlags);

    // 设置会话超时
    bool _WinHttpSetSessionTimeouts(int nResolveTimeout, int nConnectTimeout,int nSendTimeout,int nReceiveTimeout);

    // 设置请求超时
    bool _WinHttpSetRequestTimeouts(int nResolveTimeout, int nConnectTimeout,int nSendTimeout,int nReceiveTimeout);

    // 设置状态回调
    bool _WinHttpSetStatusCallback(WINHTTP_STATUS_CALLBACK lpfnInternetCallback, DWORD dwNotificationFlags);

    // 设置会话选项
    bool _WinHttpSetSessionOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);

    // 设置请求选项
    bool _WinHttpSetRequestOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);

    // 连接会话
    bool _WinHttpConnect(LPCWSTR pswzServerName, INTERNET_PORT nServerPort);

    // 打开请求
    bool _WinHttpOpenRequest(const _tstring& strVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags);

    // 发送请求
    bool _WinHttpSendRequest(_tstring strHeader, LPVOID lpData, DWORD dwSize, DWORD_PTR dwContext);

    // 查询请求头
    bool _WinHttpQueryHeaders(DWORD dwInfoLevel, LPCWSTR pwszName, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);

    // 接收响应数据
    bool _WinHttpReceiveResponse(HINTERNET hRequest);

    // 查询请求数据是否可用
    bool _WinHttpQueryDataAvailable(LPDWORD lpdwNumberOfBytesAvailable);

    // 读取请求数据
    bool _WinHttpReadData(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);

    // 宽字节字符串转多字节字符串
    static std::string _WStrToMultiStr(UINT CodePage, const std::wstring& str);

    // 多字节字符串转宽字节字符串
    static std::wstring _MultiStrToWStr(UINT CodePage, const std::string& str);

private:
    HINTERNET m_hSession;                           // 会话句柄
    HINTERNET m_hConnect;                           // 连接句柄
    HINTERNET m_hRequest;                           // 请求句柄

    WinHttpCallback m_cbProgress;                   // 进度回调
    WHTTP_ASYNC_DATA        m_AsyncData;            // 异步信息
    _tstring                m_strAgent;             // 代理字符串
    _tstring                m_strFilePath;          // 保存文件路径

    _tstring                m_strUrl;               // 请求链接
    _tstring                m_strHeader;            // 请求头信息
    _tstring                m_strParam;             // 请求参数

    ULONGLONG               m_ullStart;             // 请求数据起始位置
    ULONGLONG               m_ullLength;            // 请求长度
    ULONGLONG               m_ullDownload;          // 已下载数据
    bool                    m_fAbort;               // 终止
    bool                    m_fPrint;               // 打印进度
};
