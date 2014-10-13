#include "stdafx.h"
#include "TabbedControlStatusService.h"

#include "Plugins.h"
#include "twitter_i.h"

// CTabbedControlStatusService

STDMETHODIMP CTabbedControlStatusService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	if (pMainWindow)
	{
		CComPtr<IContainerControl> pContainerControl;
		RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
		m_pTabbedControl = pContainerControl;
	}
	else
	{
		m_pTabbedControl = m_pControl;
	}

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadServiceShowMoreTimeline));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceShowMoreTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceShowMoreTimeline));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateTimeline));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceUpdateTimeline, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceUpdateTimeline));

	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceShowMoreTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceShowMoreTimeline));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceUpdateTimeline, __uuidof(IThreadServiceEventSink), m_dwAdviceUpdateTimeline));
	RETURN_IF_FAILED(StopAnimation());
	m_pThreadServiceShowMoreTimeline.Release();
	m_pThreadServiceUpdateTimeline.Release();
	m_pTabbedControl.Release();
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::StopAnimation()
{
	if (m_bAnimating)
	{
		RETURN_IF_FAILED(m_pTabbedControl->StopAnimation());
		m_bAnimating = FALSE;
	}
	return S_OK;
}

STDMETHODIMP CTabbedControlStatusService::OnStart(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pTabbedControl->HideInfo());
	RETURN_IF_FAILED(m_pTabbedControl->StartAnimation());
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

		RETURN_IF_FAILED(m_pTabbedControl->ShowInfo(TRUE, FALSE, bstrMsg));
		return S_OK;
	}

	return S_OK;
}
