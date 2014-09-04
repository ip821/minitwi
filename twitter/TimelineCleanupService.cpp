// TimelineCleanupService.cpp : Implementation of CTimelineCleanupService

#include "stdafx.h"
#include "TimelineCleanupService.h"
#include "Plugins.h"
#include "TimelineService.h"

// CTimelineCleanupService

#define MAX_ITEMS_COUNT 100
#define MAX_MINUTES 4

STDMETHODIMP CTimelineCleanupService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(CTimelineService::GetTimelineControl(m_pControl, m_pTimelineControl));
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_CLENUP_TIMER, &m_pTimerService));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerService, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimerService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	RETURN_IF_FAILED(m_pTimerService->StartTimer(1000 * 60)); //1 minute
	return S_OK;
}

STDMETHODIMP CTimelineCleanupService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerService, __uuidof(ITimerServiceEventSink), m_dwAdviceTimerService));
	m_pTimerService.Release();
	m_pTimelineControl.Release();
	m_pImageManagerService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineCleanupService::OnTimer()
{
	UINT uiTopIndex = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetTopVisibleItemIndex(&uiTopIndex));
	if (uiTopIndex < MAX_ITEMS_COUNT)
		m_counter++;

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

	return S_OK;
}

STDMETHODIMP CTimelineCleanupService::OnItemRemoved(IVariantObject *pItemObject)
{
	std::vector<std::wstring> urls;
	RETURN_IF_FAILED(CTimelineService::GetUrls(pItemObject, urls));
	for (auto& url : urls)
	{
		m_pImageManagerService->RemoveImage(CComBSTR(url.c_str()));
	}
	return S_OK;
}
