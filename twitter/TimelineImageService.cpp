// TimelineImageService.cpp : Implementation of CTimelineImageService

#include "stdafx.h"
#include "TimelineImageService.h"
#include "Plugins.h"
#include "TimelineService.h"

// CTimelineImageService

#define MAX_ITEMS_COUNT 100
#define MAX_MINUTES 4

STDMETHODIMP CTimelineImageService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(CTimelineService::GetTimelineControl(m_pControl, m_pTimelineControl));
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_CLENUP_TIMER, &m_pTimerServiceCleanup));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerServiceCleanup, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimerServiceCleanup));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdviceDownloadService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_UPDATEIMAGES_TIMER, &m_pTimerServiceUpdate));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerServiceUpdate, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimerServiceUpdate));

	RETURN_IF_FAILED(m_pTimerServiceUpdate->StartTimer(300));
	RETURN_IF_FAILED(m_pTimerServiceCleanup->StartTimer(1000 * 60)); //1 minute
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerServiceCleanup->StopTimer());
	RETURN_IF_FAILED(m_pTimerServiceUpdate->StopTimer());
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdviceDownloadService));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerServiceCleanup, __uuidof(ITimerServiceEventSink), m_dwAdviceTimerServiceCleanup));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerServiceUpdate, __uuidof(ITimerServiceEventSink), m_dwAdviceTimerServiceUpdate));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	m_pThreadServiceShowMoreService.Release();
	m_pThreadServiceUpdateService.Release();
	m_pTimerServiceCleanup.Release();
	m_pTimerServiceUpdate.Release();
	m_pTimelineControl.Release();
	m_pImageManagerService.Release();
	m_pDownloadService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnTimer(ITimerService* pTimerService)
{
	if (pTimerService == m_pTimerServiceCleanup)
	{
		UINT uiTopIndex = 0;
		RETURN_IF_FAILED(m_pTimelineControl->GetTopVisibleItemIndex(&uiTopIndex));
		if (uiTopIndex < MAX_ITEMS_COUNT)
			m_counter++;
		else
			m_counter = 0;

		if (m_counter > MAX_MINUTES)
		{ //cleanup
			UINT uiCount = 0;
			RETURN_IF_FAILED(m_pTimelineControl->GetItemsCount(&uiCount));
			while (uiCount > MAX_ITEMS_COUNT)
			{
				RETURN_IF_FAILED(m_pTimelineControl->RemoveItemByIndex(uiCount - 2)); //remove last but not "show more"
				uiCount--;
			}
			m_counter = 0;
		}
	}
	else if (pTimerService == m_pTimerServiceUpdate)
	{
		std::hash_set<std::wstring> ids;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			ids = std::hash_set<std::wstring>(m_idsToUpdate);
			m_idsToUpdate.clear();
		}

		if (ids.size())
		{
			RETURN_IF_FAILED(m_pTimelineControl->Invalidate());
		}

		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnItemRemoved(IVariantObject *pItemObject)
{
	std::vector<std::wstring> urls;
	RETURN_IF_FAILED(GetUrls(pItemObject, urls));
	for (auto& url : urls)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			ATLASSERT(m_imageRefs.find(url) != m_imageRefs.end());
			m_imageRefs[url]--;
			if (m_imageRefs[url] <= 0)
			{
				m_pImageManagerService->RemoveImage(CComBSTR(url.c_str()));
				m_imageRefs.erase(url);
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnStart(IVariantObject *pResult)
{
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnRun(IVariantObject *pResult)
{
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnFinish(IVariantObject *pResult)
{
	CComPtr<IObjArray> pAllItems;
	RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
	RETURN_IF_FAILED(ProcessUrls(pAllItems));
	RETURN_IF_FAILED(m_pTimelineControl->OnItemsUpdated());
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnDownloadComplete(IVariantObject *pResult)
{
	CComVariant vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_OBJECT_TYPE, &vType));

	if (vType.vt != VT_BSTR || CComBSTR(vType.bstrVal) != CComBSTR(TYPE_IMAGE))
		return S_OK;

	CComVariant vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ID, &vId));
	CComVariant vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_imageRefs.find(vUrl.bstrVal) != m_imageRefs.end())
		{
			RETURN_IF_FAILED(m_pImageManagerService->AddImage(vUrl.bstrVal, vFilePath.bstrVal));
		}

		m_idsToUpdate.insert(vId.bstrVal);
	}

	return S_OK;
}

STDMETHODIMP CTimelineImageService::ProcessUrls(IObjArray* pObjectArray)
{
	{
		lock_guard<mutex> lock(m_mutex);
		m_imageRefs.clear();

		hash_set<std::wstring> urls;

		UINT uiCount = 0;
		RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

			CComVariant vId;
			RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_ID, &vId));

			std::vector<std::wstring> itemUrls;
			RETURN_IF_FAILED(GetUrls(pVariantObject, itemUrls));

			for (auto& url : itemUrls)
			{
				if (m_imageRefs.find(url) == m_imageRefs.end())
					m_imageRefs[url] = 0;
				m_imageRefs[url]++;

				BOOL bContains = TRUE;
				CComBSTR bstrImageKey(url.c_str());
				m_pImageManagerService->ContainsImageKey(bstrImageKey, &bContains);
				if (urls.find(url) == urls.end() && !bContains)
				{
					CComPtr<IVariantObject> pDownloadTask;
					RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(url.c_str())));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_ID, &vId));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(TYPE_IMAGE)));
					RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));
					urls.insert(url);
				}
			}
		}
	}
	return S_OK;
}

HRESULT CTimelineImageService::GetUrls(IVariantObject* pVariantObject, std::vector<std::wstring>& urls)
{
	CComVariant vUserImage;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_IMAGE, &vUserImage));
	if (vUserImage.vt == VT_BSTR)
	{
		urls.push_back(vUserImage.bstrVal);
	}

	CComVariant vMediaUrls;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_MEDIAURLS, &vMediaUrls));
	if (vMediaUrls.vt == VT_UNKNOWN)
	{
		CComQIPtr<IObjArray> pObjArray = vMediaUrls.punkVal;
		UINT_PTR uiCount = 0;
		pObjArray->GetCount(&uiCount);
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pMediaObject;
			pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

			CComVariant vMediaUrl;
			pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL_THUMB, &vMediaUrl);

			if (vMediaUrl.vt == VT_BSTR)
			{
				urls.push_back(vMediaUrl.bstrVal);
			}
		}
	}
	return S_OK;
}
