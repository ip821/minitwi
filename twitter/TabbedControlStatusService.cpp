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

    for (auto& it : m_guids)
    {
        ThreadHolder th;
        RETURN_IF_FAILED(pServiceProvider->QueryService(it, &th.m_pThreadService));
        if (th.m_pThreadService)
        {
            RETURN_IF_FAILED(AtlAdvise(th.m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &th.m_dwAdvice));
            m_advices[it] = th;
        }
    }

    RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ViewControllerService, &m_pViewControllerService));
    return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnShutdown()
{
    for (auto& it : m_advices)
    {
        RETURN_IF_FAILED(AtlUnadvise(it.second.m_pThreadService, __uuidof(IThreadServiceEventSink), it.second.m_dwAdvice));
    }
    RETURN_IF_FAILED(StopAnimation());
    m_advices.clear();
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

    auto it = m_advices.find(SERVICE_TIMELINE_STREAMING_THREAD);

    if (it == m_advices.cend() || it->second.m_pThreadService != pThreadService)
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
    auto it = m_advices.find(SERVICE_TIMELINE_STREAMING_THREAD);

    if (it == m_advices.cend() || it->second.m_pThreadService != pThreadService)
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
