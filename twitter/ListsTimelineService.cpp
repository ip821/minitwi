#include "stdafx.h"
#include "ListsTimelineService.h"
#include "Plugins.h"
#include "twitconn_contract_i.h"
#include "TimelineService.h"
#include "UpdateScope.h"

STDMETHODIMP CListsTimelineService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	return S_OK;
}

STDMETHODIMP CListsTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
	}

	m_pTimelineControl.Release();
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	m_pSettings.Release();
	IInitializeWithControlImpl::OnShutdown();

	return S_OK;
}

STDMETHODIMP CListsTimelineService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CListsTimelineService::OnStart(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	RETURN_IF_FAILED(m_pTimelineControl->Clear());
	return S_OK;
}

STDMETHODIMP CListsTimelineService::OnRun(IVariantObject *pResult)
{
	CComPtr<CListsTimelineService> guard = this;
	CHECK_E_POINTER(pResult);
	CComPtr<ISettings> pSettings;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
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
	//RETURN_IF_FAILED(pConnection->Search(vText.bstrVal, NULL, COUNT_ITEMS, &pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(VAR_RESULT, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CListsTimelineService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hWnd));

	if (!IsWindowVisible(hWnd))
		return S_OK;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CUpdateScope scope(m_pTimelineControl);

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pObjectArray, 0));
	return S_OK;
}
