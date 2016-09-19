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

    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
    RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));

	CComPtr<ITimelineLoadingService> pLoadingService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineLoadingService, &pLoadingService));
	RETURN_IF_FAILED(pLoadingService->SetText(L"Loading list tweets..."));

	return S_OK;
}

STDMETHODIMP CListTimelineControlService::OnShutdown()
{
    RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
		m_pVariantObject.Release();
	}

    m_pThreadServiceShowMoreService.Release();
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
    BOOL bEmpty = FALSE;
    RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
    if (bEmpty)
    {
        UINT uiMaxCount = COUNT_ITEMS;
        RETURN_IF_FAILED(pResult->SetVariantValue(ObjectModel::Metadata::Object::Count, &CComVar(uiMaxCount)));
    }
    else
    {
        CComVar vMaxId;
        RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vMaxId));
        CComPtr<IObjArray> pObjArray;
        RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pObjArray));
        if (vMaxId.vt == VT_EMPTY)
        {
            CComPtr<IVariantObject> pFirstItem;
            RETURN_IF_FAILED(pObjArray->GetAt(0, __uuidof(IVariantObject), (LPVOID*)&pFirstItem));
            CComVar vId;
            RETURN_IF_FAILED(pFirstItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
            RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::SinceId, &vId));
        }
    }
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

    CComVar vMaxId;
    RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vMaxId));

    CComVar vSinceId;
    RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::SinceId, &vSinceId));

    CComVar vCount;
    RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Count, &vCount));

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->GetListTweets(
        vListId.bstrVal, 
        vMaxId.vt == VT_BSTR ? vMaxId.bstrVal : NULL,
        vSinceId.vt == VT_BSTR ? vSinceId.bstrVal : NULL,
        vCount.vt == VT_UI4 ? vCount.uintVal : 0,
        &pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectArray)));

    BOOL bNeedUpdateMembers = FALSE;
    {
        bNeedUpdateMembers = m_pObjectArrayMembers == nullptr;
    }

    if (bNeedUpdateMembers)
    {
        const auto maxMembersCount = 5000;
        CComPtr<IObjArray> pObjectArrayMembers;
        RETURN_IF_FAILED(pConnection->GetListMembers(vListId.bstrVal, maxMembersCount, &pObjectArrayMembers));
        {
            boost::lock_guard<boost::mutex> lock(m_mutex);
            m_pObjectArrayMembers = pObjectArrayMembers;
        }
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
    if (!m_pObjectArrayMembers)
        return S_OK;
    RETURN_IF_FAILED(m_pObjectArrayMembers->QueryInterface(ppArrayMembers));
    return S_OK;
}