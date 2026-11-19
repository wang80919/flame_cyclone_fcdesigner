#pragma once
#include <Wbemidl.h>
#include <functional>

#include <string>
#include <vector>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CWmiUtils
{
public:

	CWmiUtils();
	~CWmiUtils();

	bool Initialize();
	void Uninitialize();
	bool Query(LPCTSTR lpSql, std::function<void(IWbemClassObject *pclsObj)> mb);

    static _tstring GetVariantString(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static LONG GetVariantLong(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static SHORT GetVariantShort(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static BOOL GetVariantBool(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static INT64 GetVariantInt64(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static INT8 GetVariantUInt8(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static std::vector<_tstring> GetStrArray(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
    static std::vector<short> GetShortArray(IWbemClassObject* pclsObj, LPCTSTR lpValueName);
private:
	bool m_isInit;
	IWbemServices* m_pSvc;
	IWbemLocator* m_pLoc;
};
