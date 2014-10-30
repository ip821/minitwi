#include "stdafx.h"
#include "TabbedControlStatusService.h"

#include "Plugins.h"
#include "twitter_i.h"

// CTabbedControlStatusService

STDMETHODIMP CTabbedControlStatusService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ViewControllerService, &m_pViewControllerService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreTimeline));
	if (m_pThreadServiceShowMoreTimeline)
	{
		RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceShowMoreTimeline));
	}

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateTimeline));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceUpdateTimeline));

	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnShutdown()
{
	if (m_dwAdviceShowMoreTimeline)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceShowMoreTimeline));
	}
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceUpdateTimeline));
	RETURN_IF_FAILED(StopAnimation());
	m_pThreadServiceShowMoreTimeline.Release();
	m_pThreadServiceUpdateTimeline.Release();
	m_pViewControllerService.Release();
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::StopAnimation()
{
	if (m_bAnimating)
	{
		RETURN_IF_FAILED(m_pViewControllerService->StopAnimation());
		m_bAnimating = FALSE;
	}
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnStart(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pViewControllerService->HideInfo());
	RETURN_IF_FAILED(m_pViewControllerService->StartAnimation());
	m_bAnimating = TRUE;
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnFinish(IVariantObject *pResult)
{
	RETURN_IF_FAILED(StopAnimation());

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));

	if (FAILED(vHr.intVal))
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT_DESCRIPTION, &vDesc));
		CComBSTR bstrMsg = L"Unknown error";
		if (vDesc.vt == VT_BSTR)
			bstrMsg = vDesc.bstrVal;

		RETURN_IF_FAILED(m_pViewControllerService->ShowInfo(vHr.intVal, TRUE, FALSE, bstrMsg));
		return S_OK;
	}

	return S_OK;
}