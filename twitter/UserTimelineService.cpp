// UserTimelineService.cpp : Implementation of CUserTimelineService

#include "stdafx.h"
#include "UserTimelineService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twitconn_contract_i.h"
#include "UpdateScope.h"

// CUserTimelineService

#ifdef DEBUG
#define COUNT_ITEMS 10
#else
#define COUNT_ITEMS 20
#endif

STDMETHODIMP CUserTimelineService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CUserTimelineService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	CComPtr<ITimelineLoadingService> pLoadingService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineLoadingService, &pLoadingService));
	RETURN_IF_FAILED(pLoadingService->SetText(L"Loading tweets..."));

	return S_OK;
}

STDMETHODIMP CUserTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

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

STDMETHODIMP CUserTimelineService::OnStart(IVariantObject* pResult)
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
		else
		{
			UINT uiCount = 0;
			RETURN_IF_FAILED(pObjArray->GetCount(&uiCount));
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pObjArray->GetAt(uiCount - 1, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVariant(true)));
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(uiCount - 1));
			RETURN_IF_FAILED(m_pTimelineControl->InvalidateItems(&pVariantObject.p, 1));
		}
	}
	return S_OK;
}

STDMETHODIMP CUserTimelineService::OnRun(IVariantObject* pResultObj)
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

	RETURN_IF_FAILED(pConnection->OpenConnectionWithAppAuth());

	CComVariant vMaxId;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vMaxId));

	CComVariant vSinceId;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::SinceId, &vSinceId));

	CComVariant vCount;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Count, &vCount));

	CComPtr<IObjArray> pObjectArray;

	RETURN_IF_FAILED(pConnection->GetTimeline(
		m_bstrUser,
		vMaxId.vt == VT_BSTR ? vMaxId.bstrVal : NULL,
		vSinceId.vt == VT_BSTR ? vSinceId.bstrVal : NULL,
		vCount.vt == VT_UI4 ? vCount.uintVal : 0,
		&pObjectArray));

	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CUserTimelineService::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	CComVariant vUserScreenName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserScreenName));
	ATLASSERT(vUserScreenName.vt == VT_BSTR);
	m_bstrUser = vUserScreenName.bstrVal;
	return S_OK;
}

STDMETHODIMP CUserTimelineService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	m_bShowMoreRunning = FALSE;

	{
		CComVariant vId;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vId));
		if (vId.vt == VT_BSTR)
		{
			CComPtr<IObjArray> pAllItems;
			RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
			UINT uiCount = 0;
			RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));
			if (uiCount)
			{
				CComPtr<IVariantObject> pVariantObject;
				RETURN_IF_FAILED(pAllItems->GetAt(uiCount - 1, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
				RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVariant(false)));
				RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(uiCount - 1));
			}
		}
	}

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
		BOOL bEmpty = FALSE;
		RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));

		if (bEmpty)
		{
			CComPtr<IVariantObject> pShowMoreObject;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pShowMoreObject));
			RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Metadata::Types::CustomTimelineObject)));
			RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVariant(L"Show more")));
			RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED_TEXT, &CComVariant(L"Show more")));
			CComQIPtr<IObjCollection> pObjCollection = pObjectArray;
			RETURN_IF_FAILED(pObjCollection->AddObject(pShowMoreObject));
		}

		int insertIndex = 0;
		CComVariant vId;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vId));
		if (vId.vt == VT_BSTR)
		{
			CComPtr<IObjArray> pAllItems;
			RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
			UINT uiCount = 0;
			RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));
			if (uiCount)
				insertIndex = uiCount - 1;

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

		UINT uiCurrentTopIndex = 0;
		RETURN_IF_FAILED(m_pTimelineControl->GetTopVisibleItemIndex(&uiCurrentTopIndex));
		RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pObjectArray, insertIndex));
		if (insertIndex)
		{
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(insertIndex));
		}
		else
		{
			UINT uiCount = 0;
			RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
			if (uiCurrentTopIndex)
			{
				uiCurrentTopIndex += uiCount;
			}
			RETURN_IF_FAILED(m_pTimelineControl->ScrollToItem(uiCurrentTopIndex));
		}
	}

	return S_OK;
}

STDMETHODIMP CUserTimelineService::OnColumnClick(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject)
{
	CComBSTR bstrColumnName;
	RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName));
	if (CComBSTR(bstrColumnName) == Twitter::Metadata::Column::ShowMoreColumn && !m_bShowMoreRunning)
	{
		CComPtr<IObjArray> pObjArray;
		RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pObjArray));
		UINT uiCount = 0;
		RETURN_IF_FAILED(pObjArray->GetCount(&uiCount));
		if (uiCount > 1)
		{
			CComPtr<IVariantObject> pVariantObjectItem;
			RETURN_IF_FAILED(pObjArray->GetAt(uiCount - 2, __uuidof(IVariantObject), (LPVOID*)&pVariantObjectItem));
			CComVariant vId;
			RETURN_IF_FAILED(pVariantObjectItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));

			CComPtr<IVariantObject> pThreadContext;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pThreadContext));
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(Twitter::Metadata::Object::MaxId, &vId));
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(ObjectModel::Metadata::Object::Count, &CComVariant((UINT)COUNT_ITEMS)));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->SetThreadContext(pThreadContext));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->Run());
			m_bShowMoreRunning = TRUE;
		}
	}
	return S_OK;
}