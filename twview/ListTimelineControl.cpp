#include "stdafx.h"
#include "ListTimelineControl.h"
#include "Plugins.h"

HRESULT CListTimelineControl::Initializing()
{
    RETURN_IF_FAILED(__super::Initializing());
    RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
    return S_OK;
}

HRESULT CListTimelineControl::OnActivateInternal()
{
    __super::OnActivateInternal();
    if (!m_bActivated)
    {
        CComPtr<IThreadService> pThreadService;
        RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &pThreadService));
        RETURN_IF_FAILED(pThreadService->Run());
        m_bActivated = TRUE;
    }
    return S_OK;
}

STDMETHODIMP CListTimelineControl::SetVariantObject(IVariantObject* pVariantObject)
{
    RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pPluginSupport, pVariantObject));
    return S_OK;
}

HRESULT CListTimelineControl::ShuttingDown()
{
    DestroyWindow();
    return S_OK;
}