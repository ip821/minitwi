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
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThreadService, &m_pThreadService));
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
	return S_OK;
}
