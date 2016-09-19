#include "stdafx.h"
#include "ListsService.h"

STDMETHODIMP CListsService::OnInitialized(IServiceProvider *pServiceProvider)
{
    m_pServiceProvider = pServiceProvider;

    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_LISTS_THREAD, &m_pThreadService));
    RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadService));
    RETURN_IF_FAILED(m_pThreadService->SetTimerService(SERVICE_LISTS_TIMER));

    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_LISTS_TIMER, &m_pTimerService));
    RETURN_IF_FAILED(m_pTimerService->StartTimer(10 * 60 * 1000)); //10 min

    return S_OK;
}

STDMETHODIMP CListsService::OnShutdown() 
{
    RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadService));
    RETURN_IF_FAILED(m_pTimerService->StopTimer());
    RETURN_IF_FAILED(m_pThreadService->Join());
    m_pServiceProvider.Release();
    return S_OK;
}

STDMETHODIMP CListsService::GetListMemebers(IObjArray** ppArrayMembers)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnStart(IVariantObject* pObject)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnRun(IVariantObject* pObject)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnFinish(IVariantObject* pObject)
{
    return S_OK;
}
