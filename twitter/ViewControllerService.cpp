// ViewControllerService.cpp : Implementation of CViewControllerService

#include "stdafx.h"
#include "ViewControllerService.h"
#include "Plugins.h"
#include "twitter_i.h"

// CViewControllerService

STDMETHODIMP CViewControllerService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThreadService, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_TimerService, &m_pTimerService));
	RETURN_IF_FAILED(m_pTimerService->StartTimer(60 * 1000)); //60 secs
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	m_pTimerService.Release();
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pThreadService.Release();
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnFinish(IVariantObject *pResult)
{
	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	RETURN_IF_FAILED(vHr.intVal);

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(pTabbedControl->GetPage(0, &pControl));
	CComQIPtr<ITimelineControl> pTimelineControl = pControl;

	CComVariant v;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &v));
	CComQIPtr<IObjectArray> pObjectArray = v.punkVal;

	RETURN_IF_FAILED(pTimelineControl->SetItems(pObjectArray));

	return S_OK;
}