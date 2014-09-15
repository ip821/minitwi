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

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	m_pTabbedControl = pContainerControl;
	RETURN_IF_FAILED(m_pTabbedControl->EnableCommands(FALSE));
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pTabbedControl->GetPage(0, &pControl));
	m_pTimelineControl = pControl;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(IInfoControlEventSink), &m_dwAdviceTabbedControl));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_UpdateService, &m_pUpdateService));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	RETURN_IF_FAILED(pThemeService->ApplyThemeFromSettings());

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &m_pThreadPoolService));
	RETURN_IF_FAILED(m_pThreadPoolService->SetThreadCount(6));
	RETURN_IF_FAILED(m_pThreadPoolService->Start());

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));
	RETURN_IF_FAILED(m_pThreadService->SetTimerService(SERVICE_TIMELINE_TIMER));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_TIMER, &m_pTimerService));
	RETURN_IF_FAILED(StartTimers());
	return S_OK;
}

STDMETHODIMP CViewControllerService::StartTimers()
{
	RETURN_IF_FAILED(m_pTimerService->StartTimer(60 * 1000)); //60 secs
	return S_OK;
}

STDMETHODIMP CViewControllerService::StopTimers()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	m_pTimerService.Release();
	m_pThreadPoolService.Release();
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(IInfoControlEventSink), m_dwAdviceTabbedControl));
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	m_pUpdateService.Release();
	m_pTabbedControl.Release();
	return S_OK;
}

STDMETHODIMP CViewControllerService::ShowControl(BSTR bstrMessage, BOOL bError)
{
	HWND hwndChildControl = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hwndChildControl));
	RETURN_IF_FAILED(m_pUpdateService->IsUpdateAvailable(&m_bUpdateAvailable));
	if (m_bUpdateAvailable)
	{
		RETURN_IF_FAILED(m_pTabbedControl->ShowInfo(FALSE, TRUE, L"Update available. Click here to install."));
	}
	else
	{
		BOOL bEmpty = FALSE;
		RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
		if ((bEmpty && !bError) || bError)
		{
			RETURN_IF_FAILED(m_pTabbedControl->ShowInfo(bError, FALSE,bstrMessage));
		}
	}
	return S_OK;
}

STDMETHODIMP CViewControllerService::HideControl()
{
	if (m_bUpdateAvailable)
		return S_OK;
	RETURN_IF_FAILED(m_pTabbedControl->HideInfo());
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnStart(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pTabbedControl->StartAnimation());
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnFinish(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pTabbedControl->StopAnimation());

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));

	if (SUCCEEDED(vHr.intVal))
	{
		RETURN_IF_FAILED(HideControl());
	}
	else
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT_DESCRIPTION, &vDesc));
		CComBSTR bstrMsg = L"Unknown error";
		if (vDesc.vt == VT_BSTR)
			bstrMsg = vDesc.bstrVal;
		RETURN_IF_FAILED(ShowControl(bstrMsg, TRUE));
		if (vHr.intVal == COMADMIN_E_USERPASSWDNOTVALID)
		{
			RETURN_IF_FAILED(pResult->SetVariantValue(KEY_RESTART_TIMER, &CComVariant(FALSE)));
			CComPtr<IFormManager> pFormManager;
			RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
			RETURN_IF_FAILED(pFormManager->ActivateForm(CLSID_SettingsControl));
		}
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CViewControllerService::OnLinkClick(HWND hWnd)
{
	HWND hwndChildControl = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hwndChildControl));
	if (hWnd != hwndChildControl)
		return S_OK;
	RETURN_IF_FAILED(m_pUpdateService->RunUpdate());
	return S_OK;
}

STDMETHODIMP CViewControllerService::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}