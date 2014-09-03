// TimelineService.cpp : Implementation of CTimelineService

#include "stdafx.h"
#include "TimelineService.h"
#include "Plugins.h"
#include "twitconn_contract_i.h"
#include "UpdateScope.h"
#include <boost\date_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
// CTimelineService

STDMETHODIMP CTimelineService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	m_tz = { 0 };
	GetTimeZoneInformation(&m_tz);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdviceDownloadService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(pTabbedControl->GetPage(0, &pControl));
	m_pTimelineControl = pControl;
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_UPDATEIMAGES_TIMER, &m_pTimerService));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerService, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimerService));
	m_pTimerService->StartTimer(300);

	return S_OK;
}

STDMETHODIMP CTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdviceDownloadService));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerService, __uuidof(ITimerServiceEventSink), m_dwAdviceTimerService));
	m_pTimerService.Release();
	m_pDownloadService.Release();
	m_pImageManagerService.Release();
	m_pSettings.Release();
	m_pThreadServiceUpdateService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineService::OnTimer()
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

STDMETHODIMP CTimelineService::OnDownloadComplete(IVariantObject *pResult)
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

	RETURN_IF_FAILED(m_pImageManagerService->SetImage(vUrl.bstrVal, vFilePath.bstrVal));

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_idsToUpdate.insert(vId.bstrVal);
	}

	return S_OK;
}

STDMETHODIMP CTimelineService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CTimelineService::OnRun(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	CoInitialize(NULL);

	CComPtr<ISettings> pSettings;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, KEY_TWITTERKEY, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, KEY_TWITTERSECRET, &bstrSecret));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));

	CComVariant vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ID, &vId));

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->GetHomeTimeline(vId.vt == VT_BSTR ? vId.bstrVal : NULL, &pObjectArray));
	RETURN_IF_FAILED(pResult->SetVariantValue(VAR_RESULT, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CTimelineService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
		return S_OK;

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	{
		UpdateScope scope(m_pTimelineControl);

		BOOL bEmpty = FALSE;
		RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
		if (bEmpty)
		{
			CComPtr<IVariantObject> pShowMoreObject;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pShowMoreObject));
			RETURN_IF_FAILED(pShowMoreObject->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(TYPE_SHOWMORE_OBJECT)));
			CComQIPtr<IObjCollection> pObjCollection = pObjectArray;
			RETURN_IF_FAILED(pObjCollection->AddObject(pShowMoreObject));
		}

		int insertIndex = 0;
		CComVariant vId;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ID, &vId));
		if (vId.vt == VT_BSTR)
		{
			CComPtr<IObjArray> pAllItems;
			RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
			UINT uiCount = 0;
			RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));
			if (uiCount > 1)
				insertIndex = uiCount - 2;
		}
		RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pObjectArray, insertIndex));
		RETURN_IF_FAILED(ProcessAllItems());
	}

	return S_OK;
}

STDMETHODIMP CTimelineService::ProcessAllItems()
{
	CComPtr<IObjArray> pAllItemsObjectArray;
	RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItemsObjectArray));
	RETURN_IF_FAILED(ProcessUrls(pAllItemsObjectArray));
	RETURN_IF_FAILED(UpdateRelativeTime(pAllItemsObjectArray));
	RETURN_IF_FAILED(m_pTimelineControl->OnItemsUpdated());
	return S_OK;
}

STDMETHODIMP CTimelineService::UpdateRelativeTime(IObjArray* pObjectArray)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_CREATED_AT, &v));

		if (v.vt != VT_BSTR)
			continue;

		boost::local_time::wlocal_time_input_facet* inputFacet = new boost::local_time::wlocal_time_input_facet();
		inputFacet->format(L"%a %b %d %H:%M:%S +0000 %Y");
		std::wistringstream inputStream;
		inputStream.imbue(std::locale(inputStream.getloc(), inputFacet));
		inputStream.str(std::wstring(v.bstrVal));
		boost::posix_time::ptime pt;
		inputStream >> pt;

		boost::posix_time::ptime ptCreatedAt(pt.date(), pt.time_of_day() - boost::posix_time::minutes(m_tz.Bias) - boost::posix_time::minutes(m_tz.DaylightBias));

		boost::posix_time::time_duration diff(boost::posix_time::second_clock::local_time() - ptCreatedAt);

		CString strRelTime;
		auto totalSeconds = diff.total_seconds();
		if (totalSeconds < 60)
		{
			strRelTime = CString(boost::lexical_cast<std::wstring>(totalSeconds).c_str()) + L"s";
		}
		else if (totalSeconds < 60 * 60)
		{
			strRelTime = CString(boost::lexical_cast<std::wstring>(totalSeconds / 60).c_str()) + L"m";
		}
		else if (totalSeconds < 60 * 60 * 60 * 24)
		{
			strRelTime = CString(boost::lexical_cast<std::wstring>(totalSeconds / 60 / 60).c_str()) + L"h";
		}
		else if (totalSeconds < 60 * 60 * 60 * 24 * 5)
		{
			strRelTime = CString(boost::lexical_cast<std::wstring>(totalSeconds / 60 / 60 / 24).c_str()) + L"d";
		}

		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_RELATIVE_TIME, &CComVariant(strRelTime)));
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::ProcessUrls(IObjArray* pObjectArray)
{
	std::hash_set<std::wstring> urls;

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
			BOOL bContains = TRUE;
			m_pImageManagerService->ContainsImageKey(CComBSTR(url.c_str()), &bContains);
			if (!bContains)
			{
				if (urls.find(url) == urls.end())
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

STDMETHODIMP CTimelineService::GetUrls(IVariantObject* pVariantObject, std::vector<std::wstring>& urls)
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

STDMETHODIMP CTimelineService::OnItemRemoved(IVariantObject *pItemObject)
{
	std::vector<std::wstring> urls;
	RETURN_IF_FAILED(GetUrls(pItemObject, urls));
	for (auto& url : urls)
	{
		m_pImageManagerService->RemoveImage(CComBSTR(url.c_str()));
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::OnColumnClick(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject)
{
	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_COLUMN_SHOW_MORE))
	{
		CComPtr<IObjArray> pObjArray;
		RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pObjArray));
		UINT uiCount = 0;
		RETURN_IF_FAILED(pObjArray->GetCount(&uiCount));
		if (uiCount > 1)
		{
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pObjArray->GetAt(uiCount - 2, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
			CComVariant vId;
			RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_ID, &vId));

			CComPtr<IVariantObject> pThreadContext;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pThreadContext));
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(VAR_ID, &vId));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->SetThreadContext(pThreadContext));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->Run());
		}
	}
	return S_OK;
}