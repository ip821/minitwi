#include "stdafx.h"
#include "ListTimelineControlService.h"
#include "Plugins.h"
#include "twconn_contract_i.h"
#include "HomeTimelineService.h"
#include "UpdateScope.h"

STDMETHODIMP CListTimelineControlService::OnInitialized(IServiceProvider* pServiceProvider)
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
	RETURN_IF_FAILED(pLoadingService->SetText(L"Loading list tweets..."));

	return S_OK;
}

STDMETHODIMP CListTimelineControlService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
		m_pVariantObject.Release();
	}

	m_pThreadServiceQueueService.Release();
	m_pTimelineQueueService.Release();
	m_pTimelineControl.Release();
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	IInitializeWithControlImpl::OnShutdown();

	return S_OK;
}

STDMETHODIMP CListTimelineControlService::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CListTimelineControlService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CListTimelineControlService::OnStart(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	RETURN_IF_FAILED(m_pTimelineControl->Clear());
	return S_OK;
}

STDMETHODIMP CListTimelineControlService::OnRun(IVariantObject *pResult)
{
	CComPtr<CListTimelineControlService> guard = this;
	CHECK_E_POINTER(pResult);
	CComPtr<ISettings> pSettings;
	CComPtr<IVariantObject> pListVariantObject;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pSettings = m_pSettings;
		pListVariantObject = m_pVariantObject;
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

	CComVar vListId;
	RETURN_IF_FAILED(pListVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vListId));
	ATLASSERT(vListId.vt == VT_BSTR);

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->GetListTweets(vListId.bstrVal, COUNT_ITEMS, &pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectArray)));

    const auto maxMembersCount = 5000;
    CComPtr<IObjArray> pObjectArrayMembers;
    RETURN_IF_FAILED(pConnection->GetListMembers(vListId.bstrVal, maxMembersCount, &pObjectArrayMembers));

    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pObjectArrayMembers = pObjectArrayMembers;
    }

	return S_OK;
}

STDMETHODIMP CListTimelineControlService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hWnd));

	if (!IsWindowVisible(hWnd))
		return S_OK;

	CUpdateScope scope(m_pTimelineControl);
	//RETURN_IF_FAILED(m_pTimelineControl->Clear());

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	RETURN_IF_FAILED(m_pTimelineQueueService->AddToQueue(pResult));
	RETURN_IF_FAILED(m_pThreadServiceQueueService->Run());
	return S_OK;
}

STDMETHODIMP CListTimelineControlService::GetListMemebers(IObjArray** ppArrayMembers)
{
    CHECK_E_POINTER(ppArrayMembers);
    boost::lock_guard<boost::mutex> lock(m_mutex);
    RETURN_IF_FAILED(m_pObjectArrayMembers->QueryInterface(ppArrayMembers));
    return S_OK;
}