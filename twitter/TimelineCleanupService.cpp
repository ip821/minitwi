// TimelineCleanupService.cpp : Implementation of CTimelineCleanupService

#include "stdafx.h"
#include "TimelineCleanupService.h"
#include "Plugins.h"
#include "TimelineService.h"

#define MAX_ITEMS_COUNT 50
#define MAX_MINUTES 4

// CTimelineCleanupService

STDMETHODIMP CTimelineCleanupService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_CLENUP_TIMER, &m_pTimerServiceCleanup));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerServiceCleanup, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimerServiceCleanup));

	RETURN_IF_FAILED(m_pTimerServiceCleanup->StartTimer(1000 * 60)); //1 minute
	return S_OK;
}

STDMETHODIMP CTimelineCleanupService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerServiceCleanup->StopTimer());
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerServiceCleanup, __uuidof(ITimerServiceEventSink), m_dwAdviceTimerServiceCleanup));
	m_pTimerServiceCleanup.Release();
	return S_OK;
}

STDMETHODIMP CTimelineCleanupService::OnTimer(ITimerService* pTimerService)
{
	RETURN_IF_FAILED(m_pTimerServiceCleanup->StopTimer());
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

	RETURN_IF_FAILED(m_pTimerServiceCleanup->ResumeTimer());

	return S_OK;
}

STDMETHODIMP CTimelineCleanupService::SetTimelineControl(ITimelineControl* pTimelineControl)
{
	CHECK_E_POINTER(pTimelineControl);
	m_pTimelineControl = pTimelineControl;
	return S_OK;
}