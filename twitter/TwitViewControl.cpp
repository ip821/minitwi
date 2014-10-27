#include "stdafx.h"
#include "TwitViewControl.h"
#include "Plugins.h"

STDMETHODIMP CTwitViewControl::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComQIPtr<IInitializeWithVariantObject> pInit = m_pTimelineService;
	ATLASSERT(pInit);
	RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObject));
	RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pVariantObject, 0));
	RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(0));
	//CComPtr<IThreadService> pTimelineThread;
	//RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pTimelineThread));
	//ATLASSERT(pTimelineThread);
	//RETURN_IF_FAILED(pTimelineThread->Run());

	//CComPtr<IThreadService> pParentTwitThread;
	//RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TWITVIEWPARENTTWITSERVICE_THREAD, &pParentTwitThread));
	//ATLASSERT(pParentTwitThread);
	//RETURN_IF_FAILED(pParentTwitThread->Run());

	return S_OK;
}

HRESULT CTwitViewControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CTwitViewControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	CComPtr<ITwitViewParentTwitService> pTwitViewParentTwitService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TwitViewParentTwitService, &pTwitViewParentTwitService));
	ATLASSERT(pTwitViewParentTwitService);
	RETURN_IF_FAILED(pTwitViewParentTwitService->SetTimelineControl(m_pTimelineControl));
	return S_OK;
}