#include "stdafx.h"
#include "TabbedControlStatusService.h"

#include "Plugins.h"
#include "twmdl_i.h"

// CTabbedControlStatusService

STDMETHODIMP CTabbedControlStatusService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ViewControllerService, &m_pViewControllerService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreTimeline));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FOLLOW_THREAD, &m_pThreadServiceFollow));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FOLLOW_THREAD, &m_pThreadServiceFollowStatus));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_GETUSER_THREAD, &m_pThreadServiceGetUser));
	
	if (m_pThreadServiceFollowStatus)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceFollowStatus, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceFollowStatus));
	}

	if (m_pThreadServiceShowMoreTimeline)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceShowMoreTimeline));
	}

	if (m_pThreadServiceFollow)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceFollow, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceFollow));
	}

	if (m_pThreadServiceGetUser)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceGetUser, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceGetUser));
	}

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadServiceUpdateTimeline));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceUpdateTimeline));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_STREAMING_THREAD, &m_pThreadServiceStreamingTimeline));
	if (m_pThreadServiceStreamingTimeline)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceStreamingTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceStreamingTimeline));
	}

	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnShutdown()
{
	if (m_pThreadServiceGetUser)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceGetUser, __uuidof(IThreadServiceEventSink), m_dwAdviceGetUser));
	}

	if (m_pThreadServiceStreamingTimeline)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceStreamingTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceStreamingTimeline));
	}

	if (m_dwAdviceShowMoreTimeline)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceShowMoreTimeline));
	}
	
	if (m_dwAdviceFollow)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceFollow, __uuidof(IThreadServiceEventSink), m_dwAdviceFollow));
	}

	if (m_dwAdviceFollowStatus)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceFollowStatus, __uuidof(IThreadServiceEventSink), m_dwAdviceFollowStatus));
	}

	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceUpdateTimeline));
	RETURN_IF_FAILED(StopAnimation());
	m_pThreadServiceShowMoreTimeline.Release();
	m_pThreadServiceUpdateTimeline.Release();
	m_pViewControllerService.Release();
	m_pThreadServiceFollow.Release();
	m_pThreadServiceFollowStatus.Release();
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::StartAnimation()
{
	m_cAnimationRefs++;
	RETURN_IF_FAILED(m_pViewControllerService->StartAnimation());
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::StopAnimation()
{
	if (!m_cAnimationRefs)
		return S_OK;

	--m_cAnimationRefs;

	RETURN_IF_FAILED(m_pViewControllerService->StopAnimation());
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnStart(IVariantObject *pResult)
{
	CComVar vThreadId;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::Id, &vThreadId));
	CComQIPtr<IThreadService> pThreadService = vThreadId.punkVal;
	if (m_pThreadServiceStreamingTimeline != pThreadService)
	{
		RETURN_IF_FAILED(m_pViewControllerService->HideInfo());
		RETURN_IF_FAILED(StartAnimation());
	}
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnFinish(IVariantObject *pResult)
{
	CComVar vThreadId;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::Id, &vThreadId));
	CComQIPtr<IThreadService> pThreadService = vThreadId.punkVal;
	if (m_pThreadServiceStreamingTimeline != pThreadService)
	{
		RETURN_IF_FAILED(StopAnimation());
	}

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));

	if (FAILED(vHr.intVal))
	{
		CComVar vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResultDescription, &vDesc));
		CComBSTR bstrMsg = L"Unknown error";
		if (vDesc.vt == VT_BSTR)
			bstrMsg = vDesc.bstrVal;

		RETURN_IF_FAILED(m_pViewControllerService->ShowInfo(vHr.intVal, TRUE, FALSE, bstrMsg));
		return S_OK;
	}
	else
	{
		RETURN_IF_FAILED(m_pViewControllerService->ShowInfo(vHr.intVal, TRUE, FALSE, L""));
	}
	return S_OK;
}
