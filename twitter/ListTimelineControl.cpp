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
	CComPtr<IThreadService> pThreadService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadService));
	RETURN_IF_FAILED(pThreadService->Run());
	return S_OK;
}