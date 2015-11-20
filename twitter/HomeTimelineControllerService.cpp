// HomeTimelineControllerService.cpp : Implementation of CHomeTimelineControllerService

#include "stdafx.h"
#include "HomeTimelineControllerService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twconn_contract_i.h"
#include "UpdateScope.h"

// CHomeTimelineControllerService

STDMETHODIMP CHomeTimelineControllerService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_STREAMING_THREAD, &m_pThreadServiceStreamingService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceStreamingService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceStreamingService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadServiceUpdateService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadServiceUpdateService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_TIMER, &m_pTimerService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_HomeTimelineStreamingService, &m_pHomeTimelineStreamingService));

	RETURN_IF_FAILED(m_pThreadServiceUpdateService->SetTimerService(SERVICE_TIMELINE_TIMER));

	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::OnInitCompleted()
{
	RETURN_IF_FAILED(StartConnection());
	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceStreamingService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceStreamingService));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadServiceUpdateService));
	m_pThreadServiceUpdateService.Release();
	m_pThreadServiceStreamingService.Release();
	m_pServiceProvider.Release();
	m_pTimerService.Release();
	m_pHomeTimelineStreamingService.Release();

	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::OnStart(IVariantObject* pResult)
{
	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::OnRun(IVariantObject* pResultObj)
{
	CHECK_E_POINTER(pResultObj);
	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	if (!m_running)
		return S_OK;

	CComVar vThreadId;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::Id, &vThreadId));
	CComQIPtr<IThreadService> pThreadService = vThreadId.punkVal;
	
	if (m_pThreadServiceUpdateService == pThreadService)
	{
		CComVar vHr;
		RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
		if (FAILED(vHr.intVal))
		{
			return S_OK;
		}

		RETURN_IF_FAILED(pResult->SetVariantValue(AsyncServices::Metadata::Timer::RestartFlag, &CComVar(FALSE)));
		RETURN_IF_FAILED(m_pTimerService->StopTimer());
		RETURN_IF_FAILED(m_pThreadServiceStreamingService->Run());
	}
	else if (m_pThreadServiceStreamingService)
	{
		RETURN_IF_FAILED(m_pTimerService->ResumeTimer());
	}

	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::StartConnection()
{
	m_running = true;
	RETURN_IF_FAILED(m_pTimerService->StartTimer(60 * 1000));
	return S_OK;
}

STDMETHODIMP CHomeTimelineControllerService::StopConnection()
{
	m_running = false;
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	RETURN_IF_FAILED(m_pThreadServiceUpdateService->Join());
	RETURN_IF_FAILED(m_pHomeTimelineStreamingService->Stop());
	RETURN_IF_FAILED(m_pThreadServiceStreamingService->Join());
	return S_OK;
}
