#include "CProcessUtils.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <strsafe.h>

#define DPSAPI_VERSION = 1
#pragma comment(lib, "Psapi.lib")

#pragma warning(disable:6385)

bool CProcessUtils::RunProcess(const _tstring& strCmd, const _tstring& strCurDir, bool isShow, bool isWait)
{
    TCHAR szCommandLine[MAX_PATH] = { 0 };
    SECURITY_ATTRIBUTES   sa = { 0 };
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    LPCTSTR lpCurrentDir = NULL;

    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);

    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = isShow ? SW_SHOW : SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = NULL;
    si.hStdError = NULL;

    if (!strCmd.empty())
    {
        ::StringCchCopy(szCommandLine, _countof(szCommandLine), strCmd.c_str());
    }

    if (!strCurDir.empty())
    {
        lpCurrentDir = strCurDir.c_str();
    }

    if (!::CreateProcess(NULL, szCommandLine, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, lpCurrentDir, &si, &pi))
    {
        return false;
    }

    if (isWait)
    {
        ::WaitForSingleObject(pi.hProcess, INFINITE);
    }

    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);

    return true;
}

_tstring CProcessUtils::GetProcessPath(DWORD dwProcessId, bool isNtPath)
{
    _tstring strPath;
    HANDLE hProcess = NULL;
    TCHAR szBuffer[MAX_PATH] = { 0 };
    DWORD dwSize = _countof(szBuffer);

    hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess)
    {
        goto L_Cleanup;
    }

    if (0 == ::GetProcessImageFileName(hProcess, szBuffer, dwSize))
    {
        goto L_Cleanup;
    }

    if (isNtPath)
    {
        strPath = DosDevicePathToNtPath(szBuffer);
    }
    else
    {
        strPath = szBuffer;
    }

L_Cleanup:

    if (hProcess)
    {
        ::CloseHandle(hProcess);
    }

    return strPath;
}

_tstring CProcessUtils::GetParentProcessPath(DWORD dwProcessId, bool isNtPath)
{
    _tstring strPath;
    TCHAR szBuffer[MAX_PATH] = { 0 };
    DWORD dwSize = _countof(szBuffer);
    HANDLE hProcess = NULL;
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    BOOL isSuc = FALSE;
    DWORD dwParentProcessId = 0;
    BOOL isFind = FALSE;

    hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcessSnap)
    {
        goto L_Cleanup;
    }

    pe32.dwSize = sizeof(pe32);
    isSuc = ::Process32First(hProcessSnap, &pe32);
    if (!isSuc)
    {
        goto L_Cleanup;
    }

    while (isSuc)
    {
        if (pe32.th32ProcessID == dwProcessId)
        {
            dwParentProcessId = pe32.th32ParentProcessID;
            isFind = TRUE;
            break;
        }

        isSuc = ::Process32Next(hProcessSnap, &pe32);
    }

    if (!isFind)
    {
        goto L_Cleanup;
    }

    hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwParentProcessId);
    if (!hProcess)
    {
        goto L_Cleanup;
    }

    if (0 == ::GetProcessImageFileName(hProcess, szBuffer, dwSize))
    {
        goto L_Cleanup;
    }

    if (isNtPath)
    {
        strPath = DosDevicePathToNtPath(szBuffer);
    }
    else
    {
        strPath = szBuffer;
    }

L_Cleanup:

    if (hProcess)
    {
        ::CloseHandle(hProcess);
    }

    if (hProcessSnap)
    {
        ::CloseHandle(hProcessSnap);
    }

    return strPath;
}

DWORD CProcessUtils::GetParentProcessID(DWORD dwProcessId)
{
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    BOOL isSuc = FALSE;
    DWORD dwParentProcessId = 0;

    hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcessSnap)
    {
        goto L_Cleanup;
    }

    pe32.dwSize = sizeof(pe32);
    isSuc = ::Process32First(hProcessSnap, &pe32);
    if (!isSuc)
    {
        goto L_Cleanup;
    }

    while (isSuc)
    {
        if (pe32.th32ProcessID == dwProcessId)
        {
            dwParentProcessId = pe32.th32ParentProcessID;
            break;
        }

        isSuc = ::Process32Next(hProcessSnap, &pe32);
    }

L_Cleanup:

    if (hProcessSnap)
    {
        ::CloseHandle(hProcessSnap);
    }

    return dwParentProcessId;
}

