// UpdateService.cpp : Implementation of CUpdateService

#include "stdafx.h"
#include "UpdateService.h"
#include "Plugins.h"
#include "..\ObjMdl\StringUtils.h"
#include "..\ObjMdl\textfile.h"

// CUpdateService

const CComBSTR SETTINGS_PATH = L"Software\\IP\\Minitwi";
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
	m_pDownloadService.Release();
	return S_OK;
}

STDMETHODIMP CUpdateService::OnStart(IVariantObject *pResult)
{
	return S_OK;
}

STDMETHODIMP CUpdateService::OnRun(IVariantObject *pResult)
{
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		if (m_bUpdateAvailable)
			return S_OK;
	}

	CComPtr<IVariantObject> pDownloadTask;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(L"http://version.minitwi.googlecode.com/git/version.txt")));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Metadata::Types::SoftwareUpdateVersion)));
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
		auto res = regInstalledVersion.Open(HKEY_LOCAL_MACHINE, SETTINGS_PATH, KEY_QUERY_VALUE);
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
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Type, &vType));

	if (vType.vt != VT_BSTR)
		return S_OK;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	if (CComBSTR(vType.bstrVal) == CComBSTR(Twitter::Metadata::Types::SoftwareUpdateVersion))
	{
		CComVariant vUrl;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
		CComVariant vFilePath;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::Path, &vFilePath));

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
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Metadata::Types::SoftwareUpdateMsi)));
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::File::Extension, &CComVariant(L".msi")));
			RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));
			return S_OK;
		}
	}
	else if (CComBSTR(vType.bstrVal) == CComBSTR(Twitter::Metadata::Types::SoftwareUpdateMsi))
	{
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::File::KeepFileFlag, &CComVariant(true)));

		CComVariant vFilePath;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::Path, &vFilePath));

		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			m_bUpdateAvailable = TRUE;
			if (vFilePath.vt == VT_BSTR)
			{
				m_strUpdatePath = vFilePath.bstrVal;
			}
		}

		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CUpdateService::IsUpdateAvailable(BOOL* pbUpdateAvailable)
{
	CHECK_E_POINTER(pbUpdateAvailable);

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		*pbUpdateAvailable = m_bUpdateAvailable;
	}

	return S_OK;
}

STDMETHODIMP CUpdateService::RunUpdate()
{
	CString strUpdatePath;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		if (!m_bUpdateAvailable)
			return E_PENDING;
		strUpdatePath = m_strUpdatePath;
	}

	ShellExecute(NULL, NULL, strUpdatePath, NULL, NULL, 0);

	exit(0);

	return S_OK;
}
