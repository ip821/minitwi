#include "stdafx.h"
#include "TimelineLoadingService.h"
#include "UpdateScope.h"

#define CUSTOM_OBJECT_LOADING_ID 1

STDMETHODIMP CTimelineLoadingService::OnInitialized(IServiceProvider *pServiceProvider)
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
	if (m_pThreadServiceShowMoreService)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));
	}

	return S_OK;
}

STDMETHODIMP CTimelineLoadingService::OnShutdown()
{
	if (m_pThreadServiceShowMoreService)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	}
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

	m_pThreadServiceUpdateService.Release();
	m_pTimelineControl.Release();
	m_pThreadServiceShowMoreService.Release();;
	m_pServiceProvider.Release();

	return S_OK;
}

STDMETHODIMP CTimelineLoadingService::OnStart(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	BOOL bEmpty = FALSE;
	RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
	if (bEmpty)
	{
		CUpdateScope scope(m_pTimelineControl);
		CComPtr<IVariantObject> pLoadingObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pLoadingObject));
		RETURN_IF_FAILED(pLoadingObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::CustomTimelineObject)));
		RETURN_IF_FAILED(pLoadingObject->SetVariantValue(ObjectModel::Metadata::Object::Id, &CComVar(CUSTOM_OBJECT_LOADING_ID)));
		RETURN_IF_FAILED(pLoadingObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVar(m_strText)));
		RETURN_IF_FAILED(pLoadingObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED_TEXT, &CComVar(m_strText)));
		RETURN_IF_FAILED(pLoadingObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVar(true)));
		RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pLoadingObject, 0));
	}

	return S_OK;
}

STDMETHODIMP CTimelineLoadingService::OnFinish(IVariantObject *pResult)
{
	CComPtr<IObjArray> pItems;
	RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pItems));
	UINT uiCount = 0;
	RETURN_IF_FAILED(pItems->GetCount(&uiCount));
	if (uiCount)
	{
		CComPtr<IVariantObject> pFirstItem;
		RETURN_IF_FAILED(pItems->GetAt(0, __uuidof(IVariantObject), (LPVOID*)&pFirstItem));
		CComVar vId;
		RETURN_IF_FAILED(pFirstItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
		CComVar vObjectType;
		RETURN_IF_FAILED(pFirstItem->GetVariantValue(ObjectModel::Metadata::Object::Type, &vObjectType));
		if (vId.vt == VT_I4 && vObjectType.vt == VT_BSTR && vId.intVal == CUSTOM_OBJECT_LOADING_ID && CComBSTR(vObjectType.bstrVal) == Twitter::Metadata::Types::CustomTimelineObject)
		{
			CUpdateScope scope(m_pTimelineControl);
			RETURN_IF_FAILED(m_pTimelineControl->RemoveItemByIndex(0));
		}
	}

	return S_OK;
}

STDMETHODIMP CTimelineLoadingService::SetText(BSTR bstrText)
{
	CHECK_E_POINTER(bstrText);
	m_strText = bstrText;
	return S_OK;
}