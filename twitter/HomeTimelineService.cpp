// HomeTimelineService.cpp : Implementation of CHomeTimelineService

#include "stdafx.h"
#include "HomeTimelineService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twitconn_contract_i.h"
#include "UpdateScope.h"

// CHomeTimelineService

#ifdef DEBUG
#define COUNT_ITEMS 10
#else
#define COUNT_ITEMS 100
#endif

#define CUSTOM_OBJECT_LOADING_ID 1

STDMETHODIMP CHomeTimelineService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CHomeTimelineService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceQueueService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &m_pTimelineQueueService));

	CComPtr<ITimelineLoadingService> pLoadingService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineLoadingService, &pLoadingService));
	RETURN_IF_FAILED(pLoadingService->SetText(L"Loading tweets..."));
	return S_OK;
}

STDMETHODIMP CHomeTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	m_pThreadServiceQueueService.Release();
	m_pTimelineQueueService.Release();
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
	}

	m_pThreadServiceUpdateService.Release();
	m_pTimelineControl.Release();
	m_pSettings.Release();
	m_pThreadServiceShowMoreService.Release();;
	m_pServiceProvider.Release();

	return S_OK;
}

STDMETHODIMP CHomeTimelineService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	BOOL bEmpty = FALSE;
	RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
	if (bEmpty)
	{
		UINT uiMaxCount = COUNT_ITEMS;
		RETURN_IF_FAILED(pResult->SetVariantValue(ObjectModel::Metadata::Object::Count, &CComVariant(uiMaxCount)));
	}
	else
	{
		CComVariant vMaxId;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vMaxId));
		CComPtr<IObjArray> pObjArray;
		RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pObjArray));
		if (vMaxId.vt == VT_EMPTY)
		{
			CComPtr<IVariantObject> pFirstItem;
			RETURN_IF_FAILED(pObjArray->GetAt(0, __uuidof(IVariantObject), (LPVOID*)&pFirstItem));
			CComVariant vId;
			RETURN_IF_FAILED(pFirstItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
			RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::SinceId, &vId));
		}
	}
	return S_OK;
}

STDMETHODIMP CHomeTimelineService::OnRun(IVariantObject* pResultObj)
{
	CHECK_E_POINTER(pResultObj);

	CComPtr<IVariantObject> pResult = pResultObj;
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

	CComVariant vMaxId;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vMaxId));

	CComVariant vSinceId;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::SinceId, &vSinceId));

	CComVariant vCount;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Count, &vCount));

	CComPtr<IObjArray> pObjectArray;

	RETURN_IF_FAILED(pConnection->GetTimeline(
		NULL,
		vMaxId.vt == VT_BSTR ? vMaxId.bstrVal : NULL,
		vSinceId.vt == VT_BSTR ? vSinceId.bstrVal : NULL,
		vCount.vt == VT_UI4 ? vCount.uintVal : 0,
		&pObjectArray));

	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CHomeTimelineService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		if (vHr.intVal == COMADMIN_E_USERPASSWDNOTVALID)
		{
			RETURN_IF_FAILED(pResult->SetVariantValue(AsyncServices::Metadata::Timer::RestartFlag, &CComVariant(FALSE)));
		}
		return S_OK;
	}

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Result, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	{
		CComVariant vId;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vId));
		if (vId.vt == VT_BSTR)
		{
			{ //Remove first element because it has MAX_ID identifier and exists in the timeline control as last element
				CComQIPtr<IObjCollection> pObjCollection = pObjectArray;
				ATLASSERT(pObjCollection);
				UINT uiObjectCount = 0;
				RETURN_IF_FAILED(pObjCollection->GetCount(&uiObjectCount));
				if (uiObjectCount)
				{
					RETURN_IF_FAILED(pObjCollection->RemoveObjectAt(0));
				}
			}
		}

		RETURN_IF_FAILED(m_pTimelineQueueService->AddToQueue(pResult));
		RETURN_IF_FAILED(m_pThreadServiceQueueService->Run());
	}

	return S_OK;
}
