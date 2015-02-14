// ViewControllerService.cpp : Implementation of CViewControllerService

#include "stdafx.h"
#include "ViewControllerService.h"
#include "Plugins.h"
#include "twitter_i.h"
#include <curl/curl.h>

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

	curl_global_init(CURL_GLOBAL_DEFAULT); //it's not thread safe, that's why it is called here

	m_pServiceProvider = pServiceProvider;

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	ATLASSERT(pMainWindow);

	RETURN_IF_FAILED(pMainWindow->GetMessageLoop(&m_pMessageLoop));
	RETURN_IF_FAILED(m_pMessageLoop->AddMessageFilter(this));

	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	m_pTabbedControl = pContainerControl;
	RETURN_IF_FAILED(m_pTabbedControl->EnableCommands(FALSE));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	
	RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(IInfoControlEventSink), &m_dwAdviceTabbedControl));
	RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(ITabbedControlEventSink), &m_dwAdviceTabbedControl2));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_UpdateService, &m_pUpdateService));
	
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORMS_SERVICE, &m_pFormsService));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	RETURN_IF_FAILED(pThemeService->ApplyThemeFromSettings());

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &m_pThreadPoolService));
	RETURN_IF_FAILED(m_pThreadPoolService->SetThreadCount(2));
	RETURN_IF_FAILED(m_pThreadPoolService->Start());

	return S_OK;
}

STDMETHODIMP CViewControllerService::OnInitCompleted()
{
	CComPtr<IFormManager> pFormManager;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
	CComPtr<IHomeTimelineControllerService> pHomeTimelineControllerService;
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnShutdown()
{
	RETURN_IF_FAILED(m_pMessageLoop->RemoveMessageFilter(this));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(ITabbedControlEventSink), m_dwAdviceTabbedControl2));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(IInfoControlEventSink), m_dwAdviceTabbedControl));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	m_pMessageLoop.Release();
	m_pFormsService.Release();
	m_pThreadPoolService.Release();
	m_pServiceProvider.Release();
	m_pUpdateService.Release();
	m_pTabbedControl.Release();
	m_pSettings.Release();
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnTabHeaderClick(IControl* pControl)
{
	CComPtr<IControl> pCurrentControl;
	RETURN_IF_FAILED(m_pTabbedControl->GetCurrentPage(&pCurrentControl));
	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = pControl;
	if (pControl == pCurrentControl && pTimelineControlSupport)
	{
		CComPtr<ITimelineControl> pTimelineControl;
		RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&pTimelineControl));
		RETURN_IF_FAILED(pTimelineControl->ScrollToItem(0));
	}
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnLinkClick(HWND hWnd)
{
	RETURN_IF_FAILED(m_pUpdateService->RunUpdate());
	return S_OK;
}

STDMETHODIMP CViewControllerService::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	return S_OK;
}

STDMETHODIMP CViewControllerService::StartAnimation()
{
	RETURN_IF_FAILED(m_pTabbedControl->StartAnimation());
	return S_OK;
}

STDMETHODIMP CViewControllerService::StopAnimation()
{
	UINT uiRefs = 0;
	RETURN_IF_FAILED(m_pTabbedControl->StopAnimation(&uiRefs));
	return S_OK;
}

STDMETHODIMP CViewControllerService::ShowInfo(HRESULT hr, BOOL bError, BOOL bInfoImageEnableClick, BSTR bstrMessage)
{
	RETURN_IF_FAILED(m_pUpdateService->IsUpdateAvailable(&m_bUpdateAvailable));

	if (m_bUpdateAvailable)
	{
		RETURN_IF_FAILED(m_pTabbedControl->ShowInfo(FALSE, TRUE, L"Update available. Click here to install."));
		return S_OK;
	}
	else if (FAILED(hr))
	{
		if (hr == COMADMIN_E_USERPASSWDNOTVALID)
		{
			CComPtr<IFormManager> pFormManager;
			RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
			RETURN_IF_FAILED(pFormManager->ActivateForm(CLSID_SettingsControl));
		}
		RETURN_IF_FAILED(m_pTabbedControl->ShowInfo(bError, bInfoImageEnableClick, bstrMessage));
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CViewControllerService::HideInfo()
{
	RETURN_IF_FAILED(m_pTabbedControl->HideInfo());
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnDeactivate(IControl *pControl)
{
	CComQIPtr<ITwitViewControl> pTwitViewControl = pControl;
	if (pTwitViewControl)
	{
		RETURN_IF_FAILED(m_pFormsService->Close(pControl));
		return S_OK;
	}

	CComQIPtr<IUserInfoControl> pUserInfoControl = pControl;
	if (pUserInfoControl)
	{
		RETURN_IF_FAILED(m_pFormsService->Close(pControl));
		return S_OK;
	}

	CComQIPtr<IListTimelineControl> pListTimelineControl = pControl;
	if (pListTimelineControl)
	{
		RETURN_IF_FAILED(m_pFormsService->Close(pControl));
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CViewControllerService::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pControl->GetHWND(&hWnd));
	if (pMsg->message == WM_KEYDOWN && GetActiveWindow() == hWnd)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			CComPtr<IControl> pControlHome;
			CComPtr<IFormManager> pFormManager;
			RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
			RETURN_IF_FAILED(pFormManager->ActivateForm(CLSID_HomeTimeLineControl));
		}
	}
	return 0;
}