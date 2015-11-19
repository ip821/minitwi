#include "stdafx.h"
#include "FollowingControl.h"
#include "Plugins.h"

HRESULT CFollowingControl::Initializing()
{
    RETURN_IF_FAILED(__super::Initializing());
    RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
    return S_OK;
}

HRESULT CFollowingControl::OnActivateInternal()
{
    __super::OnActivateInternal();

    BOOL bEmpty = FALSE;
    RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
    if (bEmpty)
    {
        CComPtr<IThreadService> pThreadService;
        RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &pThreadService));
        RETURN_IF_FAILED(pThreadService->Run());
    }

    return S_OK;
}

STDMETHODIMP CFollowingControl::SetVariantObject(IVariantObject* pVariantObject)
{
    RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pPluginSupport, pVariantObject));
    return S_OK;
}

HRESULT CFollowingControl::ShuttingDown()
{
    DestroyWindow();
    return S_OK;
}