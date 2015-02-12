#include "stdafx.h"
#include "HomeTimeLineControl.h"
#include "Plugins.h"

STDMETHODIMP CHomeTimeLineControl::StartTimers()
{
	RETURN_IF_FAILED(m_pTimerService->StartTimer(60 * 1000)); //60 secs
	return S_OK;
}

STDMETHODIMP CHomeTimeLineControl::StopTimers()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	return S_OK;
}

STDMETHODIMP CHomeTimeLineControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Home").Detach();
	return S_OK;
}

HRESULT CHomeTimeLineControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CHomeTimeLineControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	CComPtr<IThreadService> pThreadServiceUpdateTimeline;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &pThreadServiceUpdateTimeline));
	RETURN_IF_FAILED(pThreadServiceUpdateTimeline->SetTimerService(SERVICE_TIMELINE_TIMER));
	return S_OK;
}
