// TimelineImageService.cpp : Implementation of CTimelineImageService

#include "stdafx.h"
#include "TimelineImageService.h"
#include "Plugins.h"
#include "TimelineService.h"
#include "UpdateScope.h"
#include "GdilPlusUtils.h"

// CTimelineImageService

STDMETHODIMP CTimelineImageService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceUpdateService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
	if (m_pThreadServiceShowMoreService)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));
	}

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdviceDownloadService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_UPDATEIMAGES_TIMER, &m_pTimerServiceUpdate));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerServiceUpdate, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimerServiceUpdate));

	RETURN_IF_FAILED(m_pTimerServiceUpdate->StartTimer(300));
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnShutdown()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	RETURN_IF_FAILED(m_pTimerServiceUpdate->StopTimer());
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdviceDownloadService));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerServiceUpdate, __uuidof(ITimerServiceEventSink), m_dwAdviceTimerServiceUpdate));
	if (m_dwAdviceThreadServiceShowMoreService)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	}
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	m_pThreadServiceShowMoreService.Release();
	m_pThreadServiceUpdateService.Release();
	m_pTimerServiceUpdate.Release();
	m_pTimelineControl.Release();
	m_pImageManagerService.Release();
	m_pDownloadService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnTimer(ITimerService* pTimerService)
{
	RETURN_IF_FAILED(m_pTimerServiceUpdate->StopTimer());
	hash_set<IVariantObject*> ids;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		ids = hash_set<IVariantObject*>(m_idsToUpdate);
		m_idsToUpdate.clear();
	}

	if (ids.size())
	{
		vector<IVariantObject*> v(ids.begin(), ids.end());
		{
			CTopVisibleItemScope topVisibleItemScope(m_pTimelineControl);
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItems(&v[0], v.size()));
			RETURN_IF_FAILED(m_pTimelineControl->InvalidateItems(&v[0], v.size()));
		}
	}
	RETURN_IF_FAILED(m_pTimerServiceUpdate->ResumeTimer());
	return S_OK;
}

STDMETHODIMP CTimelineImageService::OnItemRemoved(IVariantObject *pItemObject)
{
	vector<wstring> urls;
	RETURN_IF_FAILED(GetUrls(pItemObject, urls));
	for (auto& url : urls)
	{
		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			ATLASSERT(m_imageRefs.find(url) != m_imageRefs.end());
			m_imageRefs[url].erase(pItemObject);
			if (m_imageRefs[url].size() == 0)
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
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Type, &vType));

	if (vType.vt != VT_BSTR || CComBSTR(vType.bstrVal) != Twitter::Metadata::Types::Image)
		return S_OK;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComPtr<IImageManagerService> pImageManagerService;
	CComPtr<ITimelineControl> pTimelineControl;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		if (!m_pImageManagerService)
			return S_OK;
		pImageManagerService = m_pImageManagerService;
		pTimelineControl = m_pTimelineControl;
	}

	CComVariant vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
	CComVariant vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Url, &vUrl));
	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::Path, &vFilePath));
	CComVariant vItemIndex;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Item::VAR_ITEM_INDEX, &vItemIndex));

	UINT uiCount = 0;
	RETURN_IF_FAILED(pTimelineControl->GetItemsCount(&uiCount));

	if (uiCount)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		auto it = m_imageRefs.find(vUrl.bstrVal);
		if (it != m_imageRefs.end())
		{
			auto pBitmap = std::make_shared<Gdiplus::Bitmap>(vFilePath.bstrVal);
			auto height = pBitmap->GetHeight();
			if (height > TIMELINE_IMAGE_HEIGHT)
			{
				ResizeDownImage(pBitmap, TIMELINE_IMAGE_HEIGHT);
				CBitmap bitmap;
				pBitmap->GetHBITMAP(Color::Transparent, &bitmap.m_hBitmap);
				RETURN_IF_FAILED(pImageManagerService->AddImageFromHBITMAP(vUrl.bstrVal, bitmap));
			}
			else
			{
				RETURN_IF_FAILED(pImageManagerService->AddImageFromFile(vUrl.bstrVal, vFilePath.bstrVal));
			}
			for (auto& item : it->second)
			{
				m_idsToUpdate.insert(item);
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CTimelineImageService::ProcessUrls(IObjArray* pObjectArray)
{
	CDownloadSuspendScope downloadSuspendScope(m_pDownloadService);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_imageRefs.clear();

		hash_set<wstring> urls;

		UINT uiCount = 0;
		RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

			CComVariant vId;
			RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));

			vector<wstring> itemUrls;
			RETURN_IF_FAILED(GetUrls(pVariantObject, itemUrls));

			for (auto& url : itemUrls)
			{
				if (m_imageRefs.find(url) == m_imageRefs.end())
					m_imageRefs[url].clear();
				m_imageRefs[url].insert(pVariantObject.p);

				BOOL bContains = TRUE;
				CComBSTR bstrImageKey(url.c_str());
				ASSERT_IF_FAILED(m_pImageManagerService->ContainsImageKey(bstrImageKey, &bContains));
				if (urls.find(url) == urls.end() && !bContains)
				{
					CComPtr<IVariantObject> pDownloadTask;
					RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Object::Url, &CComVariant(url.c_str())));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_INDEX, &CComVariant(i)));
					RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Metadata::Types::Image)));
					RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));
					urls.insert(url);
				}
			}
		}
	}
	return S_OK;
}

HRESULT CTimelineImageService::GetUrls(IVariantObject* pVariantObject, vector<wstring>& urls)
{
	CComVariant vUserImage;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &vUserImage));
	if (vUserImage.vt == VT_BSTR)
	{
		urls.push_back(vUserImage.bstrVal);
	}

	CComVariant vMediaUrls;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls));
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
			pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlThumb, &vMediaUrl);

			if (vMediaUrl.vt == VT_BSTR)
			{
				urls.push_back(vMediaUrl.bstrVal);
			}
		}
	}
	return S_OK;
}
