#include "resource.h"
#include <windows.h>
#include <Dbghelp.h>
#include <strsafe.h>
#include <commctrl.h>
#include "Win32Utils/CFileUtils.h"
#include "Win32Utils/CPathUtils.h"
#include "Common.h"
#include "CDesignDialog.h"

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    ::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    _tstring strFile = CPathUtils::GetCurrentModulePath();
    _tstring strDest = CPathUtils::GetRelativePath(strFile, _T(R"(C:\gitee\fcassistant\Bin\Games\01.TURTLES III 1-1.nes)"), false);

    INITCOMMONCONTROLSEX data = { 0 };
    data.dwSize = sizeof(data);

    ::InitCommonControlsEx(&data);

    CDesignDialog DesignDlg;
    DesignDlg.DoModalEx(IDD_DIALOG_DESIGN, nullptr, TRUE, TRUE);


    return 0;
}

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    SYSTEMTIME st = { 0 };

    ::GetSystemTime(&st);
    _tstring strDmpDir = CPathUtils::GetCurrentModuleDir() + _T("\\Dump");
    CFileUtils::CreateDir(strDmpDir);

    ::StringCchPrintf(szBuf, _countof(szBuf),
        _T("%s\\%s_%04d-%02d-%02d %02d_%02d_%02d.%d.dmp"),
        strDmpDir.c_str(),
        CPathUtils::GetCurrentModuleName().c_str(),
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
    );

    HANDLE lhDumpFile = ::CreateFile(szBuf, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != lhDumpFile)
    {
        MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo = { 0 };
        loExceptionInfo.ExceptionPointers = ExceptionInfo;
        loExceptionInfo.ThreadId = GetCurrentThreadId();
        loExceptionInfo.ClientPointers = TRUE;
        ::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpWithFullMemory, &loExceptionInfo, nullptr, nullptr);
        ::CloseHandle(lhDumpFile);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}