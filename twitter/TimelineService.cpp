// TimelineService.cpp : Implementation of CTimelineService

#include "stdafx.h"
#include "TimelineService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twitconn_contract_i.h"
#include "UpdateScope.h"

// CTimelineService

#ifdef DEBUG
#define COUNT_ITEMS 10
#else
#define COUNT_ITEMS 100
#endif

STDMETHODIMP CTimelineService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		lock_guard<mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::SetTimelineControl(ITimelineControl* pTimelineControl)
{
	CHECK_E_POINTER(pTimelineControl);
	m_pTimelineControl = pTimelineControl;
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
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	return S_OK;
}

STDMETHODIMP CTimelineService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

	{
		std::lock_guard<std::mutex> lock(m_mutex);
	m_pSettings.Release();
	}

	m_pThreadServiceUpdateService.Release();
	m_pTimelineControl.Release();
	m_pSettings.Release();
	m_pThreadServiceShowMoreService.Release();;
	m_pServiceProvider.Release();

	return S_OK;
}

STDMETHODIMP CTimelineService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	BOOL bEmpty = FALSE;
	RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
	if (bEmpty)
	{
		UINT uiMaxCount = COUNT_ITEMS;
		if (m_bstrUser != L"")
			uiMaxCount = 20;
		RETURN_IF_FAILED(pResult->SetVariantValue(VAR_COUNT, &CComVariant(uiMaxCount)));
	}
	else
	{
		CComVariant vMaxId;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_MAX_ID, &vMaxId));
		CComPtr<IObjArray> pObjArray;
		RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pObjArray));
		if (vMaxId.vt == VT_EMPTY)
		{
			CComPtr<IVariantObject> pFirstItem;
			RETURN_IF_FAILED(pObjArray->GetAt(0, __uuidof(IVariantObject), (LPVOID*)&pFirstItem));
			CComVariant vId;
			RETURN_IF_FAILED(pFirstItem->GetVariantValue(VAR_ID, &vId));
			RETURN_IF_FAILED(pResult->SetVariantValue(VAR_SINCE_ID, &vId));
		}
		else
		{
			UINT uiCount = 0;
			RETURN_IF_FAILED(pObjArray->GetCount(&uiCount));
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pObjArray->GetAt(uiCount - 1, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_ITEM_DISABLED, &CComVariant(true)));
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(uiCount - 1));
		}
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::OnRun(IVariantObject* pResultObj)
{
	CHECK_E_POINTER(pResultObj);
	CoInitialize(NULL);

	CComPtr<IVariantObject> pResult = pResultObj;
	CComPtr<ISettings> pSettings;
	{
		lock_guard<mutex> lock(m_mutex);
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
	if (m_bstrUser == NULL)
	{
		RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));
	}
	else
	{
		RETURN_IF_FAILED(pConnection->OpenConnectionWithAppAuth());
	}

	CComVariant vMaxId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_MAX_ID, &vMaxId));

	CComVariant vSinceId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_SINCE_ID, &vSinceId));

	CComVariant vCount;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_COUNT, &vCount));

	CComPtr<IObjArray> pObjectArray;

	RETURN_IF_FAILED(pConnection->GetTimeline(
		m_bstrUser,
		vMaxId.vt == VT_BSTR ? vMaxId.bstrVal : NULL,
		vSinceId.vt == VT_BSTR ? vSinceId.bstrVal : NULL,
		vCount.vt == VT_UI4 ? vCount.uintVal : 0,
		&pObjectArray));

	RETURN_IF_FAILED(pResult->SetVariantValue(VAR_RESULT, &CComVariant(pObjectArray)));

	return S_OK;
}

STDMETHODIMP CTimelineService::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	CComVariant vUserScreenName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserScreenName));
	ATLASSERT(vUserScreenName.vt == VT_BSTR);
	m_bstrUser = vUserScreenName.bstrVal;
	return S_OK;
}

