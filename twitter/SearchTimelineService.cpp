#include "stdafx.h"
#include "SearchTimelineService.h"
#include "Plugins.h"
#include "twitconn_contract_i.h"
#include "TimelineService.h"
#include "UpdateScope.h"

#ifdef DEBUG
#define COUNT_ITEMS 10
#else
#define COUNT_ITEMS 100
#endif

STDMETHODIMP CSearchTimelineService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CSearchTimelineService::OnShutdown()
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
	m_pVariantObject.Release();
	IInitializeWithControlImpl::OnShutdown();

	return S_OK;
}

STDMETHODIMP CSearchTimelineService::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CSearchTimelineService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CSearchTimelineService::OnStart(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	RETURN_IF_FAILED(m_pTimelineControl->Clear());

	CUpdateScope scope(m_pTimelineControl);
	CComPtr<IVariantObject> pLoadingObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pLoadingObject));
	RETURN_IF_FAILED(pLoadingObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(TYPE_CUSTOM_TIMELINE_OBJECT)));
	RETURN_IF_FAILED(pLoadingObject->SetVariantValue(VAR_TEXT, &CComVariant(L"Searching...")));
	RETURN_IF_FAILED(pLoadingObject->SetVariantValue(VAR_ITEM_DISABLED_TEXT, &CComVariant(L"Searching...")));
	RETURN_IF_FAILED(pLoadingObject->SetVariantValue(VAR_ITEM_DISABLED, &CComVariant(true)));
	RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pLoadingObject, 0));

	return S_OK;
}

STDMETHODIMP CSearchTimelineService::OnRun(IVariantObject *pResult)
{
	CComPtr<ISearchTimelineService> guard = this;
	CComPtr<IVariantObject> pVariantObjectMember = m_pVariantObject;
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
	RETURN_IF_FAILED(pConnection->OpenConnectionWithAppAuth());

	CComVariant vText;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(VAR_TEXT, &vText));
	ATLASSERT(vText.vt == VT_BSTR);

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->Search(vText.bstrVal, NULL, COUNT_ITEMS, &pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(VAR_RESULT, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CSearchTimelineService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CUpdateScope scope(m_pTimelineControl);
	RETURN_IF_FAILED(m_pTimelineControl->Clear());

	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hWnd));

	if (!IsWindowVisible(hWnd))
		return S_OK;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	if (uiCount)
	{
		RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pObjectArray, 0));
	}
	else
	{
		CComPtr<IVariantObject> pNoListsFoundObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pNoListsFoundObject));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(TYPE_CUSTOM_TIMELINE_OBJECT)));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(VAR_TEXT, &CComVariant(L"No tweets found")));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(VAR_ITEM_DISABLED_TEXT, &CComVariant(L"No tweets found")));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(VAR_ITEM_DISABLED, &CComVariant(true)));
		RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pNoListsFoundObject, 0));
	}

	return S_OK;
}
