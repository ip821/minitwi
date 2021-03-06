#include "stdafx.h"
#include "ListsTimelineService.h"
#include "Plugins.h"
#include "twconn_contract_i.h"
#include "HomeTimelineService.h"
#include "UpdateScope.h"

STDMETHODIMP CListsTimelineService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceQueueService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &m_pTimelineQueueService));

	CComPtr<ITimelineLoadingService> pLoadingService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineLoadingService, &pLoadingService));
	RETURN_IF_FAILED(pLoadingService->SetText(L"Loading lists..."));

	return S_OK;
}

STDMETHODIMP CListsTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
	}

	m_pThreadServiceQueueService.Release();
	m_pTimelineQueueService.Release();
	m_pTimelineControl.Release();
	m_pThreadService.Release();
	m_pServiceProvider.Release();
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
	RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterKey, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterSecret, &bstrSecret));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->GetLists(&pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CListsTimelineService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CUpdateScope scope(m_pTimelineControl);
	RETURN_IF_FAILED(m_pTimelineControl->Clear());

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVar vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Result, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	if (uiCount)
	{
		RETURN_IF_FAILED(m_pTimelineQueueService->AddToQueue(pResult));
		RETURN_IF_FAILED(m_pThreadServiceQueueService->Run());
	}
	else
	{
		CComPtr<IVariantObject> pNoListsFoundObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pNoListsFoundObject));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::CustomTimelineObject)));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVar(L"No lists found")));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED_TEXT, &CComVar(L"No lists found")));
		RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVar(true)));
		RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pNoListsFoundObject, 0));
	}

	return S_OK;
}