STDMETHODIMP CTimelineService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	m_bShowMoreRunning = FALSE;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
	{
		if (vHr.intVal == COMADMIN_E_USERPASSWDNOTVALID)
		{
			RETURN_IF_FAILED(pResult->SetVariantValue(KEY_RESTART_TIMER, &CComVariant(FALSE)));
		}

		CComVariant vId;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_MAX_ID, &vId));
		if (vId.vt == VT_BSTR)
		{
			CComPtr<IObjArray> pAllItems;
			RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
			UINT uiCount = 0;
			RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pAllItems->GetAt(uiCount - 1, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_ITEM_DISABLED, &CComVariant(false)));
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(uiCount - 1));
		}
		return S_OK;
	}

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	{
		CUpdateScope scope(m_pTimelineControl);

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
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_MAX_ID, &vId));
		if (vId.vt == VT_BSTR)
		{
			CComPtr<IObjArray> pAllItems;
			RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItems));
			UINT uiCount = 0;
			RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));
			if (uiCount > 1)
				insertIndex = uiCount - 2;

			RETURN_IF_FAILED(pAllItems->GetCount(&uiCount));
			CComPtr<IVariantObject> pVariantObject;
			RETURN_IF_FAILED(pAllItems->GetAt(uiCount - 1, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_ITEM_DISABLED, &CComVariant(false)));
			RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(uiCount - 1));
		}
		RETURN_IF_FAILED(UpdateRelativeTime(pObjectArray, m_tz));
		RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pObjectArray, insertIndex));
		CComPtr<IObjArray> pAllItemsObjectArray;
		RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItemsObjectArray));
		RETURN_IF_FAILED(UpdateRelativeTime(pAllItemsObjectArray, m_tz));
	}

	return S_OK;
}

HRESULT CTimelineService::UpdateRelativeTimeForTwit(IVariantObject* pVariantObject, TIME_ZONE_INFORMATION tz)
{
		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_CREATED_AT, &v));

		if (v.vt != VT_BSTR)
		return S_OK;

		local_time::wlocal_time_input_facet* inputFacet = new local_time::wlocal_time_input_facet();
		inputFacet->format(L"%a %b %d %H:%M:%S +0000 %Y");
		wistringstream inputStream;
		inputStream.imbue(locale(inputStream.getloc(), inputFacet));
		inputStream.str(wstring(v.bstrVal));
		posix_time::ptime pt;
		inputStream >> pt;

		static time_duration utcDiff;
		if (utcDiff.ticks() == 0)
		{
			ptime someUtcTime(date(2008, Jan, 1), time_duration(0, 0, 0, 0));
			ptime someLocalTime = date_time::c_local_adjustor<ptime>::utc_to_local(someUtcTime);
			utcDiff = someLocalTime - someUtcTime;
		}

		ptime ptCreatedAt(pt.date(), pt.time_of_day() + utcDiff);
		posix_time::time_duration diff(posix_time::second_clock::local_time() - ptCreatedAt);

		CString strRelTime;
		auto totalSeconds = abs(diff.total_seconds());
		if (totalSeconds < 60)
		{
			strRelTime = CString(lexical_cast<wstring>(totalSeconds).c_str()) + L"s";
		}
		else if (totalSeconds < 60 * 60)
		{
			strRelTime = CString(lexical_cast<wstring>(totalSeconds / 60).c_str()) + L"m";
		}
		else if (totalSeconds < 60 * 60 * 24)
		{
			strRelTime = CString(lexical_cast<wstring>(totalSeconds / 60 / 60).c_str()) + L"h";
		}
		else if (totalSeconds < 60 * 60 * 24 * 5)
		{
			strRelTime = CString(lexical_cast<wstring>(totalSeconds / 60 / 60 / 24).c_str()) + L"d";
		}
		else
		{
			posix_time::ptime ptNow(pt.date(), pt.time_of_day() - posix_time::minutes(tz.Bias) - posix_time::minutes(tz.DaylightBias));
			posix_time::wtime_facet* outputFacet = new posix_time::wtime_facet();
			outputFacet->format(L"%d %b %Y");
			wostringstream outputStream;
			outputStream.imbue(locale(outputStream.getloc(), outputFacet));
			outputStream << ptNow;
			strRelTime = outputStream.str().c_str();
		}

		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_RELATIVE_TIME, &CComVariant(strRelTime)));
	return S_OK;
	}

HRESULT CTimelineService::UpdateRelativeTime(IObjArray* pObjectArray, TIME_ZONE_INFORMATION tz)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
		RETURN_IF_FAILED(UpdateRelativeTimeForTwit(pVariantObject, tz));
	}
	return S_OK;
}

STDMETHODIMP CTimelineService::OnColumnClick(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject)
{
	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_COLUMN_SHOW_MORE) && !m_bShowMoreRunning)
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
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(VAR_MAX_ID, &vId));
			RETURN_IF_FAILED(pThreadContext->SetVariantValue(VAR_COUNT, &CComVariant((UINT)COUNT_ITEMS)));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->SetThreadContext(pThreadContext));
			RETURN_IF_FAILED(m_pThreadServiceShowMoreService->Run());
			m_bShowMoreRunning = TRUE;
		}
	}
	return S_OK;
}