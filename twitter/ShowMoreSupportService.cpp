// ShowMoreSupportService.cpp : Implementation of CShowMoreSupportService

#include "stdafx.h"
#include "ShowMoreSupportService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twconn_contract_i.h"
#include "UpdateScope.h"

// CShowMoreSupportService

STDMETHODIMP CShowMoreSupportService::OnInitialized(IServiceProvider *pServiceProvider)
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
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	return S_OK;
}

STDMETHODIMP CShowMoreSupportService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

	m_pThreadServiceUpdateService.Release();
	m_pTimelineControl.Release();
	m_pThreadServiceShowMoreService.Release();;
	m_pServiceProvider.Release();

	return S_OK;
}

STDMETHODIMP CShowMoreSupportService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CShowMoreSupportService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	m_bShowMoreRunning = FALSE;

	CComVar vId;
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
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVar(false)));
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItems(&pVariantObject.p, 1));
		}
	}

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVar vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Result, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	BOOL bEmpty = FALSE;
	RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));

	if (bEmpty)
	{
		CComPtr<IVariantObject> pShowMoreObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pShowMoreObject));
		RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::CustomTimelineObject)));
		RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVar(L"Show more")));
		RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED_TEXT, &CComVar(L"Loading...")));
		CComQIPtr<IObjCollection> pObjCollection = pObjectArray;
		RETURN_IF_FAILED(pObjCollection->AddObject(pShowMoreObject));
	}

	return S_OK;
}

STDMETHODIMP CShowMoreSupportService::OnColumnClick(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject)
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
			CComVar vId;
			RETURN_IF_FAILED(pVariantObjectItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));

			{
				CComPtr<IVariantObject> pVariantObjectItemShowMore;
				RETURN_IF_FAILED(pObjArray->GetAt(uiCount - 1, __uuidof(IVariantObject), (LPVOID*)&pVariantObjectItemShowMore));
				RETURN_IF_FAILED(pVariantObjectItemShowMore->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVar(true)));
				RETURN_IF_FAILED(m_pTimelineControl->RefreshItems(&pVariantObjectItemShowMore.p, 1));
				RETURN_IF_FAILED(m_pTimelineControl->InvalidateItems(&pVariantObjectItemShowMore.p, 1));
			}

			CComPtr<IVariantObject> pThreadContext;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pThreadContext));
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(Twitter::Metadata::Object::MaxId, &vId));
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(ObjectModel::Metadata::Object::Count, &CComVar((UINT)COUNT_ITEMS)));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->SetThreadContext(pThreadContext));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->Run());
			m_bShowMoreRunning = TRUE;
		}
	}
	return S_OK;
}