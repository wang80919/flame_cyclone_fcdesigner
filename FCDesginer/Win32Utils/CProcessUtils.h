#pragma once

#include <wtypesbase.h>
#include <tchar.h>
#include <vector>
#include <string>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

typedef struct _PROC_TREE_INFO
{
    DWORD dwProcessID;      //进程ID
    DWORD dwProcessesTotal; //进程总计(当前进程 + 子进程数)
    std::vector<_PROC_TREE_INFO> childProcesses;    //子进程列表
}PROC_TREE_INFO;

class CProcessUtils
{
public:
    static bool RunProcess(const _tstring& strCmd = _T(""), const _tstring& strCurDir = _T(""), bool isShow = true, bool isWait = true);
	static _tstring GetProcessPath(DWORD dwProcessId, bool isNtPath = true);
	static _tstring GetParentProcessPath(DWORD dwProcessId, bool isNtPath = true);
	static DWORD GetParentProcessID(DWORD dwProcessId);
	static _tstring DosDevicePathToNtPath(const _tstring& strPath);
	static std::vector<_tstring> GetProcessModules(DWORD dwProcessId);
    static PROC_TREE_INFO GetChildProcessList(DWORD dwParentPid);
    static bool KillProcTree(PROC_TREE_INFO info);
    static void KillProcessTree(DWORD dwParentPid);
};

