// TimelineQueueService.cpp : Implementation of CTimelineQueueService

#include "stdafx.h"
#include "TimelineQueueService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twitconn_contract_i.h"
#include "UpdateScope.h"

// CTimelineQueueService

STDMETHODIMP CTimelineQueueService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadService));

	return S_OK;
}

STDMETHODIMP CTimelineQueueService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
	}

	m_pTimelineControl.Release();
	m_pServiceProvider.Release();

	return S_OK;
}

STDMETHODIMP CTimelineQueueService::OnStart(IVariantObject* pResult)
{
	return S_OK;
}

STDMETHODIMP CTimelineQueueService::OnRun(IVariantObject* pResultObj)
{
	CHECK_E_POINTER(pResultObj);
	return S_OK;
}

STDMETHODIMP CTimelineQueueService::OnFinish(IVariantObject* pThreadResult)
{
	CHECK_E_POINTER(pThreadResult);

	boost::lock_guard<boost::mutex> lock(m_mutex);

	CComPtr<IObjArray> pAllItems;
	RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
	UINT uiCount = 0;
	RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));

	hash_set<wstring> existingIds;
	for (UINT i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pItem;
		RETURN_IF_FAILED(pAllItems->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pItem));
		CComVariant vItemId;
		RETURN_IF_FAILED(pItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vItemId));
		if (vItemId.vt != VT_BSTR)
			continue;

		existingIds.insert(vItemId.bstrVal);
	}

	CUpdateScope updateScope(m_pTimelineControl);
	while (!m_queue.empty())
	{
		CComPtr<IVariantObject> pResult = m_queue.front();
		m_queue.pop();
		CComVariant vResult;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Result, &vResult));
		CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

		int insertIndex = 0;
		CComVariant vId;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vId));
		if (vId.vt == VT_BSTR)
		{
			if (uiCount)
				insertIndex = uiCount - 1;
		}

		CComVariant vIndex;
		RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Index, &vIndex));
		if (vIndex.vt == VT_I4)
		{
			insertIndex = vIndex.intVal;
		}

		CComPtr<IObjCollection> pFilteredCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pFilteredCollection));
		UINT uiArrayCount = 0;
		RETURN_IF_FAILED(pObjectArray->GetCount(&uiArrayCount));
		for (UINT i = 0; i < uiArrayCount; i++)
		{
			CComPtr<IVariantObject> pArrayItem;
			RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pArrayItem));
			CComVariant vArrayItemId;
			RETURN_IF_FAILED(pArrayItem->GetVariantValue(ObjectModel::Metadata::Object::Id, &vArrayItemId));
			if (vArrayItemId.vt == VT_BSTR && existingIds.find(vArrayItemId.bstrVal) != existingIds.end())
				continue;
			RETURN_IF_FAILED(pFilteredCollection->AddObject(pArrayItem));
		}

		RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pFilteredCollection, insertIndex));

		if (insertIndex)
		{
			if (uiCount)
			{
				RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(insertIndex));
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CTimelineQueueService::AddToQueue(IVariantObject *pResult)
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_queue.push(pResult);
	return S_OK;
}
