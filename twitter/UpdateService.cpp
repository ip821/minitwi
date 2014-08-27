// UpdateService.cpp : Implementation of CUpdateService

#include "stdafx.h"
#include "UpdateService.h"
#include "Plugins.h"
#include "..\ObjMdl\StringUtils.h"
#include "..\ObjMdl\textfile.h"

// CUpdateService

const CComBSTR REG_SETTINGS_PATH = L"Software\\IP\\Minitwi";
const CComBSTR HKEY_SETTINGS_KEY = L"InstalledVersion";

static void Parse(int result[4], const std::wstring& input)
{
	std::wistringstream parser(input);
	parser >> result[0];
	for (int idx = 1; idx < 4; idx++)
	{
		parser.get(); //Skip period
		parser >> result[idx];
	}
}

static bool LessThanVersion(const std::wstring& a, const std::wstring& b)
{
	int parsedA[4] = { 0 }, parsedB[4] = { 0 };
	Parse(parsedA, a);
	Parse(parsedB, b);
	return std::lexicographical_compare(parsedA, parsedA + 4, parsedB, parsedB + 4);
}

STDMETHODIMP CUpdateService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_UPDATES_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadService));
	RETURN_IF_FAILED(m_pThreadService->SetTimerService(SERVICE_UPDATES_TIMER));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdviceDownloadService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_UPDATES_TIMER, &m_pTimerService));
	RETURN_IF_FAILED(m_pTimerService->StartTimer(60 * 60 * 1000)); //1 hour

	return S_OK;
}

STDMETHODIMP CUpdateService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdviceDownloadService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadService));
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	RETURN_IF_FAILED(m_pThreadService->Join());
	m_pThreadService.Release();
	m_pTimerService.Release();
	return S_OK;
}

void CUpdateService::GetFolderPath(CString& strFolderPath)
{
	auto lpszBuffer = strFolderPath.GetBuffer(MAX_PATH);
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, lpszBuffer);
	strFolderPath.ReleaseBuffer();
	StrPathAppend(strFolderPath, L"minitwi\\Updates");
}

STDMETHODIMP CUpdateService::OnStart(IVariantObject *pResult)
{
	return S_OK;
}

void CUpdateService::GetSubDirs(CString strFolderPath, std::vector<std::wstring>& dirs)
{
	CString str = strFolderPath;
	StrPathRemoveBackslash(str);
	StrPathAppend(str, L"*");
	WIN32_FIND_DATA fi = { 0 };
	HANDLE h = FindFirstFileEx(
		str,
		FindExInfoStandard,
		&fi,
		FindExSearchLimitToDirectories,
		NULL,
		0);

	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				dirs.push_back(fi.cFileName);
			}
		} while (FindNextFile(h, &fi));
		FindClose(h);
	}

	std::sort(
		dirs.begin(),
		dirs.end(),
		[&](std::wstring& a, std::wstring& b)
	{
		return LessThanVersion(a, b);
	});
}

STDMETHODIMP CUpdateService::OnRun(IVariantObject *pResult)
{
	CComPtr<IVariantObject> pDownloadTask;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(L"http://version.minitwi.googlecode.com/git/version.txt")));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(L"TYPE_SOFTWARE_UPDATE_VERSION")));
	RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));
	return S_OK;
}

STDMETHODIMP CUpdateService::OnFinish(IVariantObject *pResult)
{
	return S_OK;
}

CString CUpdateService::GetInstalledVersion()
{
	CString strInstalledVersion;
	{
		CRegKey regInstalledVersion;
		auto res = regInstalledVersion.Open(HKEY_LOCAL_MACHINE, REG_SETTINGS_PATH, KEY_QUERY_VALUE);
		if (res == S_OK)
		{
			auto lpszInstalledVersion = strInstalledVersion.GetBuffer(MAX_PATH);
			ULONG ulRead = MAX_PATH;
			regInstalledVersion.QueryStringValue(HKEY_SETTINGS_KEY, lpszInstalledVersion, &ulRead);
			strInstalledVersion.ReleaseBuffer();
		}
	}
	return strInstalledVersion;
}

STDMETHODIMP CUpdateService::OnDownloadComplete(IVariantObject *pResult)
{
	CComVariant vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_OBJECT_TYPE, &vType));

	if (vType.vt != VT_BSTR)
		return S_OK;

	if (CComBSTR(vType.bstrVal) == CComBSTR(L"TYPE_SOFTWARE_UPDATE_VERSION"))
	{
		CComVariant vUrl;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
		CComVariant vFilePath;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));

		CString strVersion;
		CTextFile::ReadAllText(vFilePath.bstrVal, strVersion);
		CString strInstalledVersion = GetInstalledVersion();
		if (LessThanVersion(std::wstring(strInstalledVersion), std::wstring(strVersion)))
		{
			CComPtr<IVariantObject> pDownloadTask;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
#ifdef __WINXP__
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(L"http://version.minitwi.googlecode.com/git/Release_XP/Setup.msi")));
#else
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(L"http://version.minitwi.googlecode.com/git/Release/Setup.msi")));
#endif
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(L"TYPE_SOFTWARE_UPDATE_MSI")));
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_FILE_EXT, &CComVariant(L".msi")));
			RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));
			return S_OK;
		}
	}
	else if (CComBSTR(vType.bstrVal) == CComBSTR(L"TYPE_SOFTWARE_UPDATE_MSI"))
	{
		RETURN_IF_FAILED(pResult->SetVariantValue(VAR_KEEP_FILE, &CComVariant(true)));
	
		CComVariant vFilePath;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));

		ShellExecute(NULL, NULL, vFilePath.bstrVal, NULL, NULL, 0);
		PostMessage(m_hControlWnd, WM_CLOSE, 0, 0);
	
		return S_OK;
	}
	return S_OK;
}