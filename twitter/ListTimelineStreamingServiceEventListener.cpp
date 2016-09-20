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

    CComPtr<IListMembershipService> pListTimelineService;
    RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ListMembershipService, &pListTimelineService));
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pListsService = pListTimelineService;
    }

    return S_OK;
}

STDMETHODIMP CListTimelineStreamingServiceEventListener::OnShutdown()
{
    RETURN_IF_FAILED(AtlUnadvise(m_pStreamingServicUnk, __uuidof(IHomeTimelineStreamingServiceEventSink), m_dwAdvice));

    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pStreamingServicUnk.Release();
        m_pListsService.Release();
        m_pServiceProvider.Release();
        m_pVariantObject.Release();
    }
    return S_OK;
}

STDMETHODIMP CListTimelineStreamingServiceEventListener::SetVariantObject(IVariantObject* pVariantObject)
{
    CHECK_E_POINTER(pVariantObject);
    m_pVariantObject = pVariantObject;
    return S_OK;
}


STDMETHODIMP CListTimelineStreamingServiceEventListener::OnMessages(IObjArray *pObjectArray)
{
    CComPtr<IListMembershipService> pListsService;
    CComPtr<IServiceProvider> pServiceProvider;
    CComPtr<IVariantObject> pListVariantObject;
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        pServiceProvider = m_pServiceProvider;
        pListsService = m_pListsService;
        pListVariantObject = m_pVariantObject;
    }

    if (!pListsService)
        return S_OK;

    CComVar vListId;
    RETURN_IF_FAILED(pListVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vListId));
    ATLASSERT(vListId.vt == VT_BSTR);

    CComPtr<IObjArray> pArrayMembers;
    RETURN_IF_FAILED(pListsService->GetListMemebers(vListId.bstrVal, &pArrayMembers));

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
                RETURN_IF_FAILED(HrVariantObjectGetBSTR(pMember, Twitter::Connection::Metadata::UserObject::Name, &bstrId));
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
            RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pObj));
            CComBSTR bstrId;
            RETURN_IF_FAILED(HrVariantObjectGetBSTR(pObj, Twitter::Connection::Metadata::UserObject::Name, &bstrId));
            if (m_userIds.find(bstrId.m_str) == m_userIds.cend())
                continue;
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
