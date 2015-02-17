#include "stdafx.h"
#include "TimelineRelativeTimeService.h"
#include "Plugins.h"
#include "UpdateScope.h"

STDMETHODIMP CTimelineRelativeTimeService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreService));
	if (m_pThreadServiceShowMoreService)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceShowMoreService));
	}
	return S_OK;
}

STDMETHODIMP CTimelineRelativeTimeService::OnShutdown()
{
	if (m_dwAdviceThreadServiceShowMoreService)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceShowMoreService));
	}
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadService));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	m_pThreadServiceShowMoreService.Release();
	m_pThreadService.Release();
	m_pTimelineControl.Release();
	return S_OK;
}

STDMETHODIMP CTimelineRelativeTimeService::OnFinish(IVariantObject *pResult)
{
	CUpdateScope updateScope(m_pTimelineControl);

	CComPtr<IObjArray> pAllItemsObjectArray;
	RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItemsObjectArray));
	RETURN_IF_FAILED(UpdateRelativeTime(pAllItemsObjectArray));

	UINT uiCount = 0;
	RETURN_IF_FAILED(pAllItemsObjectArray->GetCount(&uiCount));
	if (uiCount)
	{
		vector<IVariantObject*> vObjects(uiCount);

		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> p;
			RETURN_IF_FAILED(pAllItemsObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&p));
			vObjects[i] = p.p;
		}

		RETURN_IF_FAILED(m_pTimelineControl->RefreshItems(&vObjects[0], vObjects.size()));
	}
	return S_OK;
}

HRESULT CTimelineRelativeTimeService::UpdateRelativeTimeForTwit(IVariantObject* pVariantObject)
{
	CComVariant v;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::CreatedAt, &v));

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
		posix_time::ptime ptNow(pt.date(), pt.time_of_day());
		posix_time::wtime_facet* outputFacet = new posix_time::wtime_facet();
		outputFacet->format(L"%d %b %Y");
		wostringstream outputStream;
		outputStream.imbue(locale(outputStream.getloc(), outputFacet));
		outputStream << ptNow;
		strRelTime = outputStream.str().c_str();
	}

	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Item::VAR_TWITTER_RELATIVE_TIME, &CComVariant(strRelTime)));
	return S_OK;
}

HRESULT CTimelineRelativeTimeService::UpdateRelativeTime(IObjArray* pObjectArray)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
		RETURN_IF_FAILED(UpdateRelativeTimeForTwit(pVariantObject));
	}
	return S_OK;
}
