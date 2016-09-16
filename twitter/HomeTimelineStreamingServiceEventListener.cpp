#include "stdafx.h"
#include "HomeTimelineStreamingServiceEventListener.h"

STDMETHODIMP CHomeTimelineStreamingServiceEventListener::OnInitialized(IServiceProvider *pServiceProvider)
{
    CHECK_E_POINTER(pServiceProvider);
    m_pServiceProvider = pServiceProvider;

    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
    RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_HomeTimelineStreamingService, &m_pStreamingServicUnk));
    RETURN_IF_FAILED(AtlAdvise(m_pStreamingServicUnk, pUnk, __uuidof(IHomeTimelineStreamingServiceEventSink), &m_dwAdvice));

    return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingServiceEventListener::OnShutdown()
{
    RETURN_IF_FAILED(AtlUnadvise(m_pStreamingServicUnk, __uuidof(IHomeTimelineStreamingServiceEventSink), m_dwAdvice));
    m_pStreamingServicUnk.Release();

    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pServiceProvider.Release();
    }
    return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingServiceEventListener::OnMessages(IObjArray *pObjectArray)
{
    CComPtr<IServiceProvider> pServiceProvider;
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        pServiceProvider = m_pServiceProvider;
    }

    CComPtr<ITimelineQueueService> pTimelineQueueService;
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &pTimelineQueueService));

    CComPtr<IVariantObject> pResult;
    RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pResult));
    RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectArray)));
    RETURN_IF_FAILED(pTimelineQueueService->AddToQueue(pResult));

    //temp, replace with timer
    CComPtr<IThreadService> pThreadService;
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadService));
    if (!pThreadService)
    	return S_OK;
    RETURN_IF_FAILED(pThreadService->Run());

    return S_OK;
}
