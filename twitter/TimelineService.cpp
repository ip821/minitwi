// TimelineService.cpp : Implementation of CTimelineService

#include "stdafx.h"
#include "TimelineService.h"
#include "Plugins.h"
#include "twitconn_contract_i.h"

// CTimelineService

STDMETHODIMP CTimelineService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pSettings.Release();
	m_pThreadService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CTimelineService::OnRun(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	CoInitialize(NULL);

	CComPtr<ISettings> pSettings;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, KEY_TWITTERKEY, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, KEY_TWITTERSECRET, &bstrSecret));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->GetHomeTimeline(NULL, &pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(VAR_RESULT, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CTimelineService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
		return S_OK;

	CComPtr<IImageManagerService> pImageManagerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &pImageManagerService));

	CComPtr<IDownloadService> pDownloadService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_DownloadService, &pDownloadService));

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_IMAGE, &v));
		if (v.vt == VT_BSTR)
		{
			TBITMAP b = { 0 };
			if (pImageManagerService->GetImage(v.bstrVal, &b) == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			{
				CComVariant vId;
				RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_ID, &vId));

				CComPtr<IVariantObject> pDownloadTask;
				RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
				RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(v.bstrVal)));
				RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_ID, &vId));
				RETURN_IF_FAILED(pDownloadService->AddDownload(pDownloadTask));
			}
		}
	}

	return S_OK;
}
