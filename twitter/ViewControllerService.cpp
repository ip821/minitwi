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
	RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(ICustomTabControlEventSink), &m_dwAdviceCustomtabControl));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_UpdateService, &m_pUpdateService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORMS_SERVICE, &m_pFormsService));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	RETURN_IF_FAILED(pThemeService->ApplyThemeFromSettings());

	RETURN_IF_FAILED(AtlAdvise(m_pUpdateService, pUnk, __uuidof(IUpdateServiceEventSink), &m_dwAdviceUpdateService));

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &m_pThreadPoolService));
	RETURN_IF_FAILED(m_pThreadPoolService->SetThreadCount(2));
	RETURN_IF_FAILED(m_pThreadPoolService->Start());

	return S_OK;
}

STDMETHODIMP CViewControllerService::OnInitCompleted()
{
	RETURN_IF_FAILED(m_pTabbedControl->GetPageCount(&m_dwControlsCount));
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnShutdown()
{
	RETURN_IF_FAILED(m_pMessageLoop->RemoveMessageFilter(this));
	RETURN_IF_FAILED(AtlUnadvise(m_pUpdateService, __uuidof(IUpdateServiceEventSink), m_dwAdviceUpdateService));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(ITabbedControlEventSink), m_dwAdviceTabbedControl2));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(IInfoControlEventSink), m_dwAdviceTabbedControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(ICustomTabControlEventSink), m_dwAdviceCustomtabControl));
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

STDMETHODIMP CViewControllerService::OnBackButtonClicked()
{
	ATLASSERT(m_controlsStack.size());
	if (m_controlsStack.size())
	{
		if (m_controlsStack.size() > 1)
		{
			auto itLast = m_controlsStack.end();
			--itLast;
			--itLast;
			RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(*itLast));
		}
		else
		{
			CComPtr<IControl> pControl;
			RETURN_IF_FAILED(m_pTabbedControl->GetPage(0, &pControl));
			RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(pControl));
		}
	}
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

	RETURN_IF_FAILED(m_pUpdateService->IsUpdateAvailable(&m_bUpdateAvailable));

	if (m_bUpdateAvailable && !uiRefs)
	{
		RETURN_IF_FAILED(m_pTabbedControl->ShowInfo(FALSE, TRUE, L"Update available. Click here to install."));
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CViewControllerService::OnUpdateAvailable()
{
	PostMessage(m_hControlWnd, WM_UPDATE_AVAILABLE, 0, 0);
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

STDMETHODIMP CViewControllerService::OnActivate(IControl* pControl)
{
	DWORD dwCount = 0;
	RETURN_IF_FAILED(m_pTabbedControl->GetPageCount(&dwCount));
	if (dwCount > m_dwControlsCount)
	{
		RETURN_IF_FAILED(m_pTabbedControl->ShowBackButton(TRUE));

		DWORD dwIndex = 0;
		RETURN_IF_FAILED(m_pTabbedControl->GetPageIndex(pControl, &dwIndex));

		if (dwIndex == dwCount - 1) //moving forward
		{
			m_controlsStack.push_back(pControl);
			return S_OK;
		}
		else //moving backwards
		{
			if (dwIndex > m_dwControlsCount - 1)
			{
				//destroy previous control
				auto itLast = --m_controlsStack.end();
				RETURN_IF_FAILED(m_pFormsService->Close(*itLast));
				m_controlsStack.erase(itLast);
				return S_OK;
			}
			else
			{
				//moving to home, destroy all
				for (auto& it : m_controlsStack)
				{
					RETURN_IF_FAILED(m_pFormsService->Close(it));
				}
				m_controlsStack.clear();
				RETURN_IF_FAILED(m_pTabbedControl->ShowBackButton(FALSE));
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CViewControllerService::OnDeactivate(IControl *pControl)
{
	DWORD dwIndex = 0;
	RETURN_IF_FAILED(m_pTabbedControl->GetPageIndex(pControl, &dwIndex));

	CComQIPtr<ISearchControl> pSearchControl = pControl;
	if (pSearchControl && dwIndex < m_dwControlsCount)
	{
		RETURN_IF_FAILED(pSearchControl->Clear());
	}
	return S_OK;
}

STDMETHODIMP CViewControllerService::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pControl->GetHWND(&hWnd));

	if (pMsg->message == WM_UPDATE_AVAILABLE && pMsg->hwnd == m_hControlWnd)
	{
		RETURN_IF_FAILED(ShowInfo(S_OK, FALSE, FALSE, L""));
	}

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