// ViewControllerService.cpp : Implementation of CViewControllerService

#include "stdafx.h"
#include "ViewControllerService.h"
#include "Plugins.h"
#include "twitter_i.h"

// CViewControllerService

STDMETHODIMP CViewControllerService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	m_pServiceProvider = pServiceProvider;

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_InfoControlService, &m_pInfoControlService));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	RETURN_IF_FAILED(pThemeService->ApplyThemeFromSettings());

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThreadService, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_TimerService, &m_pTimerService));
	RETURN_IF_FAILED(m_pTimerService->StartTimer(30 * 1000)); //30 secs

	return S_OK;
}

STDMETHODIMP CViewControllerService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	m_pTimerService.Release();
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnStart(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pInfoControlService->HideControl());
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnFinish(IVariantObject *pResult)
{
	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(pTabbedControl->GetPage(0, &pControl));
	CComQIPtr<ITimelineControl> pTimelineControl = pControl;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_HRESULT_DESCRIPTION, &vDesc));
		CComBSTR bstrMsg = L"Unknown error";
		if (vDesc.vt == VT_BSTR)
			bstrMsg = vDesc.bstrVal;
		HWND hwndChildControl = 0;
		RETURN_IF_FAILED(pControl->GetHWND(&hwndChildControl));
		RETURN_IF_FAILED(m_pInfoControlService->ShowControl(hwndChildControl, bstrMsg, TRUE));
		return S_OK;
	}

	CComVariant v;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &v));
	CComQIPtr<IObjectArray> pObjectArray = v.punkVal;

	RETURN_IF_FAILED(pTimelineControl->SetItems(pObjectArray));
	return S_OK;
}