std::vector<_tstring> CProcessUtils::GetProcessModules(DWORD dwProcessId)
{
    std::vector<_tstring> modulesList;
    TCHAR szModName[MAX_PATH] = { 0 };
    HMODULE* phMods = NULL;
    HANDLE hProcess = NULL;
    DWORD cbNeeded = 0;
    DWORD dwModSize = 0;
    BOOL isSuc = FALSE;

    hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
    if (!hProcess)
    {
        goto L_Cleanup;
    }

    isSuc = EnumProcessModulesEx(hProcess, NULL, NULL, &cbNeeded, LIST_MODULES_ALL);
    dwModSize = cbNeeded * 2;

    phMods = (HMODULE*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwModSize);
    if (!phMods)
    {
        goto L_Cleanup;
    }

    if (!EnumProcessModulesEx(hProcess, phMods, dwModSize, &cbNeeded, LIST_MODULES_ALL))
    {
        goto L_Cleanup;
    }

    for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
    {
        if (GetModuleFileNameEx(hProcess, phMods[i], szModName, _countof(szModName)))
        {
            modulesList.push_back(szModName);
        }
    }

L_Cleanup:

    if (hProcess)
    {
        ::CloseHandle(hProcess);
    }

    if (phMods)
    {
        ::HeapFree(::GetProcessHeap(), 0, phMods);
    }

    return modulesList;
}

_tstring CProcessUtils::DosDevicePathToNtPath(const _tstring& strPath)
{
    _tstring strNtPath;
    TCHAR szDriveStrings[MAX_PATH] = { 0 };
    TCHAR szDosPath[MAX_PATH] = { 0 };
    TCHAR szNtPath[MAX_PATH] = { 0 };
    LPTSTR pDriveStr = NULL;

    if (!GetLogicalDriveStrings(_countof(szDriveStrings), szDriveStrings))
    {
        goto L_Cleanup;
    }

    for (int i = 0; i < _countof(szDriveStrings); i += 4)
    {
        pDriveStr = &szDriveStrings[i];
        pDriveStr[2] = _T('\0');

        if (!::QueryDosDevice(pDriveStr, szDosPath, _countof(szDosPath)))
        {
            break;
        }

        size_t nLen = _tcslen(szDosPath);

        if (0 == _tcsnicmp(strPath.c_str(), szDosPath, nLen))
        {
            lstrcpy(szNtPath, pDriveStr);
            lstrcat(szNtPath, strPath.c_str() + nLen);
            strNtPath = szNtPath;
            break;
        }
    }

L_Cleanup:

    return strNtPath;
}

PROC_TREE_INFO CProcessUtils::GetChildProcessList(DWORD dwParentPid)
{
    PROC_TREE_INFO infos;
    infos.dwProcessesTotal = 1;
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    BOOL isSuc = FALSE;

    hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcessSnap)
    {
        goto L_Cleanup;
    }

    pe32.dwSize = sizeof(pe32);
    isSuc = ::Process32First(hProcessSnap, &pe32);
    if (!isSuc)
    {
        goto L_Cleanup;
    }

    infos.dwProcessID = dwParentPid;

    while (isSuc)
    {
        if (pe32.th32ParentProcessID == dwParentPid && 0 != pe32.th32ParentProcessID)
        {
            PROC_TREE_INFO info = GetChildProcessList(pe32.th32ProcessID);
            infos.childProcesses.push_back(info);
            infos.dwProcessesTotal += info.dwProcessesTotal;
        }

        isSuc = ::Process32Next(hProcessSnap, &pe32);
    }

L_Cleanup:

    if (hProcessSnap)
    {
        ::CloseHandle(hProcessSnap);
    }

    return infos;
}

bool CProcessUtils::KillProcTree(PROC_TREE_INFO procTree)
{
    HANDLE hProcess = NULL;
    bool isSuc = false;

    //杀死子进程
    for (auto item: procTree.childProcesses)
    {
        KillProcTree(item);
    }

    //杀死进程
    hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, procTree.dwProcessID);
    if (NULL == hProcess)
    {
        return false;
    }

    isSuc = ::TerminateProcess(hProcess, 0);
    ::CloseHandle(hProcess);

    return isSuc;
}

void CProcessUtils::KillProcessTree(DWORD dwParentPid)
{
    PROC_TREE_INFO info = GetChildProcessList(dwParentPid);

    KillProcTree(info);

    return;
}
