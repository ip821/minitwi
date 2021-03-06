// ViewControllerService.cpp : Implementation of CViewControllerService

#include "stdafx.h"
#include "ViewControllerService.h"
#include "Plugins.h"
#include "twmdl_i.h"
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

    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

    m_pMainWindow = m_pControl;
    ATLASSERT(m_pMainWindow);

    RETURN_IF_FAILED(AtlAdvise(m_pMainWindow, pUnk, __uuidof(IMainWindowEventSink), &m_dwAdviceMainWindow));

    RETURN_IF_FAILED(m_pMainWindow->GetMessageLoop(&m_pMessageLoop));
    RETURN_IF_FAILED(m_pMessageLoop->AddMessageFilter(this));

    CComPtr<IContainerControl> pContainerControl;
    RETURN_IF_FAILED(m_pMainWindow->GetContainerControl(&pContainerControl));
    m_pTabbedControl = pContainerControl;
    RETURN_IF_FAILED(m_pTabbedControl->EnableCommands(FALSE));

    RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(IInfoControlEventSink), &m_dwAdviceTabbedControl));
    RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(ITabbedControlEventSink), &m_dwAdviceTabbedControl2));
    RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(ICustomTabControlEventSink), &m_dwAdviceCustomTabControl));
    RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_UpdateService, &m_pUpdateService));

    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORMS_SERVICE, &m_pFormsService));
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &m_pFormManager));

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
    RETURN_IF_FAILED(AtlUnadvise(m_pMainWindow, __uuidof(IMainWindowEventSink), m_dwAdviceMainWindow));
    RETURN_IF_FAILED(AtlUnadvise(m_pUpdateService, __uuidof(IUpdateServiceEventSink), m_dwAdviceUpdateService));
    RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(ITabbedControlEventSink), m_dwAdviceTabbedControl2));
    RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(IInfoControlEventSink), m_dwAdviceTabbedControl));
    RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(ICustomTabControlEventSink), m_dwAdviceCustomTabControl));
    RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
    m_pMessageLoop.Release();
    m_pFormManager.Release();
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
        auto itLast = m_controlsStack.end();
        --itLast;
        RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(*itLast));
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
            RETURN_IF_FAILED(m_pFormManager->ActivateForm(CLSID_SettingsControl));
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
        DWORD dwIndex = 0;
        RETURN_IF_FAILED(m_pTabbedControl->GetPageIndex(pControl, &dwIndex));

        if (dwIndex != dwCount - 1) //moving backward
        {
            if (dwIndex)
            {
                auto itLast = m_controlsStack.end();
                --itLast;
                RETURN_IF_FAILED(m_pFormsService->Close(*itLast));
                m_controlsStack.erase(itLast);

                itLast = m_controlsStack.end();
                --itLast;
                m_controlsStack.erase(itLast);
            }
            else
            {
                //moving to home, destroy all
                for (auto& it : m_controlsStack)
                {
                    DWORD dwControlIndex = 0;
                    RETURN_IF_FAILED(m_pTabbedControl->GetPageIndex(it, &dwControlIndex));
                    if (dwControlIndex >= m_dwControlsCount)
                    {
                        RETURN_IF_FAILED(m_pFormsService->Close(it));
                    }
                }
                m_controlsStack.clear();
            }
        }
    }
    else
    {
        m_controlsStack.clear();
    }

    RETURN_IF_FAILED(m_pTabbedControl->ShowBackButton(m_controlsStack.size()));

    CComPtr<IControl> pControlSearch;
    RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_SearchControl, &pControlSearch));

    if (!m_controlsStack.size() && pControl != pControlSearch)
    {
        CComQIPtr<ISearchControl> pSearchControl = pControlSearch;
        RETURN_IF_FAILED(pSearchControl->Clear());
    }

    return S_OK;
}

STDMETHODIMP CViewControllerService::OnDeactivate(IControl *pControl)
{
    DWORD dwIndex = 0;
    RETURN_IF_FAILED(m_pTabbedControl->GetPageIndex(pControl, &dwIndex));

    m_controlsStack.push_back(pControl);

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
            RETURN_IF_FAILED(m_pFormManager->ActivateForm(CLSID_HomeTimeLineControl));
        }
    }
    return 0;
}

STDMETHODIMP CViewControllerService::OnDestroy()
{
    exit(0);
}