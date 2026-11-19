#include "CWmiUtils.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "wbemuuid.lib")

CWmiUtils::CWmiUtils():
	m_pSvc(NULL),
	m_pLoc(NULL),
    m_isInit(false)
{

}

CWmiUtils::~CWmiUtils()
{

}

bool CWmiUtils::Initialize()
{
	HRESULT hres = S_OK;

	if (m_isInit)
	{
		return true;
	}

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return false;
	}

	//hres = CoInitializeSecurity(
	//	NULL,
	//	-1,                          // COM authentication
	//	NULL,                        // Authentication services
	//	NULL,                        // Reserved
	//	RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
	//	RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
	//	NULL,                        // Authentication info
	//	EOAC_NONE,                   // Additional capabilities 
	//	NULL                         // Reserved
	//);


	if (FAILED(hres))
	{
		goto L_Cleanup;
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&m_pLoc);

	if (FAILED(hres))
	{
		goto L_Cleanup;
	}

	hres = m_pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&m_pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		goto L_Cleanup;
	}

	hres = CoSetProxyBlanket(
		m_pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		goto L_Cleanup;
	}

	m_isInit = true;

L_Cleanup:

	if (!m_isInit)
	{
		if (m_pSvc)
		{
			m_pSvc->Release();
			m_pSvc = NULL;
		}

		if (m_pLoc)
		{
			m_pLoc->Release();
			m_pLoc = NULL;
		}

		CoUninitialize();
	}

	return m_isInit;
}

void CWmiUtils::Uninitialize()
{
	if (m_isInit)
	{
		m_pSvc->Release();
		m_pSvc = NULL;
		m_pLoc->Release();
		m_pLoc = NULL;
		CoUninitialize();

		m_isInit = false;
	}
}

bool CWmiUtils::Query(LPCTSTR lpSql, std::function<void(IWbemClassObject* pclsObj)> mb)
{
	HRESULT hres = S_OK;
	TCHAR szWmiSql[MAX_PATH] = { 0 };
	IEnumWbemClassObject* pEnumerator = NULL;

	if (!m_isInit || !mb)
	{
		return false;
	}

	(void)StringCchPrintf(szWmiSql, _countof(szWmiSql), _T("%s"), lpSql);

	hres = m_pSvc->ExecQuery(
		bstr_t(_T("WQL")),
		bstr_t(szWmiSql),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (true)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (FAILED(hr))
        {
            break;
        }

		if (0 == uReturn)
		{
			break;
		}

		mb(pclsObj);

		pclsObj->Release();
	}

	pEnumerator->Release();
	pEnumerator = NULL;

	return true;
}

_tstring CWmiUtils::GetVariantString(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    _tstring strValue;
    HRESULT hr = S_OK;

    VARIANT vtProp;
    VariantInit(&vtProp);

    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && VT_BSTR == vtProp.vt)
    {
        strValue = vtProp.bstrVal;
    }

    VariantClear(&vtProp);

    return strValue;
}

LONG CWmiUtils::GetVariantLong(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    HRESULT hr = S_OK;
    LONG lValue = 0;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && VT_I4 == vtProp.vt)
    {
        lValue = vtProp.lVal;
    }

    VariantClear(&vtProp);

    return lValue;
}

INT64 CWmiUtils::GetVariantInt64(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    HRESULT hr = S_OK;
    INT64 i64Value = 0;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && VT_I8 == vtProp.vt)
    {
        i64Value = vtProp.lVal;
    }

    VariantClear(&vtProp);

    return i64Value;
}

INT8 CWmiUtils::GetVariantUInt8(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    HRESULT hr = S_OK;
    INT8 I8Value = 0;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && VT_UI1 == vtProp.vt)
    {
        I8Value = vtProp.bVal;
    }

    VariantClear(&vtProp);

    return I8Value;
}

SHORT CWmiUtils::GetVariantShort(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    HRESULT hr = S_OK;
    SHORT sValue = 0;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && VT_I2 == vtProp.vt)
    {
        sValue = vtProp.iVal;
    }

    VariantClear(&vtProp);

    return sValue;
}

BOOL CWmiUtils::GetVariantBool(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    HRESULT hr = S_OK;
    BOOL bValue = 0;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && VT_BOOL == vtProp.vt)
    {
        bValue = vtProp.boolVal;
    }

    VariantClear(&vtProp);

    return bValue;
}

std::vector<_tstring> CWmiUtils::GetStrArray(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    HRESULT hr = S_OK;
    std::vector<_tstring> strList;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (VT_ARRAY | VT_BSTR) == vtProp.vt)
    {
        SAFEARRAY *pArray =  vtProp.parray;
        BSTR HUGEP* pbstr;

        //获取数组维数
        UINT uDim = SafeArrayGetDim(pArray);

        LONG lLBound = 0;
        LONG lUBound = 0;

        //取得nDim维度的上界
        SafeArrayGetLBound(pArray, uDim, &lLBound);

        //取得nDim维度的下界
        SafeArrayGetUBound(pArray, uDim, &lUBound);

        SafeArrayAccessData(pArray, (void HUGEP**)&pbstr);
        for (int i = 0; i < lUBound - lLBound; i++)
        {
            strList.push_back(*pbstr);
            pbstr++;
        }

        SafeArrayUnaccessData(pArray);

    }
    VariantClear(&vtProp);

    return strList;
}

std::vector<short> CWmiUtils::GetShortArray(IWbemClassObject* pclsObj, LPCTSTR lpValueName)
{
    std::vector<short> shortList;
    HRESULT hr = S_OK;

    VARIANT vtProp;
    VariantInit(&vtProp);
    hr = pclsObj->Get(lpValueName, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (VT_ARRAY | VT_I2) == vtProp.vt)
    {
        SAFEARRAY* pArray = vtProp.parray;
        SHORT HUGEP* pShort;

        //获取数组维数
        UINT uDim = SafeArrayGetDim(pArray);

        LONG lLBound = 0;
        LONG lUBound = 0;

        //取得nDim维度的上界
        SafeArrayGetLBound(pArray, uDim, &lLBound);

        //取得nDim维度的下界
        SafeArrayGetUBound(pArray, uDim, &lUBound);

        SafeArrayAccessData(pArray, (void HUGEP**) & pShort);
        for (int i = 0; i < lUBound - lLBound; i++)
        {
            shortList.push_back(*pShort);
            pShort++;
        }

        SafeArrayUnaccessData(pArray);

    }
    VariantClear(&vtProp);

    return shortList;
}