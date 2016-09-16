#include "stdafx.h"
#include "ListTimelineStreamingServiceEventListener.h"

STDMETHODIMP CListTimelineStreamingServiceEventListener::OnInitialized(IServiceProvider *pServiceProvider)
{
    CHECK_E_POINTER(pServiceProvider);
    m_pServiceProvider = pServiceProvider;

    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
    RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_HomeTimelineStreamingService, &m_pStreamingServicUnk));
    RETURN_IF_FAILED(AtlAdvise(m_pStreamingServicUnk, pUnk, __uuidof(IHomeTimelineStreamingServiceEventSink), &m_dwAdvice));

    CComPtr<IListTimelineControlService> pListTimelineService;
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE, &m_pListTimelineService));
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pListTimelineService = pListTimelineService;
    }

    return S_OK;
}

STDMETHODIMP CListTimelineStreamingServiceEventListener::OnShutdown()
{
    RETURN_IF_FAILED(AtlUnadvise(m_pStreamingServicUnk, __uuidof(IHomeTimelineStreamingServiceEventSink), m_dwAdvice));

    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pStreamingServicUnk.Release();
        m_pListTimelineService.Release();
        m_pServiceProvider.Release();
    }
    return S_OK;
}

STDMETHODIMP CListTimelineStreamingServiceEventListener::OnMessages(IObjArray *pObjectArray)
{
    CComPtr<IListTimelineControlService> pListTimelineService;
    CComPtr<IServiceProvider> pServiceProvider;
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        pServiceProvider = m_pServiceProvider;
        pListTimelineService = m_pListTimelineService;
    }

    if (!pListTimelineService)
        return S_OK;

    CComPtr<IObjArray> pArrayMembers;
    RETURN_IF_FAILED(pListTimelineService->GetListMemebers(&pArrayMembers));

    if (!pArrayMembers)
        return S_OK;

    { //Init user list
        boost::lock_guard<boost::mutex> lock(m_mutex);
        if (!m_userIds.size())
        {
            UINT uiCount = 0;
            RETURN_IF_FAILED(pArrayMembers->GetCount(&uiCount));
            for (size_t i = 0; i < uiCount; i++)
            {
                CComPtr<IVariantObject> pMember;
                RETURN_IF_FAILED(pArrayMembers->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMember));
                CComBSTR bstrId;
                RETURN_IF_FAILED(HrVariantObjectGetBSTR(pMember, L"id_str", &bstrId));
                m_userIds.insert(bstrId.m_str);
            }
        }
    }

    CComPtr<IObjCollection> pFilteredCollection;
    RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pFilteredCollection));
    { //Filter incoming tweets
        UINT uiCount = 0;
        RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
        for (size_t i = 0; i < uiCount; i++)
        {
            CComPtr<IVariantObject> pObj;
            RETURN_IF_FAILED(pArrayMembers->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pObj));
            if (false)
            {
                continue;
            }
            RETURN_IF_FAILED(pFilteredCollection->AddObject(pObj));
        }
    }

    CComPtr<ITimelineQueueService> pTimelineQueueService;
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &pTimelineQueueService));

    CComPtr<IVariantObject> pResult;
    RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pResult));
    RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pFilteredCollection)));
    RETURN_IF_FAILED(pTimelineQueueService->AddToQueue(pResult));

    //temp, replace with timer
    CComPtr<IThreadService> pThreadService;
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadService));
    if (!pThreadService)
        return S_OK;
    RETURN_IF_FAILED(pThreadService->Run());

    return S_OK;
}
