// WindowService.cpp : Implementation of CWindowService

#include "stdafx.h"
#include "WindowService.h"


// CWindowService

STDMETHODIMP CWindowService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	return S_OK;
}

STDMETHODIMP CWindowService::OnShutdown()
{
	m_pSettings.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CWindowService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CWindowService::OnClosed(HWND hWndSelf)
{
	auto it = m_windows.find(hWndSelf);
	ATLASSERT(it != m_windows.end());
	RETURN_IF_FAILED(AtlUnadvise(it->second.m_pWindow, __uuidof(IWindowEventSink), it->second.m_dwAdvice));
	CComQIPtr<IPluginSupportNotifications> pInit = it->second.m_pWindow;
	if (pInit)
	{
		RETURN_IF_FAILED(pInit->OnShutdown());
	}
	m_windows.erase(it);
	return S_OK;
}

STDMETHODIMP CWindowService::OpenWindow(HWND hWndParent, REFCLSID clsid, IVariantObject* pVariantObject)
{
	CComPtr<IWindow> pWindow;
	RETURN_IF_FAILED(HrCoCreateInstance(clsid, &pWindow));
	RETURN_IF_FAILED(HrInitializeWithSettings(pWindow, m_pSettings));

	{
		CComQIPtr<IThemeSupport> pThemeSupport = pWindow;
		if (pThemeSupport)
		{
			CComPtr<IThemeService> pThemeService;
			RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
			CComPtr<ITheme> pTheme;
			RETURN_IF_FAILED(pThemeService->GetCurrentTheme(&pTheme));
			RETURN_IF_FAILED(pThemeSupport->SetTheme(pTheme));
		}
	}

	RETURN_IF_FAILED(pWindow->Show(hWndParent));
	HWND hWnd = 0;
	RETURN_IF_FAILED(pWindow->GetHWND(&hWnd));
	WindowData windowData;
	windowData.m_pWindow = pWindow;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(pWindow, pUnk, __uuidof(IWindowEventSink), &windowData.m_dwAdvice));

	m_windows[hWnd] = windowData;

	RETURN_IF_FAILED(HrInitializeWithControl(pWindow, m_pControl));
	RETURN_IF_FAILED(HrNotifyOnInitialized(pWindow, m_pServiceProvider));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(pWindow, pVariantObject));
	return S_OK;
}